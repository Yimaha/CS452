#include "train_admin.h"
#include "courier_pool.h"

using namespace Train;
using namespace Message;
using namespace Sensor;

static const char extra_switch[] = { 0x99, 0x9A, 0x9B, 0x9C };
int Train::get_switch_id(int id) {
	if (1 <= id && id <= 18) {
		return id - 1;
	} else if (153 <= id && id <= 156) {
		return id - 153 + 18;
	} else {
		return NO_SWITCH;
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

struct SwitchDelayMessage {
	int from;
	char track_id;
	char dir;
};

void Train::train_admin() {
	Name::RegisterAs(TRAIN_SERVER_NAME);
	const uint64_t POOL_SIZE = 64;
	Courier::CourierPool<TrainCourierReq> courier_pool = Courier::CourierPool<TrainCourierReq>(&train_courier, Priority::HIGH_PRIORITY);
	AddressBook addr = getAddressBook();
	int uart_tid = addr.train_trans_tid;

	etl::queue<Command, 128> train_command_queue;
	etl::queue<Command, 128> track_command_queue;

	char command[2];
	int from;
	TrainAdminReq req;
	TrainRaw trains[NUM_TRAINS];

	etl::queue<SwitchDelayMessage, POOL_SIZE> switch_queue;
	TrainCourierReq req_to_courier;
	req_to_courier.header = RequestHeader::TRAIN_COUR_SENSOR_START;
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
			if (req.body.command.action < 16) {
				req.body.command.action += 16;
			}
			char desire_speed = req.body.command.action; // should be an integer within 0 - 31
			int train_index = train_num_to_index(train_id);
			trains[train_index].speed = desire_speed;
			train_command_queue.push(req.body.command);
			break;
		}
		case RequestHeader::TRAIN_REV: {
			// raw call means train server is not responsible for timing.
			Message::Reply::EmptyReply(from); // unblock after job is done
			char train_id = req.body.command.id;
			int train_index = train_num_to_index(train_id);
			trains[train_index].direction = !trains[train_index].direction;
			req.body.command.action = REV_COMMAND;
			train_command_queue.push(req.body.command);

			break;
		}
		case RequestHeader::TRAIN_SWITCH: {
			Message::Reply::EmptyReply(from);

			char track_id = req.body.command.id;
			if (switch_queue.empty()) {
				track_command_queue.push(req.body.command);
				req_to_courier.header = RequestHeader::TRAIN_COUR_SWITCH_DELAY;
				courier_pool.request(&req_to_courier);
			}
			switch_queue.push({ from, track_id, req.body.command.action });
			break;
		}
		case RequestHeader::TRAIN_SWITCH_DELAY_COMPLETE: {
			// unblock and place the courier back into the pool
			courier_pool.receive(from);

			if (switch_queue.empty()) {
				Task::_KernelCrash("Train Admin: Switch queue is empty");
			}

			SwitchDelayMessage info = switch_queue.front();
			switch_queue.pop();
			// if there is another swtich request queued up
			if (!switch_queue.empty()) {
				info = switch_queue.front();
				track_command_queue.push(Command { info.track_id, info.dir });
				req_to_courier.header = RequestHeader::TRAIN_COUR_SWITCH_DELAY;
				courier_pool.request(&req_to_courier);
			} else {
				track_command_queue.push(Command { 0, 0 });
			}
			break;
		}
		case RequestHeader::TRAIN_SENSOR_READING_COMPLETE: {
			// unblock and place the courier back into the pool
			courier_pool.receive(from);
			if (!train_command_queue.empty()) {
				command[0] = train_command_queue.front().action;
				command[1] = train_command_queue.front().id;
				UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
				train_command_queue.pop();
			} else if (!track_command_queue.empty()) {
				Command info = track_command_queue.front();
				if (info.id == 0) // since there is no swtich number 0, this means clear switch
				{
					get_clear_track_byte(command);
					UART::Putc(uart_tid, TRAIN_UART_CHANNEL, command[0]);
				} else {
					bool s = info.action == 's'; // if true, then straight, else, curved
					if (s) {
						get_straight_byte(command, info.id);
					} else {
						get_curved_byte(command, info.id);
					}
					UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
				}
				track_command_queue.pop();
			}
			req_to_courier.header = RequestHeader::TRAIN_COUR_SENSOR_START;
			courier_pool.request(&req_to_courier);
			break;
		}
		case RequestHeader::TRAIN_COURIER_COMPLETE: {
			courier_pool.receive(from);
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
	SensorAdminReq req_to_sensor;

	// worker only has few types
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainCourierReq));
		Message::Reply::EmptyReply(from); // unblock caller right away
		switch (req.header) {
		case RequestHeader::TRAIN_COUR_SWITCH_DELAY: {
			Clock::DelayUntil(addr.clock_tid, 15);
			req_to_admin = { RequestHeader::TRAIN_SWITCH_DELAY_COMPLETE, RequestBody { 0x0, 0x0 } };
			Message::Send::SendNoReply(addr.train_admin_tid, (const char*)&req_to_admin, sizeof(TrainAdminReq));
			break;
		}
		case RequestHeader::TRAIN_COUR_SENSOR_START: {
			req_to_sensor.header = Message::RequestHeader::SENSOR_START_UPDATE;
			Message::Send::SendNoReply(addr.sensor_admin_tid, (const char*)&req_to_sensor, sizeof(SensorAdminReq));
			req_to_admin = { RequestHeader::TRAIN_SENSOR_READING_COMPLETE, RequestBody { 0x0, 0x0 } };
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
