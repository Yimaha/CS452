
#include "../src/utils/buffer.h"
#include <assert.h>

int main() {
	// Let's test the buffer
	// with some unit tests

	RingBuffer<int> buffer;

	assert(buffer.is_empty());
	assert(!buffer.is_full());
	assert(buffer.top() == 0);
	assert(buffer.bottom() == 0);

	buffer.push_front(1);
	assert(buffer.top() == 1);
	assert(buffer.bottom() == 1);

	buffer.push_front(2);
	assert(buffer.top() == 2);
	assert(buffer.bottom() == 1);

	buffer.push_back(3);
	assert(buffer.top() == 2);
	assert(buffer.bottom() == 3);

	buffer.push_back(4);
	assert(buffer.top() == 2);
	assert(buffer.bottom() == 4);

	assert(buffer.pop_front() == 2);
	assert(buffer.top() == 1);
	assert(buffer.bottom() == 4);

	assert(buffer.pop_back() == 4);
	assert(buffer.top() == 1);
	assert(buffer.bottom() == 3);

	buffer.push_back(5);
	assert(buffer.top() == 1);
	assert(buffer.bottom() == 5);

	buffer.push_front(6);
	assert(buffer.top() == 6);
	assert(buffer.bottom() == 5);

	assert(buffer.pop_front() == 6);
	assert(buffer.top() == 1);
	assert(buffer.bottom() == 5);

	assert(buffer.pop_back() == 5);
	assert(buffer.top() == 1);
	assert(buffer.bottom() == 3);

	assert(buffer.pop_back() == 3);
	assert(buffer.top() == 1);
	assert(buffer.bottom() == 1);

	assert(buffer.pop_back() == 1);
	assert(buffer.top() == 0);
	assert(buffer.bottom() == 0);

	assert(buffer.is_empty());
	assert(!buffer.is_full());
}