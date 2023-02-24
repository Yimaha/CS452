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
	WorkerRequestBody worker_msg;
	char regular_msg;
};

struct UARTServerReq {
	RequestHeader header;
	RequestBody body;
} __attribute__((aligned(8)));
}