#include "train_admin.h"


void Train::train_admin() {
    Name::RegisterAs(TRAIN_SERVER_NAME);

    const int POOL_SIZE = 30;
    etl::queue<int, POOL_SIZE> courier_pool;
    for(int i = 0; i < POOL_SIZE; i++) {
        courier_pool.push(Task::Create(2, &train_courier));
    }
    int uart_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);

	int from;
	TrainAdminReq req;

    // note, similar to UART::PutC, train server does not guarentee that command is fired right away
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainAdminReq));
		switch (req.header) {
		case RequestHeader::SPEED: {
            char train_id = req.body.id;
            char desire_speed = req.body.action; // should be an integer within 0 - 31
            // char st[100];
            // sprintf(st, "reached speed, train_id: %d, desire_speed, %d, TRAIN_UART_CHANNEL: %d, uart_tid: %d \r\n", train_id, desire_speed, TRAIN_UART_CHANNEL, uart_tid);
            // Task::_KernelPrint(st);
            UART::PutC(uart_tid, TRAIN_UART_CHANNEL, desire_speed);
            UART::PutC(uart_tid, TRAIN_UART_CHANNEL, train_id);
            Message::Reply::Reply(from, nullptr, 0); // unblock after job is done
			break;
		} 
        case RequestHeader::REV: {
            // note if action is passed, it speed is the desire final speed of the train
            char train_id = req.body.id;
            UART::PutC(uart_tid, TRAIN_UART_CHANNEL, 0);
            UART::PutC(uart_tid, TRAIN_UART_CHANNEL, train_id);
            TrainCourierReq req_to_courier = {CourierRequestHeader::REV, {train_id, req.body.action}};
            Message::Send::Send(courier_pool.front(), (const char*) &req_to_courier, sizeof(TrainCourierReq), nullptr, 0);
            Message::Reply::Reply(from, nullptr, 0); // unblock after job is done
			break;
        }
        case RequestHeader::COURIER_COMPLETE: {
            Message::Reply::Reply(from, nullptr, 0);
            courier_pool.push(from);
            break;
        }
		default: {
			char exception[30];
			sprintf(exception, "illegal type: [%d]\r\n", req.header);
			Task::_KernelPrint(exception);
			while (1) {
			}
		}
		}
	}
}

void Train::train_courier() {
    int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
    int train_admin_tid = Name::WhoIs(TRAIN_SERVER_NAME);
    int uart_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);
    int from;
    TrainCourierReq req;
    TrainAdminReq req_to_admin;


    // worker only has few types 
    while(1) {
        Message::Receive::Receive(&from, (char*)&req, sizeof(TrainCourierReq));
        Message::Reply::Reply(from, nullptr, 0); // unblock caller right away
    	switch (req.header) {
        case CourierRequestHeader::REV: {
            // it wait for about 4 seconds then send in the command to reverse and speed up
            char train_id = req.body.id;
            char desire_speed = req.body.action; // should be an integer within 0 - 31
            Clock::Delay(clock_tid, 400);
            UART::PutC(uart_tid, TRAIN_UART_CHANNEL, REV_COMMAND);
            UART::PutC(uart_tid, TRAIN_UART_CHANNEL, train_id);
            UART::PutC(uart_tid, TRAIN_UART_CHANNEL, desire_speed);
            UART::PutC(uart_tid, TRAIN_UART_CHANNEL, train_id);
	        req_to_admin = { RequestHeader::COURIER_COMPLETE, RequestBody{ 0x0, 0x0 } };

            Message::Send::Send(train_admin_tid, (const char*) &req_to_admin, sizeof(TrainAdminReq), nullptr, 0);
			break;
        }
		default: {
			char exception[30];
			sprintf(exception, "illegal type: [%d]\r\n", req.header);
			Task::_KernelPrint(exception);
			while (1) {
			}
		}
		}
	}
}