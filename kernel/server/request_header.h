#pragma once
#include <stdint.h>

namespace Message
{
/**
 * It better to group all possible request header into a single enum which assign real value to them,
 * easier to debug, easier to detect issue, harder to make mistake (request can be distinguished eeasily between server even after compile)
 */
enum class RequestHeader : uint32_t {
	// error state, capture null header and crash kernel if detected.
	NONE,
	// time related
	TIME,
	DELAY,
	DELAY_UNTIL,
	NOTIFY_TIMER,
	// name related
	REGISTER_AS,
	WHO_IS,
	// sensor related
	SENSOR_UPDATE,
	GET_SENSOR_STATE,
	SENSOR_TIMEOUT,
	AWAIT_SENSOR_READING,
	SENSOR_TIMEOUT_START,
	// terminal related
	TERM_CLOCK,
	TERM_SENSORS,
	TERM_SWITCH,
	TERM_TRAIN_STATUS,
	TERM_IDLE,
	TERM_START,
	TERM_PUTC,
	TERM_REVERSE_COMPLETE,
	// term cour related
	TERM_COUR_REV,
	// train related
	TRAIN_SPEED,
	TRAIN_REV,
	TRAIN_SWITCH,
	TRAIN_COURIER_COMPLETE,
	TRAIN_SWITCH_DELAY_COMPLETE,
	TRAIN_DELAY_REV_COMPLETE,
	TRAIN_SWITCH_OBSERVE,
	TRAIN_OBSERVE,
	// train cour related
	TRAIN_COUR_REV_DELAY,
	TRAIN_COUR_SWITCH_DELAY,
	// uart related
	UART_NOTIFY_RECEIVE,
	UART_NOTIFY_TRANSMISSION,
	UART_NOTIFY_CTS,
	UART_GETC,
	UART_PUTC,
	UART_PUTS
};
}
