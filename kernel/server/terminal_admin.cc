#include "terminal_admin.h"
#include "../etl/circular_buffer.h"
#include "../server/train_admin.h"
#include "../utils/buffer.h"
#include "../utils/printf.h"
#include "courier_pool.h"
using namespace Terminal;
using namespace Message;

const char SENSOR_LETTERS[] = "ABCDE";
const char MOVE_CURSOR[] = "\033[r;cH";
constexpr char SPACES[] = "                                                               ";

struct Command {
	char cmd[CMD_LEN];
	int len;
};

void log_time(char buf[], const uint32_t ticks) {
	char to = '0' + ticks % 10;

	uint32_t seconds = (ticks / 10) % 60;
	char so = '0' + seconds % 10;
	char st = '0' + (seconds / 10) % 10;

	uint32_t minutes = ticks / 600;
	char mo = '0' + minutes % 10;
	char mt = '0' + (minutes / 10) % 10;
	char mh = '0' + (minutes / 100) % 10;

	sprintf(buf, "%c%c%c:%c%c.%c", mh, mt, mo, st, so, to);
}

void move_cursor(int r, int c, char* buf, size_t* len) {
	char numstr[10] = { 0 };
	*len = 0;
	for (size_t i = 0; i < sizeof(MOVE_CURSOR) - 1; ++i) {
		if (MOVE_CURSOR[i] == 'r') {
			itoa_10(r, numstr);
			for (int j = 0; numstr[j] != '\0'; ++j) {
				buf[*len] = numstr[j];
				*len += 1;
			}

		} else if (MOVE_CURSOR[i] == 'c') {
			itoa_10(c, numstr);
			for (int j = 0; numstr[j] != '\0'; ++j) {
				buf[*len] = numstr[j];
				*len += 1;
			}

		} else {
			buf[*len] = MOVE_CURSOR[i];
			++(*len);
		}
	}
}

// Given a switch number, find the cursor position
// of the switch in the UI
void sw_to_cursor_pos(char sw, int* r, int* c) {
	if (sw < 19) {
		*r = 9 + 2 * ((sw - 1) / 6);
		*c = 5 + 6 * ((sw - 1) % 6);
	} else {
		*r = 15;
		*c = 8 + 9 * (sw - 153);
	}
}

void error_out(const char* error_msg) {
	char buffer[64];
	sprintf(buffer, "\r\n%s", error_msg);
	UART::PutsNullTerm(UART::UART_0_TRANSMITTER_TID, 0, buffer, 64);
}

void error_and_reset(const char* error_msg, int* char_count) {
	error_out(error_msg);
	*char_count = 0;
}

bool check_char(const char c, const char expected, const char* error_msg) {
	if (c != expected) {
		error_out(error_msg);
		return true;
	}
	return false;
}

int handle_tr(int train_server_tid, const char cmd[]) {
	if (check_char(cmd[1], 'r', ERROR) || check_char(cmd[2], ' ', ERROR))
		return -1;

	int i = 3;
	int out_len = 0;
	int train = scan_int(cmd + i, 2, &out_len);
	if (train == -1) {
		error_out(ERROR);
		return -1;
	}

	i += out_len + 1;
	if (check_char(cmd[i - 1], ' ', ERROR))
		return -1;

	int speed = scan_int(cmd + i, 2, &out_len);
	if (speed == -1) {
		error_out(ERROR);
		return -1;
	}

	i += out_len;
	for (; cmd[i] != '\r'; ++i) {
		if (check_char(cmd[i], ' ', ERROR)) {
			return -1;
		}
	}

	Train::TrainAdminReq req;
	req.header = Train::RequestHeader::SPEED;
	req.body.id = train;
	req.body.action = speed;
	Message::Send::Send(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req), nullptr, 0);

	return 0;
}

int handle_rv(Courier::CourierPool<TerminalCourierMessage>& pool, const char cmd[]) {
	if (check_char(cmd[1], 'v', ERROR) || check_char(cmd[2], ' ', ERROR))
		return -1;

	int out_len = 0;
	int i = 3;
	int train = scan_int(cmd + i, 2, &out_len);
	if (train == -1) {
		error_out(ERROR);
		return -1;
	}

	i += out_len;
	for (; cmd[i] != '\r'; ++i) {
		if (check_char(cmd[i], ' ', ERROR)) {
			return -1;
		}
	}

	TerminalCourierMessage req = { CourierRequestHeader::REV, train };
	pool.request(&req, sizeof(req));
	return 0;
}

int handle_sw(int train_server_tid, const char cmd[]) {
	if (check_char(cmd[1], 'w', ERROR) || check_char(cmd[2], ' ', ERROR))
		return -1;

	char snum = 0;
	char status = 0;
	int i = 3;

	int out_len = 0;
	int switch_num = scan_int(cmd + i, 3, &out_len);

	if (switch_num == -1) {
		error_out(ERROR);
		return -1;
	} else {
		snum = switch_num;
	}

	i += out_len + 2;
	if (check_char(cmd[i - 2], ' ', ERROR))
		return -1;

	status = lower(cmd[i - 1]);
	if (status != 'c' && status != 's') {
		error_out(ERROR);
		return -1;
	}

	for (; cmd[i] != '\r'; ++i) {
		if (check_char(cmd[i], ' ', ERROR)) {
			return -1;
		}
	}

	Train::TrainAdminReq req;
	req.header = Train::RequestHeader::SWITCH;
	req.body.id = snum;
	req.body.action = status;
	Message::Send::Send(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req), nullptr, 0);

	return 0;
}

void str_cpy(const char* source, char* target, int* index, int len, bool check_null_char = false) {
	for (int i = 0; i < len && (!check_null_char || source[i] != '\0'); i++) {
		target[*index] = source[i];
		(*index) += 1;
	}
}

void set_switch(int train_server_tid, char snum, char status) {
	Train::TrainAdminReq req;
	req.header = Train::RequestHeader::SWITCH;
	req.body.id = snum;
	req.body.action = status;
	Message::Send::Send(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req), nullptr, 0);
}

void Terminal::terminal_admin() {
	Name::RegisterAs(TERMINAL_ADMIN);
	int uart_0_server_tid = Name::WhoIs(UART::UART_0_TRANSMITTER);
	int train_server_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int from;
	TerminalServerReq req;

	Courier::CourierPool<TerminalCourierMessage> courier_pool = Courier::CourierPool<TerminalCourierMessage>(&terminal_courier, Priority::TERMINAL_PRIORITY);

	UART::Puts(uart_0_server_tid, 0, START_PROMPT, sizeof(START_PROMPT) - 1);

	Task::Create(Priority::TERMINAL_PRIORITY, &terminal_clock_courier);
	Task::Create(Priority::TERMINAL_PRIORITY, &sensor_query_courier);
	Task::Create(Priority::TERMINAL_PRIORITY, &idle_time_courier);
	Task::Create(Priority::TERMINAL_PRIORITY, &user_input_courier);

	bool isRunning = false;

	char printing_buffer[1024]; // 1024 is good for now
	int printing_index = 0;
	char buf[100];
	etl::circular_buffer<Command, CMD_HISTORY_LEN> cmd_history = etl::circular_buffer<Command, CMD_HISTORY_LEN>();
	cmd_history.push(Command { { 0 }, 0 });
	size_t cmd_history_index = 0;
	int escape_status = 0;

	bool isSensorModified = false;
	char sensor_state[10];
	etl::circular_buffer<int, 10> recent_sensor;

	uint64_t idle_time, total_time;
	bool isIdleTimeModified = false;
	int char_count = 0;
	int ticks = 0;

	// This is used to keep track of number of activated sensors

	auto trigger_print = [&]() {
		if (isRunning) {
			printing_index = 0;
			str_cpy(SAVE_CURSOR, printing_buffer, &printing_index, sizeof(SAVE_CURSOR) - 1);
			str_cpy(TOP_LEFT, printing_buffer, &printing_index, sizeof(TOP_LEFT) - 1);
			log_time(buf, ticks);
			str_cpy(buf, printing_buffer, &printing_index, 8);
			if (isIdleTimeModified) {
				isIdleTimeModified = false;
				uint64_t leading = idle_time * 100 / total_time;
				uint64_t trailing = (idle_time * 100000) / total_time % 1000;

				sprintf(buf, "\033[1;80HPercent: %llu.%03llu", leading, trailing);
				str_cpy(buf, printing_buffer, &printing_index, 100, true);
			}
			if (isSensorModified) {
				isSensorModified = false;
				str_cpy(SENSOR_CURSOR, printing_buffer, &printing_index, sizeof(SENSOR_CURSOR) - 1);
				// str_cpy(RED_CURSOR, printing_buffer, &printing_index, sizeof(RED_CURSOR) - 1);
				for (int i = 0; i < Sensor::NUM_SENSOR_BYTES; i++) {
					const char l = SENSOR_LETTERS[i / 2];
					int pos = 8 * (i % 2);
					for (int j = 1; j <= 8; j++) {
						if (sensor_state[i] & (1 << (8 - j))) {
							char ones = '0' + ((j + pos) % 10);
							char write[4] = { l, ((j + pos > 9) ? '1' : '0'), ones, ' ' };
							str_cpy(write, printing_buffer, &printing_index, 4);
						}
					}
				}
				str_cpy(RESET_CURSOR, printing_buffer, &printing_index, sizeof(RESET_CURSOR) - 1);
				str_cpy(SPACES, printing_buffer, &printing_index, sizeof(SPACES) - 1);
			}

			str_cpy(RESTORE_CURSOR, printing_buffer, &printing_index, sizeof(RESTORE_CURSOR) - 1);
			if (printing_index >= 512) {
				Task::_KernelCrash("Too much printing, %d\r\n", printing_index);
			}
			UART::Puts(uart_0_server_tid, 0, printing_buffer, printing_index);
		}
	};

	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(TerminalServerReq));
		switch (req.header) {
		case RequestHeader::CLOCK: {
			// 100ms clock update
			Reply::Reply(from, nullptr, 0);
			ticks += 1;
			trigger_print();
			break;
		}
		case RequestHeader::SENSORS: {
			// Should be 10 bytes of sensor data.
			// Print out all the sensors, in a fancy UI way.
			Reply::Reply(from, nullptr, 0);
			for (int i = 0; i < 10; i++) {
				sensor_state[i] = req.body.worker_msg.msg[i];
			}
			isSensorModified = true;
			break;
		}
		case RequestHeader::IDLE: {
			Reply::Reply(from, nullptr, 0);
			Clock::IdleStats(&idle_time, &total_time);
			isIdleTimeModified = true;
			break;
		}
		case RequestHeader::START: {
			Reply::Reply(from, nullptr, 0);
			UART::Puts(uart_0_server_tid, 0, CLEAR_SCREEN, sizeof(CLEAR_SCREEN) - 1);
			UART::Puts(uart_0_server_tid, 0, TOP_LEFT, sizeof(TOP_LEFT) - 1);
			UART::Puts(uart_0_server_tid, 0, SENSOR_DATA, sizeof(SENSOR_DATA) - 1);
			for (int i = 0; i < Terminal::SWITCH_UI_LEN; ++i) {
				UART::PutsNullTerm(uart_0_server_tid, 0, Terminal::SWITCH_UI[i]);
			}
			UART::Puts(uart_0_server_tid, 0, WELCOME_MSG, sizeof(WELCOME_MSG) - 1);
			UART::Puts(uart_0_server_tid, 0, PROMPT, sizeof(PROMPT) - 1);
			// disabled for now
			// for some reason, when this is enabled, not only does switch not fire, there is also
			// for (int i = 1; i <= 18; ++i) {
			// 	set_switch(train_server_tid, i, 'c');
			// }

			// for (int i = 153; i <= 156; ++i) {
			// 	set_switch(train_server_tid, i, 'c');
			// }
			isRunning = true;
			break;
		}
		case RequestHeader::REVERSE_COMPLETE: {
			courier_pool.receive(from);
			break;
		}
		case RequestHeader::PUTC: {
			Reply::Reply(from, nullptr, 0);
			char c = req.body.regular_msg;
			if (char_count > CMD_LEN) {
				error_and_reset(LENGTH_ERROR, &char_count);
				UART::Puts(uart_0_server_tid, 0, PROMPT, sizeof(PROMPT) - 1);
			} else if (escape_status == TA_FOUND_ESCAPE) {
				escape_status = (c == '[') ? TA_FOUND_BRACKET : TA_DEFAULT_ARROW_STATE;
			} else if (escape_status == TA_FOUND_BRACKET) {
				switch (c) {
				case 'A': { // up arrow
					if (cmd_history_index > 0) {
						cmd_history_index--;
						UART::Putc(uart_0_server_tid, 0, '\r');
						if (char_count > 0) {
							UART::Puts(uart_0_server_tid, 0, SPACES, char_count + sizeof(PROMPT_NNL) - 1);
							UART::Putc(uart_0_server_tid, 0, '\r');
						}

						UART::Puts(uart_0_server_tid, 0, PROMPT_NNL, sizeof(PROMPT_NNL) - 1);
						UART::Puts(uart_0_server_tid, 0, cmd_history[cmd_history_index].cmd, cmd_history[cmd_history_index].len);

						char_count = cmd_history[cmd_history_index].len;
					}
					break;
				}
				case 'B': { // down arrow
					if (cmd_history_index < cmd_history.size() - 1) {
						cmd_history_index++;
						UART::Putc(uart_0_server_tid, 0, '\r');
						if (char_count > 0) {
							UART::Puts(uart_0_server_tid, 0, SPACES, char_count + sizeof(PROMPT_NNL) - 1);
							UART::Putc(uart_0_server_tid, 0, '\r');
						}

						UART::Puts(uart_0_server_tid, 0, PROMPT_NNL, sizeof(PROMPT_NNL) - 1);
						UART::Puts(uart_0_server_tid, 0, cmd_history[cmd_history_index].cmd, cmd_history[cmd_history_index].len);

						char_count = cmd_history[cmd_history_index].len;
					}
					break;
				}
				case 'C': { // right arrow
					if (cmd_history[cmd_history_index].cmd[char_count] != '\0') {
						char_count++;
					}
					break;
				}
				case 'D': { // left arrow
					if (char_count > 0) {
						char_count--;
					}
					break;
				}
				default: {
					char buf[3] = { '\033', '[', c };
					UART::Puts(uart_0_server_tid, 0, buf, 3);
				}
				}

				escape_status = 0;
			} else if (c == '\b') {
				if (char_count > 0) {
					cmd_history[cmd_history_index].cmd[char_count] = '\0';
					char_count--;
					UART::Puts(uart_0_server_tid, 0, "\b \b", 3);
				}
			} else if (c == '\r') {
				cmd_history[cmd_history_index].cmd[char_count] = '\r';
				switch (cmd_history[cmd_history_index].cmd[0]) {
				case 't': {
					handle_tr(train_server_tid, cmd_history[cmd_history_index].cmd);
					break;
				}
				case 'r': {
					handle_rv(courier_pool, cmd_history[cmd_history_index].cmd);
					break;
				}
				case 's': {
					handle_sw(train_server_tid, cmd_history[cmd_history_index].cmd);
					break;
				}
				case 'q': {
					int i = 1;
					while (cmd_history[cmd_history_index].cmd[i] == ' ' && i < CMD_LEN) {
						++i;
					}

					if (cmd_history[cmd_history_index].cmd[i] == '\r') {
						UART::Puts(uart_0_server_tid, 0, QUIT, sizeof(QUIT) - 1);

						for (int k = 0; k < Train::NUM_TRAINS; ++k) {
							// Stop all trains
							// set_train_speed(train_logic_server_tid, Train::TRAIN_NUMBERS[k], 0);
						}

						// Should have some kind of command to flush the buffer
						// lol I am litearry crashing the server right now, which trigger a reboot
						// oh wtf, oh I guess that makes sense, still bruh
						kernel_assert(1 == 2);
					} else {
						error_out(ERROR);
					}
					break;
				}
				default: {
					error_out(ERROR);
					break;
				}
				}

				cmd_history[cmd_history_index].len = char_count;
				char_count = 0;
				if (cmd_history_index < cmd_history.max_size() - 1) {
					cmd_history.push(Command { { 0 }, 0 });
					cmd_history_index++;
				} else {
					cmd_history.pop();
					cmd_history.push(Command { { 0 }, 0 });
				}

				UART::Puts(uart_0_server_tid, 0, PROMPT, sizeof(PROMPT) - 1);
			} else if (c == '\033') {
				// Escape sequence. Try to read an arrow key.
				escape_status = TA_FOUND_ESCAPE;
			} else {
				cmd_history[cmd_history_index].cmd[char_count++] = c;
				UART::Putc(uart_0_server_tid, 0, c);
			}
			break;
		}
		default: {
			break;
		}
		}
	}
}

void Terminal::terminal_courier() {

	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);

	Terminal::TerminalCourierMessage req;
	Terminal::TerminalServerReq req_to_admin;
	Train::TrainAdminReq req_to_train;
	int from;
	while (1) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TerminalCourierMessage));
		Message::Reply::Reply(from, nullptr, 0); // unblock caller right away
		switch (req.header) {
		case CourierRequestHeader::REV: {
			// it wait for about 4 seconds then send in the command to reverse and speed up
			req_to_train.header = Train::RequestHeader::REV;
			req_to_train.body.id = req.body;
			Message::Send::Send(train_tid, reinterpret_cast<char*>(&req_to_train), sizeof(req_to_train), nullptr, 0);
			req_to_admin = { RequestHeader::REVERSE_COMPLETE, '0' };
			Message::Send::Send(terminal_tid, (const char*)&req_to_admin, sizeof(req_to_admin), nullptr, 0);
			break;
		}
		default: {
			Task::_KernelCrash("Train Courier illegal type: [%d]\r\n", req.header);
		}
		}
	}
}

void Terminal::terminal_clock_courier() {
	Name::RegisterAs(TERMINAL_CLOCK_COURIER_NAME);

	int repeat = CLOCK_UPDATE_FREQUENCY;
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	int internal_timer = Clock::Time(clock_tid);
	Terminal::TerminalServerReq req = Terminal::TerminalServerReq(Terminal::RequestHeader::CLOCK, internal_timer);

	while (true) {
		Message::Send::Send(terminal_tid, reinterpret_cast<char*>(&req), sizeof(Terminal::TerminalServerReq), nullptr, 0);
		internal_timer += repeat;
		Clock::DelayUntil(clock_tid, internal_timer);
	}
}

void Terminal::sensor_query_courier() {
	Name::RegisterAs(TERMINAL_SENSOR_COURIER_NAME);

	int sensor_admin = Name::WhoIs(Sensor::SENSOR_ADMIN_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	Sensor::SensorAdminReq req;
	req.header = Sensor::RequestHeader::GET_SENSOR_STATE;

	Terminal::TerminalServerReq treq;
	treq.header = Terminal::RequestHeader::SENSORS;
	while (true) {
		Message::Send::Send(sensor_admin, reinterpret_cast<char*>(&req), sizeof(Sensor::SensorAdminReq), treq.body.worker_msg.msg, Sensor::NUM_SENSOR_BYTES);
		Message::Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq), nullptr, 0);
	}
}

void Terminal::idle_time_courier() {
	// technically idle_task will never need to yield to kernel
	// since there is no context swtich, only interrupt after this point
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);

	Terminal::TerminalServerReq treq;
	treq.header = Terminal::RequestHeader::IDLE;

	while (true) {
		Message::Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq), nullptr, 0);
		Clock::Delay(clock_tid, 200);
	}
}

void Terminal::user_input_courier() {
	Terminal::TerminalServerReq treq;
	treq.header = Terminal::RequestHeader::START;

	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);

	UART::Getc(UART::UART_0_RECEIVER_TID, 0);
	Message::Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq), nullptr, 0);
	treq.header = Terminal::RequestHeader::PUTC;
	while (true) {
		treq.body.regular_msg = UART::Getc(UART::UART_0_RECEIVER_TID, 0);
		Message::Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq), nullptr, 0);
	}
}
