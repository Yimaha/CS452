#include "uart_server.h"
#include "../etl/queue.h"
#include "../utils/printf.h"

using namespace UART;

void UART::uart_0_server_transmit() {
	const int uart_channel = 0;
	Name::RegisterAs(UART_0_TRANSMITTER);
	// create it's worker
	Task::Create(1, &uart_transmission_notifier);
	etl::queue<char, CHAR_QUEUE_SIZE> transmit_queue;
	int from;
	UARTServerReq req;
	bool transmit_interrupt_enable = false;

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(UARTServerReq));
		switch (req.header) {
		case RequestHeader::NOTIFY_TRANSMISSION: {
			Message::Reply::Reply(from, nullptr, 0); // unblock receiver right away right away
			transmit_interrupt_enable = false;
			while (!transmit_queue.empty()) {
				bool put_successful = UART::UartWriteRegister(uart_channel, UART_THR, transmit_queue.front());
				if (!put_successful) {
					break; // we filled the buffer again, very unlikely to happen though.
				} else {
					transmit_queue.pop();
				}
			}
			UART::TransInterrupt(uart_channel, false);
			break;
		}
		case RequestHeader::PUTC: {
			Message::Reply::Reply(from, nullptr, 0); // unblock putc guy right away right away
			// the default behaviour is putc, but if we are full, then we wait for interrupt
			char c = req.body.regular_msg;

			if (transmit_interrupt_enable) {
				transmit_queue.push(c);
			} else {
				int put_successful = UART::UartWriteRegister(uart_channel, UART_THR, c);
				if (put_successful != UART::SUCCESSFUL) {
					transmit_queue.push(c);
					// enable the interrupt
					transmit_interrupt_enable = true;
					UART::TransInterrupt(uart_channel, true);
				}
			}
			break;
		}
		default: {
			char exception[30];
			sprintf(exception, "illegal type: [%d]\r\n", req.header);
			Task::_KernelPrint(exception);
			while (1) {
			}
		}
		}
	}
}

void UART::uart_0_server_receive() {
	const int uart_channel = 0;
	Name::RegisterAs(UART_0_RECEIVER);

	// create it's worker
	Task::Create(1, &uart_receive_notifier);

	etl::queue<char, CHAR_QUEUE_SIZE> receive_queue;
	etl::queue<int, TASK_QUEUE_SIZE> await_c;
	char receive_buffer[UART_FIFO_MAX_SIZE];


	int from;
	UARTServerReq req;

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(UARTServerReq));
		switch (req.header) {
		case RequestHeader::NOTIFY_RECEIVE: {
			Message::Reply::Reply(from, nullptr, 0); // unblock receiver right away right away
			UART::ReceiveInterrupt(uart_channel, false);

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
		case RequestHeader::GETC: {
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
			char exception[30];
			sprintf(exception, "illegal type: [%d]\r\n", req.header);
			Task::_KernelPrint(exception);
			while (1) {
			}
		}
		}
	}
}

/**
 * For uart0, you will receive interrupt in the form of timeout, since the terminal does not obey the 4 byte rule and is incredibly fast
 * thus, we listen to timeout event, and decide who to queue next
 */
void UART::uart_receive_notifier() {
	int uart_tid = Name::WhoIs(UART_0_RECEIVER);
	UARTServerReq req = { RequestHeader::NOTIFY_RECEIVE, WorkerRequestBody({ 0x0, 0x0 }) };
	while (true) {
		req.body.worker_msg.msg_len = Interrupt::AwaitEventWithBuffer(UART_RX_TIMEOUT, req.body.worker_msg.msg);
		Message::Send::Send(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq), nullptr, 0); // we don't worry about response
	}
}

/**
 * The job of the transmission notifier is to prepare THR interrupt, ewhich is only triggered if the amount of space left reaches a certian point
 * default value is 0
 */
void UART::uart_transmission_notifier() {
	int uart_tid = Name::WhoIs(UART_0_TRANSMITTER);
	UARTServerReq req = { RequestHeader::NOTIFY_TRANSMISSION, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(UART_TXR_INTERRUPT);
		Message::Send::Send(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq), nullptr, 0);
	}
}
