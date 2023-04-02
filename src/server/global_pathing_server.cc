

#include "global_pathing_server.h"
#include "../routing/kinematic.h"
#include "courier_pool.h"
#include "train_admin.h"
#include <climits>
using namespace Train;
using namespace Message;
using namespace Planning;
using namespace Routing;

bool Planning::TrainStatus::toSpeed(SpeedLevel s) {
	// it a massive state machine]
	localization.acceleration_start_timestamp = Clock::Time(addr.clock_tid);
	localization.previous_velocity = getVelocity();
	// localization.acceleration = accelerations[localization.speed][s]; // more redendency, but it works, and it more failure proof
	if (s != localization.speed) {
		if (localization.speed < s) {
			localization.from = FROM_DOWN;
		} else {
			localization.from = FROM_UP;
		}
		localization.speed = s;
	}
	localization.eventual_velocity = getVelocity();
	return true;
}

bool Planning::TrainStatus::isAccelerating(int current_time) {
	if (localization.acceleration == 0) {
		return false;
	}
	// check the previous recorded started acceleration time, if it is still accelerating,
	int64_t time_diff = current_time - localization.acceleration_start_timestamp;
	int64_t acceleration_time_tick
		= (localization.eventual_velocity - localization.previous_velocity) * TWO_DECIMAL_PLACE / localization.acceleration;
	return time_diff >= acceleration_time_tick;
}

void Planning::TrainStatus::raw_reverse() {
	localization.direction = !localization.direction;
	pipe_rv();
	localization.last_node = localization.last_node->reverse;
	localization.last_reserved_node = track[localization.last_reserved_node].reverse->num;

	// debug_print(addr.term_trans_tid, "trying to reverse \r\n");

	// return true;
}

void Planning::TrainStatus::reverse() {
	if (localization.deadlocked) {
		Task::_KernelCrash("%d trying to reverse whene system is in deadlock", my_id);
	}
	localization.state = TrainState::REVERSING;
	Track::TrackServerReq req_to_track;
	req_to_track.body.reservation.len = 1;
	req_to_track.body.reservation.path[0] = localization.last_reserved_node;
	// if (!try_reserve(&req_to_track)) {
	// 	req_to_courier.header = RequestHeader::GLOBAL_COUR_BUSY_WAITING_BUNNY_HOPPING;
	// 	req_to_courier.body.command.id = my_id;
	// 	courier_pool->request(&req_to_courier);
	// } else {
	// 	if (localization.deadlocked) {
	// 		Task::_KernelCrash("%d reverse failed due to lack of ability ", my_id);
	// 	}
	int ticks = 10000 * TWO_DECIMAL_PLACE / cali_state.slow_calibration_mm;
	pipe_tr(localization.stopping_speed);
	localization.direction = !localization.direction;
	localization.last_node = localization.last_node->reverse;
	localization.last_reserved_node = track[localization.last_reserved_node].reverse->num;
	PlanningCourReq req_to_courier;
	req_to_courier.header = RequestHeader::GLOBAL_COUR_RELOCATE_END;
	req_to_courier.body.stopping_request.id = my_index;
	req_to_courier.body.stopping_request.delay = ticks;
	req_to_courier.body.stopping_request.need_reverse = true;
	courier_pool->request(&req_to_courier);
	// }
}

void Planning::TrainStatus::reverse_stopping() {
	toSpeed(SPEED_STOP);
	pipe_tr();
}

void Planning::TrainStatus::add_path(int landmark) {
	localization.path.push_back(landmark);
}

bool Planning::TrainStatus::try_reserve(Track::TrackServerReq* reservation_request) {
	reservation_request->header = RequestHeader::TRACK_TRY_RESERVE;
	reservation_request->body.reservation.train_id = my_id;
	Track::ReservationStatus status;
	Send::Send(addr.track_server_tid, (const char*)reservation_request, sizeof(Track::TrackServerReq), (char*)&status, sizeof(status));
	if (status.successful && reservation_request->body.reservation.len >= 1) {
		localization.last_reserved_node = reservation_request->body.reservation.path[reservation_request->body.reservation.len - 1];
	}
	localization.deadlocked = status.dead_lock_detected;
	// debug_print(addr.term_trans_tid, "try_reserve: is successful? %d\r\n", status.successful);
	return status.successful;
}

void Planning::TrainStatus::cancel_reservation(Track::TrackServerReq* reservation_request) {
	reservation_request->header = RequestHeader::TRACK_UNRESERVE;
	reservation_request->body.reservation.train_id = my_id;
	Send::SendNoReply(addr.track_server_tid, (const char*)reservation_request, sizeof(Track::TrackServerReq));
}

void Planning::TrainStatus::updateVelocity(uint64_t velocity) {
	// if it is either terminal velocity or the speed is not from higher velocity level, do this
	speeds[localization.from][static_cast<int>(localization.speed)].velocity
		= (speeds[localization.from][static_cast<int>(localization.speed)].velocity * 7 + velocity) / 8;
}

int64_t Planning::TrainStatus::getVelocity() {
	return speeds[localization.from][static_cast<int>(localization.speed)].velocity;
}

uint64_t Planning::TrainStatus::getTriggerCountForSpeed() {
	return calibration_info[static_cast<int>(localization.speed)].needed_trigger_for_speed;
}

uint64_t Planning::TrainStatus::getCalibrationLoopCount() {
	return calibration_info[static_cast<int>(localization.speed)].loop_count;
}

int64_t Planning::TrainStatus::getStoppingDistance() {
	int64_t ret = speeds[localization.from][static_cast<int>(localization.speed)].stopping_distance;
	return ret;
}

int64_t Planning::TrainStatus::getTrainSpeedLevel() {
	return calibration_info[static_cast<int>(localization.speed)].level_corresponding_speed;
}

int64_t Planning::TrainStatus::getMinStableDist() {
	int64_t terminal_velocity = getVelocity();
	int64_t A_1 = accelerations[SPEED_STOP][static_cast<int>(localization.speed)];
	int64_t A_2 = accelerations[static_cast<int>(localization.speed)][SPEED_STOP];
	// debug_print(addr.term_trans_tid,
	// 			"what is going on? %llu, %llu, %lld, %lld \r\n",
	// 			terminal_velocity,
	// 			A_1,
	// 			A_2,
	// 			Kinematic::calculate_traversal_min_dist(A_1, A_2, terminal_velocity));
	return Kinematic::calculate_traversal_min_dist(A_1, A_2, terminal_velocity) + cali_state.min_stable_dist_margin;
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

void Planning::TrainStatus::pipe_rv() {
	req_to_courier.header = RequestHeader::GLOBAL_COUR_REV;
	req_to_courier.body.command.id = my_id;
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::pipe_tr(char speed) {
	req_to_courier.header = RequestHeader::GLOBAL_COUR_SPEED;
	req_to_courier.body.command.id = my_id;
	req_to_courier.body.command.action = speed;
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::clear_calibration() {
	cali_state.last_trigger = 0;
	cali_state.needed_trigger = 0;
	localization.distance_traveled = 0;
}

void Planning::TrainStatus::toIdle() {
	if (localization.state != TrainState::IDLE) {
		debug_print(addr.term_trans_tid, "heading into idle for train %d\r\n", my_id);
		localization.state = TrainState::IDLE;
		toSpeed(SpeedLevel::SPEED_STOP);
		pipe_tr();
		localization.path.clear();
		while (!train_sub.empty()) {
			Reply::EmptyReply(train_sub.front());
			train_sub.pop();
		}
	}
}

void Planning::TrainStatus::toStopping(int64_t remaining_distance_mm) {
	localization.state = TrainState::STOPPING;
	int64_t ticks_delay = remaining_distance_mm * TWO_DECIMAL_PLACE / getVelocity();
	debug_print(
		addr.term_trans_tid, "trying to stop remain_dist %llu, velocity %llu, ticks %d\r\n", remaining_distance_mm, getVelocity(), ticks_delay);

	PlanningCourReq req_to_courier;
	req_to_courier.header = RequestHeader::GLOBAL_COUR_STOPPING;
	req_to_courier.body.stopping_request.id = my_index;
	req_to_courier.body.stopping_request.delay = ticks_delay;
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::toMultiStopping(int64_t remaining_distance_mm) {
	localization.state = TrainState::MULTI_STOPPING;
	int64_t ticks_delay = remaining_distance_mm * TWO_DECIMAL_PLACE / getVelocity();
	debug_print(addr.term_trans_tid,
				"%d trying to multi stop remain_dist %lld, velocity %llu, ticks %d\r\n",
				my_id,
				remaining_distance_mm,
				getVelocity(),
				ticks_delay);

	PlanningCourReq req_to_courier;
	req_to_courier.header = RequestHeader::GLOBAL_COUR_MULTI_STOPPING;
	req_to_courier.body.stopping_request.id = my_index;
	req_to_courier.body.stopping_request.delay = ticks_delay;
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::toMultiStoppingFromZero(int64_t remaining_distance_mm) {
	localization.state = TrainState::MULTI_STOPPING;
	int64_t V_a = getVelocity() / 2; // average velocity
	int64_t time = getVelocity() * TWO_DECIMAL_PLACE / accelerations[SPEED_STOP][localization.speed];
	remaining_distance_mm -= V_a * time / TWO_DECIMAL_PLACE;
	int64_t ticks_delay = time + remaining_distance_mm * TWO_DECIMAL_PLACE / getVelocity();
	debug_print(addr.term_trans_tid,
				"%d trying to multi stop from zero remain_dist %lld, velocity %llu, ticks %d, acceleration_dist %lld, acceleration_time %lld\r\n",
				my_id,
				remaining_distance_mm,
				getVelocity(),
				ticks_delay,
				V_a * time / TWO_DECIMAL_PLACE,
				time);

	PlanningCourReq req_to_courier;
	req_to_courier.header = RequestHeader::GLOBAL_COUR_MULTI_STOPPING;
	req_to_courier.body.stopping_request.id = my_index;
	req_to_courier.body.stopping_request.delay = ticks_delay;
	courier_pool->request(&req_to_courier);
}
/**
 * bunny hop should only trigger in the should stop case of multi_waiting
 */
void Planning::TrainStatus::tryBunnyHopping() {
	debug_print(addr.term_trans_tid, "%d in bunny hopping \r\n", my_id);
	if (localization.path.empty()) {
		Task::_KernelCrash("%d trying to reserve ahead on an empty queue", my_id);
	}
	Track::TrackServerReq reservation_request = {};
	reservation_request.body.reservation.len = 0;
	uint64_t D_t = 0;

	for (auto it = localization.path.begin(); it != localization.path.end();) {
		track_node* node = &track[*it];
		reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;

		it++;
		if (it == localization.path.end()) {
			break;
		}
		if (node->type == node_type::NODE_MERGE) {
			D_t += node->edge[DIR_AHEAD].dist;
		} else if (node->type == node_type::NODE_BRANCH) {
			track_node* next_node = &track[*it];
			if (next_node == node->edge[DIR_STRAIGHT].dest) {
				D_t += node->edge[DIR_STRAIGHT].dist;
			} else if (next_node == node->edge[DIR_CURVED].dest) {
				D_t += node->edge[DIR_CURVED].dist;
			} else {
				Task::_KernelCrash("impossible path passed from calibration\r\n");
			}
		} else if (node->type == node_type::NODE_SENSOR) {
			D_t += node->edge[DIR_AHEAD].dist;
		} else {
			break;
		}
	}

	if (try_reserve(&reservation_request)) {
		localization.state = TrainState::MULTI_BUNNY_HOP;
		toSpeed(SPEED_MAX);
		int64_t A_1 = accelerations[SPEED_STOP][SPEED_MAX];
		int64_t A_2 = accelerations[SPEED_MAX][SPEED_STOP];
		etl::pair<int64_t, int64_t> t = Kinematic::calculate_bunny_hop(A_1, A_2, D_t * 100);
		debug_print(addr.term_trans_tid, "%d trying to bunny hop: %lld %lld %lld %d %d \r\n", my_id, D_t, A_1, A_2, t.first, t.second);

		PlanningCourReq req_to_courier;
		req_to_courier.header = RequestHeader::GLOBAL_COUR_BUNNY_HOP_STOP;
		req_to_courier.body.stopping_request.id = my_index;
		req_to_courier.body.stopping_request.delay = t.first + cali_state.bunny_ped;
		courier_pool->request(&req_to_courier);
		pipe_tr();
	} else {
		if (localization.deadlocked) {
			// if you are in deadlock, wait a bit and recheck
			req_to_courier.header = RequestHeader::GLOBAL_COUR_BUSY_WAITING_AVAILABILITY;
			req_to_courier.body.command.id = my_id;
			courier_pool->request(&req_to_courier);
		} else {
			req_to_courier.header = RequestHeader::GLOBAL_COUR_BUSY_WAITING_BUNNY_HOPPING;
			req_to_courier.body.command.id = my_id;
			courier_pool->request(&req_to_courier);
		}
	}
}

void Planning::TrainStatus::bunnyHopStopping() {
	pipe_tr(localization.stopping_speed);
}

void Planning::TrainStatus::handle_train_bunny_hopping(int sensor_index) {
	clear_traveled_sensor(sensor_index, true);
	if (localization.path.empty()) {
		Task::_KernelCrash("empty path given to bunny hop");
	}
	debug_print(addr.term_trans_tid, "we reached here with sensor %d and expected %d \r\n", sensor_index, localization.path.back());
	if (sensor_index == localization.path.back()) { // found the last sensor
		toSpeed(SPEED_STOP);
		pipe_tr(); // actually pipe tr to stop, maybe?
		path_end_relocate();
	}
	
}

bool Planning::TrainStatus::is_next_branch() {
	return track[localization.last_reserved_node].edge[DIR_AHEAD].dest->type == node_type::NODE_BRANCH;
}

/**
 * The goal of the relocation is to fine tune the stopping location, this is typically not
 * called if you can consistently land on certain location, but rebustness is a huge issue,
 * thus to avoid accidental deadzones, etc, we going to use this function before going back into
 * state such as multi_waiting
 *
 * state assume your metal bar is on the sensor, and use calibration slow speed to "place you on the right location"
 */
void Planning::TrainStatus::path_end_relocate() {
	localization.state = TrainState::MULTI_RELOCATE;
	PlanningCourReq req_to_courier;
	req_to_courier.header = RequestHeader::GLOBAL_COUR_RELOCATE_END;
	req_to_courier.body.stopping_request.id = my_index;
	req_to_courier.body.stopping_request.need_reverse = false;
	req_to_courier.body.stopping_request.delay = 0;
	if (localization.reverse_after) {
		// if the goal is the reverse after, move the train forward under fixed velocity for reverse  (and fine toon)
		if (localization.direction) {
			req_to_courier.body.stopping_request.delay = 6000 * TWO_DECIMAL_PLACE / cali_state.slow_calibration_mm;
		} else {
			// if we are in the reverse configuration, then move forward by 5 cm (and fine toon)
			req_to_courier.body.stopping_request.delay = 1000 * TWO_DECIMAL_PLACE / cali_state.slow_calibration_mm;
		}
		pipe_tr(localization.stopping_speed); // actually pipe tr to stop, maybe?
	} else {
		// we do not wish to reverse after
		if (is_next_branch()) {
			pipe_rv(); // pipe_rv doesn't actually change the state, but rather just change direction directly
			req_to_courier.body.stopping_request.need_reverse = true;
			if (localization.direction) {
				req_to_courier.body.stopping_request.delay = 5000 * TWO_DECIMAL_PLACE / cali_state.slow_calibration_mm;
			} else {
				// if we are in the reverse configuration, then move forward by 5 cm
				req_to_courier.body.stopping_request.delay = 13000 * TWO_DECIMAL_PLACE / cali_state.slow_calibration_mm;
			}
			pipe_tr(localization.stopping_speed); // actually pipe tr to stop, maybe?
		}
	}
	courier_pool->request(&req_to_courier);

}

void Planning::TrainStatus::relocate_complete(bool need_reverse) {
	toSpeed(SPEED_STOP);
	pipe_tr();
	// wait for 1 seconds so train offcially stops.
	PlanningCourReq req_to_courier;
	req_to_courier.header = RequestHeader::GLOBAL_COUR_RELOCATE_TRANSITION;
	req_to_courier.body.stopping_request.id = my_index;
	req_to_courier.body.stopping_request.need_reverse = need_reverse;
	courier_pool->request(&req_to_courier);
}

void Planning::TrainStatus::relocate_transition(bool need_reverse) {
	if (need_reverse) {
		pipe_rv();
	}
	if (localization.path.size() == 1) {
		schedule_next_multi();
	} else {
		handle_train_multi_waiting();
	}
	manual_subscription();
}

bool Planning::TrainStatus::should_stop(int64_t error_margin) {
	int64_t remaining_distance_MM = (localization.path_len - (localization.sensor_dist[1] + localization.distance_traveled));
	if (localization.sensor_ahead == 1 || remaining_distance_MM * TWO_DECIMAL_PLACE - (getStoppingDistance()) <= error_margin) {
		return true;
	}
	return false;
}

void Planning::TrainStatus::subscribe(int from) {
	train_sub.push(from);
}

void Planning::TrainStatus::sensor_unsub() {
	for (auto it = sensor_subs->begin(); it != sensor_subs->end();) {
		if (it->first == my_index) {
			it = sensor_subs->erase(it);
		} else {
			it++;
		}
	}
}
void Planning::TrainStatus::sub_to_sensor(int sensor_id) {
	sensor_unsub();
	sensor_subs->push_back(etl::make_pair(my_index, sensor_id));
}

void Planning::TrainStatus::sub_to_sensor_no_delete(int sensor_id) {
	sensor_subs->push_back(etl::make_pair(my_index, sensor_id));
}

// Sets switches, determines next sensor to subscribe to, outputs useful info
void Planning::TrainStatus::pre_compute_path(bool early_bird_reservation) {
	/**
	 * You are not suppose to call this function under TC2 assumption, this function is limited to TC1 and calibration of velocity
	 * it will reserve and flip all the switches for you, and will cause problem if you are not the only train on the track.
	 */

	int sensors_landmark = 0;
	track_node* node;
	Track::TrackServerReq reservation_request = {};
	reservation_request.body.reservation.len = 0;
	for (auto it = localization.path.begin(); it != localization.path.end();) {
		node = &track[*it];
		reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
		it++;
		if (node->type == node_type::NODE_SENSOR) {
			sensors_landmark += 1;
			if (sensors_landmark == 2) {
				localization.next_sensor = node - track;
				sub_to_sensor(node->num);
			}
		}
		debug_print(addr.term_trans_tid, "%s ", node->name);
	}
	debug_print(addr.term_trans_tid, "\r\n");
	if (early_bird_reservation) {
		try_reserve(&reservation_request);
	}
}

void Planning::TrainStatus::simple_pre_compute_path() {
	/**
	 * You are not suppose to call this function under TC2 assumption, this function is limited to TC1 and calibration of velocity
	 * it will reserve and flip all the switches for you, and will cause problem if you are not the only train on the track.
	 */

	int sensors_landmark = 0;
	track_node* node;
	Track::TrackServerReq reservation_request = {};
	reservation_request.body.reservation.len = 0;
	for (auto it = localization.path.begin(); it != localization.path.end();) {
		node = &track[*it];
		reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
		it++;
		if (node->type == node_type::NODE_SENSOR) {
			sensors_landmark += 1;
			if (sensors_landmark == 2) {
				sub_to_sensor_no_delete(node->num);
			}
		}
		debug_print(addr.term_trans_tid, "%s ", node->name);
	}
	debug_print(addr.term_trans_tid, "\r\n");
}

bool Planning::TrainStatus::goTo(int dest, SpeedLevel speed) {
	if (localization.last_node == nullptr) {
		debug_print(addr.term_trans_tid, "trying to control train %d with unknown current location\r\n", my_id);
	} else {
		Track::PathRespond path_res = get_path(localization.last_node->num, dest);
		if (!path_res.successful) {
			return false;
		}
		store_path(path_res);

		toSpeed(speed);
		localization.path_len = path_res.path_len;

		// let train subscribe to it
		pre_compute_path();
		localization.state = TrainState::GO_TO;
		init_calibration();
		pipe_tr();
	}
	return true;
}

void Planning::TrainStatus::multi_path(int dest) {
	if (localization.last_node == nullptr || localization.last_reserved_node == -1 || localization.destinations.full()) {
		debug_print(addr.term_trans_tid, "trying to control train %d with unknown current location\r\n", my_id);
	} else {
		localization.destinations.push(dest);
		if (localization.state == TrainState::IDLE) {
			schedule_next_multi();
			manual_subscription();
		}
	}
}

bool Planning::TrainStatus::exit_loop(int dest, int offset) {
	int sub_sensor = SENSOR_B[0];
	Track::PathRespond path_res = get_path(SENSOR_B[0], dest);
	if (dest == SENSOR_B[0]) {
		refill_loop_path();
	} else if (!path_res.successful) {
		return false;
	}

	localization.path_len = path_res.path_len;
	if (localization.path_len * TWO_DECIMAL_PLACE < getStoppingDistance()) {
		path_res = get_path(SENSOR_D[3], dest);
		if (!path_res.successful) {
			return false;
		} else {
			localization.path_len = path_res.path_len;
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
	store_path(path_res);

	// let train subscribe to it
	pre_compute_path();
	sub_to_sensor(sub_sensor);
	localization.state = (sub_sensor == SENSOR_B[0]) ? TrainState::LOOP_EXIT_B1 : TrainState::LOOP_EXIT_D4;
	init_calibration();
	return true;
}

Track::PathRespond Planning::TrainStatus::get_path(int source, int dest, bool allow_reverse) {
	Track::TrackServerReq req_to_track;
	req_to_track.header = RequestHeader::TRACK_GET_PATH;
	req_to_track.body.start_and_end.start = source;
	req_to_track.body.start_and_end.end = dest;
	req_to_track.body.start_and_end.allow_reverse = allow_reverse;

	Track::PathRespond res;
	if (source == dest) {
		res.successful = false;
	} else {
		Send::Send(addr.track_server_tid, (const char*)&req_to_track, sizeof(req_to_track), (char*)&res, sizeof(res));
	}
	return res;
}

void Planning::TrainStatus::store_path(Track::PathRespond& res) {
	if (!res.successful || !localization.path.empty()) {
		Task::_KernelCrash("try to filled a path that failed to route \r\n");
	}
	for (int i = 0; i < PATH_LIMIT && res.path[i] != res.dest; i++) {
		localization.path.push_back(res.path[i]);
	}
	localization.path.push_back(res.dest);
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
	update_switch_state();
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

// Progress aint64_t the path
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
	if (from_up) {
		toSpeed(SPEED_MAX);
		pipe_tr();
	}
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
			uint64_t speed
				= ((localization.distance_traveled - cali_state.distance_traveled_since_last_calibration) * TWO_DECIMAL_PLACE * TWO_DECIMAL_PLACE)
				  / (current_time - cali_state.last_trigger); // store as MM / 10 micro seconds
			updateVelocity(speed);
			// debug_print(addr.term_trans_tid,
			// 			"time-diff %llu, total-distance %llu, new measurement %llu, actual speed %llu\r\n",
			// 			(current_time - cali_state.last_trigger),
			// 			localization.distance_traveled,
			// 			speed,
			// 			getVelocity());
		}

		// update and clear state
		cali_state.last_trigger = current_time;
		cali_state.distance_traveled_since_last_calibration = localization.distance_traveled;
		cali_state.needed_trigger = getTriggerCountForSpeed();
	}
}

/**
 * This is where we produce a sequence of prediction based on how far a sensor is to our current sensor
 * based on few information
 * 1. how far is the sensor from us, thus giving us displaement
 * 2. what is the initial velocity
 * 3. what is the final velocity
 * 4. what is our acceleration
 *
 * through a series of calculation, we can some what predict the result if our acceleration model is relatively accurate
 * assiting this with some reasonable error margin, we can have a reservation system that can predict when
 * you are going through certain sensor, and give us a more fine gram control.
 */
void Planning::TrainStatus::predict_future_sensor(int64_t* mm_look_ahead) {
	int64_t current_time = Clock::Time(addr.clock_tid);
	// int64_t tick_since_last_local = current_time - localization.time_traveled;
	localization.time_traveled = current_time;
	// int64_t diff_in_ticks = localization.expected_arrival_ticks[0] - tick_since_last_local;
	// int64_t diff_in_dist = diff_in_ticks * localization.eventual_velocity / TWO_DECIMAL_PLACE;

	// debug_print(addr.term_trans_tid, "arrival_tick_diff is %ld, arrival_dist_diff is %ld \r\n", diff_in_ticks, diff_in_dist);

	if (!isAccelerating(current_time)) {
		// we are not accelerating, thus we can actually predict with constant velocity assumption
		// debug_print(addr.term_trans_tid, "predicting_future: ");
		for (int i = 1; i < 32 && mm_look_ahead[i] != 0; i++) {
			localization.expected_arrival_ticks[i - 1] = mm_look_ahead[i] * TWO_DECIMAL_PLACE * TWO_DECIMAL_PLACE / localization.eventual_velocity;
			// debug_print(addr.term_trans_tid, "%d: %ld ", i, localization.expected_arrival_ticks[i - 1]);
		}
		// debug_print(addr.term_trans_tid, "\r\n");
	} else {
		// we are actively experiencing acceleration, first ask when will the acceleration end
	}
}

void Planning::TrainStatus::look_ahead() {
	if (localization.path.empty()) {
		toIdle();
		Task::_KernelCrash("trying to lookahead on an empty queue");
	} else {
		// note that the index = 0 is always, going to be 0, kept as a redendency value incase the first landmark is not a sensor which will be a
		int64_t mm_look_ahead[32];
		for (int i = 0; i < 32; i++) {
			mm_look_ahead[i] = 0;
		}
		int mm_look_ahead_index = 0;

		// once reservation server is ready, the LOOK_AHEAD_SENSORS can be completely removed
		for (auto it = localization.path.begin(); it != localization.path.end() && mm_look_ahead_index < LOOK_AHEAD_SENSORS;) {
			track_node* node = &track[*it];
			it++;
			if (it == localization.path.end()) {
				break;
			}
			if (node->type == node_type::NODE_MERGE) {
				mm_look_ahead[mm_look_ahead_index] += node->edge[DIR_AHEAD].dist;
			} else if (node->type == node_type::NODE_BRANCH) {
				track_node* next_node = &track[*it];
				if (next_node == node->edge[DIR_STRAIGHT].dest) {
					mm_look_ahead[mm_look_ahead_index] += node->edge[DIR_STRAIGHT].dist;
				} else if (next_node == node->edge[DIR_CURVED].dest) {
					mm_look_ahead[mm_look_ahead_index] += node->edge[DIR_CURVED].dist;
				} else {
					Task::_KernelCrash("impossible path passed from calibration\r\n");
				}
			} else if (node->type == node_type::NODE_SENSOR) {
				mm_look_ahead_index += 1;
				mm_look_ahead[mm_look_ahead_index] += node->edge[DIR_AHEAD].dist;
			} else {
				break;
			}
		}

		predict_future_sensor(mm_look_ahead);

		int64_t remaining_distance_MM = (localization.path_len - (mm_look_ahead[1] + localization.distance_traveled));
		if (mm_look_ahead_index == 1 || remaining_distance_MM * TWO_DECIMAL_PLACE - (getStoppingDistance()) <= 0) {
			toStopping((localization.path_len - localization.distance_traveled) * TWO_DECIMAL_PLACE - getStoppingDistance());
		}
	}
}

void Planning::TrainStatus::simple_look_ahead() {
	if (localization.path.empty()) {
		Task::_KernelCrash("trying to lookahead on an empty queue");
	}
	// note that the index = 0 is always, going to be 0, kept as a redendency value incase the first landmark is not a sensor which will be a
	for (int i = 0; i < 32; i++) {
		localization.sensor_dist[i] = 0;
	}
	localization.sensor_ahead = 0;
	// once reservation server is ready, the LOOK_AHEAD_SENSORS can be completely removed
	for (auto it = localization.path.begin(); it != localization.path.end() && localization.sensor_ahead < LOOK_AHEAD_SENSORS;) {
		track_node* node = &track[*it];
		it++;
		if (it == localization.path.end()) {
			break;
		}
		if (node->type == node_type::NODE_MERGE) {
			localization.sensor_dist[localization.sensor_ahead] += node->edge[DIR_AHEAD].dist;
		} else if (node->type == node_type::NODE_BRANCH) {
			track_node* next_node = &track[*it];
			if (next_node == node->edge[DIR_STRAIGHT].dest) {
				localization.sensor_dist[localization.sensor_ahead] += node->edge[DIR_STRAIGHT].dist;
			} else if (next_node == node->edge[DIR_CURVED].dest) {
				localization.sensor_dist[localization.sensor_ahead] += node->edge[DIR_CURVED].dist;
			} else {
				Task::_KernelCrash("impossible path passed from calibration\r\n");
			}
		} else if (node->type == node_type::NODE_SENSOR) {
			localization.sensor_ahead += 1;
			localization.sensor_dist[localization.sensor_ahead] += node->edge[DIR_AHEAD].dist;
		} else {
			break;
		}
	}
}

SpeedLevel Planning::TrainStatus::get_viable_speed() {
	int64_t remaining_distance_MM = (localization.path_len - (localization.distance_traveled));
	for (int i = SPEED_1; i > 0; i--) {
		toSpeed(SpeedLevel(i));
		if (remaining_distance_MM > (getMinStableDist() / 100)) {
			debug_print(addr.term_trans_tid,
						"%d picked speed %d with res_dist %lld and min_dist %lld\r\n",
						my_id,
						i,
						remaining_distance_MM,
						getMinStableDist() / 100);
			return SpeedLevel(i);
		}
	}
	toSpeed(SpeedLevel::SPEED_STOP);
	return SpeedLevel::SPEED_STOP;
}

bool Planning::TrainStatus::reserve_ahead() {
	/**
	 * The goal is that at least reserve 1 sensor + until the next sensor that is a stopping distance away from where you are
	 * or if you run out of path, just book until the path.
	 */
	if (localization.path.empty()) {
		Task::_KernelCrash("%d trying to reserve ahead on an empty queue", my_id);
	}
	int64_t stopping_disance = 0;
	int min_sensor = 0;
	Track::TrackServerReq reservation_request = {};
	reservation_request.body.reservation.len = 0;
	auto it = localization.path.begin();

	// at least book 1 sensor
	track_node* node;

	for (; it != localization.path.end(); it++) {
		node = &track[*it];
		reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
		if (node->type == node_type::NODE_SENSOR) {
			min_sensor += 1;
			if (min_sensor == RESERVE_AHEAD_MIN_SENSOR) {
				if (etl::next(it) == localization.path.end()) {
					it++;
				}
				break;
			}
		}
	}
	// for (int i = 0; i < reservation_request.body.reservation.len; i++) {
	// 	debug_print(addr.term_trans_tid, "%d ", reservation_request.body.reservation.path[i]);
	// }
	// debug_print(addr.term_trans_tid, "\r\n");

	if (it == localization.path.end()) {
		return try_reserve(&reservation_request);
	}
	// at least book until your stopping distance
	for (; it != localization.path.end() && stopping_disance <= (getStoppingDistance() / TWO_DECIMAL_PLACE);) {
		if (node->type == node_type::NODE_MERGE || node->type == node_type::NODE_SENSOR) {
			stopping_disance += node->edge[DIR_AHEAD].dist;
		} else if (node->type == node_type::NODE_BRANCH && etl::next(it) != localization.path.end()) {
			track_node* next_node = &track[*(etl::next(it))];
			if (next_node == node->edge[DIR_STRAIGHT].dest) {
				stopping_disance += node->edge[DIR_STRAIGHT].dist;
			} else if (next_node == node->edge[DIR_CURVED].dest) {
				stopping_disance += node->edge[DIR_CURVED].dist;
			} else {
				Task::_KernelCrash("impossible path passed from calibration\r\n");
			}
		}
		it++;
		if (it == localization.path.end()) {
			break;
		}
		node = &track[*it];
		reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
	}
	// for (int i = 0; i < reservation_request.body.reservation.len; i++) {
	// 	debug_print(addr.term_trans_tid, "%d ", reservation_request.body.reservation.path[i]);
	// }
	// debug_print(addr.term_trans_tid, "\r\n");
	if (it != localization.path.end()) {
		it++;
	}
	// at least book until a sensor
	for (; it != localization.path.end()
		   && track[reservation_request.body.reservation.path[reservation_request.body.reservation.len - 1]].type != NODE_SENSOR;
		 it++) {
		reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
	}

	// for (int i = 0; i < reservation_request.body.reservation.len; i++) {
	// 	debug_print(addr.term_trans_tid, "%d ", reservation_request.body.reservation.path[i]);
	// }
	// debug_print(addr.term_trans_tid, "\r\n");

	return try_reserve(&reservation_request);
}

void Planning::TrainStatus::clear_traveled_sensor(int sensor_index, bool clear_reservation) {
	if (localization.path.empty() || track[localization.path.front()].type != node_type::NODE_SENSOR) {
		Task::_KernelCrash("you are not suppose to be here \r\n");
	} else {
		Track::TrackServerReq reservation_request;
		reservation_request.body.reservation.len = 0;

		auto it = localization.path.begin();
		reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
		track_node* node = &track[*it];
		it = localization.path.erase(it);
		localization.distance_traveled += node->edge[DIR_AHEAD].dist;
		while (!localization.path.empty() && *it != sensor_index) {
			reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
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
				} else if (next_node == node->edge[DIR_CURVED].dest) {
					localization.distance_traveled += node->edge[DIR_CURVED].dist;
				} else {
					Task::_KernelCrash("impossible path passed from calibration\r\n");
				}
			} else if (node->type == node_type::NODE_SENSOR) {
				Task::_KernelCrash("received sensor %s, but expecting sensor %d\r\n", node->name, sensor_index);
			}
		}
		if (clear_reservation) {
			cancel_reservation(&reservation_request);
		}
	}
}

void Planning::TrainStatus::clear_traveled_sensor_until(int sensor_index, bool clear_reservation) {
	if (localization.path.empty() || track[localization.path.front()].type != node_type::NODE_SENSOR) {
		Task::_KernelCrash("you are not suppose to be here \r\n");
	} else {
		Track::TrackServerReq reservation_request;
		reservation_request.body.reservation.len = 0;

		auto it = localization.path.begin();
		reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
		track_node* node = &track[*it];
		it = localization.path.erase(it);
		localization.distance_traveled += node->edge[DIR_AHEAD].dist;
		while (!localization.path.empty() && *it != sensor_index) {
			reservation_request.body.reservation.path[reservation_request.body.reservation.len++] = *it;
			node = &track[*it];
			if (node->type == node_type::NODE_MERGE || node->type == node_type::NODE_SENSOR) {
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
				} else if (next_node == node->edge[DIR_CURVED].dest) {
					localization.distance_traveled += node->edge[DIR_CURVED].dist;
				} else {
					Task::_KernelCrash("impossible path passed from calibration\r\n");
				}
			}
		}
		if (clear_reservation) {
			cancel_reservation(&reservation_request);
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

int Planning::TrainStatus::get_reverse(int node_id) {
	return track[node_id].reverse->num;
}

void Planning::TrainStatus::schedule_next_multi() {
	localization.path.clear();
	if (localization.reverse_after) {
		localization.reverse_after = false;
		raw_reverse();
	}
	if (localization.destinations.empty()) {
		debug_print(addr.term_trans_tid, "empty\r\n");
		toIdle();
	} else {
		Track::PathRespond path_res;
		path_res.successful = false;
		int should_reverse = false;
		while (!path_res.successful && !localization.destinations.empty()) {
			if (localization.last_reserved_node == localization.destinations.front()) {
				localization.destinations.pop();
				continue;
			}
			path_res = get_path(localization.last_reserved_node, localization.destinations.front(), true);
			debug_print(addr.term_trans_tid,
						"%d trying location %d from last_sensor: %d\r\n",
						my_id,
						localization.destinations.front(),
						localization.last_reserved_node);
			if (!path_res.successful) {
				path_res = get_path(get_reverse(localization.last_reserved_node), localization.destinations.front(), true);
				debug_print(addr.term_trans_tid,
							"%d trying location %d from last_sensor: %d\r\n",
							my_id,
							localization.destinations.front(),
							get_reverse(localization.last_reserved_node));
				if (path_res.successful) {
					should_reverse = !should_reverse;
				}
			}

			if (!path_res.successful || !path_res.reverse) {
				// eithery have the right path, or you reverse, the n have the right path
				localization.destinations.pop();
			}
		}
		if (path_res.successful) {
			init_calibration();
			store_path(path_res);
			if (path_res.reverse) {
				debug_print(addr.term_trans_tid, "path_len, is it wrong? 2 %d %d \r\n", path_res.path_len, path_res.rev_offset);

				localization.reverse_after = true;
				localization.path_len = path_res.path_len - (localization.direction ? MULTI_ERROR_MARGIN_STRAIGHT : MULTI_ERROR_MARGIN_REVERSE);
			} else {
				debug_print(addr.term_trans_tid, "path_len, is it wrong? %d \r\n", path_res.path_len);
				localization.path_len = path_res.path_len - (localization.direction ? MULTI_ERROR_MARGIN_STRAIGHT : MULTI_ERROR_MARGIN_REVERSE);
			}
			handle_train_multi_waiting(should_reverse);

		} else {
			debug_print(addr.term_trans_tid, "%d no path \r\n", my_id);
			toIdle();
		}
	}
}

bool Planning::TrainStatus::handle_deadlock() {
	if (localization.path.empty()) {
		Task::_KernelCrash("%d empty localization path passed \r\n", my_id);
	}
	debug_print(addr.term_trans_tid, "%d detected deadlock! resolving %d %d \r\n", my_id, localization.last_reserved_node, localization.path.back());
	Track::PathRespond path_res = get_path(get_reverse(localization.last_reserved_node), localization.path.back(), true);
	if (path_res.successful) {
		localization.deadlocked = false;
		clear_calibration();
		localization.path.clear();
		store_path(path_res);
		if (path_res.reverse) {
			localization.reverse_after = true;
			localization.reverse_offset = path_res.rev_offset;
			localization.path_len
				= path_res.path_len - (localization.direction ? MULTI_ERROR_MARGIN_STRAIGHT : MULTI_ERROR_MARGIN_REVERSE) + path_res.rev_offset;
		} else {
			localization.path_len = path_res.path_len - (localization.direction ? MULTI_ERROR_MARGIN_STRAIGHT : MULTI_ERROR_MARGIN_REVERSE);
		}
		return true;
	} else {
		debug_print(addr.term_trans_tid, "%d deadlock cannot be resolved! routing to random dest nearby to resolve it\r\n", my_id);
		// which is not done at the moment lol

		return false;
	}
}

void Planning::TrainStatus::handle_train_multi_waiting(bool should_reverse) {
	localization.state = TrainState::MULTI_WAITING;
	debug_print(addr.term_trans_tid, "%d in multi_waiting \r\n", my_id);
	if (localization.deadlocked) {
		// if you are in deadlock, do a reverse and update your path
		if (handle_deadlock()) {
			should_reverse = !should_reverse;
		}
	}
	if (should_reverse) {
		reverse();
	} else {
		if (get_viable_speed() != SPEED_STOP) {
			if (!reserve_ahead()) {
				req_to_courier.header = RequestHeader::GLOBAL_COUR_BUSY_WAITING_AVAILABILITY;
				req_to_courier.body.command.id = my_id;
				courier_pool->request(&req_to_courier);
			} else {
				// instead of this, we should check if the first guy is a branch, if a first
				localization.deadlocked = false;
				simple_look_ahead();
				if (should_stop()) {
					pipe_tr();
					toMultiStoppingFromZero((localization.path_len - localization.distance_traveled
											 + (localization.direction ? MULTI_ERROR_MARGIN_STRAIGHT : MULTI_ERROR_MARGIN_REVERSE))
												* TWO_DECIMAL_PLACE
											- getStoppingDistance());
				} else {
					cali_state.ignore_first = true;
					localization.state = TrainState::MULTI_PATHING;
					pipe_tr();
				}
			}
		} else {
			tryBunnyHopping();
		}
	}
}

uint64_t Planning::TrainStatus::missing_distance() {
	uint64_t miss_dist = 0;
	bool found_last_reserve = false;
	for (auto it = localization.path.begin(); it != localization.path.end();) {
		track_node* node = &track[*it];
		it++;
		if (it == localization.path.end()) {
			break;
		}
		if (node->type == node_type::NODE_MERGE) {
			miss_dist += found_last_reserve ? node->edge[DIR_AHEAD].dist : 0;
		} else if (node->type == node_type::NODE_BRANCH) {
			track_node* next_node = &track[*it];
			if (next_node == node->edge[DIR_STRAIGHT].dest) {
				miss_dist += found_last_reserve ? node->edge[DIR_STRAIGHT].dist : 0;
			} else if (next_node == node->edge[DIR_CURVED].dest) {
				miss_dist += found_last_reserve ? node->edge[DIR_CURVED].dist : 0;
			} else {
				Task::_KernelCrash("impossible path passed from calibration\r\n");
			}
		} else if (node->type == node_type::NODE_SENSOR) {
			if (node->num == localization.last_reserved_node) {
				found_last_reserve = true;
			}
			miss_dist += found_last_reserve ? node->edge[DIR_AHEAD].dist : 0;
		} else {
			break;
		}
	}
	return miss_dist;
}

void Planning::TrainStatus::handle_train_multi_pathing(int sensor_index) {
	debug_print(addr.term_trans_tid, "%d in multi_pathing \r\n", my_id);
	clear_traveled_sensor(sensor_index, true);
	simple_look_ahead();
	if (!reserve_ahead()) {
		// need to see if lower speed helps
		toMultiStopping((localization.path_len - localization.distance_traveled - missing_distance()) * TWO_DECIMAL_PLACE - getStoppingDistance());
	} else if (should_stop()) {
		toMultiStopping((localization.path_len - localization.distance_traveled) * TWO_DECIMAL_PLACE - getStoppingDistance());
	}
}

void Planning::TrainStatus::multi_stopping_begins() {
	if (localization.last_reserved_node == -1) {
		Task::_KernelCrash("no reserved node, shouldn't be here \r\n");
	}
	debug_print(addr.term_trans_tid, "train %d in multi_stopping %d!\r\n", my_id, localization.last_reserved_node);
	pipe_tr(localization.stopping_speed);
}

void Planning::TrainStatus::handle_train_multi_stopping(int sensor_index) {
	clear_traveled_sensor(sensor_index, true);
	if (sensor_index == localization.last_reserved_node) {
		toSpeed(SpeedLevel::SPEED_STOP);
		pipe_tr();
		path_end_relocate();
	}
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
	int64_t t = current_time - cali_state.last_trigger; // 10 ms
	int64_t d = localization.distance_traveled;			// in MM
	int64_t v_2 = cali_state.target_velocity;
	int64_t v_1 = cali_state.prev_velocity;
	int64_t v_a = (v_2 + v_1) / 2; // average velocity in NM / seconds
	debug_print(
		addr.term_trans_tid, "acceleration case: d %ld, t %ld, d*t %ld, v_a %ld \r\n", d, t, (d * TWO_DECIMAL_PLACE * TWO_DECIMAL_PLACE) / t, v_a);
	if ((d * TWO_DECIMAL_PLACE * TWO_DECIMAL_PLACE) / t > v_a) {
		// convert d from mm to nm, then - nm / seconds * seconds
		int64_t t_1 = (d * TWO_DECIMAL_PLACE - (v_2 * t / TWO_DECIMAL_PLACE)) * TWO_DECIMAL_PLACE / (v_a - v_2);
		int64_t acceleration = (v_2 - v_1) * TWO_DECIMAL_PLACE / t_1; // so it is MM / ticks
		accelerations[cali_state.prev_speed][cali_state.target_speed] = acceleration;
		debug_print(addr.term_trans_tid, "debug: t %ld, d %ld, v_2 %ld, v_1 %ld, v_a %ld\r\n", t, d, v_2, v_1, v_a);
		debug_print(addr.term_trans_tid, "acceleration calibration result case 1: acceleration: %ld, t_1 %ld\r\n", acceleration, t_1);
	} else {
		int64_t v_s = (d * TWO_DECIMAL_PLACE * TWO_DECIMAL_PLACE) / t;
		int64_t v_r = v_s + (v_s - v_1);
		int64_t acceleration = (v_r - v_1) * TWO_DECIMAL_PLACE / t;
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
	int mul = TWO_DECIMAL_PLACE * TWO_DECIMAL_PLACE;
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
	int64_t slow_travled_time = Clock::Time(addr.clock_tid) - cali_state.last_trigger;
	int64_t slow_travled_distance = cali_state.slow_calibration_mm * slow_travled_time / TWO_DECIMAL_PLACE;
	clear_traveled_sensor(sensor_index);
	int64_t real_stop_distance = localization.distance_traveled * TWO_DECIMAL_PLACE - slow_travled_distance;
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
	int64_t v_a = cali_state.target_velocity / 2;
	int64_t t = real_stop_distance * TWO_DECIMAL_PLACE / v_a; // in ticks
	int64_t acceleration = -cali_state.target_velocity * TWO_DECIMAL_PLACE / t;

	speeds[localization.from][cali_state.target_speed].stopping_distance = real_stop_distance < 0 ? 0 : real_stop_distance;
	accelerations[cali_state.target_speed][SpeedLevel::SPEED_STOP] = acceleration;
	toIdle();
	debug_print(addr.term_trans_tid, "real stop distance for train %d is %ld, thus, deceleration %ld\r\n", my_id, real_stop_distance, acceleration);
}

bool TrainStatus::should_subscribe() {
	return localization.state != TrainState::IDLE && localization.state != TrainState::MULTI_WAITING
		   && localization.state != TrainState::MULTI_RELOCATE && localization.state != TrainState::REVERSING;
}

void TrainStatus::manual_subscription() {
	if (should_subscribe()) {
		simple_pre_compute_path(); // sub to te next sensor
	}
}

void TrainStatus::handle_train_locate(int sensor_index) {
	/**
	 * if you are locating and you hit a sensor you stop immediately and you set your last_node = desire node
	 */

	toIdle();
	localization.last_node = &track[sensor_index];
	// Track::TrackServerReq reservation_request;
	Track::TrackServerReq req_to_track;
	req_to_track.body.reservation.len = 1;
	req_to_track.body.reservation.path[0] = sensor_index;
	if (!try_reserve(&req_to_track)) {
		debug_print(addr.term_trans_tid, "train %d cannot resereq_to_trackrve sensor %s!\r\n", my_id, localization.last_node->name);
	} else {
		debug_print(addr.term_trans_tid, "train %d is currently at sensor %s\r\n", my_id, localization.last_node->name);
	}
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
	} else if (localization.state == TrainState::MULTI_PATHING) {
		handle_train_multi_pathing(sensor_index);
	} else if (localization.state == TrainState::MULTI_WAITING) {
		handle_train_multi_waiting();
	} else if (localization.state == TrainState::MULTI_BUNNY_HOP) {
		handle_train_bunny_hopping(sensor_index);
	} else if (localization.state == TrainState::MULTI_STOPPING) {
		handle_train_multi_stopping(sensor_index);
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

void TrainStatus::update_switch_state() {
	Track::TrackServerReq req_to_track;
	req_to_track.header = RequestHeader::TRACK_GET_SWITCH_STATE;
	Send::Send(addr.track_server_tid, (const char*)&req_to_track, sizeof(req_to_track), switch_state, sizeof(switch_state));
}

void TrainStatus::continuous_localization(int sensor_index) {
	if (localization.state == TrainState::LOCATE) {
		return; // during locate, we don't want to continuously localize, cause we are reinitializing
	}

	// last node is guarenteed to be a sensor nod
	localization.last_node = &track[sensor_index]; // this is your current location
	track_node* next_node = localization.last_node->edge[DIR_AHEAD].dest;
	int64_t cont_local_look_ahead_mm = localization.last_node->edge[DIR_AHEAD].dist;
	update_switch_state();
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
			if (should_subscribe()) {
				sub_to_sensor_no_delete(next_node->num);
			}
			break;
		} else if (next_node->type == node_type::NODE_EXIT) {
			break; // break without subscription, in this case there is nothing you can do, no more look ahead possible
		}
	}
}

void TrainStatus::go_rng() {
	if (localization.last_node == nullptr || localization.last_reserved_node == -1 || localization.destinations.full()) {
		debug_print(addr.term_trans_tid, "trying to control train %d with unknown current location\r\n", my_id);
	} else {
		Track::TrackServerReq req_to_track;
		req_to_track.header = RequestHeader::TRACK_RNG;
		req_to_track.body.start_and_end.start = localization.last_reserved_node;

		Track::PathRespond res;
		Send::Send(addr.track_server_tid, (const char*)&req_to_track, sizeof(req_to_track), (char*)&res, sizeof(res));
		localization.destinations.push(res.dest);
		if (localization.state == TrainState::IDLE) {
			schedule_next_multi();
			manual_subscription();
		}
	}
}

void initialize_all_train(TrainStatus* trains,
						  Courier::CourierPool<PlanningCourReq, 8>* couriers,
						  etl::list<etl::pair<int, int>, NUM_TRAINS>* sensors,
						  track_node track[]) {
	for (int i = 0; i < NUM_TRAINS; i++) {
		trains[i].my_id = TRAIN_NUMBERS[i];
		trains[i].my_index = i;
		trains[i].courier_pool = couriers;
		trains[i].sensor_subs = sensors;
		trains[i].addr = getAddressBook();
		trains[i].track = track;
		if (TRAIN_NUMBERS[i] == 24) {
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_1)] = { 16000, 12500 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_1)] = { 16000, 12500 };
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_MAX)] = { 55000, 101500 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0, 16 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_1)] = { 2, 3, 23 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_MAX)] = { 2, 3, 29 };
			trains[i].cali_state.slow_calibration_speed = 4;
			trains[i].cali_state.slow_calibration_mm = 3700; // 37.00 mm / s
			trains[i].localization.stopping_speed = 5;		 // will overshoot, but error is accetable
			trains[i].cali_state.min_stable_dist_margin = -20000;
			// mm / s^2 in 2 decimal places
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_1)] = 7900;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 12220;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 12700;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_STOP)] = -10240;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_1)] = -15419;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_STOP)] = -14901;
		}

		if (TRAIN_NUMBERS[i] == 78) {
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			// 29930, 30390
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_1)] = { 27000, 31500 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_1)] = { 30000, 35824 };
			// 96334 93352 93918
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_MAX)] = { 49000, 94000 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0, 16 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_1)] = { 2, 2, 26 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_MAX)] = { 2, 1, 30 };
			trains[i].cali_state.slow_calibration_speed = 4;
			// 2561, 2561, 2618,
			trains[i].cali_state.slow_calibration_mm = 2575;
			trains[i].localization.stopping_speed = 5; // will overshoot, but error is accetable
			trains[i].cali_state.bunny_ped = -10;
			trains[i].cali_state.min_stable_dist_margin = -20000;

			// mm / s^2 in 2 decimal places
			// 6752, 5448, 4297, 4847, 5452, 5538, 5451, 5369, 5295, 5432, 5606
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_1)] = 5450;
			// 5714, 5667, 5781, 5714
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 5700;
			//-12327
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_STOP)] = -12327;
			// 4627, 4562, 4800, 4846
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 4750;
			// -8707, -9313, -7661, -8407, -8407
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_1)] = -8407;
			// 13527, 12827, 12868 12793
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_STOP)] = -12800;
		}
		if (TRAIN_NUMBERS[i] == 58) { // Train 58, slow af but it seems to be semi-reliable
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			// from low 26864, 28434, 27729
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_1)] = { 25000, 26864 };
			// from up 32216, 33562, 32440
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_1)] = { 27000, 30000 };
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_MAX)] = { 55000, 120000 };

			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0, 16 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_1)] = { 2, 3, 25 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_MAX)] = { 2, 3, 30 };
			trains[i].cali_state.slow_calibration_speed = 4;
			trains[i].cali_state.slow_calibration_mm = 3205;
			trains[i].localization.stopping_speed = 5; // will overshoot, but error is accetable
			trains[i].cali_state.bunny_ped = -40;
			trains[i].cali_state.min_stable_dist_margin = -10000;

			// 7946, 8322, 7924,
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_1)] = 8064;
			// 8704, 9704, 9704
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 9704;
			// 7717, 8159, 8196
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 8024;
			// from low: -11705, -11097, - from up: -9746, -9393 -9653
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_STOP)] = -11705;
			// -11618 - 11618, - 11618
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_1)] = -11618;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_STOP)] = -12604;
		}

		if (TRAIN_NUMBERS[i] == 74) { // Train 58, slow af but it seems to be semi-reliable
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0 };

			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_1)] = { 22300, 36000 };
			trains[i].speeds[FROM_UP][static_cast<int>(SpeedLevel::SPEED_1)] = { 22300, 36000 };
			trains[i].speeds[FROM_DOWN][static_cast<int>(SpeedLevel::SPEED_MAX)] = { 39000, 70000 };

			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_STOP)] = { 0, 0, 16 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_1)] = { 2, 3, 21 };
			trains[i].calibration_info[static_cast<int>(SpeedLevel::SPEED_MAX)] = { 2, 3, 30 };
			trains[i].cali_state.slow_calibration_speed = 2;
			trains[i].cali_state.slow_calibration_mm = 7600;

			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_1)] = 4715;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_STOP)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 7360;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_MAX)] = 24500;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_1)][static_cast<int>(SpeedLevel::SPEED_STOP)] = -6907;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_1)] = -5800;
			trains[i].accelerations[static_cast<int>(SpeedLevel::SPEED_MAX)][static_cast<int>(SpeedLevel::SPEED_STOP)] = -10864;
		}
	}
}

void Planning::global_pathing_server() {
	Name::RegisterAs(GLOBAL_PATHING_SERVER_NAME);
	AddressBook addr = getAddressBook();
	TrainStatus trains[NUM_TRAINS];
	Terminal::GlobalTrainInfo global_info[NUM_TRAINS];

	Courier::CourierPool<PlanningCourReq, 8> courier_pool
		= Courier::CourierPool<PlanningCourReq, 8>(&global_pathing_courier, Priority::HIGH_PRIORITY);
	// first one tells who you are, second one tells you which sensor are you sub to
	// -1 means sub to all sensor, only used during calibration
	// (notice that it means you cannot have multiple trains calibrate at the same time)
	etl::list<etl::pair<int, int>, NUM_TRAINS> sensor_subs;

	track_node track[TRACK_MAX]; // This is guaranteed to be big enough.
	init_tracka(track);			 // default configuration is part a
	initialize_all_train(trains, &courier_pool, &sensor_subs, track);
	// ask to observe the state of the sensor
	PlanningCourReq req_to_unblock = { RequestHeader::GLOBAL_COUR_AWAIT_SENSOR, { 0x0 } };
	courier_pool.request(&req_to_unblock);

	auto sensor_update = [&](char* sensor_state) {
		for (int i = 0; i < Sensor::NUM_SENSOR_BYTES; i++) {
			for (int j = 1; j <= CHAR_BIT; j++) {
				if (sensor_state[i] & (1 << (CHAR_BIT - j))) {
					int sensor_index = i * CHAR_BIT + j - 1;
					for (auto it = sensor_subs.begin(); it != sensor_subs.end(); it++) {
						if (it->second == -1) {
							bool identified = true;
							for (int i = 0; i < Train::NUM_TRAINS; i++) {
								if (i != it->first && trains[i].localization.last_node != nullptr
									&& trains[i].localization.last_node->num == sensor_index) {
									identified = false;
									break;
								}
							}
							if (identified) {
								trains[it->first].sensor_notify(sensor_index);
								it = sensor_subs.erase(it);
								break;
							}
						} else if (it->second == sensor_index) {
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
			char* sensor_state = req.body.sensor_state;
			courier_pool.request(&req_to_unblock);
			// now we unblock each of the sensor if needed.
			sensor_update(sensor_state);
			break;
		}
		case RequestHeader::GLOBAL_RNG: {
			int train_index = Train::train_num_to_index(req.body.routing_request.id);
			trains[train_index].go_rng();
			Reply::EmptyReply(from);
			break;
		}
		case RequestHeader::GLOBAL_COURIER_COMPLETE: {
			courier_pool.receive(from);
			break;
		}
		case RequestHeader::GLOBAL_BUSY_WAITING_AVAILABILITY: {
			courier_pool.receive(from);
			int train_index = Train::train_num_to_index(req.body.command.id);
			trains[train_index].handle_train_multi_waiting();
			trains[train_index].manual_subscription();
			break;
		}
		case RequestHeader::GLOBAL_BUSY_WAITING_BUNNY_HOPPING: {
			courier_pool.receive(from);
			int train_index = Train::train_num_to_index(req.body.command.id);
			trains[train_index].tryBunnyHopping();
			trains[train_index].manual_subscription();
			break;
		}
		case RequestHeader::GLOBAL_BUSY_WAITING_REVERSE: {
			courier_pool.receive(from);
			int train_index = Train::train_num_to_index(req.body.command.id);
			trains[train_index].reverse();
			trains[train_index].manual_subscription();
			break;
		}
		case RequestHeader::GLOBAL_STOPPING_COMPLETE: {
			courier_pool.receive(from);
			int train_index = req.body.stopping_request.id;
			trains[train_index].toIdle();
			break;
		}

		case RequestHeader::GLOBAL_BUNNY_DIST: {
			Reply::EmptyReply(from);
			int train_index = Train::train_num_to_index(req.body.pedding_request.id);
			trains[train_index].cali_state.bunny_ped = req.body.pedding_request.pedding;
			break;
		}

		case RequestHeader::GLOBAL_MULTI_STOPPING_COMPLETE: {
			courier_pool.receive(from);
			int train_index = req.body.stopping_request.id;
			trains[train_index].multi_stopping_begins();
			break;
		}
		case RequestHeader::GLOBAL_BUNNY_HOP_STOP_COMPLETE: {
			courier_pool.receive(from);
			int train_index = req.body.stopping_request.id;
			trains[train_index].bunnyHopStopping();
			break;
		}
		case RequestHeader::GLOBAL_RELOCATE_END_COMPLETE: {
			courier_pool.receive(from);
			int train_index = req.body.stopping_request.id;
			trains[train_index].relocate_complete(req.body.stopping_request.need_reverse);
			break;
		}

		case RequestHeader::GLOBAL_RELOCATE_TRANSITION_COMPLETE: {
			courier_pool.receive(from);
			int train_index = req.body.stopping_request.id;
			trains[train_index].relocate_transition(req.body.stopping_request.need_reverse);
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

			if (trains[train_index].exit_loop(dest, offset)) {
				trains[train_index].subscribe(from);
			} else {
				debug_print(addr.term_trans_tid, "Unreachable destination from loop!\r\n");
				Reply::EmptyReply(from);
			}

			break;
		}
		case RequestHeader::GLOBAL_SET_TRACK: {
			Message::Reply::EmptyReply(from);
			PlanningCourReq req_to_courier;
			req_to_courier.header = RequestHeader::GLOBAL_COUR_INIT_TRACK;
			req_to_courier.body.info = req.body.info;

			if (req.body.info == Track::TRACK_A_ID) {
				init_tracka(track);
			} else if (req.body.info == Track::TRACK_B_ID) {
				init_trackb(track);
			} else {
				Task::_KernelCrash("invalid configuration for tracks\r\n");
			}
			courier_pool.request(&req_to_courier);
			break;
		}
		case RequestHeader::GLOBAL_PATH: {
			int train_index = Train::train_num_to_index(req.body.routing_request.id);
			int dest = req.body.routing_request.dest;
			SpeedLevel speed = req.body.routing_request.speed;

			if (trains[train_index].goTo(dest, speed)) {
				trains[train_index].subscribe(from);
			} else {
				debug_print(addr.term_trans_tid, "unreachable location, refuse to path!\r\n");
				Reply::Reply(from, (const char*)UNABLE_TO_PATH, sizeof(int));
			}
			break;
		}
		case RequestHeader::GLOBAL_MULTI_PATH: {
			int train_index = Train::train_num_to_index(req.body.routing_request.id);
			int dest = req.body.routing_request.dest;
			trains[train_index].multi_path(dest);
			Reply::EmptyReply(from);
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

		case RequestHeader::GLOBAL_OBSERVE: {
			for (int i = 0; i < NUM_TRAINS; ++i) {
				global_info[i].velocity = trains[i].getVelocity();
				global_info[i].next_sensor = trains[i].localization.next_sensor;
				int prev = trains[i].localization.last_node - track;
				if (prev < TOTAL_SENSORS) {
					global_info[i].prev_sensor = prev;
				} else {
					global_info[i].prev_sensor = NO_SENSOR;
				}

				if (!trains[i].localization.path.empty()) {
					global_info[i].path_src = trains[i].localization.path.front();
					global_info[i].path_dest = trains[i].localization.path.back();
				} else {
					global_info[i].path_src = NO_PATH;
					global_info[i].path_dest = NO_PATH;
				}

				long current_time = Clock::Time(addr.clock_tid);
				long tick_since_last_local = current_time - trains[i].localization.time_traveled;
				global_info[i].time_to_next_sensor = trains[i].localization.expected_arrival_ticks[0] - tick_since_last_local;
				global_info[i].dist_to_next_sensor
					= global_info[i].time_to_next_sensor * trains[i].localization.eventual_velocity / TWO_DECIMAL_PLACE;
			}

			Reply::Reply(from, reinterpret_cast<char*>(global_info), sizeof(global_info));
			break;
		}

		default: {
			Task::_KernelCrash("Received invalid request: %d at global pathing from %d \r\n", req.header, from);
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
	Track::TrackServerReq req_to_track = {};

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
		case RequestHeader::GLOBAL_COUR_BUSY_WAITING_AVAILABILITY: {
			req_to_admin.header = RequestHeader::GLOBAL_BUSY_WAITING_AVAILABILITY;
			req_to_admin.body.command = req.body.command;
			Clock::Delay(addr.clock_tid, 300);
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		case RequestHeader::GLOBAL_COUR_BUSY_WAITING_BUNNY_HOPPING: {
			req_to_admin.header = RequestHeader::GLOBAL_BUSY_WAITING_BUNNY_HOPPING;
			req_to_admin.body.command = req.body.command;
			Clock::Delay(addr.clock_tid, 300);
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		case RequestHeader::GLOBAL_COUR_BUSY_WAITING_REVERSE: {
			req_to_admin.header = RequestHeader::GLOBAL_BUSY_WAITING_REVERSE;
			req_to_admin.body.command = req.body.command;
			Clock::Delay(addr.clock_tid, 300);
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
		case RequestHeader::GLOBAL_COUR_REV: {
			req_to_admin = { RequestHeader::GLOBAL_COURIER_COMPLETE, RequestBody { 0x0 } };

			req_to_train.header = RequestHeader::TRAIN_REV;
			req_to_train.body.command.id = req.body.command.id;
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
		case RequestHeader::GLOBAL_COUR_RELOCATE_END: {
			if (req.body.stopping_request.delay > 0) {
				Clock::Delay(addr.clock_tid, req.body.stopping_request.delay);
			}
			req_to_admin = { RequestHeader::GLOBAL_RELOCATE_END_COMPLETE, RequestBody { 0x0 } };
			req_to_admin.body.stopping_request.id = req.body.stopping_request.id;
			req_to_admin.body.stopping_request.need_reverse = req.body.stopping_request.need_reverse;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}

		case RequestHeader::GLOBAL_COUR_RELOCATE_TRANSITION: {
			Clock::Delay(addr.clock_tid, 100);
			req_to_admin = { RequestHeader::GLOBAL_RELOCATE_TRANSITION_COMPLETE, RequestBody { 0x0 } };
			req_to_admin.body.stopping_request.id = req.body.stopping_request.id;
			req_to_admin.body.stopping_request.need_reverse = req.body.stopping_request.need_reverse;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		case RequestHeader::GLOBAL_COUR_MULTI_STOPPING: {
			if (req.body.stopping_request.delay > 0) {
				Clock::Delay(addr.clock_tid, req.body.stopping_request.delay);
			}
			req_to_admin = { RequestHeader::GLOBAL_MULTI_STOPPING_COMPLETE, RequestBody { 0x0 } };
			req_to_admin.body.stopping_request.id = req.body.stopping_request.id;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		case RequestHeader::GLOBAL_COUR_MULTI_STOPPING_END: {
			if (req.body.stopping_request.delay > 0) {
				Clock::Delay(addr.clock_tid, req.body.stopping_request.delay);
			}
			req_to_admin = { RequestHeader::GLOBAL_MULTI_STOPPING_END, RequestBody { 0x0 } };
			req_to_admin.body.stopping_request.id = req.body.stopping_request.id;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		case RequestHeader::GLOBAL_COUR_BUNNY_HOP_STOP: {
			if (req.body.stopping_request.delay > 0) {
				Clock::Delay(addr.clock_tid, req.body.stopping_request.delay);
			}
			req_to_admin = { RequestHeader::GLOBAL_BUNNY_HOP_STOP_COMPLETE, RequestBody { 0x0 } };
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

		case RequestHeader::GLOBAL_COUR_INIT_TRACK: {
			req_to_track.header = RequestHeader::TRACK_INIT;
			req_to_track.body.info = req.body.info;
			req_to_admin = { RequestHeader::GLOBAL_COURIER_COMPLETE, RequestBody { 0x0 } };
			Send::SendNoReply(addr.track_server_tid, reinterpret_cast<char*>(&req_to_track), sizeof(req_to_track));
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		default:
			Task::_KernelCrash("GP_Train Courier illegal type: [%d]\r\n", req.header);
		}
	}
}
