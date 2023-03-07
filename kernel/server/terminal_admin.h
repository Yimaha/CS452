#pragma once

#include "../kernel.h"
#include "../server/sensor_admin.h"
#include "../utils/utility.h"
#include "request_header.h"

namespace Terminal
{

constexpr char TERMINAL_ADMIN[] = "TERMINAL_ADMIN";
constexpr char TERMINAL_CLOCK_COURIER_NAME[] = "TERMINAL_CLOCK";
constexpr char TERMINAL_SWITCH_COURIER_NAME[] = "TERMINAL_CLOCK";
constexpr char TERMINAL_SENSOR_COURIER_NAME[] = "TERMINAL_SENSOR";
constexpr char TERMINAL_PRINTER_NAME[] = "TERMINAL_PRINT";
constexpr char IDLE_TIME_TASK_NAME[] = "IDLE_TIME_TASK";

constexpr char CLEAR_SCREEN[] = "\033[2J";
constexpr char TOP_LEFT[] = "\033[H";
constexpr char RESET_CURSOR[] = "\033[0m";
constexpr char SAVE_CURSOR[] = "\033[s\033[H";
constexpr char RESTORE_CURSOR[] = "\033[u";
constexpr char SENSOR_CURSOR[] = "\033[5;1H\033[31m";
constexpr char RED_CURSOR[] = "\033[31m";

constexpr char START_PROMPT[] = "Press any key to enter OS mode\r\n";
constexpr char SENSOR_DATA[] = "\r\n\r\n\r\nRECENT SENSOR DATA:\r\n\r\n\r\n";
constexpr char WELCOME_MSG[] = "Welcome to AbyssOS!\r\n";
constexpr char SWITCH_UI_L0[] = "SWITCHES:\r\n";
constexpr char PROMPT[] = "\r\nABYSS> ";
constexpr char PROMPT_NNL[] = "ABYSS> ";
constexpr char SETUP_SCROLL[] = "\033[19;50r";

const char ERROR[] = "ERROR: INVALID COMMAND\r\n";
const char LENGTH_ERROR[] = "ERROR: COMMAND TOO LONG\r\n";
const char QUIT[] = "\r\nQUITTING...\r\n\r\n";

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

const char TRAIN_UI_L0[] = "                 _-====-__-======-__-========-_____-============-__\r\n";
const char TRAIN_UI_L1[] = "               _(                                                 _)\r\n";
const char TRAIN_UI_L2[] = "            OO(     1:              24:              74:          )_\r\n";
const char TRAIN_UI_L3[] = "           0  (_    2:              58:              78:           _)\r\n";
const char TRAIN_UI_L4[] = "         o0     (_                                                _)\r\n";
const char TRAIN_UI_L5[] = "        o         '=-___-===-_____-========-___________-===-dwb-='\r\n";
const char TRAIN_UI_L6[] = "      .o\r\n";
const char TRAIN_UI_L7[] = "     . ______          :::::::::::::::::: :::::::::::::::::: __|-----|__\r\n";
const char TRAIN_UI_L8[] = "   _()_||__|| --++++++ |[][][][][][][][]| |[][][][][][][][]| |  [] []  |\r\n";
const char TRAIN_UI_L9[] = "  (BNSF 1995|;|______|;|________________|;|________________|;|_________|;\r\n";
const char TRAIN_UI_L10[] = " /-OO----OO    oo  oo   oo oo      oo oo   oo oo      oo oo   oo     oo\r\n";
const char TRAIN_UI_L11[] = "#########################################################################\r\n";
const char* const TRAIN_UI[] = { TRAIN_UI_L0, TRAIN_UI_L1, TRAIN_UI_L2, TRAIN_UI_L3, TRAIN_UI_L4, TRAIN_UI_L5, TRAIN_UI_L6, TRAIN_UI_L7, TRAIN_UI_L8, TRAIN_UI_L9, TRAIN_UI_L10, TRAIN_UI_L11 };

const char DELIMINATION[] = "================================================================";

const int TERM_A_BUFLEN = 100;
const int SWITCH_UI_LEN = 10;
const int TRAIN_UI_LEN = 12;
const int TRAIN_PRINTOUT_ROW = 5;
const int TRAIN_PRINTOUT_COLUMN = 45;
const int TRAIN_PRINTOUT_FIRST = 68;
const int TRAIN_PRINTOUT_WIDTH = 17;
const int RECENT_SENSOR_COUNT = 10;
const int MAX_PUTS_LEN = 64;

const int CLOCK_UPDATE_FREQUENCY = 10;
constexpr int CMD_LEN = 64;
constexpr int CMD_HISTORY_LEN = 128;
enum class TAState : uint32_t { TA_DEFAULT_ARROW_STATE, TA_FOUND_ESCAPE, TA_FOUND_BRACKET };
void terminal_admin();
void terminal_courier();
void terminal_clock_courier();
void sensor_query_courier();
void idle_time_courier();
void user_input_courier();
void switch_state_courier();
void train_state_courier();

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
	Message::RequestHeader header = Message::RequestHeader::NONE;
	RequestBody body = { '0' };

	TerminalServerReq() { }

	TerminalServerReq(Message::RequestHeader h, char b) {
		header = h;
		body.regular_msg = b;
	}

	TerminalServerReq(Message::RequestHeader h, WorkerRequestBody worker_msg) {
		header = h;
		body.worker_msg = worker_msg;
	}

} __attribute__((aligned(8)));


struct TerminalCourierMessage {
	Message::RequestHeader header = Message::RequestHeader::NONE;
	int body;

} __attribute__((aligned(8)));
}