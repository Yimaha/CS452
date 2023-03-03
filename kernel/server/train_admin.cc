#include "train_admin.h"
#include "courier_pool.h"

using namespace Train;

static const char extra_switch[] = { 0x99, 0x9A, 0x9B, 0x9C };
int get_switch_id(int id) {
	if (1 <= id && id <= 18) {
		return id;
	} else if (153 <= id && id <= 156) {
		return id - 153 + 19;
	} else {
		return 0;
	}
}

void get_straight_byte(char code[], int track_id) {
	code[0] = '\0' + 33;
	code[1] = '\0' + track_id;
}

void get_curved_byte(char code[], int track_id) {
	code[0] = '\0' + 34;
	code[1] = '\0' + track_id;
}

void get_clear_track_byte(char code[]) {
	code[0] = '\0' + 32;
}

int train_num_to_index(int train_num) {
	for (int i = 0; i < Train::NUM_TRAINS; i++) {
		if (Train::TRAIN_NUMBERS[i] == train_num) {
			return i;
		}
	}
	return -1;
}

bool Train::TrainStatus::setSpeed(int s) {
	command({ State::FREE, s });
	return (state == State::FREE);
}

int Train::TrainStatus::getSpeed() {
	return speed;
}

bool Train::TrainStatus::revStart() {
	return command({ State::REVERSING, 0 });
}

// return -1 if need reverse again
// return speed if no need
bool Train::TrainStatus::revClear() {
	kernel_assert(state == State::REVERSING);
	state = State::FREE;

	if (direction != desire_direction) {
		return false;
	} else {
		return true;
	}
}

bool TrainStatus::command(Command comm) {
	switch (comm.state) {
	case State::REVERSING: {
		if (state == State::REVERSING) {
			desire_direction = !desire_direction;
			return false; // meaning could not fire right away
		} else {
			state = State::REVERSING;
			direction = !direction;
			desire_direction = direction;
			return true;
		}
	}
	case State::FREE: {
		speed = comm.action;
		return true;
	};
	}
	return true;
}

struct SwitchDelayMessage {
	int from;
	int track_id;
	bool straight;
};

void Train::train_admin() {
	Name::RegisterAs(TRAIN_SERVER_NAME);
	const uint64_t POOL_SIZE = 16;
	Courier::CourierPool<TrainCourierReq> courier_pool = Courier::CourierPool<TrainCourierReq>(&train_courier, 2);
	int uart_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);
	char command[2];
	int from;
	TrainAdminReq req;
	TrainStatus trains[NUM_TRAINS];
	char switch_state[NUM_SWITCHES] = { 'c' };
	etl::queue<SwitchDelayMessage, POOL_SIZE> switch_queue;

	// once we pushed all jobs, initialize a job that just setup all the switches

	/**
	 * Since the server behave differently upon command type, we should elaborate
	 *
	 * SPEED command is the only command that immediately reply the caller instead of blocking it, as it simply change the speed of the train
	 * if the train is under reverse at the moment, then the speed of the train after train awake is set to the desire speed. Assuming no new call overwrite your call
	 *
	 * REV command blocks the caller until the rev is completed. multiple rev command can be sent to the server, and the train keep track of the goal reverse diretion
	 * until it is achieved. For example, if you have 4 workers sending reverse, the goal state is actually, no reverse! this means we need to call reverse again after the initial reverse came back
	 * All the caller of reverse will be blocked until the reverse delay is completed.
	 *
	 * At the moment as of now, all SWITCH command are seperated by 150 ms apart, regardless of ID, the caller is blocked until it's desire swtich is turned.
	 */
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainAdminReq));
		switch (req.header) {
		case RequestHeader::SPEED: {
			Message::Reply::Reply(from, nullptr, 0); // unblock after job is done
			char train_id = req.body.id;
			char desire_speed = req.body.action; // should be an integer within 0 - 31
			int train_index = train_num_to_index(train_id);
			bool is_free = trains[train_index].setSpeed(desire_speed);
			if (is_free) {
				command[0] = desire_speed;
				command[1] = train_id;
				UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			}
			break;
		}
		case RequestHeader::REV: {
			char train_id = req.body.id;
			command[0] = 0;
			command[1] = train_id;
			UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			int train_index = train_num_to_index(train_id);
			bool revStart = trains[train_index].revStart();
			trains[train_index].rev_subscribers.push(from);
			if (revStart) {
				TrainCourierReq req_to_courier = { CourierRequestHeader::REV_DELAY, { train_id, req.body.action } };
				courier_pool.request(&req_to_courier, sizeof(req_to_courier));
			}
			break;
		}
		case RequestHeader::SWITCH: {
			char track_id = req.body.id;
			bool s = req.body.action == 's'; // if true, then straight, else, curved
			if (s) {
				get_straight_byte(command, track_id);
			} else {
				get_curved_byte(command, track_id);
			}

			if (switch_queue.empty()) {
				UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
				TrainCourierReq req_to_courier = { CourierRequestHeader::SWITCH_DELAY, { 0x0, 0x0 } };
				courier_pool.request(&req_to_courier, sizeof(TrainCourierReq));
			}
			switch_queue.push({ from, track_id, s });
			break;
		}
		case RequestHeader::SWITCH_DELAY_COMPLETE: {
			// unblock and place the courier back into the pool
			courier_pool.receive(from);

			if (switch_queue.empty()) {
				Task::_KernelCrash("switch queue is empty");
			}

			SwitchDelayMessage info = switch_queue.front();
			if (info.from > 0) {
				Message::Reply::Reply(info.from, nullptr, 0);
			}
			switch_queue.pop();

			if (!switch_queue.empty()) {
				info = switch_queue.front();
				command[0] = info.straight;
				command[1] = info.track_id;
				UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
				TrainCourierReq req_to_courier = { CourierRequestHeader::SWITCH_DELAY, { 0x0, 0x0 } };
				courier_pool.request(&req_to_courier, sizeof(TrainCourierReq));
			} else {
				get_clear_track_byte(command);
				UART::Putc(uart_tid, TRAIN_UART_CHANNEL, command[0]);
				// no need to send any more message
			}
			break;
		}
		case RequestHeader::DELAY_REV_COMPLETE: {
			courier_pool.receive(from);
			int train_index = train_num_to_index(req.body.id);
			bool reverse = trains[train_index].revClear();
			if (reverse) {
				// need to reverse again
				command[0] = REV_COMMAND;
				command[1] = req.body.id;
				UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			}
			// if we completed reverse, then simply just release control
			command[0] = trains[train_index].getSpeed();
			command[1] = req.body.id;
			UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			// also free anyone who is subscribed to the reverse command
			while (!trains[train_index].rev_subscribers.empty()) {
				Message::Reply::Reply(trains[train_index].rev_subscribers.front(), nullptr, 0);
				trains[train_index].rev_subscribers.pop();
			}
			break;
		}
		case RequestHeader::COURIER_COMPLETE: {
			// default fall through if you don't desire the any job to be done at courier end
			courier_pool.receive(from);
			break;
		}
		default: {
			char exception[50];
			sprintf(exception, "Train Admin illegal type: [%d]\r\n", req.header);
			Task::_KernelCrash(exception);
		}
		}
	}
}

void Train::train_courier() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int train_admin_tid = Name::WhoIs(TRAIN_SERVER_NAME);
	int from;
	TrainCourierReq req;
	TrainAdminReq req_to_admin;

	// worker only has few types
	while (1) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainCourierReq));
		Message::Reply::Reply(from, nullptr, 0); // unblock caller right away
		switch (req.header) {
		case CourierRequestHeader::REV_DELAY: {
			// it wait for about 4 seconds then send in the command to reverse and speed up
			char train_id = req.body.id;
			Clock::Delay(clock_tid, 400);
			req_to_admin = { RequestHeader::DELAY_REV_COMPLETE, RequestBody { train_id, 0x0 } };
			Message::Send::Send(train_admin_tid, (const char*)&req_to_admin, sizeof(TrainAdminReq), nullptr, 0);
			break;
		}
		case CourierRequestHeader::SWITCH_DELAY: {
			Clock::Delay(clock_tid, 15); // delay by 150 ms
			req_to_admin = { RequestHeader::SWITCH_DELAY_COMPLETE, RequestBody { 0x0, 0x0 } };
			Message::Send::Send(train_admin_tid, (const char*)&req_to_admin, sizeof(TrainAdminReq), nullptr, 0);
			break;
		}
		default: {
			char exception[50];
			sprintf(exception, "Train Courier illegal type: [%d]\r\n", req.header);
			Task::_KernelCrash(exception);
		}
		}
	}
}
