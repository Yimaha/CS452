#pragma once

#include "../etl/queue.h"
#include "../kernel.h"
#include "../rpi.h"
namespace Sensor
{

constexpr char SENSOR_ADMIN_NAME[] = "SENSOR_ADMIN";
constexpr int SENSOR_UART_CHANNEL = 1;
constexpr int SENSOR_ADMIN_NUM_SUBSCRIBERS = 32;
constexpr int NUM_SENSOR_BYTES = 10;
constexpr int SENSOR_DELAY = 10; // if you want, set this to 0

void sensor_admin();
void sensor_courier();
void courier_time_out();

enum class RequestHeader : uint32_t { SENSOR_UPDATE, GET_SENSOR_STATE, SENSOR_TIME_OUT }; // note that notify is an exclusive, clock notifier message.

union RequestBody {
	char sensor_state[NUM_SENSOR_BYTES];
	uint64_t time_out_id;
};

struct SensorAdminReq {
	RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

enum class CourierRequestHeader : uint32_t { OBSERVER, TIMEOUT }; // note that notify is an exclusive, clock notifier message.

struct CourierRequestBody {
	uint64_t info = 0;
};

struct SensorCourierReq {
	CourierRequestHeader header;
	CourierRequestBody body;
} __attribute__((aligned(8)));
}
