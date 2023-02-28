#include "train_admin.h"
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

struct SwitchDelayMessage {
	int from;
	int track_id;
	bool straight;
};

void Train::train_admin() {
	Name::RegisterAs(TRAIN_SERVER_NAME);

	const int POOL_SIZE = 30;
	etl::queue<int, POOL_SIZE> courier_pool;
	for (int i = 0; i < POOL_SIZE; i++) {
		courier_pool.push(Task::Create(2, &train_courier));
	}
	int uart_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);

	const int POOL_SIZE = 32;
	etl::queue<int, POOL_SIZE> courier_pool;
	for (int i = 0; i < POOL_SIZE; i++) {
		courier_pool.push(Task::Create(2, &train_courier));
	}

	etl::queue<SwitchDelayMessage, POOL_SIZE> switch_queue;

	int uart_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);
	int print_tid = Name::WhoIs(UART::UART_0_TRANSMITTER);
	char command[2];
	int from;
	TrainAdminReq req;

	// note, similar to UART::Putc, train server does not guarentee that command is fired right away
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainAdminReq));
		switch (req.header) {
		case RequestHeader::SPEED: {
			char train_id = req.body.id;
			char desire_speed = req.body.action; // should be an integer within 0 - 31
			command[0] = desire_speed;
			command[1] = train_id;
			UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			Message::Reply::Reply(from, nullptr, 0); // unblock after job is done
			break;
		}
		case RequestHeader::REV: {
			// note if action is passed, it speed is the desire final speed of the train
			char train_id = req.body.id;
			command[0] = 0;
			command[1] = train_id;
			UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			TrainCourierReq req_to_courier = { CourierRequestHeader::REV, { train_id, req.body.action } };
			Message::Send::Send(courier_pool.front(), (const char*)&req_to_courier, sizeof(TrainCourierReq), nullptr, 0);
			courier_pool.pop();
			Message::Reply::Reply(from, nullptr, 0); // unblock after job is done
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
				Message::Send::Send(courier_pool.front(), (const char*)&req_to_courier, sizeof(TrainCourierReq), nullptr, 0);
				courier_pool.pop();
			}
			switch_queue.push({ from, track_id, s });
			break;
		}
		case RequestHeader::SWITCH_DELAY_COMPLETE: {
			// unblock and place the courier back into the pool
			Message::Reply::Reply(from, nullptr, 0);
			courier_pool.push(from);

			if (switch_queue.empty()) {
				Task::_KernelPrint("switch queue is empty");
				while (1) {
				}
			}

			SwitchDelayMessage info = switch_queue.front();
			Message::Reply::Reply(info.from, nullptr, 0);
			switch_queue.pop();

			if (!switch_queue.empty()) {
				info = switch_queue.front();
				command[0] = info.straight;
				command[1] = info.track_id;
				UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
				TrainCourierReq req_to_courier = { CourierRequestHeader::SWITCH_DELAY, { 0x0, 0x0 } };
				Message::Send::Send(courier_pool.front(), (const char*)&req_to_courier, sizeof(TrainCourierReq), nullptr, 0);
				courier_pool.pop();
			} else {
				get_clear_track_byte(command);
				UART::Putc(uart_tid, TRAIN_UART_CHANNEL, command[0]);
				// no need to send any more message
			}
			break;
		}
		case RequestHeader::COURIER_COMPLETE: {
			Message::Reply::Reply(from, nullptr, 0);
			courier_pool.push(from);
			break;
		}
		default: {
			char exception[30];
			sprintf(exception, "illegal type: [%d]\r\n", req.header);
			Task::_KernelPrint(exception);
			while (1) {
			}
		}
		}
	}
}

void Train::train_courier() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int train_admin_tid = Name::WhoIs(TRAIN_SERVER_NAME);
	int uart_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);
	int from;
	TrainCourierReq req;
	TrainAdminReq req_to_admin;
	char command[2];

	// worker only has few types
	while (1) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainCourierReq));
		Message::Reply::Reply(from, nullptr, 0); // unblock caller right away
		switch (req.header) {
		case CourierRequestHeader::REV: {
			// it wait for about 4 seconds then send in the command to reverse and speed up
			char train_id = req.body.id;
			char desire_speed = req.body.action; // should be an integer within 0 - 31
			Clock::Delay(clock_tid, 400);
			command[0] = REV_COMMAND;
			command[1] = train_id;
			UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			command[0] = desire_speed;
			Clock::Delay(clock_tid, 1);
			UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
			req_to_admin = { RequestHeader::COURIER_COMPLETE, RequestBody { 0x0, 0x0 } };

			Message::Send::Send(train_admin_tid, (const char*)&req_to_admin, sizeof(TrainAdminReq), nullptr, 0);
			break;
		}
		case CourierRequestHeader::SWITCH_DELAY: {
			Clock::Delay(clock_tid, 15); // delay by 150 ms
			// get_clear_track_byte(command);
			// UART::Putc(uart_tid, TRAIN_UART_CHANNEL, command[0]);
			req_to_admin = { RequestHeader::SWITCH_DELAY_COMPLETE, RequestBody { 0x0, 0x0 } };

			Message::Send::Send(train_admin_tid, (const char*)&req_to_admin, sizeof(TrainAdminReq), nullptr, 0);
			break;
		}
		default: {
			char exception[30];
			sprintf(exception, "illegal type: [%d]\r\n", req.header);
			Task::_KernelPrint(exception);
			while (1) {
			}
		}
		}
	}
}