#include "train_admin.h"


void Train::train_admin() {
    Name::RegisterAs(TRAIN_SERVER_NAME);

    const int POOL_SIZE = 30;
    etl::queue<int, POOL_SIZE> courier_pool;
    for(int i = 0; i < POOL_SIZE; i++) {
        courier_pool.push(Task::Create(2, &train_courier));
    }
    int uart_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);
    char command[2];
	int from;
	TrainAdminReq req;

    // note, similar to UART::Putc, train server does not guarentee that command is fired right away
	while (true) {
		Message::Receive::Receive(&from, (char*)&req, sizeof(TrainAdminReq));
		switch (req.header) {
		case RequestHeader::SPEED: {
            char train_id = req.body.id;
            char desire_speed = req.body.action; // should be an integer within 0 - 31
            command[0] = desire_speed;
            command[1] = train_id;
            UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
            Message::Reply::Reply(from, nullptr, 0); // unblock after job is done
			break;
		} 
        case RequestHeader::REV: {
            // note if action is passed, it speed is the desire final speed of the train
            char train_id = req.body.id;
            command[0] = 0;
            command[1] = train_id;
            UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
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
    char command[2];
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
            command[0] = REV_COMMAND;
            command[1] = train_id;
            UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
            command[0] = desire_speed;
            UART::Puts(uart_tid, TRAIN_UART_CHANNEL, command, 2);
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