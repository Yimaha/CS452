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
void uart_server();

void uart_receive_notifier();

void uart_transmission_notifier();


enum class RequestHeader : uint32_t { NOTIFY_RECEIVE, NOTIFY_TRANSMISSION, GETC, PUTC };


struct WorkerRequestBody {
	uint64_t msg_len = 0;
	char msg[64];
};



union RequestBody {
	char regular_msg;
	WorkerRequestBody worker_msg;
};

struct UARTServerReq {
	RequestHeader header = RequestHeader::NOTIFY_RECEIVE;
	RequestBody body = {0};

	UARTServerReq() {}

	UARTServerReq(RequestHeader header, char b) {
		header = header;
		body.regular_msg = b;
	}


	UARTServerReq(RequestHeader header, WorkerRequestBody worker_msg) {
		header = header;
		body.worker_msg = worker_msg;
	}

} __attribute__((aligned(8)));
}