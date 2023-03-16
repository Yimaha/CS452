#include "k4_client.h"
using namespace Message;

void SystemTask::k4_dummy() {
	while (true) {
		char c = UART::Getc(UART::UART_0_RECEIVER_TID, 0);
		UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, c);
	}
}

void SystemTask::k4_dummy_train() {
	int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	Train::TrainAdminReq req = { RequestHeader::TRAIN_SPEED, Train::RequestBody { 2, 0 } };
	while (true) {
		for (int i = 0; i < 10; i++) {
			req.body.command.id = 78;
			req.body.command.action = i * 2;
			Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
			Clock::Delay(clock_tid, 100);
		}
	}
}

void SystemTask::k4_dummy_train_rev() {
	int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	Train::TrainAdminReq req = { RequestHeader::TRAIN_SPEED, Train::RequestBody { 2, 0 } };
	req.body.command.id = 78;
	req.body.command.action = 12;
	Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
	Clock::Delay(clock_tid, 100);
	while (true) {
		req.header = RequestHeader::TRAIN_REV;
		req.body.command.id = 78;
		req.body.command.action = 12;
		Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
		Clock::Delay(clock_tid, 1000);
	}
}

void SystemTask::k4_dummy_train_sensor() {

	int sensor_admin = Name::WhoIs(Sensor::SENSOR_ADMIN_NAME);
	int uart_tid = Name::WhoIs(UART::UART_0_TRANSMITTER);
	Sensor::SensorAdminReq req;
	req.header =  RequestHeader::SENSOR_AWAIT_STATE;
	char result[10];

	while (true) {
		Send::Send(sensor_admin, (const char*)&req, sizeof(req), result, 10);
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

void SystemTask::k4_dummy_train_switch() {
	int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int uart_0_tid = Name::WhoIs(UART::UART_0_TRANSMITTER);
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);

	while (true) {
		Train::TrainAdminReq req = { RequestHeader::TRAIN_SWITCH, Train::RequestBody { 10, 'c' } };
		Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
		Clock::Delay(clock_tid, 200);
		UART::Puts(uart_0_tid, 0, "curve\r\n", 7);
		req = { RequestHeader::TRAIN_SWITCH, Train::RequestBody { 10, 's' } };
		Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
		Clock::Delay(clock_tid, 200);
		UART::Puts(uart_0_tid, 0, "straight\r\n", 10);
	}
}
