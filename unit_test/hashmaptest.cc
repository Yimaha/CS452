
#include "../src/utils/hashmap.h"
#include "../src/utils/utility.h"
#include <cassert>
#include <iostream>

#define MAX_STRING_LEN 256

struct StringContainer {
	char str[MAX_STRING_LEN];

	StringContainer(const char* str) {
		for (int i = 0; i < MAX_STRING_LEN; i++) {
			this->str[i] = str[i];
		}
	}

	StringContainer() {
		for (int i = 0; i < MAX_STRING_LEN; i++) {
			this->str[i] = '\0';
		}
	}

	bool operator==(const StringContainer& other) {
		for (int i = 0; i < MAX_STRING_LEN; i++) {
			if (this->str[i] != other.str[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator!=(const StringContainer& other) {
		return !(*this == other);
	}

	bool operator<(const StringContainer& other) {
		for (int i = 0; i < MAX_STRING_LEN; i++) {
			if (this->str[i] < other.str[i]) {
				return true;
			} else if (this->str[i] > other.str[i]) {
				return false;
			}
		}
		return false;
	}

	bool operator>(const StringContainer& other) {
		for (int i = 0; i < MAX_STRING_LEN; i++) {
			if (this->str[i] > other.str[i]) {
				return true;
			} else if (this->str[i] < other.str[i]) {
				return false;
			}
		}
		return false;
	}

	bool operator<=(const StringContainer& other) {
		return !(*this > other);
	}

	bool operator>=(const StringContainer& other) {
		return !(*this < other);
	}

	StringContainer& operator=(const StringContainer& other) {
		for (int i = 0; i < MAX_STRING_LEN; i++) {
			this->str[i] = other.str[i];
		}
		return *this;
	}

	StringContainer& operator=(const char* str) {
		for (int i = 0; i < MAX_STRING_LEN; i++) {
			this->str[i] = str[i];
		}
		return *this;
	}
};

struct Value {
	int value1;
	int value2;
};

StringContainer strings[10] = { { "Hello" }, { "World" }, { "This" }, { "Is" }, { "A" }, { "Test" }, { "Of" }, { "The" }, { "Emergency" }, { "Broadcast" } };

int main(void) {
	Hashmap<int, int> map = Hashmap<int, int>();
	assert(map.is_empty());

	// These two values were selected because they have the same hash
	map.insert(239, 1);
	map.insert(257, 2);

	assert(map.len() == 2);
	assert(*map.get(239) == 1);
	assert(*map.get(257) == 2);

	map.del(239);
	assert(map.len() == 1);
	assert(map.get(239) == nullptr);
	assert(*map.get(257) == 2);

	map.del(257);
	assert(map.len() == 0);
	assert(map.get(257) == nullptr);
	assert(map.is_empty());

	for (int i = 10; i < MAX_MAP_SIZE + 9; i++) {
		map.insert(i, i);
	}

	assert(map.len() == MAX_MAP_SIZE - 1);
	assert(map.is_full());
	assert(map.insert(0, 0) == false);

	// Test string keys
	Hashmap<StringContainer, int> strmap = Hashmap<StringContainer, int>();
	assert(strmap.is_empty());

	for (int i = 0; i < 10; i++) {
		strmap.insert(strings[i], i);
	}

	assert(strmap.len() == 10);
	assert(strmap.get(strings[0]) != nullptr);
	assert(*strmap.get(strings[0]) == 0);

	strmap.del(strings[0]);
	assert(strmap.len() == 9);
	assert(strmap.get(strings[0]) == nullptr);

	// Delete everything else
	for (int i = 1; i < 10; i++) {
		strmap.del(strings[i]);
	}

	assert(strmap.len() == 0);
	assert(strmap.is_empty());

	// Test string keys with values
	Hashmap<StringContainer, Value> strmap2 = Hashmap<StringContainer, Value>();
	assert(strmap2.is_empty());

	for (int i = 0; i < 10; i++) {
		Value v = { i, i + 1 };
		strmap2.insert(strings[i], v);
	}

	assert(strmap2.len() == 10);
	assert(strmap2.get(strings[0]) != nullptr);
	assert(strmap2.get(strings[0])->value1 == 0);
	assert(strmap2.get(strings[0])->value2 == 1);

	strmap2.del(strings[0]);
	assert(strmap2.len() == 9);
	assert(strmap2.get(strings[0]) == nullptr);

	// Delete everything else
	for (int i = 1; i < 10; i++) {
		strmap2.del(strings[i]);
	}

	assert(strmap2.len() == 0);
	assert(strmap2.is_empty());

	return 0;
}