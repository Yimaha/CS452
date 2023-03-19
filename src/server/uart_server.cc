#include "uart_server.h"
#include "../etl/queue.h"
#include "../rpi.h"
#include "../utils/printf.h"

using namespace UART;
using namespace Message;

void UART::uart_0_server_transmit() {
	const int uart_channel = 0;
	Name::RegisterAs(UART_0_TRANSMITTER);
	// create it's worker
	Task::Create(Priority::CRITICAL_PRIORITY, &uart_0_transmission_notifier);
	etl::queue<char, CHAR_QUEUE_SIZE> transmit_queue;
	int from;
	UARTServerReq req;
	bool transmit_interrupt_enable = false;

	auto enable_interrupt = [&]() {
		transmit_interrupt_enable = true;
		UART::TransInterrupt(uart_channel, true);
	};

	auto tryPutC = [&](char c) {
		int put_successful = UART::UartWriteRegister(uart_channel, UART_THR, c);
		if (put_successful != UART::SUCCESSFUL) {
			transmit_queue.push(c);
			// enable the interrupt
			enable_interrupt();
		}
	};

	auto tryPutS = [&](const char* s, int len) {
		int i = 0;
		int put_successful = UART::SUCCESSFUL;
		while (i < len && put_successful == UART::SUCCESSFUL) {
			put_successful = UART::UartWriteRegister(uart_channel, UART_THR, s[i]);
			i++;
		}
		if (i != len) { // we couldn't push everything
			for (i = i - 1; i < len; i++) {
				transmit_queue.push(s[i]);
			}
			// enable the interrupt
			enable_interrupt();
		}
	};

	auto tryClearTransmit = [&]() {
		int put_successful = UART::SUCCESSFUL;
		while (!transmit_queue.empty() && put_successful == UART::SUCCESSFUL) {
			put_successful = UART::UartWriteRegister(uart_channel, UART_THR, transmit_queue.front());
			if (put_successful == UART::SUCCESSFUL) {
				transmit_queue.pop();
			}
		}
		if (!transmit_queue.empty()) { // we couldn't push everything
			enable_interrupt();
			return false; // could not clear
		} else {
			return true; // cleared
		}
	};

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(UARTServerReq));
		switch (req.header) {
		case RequestHeader::UART_NOTIFY_TRANSMISSION: {
			Message::Reply::EmptyReply(from); // unblock receiver right away right away
			transmit_interrupt_enable = false;
			tryClearTransmit();
			break;
		}
		case RequestHeader::UART_PUTC: {
			Message::Reply::EmptyReply(from); // unblock putc guy right away
			// the default behaviour is putc, but if we are full, then we wait for interrupt
			char c = req.body.regular_msg;

			if (transmit_interrupt_enable) {
				transmit_queue.push(c);
			} else {
				if (tryClearTransmit()) {
					tryPutC(c);
				} else {
					transmit_queue.push(c);
					enable_interrupt();
				}
			}
			break;
		}
		case RequestHeader::UART_PUTS: {
			Message::Reply::EmptyReply(from); // unblock putc guy right away right away
			// the default behaviour is putc, but if we are full, then we wait for interrupt
			const char* s = req.body.worker_msg.msg;
			int len = req.body.worker_msg.msg_len;

			if (len > 0) {
				if (transmit_interrupt_enable) {
					for (int i = 0; i < len; i++) {
						transmit_queue.push(s[i]);
					}
				} else {
					if (tryClearTransmit()) {
						tryPutS(s, len);
					} else {
						for (int i = 0; i < len; i++) {
							transmit_queue.push(s[i]);
						}
						// enable the interrupt
						enable_interrupt();
					}
				}
			}
			break;
		}
		default: {
			Task::_KernelCrash("UART0 trans: illegal type: [%d]\r\n", req.header);
		}
		}
	}
}

void UART::uart_0_server_receive() {
	const int uart_channel = 0;
	Name::RegisterAs(UART_0_RECEIVER);

	// create it's worker
	Task::Create(Priority::CRITICAL_PRIORITY, &uart_0_receive_notifier);

	etl::queue<char, CHAR_QUEUE_SIZE> receive_queue;
	etl::queue<int, TASK_QUEUE_SIZE> await_c;
	char receive_buffer[UART_FIFO_MAX_SIZE];

	int from;
	UARTServerReq req;

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(UARTServerReq));
		switch (req.header) {
		case RequestHeader::UART_NOTIFY_RECEIVE: {
			Message::Reply::EmptyReply(from); // unblock receiver right away right away

			WorkerRequestBody body = req.body.worker_msg;
			int length = body.msg_len;
			int i = 0;
			for (; !await_c.empty() && i < length; i++) {
				Message::Reply::Reply(await_c.front(), &body.msg[i], 1);
				await_c.pop();
			}
			for (; i < length; i++) {
				receive_queue.push(body.msg[i]);
			}
			break;
		}
		case RequestHeader::UART_GETC: {
			// you first need to exhaust the existing queue
			if (!receive_queue.empty()) {
				Message::Reply::Reply(from, &receive_queue.front(), 1); // unblock receiver right away right away
				receive_queue.pop();
			} else {
				// we now have an empty queue, try to see if there is any uart event
				int length = UartReadAll(uart_channel, receive_buffer);
				if (length == 0) {
					// we now have an empty receive queue as well, meaning we need interrupt
					UART::ReceiveInterrupt(uart_channel, true);
					await_c.push(from); // block until we receive some uart in future
				} else {
					Message::Reply::Reply(from, receive_buffer, 1); // unblock receiver right away right away
					for (int i = 1; i < length; i++) {
						receive_queue.push(receive_buffer[i]);
					}
				}
			}
			break;
		}
		default: {
			Task::_KernelCrash("UART0 receive: illegal type: [%d]\r\n", req.header);
		}
		}
	}
}

/**
 * For uart0, you will receive interrupt in the form of timeout, since the terminal does not obey the 4 byte rule and is
 * incredibly fast thus, we listen to timeout event, and decide who to queue next
 */
void UART::uart_0_receive_notifier() {
	int uart_tid = Name::WhoIs(UART_0_RECEIVER);
	UARTServerReq req = { RequestHeader::UART_NOTIFY_RECEIVE, WorkerRequestBody { 0x0, 0x0 } };
	while (true) {
		req.body.worker_msg.msg_len = Interrupt::AwaitEventWithBuffer(UART_0_RX_TIMEOUT, req.body.worker_msg.msg);
		Message::Send::Send(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq), nullptr,
							0); // we don't worry about response
	}
}

/**
 * The job of the transmission notifier is to prepare THR interrupt, ewhich is only triggered if the amount of space
 * left reaches a certian point default value is 0
 */
void UART::uart_0_transmission_notifier() {
	int uart_tid = Name::WhoIs(UART_0_TRANSMITTER);
	UARTServerReq req = { RequestHeader::UART_NOTIFY_TRANSMISSION, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(UART_0_TXR_INTERRUPT);
		Message::Send::SendNoReply(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq));
	}
}

/**
 * Unlike uart0, uart1 has fifo and CTS on transmitter
 */
void UART::uart_1_server_transmit() {
	const int uart_channel = 1;
	Name::RegisterAs(UART_1_TRANSMITTER);
	// create it's worker
	Task::Create(Priority::CRITICAL_PRIORITY, &uart_1_transmission_notifier);
	Task::Create(Priority::CRITICAL_PRIORITY, &uart_1_CTS_notifier);

	etl::queue<char, CHAR_QUEUE_SIZE> transmit_queue;
	int from;
	UARTServerReq req;

	/**
	 * Idea, putc keep accept input, yet the input is either fired right away, or wait until all interrupt came back.
	 * it needs to wait for both CTS and the interrput come back being ready. if you don't
	 */

	int CTS_await = 1; // wait for it to go up
	bool TX_await = false;

	auto send_if_possible = [&](char c) {

#ifdef SIMULATED_UART_1
		if (!TX_await) {
#else
		if (CTS_await == 1 && !TX_await) {
#endif
			UART::UartWriteRegister(uart_channel, UART_THR, c);
			// enable the interrupt
			CTS_await = 0;
			TX_await = true;
			UART::TransInterrupt(uart_channel, true); // turn on the fifo interrupt.
			// MSR (another way to detect CTS) interrupt is turned on by default
			return true;
		} else {
			return false;
		}
	};

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(UARTServerReq));
		switch (req.header) {
		case RequestHeader::UART_NOTIFY_TRANSMISSION: {
			Message::Reply::EmptyReply(from); // unblock receiver right away right away
			TX_await = false;

			if (!transmit_queue.empty()) {
				if (send_if_possible(transmit_queue.front())) {
					transmit_queue.pop();
				}
			}
			break;
		}
		case RequestHeader::UART_NOTIFY_CTS: {
			Message::Reply::EmptyReply(from); // unblock receiver right away right away
			CTS_await += 1;

			if (!transmit_queue.empty()) {
				if (send_if_possible(transmit_queue.front())) {
					transmit_queue.pop();
				}
			}
			break;
		}
		case RequestHeader::UART_PUTC: {

			Message::Reply::EmptyReply(from); // unblock putc guy right away right away
			// the default behaviour is putc, but if we are full, then we wait for interrupt
			char c = req.body.regular_msg;
			if (transmit_queue.empty()) {
				if (!send_if_possible(c)) {
					transmit_queue.push(c);
				}
			} else {
				if (send_if_possible(transmit_queue.front())) {
					transmit_queue.pop();
				}
				transmit_queue.push(c);
			}

			break;
		}
		case RequestHeader::UART_PUTS: {
			Message::Reply::EmptyReply(from); // unblock putc guy right away right away
			// the default behaviour is putc, but if we are full, then we wait for interrupt
			const char* s = req.body.worker_msg.msg;
			int len = req.body.worker_msg.msg_len;
			if (transmit_queue.empty()) {
				if (!send_if_possible(s[0])) {
					for (int i = 0; i < len; i++) {
						transmit_queue.push(s[i]);
					}
				} else {
					for (int i = 1; i < len; i++) {
						transmit_queue.push(s[i]);
					}
				}
			} else {
				if (send_if_possible(transmit_queue.front())) {
					transmit_queue.pop();
				}
				for (int i = 0; i < len; i++) {
					transmit_queue.push(s[i]);
				}
			}

			break;
		}
		default: {
			Task::_KernelCrash("UART1 trans: illegal type: [%d]\r\n", req.header);
		}
		}
	}
}

void UART::uart_1_server_receive() {
	const int uart_channel = 1;
	Name::RegisterAs(UART_1_RECEIVER);

	// create it's worker
	Task::Create(Priority::CRITICAL_PRIORITY, &uart_1_receive_notifier);
	Task::Create(Priority::CRITICAL_PRIORITY, &uart_1_receive_timeout_notifier);

	etl::queue<char, CHAR_QUEUE_SIZE> receive_queue;
	etl::queue<int, TASK_QUEUE_SIZE> await_c;

	int from;
	UARTServerReq req;

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(UARTServerReq));
		switch (req.header) {
		case RequestHeader::UART_NOTIFY_RECEIVE: {
			Message::Reply::EmptyReply(from); // unblock receiver right away right away
			// body is irrlevant, we simply try to read until we
			// this call ideally should never happen, if it does, then we have issue with sensor not coming back fast
			// enough.
			int c = UartReadRegister(uart_channel, UART_RHR);
			while (c != UART::Exception::FAILED_TO_READ) { // until there is nothing to read
				char reply = (char)c;
				if (!await_c.empty()) {
					Message::Reply::Reply(await_c.front(), &reply, 1);
					await_c.pop();
				} else {
					receive_queue.push(reply);
				}
				c = UartReadRegister(uart_channel, UART_RHR);
			}
			break;
		}
		case RequestHeader::UART_GETC: {
			// you first need to exhaust the existing queue
			if (!receive_queue.empty()) {
				Message::Reply::Reply(from, &receive_queue.front(), 1); // unblock receiver right away right away
				receive_queue.pop();
			} else {
				// we now have an empty queue, try to see if there is any uart event
				int output = UartReadRegister(uart_channel, UART_RHR);
				if (output == UART::Exception::FAILED_TO_READ) {
					// we now have an empty receive queue as well, meaning we need interrupt
					UART::ReceiveInterrupt(uart_channel, true);
					await_c.push(from); // block until we receive some uart in future
				} else {
					char real_value = (char)output;
					Message::Reply::Reply(from, (const char*)&real_value, 1);
					output = UartReadRegister(uart_channel, UART_RHR);
					while (output != UART::Exception::FAILED_TO_READ) {
						receive_queue.push((char)output);
						output = UartReadRegister(uart_channel, UART_RHR);
					}
				}
			}
			break;
		}
		default: {
			Task::_KernelCrash("UART1 receive: illegal type: [%d]\r\n", req.header);
		}
		}
	}
}

void UART::uart_1_transmission_notifier() {
	int uart_tid = Name::WhoIs(UART_1_TRANSMITTER);
	UARTServerReq req = { RequestHeader::UART_NOTIFY_TRANSMISSION, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(UART_1_TXR_INTERRUPT);
		Message::Send::SendNoReply(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq));
	}
}

void UART::uart_1_CTS_notifier() {
	int uart_tid = Name::WhoIs(UART_1_TRANSMITTER);
	UARTServerReq req = { RequestHeader::UART_NOTIFY_CTS, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(UART_1_MSR_INTERRUPT);
		Message::Send::SendNoReply(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq));
	}
}

void UART::uart_1_receive_notifier() {
	int uart_tid = Name::WhoIs(UART_1_RECEIVER);
	UARTServerReq req = { RequestHeader::UART_NOTIFY_RECEIVE, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(UART_1_RX_INTERRUPT);
		Message::Send::SendNoReply(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq));
	}
}

void UART::uart_1_receive_timeout_notifier() {
	int uart_tid = Name::WhoIs(UART_1_RECEIVER);
	UARTServerReq req = { RequestHeader::UART_NOTIFY_RECEIVE, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(UART_1_RX_TIMEOUT);
		Message::Send::SendNoReply(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq));
	}
}