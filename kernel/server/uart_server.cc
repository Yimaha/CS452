#include "uart_server.h"
#include "../etl/queue.h"
#include "../utils/printf.h"

using namespace UART;

// right now we are running the worst model, where we simply have all interrupt enable,
// and the agent (both receive and transmit) are ready as much as possible, keep fetching user input through interrupt
// ideally, both interrupt should be off by default, and server determine if certain register needed to be flipped
void UART::uart_server() {
	Name::RegisterAs(UART_SERVER_NAME);
	etl::queue<char, 128> receive_queue;
	etl::queue<char, 128> transmit_queue;
	etl::queue<int, 32> await_c;
	int from;
	UARTServerReq req = { RequestHeader::NOTIFY_RECEIVE, (char)0 };
	bool full_transmit_buffer = false;

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(UARTServerReq));
		switch (req.header) {
		case RequestHeader::NOTIFY_RECEIVE: {
			Message::Reply::Reply(from, nullptr, 0); // unblock receiver right away right away
			// then we worry about the message
			WorkerRequestBody body = req.body.worker_msg;
			int length = body.msg_len;
			for (int i = 0; i < length; i++) {
				receive_queue.push(body.msg[i]);
			}
			break;
		}
		case RequestHeader::GETC: {
			// getC is a potentially blocking call
			if (receive_queue.empty()) {
				await_c.push(from); // block until we receive some uart in future
			} else {
				Message::Reply::Reply(from, &receive_queue.front(), 1); // unblock receiver right away right away
				receive_queue.pop();
			}
			break;
		}
		case RequestHeader::PUTC: {
			Message::Reply::Reply(from, nullptr, 0); // unblock putc guy right away right away
			// the default behaviour is putc, but if we are full, then we wait for interrupt
			char c = req.body.regular_msg;
			if (full_transmit_buffer) {
				transmit_queue.push(c);
			} else {
				bool put_successful = uart_put(0, 0, UART_THR, c);
				if (!put_successful) {
					transmit_queue.push(c);
					// enable the interrupt
					full_transmit_buffer = true;
					uart_put(0, 0, UART_IER, 0b01);

				}
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
	UARTServerReq req = { RequestHeader::NOTIFY_RECEIVE, WorkerRequestBody({ 0, 0 }) };
	while (true) {
		req.body.worker_msg.msg_len = Interrupt::AwaitEventWithBuffer(UART_RX_TIMEOUT, req.body.worker_msg.msg);
		Message::Send::Send(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq), nullptr, 0); // we don't worry about response
	}
}

void UART::uart_transmission_notifier() {
	int uart_tid = Name::WhoIs(UART_SERVER_NAME);
	UARTServerReq req = { RequestHeader::NOTIFY_TRANSMISSION, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(UART_RX_TIMEOUT);
		Message::Send::Send(uart_tid, reinterpret_cast<const char*>(&req), sizeof(UARTServerReq), nullptr, 0);
	}
}
