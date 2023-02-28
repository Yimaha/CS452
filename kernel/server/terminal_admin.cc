#include "terminal_admin.h"
#include "../utils/printf.h"

using namespace Terminal;
using namespace Message;

const char SENSOR_LETTERS[] = "ABCDE";
const char MOVE_CURSOR[] = "\033[r;cH";

void log_time(char buf[], const uint32_t ticks) {
	char to = '0' + ticks % 10;

	uint32_t seconds = ticks / 10;
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

void Terminal::terminal_puts(const char* msg) {
	for (int i = 0; msg[i] != '\0'; i++) {
		UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, msg[i]);
	}
}

void Terminal::terminal_admin() {
	Name::RegisterAs(TERMINAL_ADMIN);
	int from;
	int ticks = 0;
	TerminalServerReq req;
	char buf[16];
	int buflen = 0;

	int tid = Task::MyTid();
	sprintf(buf, "T-A TID: %d\r\n", tid);
	terminal_puts(buf);

	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(TerminalServerReq));
		switch (req.header) {
		case RequestHeader::PUTC: {
			UART::Putc(UART::UART_0_TRANSMITTER_TID, 0, req.body.regular_msg);
			break;
		}
		case RequestHeader::CLOCK: {
			// 100ms clock update
			ticks += 1;
			terminal_puts(SAVE_CURSOR);
			terminal_puts(TOP_LEFT);
			log_time(buf, ticks);
			terminal_puts(buf);
			terminal_puts(RESTORE_CURSOR);

			break;
		}
		case RequestHeader::SENSORS: {
			// Should be 10 bytes of sensor data.
			// Print out all the sensors, in a fancy UI way.
			break;
		}
		case RequestHeader::SWITCH: {
			// Update the switch diplay.
			break;
		}
		default: {
			break;
		}
		}

		Reply::Reply(from, nullptr, 0);
	}
}