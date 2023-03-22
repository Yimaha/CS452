#include "train_admin.h"
#include "courier_pool.h"

using namespace Train;
using namespace Message;

static const char extra_switch[] = { 0x99, 0x9A, 0x9B, 0x9C };
int Train::get_switch_id(int id) {
	if (1 <= id && id <= 18) {
		return id - 1;
	} else if (153 <= id && id <= 156) {
		return id - 153 + 18;
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

int Train::train_num_to_index(int train_num) {
	for (int i = 0; i < Train::NUM_TRAINS; i++) {
		if (Train::TRAIN_NUMBERS[i] == train_num) {
			return i;
		}
	}

	return NO_TRAIN;
}

struct SwitchDelayMessage {
	int from;
	int track_id;
	bool straight;
};

void Train::train_admin() {
	Name::RegisterAs(TRAIN_SERVER_NAME);
	const uint64_t POOL_SIZE = 64;
	Courier::CourierPool<TrainCourierReq> courier_pool = Courier::CourierPool<TrainCourierReq>(&train_courier, Priority::HIGH_PRIORITY);
	AddressBook addr = getAddressBook();
	int uart_tid = addr.train_trans_tid;

	char command[2];
	int from;
	TrainAdminReq req;
	TrainRaw trains[NUM_TRAINS];

	char switch_state[NUM_SWITCHES] = { 'c' };
	etl::queue<SwitchDelayMessage, POOL_SIZE> switch_queue;
	etl::queue<int, POOL_SIZE> switch_subscriber;

	// auto resync = [&]() { Send::SendEmptyReply(sensor_admin, (const char*)&req_to_sensor, sizeof(req_to_sensor));
	// };

	for (int i = 1; i <= 18; i++) {
		switch_queue.push({ -1, i, false });
	}

	for (int i = 153; i <= 156; i++) {
		switch_queue.push({ -1, i, false });
	}

	get_curved_byte(command, 1);
	UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
	TrainCourierReq req_to_courier = { RequestHeader::TRAIN_COUR_SWITCH_DELAY, { 0x0, 0x0 } };
	req_to_courier.body.next_delay = Clock::Time(addr.clock_tid) + Sensor::SENSOR_DELAY; // timing is not relevant at startup
	courier_pool.request(&req_to_courier);

	// once we pushed all jobs, initialize a job that just setup all the switches

	/**
	 * Train server is downgrading to a command firing server, who is responsible to double check and see if timing is
	 * correct It keep track of the raw state of train as well, but that is about it.
	 */
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainAdminReq));
		switch (req.header) {
		case RequestHeader::TRAIN_SPEED: {
			// raw call means train server is not responsible for timing.
			Message::Reply::EmptyReply(from); // unblock after job is done
			char train_id = req.body.command.id;
			char desire_speed = req.body.command.action; // should be an integer within 0 - 31
			int train_index = train_num_to_index(train_id);
			trains[train_index].speed = desire_speed;
			command[0] = desire_speed;
			command[1] = train_id;
			UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			break;
		}
		case RequestHeader::TRAIN_REV: {
			// raw call means train server is not responsible for timing.
			Message::Reply::EmptyReply(from); // unblock after job is done
			char train_id = req.body.command.id;
			int train_index = train_num_to_index(train_id);
			trains[train_index].direction = !trains[train_index].direction;
			command[0] = REV_COMMAND;
			command[1] = train_id;
			UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			break;
		}
		case RequestHeader::TRAIN_SWITCH: {
			char track_id = req.body.command.id;
			if (get_switch_id(track_id) == req.body.command.action) {
				// no need to do anything
				Message::Reply::EmptyReply(from);
			} else {
				bool s = req.body.command.action == 's'; // if true, then straight, else, curved
				if (s) {
					get_straight_byte(command, track_id);
				} else {
					get_curved_byte(command, track_id);
				}
				if (switch_queue.empty()) {
					UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
					switch_state[get_switch_id(track_id)] = req.body.command.action;
					courier_pool.request(&req_to_courier);
				}
				switch_queue.push({ from, track_id, s });
			}

			break;
		}
		case RequestHeader::TRAIN_SWITCH_DELAY_COMPLETE: {
			// unblock and place the courier back into the pool
			courier_pool.receive(from);

			if (switch_queue.empty()) {
				Task::_KernelCrash("Train Admin: Switch queue is empty");
			}

			SwitchDelayMessage info = switch_queue.front();
			if (info.from > 0) {
				Message::Reply::EmptyReply(info.from);
			}
			switch_queue.pop();
			// if there is another swtich request queued up
			if (!switch_queue.empty()) {
				info = switch_queue.front();
				if (info.straight) {
					get_straight_byte(command, info.track_id);
				} else {
					get_curved_byte(command, info.track_id);
				}
				UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
				switch_state[get_switch_id(info.track_id)] = info.straight ? 's' : 'c';
				courier_pool.request(&req_to_courier);
			} else {
				get_clear_track_byte(command);
				UART::Putc(uart_tid, TRAIN_UART_CHANNEL, command[0]);
				while (!switch_subscriber.empty()) {
					Message::Reply::Reply(switch_subscriber.front(), switch_state, sizeof(switch_state));
					switch_subscriber.pop();
				}
			}
			break;
		}
		case RequestHeader::TRAIN_COURIER_COMPLETE: {
			// default fall through if you don't desire the any job to be done at courier end
			courier_pool.receive(from);
			break;
		}
		case RequestHeader::TRAIN_SWITCH_OBSERVE: {
			switch_subscriber.push(from);
			break;
		}
		case RequestHeader::TRAIN_OBSERVE: {
			Message::Reply::Reply(from, reinterpret_cast<char*>(trains), sizeof(trains));
			break;
		}
		default: {
			Task::_KernelCrash("Train Admin illegal type: [%d]\r\n", req.header);
		}
		}
	}
}

void Train::train_courier() {
	AddressBook addr = getAddressBook();

	int from;
	TrainCourierReq req;
	TrainAdminReq req_to_admin;

	// worker only has few types
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainCourierReq));
		Message::Reply::EmptyReply(from); // unblock caller right away
		switch (req.header) {
		case RequestHeader::TRAIN_COUR_SWITCH_DELAY: {
			Clock::DelayUntil(addr.clock_tid, Sensor::SENSOR_DELAY * 2);
			// delay by 2 ticks of sensor intervel, so block until + one following it

			req_to_admin = { RequestHeader::TRAIN_SWITCH_DELAY_COMPLETE, RequestBody { 0x0, 0x0 } };
			Message::Send::SendNoReply(addr.train_admin_tid, (const char*)&req_to_admin, sizeof(TrainAdminReq));
			break;
		}

		default:
			Task::_KernelCrash("Train_A Train Courier illegal type: [%d]\r\n", req.header);
		}
	}
}

bool operator==(const Train::TrainRaw& lhs, const Train::TrainRaw& rhs) {
	return lhs.speed == rhs.speed && lhs.direction == rhs.direction;
}

bool operator!=(const Train::TrainRaw& lhs,
				const /* data */
				Train::TrainRaw& rhs) {
	return !(lhs == rhs);
}
