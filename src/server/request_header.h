#pragma once
#include <stdint.h>

namespace Message
{
/**
 * It better to group all possible request header into a single enum which assign real value to them,
 * easier to debug, easier to detect issue, harder to make mistake (request can be distinguished eeasily between server
 * even after compile)
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
	SENSOR_AWAIT_STATE,
	SENSOR_TIMEOUT,
	SENSOR_AWAIT_UPDATE,

	// sensor cour related
	SENSOR_COUR_AWAIT_READING,
	SENSOR_COUR_TIMEOUT_START,
	SENSOR_RESYNC,

	// terminal related
	TERM_CLOCK,
	TERM_SENSORS,
	TERM_SWITCH,
	TERM_TRAIN_STATUS,
	TERM_IDLE,
	TERM_START,
	TERM_PUTC,
	TERM_REVERSE_COMPLETE,
	TERM_DEBUG_PUTS,
	TERM_LOCAL_COMPLETE,

	// term cour related
	TERM_COUR_REV,
	TERM_COUR_LOCAL_GO,
	TERM_COUR_LOCAL_LOCATE,
	TERM_COUR_LOCAL_LOOP,
	TERM_COUR_LOCAL_EXLOOP,
	TERM_COUR_LOCAL_INIT,
	TERM_COUR_LOCAL_CALI,
	TERM_COUR_LOCAL_CALI_BASE_SPEED,
	TERM_COUR_LOCAL_CALI_ACCELERATION,
	TERM_COUR_LOCAL_CALI_STOPPING_DIST,

	// train related
	TRAIN_SPEED,
	TRAIN_REV,
	TRAIN_SWITCH,
	TRAIN_COURIER_COMPLETE,
	TRAIN_SWITCH_DELAY_COMPLETE,
	TRAIN_SWITCH_OBSERVE,
	TRAIN_OBSERVE,

	// train cour related
	TRAIN_COUR_SWITCH_DELAY,

	// uart related
	UART_NOTIFY_RECEIVE,
	UART_NOTIFY_TRANSMISSION,
	UART_NOTIFY_CTS,
	UART_GETC,
	UART_PUTC,
	UART_PUTS,

	// Global Pathing Related,
	GLOBAL_SET_TRACK,		   // determine which trakc are you on
	GLOBAL_LOCATE,			   // locate all trains
	GLOBAL_PATH,			   // ask a certain train to complete a certain path
	GLOBAL_LOOP,			   // ask a certain train to continuously loop in a certain path
	GLOBAL_EXIT_LOOP,		   // ask a certain train to exit loop
	GLOBAL_CALIBRATE_VELOCITY, // provide a train id, allow it to calibrate the velocity (running it at max velocity and
							   // update accordingly)
	GLOBAL_CALIBRATE_ACCELERATION,
	GLOBAL_CALIBRATE_STARTING,
	GLOBAL_CALIBRATE_STOPPING_DISTANCE, // you have to do the calibration manually, but it just run the train and stop
	GLOBAL_CALIBRATE_BASE_VELOCITY,
	// at the specific sensor
	GLOBAL_CLEAR_TO_SEND, // clear the dirty bit for train server to talk to
	GLOBAL_STOPPING_COMPLETE,
	GLOBAL_STOPPING_DISTANCE_START_PHASE_2,
	GLOBAL_COURIER_COMPLETE,
	GLOBAL_COUR_AWAIT_SENSOR,
	GLOBAL_COUR_SPEED,
	GLOBAL_COUR_SWITCH,
	GLOBAL_COUR_STOPPING,
	GLOBAL_COUR_STOPPING_DISTANCE_PHASE_2_DELAY,

	GLOBAL_COUR_CALIBRATE_VELOCITY,

	// Local Pathing Related
	LOCAL_PATH_SET_TRAIN,
	LOCAL_PATH_SET_PATH,
	LOCAL_PATH_LOCATE,
	LOCAL_PATH_LOOP,
	LOCAL_PATH_EXLOOP,
	LOCAL_PATH_INIT,
	LOCAL_PATH_CALI,
	LOCAL_PATH_CALI_BASE_SPEED,
	LOCAL_PATH_CALI_STOPPING_DISTANCE,
	LOCAL_PATH_CALI_ACCELERATION,
};

struct AddressBook {
	int clock_tid;
	int term_trans_tid;
	int term_receive_tid;
	int train_trans_tid;
	int train_receive_tid;
	int train_admin_tid;
	int sensor_admin_tid;
	int terminal_admin_tid;
	int global_pathing_tid;

	int local_pathing_tids[10]; // I want this to be Trains::NUM_TRAINS, but circular dependencies :(
};

AddressBook getAddressBook();
}
