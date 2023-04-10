#pragma once

#include "../etl/queue.h"
#include "../rpi.h"
#include "request_header.h"
namespace Train
{

constexpr char TRAIN_SERVER_NAME[] = "TRAIN_ADMIN";
constexpr int TRAIN_UART_CHANNEL = 1;
constexpr char REV_COMMAND = 15 + 16;
constexpr int NUM_TRAINS = 6;
constexpr int NUM_SWITCHES = 22;
constexpr int TRAIN_NUMBERS[NUM_TRAINS] = { 1, 2, 24, 58, 74, 78 };
int get_switch_id(int id);
struct TrainRaw {
public:
	/**
	 * TrainRaw doesn't contain, or manage any of the logic, it simply is a struct that keeps information about the train based on the command
	 * any state management is shifted to global path planning server
	 */
	int speed = 0;
	bool direction = true;
};

constexpr int NO_TRAIN = -1;
constexpr int NO_SWITCH = -1;
constexpr int train_num_to_index(int train_num) {
	for (int i = 0; i < Train::NUM_TRAINS; i++) {
		if (TRAIN_NUMBERS[i] == train_num) {
			return i;
		}
	}
	return NO_TRAIN;
}

void train_admin();
void train_courier();

struct Command {
	char id;
	char action;
};
union RequestBody
{
	Command command;
	uint64_t next_delay;
};

struct TrainAdminReq {
	Message::RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

struct TrainCourierReq {
	Message::RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

}

bool operator==(const Train::TrainRaw& lhs, const Train::TrainRaw& rhs);
bool operator!=(const Train::TrainRaw& lhs, const Train::TrainRaw& rhs);