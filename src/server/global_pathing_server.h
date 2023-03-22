#pragma once

#include "../etl/circular_buffer.h"
#include "../interrupt/clock.h"
#include "../routing/dijkstra.h"
#include "../routing/track_data_new.h"
#include "../rpi.h"
#include "../utils/utility.h"
#include "courier_pool.h"
#include "request_header.h"
#include "train_admin.h"
using namespace Train;
using namespace Message;
using namespace Routing;

/**
 * All velocity unit is ideally MM, expressed to 2 decimals.
 * for example, 134.04 mm/s = 13404
 * 1524.24 mm = 152424
 *
 */
namespace Planning
{
constexpr char GLOBAL_PATHING_SERVER_NAME[] = "GPATH";
constexpr uint64_t GLOBAL_PATHING_TRACK_A_ID = 1;
constexpr uint64_t GLOBAL_PATHING_TRACK_B_ID = 2;
constexpr long PER_SEC_TO_PER_10_MS = 100;
constexpr long TWO_DECIMAL_PLACE = 100;
constexpr long OFFSET_BOUND = 200;
constexpr int CLEAR_TO_SEND_LIMIT = 1;
constexpr int FROM_DOWN = 0;
constexpr int FROM_UP = 1;
constexpr int NUM_TRAIN_SUBS = 16;

const int FAST_CALIBRATION_SPEED = 13;
const int LOOK_AHEAD_SENSORS = 4;
const int LOOK_AHEAD_DISTANCE = 2;
const int PHASE_2_CALIBRATION_PAUSE = 700;

const int SENSORS_PER_LETTER = 16;
const int SENSOR_A[SENSORS_PER_LETTER] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const int SENSOR_B[SENSORS_PER_LETTER] = { 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
const int SENSOR_C[SENSORS_PER_LETTER] = { 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47 };
const int SENSOR_D[SENSORS_PER_LETTER] = { 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 };
const int SENSOR_E[SENSORS_PER_LETTER] = { 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79 };

const int TRACK_BRANCHES[NUM_SWITCHES] = { 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122 };
const int TRACK_MERGES[NUM_SWITCHES] = { 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123 };

enum SpeedLevel { SPEED_STOP = 0, SPEED_1 = 1, SPEED_MAX = 2 };
enum class TrainState : uint32_t {
	IDLE,
	LOCATE,
	GO_TO,
	LOOP,
	LOOP_EXIT_B1,
	LOOP_EXIT_D4,
	STOPPING,
	CALIBRATE_VELOCITY,
	CALIBRATE_STOPPING_DISTANCE_PHASE_1,
	CALIBRATE_STOPPING_DISTANCE_PHASE_2,
	CALIBRATE_BASE_VELOCITY,
	CALIBRATE_ACCELERATION,
	CALIBRATE_STARTING,
};

enum Exception : uint32_t { NONE, UNABLE_TO_PATH };
struct Command {
	char id;
	char action;
};

struct StoppingRequset {
	char id;
	uint64_t delay;
};

struct CalibrationRequest {
	char id;
	SpeedLevel level;
	bool from_up;
};

struct RoutingRequest {
	char id;
	int dest;
	int offset;
	SpeedLevel speed;
};

struct AccelerationCalibrationRequest {
	char id;
	SpeedLevel from;
	SpeedLevel to;
};
union RequestBody
{
	uint64_t info;
	Command command;
	StoppingRequset stopping_request;
	RoutingRequest routing_request;
	CalibrationRequest calibration_request;
	AccelerationCalibrationRequest calibration_request_acceleration;
	char sensor_state[Sensor::NUM_SENSOR_BYTES];
};

struct PlanningServerReq {
	Message::RequestHeader header;
	RequestBody body;
} __attribute__((aligned(8)));

struct PlanningCourReq {
	Message::RequestHeader header;
	RequestBody body;
} __attribute__((aligned(8)));
class TrainStatus {
public:
	struct SpeedInfo {
		// the terminal velocity, max, stop only uses the regular velocity
		uint64_t velocity;
		uint64_t stopping_distance;
	};

	struct CalibrationInfo {
		uint64_t needed_trigger_for_speed;
		uint64_t loop_count;
		uint64_t level_corresponding_speed;
	};

	struct CalibrationState {
		uint64_t slow_calibration_speed = 2; // speed level used for localization.
		long slow_calibration_mm = 0;
		uint64_t last_trigger = 0;
		uint64_t needed_trigger = 0;
		long distance_traveled_since_last_calibration = 0;
		bool ignore_first = false;

		// acceleration related parameters
		long prev_velocity;
		long target_velocity;
		SpeedLevel prev_speed;
		SpeedLevel target_speed;
		long expected_sensor_hit = 0;
	};

	struct LocalizationInfo {
		TrainState state = TrainState::IDLE;
		SpeedLevel speed = SpeedLevel::SPEED_STOP;
		bool from = FROM_DOWN;
		bool direction = false;
		track_node* last_node = nullptr;
		uint64_t mm_offset = 0;
		etl::list<int, PATH_LIMIT> path;
		long path_len = 0;
		long distance_traveled = 0;
		long time_traveled = 0;
		long active_velocity = 0;
		long expected_arrival_ticks = 0;
	};

	TrainStatus() { }
	// initialization related functions
	void seedSpeedInfo(uint64_t velocity_1, uint64_t velocity_1_from_up, uint64_t velocity_max);
	// setup function before setting state for each state transition
	void locate();
	void pre_compute_path(bool set_switches = true);
	bool goTo(Dijkstra& dijkstra, int dest, SpeedLevel speed);
	bool calibrate_velocity(bool from_up, int from, SpeedLevel speed);
	void calibrate_stopping_distance(bool from_up, int from, SpeedLevel speed);
	void calibrate_stopping_distance_phase_2();

	bool calibrate_base_velocity(int from);
	void calibrate_starting(int from, SpeedLevel speed);
	void calibrate_acceleration(int from, SpeedLevel start, SpeedLevel end);
	void enter_loop(SpeedLevel speed);
	bool exit_loop(Dijkstra& dijkstra, int dest, int offset);

	void updateVelocity(uint64_t velocity);

	long getVelocity();
	uint64_t getTriggerCountForSpeed();
	long getTrainSpeedLevel();
	uint64_t getCalibrationLoopCount();
	long getStoppingDistance();

	bool rev();
	void subscribe(int from);
	void sensor_unsub();
	void sub_to_sensor(int sensor_id);
	void add_path(int landmark);

	void sensor_notify(int sensor_index);
	void continuous_localization(int sensor_index);
	void continuous_velocity_calibration();
	void look_ahead();
	void clear_traveled_sensor(int sensor_index);
	void handle_train_goto(int sensor_index);
	void handle_train_calibrate_velocity(int sensor_index);
	void handle_train_locate(int sensor_index);
	void handle_train_calibrate_base_velocity();
	void end_acceleration_calibrate();
	void handle_train_calibrate_acceleration(int sensor_index);
	void handle_train_calibrate_starting(int sensor_index);
	void handle_train_calibrate_stopping_distance_phase_1(int sensor_index);
	void handle_train_calibrate_stopping_distance_phase_2(int sensor_index);
	void handle_train_loop(int sensor_index);
	void handle_train_exit_loop(int sensor_index, int sub_sensor);

	void clear_calibration();
	void init_calibration();

	char my_id = 0;
	int my_index = -1;

	// localization related parameter
	LocalizationInfo localization;
	// calibration related parameters
	SpeedInfo speeds[2][static_cast<int>(SpeedLevel::SPEED_MAX) + 1];
	long accelerations[static_cast<int>(SpeedLevel::SPEED_MAX) + 1][static_cast<int>(SpeedLevel::SPEED_MAX) + 1] = { 0 };
	// constant information related to calibration
	CalibrationInfo calibration_info[static_cast<int>(SpeedLevel::SPEED_MAX) + 1];
	// non-constant information related to calibration
	CalibrationState cali_state;

	// shared attribute with main thread
	AddressBook addr;
	Courier::CourierPool<PlanningCourReq>* courier_pool = nullptr;
	etl::list<etl::pair<int, int>, Train::NUM_TRAINS>* sensor_subs = nullptr;
	etl::queue<int, NUM_TRAIN_SUBS> train_sub;
	track_node* track;
	PlanningCourReq req_to_courier;
	char* switch_state;

	void toIdle();

private:
	bool toSpeed(SpeedLevel s);
	void pipe_sw(char id, char dir);
	void pipe_tr();
	void pipe_tr(char speed);
	void toStopping(long remaining_distance_NM);
	void refill_loop_path();
};

void global_pathing_server();
void global_pathing_courier();
}
