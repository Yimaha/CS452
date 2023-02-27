#include "uart_server.h"
#include "../etl/queue.h"
#include "../utils/printf.h"

using namespace UART;

// right now we are running the worst model, where we simply have all interrupt enable,
// and the agent (both receive and transmit) are ready as much as possible, keep fetching user input through interrupt
// ideally, both interrupt should be off by default, and server determine if certain register needed to be flipped



void UART::uart_server() {
	Name::RegisterAs(UART_SERVER_NAME);

	// create it's worker
	Task::Create(1, &uart_receive_notifier);
	Task::Create(1, &uart_transmission_notifier);

	etl::queue<char, CHAR_QUEUE_SIZE> receive_queue;
	etl::queue<char, CHAR_QUEUE_SIZE> transmit_queue;
	etl::queue<int, TASK_QUEUE_SIZE> await_c;
	char receive_buffer[UART_FIFO_MAX_SIZE];

	int from;
	UARTServerReq req;

	bool transmit_interrupt_enable = false;
	bool receive_interrupt_enable = false;

	auto uart_interrupt = [&](int channel) {
		UART::UartWriteRegister(channel, UART_IER, get_control_bits(transmit_interrupt_enable, receive_interrupt_enable));
	};

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(UARTServerReq));
		switch (req.header) {
		case RequestHeader::NOTIFY_RECEIVE: {
			Message::Reply::Reply(from, nullptr, 0); // unblock receiver right away right away
			receive_interrupt_enable = false; // we are now receiving message, no need for this interrupt anymore
			uart_interrupt(0);

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
		case RequestHeader::NOTIFY_TRANSMISSION: {
			Message::Reply::Reply(from, nullptr, 0); // unblock receiver right away right away
			transmit_interrupt_enable = false;
			while (!transmit_queue.empty()) {
				bool put_successful = UART::UartWriteRegister(0, UART_THR, transmit_queue.front());
				if (!put_successful) {
					break; // we filled the buffer again, very unlikely to happen though.
				} else {
					transmit_queue.pop();
				}
			}
			uart_interrupt(0);
			break;
		}
		case RequestHeader::GETC: {
			// you first need to exhaust the existing queue
			if (!receive_queue.empty()) {
				Message::Reply::Reply(from, &receive_queue.front(), 1); // unblock receiver right away right away
				receive_queue.pop();
			} else {
				// we now have an empty queue, try to see if there is any uart event
				int length = UartReadAll(0, receive_buffer);
				if (length == 0) {
					// we now have an empty receive queue as well, meaning we need interrupt
					receive_interrupt_enable = true;
					uart_interrupt(0);
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
		case RequestHeader::PUTC: {
			Message::Reply::Reply(from, nullptr, 0); // unblock putc guy right away right away
			// the default behaviour is putc, but if we are full, then we wait for interrupt
			char c = req.body.regular_msg;

			if (transmit_interrupt_enable) {
				transmit_queue.push(c);
			} else {
				int put_successful = UART::UartWriteRegister(0, UART_THR, c);
				if (put_successful != UART::SUCCESSFUL) {
					transmit_queue.push(c);
					// enable the interrupt
					transmit_interrupt_enable = true;
					uart_interrupt(0);
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
	int uart_tid = Name::WhoIs(UART_SERVER_NAME);
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
	int uart_tid = Name::WhoIs(UART_SERVER_NAME);
	UARTServerReq req = { RequestHeader::NOTIFY_TRANSMISSION, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(UART_TXR_INTERRUPT);
		Message::Send::Send(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq), nullptr, 0);
	}
}
