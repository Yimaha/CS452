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
	SENSOR_START_UPDATE,

	// sensor cour related
	SENSOR_COUR_AWAIT_READING,
	SENSOR_COUR_TIMEOUT_START,

	// terminal related
	TERM_CLOCK,
	TERM_SENSORS,
	TERM_SWITCH,
	TERM_RESERVATION,
	TERM_TRAIN_STATUS,
	TERM_TRAIN_STATUS_MORE,
	TERM_IDLE,
	TERM_START,
	TERM_DEBUG_START,
	TERM_PUTC,
	TERM_REVERSE_COMPLETE,
	TERM_DEBUG_PUTS,
	TERM_LOCAL_COMPLETE,

	// term cour related
	TERM_COUR_REV,
	TERM_COUR_LOCAL_GO,
	TERM_COUR_LOCAL_LOCATE,
	TERM_COUR_LOCAL_INIT,
	TERM_COUR_LOCAL_CALI,
	TERM_COUR_LOCAL_CALI_BASE_SPEED,
	TERM_COUR_LOCAL_CALI_ACCELERATION,
	TERM_COUR_LOCAL_CALI_STOPPING_DIST,
	TERM_COUR_LOCAL_DEST,
	TERM_COUR_LOCAL_RNG,
	TERM_COUR_LOCAL_BUN_DIST,
	TERM_COUR_LOCAL_KNIGHT,
	TERM_COUR_KNIGHT_REV,

	// train related
	TRAIN_SPEED,
	TRAIN_REV,
	TRAIN_SWITCH,
	TRAIN_COURIER_COMPLETE,
	TRAIN_SWITCH_DELAY_COMPLETE,
	TRAIN_SENSOR_READING_COMPLETE,
	TRAIN_OBSERVE,

	// train cour related
	TRAIN_COUR_SWITCH_DELAY,
	TRAIN_COUR_SENSOR_START,

	// uart related
	UART_NOTIFY_RECEIVE,
	UART_NOTIFY_TRANSMISSION,
	UART_NOTIFY_CTS,
	UART_GETC,
	UART_PUTC,
	UART_PUTS,

	// Global Pathing Related,
	GLOBAL_SET_TRACK,			  // determine which trakc are you on
	GLOBAL_LOCATE,				  // locate all trains
	GLOBAL_PATH,				  // ask a certain train to complete a certain path
	GLOBAL_MULTI_PATH,			  // ask a train to traverse to the given location (up to 3 at a time), note that this call is non-blocking
	GLOBAL_MULTI_PATH_KNIGHT_REV, // make the knight reverse
	GLOBAL_DEADLOCK_UNBLOCK,
	GLOBAL_CALIBRATE_VELOCITY, // provide a train id, allow it to calibrate the velocity (running it at max velocity and update accordingly)
	GLOBAL_CALIBRATE_ACCELERATION,
	GLOBAL_CALIBRATE_STARTING,
	GLOBAL_CALIBRATE_STOPPING_DISTANCE, // you have to do the calibration manually, but it just run the train and stop
	GLOBAL_CALIBRATE_BASE_VELOCITY,
	// at the specific sensor
	GLOBAL_CLEAR_TO_SEND, // clear the dirty bit for train server to talk to
	GLOBAL_STOPPING_COMPLETE,
	GLOBAL_MULTI_STOPPING_COMPLETE,
	GLOBAL_MULTI_STOPPING_END,
	GLOBAL_BUNNY_HOP_STOP_COMPLETE,
	GLOBAL_BUNNY_HOP_END_COMPLETE,
	GLOBAL_RELOCATE_END_COMPLETE,
	GLOBAL_REVERSE_END_COMPLETE,
	GLOBAL_RELOCATE_TRANSITION_COMPLETE,
	GLOBAL_REV_HOP_STOP_COMPLETE,
	GLOBAL_REV_HOP_END_COMPLETE,
	GLOBAL_RNG,
	GLOBAL_FULL_RNG,
	GLOBAL_SET_KNIGHT,

	GLOBAL_STOPPING_DISTANCE_START_PHASE_2,
	GLOBAL_COURIER_COMPLETE,
	GLOBAL_BUSY_WAITING_AVAILABILITY,
	GLOBAL_BUSY_WAITING_BUNNY_HOPPING,

	GLOBAL_BUNNY_DIST,

	GLOBAL_COUR_INIT_TRACK,
	GLOBAL_COUR_AWAIT_SENSOR,
	GLOBAL_COUR_SPEED,
	GLOBAL_COUR_REV,
	GLOBAL_COUR_MULTI_STOPPING,
	GLOBAL_COUR_MULTI_STOPPING_END,
	GLOBAL_COUR_BUNNY_HOP_STOP,
	GLOBAL_COUR_BUNNY_HOP_END,
	GLOBAL_COUR_RELOCATE_END,
	GLOBAL_COUR_REVERSE_END,
	GLOBAL_COUR_RELOCATE_TRANSITION,
	GLOBAL_COUR_REV_HOP_STOP,
	GLOBAL_COUR_REV_HOP_END,
	GLOBAL_COUR_STOPPING,
	GLOBAL_COUR_STOPPING_DISTANCE_PHASE_2_DELAY,
	GLOBAL_COUR_CALIBRATE_VELOCITY,
	GLOBAL_OBSERVE,
	GLOBAL_COUR_DEADLOCK_UNBLOCK,
	GLOBAL_COUR_BUSY_WAITING_AVAILABILITY,
	GLOBAL_COUR_BUSY_WAITING_BUNNY_HOPPING,
	// Local Pathing Related
	LOCAL_PATH_SET_TRAIN,
	LOCAL_PATH_SET_PATH,
	LOCAL_PATH_LOCATE,
	LOCAL_PATH_INIT,
	LOCAL_PATH_CALI,
	LOCAL_PATH_CALI_BASE_SPEED,
	LOCAL_PATH_CALI_STOPPING_DISTANCE,
	LOCAL_PATH_CALI_ACCELERATION,
	LOCAL_PATH_DEST,
	LOCAL_PATH_RNG,
	LOCAL_PATH_BUNNY_DIST,

	// Track Server Related
	TRACK_INIT,	  // determine which track are you on
	TRACK_SWITCH, // try to flip a switch, with state update
	TRACK_GET_PATH,
	TRACK_GET_HOT_PATH,		 // hot path ignore all reserved path
	TRACK_TRY_RESERVE,		 // provide a path of connected nodes, try to reserve the corresponding section of the tracks
	TRACK_UNRESERVE,		 // used for unreserve your track once your usage is over
	TRACK_COURIER_COMPLETE,	 // completion of courier with no side-affect
	TRACK_SWITCH_SUBSCRIBE,	 // allowing arbitrary server to subscribe to the state of the switch
	TRACK_GET_SWITCH_STATE,	 // noneblocking, just get the most up-to-date switch state.
	TRACK_GET_RESERVE_STATE, // get the up-to-date reservation state of the track
	TRACK_RNG,				 // rng commands

	TRACK_COUR_SWITCH,
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
	int track_server_tid;

	int local_pathing_tids[10]; // I want this to be Trains::NUM_TRAINS, but circular dependencies :(
};

AddressBook getAddressBook();
}
