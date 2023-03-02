
#pragma once

#include "../kernel.h"

namespace TrainLogic
{

constexpr char TRAIN_LOGIC_SERVER_NAME[] = "TRAIN_LOGIC";

constexpr int NUM_TRAINS = 6;
const int TRAIN_NUMBERS[NUM_TRAINS] = { 1, 2, 24, 58, 74, 78 };

enum class RequestHeader : uint32_t { NONE, SPEED, REV };

struct RequestBody {
	char id;
	char action;
};

struct TrainLogicServerReq {
	RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

void train_logic_server();
}