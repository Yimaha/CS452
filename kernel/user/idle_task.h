
#pragma once

namespace SystemTask
{
void k3_client_task();

const char IDLE_TASK_NAME[] = "idle_task";
const int IDLE_TID = 4;
void idle_task();
}