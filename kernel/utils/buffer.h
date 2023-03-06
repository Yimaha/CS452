#pragma once
#define MAX_BUFFER_SIZE 256
#include "../rpi.h"
#include "utility.h"

template <typename T>
class RingBuffer {
public:
	RingBuffer();
	~RingBuffer();
	bool is_empty();
	bool is_full();
	int length();
	T top();
	T bottom();
	void push_front(T data);
	void push_back(T data);
	T pop_front();
	T pop_back();
	void insert_sorted(T data);

private:
	T buffer[MAX_BUFFER_SIZE];
	int head;
	int tail;
	int size;
};

template <typename T>
RingBuffer<T>::RingBuffer()
	: head(0)
	, tail(0)
	, size(0) { }

template <typename T>
RingBuffer<T>::~RingBuffer() { }

template <typename T>
bool RingBuffer<T>::is_empty() {
	return size == 0;
}

template <typename T>
bool RingBuffer<T>::is_full() {
	return size == MAX_BUFFER_SIZE - 1;
}

template <typename T>
int RingBuffer<T>::length() {
	return size;
}

template <typename T>
T RingBuffer<T>::top() {
#ifdef OUR_DEBUG
	if (is_empty()) {
		return 0;
	}
#endif
	return buffer[head];
}

template <typename T>
T RingBuffer<T>::bottom() {
#ifdef OUR_DEBUG
	if (is_empty()) {
		return 0;
	}
#endif
	return buffer[(tail - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE];
}

template <typename T>
void RingBuffer<T>::push_front(T data) {
#ifdef OUR_DEBUG
	if (is_full()) {
		return;
	}
#endif
	head = (head - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
	buffer[head] = data;
	size++;
}

template <typename T>
void RingBuffer<T>::push_back(T data) {
#ifdef OUR_DEBUG
	if (is_full()) {
		return;
	}
#endif
	buffer[tail] = data;
	tail = (tail + 1) % MAX_BUFFER_SIZE;
	size++;
}

template <typename T>
T RingBuffer<T>::pop_front() {
#ifdef OUR_DEBUG
	if (is_empty()) {
		print("trying to pop from an empty buffer \r\n", 37);
		restart();
	}
#endif
	T data = buffer[head];
	head = (head + 1) % MAX_BUFFER_SIZE;
	size--;
	return data;
}

template <typename T>
T RingBuffer<T>::pop_back() {
#ifdef OUR_DEBUG
	if (is_empty()) {
		print("trying to pop from an empty buffer \r\n", 37);
		restart();
	}
#endif
	tail = (tail - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
	T data = buffer[tail];
	size--;
	return data;
}

template <typename T>
void RingBuffer<T>::insert_sorted(T data) {
	if (is_full()) {
		return;
	} else if (data > buffer[(tail - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE]) {
		push_back(data);
		return;
	}

	int i = (tail - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
	for (; i != head; i = (i - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE) {
		if (buffer[i] <= data) {
			break;
		}
	}

	int j = tail;
	for (; j != i; j = (j - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE) {
		buffer[j] = buffer[(j - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE];
	}
	buffer[i] = data;
	tail = (tail + 1) % MAX_BUFFER_SIZE;
	size++;
}
