#include "terminal_admin.h"
#include "../etl/circular_buffer.h"
#include "../etl/deque.h"
#include "../etl/queue.h"
#include "../server/local_pathing_server.h"
#include "../server/train_admin.h"
#include "../utils/buffer.h"
#include "../utils/printf.h"
#include "courier_pool.h"
#include <climits>
using namespace Terminal;
using namespace Message;

const int HANDLE_FAIL = -1;
const char SENSOR_LETTERS[] = "ABCDE";
constexpr char SPACES[] = "                                                                      ";

struct Command {
	char cmd[CMD_LEN];
	int len;
};

void str_cpy(const char* source, char* target, int* index, int len, bool check_null_char = false) {
	for (int i = 0; i < len && (!check_null_char || source[i] != '\0'); i++) {
		target[*index] = source[i];
		(*index) += 1;
	}
}

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


int handle_tr(AddressBook& addr, const char cmd[]) {
	int i = 3;
	int out_len = 0;
	int train = scan_int(cmd + i, &out_len, 2);
	if (Train::train_num_to_index(train) == Train::NO_TRAIN) {
		return HANDLE_FAIL;
	}

	i += out_len + 1;
	if (cmd[i - 1] != ' ')
		return HANDLE_FAIL;

	int speed = scan_int(cmd + i, &out_len, 2);
	if (speed == -1) {
		return HANDLE_FAIL;
	}

	i += out_len;
	for (; cmd[i] != '\r'; ++i) {
		if (cmd[i] != ' ') {
			return HANDLE_FAIL;
		}
	}

	Train::TrainAdminReq req;
	req.header = RequestHeader::TRAIN_SPEED;
	req.body.command.id = train;
	req.body.command.action = speed;
	Send::SendNoReply(addr.train_admin_tid, reinterpret_cast<char*>(&req), sizeof(req));

	return 0;
}

int handle_rv(Courier::CourierPool<TerminalCourierMessage>& pool, const char cmd[]) {
	int out_len = 0;
	int i = 3;
	int train = scan_int(cmd + i, &out_len, 2);
	if (Train::train_num_to_index(train) == Train::NO_TRAIN) {
		return HANDLE_FAIL;
	}

	i += out_len;
	for (; cmd[i] != '\r'; ++i) {
		if (cmd[i] != ' ') {
			return HANDLE_FAIL;
		}
	}

	TerminalCourierMessage req = { RequestHeader::TERM_COUR_REV, train };
	pool.request(&req);
	return 0;
}

int handle_sw(AddressBook& addr, const char cmd[]) {
	char snum = 0;
	char status = 0;
	int i = 3;

	int out_len = 0;
	int switch_num = scan_int(cmd + i, &out_len, 3);
	if (switch_num == READ_INT_FAIL || switch_num < 1) {
		return HANDLE_FAIL;
	} else {
		snum = switch_num;
	}

	i += out_len + 2;
	if (cmd[i - 2] != ' ')
		return HANDLE_FAIL;

	status = lower(cmd[i - 1]);
	if (status != 'c' && status != 's')
		return HANDLE_FAIL;

	for (; cmd[i] != '\r'; ++i) {
		if (cmd[i] != ' ') {
			return HANDLE_FAIL;
		}
	}

	Train::TrainAdminReq req;
	req.header = RequestHeader::TRAIN_SWITCH;
	req.body.command.id = snum;
	req.body.command.action = status;
	Send::SendNoReply(addr.train_admin_tid, reinterpret_cast<char*>(&req), sizeof(req));

	return 0;
}

/*
 * Handles a global pathing server command, which is one of:
 *  - go <train> <nodes...>
 *  - locate <train> <sensor>
 *  - init <train> <nodes...>
 * This is essentially a wrapper around sending a terminal courier and performing some error checking,
 * so it's pretty generic.
 */
int handle_global_pathing(Courier::CourierPool<TerminalCourierMessage>& pool, GenericCommand& cmd, RequestHeader header) {
	if (cmd.args.size() < 1 || (header == RequestHeader::TERM_COUR_LOCAL_LOCATE && cmd.args.size() != 1)) {
		return HANDLE_FAIL;
	}

	int train = cmd.args.front();
	TerminalCourierMessage req;
	req.header = header;
	int index = Train::train_num_to_index(train);
	if (index == Train::NO_TRAIN) {
		return HANDLE_FAIL;
	}

	req.body.courier_body.num_args = cmd.args.size();
	for (int i = 0; !cmd.args.empty(); ++i) {
		req.body.courier_body.args[i] = cmd.args.front();
		cmd.args.pop();
	}

	pool.request(&req);
	return 0;
}

GenericCommand handle_generic(const char cmd[]) {
	GenericCommand command = GenericCommand();
	int i = 0;

	for (; cmd[i] != ' ' && cmd[i] != '\r' && i < MAX_COMMAND_LEN - 1; ++i) {
		command.name[i] = cmd[i];
	}

	if (i == 0 || i == MAX_COMMAND_LEN - 1) {
		return command;
	}

	command.name[i] = '\0';
	while (cmd[i] == ' ') {
		i++;
	}

	int out_len = 0;
	while (cmd[i] != '\r') {
		int arg = scan_int(cmd + i, &out_len);
		if (arg == READ_INT_FAIL) {
			return command;
		}

		command.args.push(arg);
		i += out_len;
		while (cmd[i] == ' ') {
			i++;
		}
	}

	command.success = true;
	return command;
}

void set_switch(int train_server_tid, char snum, char status) {
	Train::TrainAdminReq req;
	req.header = RequestHeader::TRAIN_SWITCH;
	req.body.command.id = snum;
	req.body.command.action = status;
	Send::SendNoReply(train_server_tid, reinterpret_cast<char*>(&req), sizeof(req));
}

void Terminal::terminal_admin() {
	Name::RegisterAs(TERMINAL_ADMIN);
	AddressBook addr = getAddressBook();
	int from;
	TerminalServerReq req;

	Courier::CourierPool<TerminalCourierMessage> courier_pool
		= Courier::CourierPool<TerminalCourierMessage>(&terminal_courier, Priority::TERMINAL_PRIORITY);

	UART::Puts(addr.term_trans_tid, 0, START_PROMPT, sizeof(START_PROMPT) - 1);

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
	Train::TrainRaw train_state[Train::NUM_TRAINS];

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
					char dir = train_state[i].direction ? 'S' : 'R';
					sprintf(buf, "\033[%d;%dH%d%c ", pos.first, pos.second, speed, dir);
					str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
				}
			}

			str_cpy(RESTORE_CURSOR, printing_buffer, &printing_index, sizeof(RESTORE_CURSOR) - 1);
			if (printing_index >= UART::UART_MESSAGE_LIMIT) {
				Task::_KernelCrash("Too much printing, %d\r\n", printing_index);
			}
			UART::Puts(addr.term_trans_tid, 0, printing_buffer, printing_index);
		}
	};

	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(TerminalServerReq));
		switch (req.header) {
		case RequestHeader::TERM_CLOCK: {
			// 100ms clock update
			Reply::EmptyReply(from);
			ticks += 1;
			trigger_print();
			break;
		}
		case RequestHeader::TERM_SENSORS: {
			// Should be 10 bytes of sensor data.
			// Print out all the sensors, in a fancy UI way.
			Reply::EmptyReply(from);
			for (int i = 0; i < Sensor::NUM_SENSOR_BYTES; i++) {
				sensor_state[i] = req.body.worker_msg.msg[i];
			}
			isSensorModified = true;
			break;
		}
		case RequestHeader::TERM_IDLE: {
			Reply::EmptyReply(from);
			Clock::IdleStats(&idle_time, &total_time);
			isIdleTimeModified = true;
			break;
		}
		case RequestHeader::TERM_START: {
			Reply::EmptyReply(from);
			printing_index = 0;
			str_cpy(CLEAR_SCREEN, printing_buffer, &printing_index, sizeof(CLEAR_SCREEN) - 1);
			str_cpy(TOP_LEFT, printing_buffer, &printing_index, sizeof(TOP_LEFT) - 1);

			int terminal_admin_tid = Task::MyTid();
			int len = sprintf(buf, TERM_A_TID_MSG, terminal_admin_tid);
			str_cpy(buf, printing_buffer, &printing_index, len);

			str_cpy(SENSOR_DATA, printing_buffer, &printing_index, sizeof(SENSOR_DATA) - 1);
			for (int i = 0; i < Terminal::SWITCH_UI_LEN; ++i) {
				str_cpy(Terminal::SWITCH_UI[i], printing_buffer, &printing_index, UART::UART_MESSAGE_LIMIT, true);
			}

			UART::Puts(addr.term_trans_tid, 0, printing_buffer, printing_index);
			printing_index = 0;

			str_cpy(SAVE_CURSOR_NO_JUMP, printing_buffer, &printing_index, sizeof(SAVE_CURSOR_NO_JUMP) - 1);
			for (int t = 0; t < Terminal::TRAIN_UI_LEN; ++t) {
				sprintf(buf, "\033[%d;%dH", TRAIN_PRINTOUT_ROW + t, TRAIN_PRINTOUT_COLUMN);
				str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
				str_cpy(Terminal::TRAIN_UI[t], printing_buffer, &printing_index, UART::UART_MESSAGE_LIMIT, true);

				UART::Puts(addr.term_trans_tid, 0, printing_buffer, printing_index);
				printing_index = 0;
			}

			str_cpy(RESTORE_CURSOR, printing_buffer, &printing_index, sizeof(RESTORE_CURSOR) - 1);
			str_cpy(WELCOME_MSG, printing_buffer, &printing_index, sizeof(WELCOME_MSG) - 1);
			str_cpy(DELIMINATION, printing_buffer, &printing_index, sizeof(DELIMINATION) - 1);
			str_cpy(SAVE_CURSOR_NO_JUMP, printing_buffer, &printing_index, sizeof(SAVE_CURSOR_NO_JUMP) - 1);

			len = sprintf(buf, SETUP_SCROLL, SCROLL_TOP, SCROLL_BOTTOM);
			str_cpy(buf, printing_buffer, &printing_index, len);
			str_cpy(RESTORE_CURSOR, printing_buffer, &printing_index, sizeof(RESTORE_CURSOR) - 1);
			str_cpy("\r\n", printing_buffer, &printing_index, 2);
			str_cpy(PROMPT, printing_buffer, &printing_index, sizeof(PROMPT) - 1);
			str_cpy("\r\n\r\n", printing_buffer, &printing_index, 4);
			str_cpy(DELIMINATION, printing_buffer, &printing_index, sizeof(DELIMINATION) - 1);
			str_cpy("\r\n", printing_buffer, &printing_index, 2);
			str_cpy(DEBUG_TITLE, printing_buffer, &printing_index, sizeof(DEBUG_TITLE) - 1);
			str_cpy(HIDE_CURSOR, printing_buffer, &printing_index, sizeof(HIDE_CURSOR) - 1);

			UART::Puts(addr.term_trans_tid, 0, printing_buffer, printing_index);
			isRunning = true;
			break;
		}
		case RequestHeader::TERM_REVERSE_COMPLETE: {
			courier_pool.receive(from);
			break;
		}
		case RequestHeader::TERM_LOCAL_COMPLETE: {
			courier_pool.receive(from);
			break;
		}
		case RequestHeader::TERM_SWITCH: {
			Reply::EmptyReply(from);
			for (uint64_t i = 0; i < sizeof(switch_state); i++) {
				switch_state[i] = req.body.worker_msg.msg[i];
			}
			isSwitchStateModified = true;
			break;
		}
		case RequestHeader::TERM_TRAIN_STATUS: {
			Reply::EmptyReply(from);
			Train::TrainRaw* body = reinterpret_cast<Train::TrainRaw*>(req.body.worker_msg.msg);
			for (int i = 0; i < Train::NUM_TRAINS; ++i) {
				isTrainStateModified = isTrainStateModified || (train_state[i] != body[i]);
				train_state[i] = body[i];
			}
			break;
		}
		case RequestHeader::TERM_PUTC: {
			Reply::EmptyReply(from);
			char c = req.body.regular_msg;
			int result = 0;
			int printing_index = 0;

			str_cpy(SAVE_CURSOR_NO_JUMP, printing_buffer, &printing_index, sizeof(SAVE_CURSOR_NO_JUMP) - 1);
			sprintf(buf, PROMPT_CURSOR, sizeof(PROMPT_NNL) + char_count);
			str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);

			if (char_count > CMD_LEN) {
				sprintf(buf, "\033M\r%s", ERROR);
				str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
				char_count = 0;
				str_cpy(PROMPT_NNL, printing_buffer, &printing_index, sizeof(PROMPT_NNL) - 1);

			} else if (escape_status == TAState::TA_FOUND_ESCAPE) {
				escape_status = (c == '[') ? TAState::TA_FOUND_BRACKET : TAState::TA_DEFAULT_ARROW_STATE;
			} else if (escape_status == TAState::TA_FOUND_BRACKET) {
				switch (c) {
				case 'A': { // up arrow
					if (cmd_history_index > 0) {
						cmd_history_index--;
						str_cpy("\r", printing_buffer, &printing_index, 1);
						if (char_count > 0) {
							str_cpy(SPACES, printing_buffer, &printing_index, char_count + sizeof(PROMPT_NNL) - 1);
							str_cpy("\r", printing_buffer, &printing_index, 1);
						}
						str_cpy(PROMPT_NNL, printing_buffer, &printing_index, sizeof(PROMPT_NNL) - 1);
						str_cpy(cmd_history[cmd_history_index].cmd, printing_buffer, &printing_index, cmd_history[cmd_history_index].len);

						char_count = cmd_history[cmd_history_index].len;
					}
					break;
				}
				case 'B': { // down arrow
					if (cmd_history_index < cmd_history.size() - 1) {
						cmd_history_index++;
						str_cpy("\r", printing_buffer, &printing_index, 1);
						if (char_count > 0) {
							str_cpy(SPACES, printing_buffer, &printing_index, char_count + sizeof(PROMPT_NNL) - 1);
							str_cpy("\r", printing_buffer, &printing_index, 1);
						}

						str_cpy(PROMPT_NNL, printing_buffer, &printing_index, sizeof(PROMPT_NNL) - 1);
						str_cpy(cmd_history[cmd_history_index].cmd, printing_buffer, &printing_index, cmd_history[cmd_history_index].len);

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
					str_cpy(buf_3, printing_buffer, &printing_index, 3);
				}
				}

				escape_status = TAState::TA_DEFAULT_ARROW_STATE;
			} else if (c == '\b') {
				if (char_count > 0) {
					cmd_history[cmd_history_index].cmd[char_count] = '\0';
					char_count--;
					str_cpy("\b \b", printing_buffer, &printing_index, 3);
				}
			} else if (c == '\r') {
				cmd_history[cmd_history_index].cmd[char_count] = '\r';
				GenericCommand cmd_parsed = handle_generic(cmd_history[cmd_history_index].cmd);

				// Restore the cursor so functions can use debug printing unimpeded
				// UART::PutsNullTerm(addr.term_trans_tid, 0, RESTORE_CURSOR, sizeof(RESTORE_CURSOR) - 1);

				if (strncmp(cmd_parsed.name, "tr", MAX_COMMAND_LEN) == 0) {
					result = handle_tr(addr, cmd_history[cmd_history_index].cmd);
				} else if (strncmp(cmd_parsed.name, "rv", MAX_COMMAND_LEN) == 0) {
					result = handle_rv(courier_pool, cmd_history[cmd_history_index].cmd);
				} else if (strncmp(cmd_parsed.name, "sw", MAX_COMMAND_LEN) == 0) {
					result = handle_sw(addr, cmd_history[cmd_history_index].cmd);
				} else if (strncmp(cmd_parsed.name, "q", MAX_COMMAND_LEN) == 0) {
					int i = 1;
					while (cmd_history[cmd_history_index].cmd[i] == ' ' && i < CMD_LEN) {
						++i;
					}

					if (cmd_history[cmd_history_index].cmd[i] == '\r') {
						UART::Puts(addr.term_trans_tid, 0, QUIT, sizeof(QUIT) - 1);
						for (int k = 0; k < Train::NUM_TRAINS; ++k) {
							// Stop all trains
							// set_train_speed(train_logic_server_tid, Train::TRAIN_NUMBERS[k], 0);
						}

						restart();
					} else {
						result = HANDLE_FAIL;
					}
					// } else if (strncmp(cmd_parsed.name, "clear", MAX_COMMAND_LEN) == 0) {
					// 	for (int r = SCROLL_BOTTOM; r >= SCROLL_TOP; --r) {
					// 		int len = sprintf(buf, MOVE_CURSOR_F, r, 1);
					// 		UART::Puts(addr.term_trans_tid, 0, buf, len);
					// 		UART::Puts(addr.term_trans_tid, 0, CLEAR_LINE, sizeof(CLEAR_LINE) - 1);
					// 		Clock::Delay(addr.clock_tid, 1);
					// 	}
				} else if (!cmd_parsed.success) {
					result = HANDLE_FAIL;
				} else if (strncmp(cmd_parsed.name, "go", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_GO);
				} else if (strncmp(cmd_parsed.name, "locate", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_LOCATE);
				} else if (strncmp(cmd_parsed.name, "loop", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_LOOP);
				} else if (strncmp(cmd_parsed.name, "exloop", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_EXLOOP);
				} else if (strncmp(cmd_parsed.name, "init", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_INIT);
				} else if (strncmp(cmd_parsed.name, "cali", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_CALI); // not working
				} else if (strncmp(cmd_parsed.name, "base", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_CALI_BASE_SPEED);
				} else if (strncmp(cmd_parsed.name, "accele", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_CALI_ACCELERATION);
				} else if (strncmp(cmd_parsed.name, "sdist", MAX_COMMAND_LEN) == 0) {
					result = handle_global_pathing(courier_pool, cmd_parsed, RequestHeader::TERM_COUR_LOCAL_CALI_STOPPING_DIST);
				} else {
					result = HANDLE_FAIL;
				}

				cmd_history[cmd_history_index].len = char_count;
				char_count = 0;
				cmd_history.push(Command { { 0 }, 0 });
				if (cmd_history_index < cmd_history.max_size() - 1) {
					cmd_history_index++;
				}

				// Have a block of code here to save the cursor so the terminal can print regular stuff
				// Clock::Delay(addr.clock_tid, 5);
				// UART::Puts(addr.term_trans_tid, 0, SAVE_CURSOR_NO_JUMP, sizeof(SAVE_CURSOR_NO_JUMP) - 1);
				// sprintf(buf, PROMPT_CURSOR, sizeof(PROMPT_NNL) + char_count);
				// UART::PutsNullTerm(addr.term_trans_tid, 0, buf, TERM_A_BUFLEN);

				if (result == HANDLE_FAIL) {
					sprintf(buf, "\033M\r%s", ERROR);
					str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
				} else {
					sprintf(buf, "\033M\r%s\r\n", CLEAR_LINE);
					str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
				}

				sprintf(buf, "%s%s", CLEAR_LINE, PROMPT_NNL);
				str_cpy(buf, printing_buffer, &printing_index, TERM_A_BUFLEN, true);
			} else if (c == '\033') {
				// Escape sequence. Try to read an arrow key.
				escape_status = TAState::TA_FOUND_ESCAPE;
			} else {
				cmd_history[cmd_history_index].cmd[char_count++] = c;
				str_cpy(&c, printing_buffer, &printing_index, 1);
			}
			str_cpy(RESTORE_CURSOR, printing_buffer, &printing_index, sizeof(RESTORE_CURSOR) - 1);
			UART::Puts(addr.term_trans_tid, 0, printing_buffer, printing_index);

			break;
		}
		case RequestHeader::TERM_DEBUG_PUTS: {
			Message::Reply::EmptyReply(from);
			UART::Puts(addr.term_trans_tid, 0, req.body.worker_msg.msg, req.body.worker_msg.msg_len);
			break;
		}
		default: {
			Task::_KernelCrash("Illegal command passed to terminal admin: [%d]\r\n", req.header);
		}
		}
	}
}

void Terminal::terminal_courier() {

	AddressBook addr = Message::getAddressBook();
	Terminal::TerminalCourierMessage req;
	Terminal::TerminalServerReq req_to_admin;
	Train::TrainAdminReq req_to_train;
	LocalPathing::LocalPathingReq req_to_local_pathing;

	auto local_server_redirect = [&](RequestHeader header) {
		req_to_local_pathing.header = header;
		req_to_local_pathing.body.command.num_args = req.body.courier_body.num_args - 1;
		for (uint32_t i = 1; i < req.body.courier_body.num_args; ++i) {
			req_to_local_pathing.body.command.args[i - 1] = req.body.courier_body.args[i];
		}

		int train = req.body.courier_body.args[0];
		int index = Train::train_num_to_index(train);
		int local_pathing_tid = addr.local_pathing_tids[index];
		Send::SendNoReply(local_pathing_tid, reinterpret_cast<char*>(&req_to_local_pathing), sizeof(req_to_local_pathing));

		req_to_admin = { RequestHeader::TERM_LOCAL_COMPLETE, '0' };
		Send::SendNoReply(addr.terminal_admin_tid, reinterpret_cast<char*>(&req_to_admin), sizeof(req_to_admin));
	};

	int from;
	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(TerminalCourierMessage));
		Reply::EmptyReply(from); // unblock caller right away
		switch (req.header) {
		case RequestHeader::TERM_COUR_REV: {
			// it wait for about 4 seconds then send in the command to reverse and speed up
			req_to_train.header = RequestHeader::TRAIN_REV;
			req_to_train.body.command.id = req.body.regular_body;
			Send::SendNoReply(addr.train_admin_tid, reinterpret_cast<char*>(&req_to_train), sizeof(req_to_train));
			req_to_admin = { RequestHeader::TERM_REVERSE_COMPLETE, '0' };
			Send::SendNoReply(addr.terminal_admin_tid, reinterpret_cast<char*>(&req_to_admin), sizeof(req_to_admin));
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_GO: {
			local_server_redirect(RequestHeader::LOCAL_PATH_SET_PATH);
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_LOCATE: {
			local_server_redirect(RequestHeader::LOCAL_PATH_LOCATE);
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_LOOP: {
			local_server_redirect(RequestHeader::LOCAL_PATH_LOOP);
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_EXLOOP: {
			local_server_redirect(RequestHeader::LOCAL_PATH_EXLOOP);
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_INIT: {
			local_server_redirect(RequestHeader::LOCAL_PATH_INIT);
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_CALI: {
			local_server_redirect(RequestHeader::LOCAL_PATH_CALI);
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_CALI_BASE_SPEED: {
			local_server_redirect(RequestHeader::LOCAL_PATH_CALI_BASE_SPEED);
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_CALI_ACCELERATION: {
			local_server_redirect(RequestHeader::LOCAL_PATH_CALI_ACCELERATION);
			break;
		}
		case RequestHeader::TERM_COUR_LOCAL_CALI_STOPPING_DIST: {
			local_server_redirect(RequestHeader::LOCAL_PATH_CALI_STOPPING_DISTANCE);
			break;
		}
		default: {
			Task::_KernelCrash("Term_A Train Courier illegal type: [%d]\r\n", req.header);
		}
		} // switch
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
		Send::SendNoReply(terminal_tid, reinterpret_cast<char*>(&req), sizeof(Terminal::TerminalServerReq));
		internal_timer += repeat;
		Clock::DelayUntil(clock_tid, internal_timer);
	}
}

void Terminal::sensor_query_courier() {
	Name::RegisterAs(TERMINAL_SENSOR_COURIER_NAME);

	int sensor_admin = Name::WhoIs(Sensor::SENSOR_ADMIN_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	Sensor::SensorAdminReq req;
	req.header = RequestHeader::SENSOR_AWAIT_STATE;

	Terminal::TerminalServerReq treq;
	treq.header = RequestHeader::TERM_SENSORS;
	while (true) {
		Send::Send(sensor_admin, reinterpret_cast<char*>(&req), sizeof(Sensor::SensorAdminReq), treq.body.worker_msg.msg, Sensor::NUM_SENSOR_BYTES);
		Send::SendNoReply(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq));
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
		Send::SendNoReply(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq));
		Clock::Delay(clock_tid, 200);
	}
}

void Terminal::user_input_courier() {
	Terminal::TerminalServerReq treq;
	treq.header = RequestHeader::TERM_START;

	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);

	UART::Getc(UART::UART_0_RECEIVER_TID, 0);
	Send::SendNoReply(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq));
	treq.header = RequestHeader::TERM_PUTC;
	while (true) {
		treq.body.regular_msg = UART::Getc(UART::UART_0_RECEIVER_TID, 0);
		Send::SendNoReply(terminal_tid, reinterpret_cast<char*>(&treq), sizeof(Terminal::TerminalServerReq));
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
		Send::Send(train_admin,
				   reinterpret_cast<char*>(&req_to_train),
				   sizeof(Train::TrainAdminReq),
				   req_to_terminal.body.worker_msg.msg,
				   req_to_terminal.body.worker_msg.msg_len);
		Send::SendNoReply(terminal_admin, reinterpret_cast<char*>(&req_to_terminal), sizeof(Terminal::TerminalServerReq));
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
	req_to_terminal.body.worker_msg.msg_len = Train::NUM_TRAINS * sizeof(Train::TrainRaw);
	int update_frequency = 100; // update once a second
	while (true) {
		Send::Send(train_admin,
				   reinterpret_cast<char*>(&req_to_train),
				   sizeof(Train::TrainAdminReq),
				   req_to_terminal.body.worker_msg.msg,
				   req_to_terminal.body.worker_msg.msg_len);
		Send::SendNoReply(terminal_admin, reinterpret_cast<char*>(&req_to_terminal), sizeof(Terminal::TerminalServerReq));
		Clock::Delay(clock_tid, update_frequency);
	}
}