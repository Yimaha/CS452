#pragma once

#include "../interrupt/uart.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
namespace UART
{

// the goal of UART server is providing a non-blocking uart job
// meaning, by nature, it must have some form of queue, that queues up input message
// similarly, while reading, it is also nice to have a buffer that keeps reading whenever intrrupted

constexpr char UART_SERVER_NAME[] = "UART_SERVER";
constexpr int UART_0_SERVER_TID = 5;
constexpr int CHAR_QUEUE_SIZE = 128;
constexpr int TASK_QUEUE_SIZE = 64;
constexpr int UART_FIFO_MAX_SIZE = 64;

void uart_server();
void uart_receive_notifier();
void uart_transmission_notifier();

enum class RequestHeader : uint32_t { NONE, NOTIFY_RECEIVE, NOTIFY_TRANSMISSION, GETC, PUTC };

struct WorkerRequestBody {
	uint64_t msg_len = 0;
	char msg[32];
};

union RequestBody
{
	char regular_msg;
	WorkerRequestBody worker_msg;
};

struct UARTServerReq {
	RequestHeader header = RequestHeader::NONE;
	RequestBody body = {'0'};

	UARTServerReq() {}

	UARTServerReq(RequestHeader h, char b) {
		header = h;
		body.regular_msg = b;
	}

	UARTServerReq(RequestHeader h, WorkerRequestBody worker_msg) {
		header = h;
		body.worker_msg = worker_msg;
	}

} __attribute__((aligned(8)));
}