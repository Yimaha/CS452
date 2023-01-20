#pragma once

#define MAX_BUFFER_SIZE 500

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