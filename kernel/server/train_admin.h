#pragma once

#include "../etl/queue.h"
#include "../kernel.h"
#include "../rpi.h"
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


constexpr char TRAIN_SERVER_NAME[] = "TRAIN_ADMIN";
constexpr int TRAIN_UART_CHANNEL = 1;
constexpr char REV_COMMAND = 15;
constexpr int NUM_TRAINS = 6;
constexpr int NUM_SWITCHES = 22;
const int TRAIN_NUMBERS[NUM_TRAINS] = { 1, 2, 24, 58, 74, 78 };
void train_admin();
void train_courier();

enum class RequestHeader : uint32_t { SPEED, REV, SWITCH, COURIER_COMPLETE, SWITCH_DELAY_COMPLETE, DELAY_REV_COMPLETE }; // note that notify is an exclusive, clock notifier message.

struct RequestBody {
	char id;
	char action;
};

struct TrainAdminReq {
	RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

enum class CourierRequestHeader : uint32_t { REV_DELAY, SWITCH_DELAY }; // note that notify is an exclusive, clock notifier message.

struct TrainCourierReq {
	CourierRequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));





}
