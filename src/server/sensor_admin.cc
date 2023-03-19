#include "sensor_admin.h"
#include "courier_pool.h"
using namespace Message;

void Sensor::sensor_admin() {
	Name::RegisterAs(SENSOR_ADMIN_NAME);
	AddressBook addr = getAddressBook();

	// sensor subscribers
	etl::queue<int, SENSOR_ADMIN_NUM_SUBSCRIBERS> subscribers;
	etl::queue<int, SENSOR_ADMIN_NUM_SUBSCRIBERS> delay_subscribers;

	// sensor couriers
	int courier = Task::Create(Priority::HIGH_PRIORITY, &sensor_courier);
	// sensor requests
	int from;
	SensorAdminReq req;
	bool resync = false;
	char sensor_state[NUM_SENSOR_BYTES] = { 0b10101010 };
	if (UART::Putc(addr.train_trans_tid, 1, 0xc0) == -1) {
		Task::_KernelCrash("Failed to enable sensor\r\n");
	}
	uint64_t delay_ticks = Clock::Time(addr.clock_tid) + SENSOR_DELAY;
	SensorCourierReq req_to_courier = { Message::RequestHeader::SENSOR_COUR_AWAIT_READING, { delay_ticks } };
	Message::Send::SendNoReply(courier, (const char*)&req_to_courier, sizeof(SensorCourierReq));
#ifdef TIME_OUT
	uint64_t sensor_query_counter = 0;
	Courier::CourierPool<SensorCourierReq> courier_pool = Courier::CourierPool<SensorCourierReq>(&courier_time_out, Priority::TERMINAL_PRIORITY);
	SensorCourierReq req_to_timeout = { Message::RequestHeader : SENSOR_TIMEOUT_START, { sensor_query_counter } };
#endif

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorAdminReq));
		switch (req.header) {
		case Message::RequestHeader::SENSOR_UPDATE: {
			if (resync) {
				uint64_t time = Clock::Time(addr.clock_tid);
				if (time - delay_ticks != 8) {
					delay_ticks = Clock::Time(addr.clock_tid); // if we are still not caught up
				} else {
					resync = false; // we caught up
				}
			}
			delay_ticks += SENSOR_DELAY;
			// sprintf(buf, "delay in sensor admin: %d realtime: %d\r\n", delay_ticks, Clock::Time(clock_tid));
			Message::Reply::Reply(from, (const char*)&delay_ticks, sizeof(uint64_t)); // after copying
#ifdef TIME_OUT
			sensor_query_counter += 1;
			req_to_timeout.body.info = sensor_query_counter;
			courier_pool.request(&req_to_timeout, sizeof(req_to_timeout));
#endif
			for (int i = 0; i < NUM_SENSOR_BYTES; i++) {
				sensor_state[i] = req.body.sensor_state[i];
			}
			while (!subscribers.empty()) {
				// regular subscriber gets information on current sensor state
				Message::Reply::Reply(subscribers.front(), sensor_state, NUM_SENSOR_BYTES);
				subscribers.pop();
			}
			while (!delay_subscribers.empty()) {
				// delay subscriber gets information on when the next update will be
				Message::Reply::Reply(delay_subscribers.front(), (const char*)&delay_ticks, sizeof(uint64_t));
				delay_subscribers.pop();
			}
			break;
		}
		case Message::RequestHeader::SENSOR_AWAIT_STATE: {
			subscribers.push(from);
			break;
		}
		case Message::RequestHeader::SENSOR_AWAIT_UPDATE: {
			delay_subscribers.push(from);
			break;
		}
		case Message::RequestHeader::SENSOR_RESYNC: {
			// in the future, resync is not suppose to be called from here, but from the global pathing server.
			Message::Reply::EmptyReply(from);
			resync = true;
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
	AddressBook addr = getAddressBook();

	int from;

	SensorCourierReq req;
	SensorAdminReq req_to_admin;

	Message::Receive::Receive(&from, (char*)&req, sizeof(SensorCourierReq));
	Message::Reply::EmptyReply(from); // unblock caller right away

	switch (req.header) {
	case Message::RequestHeader::SENSOR_COUR_AWAIT_READING: {
		while (true) {
			UART::Putc(addr.train_trans_tid, 1, (char)133); // random delay between 50 - 60 ms

			Clock::DelayUntil(addr.clock_tid, req.body.info); // actual delay time is managed by the main server
			for (int i = 0; i < NUM_SENSOR_BYTES; i++) {
				req_to_admin.body.sensor_state[i] = UART::Getc(addr.train_receive_tid, 1);
			}
			req_to_admin.header = Message::RequestHeader::SENSOR_UPDATE;
			Message::Send::Send(addr.sensor_admin_tid, (const char*)&req_to_admin, sizeof(SensorAdminReq), (char*)&req.body.info, sizeof(uint64_t));
		}
		break;
	}
	default: {
		Task::_KernelCrash("Sensor Courier: illegal type: [%d]\r\n", req.header);
	}
	} // switch
}

void Sensor::courier_time_out() {
	AddressBook addr = getAddressBook();

	int from;
	const int DELAY = 100;
	SensorCourierReq req;
	SensorAdminReq req_to_admin = { Message::RequestHeader::SENSOR_TIMEOUT, { 0 } };

	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(SensorCourierReq));
		Message::Reply::EmptyReply(from); // unblock caller right away
		switch (req.header) {
		case Message::RequestHeader::SENSOR_COUR_TIMEOUT_START: {
			Clock::Delay(addr.clock_tid, DELAY); // it could delay by 0 as well, which might clog the uart channel too much
			req_to_admin.body.time_out_id = req.body.info;
			Message::Send::SendNoReply(addr.sensor_admin_tid, (const char*)&req_to_admin, sizeof(req_to_admin));
			break;
		}
		default: {
			Task::_KernelCrash("Sensor Courier: illegal type: [%d]\r\n", req.header);
		}
		} // switch
	}
}