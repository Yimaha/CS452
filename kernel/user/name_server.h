
#pragma once
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/hashmap.h"
#include "../utils/utility.h"

namespace Name
{
constexpr int MAX_NAME_SERVER_SIZE = 256;

/*
 * The name server is a simple server that maps names to tids.
 * Tasks can register their name with the name server and then
 * use the name server to look up the tid of tasks by name.
 * The name server itself simply waits for requests from tasks
 * and then responds to them.
 */
extern "C" void name_server();

enum class Iden : uint64_t { REGISTER_AS, WHO_IS };
enum Exception { INVALID_NS_TASK_ID = -1, NAME_NOT_REGISTERED = -2, INVALID_IDEN = -3 };
struct NameContainer {
	char arr[MAX_NAME_LENGTH];

	friend bool operator==(const NameContainer& a, const NameContainer& b) {
		// const uint64_t first = *((uint64_t*)(a.arr - 1)) & 0x00FFFFFFFFFFFFFF;
		// const uint64_t second = *((uint64_t*)(a.arr + sizeof(uint64_t) - 1));
		// const uint64_t ofirst = *((uint64_t*)(b.arr - 1)) & 0x00FFFFFFFFFFFFFF;
		// const uint64_t osecond = *((uint64_t*)(b.arr + sizeof(uint64_t) - 1));

		// return first == ofirst && second == osecond;

		for (int i = 0; i < MAX_NAME_LENGTH; i++) {
			if (a.arr[i] != b.arr[i]) {
				return false;
			} else if (a.arr[i] == '\0' || b.arr[i] == '\0') {
				return true;
			}
		}

		return true;
	}

	friend bool operator!=(const NameContainer& a, const NameContainer& b) {
		return !(a == b);
	}
};

struct NameServerReq {
	Iden iden;
	NameContainer name;
} __attribute__((packed, aligned(8)));
}