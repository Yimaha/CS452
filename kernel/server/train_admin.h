#pragma once

#include "../kernel.h"
#include "../rpi.h"
#include "../etl/queue.h"
namespace Train
{

constexpr char TRAIN_SERVER_NAME[] = "TRAIN_ADMIN";
constexpr int TRAIN_UART_CHANNEL = 1;
constexpr char REV_COMMAND = 15;
void train_admin();
void train_courier();

enum class RequestHeader : uint32_t { SPEED, REV, SWITCH, COURIER_COMPLETE, SWITCH_DELAY_COMPLETE }; // note that notify is an exclusive, clock notifier message.

struct RequestBody {
	char id;
	char action;
};

struct TrainAdminReq {
	RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

enum class CourierRequestHeader : uint32_t { REV, SWITCH_DELAY }; // note that notify is an exclusive, clock notifier message.

struct TrainCourierReq {
	CourierRequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));
}

