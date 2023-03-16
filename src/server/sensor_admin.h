#pragma once

#include "../etl/queue.h"
#include "../kernel.h"
#include "../rpi.h"
#include "request_header.h"
namespace Sensor
{

constexpr char SENSOR_ADMIN_NAME[] = "SENSOR_ADMIN";
constexpr int SENSOR_UART_CHANNEL = 1;
constexpr int SENSOR_ADMIN_NUM_SUBSCRIBERS = 16;
constexpr int NUM_SENSOR_BYTES = 10;
constexpr int SENSOR_DELAY = 7;

void sensor_admin();
void sensor_courier();
void courier_time_out();


union RequestBody {
	char sensor_state[NUM_SENSOR_BYTES];
	uint64_t time_out_id;
};

struct SensorAdminReq {
	Message::RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));


struct CourierRequestBody {
	uint64_t info = 0;
};

struct SensorCourierReq {
	Message::RequestHeader header;
	CourierRequestBody body;
} __attribute__((aligned(8)));
}
