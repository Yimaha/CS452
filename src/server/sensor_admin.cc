#include "sensor_admin.h"
#include "courier_pool.h"
using namespace Message;

void Sensor::sensor_admin() {
	Name::RegisterAs(SENSOR_ADMIN_NAME);
	AddressBook addr = getAddressBook();

	// sensor subscribers
	etl::queue<int, SENSOR_ADMIN_NUM_SUBSCRIBERS> subscribers;

	// sensor couriers
	int courier = Task::Create(Priority::HIGH_PRIORITY, &sensor_courier);
	// sensor requests
	int from;
	SensorAdminReq req;
	char sensor_state[NUM_SENSOR_BYTES] = { 0b10101010 };
	if (UART::Putc(addr.train_trans_tid, 1, 0xc0) == -1) {
		Task::_KernelCrash("Failed to enable sensor\r\n");
	}
	SensorCourierReq req_to_courier = { Message::RequestHeader::SENSOR_COUR_AWAIT_READING, { 0x0 } };
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorAdminReq));
		switch (req.header) {
		case Message::RequestHeader::SENSOR_UPDATE: {
			Message::Reply::EmptyReply(from); // after copying
			for (int i = 0; i < NUM_SENSOR_BYTES; i++) {
				sensor_state[i] = req.body.sensor_state[i];
			}
			while (!subscribers.empty()) {
				// regular subscriber gets information on current sensor state
				Message::Reply::Reply(subscribers.front(), sensor_state, NUM_SENSOR_BYTES);
				subscribers.pop();
			}
			break;
		}
		case Message::RequestHeader::SENSOR_AWAIT_STATE: {
			subscribers.push(from);
			break;
		}
		case Message::RequestHeader::SENSOR_START_UPDATE: {
			/**
			 * This call subscribe yourself to the right sensor while initializing a sensor reading
			 */
			subscribers.push(from);
			Message::Send::SendNoReply(courier, (const char*)&req_to_courier, sizeof(SensorCourierReq));
			break;
		}
		default: {
			Task::_KernelCrash("Sensor Admin: illegal type: [%d]\r\n", req.header);
		}
		}
	}
}

void Sensor::sensor_courier() {
	AddressBook addr = getAddressBook();

	int from;

	SensorCourierReq req;
	SensorAdminReq req_to_admin;
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorCourierReq));
		Message::Reply::EmptyReply(from); // unblock caller right away
		switch (req.header) {
		case Message::RequestHeader::SENSOR_COUR_AWAIT_READING: {
			UART::Putc(addr.train_trans_tid, 1, (char)133);
			Clock::Delay(addr.clock_tid, SENSOR_DELAY); // actual delay time is managed by the main server

			for (int i = 0; i < NUM_SENSOR_BYTES; i++) {
				req_to_admin.body.sensor_state[i] = UART::Getc(addr.train_receive_tid, 1);
			}
			req_to_admin.header = Message::RequestHeader::SENSOR_UPDATE;
			Message::Send::SendNoReply(addr.sensor_admin_tid, (const char*)&req_to_admin, sizeof(SensorAdminReq));
			break;
		}
		default: {
			Task::_KernelCrash("Sensor Courier: illegal type: [%d]\r\n", req.header);
		}
		} // switch
	}
	Task::_KernelCrash("Sensor Courier: Exited, should not exit!\r\n");
}
