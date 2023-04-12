#pragma once

#include "../etl/queue.h"
#include "../kernel.h"
#include "../routing/track_data_new.h"
#include "../utils/utility.h"
#include "request_header.h"
#include "sensor_admin.h"
#include "train_admin.h"

namespace Terminal
{

constexpr char TERMINAL_ADMIN[] = "TERMINAL_ADMIN";
constexpr char TERMINAL_CLOCK_COURIER_NAME[] = "TERMINAL_CLOCK";
constexpr char TERMINAL_SWITCH_COURIER_NAME[] = "TERMINAL_CLOCK";
constexpr char TERMINAL_SENSOR_COURIER_NAME[] = "TERMINAL_SENSOR";
constexpr char TERMINAL_PRINTER_NAME[] = "TERMINAL_PRINT";

constexpr char CLEAR_SCREEN[] = "\033[2J";
constexpr char TOP_LEFT[] = "\033[H";
constexpr char HIDE_CURSOR[] = "\033[?25l";
constexpr char RESET_CURSOR[] = "\033[0m";
constexpr char SAVE_CURSOR[] = "\033[s\033[H";
constexpr char SAVE_CURSOR_NO_JUMP[] = "\033[s";
constexpr char RESTORE_CURSOR[] = "\033[u";
constexpr char SENSOR_CURSOR[] = "\033[44;2H";
constexpr char BLACK_CURSOR[] = "\033[30m";
constexpr char RED_CURSOR[] = "\033[31m";
constexpr char GREEN_CURSOR[] = "\033[32m";
constexpr char YELLOW_CURSOR[] = "\033[33m";
constexpr char BLUE_CURSOR[] = "\033[34m";
constexpr char MAGENTA_CURSOR[] = "\033[35m";
constexpr char CYAN_CURSOR[] = "\033[36m";
constexpr char WHITE_CURSOR[] = "\033[37m";
constexpr char CLEAR_LINE[] = "\r\033[K";

const char* const TRAIN_COLOURS[] = {
	// 1, 2, 24, 58, 74, 78
	CYAN_CURSOR, RED_CURSOR, MAGENTA_CURSOR, WHITE_CURSOR, YELLOW_CURSOR, BLUE_CURSOR
};

constexpr char START_PROMPT[] = "Press [Dd] to enter debug mode, or any other key to enter OS mode\r\n";
constexpr char SENSOR_DATA[] = "\r\nRECENT SENSOR DATA:\r\n";
constexpr char DEBUG_TITLE[] = "Debug:\r\n";
constexpr char WELCOME_MSG[] = "Welcome to AbyssOS! ©Pi Technologies, 2023\r\n";
constexpr char SWITCH_UI_L0[] = "SWITCHES:\r\n";
constexpr char RESERVE_UI_L0[] = "RESERVATIONS:\r\n";
constexpr char PROMPT[] = "\r\nABYSS> ";
constexpr char PROMPT_NNL[] = "ABYSS> ";
constexpr char SETUP_SCROLL[] = "\033[%d;%dr";
constexpr char MOVE_CURSOR_F[] = "\033[%d;%dH";
constexpr char MOVE_CURSOR_FO[] = "\033[%d;%dH%c";
constexpr char MOVE_CURSOR_FT[] = "\033[%d;%dH▄";
constexpr char MOVE_CURSOR_FS[] = "\033[%d;%dH🗡️";
constexpr char PROMPT_CURSOR[] = "\033[28;%dH";

const char TARGETS_TITLE[] = "UNREACHED DESTINATIONS:";
const char TARGETS[] = "| B13-B14 |  C1-C2  |  D1-D2  |  E1-E2  |";
const char* const TARGET_SENSORS[] = { "B13-B14", " C1-C2", " D1-D2", " E1-E2" };
const int TARGET_IDS[] = { 28, 29, 32, 33, 48, 49, 64, 65 };
const int NUM_TARGETS = 4;
const int TARGET_IDS_LEN = 2 * NUM_TARGETS;

const int FINISH_SENSORS[] = { 0, 1, 12, 13, 14, 15 };
const int NUM_FINISH_SENSORS = sizeof(FINISH_SENSORS) / sizeof(int);

const char INIT_WARN[] = "DON'T FORGET TO INITIALISE THE TRACK!";
const char ERROR[] = "ERROR: INVALID COMMAND\r\n";
const char LENGTH_ERROR[] = "ERROR: COMMAND TOO LONG\r\n";
const char QUIT[] = "\r\nQUITTING...\r\n\r\n";
const char WIN[] = "YOU WIN!\r\n";

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
	= "           _(     T:     t P:     mm |  T:     t P:     mm |  T:     t P:     mm  |  T:     t P:     mm  |  T:     t P:    "
	  " mm  |  T:     t P:     mm    _)\r\n";
const char TRAIN_UI_L4[]
	= "           (_     S:       D:        |  S:       D:        |  S:       D:         |  S:       D:         |  S:       D:    "
	  "     |  S:       D:          _)\r\n";
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
const char TRAIN_PRINTOUT_L2[] = "T: %04dt P: %04dmm";
const char TRAIN_PRINTOUT_L3[] = "S: %5s D: %5s";
const char TRAIN_PRINTOUT_L4[] = "BgC: %03d BgW: %03d";
const char* const TRAIN_PRINTOUT[] = { TRAIN_PRINTOUT_L0, TRAIN_PRINTOUT_L1, TRAIN_PRINTOUT_L2, TRAIN_PRINTOUT_L3, TRAIN_PRINTOUT_L4 };

const char TRAIN_LEGEND[] = "██ %02d";

enum class WhichTrack { TRACK_A, TRACK_B };

const char TRACK_A_L00[] = "▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁    ";
const char TRACK_A_L01[] = "                     ╱    ╱                                                     ╲";
const char TRACK_A_L02[] = "▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁╱    ╱ ▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁  o  ";
const char TRACK_A_L03[] = "                   ╱    ╱ ╱                 ╲             ╱                    ╲  ╲";
const char TRACK_A_L04[] = "▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁╱    ╱ ╱                   ╲           ╱                      o  ⧫";
const char TRACK_A_L05[] = "                      ╱ ╱                     o         o                        ╲ │";
const char TRACK_A_L06[] = "                     ╱ ╱                       ╲   │   ╱                          ╲│";
const char TRACK_A_L07[] = "                    ⧫ ╱                         o  │  o                            │";
const char TRACK_A_L08[] = "                    │╱                           ╲ │ ╱                             │";
const char TRACK_A_L09[] = "                    o                             ╲│╱                              │";
const char TRACK_A_L10[] = "                    │                              ▼                               │";
const char TRACK_A_L11[] = "                    │                              │                               │";
const char TRACK_A_L12[] = "                    o                              ▲                               │";
const char TRACK_A_L13[] = "                    │╲                            ╱│╲                              │";
const char TRACK_A_L14[] = "                    ⧫ ╲                          o │ o                             │";
const char TRACK_A_L15[] = "                     ╲ ╲                        ╱  │  ╲                           ╱│";
const char TRACK_A_L16[] = "▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁    ╲ ╲                      o       o                         ╱ ⧫";
const char TRACK_A_L17[] = "                  ╲    ╲ ╲                    ╱         ╲                       o ╱";
const char TRACK_A_L18[] = "▁o▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁╲    ╲ ╲▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁╱▁o▁▁▁▁▁▁▁o▁╲▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁╱ o";
const char TRACK_A_L19[] = "                    ╲    ╲                                                      ╱";
const char TRACK_A_L20[] = "▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁╲    ╲▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁╱    ";
const char TRACK_A_L21[] = "                      ╲                  ╲                   ╱   ";
const char TRACK_A_L22[] = "▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁╲▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁╲▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁╱▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁";

const char TRACK_B_L00[] = "▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁";
const char TRACK_B_L01[] = "                       ╱                 ╲                  ╲                       ";
const char TRACK_B_L02[] = "     ▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁╱▁▁▁o▁▁▁▁▁▁▁▁▁▁o▁▁▁▁╲▁▁▁▁▁▁▁▁o▁▁▁▁▁    ╲▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁";
const char TRACK_B_L03[] = "    ╱                                                    ╲    ╲                     ";
const char TRACK_B_L04[] = "   ╱ ▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁ ╲    ╲▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁o▁";
const char TRACK_B_L05[] = "  o ╱                     ╲           ╱                  ╲ ╲    ╲                   ";
const char TRACK_B_L06[] = " ╱ o                       ╲         ╱                    ╲ ╲    ╲                  ";
const char TRACK_B_L07[] = "⧫ ╱                         o       o                      ╲ ╲    ╲                 ";
const char TRACK_B_L08[] = "│╱                           ╲  │  ╱                        ╲ ╲    ⧫                ";
const char TRACK_B_L09[] = "│                             o │ o                          ╲ ⧫   │                ";
const char TRACK_B_L10[] = "│                              ╲│╱                            ╲│   o                ";
const char TRACK_B_L11[] = "│                               ▼                              o   │                ";
const char TRACK_B_L12[] = "│                               │                              │   │                ";
const char TRACK_B_L13[] = "│                               ▲                              │   │                ";
const char TRACK_B_L14[] = "│                              ╱│╲                             o   │                ";
const char TRACK_B_L15[] = "│                             ╱ │ ╲                           ╱│   o                ";
const char TRACK_B_L16[] = "│                            o  │  o                         ╱ ⧫   │                ";
const char TRACK_B_L17[] = "│╲                          ╱       ╲                       ╱ ╱    ⧫                ";
const char TRACK_B_L18[] = "⧫ ╲                        o         o                     ╱ ╱    ╱                 ";
const char TRACK_B_L19[] = " ╲ o                      ╱           ╲                   ╱ ╱    ╱                  ";
const char TRACK_B_L20[] = "  o ╲▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁╱▁▁o▁▁▁▁▁▁▁o▁▁╲▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁╱ ╱    ╱▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁";
const char TRACK_B_L21[] = "   ╲                                                      ╱    ╱                    ";
const char TRACK_B_L22[] = "    ╲▁▁▁▁▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁o▁▁▁▁▁▁╱▁▁▁▁╱▁▁▁▁o▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁";

const char* const TRACK_A[] = { TRACK_A_L00, TRACK_A_L01, TRACK_A_L02, TRACK_A_L03, TRACK_A_L04, TRACK_A_L05, TRACK_A_L06, TRACK_A_L07,
								TRACK_A_L08, TRACK_A_L09, TRACK_A_L10, TRACK_A_L11, TRACK_A_L12, TRACK_A_L13, TRACK_A_L14, TRACK_A_L15,
								TRACK_A_L16, TRACK_A_L17, TRACK_A_L18, TRACK_A_L19, TRACK_A_L20, TRACK_A_L21, TRACK_A_L22 };

const char* const TRACK_B[] = { TRACK_B_L00, TRACK_B_L01, TRACK_B_L02, TRACK_B_L03, TRACK_B_L04, TRACK_B_L05, TRACK_B_L06, TRACK_B_L07,
								TRACK_B_L08, TRACK_B_L09, TRACK_B_L10, TRACK_B_L11, TRACK_B_L12, TRACK_B_L13, TRACK_B_L14, TRACK_B_L15,
								TRACK_B_L16, TRACK_B_L17, TRACK_B_L18, TRACK_B_L19, TRACK_B_L20, TRACK_B_L21, TRACK_B_L22 };
const int TRACK_B_LEN = sizeof(TRACK_B) / sizeof(TRACK_B[0]);
const char* const* const TRACKS[] = { TRACK_A, TRACK_B };

const char WASD[] = { 'W', 'A', 'S', 'D', 'E' };
const int WASD_LEN = sizeof(WASD) / sizeof(WASD[0]);

const char NAIL_L0[] = "                                                 __";
const char NAIL_L1[] = "                                    _______------  |               ";
const char NAIL_L2[] = "                __________-----▄▄█▀▀         ▄▄▄▄█▀▀▌              ";
const char NAIL_L3[] = "     _--------▄█__________▄▄█▀▀__________▄▄▀▀▀______█▄▄▄▄▄▄▄▄▄▄▄▄▄▄";
const char NAIL_L4[] = "---▄█______▄█▀__       ▄█▀           ▄▄██▀▀▀▀▀▀▀▀▀▀▀█▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
const char NAIL_L5[] = "                ------▀---______ ▄▄█▀         ▄▄▄█▀▀▌              ";
const char NAIL_L6[] = "                                ▀-------__▄▄█▀▀____|               ";

const char* const NAIL[] = { NAIL_L0, NAIL_L1, NAIL_L2, NAIL_L3, NAIL_L4, NAIL_L5, NAIL_L6 };
const int NAIL_LEN = sizeof(NAIL) / sizeof(NAIL[0]);

const char KNIGHT_L00[] = "⠀⠀⠀⡄⡀⠀⠀⠀⠀⠀⠀⠀";
const char KNIGHT_L01[] = "⠀⠀⣼⣿⠇⠀⠀⠈⢿⣿⣦⠀";
const char KNIGHT_L02[] = "⠀⢰⣿⣿⠀⠀⠀⠀⠀⢿⣿⡇";
const char KNIGHT_L03[] = "⠀⠸⣿⣿⣷⣶⣿⣿⣿⣿⣿⡇";
const char KNIGHT_L04[] = "⠀⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿⡆";
const char KNIGHT_L05[] = "⠀⠀⡾⢿⣿⣿⠛⢿⣿⣿⣿⣿";
const char KNIGHT_L06[] = "⠀⠀⢷⡈⣿⣿⣆⢘⣿⣿⣿⡟";
const char KNIGHT_L07[] = "⠀⠀⠘⢿⣿⣿⣿⣿⣿⣿⡟⠁";
const char KNIGHT_L08[] = "⠀⣀⡄⣰⣿⣿⣿⣿⣿⣿⣿⠀";
const char KNIGHT_L09[] = "⠈⠛⢸⣿⣿⣿⣿⣿⣿⣿⣿⡇";
const char KNIGHT_L10[] = "⠀⠀⠈⠿⣿⣿⣿⣿⣿⣿⣿⡇";
const char KNIGHT_L11[] = "⠀⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⠇";
const char KNIGHT_L12[] = "⠀⠀⠀⠀⠸⣿⠁⠙⠃⢿⠟⠀";

const char* const KNIGHT_SPRITE[] = { KNIGHT_L00, KNIGHT_L01, KNIGHT_L02, KNIGHT_L03, KNIGHT_L04, KNIGHT_L05, KNIGHT_L06,
									  KNIGHT_L07, KNIGHT_L08, KNIGHT_L09, KNIGHT_L10, KNIGHT_L11, KNIGHT_L12 };
const int KNIGHT_SPRITE_LEN = sizeof(KNIGHT_SPRITE) / sizeof(KNIGHT_SPRITE[0]);

const int TRACK_STARTING_ROW = 32;
const int TRACK_STARTING_COLUMN = 130;
const int NAIL_ROW = 47;
const int NAIL_COL = 45;
const int KNIGHT_ROW = 42;
const int KNIGHT_COL = 105;
const int TARGET_ROW = 43;
const int TARGET_COL = 54;
const int TARGET_WIDTH = 10;

struct UIPosition {
	int r;
	int c;
};

enum class Arrangement { Vertical, HLeft, HRight };

const UIPosition TRACK_A_SENSORS[] = {
	{ 0, 16 },	{ 0, 16 },	{ 9, 20 },	{ 9, 20 },	{ 22, 18 }, { 22, 18 }, { 20, 16 }, { 20, 16 }, { 18, 14 }, { 18, 14 }, { 16, 12 }, { 16, 12 },
	{ 2, 13 },	{ 2, 13 },	{ 4, 12 },	{ 4, 12 },	{ 18, 47 }, { 18, 47 }, { 16, 47 }, { 16, 47 }, { 2, 47 },	{ 2, 47 },	{ 18, 1 },	{ 18, 1 },
	{ 22, 1 },	{ 22, 1 },	{ 20, 1 },	{ 20, 1 },	{ 14, 53 }, { 14, 53 }, { 12, 20 }, { 12, 20 }, { 14, 49 }, { 14, 49 }, { 22, 66 }, { 22, 66 },
	{ 20, 32 }, { 20, 32 }, { 22, 34 }, { 22, 34 }, { 18, 31 }, { 18, 31 }, { 2, 35 },	{ 2, 35 },	{ 0, 33 },	{ 0, 33 },	{ 20, 46 }, { 20, 46 },
	{ 7, 54 },	{ 7, 54 },	{ 2, 55 },	{ 2, 55 },	{ 4, 80 },	{ 4, 80 },	{ 2, 81 },	{ 2, 81 },	{ 18, 81 }, { 18, 81 }, { 20, 57 }, { 20, 57 },
	{ 18, 55 }, { 18, 55 }, { 16, 55 }, { 16, 55 }, { 7, 48 },	{ 7, 48 },	{ 5, 56 },	{ 5, 56 },	{ 2, 68 },	{ 2, 68 },	{ 0, 70 },	{ 0, 70 },
	{ 17, 80 }, { 17, 80 }, { 20, 70 }, { 20, 70 }, { 18, 68 }, { 18, 68 }, { 5, 46 },	{ 5, 46 },
};

const Arrangement TRACK_A_ARRANGEMENTS[]
	= { Arrangement::HRight,   Arrangement::HRight,	  Arrangement::Vertical, Arrangement::Vertical, Arrangement::HRight,   Arrangement::HRight,
		Arrangement::HRight,   Arrangement::HRight,	  Arrangement::HRight,	 Arrangement::HRight,	Arrangement::HRight,   Arrangement::HRight,
		Arrangement::HRight,   Arrangement::HRight,	  Arrangement::HRight,	 Arrangement::HRight,	Arrangement::HRight,   Arrangement::HRight,
		Arrangement::Vertical, Arrangement::Vertical, Arrangement::HRight,	 Arrangement::HRight,	Arrangement::HRight,   Arrangement::HRight,
		Arrangement::HRight,   Arrangement::HRight,	  Arrangement::HRight,	 Arrangement::HRight,	Arrangement::Vertical, Arrangement::Vertical,
		Arrangement::Vertical, Arrangement::Vertical, Arrangement::Vertical, Arrangement::Vertical, Arrangement::HLeft,	   Arrangement::HLeft,
		Arrangement::HRight,   Arrangement::HRight,	  Arrangement::HRight,	 Arrangement::HRight,	Arrangement::HRight,   Arrangement::HRight,
		Arrangement::HRight,   Arrangement::HRight,	  Arrangement::HRight,	 Arrangement::HRight,	Arrangement::HRight,   Arrangement::HRight,
		Arrangement::Vertical, Arrangement::Vertical, Arrangement::HLeft,	 Arrangement::HLeft,	Arrangement::Vertical, Arrangement::Vertical,
		Arrangement::Vertical, Arrangement::Vertical, Arrangement::Vertical, Arrangement::Vertical, Arrangement::HLeft,	   Arrangement::HLeft,
		Arrangement::HLeft,	   Arrangement::HLeft,	  Arrangement::Vertical, Arrangement::Vertical, Arrangement::Vertical, Arrangement::Vertical,
		Arrangement::Vertical, Arrangement::Vertical, Arrangement::HLeft,	 Arrangement::HLeft,	Arrangement::HLeft,	   Arrangement::HLeft,
		Arrangement::Vertical, Arrangement::Vertical, Arrangement::HLeft,	 Arrangement::HLeft,	Arrangement::HLeft,	   Arrangement::HLeft,
		Arrangement::Vertical, Arrangement::Vertical };

const UIPosition TRACK_B_SENSORS[] = {
	{ 22, 67 }, { 22, 67 }, { 14, 63 }, { 14, 63 }, { 0, 65 },	{ 0, 65 },	{ 2, 67 },	{ 2, 67 },	{ 4, 69 },	{ 4, 69 },	{ 10, 67 }, { 10, 67 },
	{ 20, 70 }, { 20, 70 }, { 15, 67 }, { 15, 67 }, { 4, 36 },	{ 4, 36 },	{ 7, 36 },	{ 7, 36 },	{ 20, 36 }, { 20, 36 }, { 4, 82 },	{ 4, 82 },
	{ 0, 82 },	{ 0, 82 },	{ 2, 82 },	{ 2, 82 },	{ 9, 30 },	{ 9, 30 },	{ 11, 63 }, { 11, 63 }, { 9, 34 },	{ 9, 34 },	{ 0, 17 },	{ 0, 17 },
	{ 2, 51 },	{ 2, 51 },	{ 0, 49 },	{ 0, 49 },	{ 4, 52 },	{ 4, 52 },	{ 20, 48 }, { 20, 48 }, { 22, 50 }, { 22, 50 }, { 2, 37 },	{ 2, 37 },
	{ 16, 29 }, { 16, 29 }, { 20, 28 }, { 20, 28 }, { 19, 3 },	{ 19, 3 },	{ 20, 2 },	{ 20, 2 },	{ 5, 2 },	{ 5, 2 },	{ 2, 26 },	{ 2, 26 },
	{ 4, 28 },	{ 4, 28 },	{ 7, 28 },	{ 7, 28 },	{ 16, 35 }, { 16, 35 }, { 18, 27 }, { 18, 27 }, { 20, 15 }, { 20, 15 }, { 22, 13 }, { 22, 13 },
	{ 6, 3 },	{ 6, 3 },	{ 2, 13 },	{ 2, 13 },	{ 4, 15 },	{ 4, 15 },	{ 18, 37 }, { 18, 37 },
};

const UIPosition* const TRACK_SENSORS[] = { TRACK_A_SENSORS, TRACK_B_SENSORS };

const UIPosition TRACK_BRANCH_MERGES[] = {
	{ 0, 0 },  { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }, { 0, 5 }, { 0, 6 }, { 0, 7 }, { 0, 8 }, { 0, 9 }, { 0, 10 },
	{ 0, 11 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, { 1, 6 }, { 1, 7 }, { 1, 8 }, { 1, 9 },
	{ 2, 0 },  { 2, 1 }, { 2, 2 }, { 2, 3 }, { 2, 4 }, { 2, 5 }, { 2, 6 }, { 2, 7 }, { 2, 8 }, { 2, 9 }, { 2, 10 },
	{ 2, 11 }, { 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 }, { 3, 4 }, { 3, 5 }, { 3, 6 }, { 3, 7 }, { 3, 8 }, { 3, 9 },
};

const char SWITCH_LEFTS[] = { 'c', 'c', 'c', 's', 'c', 's', 'c', 's', 'c', 'c', 'c', 'c', 's', 's', 'c', 'c', 's', 's', 's', 'c', 's', 'c' };
const char SWITCH_RIGHTS[] = { 's', 's', 's', 'c', 's', 'c', 's', 'c', 's', 's', 's', 's', 'c', 'c', 's', 's', 'c', 'c', 'c', 's', 'c', 's' };
const int SET_AHEAD_ALSO[] = { 154, 156 };
const int SET_AHEAD_ALSO_LEN = sizeof(SET_AHEAD_ALSO) / sizeof(int);

// Places the Knight should leapfrog to when you press W or E
const int STOPS[] = {
	0,	1,	10, 11, 12, 13, 14, 15, 22, 23, 24, 25, 26, 27, 28, 29, 32, 33, 34, 35, 36,
	37, 38, 39, 40, 41, 42, 43, 44, 45, 48, 49, 64, 65, 68, 69, 70, 71, 74, 75, 76, 77,
};
const int STOPS_LEN = sizeof(STOPS) / sizeof(int);
const bool STOP_CHECK[] = {
	true,  true,  false, false, false, false, false, false, false, false, true,	 true,	true,  true,  true,	 true,	false, false, false, false, false,
	false, true,  true,	 true,	true,  true,  true,	 true,	true,  false, false, true,	true,  true,  true,	 true,	true,  true,  true,	 true,	true,
	true,  true,  true,	 true,	false, false, true,	 true,	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, true,  true,	 false, false, true,  true,	 true,	true,  false, false, true,	true,  true,  true,	 false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
};

const char DELIMINATION[] = "================================================================";
const char HUGE_DELIMINATION[] = "==================================================================================================================="
								 "========================================================================================================\r\n";
const char THIN_DELIMINATION[]
	= "----------------------------------------------------------------------------------------------------------------------------\r\n";

const int TERM_A_BUFLEN = 100;
const int SWITCH_UI_LEN = 10;
const int TRAIN_UI_LEN = 15;
const int TRAIN_PRINTOUT_ROW = 57;
const int TRAIN_PRINTOUT_COLUMN = 1;
const int TRAIN_PRINTOUT_FIRST = 19;
const int TRAIN_PRINTOUT_WIDTH = 22;
const int RECENT_SENSOR_COUNT = 10;
const int MAX_PUTS_LEN = 256;
const int TRAIN_PRINTOUT_UI_OFFSETS[] = { 0, 0, 1, 2, 3, 4 };

constexpr int KNIGHT = 78;
constexpr int KNIGHT_INDEX = Train::train_num_to_index(KNIGHT);

const int SCROLL_TOP = 1;
const int SCROLL_BOTTOM = 25;
const int SWITCH_TABLE_BASE = 48;
const int RESERVE_TABLE_BASE = 32;
const int UI_TOP = SCROLL_BOTTOM + 5;
const int UI_BOT = 72;

const int ONE_DIGIT = 10;
const int TWO_DIGITS = 100;
const int THREE_DIGITS = 1000;
const int FOUR_DIGITS = 10000;

const int MAX_COMMAND_LEN = 8;
const int MAX_COMMAND_NUMS = 32;
const int TERM_NUM_TRAINS = 6;
const int NO_NODE = -1;

const int CLOCK_UPDATE_FREQUENCY = 10;
const int CMD_LEN = 64;
const int CMD_HISTORY_LEN = 128;

enum class TAState : uint32_t { TA_DEFAULT_ARROW_STATE, TA_FOUND_ESCAPE, TA_FOUND_BRACKET };

void terminal_admin();
void terminal_courier();
void terminal_clock_courier();
void sensor_query_courier();
void idle_time_courier();
void user_input_courier();
void switch_state_courier();
void reservation_courier();
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
	char reserve_state[TRACK_MAX];
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