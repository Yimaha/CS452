#pragma once
#include <cstdint>


class Descriptor {
    public:
        Descriptor(
            uint32_t tid, Descriptor* parent, uint32_t priority,
            Descriptor* next_ready, Descriptor* next_send,
            void* stack
        );
        ~Descriptor();
        uint32_t tid;
        Descriptor* parent;
        uint32_t priority;
        Descriptor* next_ready;
        Descriptor* next_send;
        // Need run state when I figure out how to do that
        void* stack;
};
