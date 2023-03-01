#include "k4_client.h"

void SystemTask::k4_dummy() {
	while (1) {
		char c = UART::Getc(UART::UART_0_RECEIVER_TID, 0);
		UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, c);
	}
}

void SystemTask::k4_dummy_train() {
	int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	Train::TrainAdminReq req = { Train::RequestHeader::SPEED, Train::RequestBody { 2, 0 } };
	while (1) {
		for (int i = 0; i < 10; i++) {
			req.body.id = 2;
			req.body.action = i * 2;
			Message::Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
			Clock::Delay(clock_tid, 100);
		}
	}
}

void SystemTask::k4_dummy_train_rev() {
	int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	Train::TrainAdminReq req = { Train::RequestHeader::SPEED, Train::RequestBody { 2, 0 } };
	req.body.id = 78;
	req.body.action = 12;
	Message::Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
	Clock::Delay(clock_tid, 100);
	while (1) {
		req.header = Train::RequestHeader::REV;
		req.body.id = 78;
		req.body.action = 12;
		Message::Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
		Clock::Delay(clock_tid, 1000);
	}
}

void SystemTask::k4_dummy_train_sensor() {

	int sensor_admin = Name::WhoIs(Sensor::SENSOR_ADMIN_NAME);
	int uart_tid = Name::WhoIs(UART::UART_0_TRANSMITTER);
	Sensor::SensorAdminReq req;
	req.header = Sensor::RequestHeader::GET_SENSOR_STATE;
	char result[10];

	while (1) {
		Message::Send::Send(sensor_admin, (const char*)&req, sizeof(req), result, 10);
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 8; j++) {
				UART::Putc(uart_tid, 0, ((result[i] & (0x80)) == (0x80)) ? '1' : '0');
				result[i] = result[i] << 1;
			}
			if (i % 2 == 1) {
				UART::Puts(uart_tid, 0, "\r\n", 2);
			}
		}
		UART::Puts(uart_tid, 0, "\r\n", 2);
	}
}