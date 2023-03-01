#pragma once

#include "../kernel.h"
#include "../rpi.h"
#include "../etl/queue.h"
namespace Sensor
{

constexpr char SENSOR_ADMIN_NAME[] = "SENSOR_ADMIN";
constexpr int SENSOR_UART_CHANNEL = 1;

void sensor_admin();
void sensor_courier();

enum class RequestHeader : uint32_t { SENSOR_UPDATE }; // note that notify is an exclusive, clock notifier message.

struct RequestBody {
	char id;
	char action;
};

struct SensorAdminReq {
	RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

// enum class CourierRequestHeader : uint32_t { REV }; // note that notify is an exclusive, clock notifier message.

// struct TrainCourierReq {
// 	CourierRequestHeader header;
// 	RequestBody body; // depending on the header, it treats the body differently
// } __attribute__((aligned(8)));
}

