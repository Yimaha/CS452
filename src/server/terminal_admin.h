#pragma once

#include "../etl/queue.h"
#include "../kernel.h"
#include "../utils/utility.h"
#include "request_header.h"
#include "sensor_admin.h"

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
constexpr char HIDE_CURSOR[] = "\033[?25l";
constexpr char RESET_CURSOR[] = "\033[0m";
constexpr char SAVE_CURSOR[] = "\033[s\033[H";
constexpr char SAVE_CURSOR_NO_JUMP[] = "\033[s";
constexpr char RESTORE_CURSOR[] = "\033[u";
constexpr char SENSOR_CURSOR[] = "\033[5;1H";
constexpr char BLACK_CURSOR[] = "\033[30m";
constexpr char RED_CURSOR[] = "\033[31m";
constexpr char GREEN_CURSOR[] = "\033[32m";
constexpr char YELLOW_CURSOR[] = "\033[33m";
constexpr char BLUE_CURSOR[] = "\033[34m";
constexpr char MAGENTA_CURSOR[] = "\033[35m";
constexpr char CYAN_CURSOR[] = "\033[36m";
constexpr char WHITE_CURSOR[] = "\033[37m";
constexpr char CLEAR_LINE[] = "\r\033[K";

const int SCROLL_TOP = 23;
const int SCROLL_BOTTOM = 80;

constexpr char START_PROMPT[] = "Press [Dd] to enter debug mode, or any other key to enter OS mode\r\n";
constexpr char SENSOR_DATA[] = "\r\nRECENT SENSOR DATA:\r\n\r\n\r\n";
constexpr char DEBUG_TITLE[] = "Debug:\r\n";
constexpr char WELCOME_MSG[] = "\r\nWelcome to AbyssOS! ©Pi Technologies, 2023\r\n";
constexpr char SWITCH_UI_L0[] = "SWITCHES:\r\n";
constexpr char PROMPT[] = "\r\nABYSS> ";
constexpr char PROMPT_NNL[] = "ABYSS> ";
constexpr char SETUP_SCROLL[] = "\033[%d;%dr";
constexpr char MOVE_CURSOR_F[] = "\033[%d;%dH";
constexpr char PROMPT_CURSOR[] = "\033[19;%dH";

const char ERROR[] = "ERROR: INVALID COMMAND\r\n";
const char LENGTH_ERROR[] = "ERROR: COMMAND TOO LONG\r\n";
const char QUIT[] = "\r\nQUITTING...\r\n\r\n";

const char SWITCH_UI_L1[] = "+-----+-----+-----+-----+-----+-----+\r\n";
const char SWITCH_UI_L2[] = "|01 $ |02 $ |03 $ |04 $ |05 $ |06 $ |\r\n";
const char SWITCH_UI_L3[] = "+-----+-----+-----+-----+-----+-----+\r\n";
const char SWITCH_UI_L4[] = "|07 $ |08 $ |09 $ |10 $ |11 $ |12 $ |\r\n";
const char SWITCH_UI_L5[] = "+-----+-----+-----+-----+-----+-----+\r\n";
const char SWITCH_UI_L6[] = "|13 $ |14 $ |15 $ |16 $ |17 $ |18 $ |\r\n";
const char SWITCH_UI_L7[] = "======================================\r\n";
const char SWITCH_UI_L8[] = "|0x99  $ |0x9A  $ |0x9B  $ |0x9C  $ | \\\r\n";
const char SWITCH_UI_L9[] = "+--------+--------+--------+--------+  \\\r\n";
const char* const SWITCH_UI[]
	= { SWITCH_UI_L0, SWITCH_UI_L1, SWITCH_UI_L2, SWITCH_UI_L3, SWITCH_UI_L4, SWITCH_UI_L5, SWITCH_UI_L6, SWITCH_UI_L7, SWITCH_UI_L8, SWITCH_UI_L9 };

const char TRAIN_UI_L0[]
	= "               "
	  "_--====-===-========-----========-==--=--============---===--=--=======-====-=========---=-========-==--=-============-======-====-====-_\r\n";
const char TRAIN_UI_L1[] = "             _(  1:           mm/s - | 2:           mm/s - | 24:           mm/s - | 58:           mm/s - | 74:           "
						   "mm/s - | 78:           mm/s -   )_\r\n";
const char TRAIN_UI_L2[]
	= "            (     NxS:     PrS:      |  NxS:     PrS:      |  NxS:      PrS:      |  NxS:     PrS:       |  NxS:     PrS:  "
	  "     |  NxS:     PrS:          )\r\n";
const char TRAIN_UI_L3[]
	= "           _(     T:     t D:     mm |  T:     t D:     mm |  T:     t D:     mm  |  T:     t D:     mm  |  T:     t D:    "
	  " mm  |  T:     t D:     mm    _)\r\n";
const char TRAIN_UI_L4[]
	= "           (_     Src:     Dst:      |  Src:     Dst:      |  Src:     Dst:       |  Src:     Dst:       |  Src:     Dst:  "
	  "     |  Src:     Dst:        _)\r\n";
const char TRAIN_UI_L5[]
	= "           OO(    BgC:     BgW:      |  BgC:     BgW:      |  BgC:     BgW:       |  BgC:     BgW:       |  BgC:     BgW:  "
	  "     |  BgC:     BgW:        )_\r\n";
const char TRAIN_UI_L6[]
	= "          0 (                        |                     |                      |                      |                 "
	  "     |                       __)\r\n";
const char TRAIN_UI_L7[]
	= "         o   (_                      |                     |                      |                      |                 "
	  "     |                      _)\r\n";
const char TRAIN_UI_L8[]
	= "        o      "
	  "'=____________________|_____________________|______________________|______________________|______________________|_____________________'\r\n";
const char TRAIN_UI_L9[] = "      .o\r\n";
const char TRAIN_UI_L10[] = "     . ______          :::::::::::::::::: :::::::::::::::::: __|-----|__ :::::::::::::::::: __|-----|__ "
							":::::::::::::::::: :::::::::::::::::: _______________\r\n";
const char TRAIN_UI_L11[] = "   _()_||__|| --++++++ |[][][][][][][][]| |[][][][][][][][]| |  [] []  | |[][][][][][][][]| |  [] []  | "
							"|[][][][][][][][]| |[][][][][][][][]| |    |.\\/.|   |\r\n";
const char TRAIN_UI_L12[]
	= "  (BNSF "
	  "1995|;|______|;|________________|;|________________|;|_________|;|________________|;|_________|;|________________|;|______"
	  "__________|;|____|_/\\_|___|\r\n";
const char TRAIN_UI_L13[]
	= " /-OO----OO\"\"  oo  oo   oo oo      oo oo   oo oo      oo oo   oo     oo   oo oo      oo oo   oo     oo   oo oo      oo oo   oo oo      "
	  "oo oo   o^o       o^o\r\n";
const char TRAIN_UI_L14[]
	= "=========================================================================================================================="
	  "====================================\r\n";
const char* const TRAIN_UI[] = { TRAIN_UI_L0, TRAIN_UI_L1, TRAIN_UI_L2,	 TRAIN_UI_L3,  TRAIN_UI_L4,	 TRAIN_UI_L5,  TRAIN_UI_L6, TRAIN_UI_L7,
								 TRAIN_UI_L8, TRAIN_UI_L9, TRAIN_UI_L10, TRAIN_UI_L11, TRAIN_UI_L12, TRAIN_UI_L13, TRAIN_UI_L14 };

enum class TrainUIReq { TrainUISpeedDir = 0, TrainUINextPrev, TrainUITimeDist, TrainUISrcDst, TrainUIBarge, DEFAULT };

const char TRAIN_PRINTOUT_L0[] = ": %02d%c %03ld.%02ld";
const char TRAIN_PRINTOUT_L1[] = "NxS: %c%02d PrS: %c%02d";
const char TRAIN_PRINTOUT_L2[] = "T: %04dt D: %04dmm";
const char TRAIN_PRINTOUT_L3[] = "Src: %03d Dst: %03d";
const char TRAIN_PRINTOUT_L4[] = "BgC: %03d BgW: %03d";
const char* const TRAIN_PRINTOUT[] = { TRAIN_PRINTOUT_L0, TRAIN_PRINTOUT_L1, TRAIN_PRINTOUT_L2, TRAIN_PRINTOUT_L3, TRAIN_PRINTOUT_L4 };

const char DELIMINATION[] = "================================================================";

const int TERM_A_BUFLEN = 100;
const int SWITCH_UI_LEN = 10;
const int TRAIN_UI_LEN = 15;
const int TRAIN_PRINTOUT_ROW = 3;
const int TRAIN_PRINTOUT_COLUMN = 45;
const int TRAIN_PRINTOUT_FIRST = 63;
const int TRAIN_PRINTOUT_WIDTH = 22;
const int RECENT_SENSOR_COUNT = 10;
const int MAX_PUTS_LEN = 256;
const int TRAIN_PRINTOUT_UI_OFFSETS[] = { 0, 0, 1, 2, 3, 4 };

const int ONE_DIGIT = 10;
const int TWO_DIGITS = 100;
const int THREE_DIGITS = 1000;
const int FOUR_DIGITS = 10000;

// Hardcoded, best-guess TID value.
const int TERMINAL_ADMIN_TID = 24;

const int MAX_COMMAND_LEN = 8;
const int MAX_COMMAND_NUMS = 32;
const int TERM_NUM_TRAINS = 6;

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

struct GenericCommand {
	char name[MAX_COMMAND_LEN] = { 0 };
	etl::queue<int, MAX_COMMAND_NUMS> args = etl::queue<int, MAX_COMMAND_NUMS>();
	bool success = false;

	GenericCommand() { }
};

struct GlobalTrainInfo {
	long velocity;
	int next_sensor;
	int prev_sensor;

	long time_to_next_sensor; // expected time to next sensor in ticks
	long dist_to_next_sensor; // expected distance to next sensor in mm

	int path_src;
	int path_dest;

	int barge_count = 0;
	int barge_weight = 1;

	friend bool operator==(const GlobalTrainInfo& lhs, const GlobalTrainInfo& rhs) {
		return lhs.velocity == rhs.velocity && lhs.next_sensor == rhs.next_sensor && lhs.prev_sensor == rhs.prev_sensor
			   && lhs.time_to_next_sensor == rhs.time_to_next_sensor && lhs.dist_to_next_sensor == rhs.dist_to_next_sensor
			   && lhs.path_src == rhs.path_src && lhs.path_dest == rhs.path_dest && lhs.barge_count == rhs.barge_count
			   && lhs.barge_weight == rhs.barge_weight;
	}

	friend bool operator!=(const GlobalTrainInfo& lhs, const GlobalTrainInfo& rhs) {
		return !(lhs == rhs);
	}
};

struct WorkerRequestBody {
	uint32_t msg_len = 0;
	char msg[MAX_PUTS_LEN];
};

union RequestBody
{
	char regular_msg;
	WorkerRequestBody worker_msg;
	GlobalTrainInfo train_info[TERM_NUM_TRAINS];
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

struct CourierRequestArgs {
	int args[MAX_COMMAND_NUMS];
	uint32_t num_args;
};

union CourierRequestBody
{
	int regular_body;
	CourierRequestArgs courier_body;
};

struct TerminalCourierMessage {
	Message::RequestHeader header = Message::RequestHeader::NONE;
	CourierRequestBody body;

} __attribute__((aligned(8)));
}