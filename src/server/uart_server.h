#pragma once

#include "../interrupt/uart.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
#include "request_header.h"
namespace UART
{

// the goal of UART server is providing a non-blocking uart job
// meaning, by nature, it must have some form of queue, that queues up input message
// similarly, while reading, it is also nice to have a buffer that keeps reading whenever intrrupted

constexpr char UART_0_TRANSMITTER[] = "UART_0_TRANS";
constexpr char UART_0_RECEIVER[] = "UART_0_RECEIVE";
constexpr char UART_1_TRANSMITTER[] = "UART_1_TRANS";
constexpr char UART_1_RECEIVER[] = "UART_1_RECEIVE";
constexpr int UART_0_TRANSMITTER_TID = 5;
constexpr int UART_0_RECEIVER_TID = 6;
constexpr int UART_1_TRANSMITTER_TID = 7;
constexpr int UART_1_RECEIVER_TID = 8;
constexpr int CHAR_QUEUE_SIZE = 16384 * 2;
constexpr int TASK_QUEUE_SIZE = 64;
constexpr int UART_FIFO_MAX_SIZE = 64;
constexpr int UART_MESSAGE_LIMIT = 512;

// broken down version of uart_server
void uart_0_server_transmit();
void uart_0_server_receive();
void uart_0_receive_notifier();
void uart_0_transmission_notifier();
void uart_1_server_transmit();
void uart_1_server_receive();
void uart_1_transmission_notifier();
void uart_1_CTS_notifier();
void uart_1_receive_notifier();
void uart_1_receive_timeout_notifier();

struct WorkerRequestBody {
	uint64_t msg_len = 0;
	char msg[UART_MESSAGE_LIMIT];
};

union RequestBody
{
	char regular_msg;
	WorkerRequestBody worker_msg;
};

struct UARTServerReq {
	Message::RequestHeader header = Message::RequestHeader::NONE;
	RequestBody body = { '0' };

	UARTServerReq() { }

	UARTServerReq(Message::RequestHeader h, char b) {
		header = h;
		body.regular_msg = b;
	}

	UARTServerReq(Message::RequestHeader h, WorkerRequestBody worker_msg) {
		header = h;
		body.worker_msg = worker_msg;
	}

} __attribute__((aligned(8)));
}