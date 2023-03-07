
#pragma once
#define MAX_MAP_SIZE 256
#include "utility.h"

/*
 * A simple hashmap implementation.
 * This is a template class, so it can be used with any data type.
 */
template <typename T, typename U>
class Hashmap {
public:
	Hashmap();
	~Hashmap();
	bool is_empty();
	bool is_full();
	int len();
	bool insert(T key, U data);
	bool del(T key);
	U* get(T key);
	int hash(T* t);

private:
	struct MapItem {
		bool used;
		T t;
		U u;
	};

	// only accept non-negative keys
	MapItem buffer[MAX_MAP_SIZE];
	uint32_t size;
};

template <typename T, typename U>
Hashmap<T, U>::Hashmap()
	: size { 0 } {

	for (int i = 0; i < MAX_MAP_SIZE; i++) {
		buffer[i] = { false, T(), U() };
	}
}

template <typename T, typename U>
Hashmap<T, U>::~Hashmap() { }

template <typename T, typename U>
bool Hashmap<T, U>::is_empty() {
	return size == 0;
}

template <typename T, typename U>
bool Hashmap<T, U>::is_full() {
	return size == MAX_MAP_SIZE - 1;
}

/*
 * Returns the number of items in the map.
 */
template <typename T, typename U>
int Hashmap<T, U>::len() {
	return size;
}

/*
 * Returns the hash of the key as an int.
 * This is a very simple hash function that is not cryptographically secure.
 * It is based on the FNV-1a hash function,
 * but modified to work with arbitrary data types
 * by converting the data to an array of uint64_t's.
 * See https://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
 * for more information.
 */
template <typename T, typename U>
int Hashmap<T, U>::hash(T* t) {
	// Convert to char array
	const uint64_t* c = reinterpret_cast<const uint64_t*>(t);
	uint64_t digest = 14695981039346656037ULL;
	size_t i = 0;
	for (; i < sizeof(T) / 8; i++) {
		digest = digest ^ c[i];
		digest = digest * 1099511628211;
	}

	// This calculates a mask like 0xffff00..00
	int shift = 8 * (sizeof(T) % 8);
	uint64_t mask = ((1 << shift) - 1) << (64 - shift);

	// Now we can handle the last few bytes
	digest = digest ^ ((c[i] & mask) >> (64 - shift));
	digest = digest * 1099511628211;
	return digest % MAX_MAP_SIZE;
}

/*
 * Inserts the key and data into the map.
 * Returns true if the key was inserted, false otherwise.
 */
template <typename T, typename U>
bool Hashmap<T, U>::insert(T key, U data) {
	if (is_full()) {
		// print("unable to insert\r\n", 18);
		return false;
	} else {
		int digest = hash(&key);
		while (buffer[digest].used) {
			digest = (digest + 1) % MAX_MAP_SIZE;
		}
		buffer[digest] = { true, key, data };
		size++;
		return true;
	}
}

/*
 * Deletes the key from the map.
 * Returns true if the key was found and deleted, false otherwise.
 */
template <typename T, typename U>
bool Hashmap<T, U>::del(T key) {
	if (is_empty()) {
		// print("unable to delete\r\n", 18);
		return false;
	} else {
		int digest = hash(&key);
		while (!buffer[digest].used || buffer[digest].t != key) {
			digest = (digest + 1) % MAX_MAP_SIZE;
		}

		buffer[digest] = { false, T(), U() };
		size--;
		return true;
	}
}

/*
 * Returns a pointer to the data associated with the key.
 * If the key is not found, returns nullptr.
 */
template <typename T, typename U>
U* Hashmap<T, U>::get(T key) {
	if (is_empty()) {
		return nullptr;
	} else {
		int digest = hash(&key);
		int num_tries = 0;
		while (!buffer[digest].used || buffer[digest].t != key) {
			if (num_tries == MAX_MAP_SIZE) {
				return nullptr;
			}

			digest = (digest + 1) % MAX_MAP_SIZE;
			num_tries++;
		}

		return &buffer[digest].u;
	}
}
