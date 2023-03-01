
#pragma once
#include "../kernel.h"
#include "../server/uart_server.h"
#include "../server/train_admin.h"
#include "../server/sensor_admin.h"
#include "../utils/printf.h"
#include "../user/user_tasks.h"

namespace SystemTask
{

void k4_dummy(); 
void k4_dummy_train();
void k4_dummy_train_rev();
void k4_dummy_train_sensor();
void k4_dummy_train_switch();
}