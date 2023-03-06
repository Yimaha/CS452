#include "sensor_admin.h"
#include "courier_pool.h"

void Sensor::sensor_admin() {
	Name::RegisterAs(SENSOR_ADMIN_NAME);

	etl::queue<int, SENSOR_ADMIN_NUM_SUBSCRIBERS> subscribers;
	int uart_trans_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);

	int courier = Task::Create(Priority::HIGH_PRIORITY, &sensor_courier);

	int from;
	SensorAdminReq req;
	if (UART::Putc(uart_trans_tid, 1, 0xc0) == -1) {
		Task::_KernelCrash("Failed to enable sensor\r\n");
	}
	char sensor_state[NUM_SENSOR_BYTES] = { 0b10101010 };
	SensorCourierReq req_to_courier = { CourierRequestHeader::OBSERVER, { SENSOR_DELAY } };
	Message::Send::Send(courier, (const char*)&req_to_courier, sizeof(SensorCourierReq), nullptr, 0);
#ifdef TIME_OUT
	uint64_t sensor_query_counter = 0;
	Courier::CourierPool<SensorCourierReq> courier_pool = Courier::CourierPool<SensorCourierReq>(&courier_time_out, Priority::TERMINAL_PRIORITY);
	SensorCourierReq req_to_timeout = { CourierRequestHeader::TIMEOUT, { sensor_query_counter } };
#endif

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorAdminReq));
		switch (req.header) {
		case RequestHeader::SENSOR_UPDATE: {
			Message::Reply::Reply(from, nullptr, 0); // after copying
			Message::Send::Send(from, (const char*)&req_to_courier, sizeof(SensorCourierReq), nullptr, 0);
#ifdef TIME_OUT
			sensor_query_counter += 1;
			req_to_timeout.body.info = sensor_query_counter;
			courier_pool.request(&req_to_timeout, sizeof(req_to_timeout));
#endif
			for (int i = 0; i < NUM_SENSOR_BYTES; i++) {
				sensor_state[i] = req.body.sensor_state[i];
			}
			while (!subscribers.empty()) {
				Message::Reply::Reply(subscribers.front(), sensor_state, NUM_SENSOR_BYTES);
				subscribers.pop();
			}

			break;
		}
		case RequestHeader::GET_SENSOR_STATE: {
			subscribers.push(from);
			break;
		}
#ifdef TIME_OUT
		case RequestHeader::SENSOR_TIME_OUT: {
			if (req.body.time_out_id >= sensor_query_counter) {
				Task::_KernelCrash("Sensor Admin reading time out\r\n");
			}
			courier_pool.receive(from);
			break;
		}
#endif
		default: {
			Task::_KernelCrash("Sensor Admin: illegal type: [%d]\r\n", req.header);
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

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorCourierReq));
		Message::Reply::Reply(from, nullptr, 0); // unblock caller right away
		switch (req.header) {
		case CourierRequestHeader::OBSERVER: {
			UART::Putc(uart_trans_tid, 1, (char)133); // random delay between 50 - 60 ms
			Clock::Delay(clock_tid, req.body.info); // delay by 60 always to unify result
			for (int i = 0; i < NUM_SENSOR_BYTES; i++) {
				req_to_admin.body.sensor_state[i] = UART::Getc(uart_rec_tid, 1);
			}
			req_to_admin.header = RequestHeader::SENSOR_UPDATE;
			Message::Send::Send(sensor_admin_tid, (const char*)&req_to_admin, sizeof(SensorAdminReq), nullptr, 0);
			break;
		}
		default: {
			Task::_KernelCrash("Sensor Courier: illegal type: [%d]\r\n", req.header);
		}
		} // switch
	}
}

void Sensor::courier_time_out() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int sensor_admin_tid = Name::WhoIs(SENSOR_ADMIN_NAME);
	int from;
	const int DELAY = 100;
	SensorCourierReq req;
	SensorAdminReq req_to_admin = { RequestHeader::SENSOR_TIME_OUT, { 0 } };

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorCourierReq));
		Message::Reply::Reply(from, nullptr, 0); // unblock caller right away
		switch (req.header) {
		case CourierRequestHeader::TIMEOUT: {
			Clock::Delay(clock_tid, DELAY); // it could delay by 0 as well, which might clog the uart channel too much
			req_to_admin.body.time_out_id = req.body.info;
			Message::Send::Send(sensor_admin_tid, (const char*)&req_to_admin, sizeof(req_to_admin), nullptr, 0);
			break;
		}
		default: {
			Task::_KernelCrash("Sensor Courier: illegal type: [%d]\r\n", req.header);
		}
		} // switch
	}
}