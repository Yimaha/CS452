#include "sensor_admin.h"

void Sensor::sensor_admin() {
	Name::RegisterAs(SENSOR_ADMIN_NAME);

	// const int POOL_SIZE = 20;
	// etl::queue<int, POOL_SIZE> courier_pool;
	// for (int i = 0; i < POOL_SIZE; i++) {
	// 	courier_pool.push(Task::Create(2, &sensor_courier));
	// }
	etl::queue<int, 32> subscribers;
	int uart_trans_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);

	int courier = Task::Create(2, &sensor_courier);
	int from;
	SensorAdminReq req;
	if (UART::Putc(uart_trans_tid, 1, 0xc0) == -1) {
		Task::_KernelCrash("Failed to enable sensor\r\n");
	}
	char sensor_state[10] = { 0b10101010 };
	SensorCourierReq req_to_courier = { CourierRequestHeader::OBSERVER, { 10 } };
	Message::Send::Send(courier, (const char*)&req_to_courier, sizeof(SensorCourierReq), nullptr, 0);

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorAdminReq));
		switch (req.header) {
		case RequestHeader::SENSOR_UPDATE: {
			Message::Reply::Reply(from, nullptr, 0); // after copying
			Message::Send::Send(from, (const char*)&req_to_courier, sizeof(SensorCourierReq), nullptr, 0);

			for (int i = 0; i < 10; i++) {
				sensor_state[i] = req.body.sensor_state[i];
			}
			while (!subscribers.empty()) {
				Message::Reply::Reply(subscribers.front(), sensor_state, 10);
				subscribers.pop();
			}

			break;
		}

		case RequestHeader::GET_SENSOR_STATE: {
			subscribers.push(from);
			break;
		}
		default: {
			char exception[30];
			sprintf(exception, "Sensor Admin: illegal type: [%d]\r\n", req.header);
			Task::_KernelCrash(exception);
		}
		}
	}
}

void Sensor::sensor_courier() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int sensor_admin_tid = Name::WhoIs(SENSOR_ADMIN_NAME);
	int uart_trans_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);
	int uart_rec_tid = Name::WhoIs(UART::UART_1_RECEIVER);
	int from;

	SensorCourierReq req;
	SensorAdminReq req_to_admin;

	while (1) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorCourierReq));
		Message::Reply::Reply(from, nullptr, 0); // unblock caller right away
		switch (req.header) {
		case CourierRequestHeader::OBSERVER: {
			UART::Putc(uart_trans_tid, 1, (char)133);
			Clock::Delay(clock_tid, req.body.delay);
			for (int i = 0; i < 10; i++) {
				req_to_admin.body.sensor_state[i] = UART::Getc(uart_rec_tid, 1);
			}
			req_to_admin.header = RequestHeader::SENSOR_UPDATE;
			Message::Send::Send(sensor_admin_tid, (const char*)&req_to_admin, sizeof(SensorAdminReq), nullptr, 0);
			break;
		}
		default: {
			char exception[30];
			sprintf(exception, "Sensor Courier: illegal type: [%d]\r\n", req.header);
			Task::_KernelCrash(exception);
		}
		}
	}
}