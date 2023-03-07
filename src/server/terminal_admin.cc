#include "terminal_admin.h"
#include "../etl/circular_buffer.h"
#include "../etl/deque.h"
#include "../server/train_admin.h"
#include "../utils/buffer.h"
#include "../utils/printf.h"
#include "courier_pool.h"
#include <climits>
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
		*c = 8 + 9 * (sw - 19);
	}
}

// Given a train number, find the cursor position
// of the train in the UI
etl::pair<int, int> train_to_cursor_pos(int train) {
	switch (train) {
	case Train::TRAIN_NUMBERS[0]:
		return { TRAIN_PRINTOUT_ROW + 2, TRAIN_PRINTOUT_FIRST };
	case Train::TRAIN_NUMBERS[1]:
		return { TRAIN_PRINTOUT_ROW + 3, TRAIN_PRINTOUT_FIRST };
	case Train::TRAIN_NUMBERS[2]:
		return { TRAIN_PRINTOUT_ROW + 2, TRAIN_PRINTOUT_FIRST + TRAIN_PRINTOUT_WIDTH };
	case Train::TRAIN_NUMBERS[3]:
		return { TRAIN_PRINTOUT_ROW + 3, TRAIN_PRINTOUT_FIRST + TRAIN_PRINTOUT_WIDTH };
	case Train::TRAIN_NUMBERS[4]:
		return { TRAIN_PRINTOUT_ROW + 2, TRAIN_PRINTOUT_FIRST + 2 * TRAIN_PRINTOUT_WIDTH };
	case Train::TRAIN_NUMBERS[5]:
		return { TRAIN_PRINTOUT_ROW + 3, TRAIN_PRINTOUT_FIRST + 2 * TRAIN_PRINTOUT_WIDTH };
	default:
		return { -1, -1 };
	}
}

void error_out(const char* error_msg) {
	char buffer[64];
	sprintf(buffer, "\r\n%s", error_msg);
	UART::PutsNullTerm(UART::UART_0_TRANSMITTER_TID, 0, buffer, UART::UART_MESSAGE_LIMIT);
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
	req.header = RequestHeader::TRAIN_SPEED;
	req.body.id = train;
	req.body.action = speed;
	Send::Send(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req), nullptr, 0);

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

	TerminalCourierMessage req = { RequestHeader::TERM_COUR_REV, train };
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
	req.header = RequestHeader::TRAIN_SWITCH;
	req.body.id = snum;
	req.body.action = status;
	Send::Send(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req), nullptr, 0);

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
	req.header = RequestHeader::TRAIN_SWITCH;
	req.body.id = snum;
	req.body.action = status;
	Send::Send(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req), nullptr, 0);
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
	Task::Create(Priority::TERMINAL_PRIORITY, &switch_state_courier);
	Task::Create(Priority::TERMINAL_PRIORITY, &train_state_courier);

	bool isRunning = false;

	char printing_buffer[UART::UART_MESSAGE_LIMIT]; // 512 is good for now
	int printing_index = 0;
	char buf[TERM_A_BUFLEN];
	etl::circular_buffer<Command, CMD_HISTORY_LEN> cmd_history = etl::circular_buffer<Command, CMD_HISTORY_LEN>();
	cmd_history.push(Command { { 0 }, 0 });
	size_t cmd_history_index = 0;
	TAState escape_status = TAState::TA_DEFAULT_ARROW_STATE;

	bool isSensorModified = false;
	char sensor_state[Sensor::NUM_SENSOR_BYTES];
	etl::deque<etl::pair<int, int>, RECENT_SENSOR_COUNT> recent_sensor = etl::deque<etl::pair<int, int>, RECENT_SENSOR_COUNT>();
	bool sensor_table[Sensor::NUM_SENSOR_BYTES][CHAR_BIT] = { false };

	uint64_t idle_time, total_time;
	bool isIdleTimeModified = false;
	int char_count = 0;
	int ticks = 0;

	bool isSwitchStateModified = false;
	char switch_state[Train::NUM_SWITCHES];

	bool isTrainStateModified = false;
	Train::TerminalTrainStatus train_state[Train::NUM_TRAINS];

	// This is used to keep track of number of activated sensors

	auto trigger_print = [&]() {
		if (isRunning) {
			printing_index = 0;
			str_cpy(SAVE_CURSOR, printing_buffer, &printing_index, sizeof(SAVE_CURSOR) - 1);
			log_time(buf, ticks);
			str_cpy(buf, printing_buffer, &printing_index, 8);
			if (isIdleTimeModified) {
				isIdleTimeModified = false;
				uint64_t leading = idle_time * 100 / total_time;
				uint64_t trailing = (idle_time * 100000) / total_time % 1000;

				sprintf(buf, "\033[1;60HPercent: %llu.%03llu", leading, trailing);
				str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
			}

			if (isSensorModified) {
				isSensorModified = false;
				str_cpy(SENSOR_CURSOR, printing_buffer, &printing_index, sizeof(SENSOR_CURSOR) - 1);
				for (int i = 0; i < Sensor::NUM_SENSOR_BYTES; i++) {
					for (int j = 1; j <= CHAR_BIT; j++) {
						if (sensor_state[i] & (1 << (CHAR_BIT - j)) && !sensor_table[i][j - 1]) {
							if (recent_sensor.size() == recent_sensor.max_size()) {
								// Full deque, remove last element
								recent_sensor.pop_back();
							}
							recent_sensor.push_front(etl::pair<int, int> { i, j });
						}

						sensor_table[i][j - 1] = sensor_state[i] & (1 << (CHAR_BIT - j));
					}
				}

				// Print every sensor that has been activated
				for (auto& it : recent_sensor) {
					const char l = SENSOR_LETTERS[it.first / 2];
					int pos = CHAR_BIT * (it.first % 2);
					char ones = '0' + ((it.second + pos) % 10);
					char write[4] = { l, ((it.second + pos > 9) ? '1' : '0'), ones, ' ' };
					str_cpy(write, printing_buffer, &printing_index, 4);
				}

				str_cpy(RESET_CURSOR, printing_buffer, &printing_index, sizeof(RESET_CURSOR) - 1);
			}

			if (isSwitchStateModified) {
				isSwitchStateModified = false;
				int r;
				int c;
				for (uint64_t i = 0; i < sizeof(switch_state); i++) {
					sw_to_cursor_pos(i + 1, &r, &c);
					sprintf(buf, "\033[%d;%dH%c", r, c, switch_state[i]);
					str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
				}
			}

			if (isTrainStateModified) {
				isTrainStateModified = false;
				for (int i = 0; i < Train::NUM_TRAINS; ++i) {
					int train_num = Train::TRAIN_NUMBERS[i];
					etl::pair<int, int> pos = train_to_cursor_pos(train_num);
					int speed = train_state[i].speed;
					char dir = train_state[i].direction ? 'R' : 'S';
					sprintf(buf, "\033[%d;%dH%d%c ", pos.first, pos.second, speed, dir);
					str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
				}
			}

			str_cpy(RESTORE_CURSOR, printing_buffer, &printing_index, sizeof(RESTORE_CURSOR) - 1);
			if (printing_index >= UART::UART_MESSAGE_LIMIT) {
				Task::_KernelCrash("Too much printing, %d\r\n", printing_index);
			}
			UART::Puts(uart_0_server_tid, 0, printing_buffer, printing_index);
		}
	};

	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(TerminalServerReq));
		switch (req.header) {
		case RequestHeader::TERM_CLOCK: {
			// 100ms clock update
			Reply::Reply(from, nullptr, 0);
			ticks += 1;
			trigger_print();
			break;
		}
		case RequestHeader::TERM_SENSORS: {
			// Should be 10 bytes of sensor data.
			// Print out all the sensors, in a fancy UI way.
			Reply::Reply(from, nullptr, 0);
			for (int i = 0; i < Sensor::NUM_SENSOR_BYTES; i++) {
				sensor_state[i] = req.body.worker_msg.msg[i];
			}
			isSensorModified = true;
			break;
		}
		case RequestHeader::TERM_IDLE: {
			Reply::Reply(from, nullptr, 0);
			Clock::IdleStats(&idle_time, &total_time);
			isIdleTimeModified = true;
			break;
		}
		case RequestHeader::TERM_START: {
			Reply::Reply(from, nullptr, 0);
			printing_index = 0;
			str_cpy(CLEAR_SCREEN, printing_buffer, &printing_index, sizeof(CLEAR_SCREEN) - 1);
			str_cpy(TOP_LEFT, printing_buffer, &printing_index, sizeof(TOP_LEFT) - 1);
			str_cpy(SENSOR_DATA, printing_buffer, &printing_index, sizeof(SENSOR_DATA) - 1);
			for (int i = 0; i < Terminal::SWITCH_UI_LEN; ++i) {
				str_cpy(Terminal::SWITCH_UI[i], printing_buffer, &printing_index, UART::UART_MESSAGE_LIMIT, true);
			}

			UART::Puts(uart_0_server_tid, 0, printing_buffer, printing_index);
			printing_index = 0;

			str_cpy(SAVE_CURSOR, printing_buffer, &printing_index, sizeof(SAVE_CURSOR) - 1);
			for (int t = 0; t < Terminal::TRAIN_UI_LEN; ++t) {
				sprintf(buf, "\033[%d;%dH", TRAIN_PRINTOUT_ROW + t, TRAIN_PRINTOUT_COLUMN);
				str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
				str_cpy(Terminal::TRAIN_UI[t], printing_buffer, &printing_index, UART::UART_MESSAGE_LIMIT, true);

				UART::Puts(uart_0_server_tid, 0, printing_buffer, printing_index);
				printing_index = 0;
			}

			str_cpy(RESTORE_CURSOR, printing_buffer, &printing_index, sizeof(RESTORE_CURSOR) - 1);
			str_cpy(WELCOME_MSG, printing_buffer, &printing_index, sizeof(WELCOME_MSG) - 1);
			str_cpy(DELIMINATION, printing_buffer, &printing_index, sizeof(DELIMINATION) - 1);
			str_cpy(SAVE_CURSOR, printing_buffer, &printing_index, sizeof(SAVE_CURSOR) - 1);
			str_cpy(SETUP_SCROLL, printing_buffer, &printing_index, sizeof(SETUP_SCROLL) - 1);
			str_cpy(RESTORE_CURSOR, printing_buffer, &printing_index, sizeof(RESTORE_CURSOR) - 1);
			str_cpy(PROMPT, printing_buffer, &printing_index, sizeof(PROMPT) - 1);

			UART::Puts(uart_0_server_tid, 0, printing_buffer, printing_index);
			isRunning = true;
			break;
		}
		case RequestHeader::TERM_REVERSE_COMPLETE: {
			courier_pool.receive(from);
			break;
		}
		case RequestHeader::TERM_SWITCH: {
			Reply::Reply(from, nullptr, 0);
			for (uint64_t i = 0; i < sizeof(switch_state); i++) {
				switch_state[i] = req.body.worker_msg.msg[i];
			}
			isSwitchStateModified = true;
			break;
		}
		case RequestHeader::TERM_TRAIN_STATUS: {
			Reply::Reply(from, nullptr, 0);
			Train::TerminalTrainStatus* body = reinterpret_cast<Train::TerminalTrainStatus*>(req.body.worker_msg.msg);
			for (int i = 0; i < Train::NUM_TRAINS; ++i) {
				isTrainStateModified = isTrainStateModified || (train_state[i] != body[i]);
				train_state[i] = body[i];
			}
			break;
		}
		case RequestHeader::TERM_PUTC: {
			Reply::Reply(from, nullptr, 0);
			char c = req.body.regular_msg;
			if (char_count > CMD_LEN) {
				error_and_reset(LENGTH_ERROR, &char_count);
				UART::Puts(uart_0_server_tid, 0, PROMPT, sizeof(PROMPT) - 1);
			} else if (escape_status == TAState::TA_FOUND_ESCAPE) {
				escape_status = (c == '[') ? TAState::TA_FOUND_BRACKET : TAState::TA_DEFAULT_ARROW_STATE;
			} else if (escape_status == TAState::TA_FOUND_BRACKET) {
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
					char buf_3[3] = { '\033', '[', c };
					UART::Puts(uart_0_server_tid, 0, buf_3, 3);
				}
				}

				escape_status = TAState::TA_DEFAULT_ARROW_STATE;
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
						restart();
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
				escape_status = TAState::TA_FOUND_ESCAPE;
			} else {
				cmd_history[cmd_history_index].cmd[char_count++] = c;
				UART::Putc(uart_0_server_tid, 0, c);
			}
			break;
		}
		default: {
			Task::_KernelCrash("Illegal command passed to terminal admin: [%d]\r\n", req.header);
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
	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(TerminalCourierMessage));
		Reply::Reply(from, nullptr, 0); // unblock caller right away
		switch (req.header) {
		case RequestHeader::TERM_COUR_REV: {
			// it wait for about 4 seconds then send in the command to reverse and speed up
			req_to_train.header = RequestHeader::TRAIN_REV;
			req_to_train.body.id = req.body;
			Send::Send(train_tid, reinterpret_cast<char*>(&req_to_train), sizeof(req_to_train), nullptr, 0);
			req_to_admin = { RequestHeader::TERM_REVERSE_COMPLETE, '0' };
			Send::Send(terminal_tid, reinterpret_cast<char*>(&req_to_admin), sizeof(req_to_admin), nullptr, 0);
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
	Terminal::TerminalServerReq req = Terminal::TerminalServerReq(RequestHeader::TERM_CLOCK, internal_timer);

	while (true) {
		Send::Send(terminal_tid, reinterpret_cast<char*>(&req), sizeof(Terminal::TerminalServerReq), nullptr, 0);
		internal_timer += repeat;
		Clock::DelayUntil(clock_tid, internal_timer);
	}
}

void Terminal::sensor_query_courier() {
	Name::RegisterAs(TERMINAL_SENSOR_COURIER_NAME);

	int sensor_admin = Name::WhoIs(Sensor::SENSOR_ADMIN_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	Sensor::SensorAdminReq req;
	req.header = RequestHeader::GET_SENSOR_STATE;

	Terminal::TerminalServerReq treq;
	treq.header = RequestHeader::TERM_SENSORS;
	while (true) {
		Send::Send(sensor_admin, reinterpret_cast<char*>(&req), sizeof(Sensor::SensorAdminReq), treq.body.worker_msg.msg, Sensor::NUM_SENSOR_BYTES);
		Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq), nullptr, 0);
	}
}

void Terminal::idle_time_courier() {
	// technically idle_task will never need to yield to kernel
	// since there is no context swtich, only interrupt after this point
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);

	Terminal::TerminalServerReq treq;
	treq.header = RequestHeader::TERM_IDLE;

	while (true) {
		Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq), nullptr, 0);
		Clock::Delay(clock_tid, 200);
	}
}

void Terminal::user_input_courier() {
	Terminal::TerminalServerReq treq;
	treq.header = RequestHeader::TERM_START;

	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);

	UART::Getc(UART::UART_0_RECEIVER_TID, 0);
	Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq), nullptr, 0);
	treq.header = RequestHeader::TERM_PUTC;
	while (true) {
		treq.body.regular_msg = UART::Getc(UART::UART_0_RECEIVER_TID, 0);
		Send::Send(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq), nullptr, 0);
	}
}

void Terminal::switch_state_courier() {
	Terminal::TerminalServerReq req_to_terminal;
	Train::TrainAdminReq req_to_train;
	int train_admin = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int terminal_admin = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);

	req_to_train.header = RequestHeader::TRAIN_SWITCH_OBSERVE;
	req_to_terminal.header = RequestHeader::TERM_SWITCH;
	req_to_terminal.body.worker_msg.msg_len = Train::NUM_SWITCHES;
	int update_frequency = 100; // update once a second
	while (true) {
		Send::Send(train_admin, reinterpret_cast<char*>(&req_to_train), sizeof(Train::TrainAdminReq), req_to_terminal.body.worker_msg.msg, req_to_terminal.body.worker_msg.msg_len);
		Send::Send(terminal_admin, reinterpret_cast<char*>(&req_to_terminal), sizeof(Terminal::TerminalServerReq), nullptr, 0);
		Clock::Delay(clock_tid, update_frequency);
	}
}

void Terminal::train_state_courier() {
	Terminal::TerminalServerReq req_to_terminal;
	Train::TrainAdminReq req_to_train;
	int train_admin = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int terminal_admin = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);

	req_to_train.header = RequestHeader::TRAIN_OBSERVE;
	req_to_terminal.header = RequestHeader::TERM_TRAIN_STATUS;
	req_to_terminal.body.worker_msg.msg_len = Train::NUM_TRAINS * sizeof(Train::TrainStatus);
	int update_frequency = 100; // update once a second
	while (true) {
		Send::Send(train_admin, reinterpret_cast<char*>(&req_to_train), sizeof(Train::TrainAdminReq), req_to_terminal.body.worker_msg.msg, req_to_terminal.body.worker_msg.msg_len);
		Send::Send(terminal_admin, reinterpret_cast<char*>(&req_to_terminal), sizeof(Terminal::TerminalServerReq), nullptr, 0);
		Clock::Delay(clock_tid, update_frequency);
	}
}