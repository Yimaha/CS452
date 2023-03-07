
#pragma once
#include "../kernel.h"
#include "../server/clock_server.h"
#include "../utils/printf.h"
#include "../user/user_tasks.h"

namespace SystemTask
{

const int IDLE_BUFSIZE = 64;
const int CLIENT_BUFSIZE = 2;
void k3_client_task(); 
}