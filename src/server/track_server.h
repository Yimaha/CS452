#pragma once
#include "../routing/dijkstra.h"
#include "../rpi.h"
#include "courier_pool.h"
#include "request_header.h"

namespace Track
{

constexpr char TRACK_SERVER_NAME[] = "TRACK_SERVER";
constexpr int NUM_SWITCHES = 22;
constexpr uint64_t TRACK_A_ID = 1;
constexpr uint64_t TRACK_B_ID = 2;
constexpr int SAFETY_DISTANCE = 200; // 40 cm or 400 mm

constexpr int NUM_SWITCH_SUBS = 32;

void track_server();
void track_courier();

struct Command {
	char id;
	char action;
};

struct StartAndDest {
	int start;
	int end;
	bool allow_reverse;
};

struct Reserve {
	int path[Routing::PATH_LIMIT] = { -1 };
	int len = 0;
	int train_id = 0;
};

union RequestBody
{
	uint64_t info;
	Command command;
	StartAndDest start_and_end;
	Reserve reservation;

	RequestBody() {
		this->info = 0;
	}
};

struct PathRespond {
	bool successful;
	int path[Routing::PATH_LIMIT];
	int path_len;
	int source;
	int dest;
	bool reverse; // should it reverse at the end
	int rev_offset;
};

struct ReservationStatus {
	bool successful;
	bool dead_lock_detected;
	int dead_lock_other_id;
	int64_t res_dist;
};

struct TrackServerReq {
	Message::RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

struct TrackCourierReq {
	Message::RequestHeader header;
	RequestBody body; // depending on the header, it treats the body differently
} __attribute__((aligned(8)));

}
