#pragma once

#include "../etl/queue.h"
#include "../kernel.h"
#include "../rpi.h"
#include "request_header.h"
namespace Train
{

constexpr int TRAIN_STATUS_BUFFER_SIZE = 16;
class TrainStatus {
public:
	enum class State : uint32_t { REVERSING, FREE };
	struct Command {
		State state;
		int action;
	};
	TrainStatus()
		: state { State::FREE }
		, speed { 0 }
		, direction { false } { }
	// idea, the set speed command should be able to just barge in, while reverse is queued up
	bool setSpeed(int s);
	bool revStart();
	bool revClear();
	bool getDirection();
	int getSpeed();
	// the result value indicate if command can be fired right away or not

	// subscriber stores any caller that is waiting on the reverse to be completed
	etl::queue<int, TRAIN_STATUS_BUFFER_SIZE> rev_subscribers;

private:
	bool command(Command comm);
	void handleCommand(Command comm);
	State state;
	int speed;
	bool direction;
	bool desire_direction;
};

struct TerminalTrainStatus {
	char speed;
	bool direction;
};

constexpr char TRAIN_SERVER_NAME[] = "TRAIN_ADMIN";
constexpr int TRAIN_UART_CHANNEL = 1;
constexpr char REV_COMMAND = 15;
constexpr int NUM_TRAINS = 6;
constexpr int NUM_SWITCHES = 22;
constexpr int TRAIN_NUMBERS[NUM_TRAINS] = { 1, 2, 24, 58, 74, 78 };

int train_num_to_index(int train_num);
void train_admin();
void train_courier();


struct RequestBody {
	char id;
	char action;
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

bool operator==(const Train::TerminalTrainStatus& lhs, const Train::TerminalTrainStatus& rhs);
bool operator!=(const Train::TerminalTrainStatus& lhs, const Train::TerminalTrainStatus& rhs);
