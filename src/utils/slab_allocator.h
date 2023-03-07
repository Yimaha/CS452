

// this should be templated class that allocates space based on the object passed to it automatically.
// relying on a ring buffer.

#pragma once

#include "buffer.h"
#include "utility.h"

template <typename T, typename... Args>
class SlabAllocator {
public:
	SlabAllocator(char* starting_location, int total_slabs);
	~SlabAllocator();
	T* get(Args... arguments);
	void del(T* target); // when you done with the memory please push it back
	int get_remaining_size();

private:
	int size;
	int T_size;
	RingBuffer<char*> slabs; // N
};

template <typename T, typename... Args>
SlabAllocator<T, Args...>::SlabAllocator(char* starting_location, int total_slabs)
	: size(total_slabs)
	, T_size { sizeof(T) } {
	if (size > MAX_BUFFER_SIZE) {
		print("Slab Allocator received too big of size!\r\n", 42);
	}
	for (int i = 0; i < total_slabs; i++) {
		slabs.push_back(starting_location);
		starting_location += T_size;
	}
}

template <typename T, typename... Args>
SlabAllocator<T, Args...>::~SlabAllocator() { }

template <typename T, typename... Args>
T* SlabAllocator<T, Args...>::get(Args... arguments) {
	if (slabs.is_empty()) {
		print("all slabs are allocated!\r\n", 26);
		return nullptr;
	}
	char* address = slabs.pop_front();
	return new (address) T(arguments...);
}

template <typename T, typename... Args>
void SlabAllocator<T, Args...>::del(T* target) {
	char* address = (char*)target;
	slabs.push_back(address); // put the address back in the queue.
}

template <typename T, typename... Args>
int SlabAllocator<T, Args...>::get_remaining_size() {
	return slabs.size; // how many slabs we have left
}
