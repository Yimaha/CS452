#pragma once

#include "../kernel.h"
#include "../rpi.h"
#include "request_header.h"

namespace LocalPathing
{

constexpr char LOCAL_PATHING_NAME[] = "LOCAL_PATH_%d";
constexpr int MAX_COMMAND_NUMS = 32;

const int LOCAL_PATHING_BUFLEN = 100;

struct Command {
	int args[MAX_COMMAND_NUMS];
	uint32_t num_args;
};

union RequestBody
{
	int train_num;
	Command command;
};

struct LocalPathingReq {
	Message::RequestHeader header;
	RequestBody body;
} __attribute__((aligned(8)));

void local_pathing_worker();

}