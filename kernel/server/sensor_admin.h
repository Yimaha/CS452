#pragma once

#include "../kernel.h"
#include "../rpi.h"
#include "../etl/queue.h"
namespace Sensor
{

constexpr char SENSOR_ADMIN_NAME[] = "SENSOR_ADMIN";
constexpr int SENSOR_UART_CHANNEL = 1;
const int NUM_SENSOR_BYTES = 10;

void sensor_admin();
void sensor_courier();

enum class RequestHeader : uint32_t { SENSOR_UPDATE, GET_SENSOR_STATE }; // note that notify is an exclusive, clock notifier message.

struct RequestBody {
	char sensor_state[10];
};

struct SensorAdminReq {
	RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

enum class CourierRequestHeader : uint32_t { OBSERVER }; // note that notify is an exclusive, clock notifier message.

struct CourierRequestBody {
	int delay = 0;
};


struct SensorCourierReq {
	CourierRequestHeader header;
	CourierRequestBody body;
} __attribute__((aligned(8)));
}

