#pragma once

#include "../etl/list.h"
#include "../interrupt/clock.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
namespace Clock
{

constexpr static uint64_t DELAY_QUEUE_SIZE = 256;
constexpr static char CLOCK_SERVER_NAME[] = "CLOCK_SERVER";
extern "C" void clock_server();
extern "C" void clock_notifier();

enum class RequestHeader : uint32_t { TIME, DELAY, DELAY_UNTIL, NOTIFY }; // note that notify is an exclusive, clock notifier message.
enum Exception { INVALID_ID = -1, NEGATIVE_DELAY = -2, SEND_FAILED };

struct RequestBody {
	uint32_t ticks;
};

struct ClockServerReq {
	RequestHeader header;
	RequestBody body;
} __attribute__((aligned(8)));
}