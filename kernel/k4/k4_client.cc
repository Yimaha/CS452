#include "k4_client.h"

void SystemTask::k4_dummy() {
	while (1) {
        char c = UART::GetC(UART::UART_0_RECEIVER_TID, 0);
        Task::_KernelPrint(&c);
        UART::PutC(UART::UART_0_TRANSMITTER_TID, 0, c);
    }
}
