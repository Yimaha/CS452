#include "couriers.h"
#include "../server/sensor_admin.h"
#include "../server/terminal_admin.h"
#include "../server/train_admin.h"
#include "../server/train_logic.h"
#include "../utils/printf.h"
#include "../utils/utility.h"

using namespace Courier;

void error_and_prompt(int terminal_tid, const char* error_msg, const char* prompt, int* char_count) {
	Terminal::Puts(terminal_tid, "\r\n");
	Terminal::Puts(terminal_tid, error_msg);
	Terminal::Puts(terminal_tid, "\r\n");
	Terminal::Puts(terminal_tid, prompt);
	*char_count = 0;
}

bool check_char(int terminal_tid, const char c, const char expected, const char* error_msg, const char* prompt, int* char_count) {
	if (c != expected) {
		error_and_prompt(terminal_tid, error_msg, prompt, char_count);
		return true;
	}
	return false;
}

void set_train_speed(int train_logic_server_tid, int train, int speed) {
	TrainLogic::TrainLogicServerReq req;
	req.header = TrainLogic::RequestHeader::SPEED;
	req.body.id = train;
	req.body.action = speed;
	Message::Send::Send(train_logic_server_tid, reinterpret_cast<char*>(&req), sizeof(TrainLogic::TrainLogicServerReq), nullptr, 0);
}

void set_switch(int terminal_tid, int train_server_tid, char snum, char status) {
	Train::TrainAdminReq req;
	req.header = Train::RequestHeader::SWITCH;
	req.body.id = snum;
	req.body.action = status;
	Message::Send::Send(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req), nullptr, 0);

	Terminal::TerminalServerReq treq;
	treq.header = Terminal::RequestHeader::SWITCH;
	treq.body.worker_msg.msg[0] = snum;
	treq.body.worker_msg.msg[1] = status;
	Message::Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(treq), nullptr, 0);
}

int handle_tr(int terminal_tid, int train_server_tid, const char cmd[], int* char_count) {
	// Okay we assume good faith actors for now
	if (check_char(terminal_tid, cmd[1], 'r', ERROR, PROMPT, char_count))
		return -1;
	if (check_char(terminal_tid, cmd[2], ' ', ERROR, PROMPT, char_count))
		return -1;

	int i = 3;
	int out_len = 0;
	int train = scan_int(cmd + i, 2, &out_len);
	if (train == -1) {
		error_and_prompt(terminal_tid, ERROR, PROMPT, char_count);
		return -1;
	}

	i += out_len + 1;
	if (i > CMD_LEN) {
		error_and_prompt(terminal_tid, LENGTH_ERROR, PROMPT, char_count);
		return -1;
	} else if (check_char(terminal_tid, cmd[i - 1], ' ', ERROR, PROMPT, char_count))
		return -1;

	int speed = scan_int(cmd + i, 2, &out_len);
	if (speed == -1) {
		error_and_prompt(terminal_tid, ERROR, PROMPT, char_count);
		return -1;
	}

	i += out_len;
	if (i > CMD_LEN) {
		error_and_prompt(terminal_tid, LENGTH_ERROR, PROMPT, char_count);
		return -1;
	}

	for (; cmd[i] != '\r'; ++i) {
		if (i > CMD_LEN) {
			error_and_prompt(terminal_tid, LENGTH_ERROR, PROMPT, char_count);
			return -1;
		} else if (check_char(terminal_tid, cmd[i], ' ', ERROR, PROMPT, char_count)) {
			return -1;
		}
	}

	set_train_speed(train_server_tid, train, speed);
	return 0;
}

int handle_rv(int terminal_tid, int train_server_tid, const char cmd[], int* char_count) {
	if (check_char(terminal_tid, cmd[1], 'v', ERROR, PROMPT, char_count))
		return -1;
	if (check_char(terminal_tid, cmd[2], ' ', ERROR, PROMPT, char_count))
		return -1;

	int out_len = 0;
	int i = 3;
	int train = scan_int(cmd + i, 2, &out_len);
	if (train == -1) {
		error_and_prompt(terminal_tid, ERROR, PROMPT, char_count);
		return -1;
	}

	i += out_len;
	if (i > CMD_LEN) {
		error_and_prompt(terminal_tid, LENGTH_ERROR, PROMPT, char_count);
		return -1;
	}

	for (; cmd[i] != '\r'; ++i) {
		if (i > CMD_LEN) {
			error_and_prompt(terminal_tid, LENGTH_ERROR, PROMPT, char_count);
			return -1;
		} else if (check_char(terminal_tid, cmd[i], ' ', ERROR, PROMPT, char_count)) {
			return -1;
		}
	}

	TrainLogic::TrainLogicServerReq req;
	req.header = TrainLogic::RequestHeader::REV;
	req.body.id = train;
	Message::Send::Send(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req), nullptr, 0);

	return 0;
}

int handle_sw(int terminal_tid, int train_server_tid, const char cmd[], int* char_count) {
	if (check_char(terminal_tid, cmd[1], 'w', ERROR, PROMPT, char_count))
		return -1;
	if (check_char(terminal_tid, cmd[2], ' ', ERROR, PROMPT, char_count))
		return -1;

	char snum = 0;
	char status = 0;
	int i = 3;

	int out_len = 0;
	int switch_num = scan_int(cmd + i, 3, &out_len);

	if (switch_num == -1) {
		error_and_prompt(terminal_tid, ERROR, PROMPT, char_count);
		return -1;
	} else {
		snum = switch_num;
	}

	i += out_len + 2;
	if (i > CMD_LEN) {
		error_and_prompt(terminal_tid, LENGTH_ERROR, PROMPT, char_count);
		return -1;
	} else if (check_char(terminal_tid, cmd[i - 2], ' ', ERROR, PROMPT, char_count))
		return -1;

	status = lower(cmd[i - 1]);
	if (status != 'c' && status != 's') {
		error_and_prompt(terminal_tid, ERROR, PROMPT, char_count);
		return -1;
	}

	for (; cmd[i] != '\r'; ++i) {
		if (i > CMD_LEN) {
			error_and_prompt(terminal_tid, LENGTH_ERROR, PROMPT, char_count);
			return -1;
		} else if (check_char(terminal_tid, cmd[i], ' ', ERROR, PROMPT, char_count)) {
			return -1;
		}
	}

	set_switch(terminal_tid, train_server_tid, snum, status);
	return 0;
}

void Courier::user_input() {
	char cmd[CMD_LEN] = { 0 };
	char c;
	int char_count = 0;
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	int train_server_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int train_logic_server_tid = Name::WhoIs(TrainLogic::TRAIN_LOGIC_SERVER_NAME);
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);

	Terminal::Puts(terminal_tid, "Press any key to enter OS mode\r\n");
	UART::Getc(UART::UART_0_RECEIVER_TID, 0);
	Terminal::Puts(terminal_tid, Terminal::CLEAR_SCREEN);
	Terminal::Puts(terminal_tid, Terminal::TOP_LEFT);
	Terminal::Puts(terminal_tid, "\r\n\r\n\r\nSENSOR DATA:\r\n\r\n\r\n");

	for (int i = 0; i < Terminal::SWITCH_UI_LEN; ++i) {
		Terminal::Puts(terminal_tid, Terminal::SWITCH_UI[i]);
		Clock::Delay(clock_tid, 5);
	}

	// delay for a bit to let the switches print
	Clock::Delay(clock_tid, 100);

	// Set all the switches to curved
	for (int i = 0; i <= 18; ++i) {
		set_switch(terminal_tid, train_server_tid, i, 'c');
		Clock::Delay(clock_tid, 20);
	}

	for (int i = 153; i <= 156; ++i) {
		set_switch(terminal_tid, train_server_tid, i, 'c');
		Clock::Delay(clock_tid, 20);
	}

	Terminal::Puts(terminal_tid, "Welcome to AbyssOS!\r\n");
	Terminal::Puts(terminal_tid, PROMPT);
	Clock::Delay(clock_tid, 10);

	// Activate the terminal timer
	int clock_courier_tid = Name::WhoIs(CLOCK_COURIER_NAME);
	Message::Send::Send(clock_courier_tid, nullptr, 0, nullptr, 0);

	// Activate the sensor courier
	int sensor_courier_tid = Name::WhoIs(SENSOR_QUERY_COURIER_NAME);
	Message::Send::Send(sensor_courier_tid, nullptr, 0, nullptr, 0);

	while (true) {
		c = UART::Getc(UART::UART_0_RECEIVER_TID, 0);
		if (char_count > CMD_LEN) {
			error_and_prompt(terminal_tid, LENGTH_ERROR, PROMPT, &char_count);
			continue;
		} else if (c == '\b') {
			if (char_count > 0) {
				cmd[char_count] = '\0';
				char_count--;
				Terminal::Puts(terminal_tid, "\b \b");
			}
		} else if (c == '\r') {
			cmd[char_count] = '\r';
			switch (cmd[0]) {
			case 't': {
				int res = handle_tr(terminal_tid, train_logic_server_tid, cmd, &char_count);
				if (res == -1) {
					continue;
				}
				// TODO: Send to train administrator
				break;
			}
			case 'r': {
				int res = handle_rv(terminal_tid, train_logic_server_tid, cmd, &char_count);
				if (res == -1) {
					continue;
				}
				// TODO: Send to train administrator
				break;
			}
			case 's': {
				int res = handle_sw(terminal_tid, train_server_tid, cmd, &char_count);
				if (res == -1) {
					continue;
				}
				// TODO: Send to train administrator
				break;
			}
			case 'q': {
				int i = 1;
				while (cmd[i] == ' ' && i < CMD_LEN) {
					++i;
				}

				if (cmd[i] == '\r') {
					Terminal::Puts(terminal_tid, "\r\n");
					Terminal::Puts(terminal_tid, QUIT);
					Terminal::Puts(terminal_tid, "\r\n");

					for (int k = 0; k < TrainLogic::NUM_TRAINS; ++k) {
						// Stop all trains
						set_train_speed(train_logic_server_tid, TrainLogic::TRAIN_NUMBERS[k], 0);
						Clock::Delay(clock_tid, 2);
					}

					// Should have some kind of command to flush the buffer
					Task::Exit();
				} else {
					error_and_prompt(terminal_tid, ERROR, PROMPT, &char_count);
					continue;
				}
				break;
			}
			default: {
				error_and_prompt(terminal_tid, ERROR, PROMPT, &char_count);
				continue;
			}
			}

			char_count = 0;
			Terminal::Puts(terminal_tid, "\r\n");
			Terminal::Puts(terminal_tid, PROMPT);

			// Reset cmd
			for (int i = 0; i < CMD_LEN; ++i) {
				cmd[i] = '\0';
			}
		} else {
			cmd[char_count++] = c;
			Terminal::Putc(terminal_tid, c);
		}
	}

	Task::Exit();
}

void Courier::terminal_clock_courier() {
	int repeat = TERMINAL_TIMER_TICKS;
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	Terminal::TerminalServerReq req = Terminal::TerminalServerReq(Terminal::RequestHeader::CLOCK, 'C');

	Name::RegisterAs(CLOCK_COURIER_NAME);

	int from;
	Message::Receive::Receive(&from, nullptr, 0);
	Message::Reply::Reply(from, nullptr, 0);

	int internal_timer = Clock::Time(clock_tid);
	while (true) {
		internal_timer += repeat;
		Clock::DelayUntil(clock_tid, internal_timer);
		Message::Send::Send(terminal_tid, reinterpret_cast<char*>(&req), sizeof(Terminal::TerminalServerReq), nullptr, 0);
	}
}

void Courier::sensor_query_courier() {
	int sensor_admin = Name::WhoIs(Sensor::SENSOR_ADMIN_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	Sensor::SensorAdminReq req;
	req.header = Sensor::RequestHeader::GET_SENSOR_STATE;
	char result[10];

	Terminal::TerminalServerReq treq;
	treq.header = Terminal::RequestHeader::SENSORS;

	Name::RegisterAs(SENSOR_QUERY_COURIER_NAME);

	int from;
	Message::Receive::Receive(&from, nullptr, 0);
	Message::Reply::Reply(from, nullptr, 0);

	while (true) {
		Message::Send::Send(sensor_admin, reinterpret_cast<char*>(&req), sizeof(req), result, Terminal::NUM_SENSOR_BYTES);
		for (int i = 0; i < Terminal::NUM_SENSOR_BYTES; ++i) {
			treq.body.worker_msg.msg[i] = result[i];
		}

		Message::Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(treq), nullptr, 0);
	}
}
