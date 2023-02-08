
#pragma once
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/hashmap.h"
#include "../utils/utility.h"

namespace Name
{
constexpr int MAX_NAME_SERVER_SIZE = 256;
constexpr uint64_t MAX_NAME_LENGTH = 16; // max length of a name
constexpr uint64_t NAME_REQ_LENGTH = MAX_NAME_LENGTH + 8;

/*
 * The name server is a simple server that maps names to tids.
 * Tasks can register their name with the name server and then
 * use the name server to look up the tid of tasks by name.
 * The name server itself simply waits for requests from tasks
 * and then responds to them.
 */
extern "C" void name_server();

enum class RequestHeader : uint64_t { REGISTER_AS, WHO_IS };
enum Exception { INVALID_NS_TASK_ID = -1, NAME_NOT_REGISTERED = -2, INVALID_IDEN = -3 };
struct RequestBody {
	char arr[MAX_NAME_LENGTH];

	friend bool operator==(const RequestBody& a, const RequestBody& b) {
		for (uint64_t i = 0; i < MAX_NAME_LENGTH; i++) {
			if (a.arr[i] != b.arr[i]) {
				return false;
			} else if (a.arr[i] == '\0' || b.arr[i] == '\0') {
				return true;
			}
		}

		return true;
	}

	friend bool operator!=(const RequestBody& a, const RequestBody& b) {
		return !(a == b);
	}
};

struct NameServerReq {
	RequestHeader header;
	RequestBody name;
} __attribute__((packed, aligned(8)));
}