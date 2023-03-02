#include "terminal_admin.h"
#include "../utils/buffer.h"
#include "../utils/printf.h"

using namespace Terminal;
using namespace Message;

const char SENSOR_LETTERS[] = "ABCDE";
const char MOVE_CURSOR[] = "\033[r;cH";
constexpr char SPACES[] = "                                ";

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

void Terminal::terminal_puts(const char* msg, int clock_server_tid, int delay) {
	for (int i = 0; msg[i] != '\0'; i++) {
		UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, msg[i]);
	}

	// int len = 0;
	// while (msg[len] != '\0') {
	// 	len++;
	// }

	// UART::Puts(UART::UART_0_TRANSMITTER_TID, 0, msg, len);
	Clock::Delay(clock_server_tid, delay);
}

void Terminal::terminal_admin() {
	Name::RegisterAs(TERMINAL_ADMIN);
	int from;
	int ticks = 0;
	TerminalServerReq req;
	char buf[16];

	int clock_server_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);

	int tid = Task::MyTid();
	sprintf(buf, "T-A TID: %d\r\n", tid);
	terminal_puts(buf, clock_server_tid);

	// This is used to keep track of number of activated sensors
	int prev_on = 0;

	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(TerminalServerReq));
		switch (req.header) {
		case RequestHeader::PUTC: {
			UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, req.body.regular_msg);
			break;
		}
		case RequestHeader::PUTS: {
			terminal_puts(req.body.worker_msg.msg, clock_server_tid, 0);
			break;
		}
		case RequestHeader::CLOCK: {
			// 100ms clock update
			ticks += 1;
			terminal_puts(SAVE_CURSOR, clock_server_tid, 0);
			terminal_puts(TOP_LEFT, clock_server_tid, 0);
			log_time(buf, ticks);
			terminal_puts(buf, clock_server_tid, 0);
			terminal_puts(RESTORE_CURSOR, clock_server_tid);

			break;
		}
		case RequestHeader::SENSORS: {
			// Should be 10 bytes of sensor data.
			// Print out all the sensors, in a fancy UI way.
			const char* sensor_data = req.body.worker_msg.msg;

			terminal_puts(SAVE_CURSOR, clock_server_tid, 0);
			terminal_puts(SENSOR_CURSOR, clock_server_tid, 0);
			terminal_puts(RED_CURSOR, clock_server_tid, 0);

			// Print out the sensor data
			int curr_on = 0;
			for (int i = 0; i < NUM_SENSOR_BYTES; i++) {
				const char l = SENSOR_LETTERS[i / 2];
				int pos = 8 * (i % 2);
				for (int j = 1; j <= 8; j++) {
					if (sensor_data[i] & (1 << (8 - j))) {
						char tens = (j + pos > 9) ? '1' : '0';
						char ones = '0' + (j + pos) % 10;

						UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, l);
						UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, tens);
						UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, ones);
						UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, ' ');
						curr_on += 1;
					}
				}
			}

			// Clear out old sensor data
			if (curr_on < prev_on) {
				terminal_puts(SPACES, clock_server_tid, 0);
			}

			terminal_puts(RESET_CURSOR, clock_server_tid, 0);
			terminal_puts(RESTORE_CURSOR, clock_server_tid, 1);
			prev_on = curr_on;

			break;
		}
		case RequestHeader::SWITCH: {
			// Update the switch diplay.

			int snum = req.body.worker_msg.msg[0];
			char status = req.body.worker_msg.msg[1];

			int row, col;
			char buf[10] = { 0 };
			terminal_puts(SAVE_CURSOR, clock_server_tid, 0);
			sw_to_cursor_pos(snum, &row, &col);
			size_t len = 0;
			move_cursor(row, col, buf, &len);
			terminal_puts(buf, clock_server_tid, 0);
			UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, status);
			terminal_puts(RESTORE_CURSOR, clock_server_tid);
			break;
		}
		default: {
			break;
		}
		}

		Reply::Reply(from, nullptr, 0);
	}
}