#include "k4_client.h"

void SystemTask::k4_dummy() {
	while (1) {
        char c = UART::GetC(UART::UART_0_RECEIVER_TID, 0);
        UART::PutC(UART::UART_0_TRANSMITTER_TID, 0, c);
    }
}


void SystemTask::k4_dummy_train() {
    int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
    int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
    Train::TrainAdminReq req = {Train::RequestHeader::SPEED, Train::RequestBody{2, 0}};
	while (1) {
        for ( int i = 0; i < 10; i ++) {
            req.body.id = 2;
            req.body.action = i * 2 ;
            Message::Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
            Clock::Delay(clock_tid, 100);
        }
    }
}


void SystemTask::k4_dummy_train_rev() {
    int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
    int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
    Train::TrainAdminReq req = {Train::RequestHeader::SPEED, Train::RequestBody{2, 0}};
    req.body.id = 2;
    req.body.action = 10 ;
    Message::Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
    Clock::Delay(clock_tid, 100);
	while (1) {
        req.header = Train::RequestHeader::REV;
        req.body.id = 2;
        req.body.action = 10;
        Message::Send::Send(train_tid, (const char*)&req, sizeof(req), nullptr, 0);
        Clock::Delay(clock_tid, 1000);   
    }
}



void SystemTask::k4_dummy_train_sensor() {

    int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
    int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
    int uart_send_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);
    int uart_receive_tid = Name::WhoIs(UART::UART_1_RECEIVER);
    UART::PutC(uart_send_tid, 1, 133);
    for (int i = 0; i < 10; i++) {
        UART::GetC(uart_receive_tid, 1);
    }
    Task::_KernelPrint("down\r\n");
    Task::Exit();
}