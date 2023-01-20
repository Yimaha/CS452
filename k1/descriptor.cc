
#include "descriptor.h"

Descriptor::Descriptor(
    uint32_t tid, Descriptor* parent, uint32_t priority,
    Descriptor* next_ready, Descriptor* next_send,
    void* stack
) : tid(tid), parent(parent), priority(priority),
    next_ready(next_ready), next_send(next_send), stack(stack) {}

Descriptor::~Descriptor() {}