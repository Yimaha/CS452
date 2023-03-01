#pragma once

#include "../kernel.h"
#include "../utils/utility.h"

namespace Terminal
{

constexpr char TERMINAL_ADMIN[] = "TERMINAL_ADMIN";
constexpr char CLEAR_SCREEN[] = "\x1b[2J";
constexpr char TOP_LEFT[] = "\x1b[H";
constexpr char RESET_CURSOR[] = "\033[0m";
constexpr char SAVE_CURSOR[] = "\0337";
constexpr char RESTORE_CURSOR[] = "\0338";
constexpr char SENSOR_CURSOR[] = "\033[5;1H";
constexpr char RED_CURSOR[] = "\033[31m";

const char SWITCH_UI_L0[] = "SWITCHES:\r\n";
const char SWITCH_UI_L1[] = "+-----+-----+-----+-----+-----+-----+\r\n";
const char SWITCH_UI_L2[] = "|01 $ |02 $ |03 $ |04 $ |05 $ |06 $ |\r\n";
const char SWITCH_UI_L3[] = "+-----+-----+-----+-----+-----+-----+\r\n";
const char SWITCH_UI_L4[] = "|07 $ |08 $ |09 $ |10 $ |11 $ |12 $ |\r\n";
const char SWITCH_UI_L5[] = "+-----+-----+-----+-----+-----+-----+\r\n";
const char SWITCH_UI_L6[] = "|13 $ |14 $ |15 $ |16 $ |17 $ |18 $ |\r\n";
const char SWITCH_UI_L7[] = "=====================================\r\n";
const char SWITCH_UI_L8[] = "|0x99  $ |0x9A  $ |0x9B  $ |0x9C  $ |\r\n";
const char SWITCH_UI_L9[] = "+--------+--------+--------+--------+\r\n";
const char* const SWITCH_UI[] = { SWITCH_UI_L0, SWITCH_UI_L1, SWITCH_UI_L2, SWITCH_UI_L3, SWITCH_UI_L4, SWITCH_UI_L5, SWITCH_UI_L6, SWITCH_UI_L7, SWITCH_UI_L8, SWITCH_UI_L9 };

const int SWITCH_UI_LEN = 10;
const int NUM_SENSOR_BYTES = 10;
const int MAX_PUTS_LEN = 64;

void terminal_puts(const char* msg, int clock_server_tid, int delay = 1);
void terminal_admin();

enum class RequestHeader : uint32_t { NONE, PUTC, PUTS, CLOCK, SENSORS, SWITCH };

struct WorkerRequestBody {
	uint64_t msg_len = 0;
	char msg[MAX_PUTS_LEN];
};

union RequestBody
{
	char regular_msg;
	WorkerRequestBody worker_msg;
};

struct TerminalServerReq {
	RequestHeader header = RequestHeader::NONE;
	RequestBody body = { '0' };

	TerminalServerReq() { }

	TerminalServerReq(RequestHeader h, char b) {
		header = h;
		body.regular_msg = b;
	}

	TerminalServerReq(RequestHeader h, WorkerRequestBody worker_msg) {
		header = h;
		body.worker_msg = worker_msg;
	}

} __attribute__((aligned(8)));
}