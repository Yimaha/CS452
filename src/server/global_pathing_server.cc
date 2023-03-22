

#include "global_pathing_server.h"
#include "courier_pool.h"
#include "train_admin.h"
#include <climits>
using namespace Train;
using namespace Message;
using namespace Planning;
using namespace Routing;

bool Planning::TrainStatus::toSpeed(SpeedLevel s) {
	// it a massive state machine
	if (s == localization.speed) {
		return false;
	}
	if (s == SpeedLevel::SPEED_STOP || s == SpeedLevel::SPEED_MAX) {
		localization.speed = s;
	} else {
		if (localization.speed < s) {
			localization.from = FROM_DOWN;
		} else {
			localization.from = FROM_UP;
		}
		localization.speed = s;
	}
	localization.active_velocity = getVelocity();
	return true;
}

bool Planning::TrainStatus::rev() {
	if (localization.speed != SpeedLevel::SPEED_STOP) {
		return false;
	}
	localization.direction = !localization.direction;
	return true;
}

void Planning::TrainStatus::add_path(int landmark) {
	localization.path.push_back(landmark);
}

void Planning::TrainStatus::updateVelocity(uint64_t velocity) {
	// if it is either terminal velocity or the speed is not from higher velocity level, do this
	speeds[localization.from][static_cast<int>(localization.speed)].velocity
		= (speeds[localization.from][static_cast<int>(localization.speed)].velocity * 7 + velocity) / 8;
}

long Planning::TrainStatus::getVelocity() {
	return speeds[localization.from][static_cast<int>(localization.speed)].velocity;
}

uint64_t Planning::TrainStatus::getTriggerCountForSpeed() {
	return calibration_info[static_cast<int>(localization.speed)].needed_trigger_for_speed;
}

uint64_t Planning::TrainStatus::getCalibrationLoopCount() {
	return calibration_info[static_cast<int>(localization.speed)].loop_count;
}

long Planning::TrainStatus::getStoppingDistance() {
	long ret = speeds[localization.from][static_cast<int>(localization.speed)].stopping_distance;
	// if (localization.speed != SPEED_1) {
	// 	ret += speeds[localization.from][static_cast<int>(SPEED_1)].stopping_distance;
	// }

	return ret;
}

long Planning::TrainStatus::getTrainSpeedLevel() {
	return calibration_info[static_cast<int>(localization.speed)].level_corresponding_speed;
}

void Planning::TrainStatus::locate() {
	localization.state = TrainState::LOCATE;
	pipe_tr(cali_state.slow_calibration_speed);
}

void Planning::TrainStatus::pipe_tr() {
	req_to_courier.header = RequestHeader::GLOBAL_COUR_SPEED;
	req_to_courier.body.command.id = my_id;
	req_to_courier.body.command.action = getTrainSpeedLevel();
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::pipe_tr(char speed) {
	req_to_courier.header = RequestHeader::GLOBAL_COUR_SPEED;
	req_to_courier.body.command.id = my_id;
	req_to_courier.body.command.action = speed;
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::pipe_sw(char id, char dir) {
	switch_state[get_switch_id(id)] = dir;
	req_to_courier.header = RequestHeader::GLOBAL_COUR_SWITCH;
	req_to_courier.body.command.id = id;
	req_to_courier.body.command.action = dir;
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::clear_calibration() {
	cali_state.last_trigger = 0;
	cali_state.needed_trigger = 0;
	localization.distance_traveled = 0;
}

void Planning::TrainStatus::toIdle() {
	localization.state = TrainState::IDLE;
	toSpeed(SpeedLevel::SPEED_STOP);
	pipe_tr();
	localization.path.clear();
	while (!train_sub.empty()) {
		Reply::EmptyReply(train_sub.front());
		train_sub.pop();
	}
}

void Planning::TrainStatus::toStopping(long remaining_distance_mm) {
	localization.state = TrainState::STOPPING;
	long ticks_delay = remaining_distance_mm * PER_SEC_TO_PER_10_MS / getVelocity();
	debug_print(
		addr.term_trans_tid, "trying to stop remain_dist %llu, velocity %llu, ticks %d\r\n", remaining_distance_mm, getVelocity(), ticks_delay);

	PlanningCourReq req_to_courier;
	req_to_courier.header = RequestHeader::GLOBAL_COUR_STOPPING;
	req_to_courier.body.stopping_request.id = my_index;
	req_to_courier.body.stopping_request.delay = ticks_delay;
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::subscribe(int from) {
	train_sub.push(from);
}

void Planning::TrainStatus::sensor_unsub() {
	for (auto it = sensor_subs->begin(); it != sensor_subs->end(); it++) {
		if (it->first == my_index) {
			it = sensor_subs->erase(it);
		}
	}
}
void Planning::TrainStatus::sub_to_sensor(int sensor_id) {
	sensor_unsub();
	sensor_subs->push_back(etl::make_pair(my_index, sensor_id));
}

// Sets switches, determines next sensor to subscribe to, outputs useful info
void Planning::TrainStatus::pre_compute_path(bool set_switches) {

	int sensors_landmark = 0;
	track_node* node;
	for (auto it = localization.path.begin(); it != localization.path.end();) {
		node = &track[*it];
		it++;
		if (node->type == node_type::NODE_BRANCH) {
			if (it == localization.path.end()) {
				break;
			}

			track_node* next_node = &track[*it];
			if (next_node == node->edge[DIR_STRAIGHT].dest) {
				if (set_switches) {
					pipe_sw(node->num, 's');
				}
			} else if (next_node == node->edge[DIR_CURVED].dest) {
				if (set_switches) {
					pipe_sw(node->num, 'c');
				}
			} else {
				Task::_KernelCrash("impossible path from %d to %d\r\n", node - track, next_node - track);
			}
		} else if (node->type == node_type::NODE_SENSOR) {
			sensors_landmark += 1;
			if (sensors_landmark == 2) {
				sub_to_sensor(node->num);
			}
		}
		debug_print(addr.term_trans_tid, "%s ", node->name);
	}
	debug_print(addr.term_trans_tid, "\r\n");
}

bool Planning::TrainStatus::goTo(Dijkstra& dijkstra, int dest, SpeedLevel speed) {
	if (!dijkstra.path(&localization.path, localization.last_node->num, dest)) {
		return false;
	}

	toSpeed(speed);
	localization.path_len = dijkstra.get_dist(dest);

	// let train subscribe to it
	pre_compute_path();
	localization.state = TrainState::GO_TO;
	init_calibration();
	pipe_tr();
	return true;
}

bool Planning::TrainStatus::exit_loop(Dijkstra& dijkstra, int dest, int offset) {
	int sub_sensor = SENSOR_B[0];
	if (dest == SENSOR_B[0]) {
		refill_loop_path();
	} else if (!dijkstra.is_path_possible(SENSOR_B[0], dest)) {
		return false;
	}

	localization.path_len = dijkstra.get_dist(dest);
	if (localization.path_len * TWO_DECIMAL_PLACE < getStoppingDistance()) {
		// lmao try again with D4
		if (!dijkstra.is_path_possible(SENSOR_D[3], dest)) {
			return false;
		} else {
			localization.path_len = dijkstra.get_dist(dest);
			sub_sensor = SENSOR_D[3];
		}
	}

	// Offset error checking
	localization.path_len += offset;
	debug_print(addr.term_trans_tid, "Offset of %d, new path length %d\r\n", offset, localization.path_len);
	if (-offset * TWO_DECIMAL_PLACE > localization.path_len * TWO_DECIMAL_PLACE - getStoppingDistance()) {
		debug_print(addr.term_trans_tid,
					"Negative offset of %d too large for path length %d, stopping distance %d. Ignoring\r\n",
					offset,
					localization.path_len,
					getStoppingDistance());
		return false;
	} else if (offset > OFFSET_BOUND || -offset > OFFSET_BOUND) {
		debug_print(addr.term_trans_tid, "Offset of %d too large! Ignoring\r\n", offset);
		return false;
	} else if (offset > 0) {
		// Positive offset. Create more path, I guess?
		int next_dist = track[dest].edge[DIR_AHEAD].dist;
		while (offset > next_dist) {
			offset -= next_dist;
			dest = track[dest].edge[DIR_AHEAD].dest - track;
			next_dist = track[dest].edge[DIR_AHEAD].dist;
			localization.path.push_back(dest);
		}
	}

	localization.path.clear();
	dijkstra.path(&localization.path, sub_sensor, dest);

	// let train subscribe to it
	pre_compute_path(false);
	sub_to_sensor(sub_sensor);
	localization.state = (sub_sensor == SENSOR_B[0]) ? TrainState::LOOP_EXIT_B1 : TrainState::LOOP_EXIT_D4;
	init_calibration();
	return true;
}

bool Planning::TrainStatus::calibrate_base_velocity(int from) {
	/**
	 * Base velocity calibration is calibrated by first assuming you are already at a fixed location by calling locate
	 * Follow by this call, which slowly advance to the next state
	 */
	subscribe(from);
	init_calibration();
	localization.state = TrainState::CALIBRATE_BASE_VELOCITY;
	pipe_tr(cali_state.slow_calibration_speed);

	localization.distance_traveled = localization.last_node->edge[DIR_AHEAD].dist;
	cali_state.last_trigger = Clock::Time(addr.clock_tid);

	track_node* next_node = localization.last_node->edge[DIR_AHEAD].dest;
	debug_print(addr.term_trans_tid, "last_node %s\r\n", localization.last_node->name);
	while (true) {
		if (next_node->type == node_type::NODE_MERGE) {
			// merge node simply ignore and add the length
			localization.distance_traveled += next_node->edge[DIR_AHEAD].dist;
			next_node = next_node->edge[DIR_AHEAD].dest;
		} else if (next_node->type == node_type::NODE_BRANCH) {
			int switch_id = get_switch_id(next_node->num);
			if (switch_state[switch_id] == 's') {
				localization.distance_traveled += next_node->edge[DIR_STRAIGHT].dist;
				next_node = next_node->edge[DIR_STRAIGHT].dest;
			} else if (switch_state[switch_id] == 'c') {
				localization.distance_traveled += next_node->edge[DIR_CURVED].dist;
				next_node = next_node->edge[DIR_CURVED].dest;
			} else {
				Task::_KernelCrash(
					"impossible path passed from continuous localizations %d %s %d\r\n", switch_id, next_node->name, switch_state[switch_id]);
			}
		} else if (next_node->type == node_type::NODE_SENSOR) {
			sub_to_sensor(next_node->num);
			break;
		} else if (next_node->type == node_type::NODE_EXIT) {
			break; // break without subscription, in this case there is nothing you can do, no more look ahead possible
		}
	}
	return true;
}

// Add a big loop path to the localization structure's path
void Planning::TrainStatus::refill_loop_path() {
	add_path(61);  // D14
	add_path(113); // MR17.
	add_path(77);  // E14
	add_path(72);  // E9
	add_path(95);  // MR8
	add_path(96);  // BR9
	add_path(52);  // D5
	add_path(69);  // E6
	add_path(98);  // BR10
	add_path(51);  // D4
	add_path(21);  // B6
	add_path(105); // MR13
	add_path(43);  // C12
	add_path(107); // MR14
	add_path(3);   // A4
	add_path(31);  // B16
	add_path(108); // MR15
	add_path(41);  // C10
	add_path(110); // BR16
	add_path(16);  // B1
}

bool Planning::TrainStatus::calibrate_velocity(bool from_up, int from, SpeedLevel speed) {
	subscribe(from);
	toSpeed(speed);
	init_calibration();
	for (uint64_t i = 0; i < 3; i++) {
		refill_loop_path();
	}
	localization.path_len = INT_MAX; // we do not need pre-stop for velocity calibration.

	pre_compute_path();
	localization.state = TrainState::CALIBRATE_VELOCITY;
	if (from_up) {
		pipe_tr(FAST_CALIBRATION_SPEED);
	}
	pipe_tr();
	return true;
}

void Planning::TrainStatus::enter_loop(SpeedLevel speed) {
	toSpeed(speed);
	refill_loop_path();
	localization.path_len = INT_MAX; // we do not need pre-stop for velocity calibration.

	init_calibration();
	pre_compute_path();
	localization.state = TrainState::LOOP;
	debug_print(addr.term_trans_tid, "Entering loop\r\n");
	pipe_tr();
}

// Basically, progress in the loop and restore the loop if it is about to run out
void Planning::TrainStatus::handle_train_loop(int sensor_index) {
	clear_traveled_sensor(sensor_index);
	continuous_velocity_calibration();
	if (localization.path.size() == 1) {
		// about to run out of stuff to do, refill the path
		refill_loop_path();
	}
}

// Progress along the path
void Planning::TrainStatus::handle_train_exit_loop(int sensor_index, int sub_sensor) {
	if (sensor_index != sub_sensor) {
		clear_traveled_sensor(sensor_index);
		continuous_velocity_calibration();
	}

	look_ahead();
}

void TrainStatus::calibrate_starting(int from, SpeedLevel speed) {
	/**
	 * if you are locating and you hit a sensor you stop immediately and you set your last_node = desire node
	 */
	toIdle();
	subscribe(from);
	init_calibration();
	cali_state.prev_velocity = getVelocity();
	cali_state.prev_speed = localization.speed;
	toSpeed(speed);
	cali_state.target_velocity = getVelocity();
	cali_state.target_speed = speed;

	add_path(SENSOR_B[0]);		// B1
	add_path(SENSOR_D[13]);		// D14
	add_path(TRACK_MERGES[16]); // MR17.
	add_path(SENSOR_E[13]);		// E14

	localization.distance_traveled = 0;
	localization.distance_traveled += track[SENSOR_B[0]].edge[DIR_AHEAD].dist;
	localization.distance_traveled += track[SENSOR_D[13]].edge[DIR_AHEAD].dist;
	localization.distance_traveled += track[TRACK_MERGES[16]].edge[DIR_AHEAD].dist;

	pre_compute_path();
	localization.state = TrainState::CALIBRATE_STARTING;
	pipe_tr();
}

void TrainStatus::calibrate_acceleration(int from, SpeedLevel start, SpeedLevel end) {
	/**
	 * if you are locating and you hit a sensor you stop immediately and you set your last_node = desire node
	 */
	toIdle();
	subscribe(from);
	init_calibration();
	toSpeed(start);
	cali_state.prev_velocity = getVelocity();
	cali_state.prev_speed = start;
	toSpeed(end);
	cali_state.target_velocity = getVelocity();
	cali_state.target_speed = end;
	cali_state.expected_sensor_hit = 2;
	toSpeed(start);
	// first loop around to reach constant velocity
	add_path(16);  // B1
	add_path(61);  // D14
	add_path(113); // MR17.
	add_path(77);  // E14
	add_path(72);  // E9
	add_path(95);  // MR8
	add_path(96);  // BR9
	add_path(52);  // D5
	add_path(69);  // E6
	add_path(98);  // BR10
	add_path(51);  // D4
	add_path(21);  // B6
	add_path(105); // MR13
	add_path(43);  // C12
	add_path(107); // MR14
	add_path(3);   // A4
	add_path(31);  // B16
	add_path(108); // MR15
	add_path(41);  // C10
	add_path(110); // BR16
	// follow by sudden slow down
	add_path(16);  // B1
	add_path(61);  // D14
	add_path(113); // MR17.
	add_path(77);  // E14

	localization.distance_traveled = 0;
	localization.distance_traveled += track[16].edge[DIR_AHEAD].dist;
	localization.distance_traveled += track[61].edge[DIR_AHEAD].dist;
	localization.distance_traveled += track[113].edge[DIR_AHEAD].dist;

	pre_compute_path();
	localization.state = TrainState::CALIBRATE_ACCELERATION;
	pipe_tr();
}

void Planning::TrainStatus::calibrate_stopping_distance(bool from_up, int from, SpeedLevel speed) {
	toIdle();

	subscribe(from);
	toSpeed(speed);
	init_calibration();
	// second loop is added so the stopping distance will never exceed expectation
	for (int i = 0; i < 2; i++) {
		add_path(16);  // B1
		add_path(61);  // D14
		add_path(113); // MR17.
		add_path(77);  // E14
		add_path(72);  // E9
		add_path(95);  // MR8
		add_path(96);  // BR9
		add_path(52);  // D5
		add_path(69);  // E6
		add_path(98);  // BR10
		add_path(51);  // D4
		add_path(21);  // B6
		add_path(105); // MR13
		add_path(43);  // C12
		add_path(107); // MR14
		add_path(3);   // A4
		add_path(31);  // B16
		add_path(108); // MR1u
		add_path(41);  // C10
		add_path(110); // BR16
	}
	add_path(16);					 // B1
	localization.path_len = INT_MAX; // we do not need pre-stop for velocity calibration.
	cali_state.target_speed = speed;
	cali_state.target_velocity = getVelocity();

	pre_compute_path();

	localization.state = TrainState::CALIBRATE_STOPPING_DISTANCE_PHASE_1;
	if (from_up) {
		pipe_tr(FAST_CALIBRATION_SPEED);
	}
	pipe_tr();
}

void Planning::TrainStatus::calibrate_stopping_distance_phase_2() {
	/**
	 * March slowly toward the next sensor, also record the time
	 */
	localization.state = TrainState::CALIBRATE_STOPPING_DISTANCE_PHASE_2;
	cali_state.last_trigger = Clock::Time(addr.clock_tid);
	pipe_tr(cali_state.slow_calibration_speed);
}

void Planning::TrainStatus::continuous_velocity_calibration() {
	cali_state.needed_trigger -= 1;
	uint64_t current_time = Clock::Time(addr.clock_tid);
	if (cali_state.needed_trigger == 0) {
		if (cali_state.ignore_first) {
			cali_state.ignore_first = false;
		} else {
			// first 6 zeros to nano meter, later 2 zeros from /10ms to /seconds
			uint64_t speed = ((localization.distance_traveled - cali_state.distance_traveled_since_last_calibration) * PER_SEC_TO_PER_10_MS
							  * PER_SEC_TO_PER_10_MS)
							 / (current_time - cali_state.last_trigger); // store as MM / 10 micro seconds
			updateVelocity(speed);
			debug_print(addr.term_trans_tid,
						"time-diff %llu, total-distance %llu, new measurement %llu, actual speed %llu\r\n",
						(current_time - cali_state.last_trigger),
						localization.distance_traveled,
						speed,
						getVelocity());
		}

		// update and clear state
		cali_state.last_trigger = current_time;
		cali_state.distance_traveled_since_last_calibration = localization.distance_traveled;
		cali_state.needed_trigger = getTriggerCountForSpeed();
	}
}

void Planning::TrainStatus::look_ahead() {
	if (localization.path.empty()) {
		toIdle();
		Task::_KernelCrash("trying to lookahead on an empty queue");
	} else {
		int looked_ahead_count = 0;
		long mm_look_ahead = 0;
		for (auto it = localization.path.begin(); it != localization.path.end() && looked_ahead_count < LOOK_AHEAD_SENSORS;) {
			track_node* node = &track[*it];
			it++;
			if (node->type == node_type::NODE_MERGE) {
				mm_look_ahead += (looked_ahead_count < 2) ? node->edge[DIR_AHEAD].dist : 0;
			} else if (node->type == node_type::NODE_BRANCH) {
				if (it == localization.path.end()) {
					break;
				}

				track_node* next_node = &track[*it];
				if (next_node == node->edge[DIR_STRAIGHT].dest) {
					pipe_sw(node->num, 's');
					mm_look_ahead += (looked_ahead_count < LOOK_AHEAD_DISTANCE) ? node->edge[DIR_STRAIGHT].dist : 0;
				} else if (next_node == node->edge[DIR_CURVED].dest) {
					pipe_sw(node->num, 'c');
					mm_look_ahead += (looked_ahead_count < LOOK_AHEAD_DISTANCE) ? node->edge[DIR_CURVED].dist : 0;
				} else {
					Task::_KernelCrash("impossible path passed from calibration\r\n");
				}
			} else if (node->type == node_type::NODE_SENSOR) {
				looked_ahead_count += 1;
				if (looked_ahead_count != LOOK_AHEAD_DISTANCE) {
					mm_look_ahead += (looked_ahead_count < LOOK_AHEAD_DISTANCE) ? node->edge[DIR_AHEAD].dist : 0;
				}
			} else {
				break;
			}
		}

		long remaining_distance_MM = (localization.path_len - (mm_look_ahead + localization.distance_traveled));
		if (looked_ahead_count == 1) {
			// edge case
			toStopping((localization.path_len - localization.distance_traveled) * PER_SEC_TO_PER_10_MS - getStoppingDistance());
		} else if (remaining_distance_MM * PER_SEC_TO_PER_10_MS <= (getStoppingDistance())) {
			toStopping((localization.path_len - localization.distance_traveled) * PER_SEC_TO_PER_10_MS - getStoppingDistance());
		}
	}
}

void Planning::TrainStatus::clear_traveled_sensor(int sensor_index) {
	if (localization.path.empty() || track[localization.path.front()].type != node_type::NODE_SENSOR) {
		Task::_KernelCrash("you are not suppose to be here \r\n");
	} else {
		auto it = localization.path.begin();
		track_node* node = &track[*it];
		it = localization.path.erase(it);
		localization.distance_traveled += node->edge[DIR_AHEAD].dist;
		while (!localization.path.empty() && *it != sensor_index) {
			node = &track[*it];
			if (node->type == node_type::NODE_MERGE) {
				// merge node simply ignore and add the length
				it = localization.path.erase(it);
				if (it == localization.path.end()) {
					break; // nothing more to check
				}
				localization.distance_traveled += node->edge[DIR_AHEAD].dist;
			} else if (node->type == node_type::NODE_BRANCH) {
				it = localization.path.erase(it);
				if (it == localization.path.end()) {
					break; // nothing more to check
				}
				track_node* next_node = &track[*it];
				if (next_node == node->edge[DIR_STRAIGHT].dest) {
					localization.distance_traveled += node->edge[DIR_STRAIGHT].dist;
					pipe_sw(node->num, 's');
				} else if (next_node == node->edge[DIR_CURVED].dest) {
					localization.distance_traveled += node->edge[DIR_CURVED].dist;
					pipe_sw(node->num, 'c');
				} else {
					Task::_KernelCrash("impossible path passed from calibration\r\n");
				}
			} else if (node->type == node_type::NODE_SENSOR) {
				Task::_KernelCrash("you are not suppose to be here, it should be captured by the loop condition\r\n");
			}
		}
	}
}

void Planning::TrainStatus::handle_train_goto(int sensor_index) {
	/**
	 *
	 * 4 step processes
	 * 1. shift location up until the point where you are, update total_dist
	 * 2. if needed, calibrate velocity
	 * 3. if you are not done, figure out who are you goingto trigger next (the next sensor), without shifting the
	 * path_index
	 * 4. keep looking up to a distance and flip all switches alone the way
	 *
	 * note that it is assumed that the last element of the path must be a sensor,
	 */

	// step 1
	clear_traveled_sensor(sensor_index);
	continuous_velocity_calibration();
	look_ahead();
}

void Planning::TrainStatus::handle_train_calibrate_velocity(int sensor_index) {
	/**
	 *
	 * 4 step processes
	 * 1. shift locateion up until the point where you are, update total_dist
	 * 2. if needed, calibrate velocity
	 * 3. if you are not done, figure out who are you goingto trigger next (the next sensor), without shifting the
	 * path_index
	 * 4. keep looking up to a distance and flip all switches alone the way
	 *
	 * note that it is assumed that the last element of the path must be a sensor,
	 */

	// step 1
	clear_traveled_sensor(sensor_index);
	continuous_velocity_calibration();
	if (localization.path.empty()) {
		toIdle();
	}
}

void Planning::TrainStatus::end_acceleration_calibrate() {
	uint64_t current_time = Clock::Time(addr.clock_tid);
	toIdle();
	long t = current_time - cali_state.last_trigger; // 10 ms
	long d = localization.distance_traveled;		 // in MM
	long v_2 = cali_state.target_velocity;
	long v_1 = cali_state.prev_velocity;
	long v_a = (v_2 + v_1) / 2; // average velocity in NM / seconds
	if ((d * TWO_DECIMAL_PLACE) / t < v_a) {
		// convert d from mm to nm, then - nm / seconds * seconds
		long t_1 = (d * PER_SEC_TO_PER_10_MS - (v_2 * t / PER_SEC_TO_PER_10_MS)) * PER_SEC_TO_PER_10_MS / (v_a - v_2);
		long acceleration = (v_2 - v_1) * PER_SEC_TO_PER_10_MS / t_1; // so it is MM / ticks
		accelerations[cali_state.prev_speed][cali_state.target_speed] = acceleration;
		debug_print(addr.term_trans_tid, "debug: t %ld, d %ld, v_2 %ld, v_1 %ld, v_a %ld\r\n", t, d, v_2, v_1, v_a);
		debug_print(addr.term_trans_tid, "acceleration calibration result case 1: acceleration: %ld, t_1 %ld\r\n", acceleration, t_1);
	} else {
		long v_s = (d * PER_SEC_TO_PER_10_MS) / t;
		long v_r = v_s + (v_s - v_1);
		long acceleration = (v_r - v_1) * PER_SEC_TO_PER_10_MS / t; // in nano per second;
		accelerations[cali_state.prev_speed][cali_state.target_speed] = acceleration;
		debug_print(addr.term_trans_tid, "debug: t %ld, d %ld, v_2 %ld, v_1 %ld, v_a %ld, v_s %ld, v_r %ld\r\n", t, d, v_2, v_1, v_a, v_s, v_r);
		debug_print(addr.term_trans_tid, "acceleration calibration result case 2: acceleration: %ld, v_r %ld\r\n", acceleration, v_r);
	}
}

void Planning::TrainStatus::handle_train_calibrate_acceleration(int sensor_index) {
	/**
	 *
	 * 4 step processes
	 * 1. shift locateion up until the point where you are, update total_dist
	 * 2. if needed, calibrate velocity
	 * 3. if you are not done, figure out who are you goingto trigger next (the next sensor), without shifting the
	 * path_index
	 * 4. keep looking up to a distance and flip all switches alone the way
	 *
	 * note that it is assumed that the last element of the path must be a sensor,
	 */

	// step 1
	if (sensor_index == SENSOR_E[13]) {
		cali_state.expected_sensor_hit -= 1;
		if (cali_state.expected_sensor_hit == 0) { // if it hit E14 the second time, calibrate
			end_acceleration_calibrate();
		}
	} else if (sensor_index == SENSOR_B[0]) { // speed it up when it hit the B1 the first time
		toSpeed(cali_state.target_speed);
		cali_state.last_trigger = Clock::Time(addr.clock_tid);
		pipe_tr();
	}
}

void Planning::TrainStatus::handle_train_calibrate_starting(int sensor_index) {
	/**
	 * acceleration assume you are currently on sensor B1 and await your arrival at sensor E14, which is 77
	 *
	 * once it reaches 77, based on the current velocity data, calibrate acceleration
	 */

	if (sensor_index == SENSOR_E[13]) {
		end_acceleration_calibrate();
	}
}

void TrainStatus::handle_train_calibrate_base_velocity() {
	/**
	 * if you are locating and you hit a sensor you stop immediately and you set your last_node = desire node
	 */
	uint64_t current_time = Clock::Time(addr.clock_tid);
	// distance traveled is not in 2 decimal place, and / 10 ms instead of 1s, thus *100 *100
	int mul = TWO_DECIMAL_PLACE * PER_SEC_TO_PER_10_MS;
	cali_state.slow_calibration_mm = (localization.distance_traveled) * mul / (current_time - cali_state.last_trigger);
	toIdle();
	debug_print(addr.term_trans_tid, "base speed of train %d is %llu\r\n", my_id, cali_state.slow_calibration_mm);
}

void TrainStatus::handle_train_calibrate_stopping_distance_phase_1(int sensor_index) {
	/**
	 * if you are locating and you hit a sensor you stop immediately and you set your last_node = desire node
	 */
	clear_traveled_sensor(sensor_index);
	if (sensor_index == SENSOR_B[15]) {
		toSpeed(SpeedLevel::SPEED_STOP);
		pipe_tr();
		debug_print(addr.term_trans_tid, "stopping train %d\r\n", my_id);
		localization.distance_traveled = 0; // previously traveled distance before stopping serves no purpose
		req_to_courier.header = RequestHeader::GLOBAL_COUR_STOPPING_DISTANCE_PHASE_2_DELAY;
		req_to_courier.body.stopping_request.id = my_index;
		courier_pool->request(&req_to_courier);
	}
}

void TrainStatus::handle_train_calibrate_stopping_distance_phase_2(int sensor_index) {
	/**
	 * you should be at stopping speed, which means, you listen to sensor, and record their distance traveld for all
	 * that hits
	 */
	long slow_travled_time = Clock::Time(addr.clock_tid) - cali_state.last_trigger;
	long slow_travled_distance = cali_state.slow_calibration_mm * slow_travled_time / TWO_DECIMAL_PLACE;
	clear_traveled_sensor(sensor_index);
	long real_stop_distance = localization.distance_traveled * TWO_DECIMAL_PLACE - slow_travled_distance;
	// note that now we have real_stopping_distance, assuming our velocity is valid,
	// we can actually also reverse calculate deceleration from speed back to 0,
	// note that real_displacement should never be negative, or at least it shouldn't be negative
	debug_print(addr.term_trans_tid,
				"from %d targetspeed %d, velocity %ld, speedstop %d, real_stop_distance %ld\r\n",
				localization.from,
				cali_state.target_speed,
				cali_state.target_velocity,
				SpeedLevel::SPEED_STOP,
				real_stop_distance);
	long v_a = cali_state.target_velocity / 2;
	long t = real_stop_distance * TWO_DECIMAL_PLACE / v_a; // in ticks
	long acceleration = -cali_state.target_velocity * TWO_DECIMAL_PLACE / t;

	speeds[localization.from][cali_state.target_speed].stopping_distance = real_stop_distance < 0 ? 0 : real_stop_distance;
	accelerations[cali_state.target_speed][SpeedLevel::SPEED_STOP] = acceleration;
	toIdle();
	debug_print(addr.term_trans_tid, "real stop distance for train %d is %ld, thus, deceleration %ld\r\n", my_id, real_stop_distance, acceleration);
}

void TrainStatus::handle_train_locate(int sensor_index) {
	/**
	 * if you are locating and you hit a sensor you stop immediately and you set your last_node = desire node
	 */

	toIdle();
	localization.last_node = &track[sensor_index];
	localization.mm_offset = 0;
	debug_print(addr.term_trans_tid, "train %d is currently at sensor %s\r\n", my_id, localization.last_node->name);
}

void Planning::TrainStatus::sensor_notify(int sensor_index) {
	if (localization.state == TrainState::GO_TO) {
		handle_train_goto(sensor_index);
	} else if (localization.state == TrainState::LOOP) {
		handle_train_loop(sensor_index);
	} else if (localization.state == TrainState::LOOP_EXIT_B1) {
		handle_train_exit_loop(sensor_index, SENSOR_B[0]);
	} else if (localization.state == TrainState::LOOP_EXIT_D4) {
		handle_train_exit_loop(sensor_index, SENSOR_D[3]);
	} else if (localization.state == TrainState::CALIBRATE_VELOCITY) {
		handle_train_calibrate_velocity(sensor_index);
	} else if (localization.state == TrainState::CALIBRATE_ACCELERATION) {
		handle_train_calibrate_acceleration(sensor_index);
	} else if (localization.state == TrainState::CALIBRATE_STARTING) {
		handle_train_calibrate_starting(sensor_index);
	} else if (localization.state == TrainState::LOCATE) {
		handle_train_locate(sensor_index);
	} else if (localization.state == TrainState::CALIBRATE_BASE_VELOCITY) {
		handle_train_calibrate_base_velocity();
	} else if (localization.state == TrainState::CALIBRATE_STOPPING_DISTANCE_PHASE_1) {
		handle_train_calibrate_stopping_distance_phase_1(sensor_index);
	} else if (localization.state == TrainState::CALIBRATE_STOPPING_DISTANCE_PHASE_2) {
		handle_train_calibrate_stopping_distance_phase_2(sensor_index);
	}
	continuous_localization(sensor_index);
}

void TrainStatus::init_calibration() {
	localization.distance_traveled = 0;
	localization.time_traveled = Clock::Time(addr.clock_tid);
	cali_state.ignore_first = true;
	cali_state.distance_traveled_since_last_calibration = 0;
	cali_state.needed_trigger = getTriggerCountForSpeed();
	cali_state.last_trigger = localization.time_traveled; // the first calibration is irrlevant anyway
}

void TrainStatus::continuous_localization(int sensor_index) {
	if (localization.state == TrainState::LOCATE) {
		return; // during locate, we don't want to continuously localize, cause we are reinitializing
	}

	long current_time = Clock::Time(addr.clock_tid);
	long tick_since_last_local = current_time - localization.time_traveled;
	localization.time_traveled = current_time;
	long diff_in_ticks = localization.expected_arrival_ticks - tick_since_last_local;
	long diff_in_dist = diff_in_ticks * localization.active_velocity / TWO_DECIMAL_PLACE;

	// last node is guarenteed to be a sensor nod
	localization.last_node = &track[sensor_index]; // this is your current location
	track_node* next_node = localization.last_node->edge[DIR_AHEAD].dest;
	long cont_local_look_ahead_mm = localization.last_node->edge[DIR_AHEAD].dist;
	while (true) {
		if (next_node->type == node_type::NODE_MERGE) {
			// merge node simply ignore and add the length
			cont_local_look_ahead_mm += next_node->edge[DIR_AHEAD].dist;
			next_node = next_node->edge[DIR_AHEAD].dest;
		} else if (next_node->type == node_type::NODE_BRANCH) {
			int switch_id = get_switch_id(next_node->num);
			if (switch_state[switch_id] == 's') {
				cont_local_look_ahead_mm += next_node->edge[DIR_STRAIGHT].dist;
				next_node = next_node->edge[DIR_STRAIGHT].dest;
			} else if (switch_state[switch_id] == 'c') {
				cont_local_look_ahead_mm += next_node->edge[DIR_CURVED].dist;
				next_node = next_node->edge[DIR_CURVED].dest;
			} else {
				Task::_KernelCrash(
					"impossible path passed from continuous localizations %d %s %d\r\n", switch_id, next_node->name, switch_state[switch_id]);
			}
		} else if (next_node->type == node_type::NODE_SENSOR) {
			sensor_subs->push_back(etl::make_pair(my_index, next_node->num)); // let train subscribe to it
			break;
		} else if (next_node->type == node_type::NODE_EXIT) {
			break; // break without subscription, in this case there is nothing you can do, no more look ahead possible
		}
	}

	debug_print(addr.term_trans_tid,
				"train %d is at %s, next is %s, arrival_tick_diff is %ld, arrival_dist_diff is %ld\r\n",
				my_id,
				localization.last_node->name,
				next_node->name,
				diff_in_ticks,
				diff_in_dist);

	localization.expected_arrival_ticks = cont_local_look_ahead_mm * TWO_DECIMAL_PLACE * TWO_DECIMAL_PLACE / localization.active_velocity;
}

void initialize_all_train(TrainStatus* trains,
						  Courier::CourierPool<PlanningCourReq>* couriers,
						  etl::list<etl::pair<int, int>, NUM_TRAINS>* sensors,
						  track_node track[],
						  char* switch_state) {
	for (int i = 0; i < NUM_TRAINS; i++) {
		trains[i].my_id = TRAIN_NUMBERS[i];
		trains[i].my_index = i;
		trains[i].courier_pool = couriers;
		trains[i].sensor_subs = sensors;
		trains[i].addr = getAddressBook();
		trains[i].track = track;
		trains[i].switch_state = switch_state;
		if (TRAIN_NUMBERS[i] == 24) {
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_1)] = { 16000, 12500 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_1)] = { 20000, 12500 };
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_MAX)] = { 55000, 101500 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0, 0 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_1)] = { 2, 3, 7 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_MAX)] = { 2, 3, 13 };
			trains[i].cali_state.slow_calibration_speed = 3;
			trains[i].cali_state.slow_calibration_mm = 1684; // 16.84 mm / s
			// mm / s^2 in 2 decimal places
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_1)] = 7800;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 12220;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 12700;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_1)] = -15419;
		}

		if (TRAIN_NUMBERS[i] == 78) {
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_1)] = { 12700, 7966 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_1)] = { 12700, 7966 };
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_MAX)] = { 49000, 96523 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0, 0 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_1)] = { 2, 3, 7 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_MAX)] = { 2, 3, 14 };
			trains[i].cali_state.slow_calibration_speed = 4;
			trains[i].cali_state.slow_calibration_mm = 2507;
			// mm / s^2 in 2 decimal places
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_1)] = 3990;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 6939;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 7959;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_1)] = -13900;
		}
	}
}

void Planning::global_pathing_server() {
	Name::RegisterAs(GLOBAL_PATHING_SERVER_NAME);
	AddressBook addr = getAddressBook();
	TrainStatus trains[NUM_TRAINS];
	char switch_state[NUM_SWITCHES];
	for (int i = 0; i < NUM_SWITCHES; i++) {
		switch_state[i] = 'c';
	}

	Courier::CourierPool<PlanningCourReq> courier_pool = Courier::CourierPool<PlanningCourReq>(&global_pathing_courier, Priority::HIGH_PRIORITY);
	// first one tells who you are, second one tells you which sensor are you sub to
	// -1 means sub to all sensor, only used during calibration
	// (notice that it means you cannot have multiple trains calibrate at the same time)
	etl::list<etl::pair<int, int>, NUM_TRAINS> sensor_subs;
	track_node track[TRACK_MAX]; // This is guaranteed to be big enough.
	init_trackb(track);			 // default configuration is part a
	initialize_all_train(trains, &courier_pool, &sensor_subs, track, switch_state);
	Dijkstra dijkstra = Dijkstra(track);

	int clear_to_send = CLEAR_TO_SEND_LIMIT;
	(void)clear_to_send;
	// auto check_clear_to_send = [&]() {
	// 	if (!clear_to_send) {
	// 		// need better log system.
	// 		debug_print(addr.term_trans_tid, "you are not clear to send, yet receive a clear to send request \r\n");
	// 	} else {
	// 		clear_to_send -= 1;
	// 	}
	// };

	PlanningCourReq req_to_unblock = { RequestHeader::GLOBAL_COUR_AWAIT_SENSOR, { 0x0 } };
	courier_pool.request(&req_to_unblock);

	auto sensor_update = [&](char* sensor_state) {
		for (int i = 0; i < Sensor::NUM_SENSOR_BYTES; i++) {
			for (int j = 1; j <= CHAR_BIT; j++) {
				if (sensor_state[i] & (1 << (CHAR_BIT - j))) {
					int sensor_index = i * CHAR_BIT + j - 1;
					for (auto it = sensor_subs.begin(); it != sensor_subs.end(); it++) {
						if (it->second == sensor_index || it->second == -1) {
							trains[it->first].sensor_notify(sensor_index);
							it = sensor_subs.erase(it);
							break;
						}
					}
				}
			}
		}
	};

	int from;
	PlanningServerReq req;
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(req));
		switch (req.header) {
		case RequestHeader::GLOBAL_CLEAR_TO_SEND: {
			courier_pool.receive(from);
			clear_to_send = CLEAR_TO_SEND_LIMIT;
			char* sensor_state = req.body.sensor_state;
			courier_pool.request(&req_to_unblock);
			// now we unblock each of the sensor if needed.
			sensor_update(sensor_state);
			break;
		}

		case RequestHeader::GLOBAL_COURIER_COMPLETE: {
			courier_pool.receive(from);
			break;
		}
		case RequestHeader::GLOBAL_STOPPING_COMPLETE: {
			courier_pool.receive(from);
			int train_index = req.body.stopping_request.id;
			trains[train_index].toIdle();
			break;
		}
		case RequestHeader::GLOBAL_STOPPING_DISTANCE_START_PHASE_2: {
			courier_pool.receive(from);
			int train_index = req.body.stopping_request.id;
			trains[train_index].calibrate_stopping_distance_phase_2();
			break;
		}
		case RequestHeader::GLOBAL_LOCATE: {
			int train_index = Train::train_num_to_index(req.body.command.id);

			trains[train_index].subscribe(from);
			trains[train_index].sub_to_sensor(-1);
			trains[train_index].locate();
			break;
		}
		case RequestHeader::GLOBAL_LOOP: {
			int train_index = Train::train_num_to_index(req.body.command.id);
			SpeedLevel speed = req.body.routing_request.speed;
			trains[train_index].enter_loop(speed);

			Reply::EmptyReply(from);
			break;
		}
		case RequestHeader::GLOBAL_EXIT_LOOP: {
			int train_index = Train::train_num_to_index(req.body.command.id);
			int dest = req.body.routing_request.dest;
			int offset = req.body.routing_request.offset;
			if (trains[train_index].exit_loop(dijkstra, dest, offset)) {
				trains[train_index].subscribe(from);
			} else {
				debug_print(addr.term_trans_tid, "Unreachable destination from loop!\r\n");
				Reply::EmptyReply(from);
			}

			break;
		}
		case RequestHeader::GLOBAL_SET_TRACK: {
			Message::Reply::EmptyReply(from); // unblock after job is done
			if (req.body.info == GLOBAL_PATHING_TRACK_A_ID) {
				init_tracka(track);
			} else if (req.body.info == GLOBAL_PATHING_TRACK_B_ID) {
				init_trackb(track);
			} else {
				Task::_KernelCrash("invalid configuration for tracks\r\n");
			}
			break;
		}
		case RequestHeader::GLOBAL_PATH: {
			int train_index = Train::train_num_to_index(req.body.routing_request.id);
			int dest = req.body.routing_request.dest;
			SpeedLevel speed = req.body.routing_request.speed;

			if (trains[train_index].goTo(dijkstra, dest, speed)) {
				trains[train_index].subscribe(from);
			} else {
				debug_print(addr.term_trans_tid, "unreachable location, refuse to path!\r\n");
				Reply::Reply(from, (const char*)UNABLE_TO_PATH, sizeof(int));
			}
			break;
		}
		case RequestHeader::GLOBAL_CALIBRATE_VELOCITY: {
			/**
			 *  assuming you are starting at location B1 -> D14 on track A
			 * 	Note that global_calibration is not gonna be blocking, thus once the environment is set,
			 *  each of the smaller train is basically block on waiting on sensor reading
			 *
			 * 	In the future we should introduce mechanism that route train out of the loop for calibration,
			 *  and introduce dynamic calibration.
			 */
			int train_index = Train::train_num_to_index(req.body.calibration_request.id);
			Planning::SpeedLevel speed = req.body.calibration_request.level;
			bool from_up = req.body.calibration_request.from_up;
			trains[train_index].calibrate_velocity(from_up, from, speed);
			break;
		}
		case RequestHeader::GLOBAL_CALIBRATE_BASE_VELOCITY: {
			int train_index = Train::train_num_to_index(req.body.command.id);
			trains[train_index].calibrate_base_velocity(from);
			break;
		}

		case RequestHeader::GLOBAL_CALIBRATE_ACCELERATION: {
			int train_index = Train::train_num_to_index(req.body.calibration_request_acceleration.id);
			Planning::SpeedLevel start = req.body.calibration_request_acceleration.from;
			Planning::SpeedLevel end = req.body.calibration_request_acceleration.to;
			trains[train_index].calibrate_acceleration(from, start, end);
			break;
		}

		case RequestHeader::GLOBAL_CALIBRATE_STARTING: {
			int train_index = Train::train_num_to_index(req.body.calibration_request.id);
			Planning::SpeedLevel speed = req.body.calibration_request.level;
			trains[train_index].calibrate_starting(from, speed);
			break;
		}

		case RequestHeader::GLOBAL_CALIBRATE_STOPPING_DISTANCE: {
			int train_index = Train::train_num_to_index(req.body.calibration_request.id);
			Planning::SpeedLevel speed = req.body.calibration_request.level;
			bool from_up = req.body.calibration_request.from_up;
			trains[train_index].calibrate_stopping_distance(from_up, from, speed);
			break;
		}

		default: {
			Task::_KernelCrash("Received invalid request: %d at global pathing\r\n", req.header);
		}
		} // switch
	}
}

void Planning::global_pathing_courier() {
	AddressBook addr = getAddressBook();

	int from;
	PlanningCourReq req;
	PlanningServerReq req_to_admin;
	TrainAdminReq req_to_train;

	Sensor::SensorAdminReq req_to_sensor = { RequestHeader::SENSOR_AWAIT_STATE, 0x0 }; // this type of request doesn't need a body

	// worker only has few types
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(PlanningCourReq));
		Message::Reply::EmptyReply(from); // unblock caller right away
		switch (req.header) {
		case RequestHeader::GLOBAL_COUR_AWAIT_SENSOR: {
			req_to_admin = { RequestHeader::GLOBAL_CLEAR_TO_SEND, RequestBody { 0x0 } };

			Message::Send::Send(addr.sensor_admin_tid,
								(const char*)&req_to_sensor,
								sizeof(Sensor::SensorAdminReq),
								(char*)&req_to_admin.body.sensor_state,
								Sensor::NUM_SENSOR_BYTES);
			// now we have the next update time, we should notify trian admin that we are allow to sent again.
			Message::Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		case RequestHeader::GLOBAL_COUR_SWITCH: {
			req_to_admin = { RequestHeader::GLOBAL_COURIER_COMPLETE, RequestBody { 0x0 } };

			req_to_train.header = RequestHeader::TRAIN_SWITCH;
			req_to_train.body.command.id = req.body.command.id;
			req_to_train.body.command.action = req.body.command.action;
			Send::SendNoReply(addr.train_admin_tid, reinterpret_cast<char*>(&req_to_train), sizeof(req_to_train));
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}

		case RequestHeader::GLOBAL_COUR_SPEED: {
			req_to_admin = { RequestHeader::GLOBAL_COURIER_COMPLETE, RequestBody { 0x0 } };

			req_to_train.header = RequestHeader::TRAIN_SPEED;
			req_to_train.body.command.id = req.body.command.id;
			req_to_train.body.command.action = req.body.command.action;
			Send::SendNoReply(addr.train_admin_tid, reinterpret_cast<char*>(&req_to_train), sizeof(req_to_train));
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		case RequestHeader::GLOBAL_COUR_STOPPING: {
			if (req.body.stopping_request.delay > 0) {
				Clock::Delay(addr.clock_tid, req.body.stopping_request.delay);
			}
			req_to_admin = { RequestHeader::GLOBAL_STOPPING_COMPLETE, RequestBody { 0x0 } };
			req_to_admin.body.stopping_request.id = req.body.stopping_request.id;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		case RequestHeader::GLOBAL_COUR_STOPPING_DISTANCE_PHASE_2_DELAY: {
			Clock::Delay(addr.clock_tid, PHASE_2_CALIBRATION_PAUSE);
			req_to_admin = { RequestHeader::GLOBAL_STOPPING_DISTANCE_START_PHASE_2, RequestBody { 0x0 } };
			req_to_admin.body.stopping_request.id = req.body.stopping_request.id;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		default:
			Task::_KernelCrash("GP_Train Courier illegal type: [%d]\r\n", req.header);
		}
	}
}
