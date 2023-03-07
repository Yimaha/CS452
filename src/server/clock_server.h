#pragma once

#include "../etl/list.h"
#include "../interrupt/clock.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
#include "request_header.h"
namespace Clock
{

constexpr static uint64_t DELAY_QUEUE_SIZE = 64;
constexpr char CLOCK_SERVER_NAME[] = "CLOCK_SERVER";
void clock_server();
void clock_notifier();

enum Exception { INVALID_ID = -1, NEGATIVE_DELAY = -2, SEND_FAILED };

struct RequestBody {
	uint32_t ticks;
};

struct ClockServerReq {
	Message::RequestHeader header;
	RequestBody body;
} __attribute__((aligned(8)));
}