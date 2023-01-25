#pragma once
#define MAX_BUFFER_SIZE 512

template <typename T>
class RingBuffer {
    public:
        RingBuffer();
        ~RingBuffer();
        bool is_empty();
        bool is_full();
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
RingBuffer<T>::RingBuffer() : head(0), tail(0), size(0) {}

template <typename T>
RingBuffer<T>::~RingBuffer() {}

template <typename T>
bool RingBuffer<T>::is_empty() {
    return size == 0;
}

template <typename T>
bool RingBuffer<T>::is_full() {
    return size == MAX_BUFFER_SIZE - 1;
}

template <typename T>
T RingBuffer<T>::top() {
    if (is_empty()) {
        return 0;
    }
    return buffer[head];
}

template <typename T>
T RingBuffer<T>::bottom() {
    if (is_empty()) {
        return 0;
    }
    return buffer[(tail - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE];
}

template <typename T>
void RingBuffer<T>::push_front(T data) {
    if (is_full()) {
        return;
    }
    head = (head - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
    buffer[head] = data;
    size++;
}

template <typename T>
void RingBuffer<T>::push_back(T data) {
    if (is_full()) {
        return;
    }
    buffer[tail] = data;
    tail = (tail + 1) % MAX_BUFFER_SIZE;
    size++;
}

template <typename T>
T RingBuffer<T>::pop_front() {
    if (is_empty()) {
        return 0;
    }
    T data = buffer[head];
    head = (head + 1) % MAX_BUFFER_SIZE;
    size--;
    return data;
}

template <typename T>
T RingBuffer<T>::pop_back() {
    if (is_empty()) {
        return 0;
    }
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
