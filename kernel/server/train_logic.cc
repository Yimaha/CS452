
#include "train_logic.h"
#include "../server/train_admin.h"

int train_num_to_index(int train_num) {
	for (int i = 0; i < TrainLogic::NUM_TRAINS; i++) {
		if (TrainLogic::TRAIN_NUMBERS[i] == train_num) {
			return i;
		}
	}
	return -1;
}

void TrainLogic::train_logic_server() {
	Name::RegisterAs(TRAIN_LOGIC_SERVER_NAME);

	int train_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	int from;
	TrainLogicServerReq req;

	char train_speeds[NUM_TRAINS] = { 0, 0, 0, 0, 0, 0 };

	// note, similar to UART::Putc, train server does not guarentee that command is fired right away
	while (true) {
		Message::Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(TrainLogicServerReq));
		switch (req.header) {
		case RequestHeader::SPEED: {
			char train_id = req.body.id;
			char desire_speed = req.body.action; // should be an integer within 0 - 31
			int train_index = train_num_to_index(train_id);

			Message::Reply::Reply(from, nullptr, 0); // unblock after job is done
			if (train_index == -1) {
				break;
			}

			train_speeds[train_index] = desire_speed;
			Train::TrainAdminReq req_to_admin = { Train::RequestHeader::SPEED, { train_id, desire_speed } };
			Message::Send::Send(train_tid, reinterpret_cast<char*>(&req_to_admin), sizeof(req_to_admin), nullptr, 0);
			break;
		}
		case RequestHeader::REV: {
			// note if action is passed, it speed is the desire final speed of the train
			char train_id = req.body.id;
			int train_index = train_num_to_index(train_id);

			// For whatever reason, these prints broke the train logic server.
			// Something to investigate.
			// Terminal::Putc(terminal_tid, '0' + train_id);
			// Terminal::Putc(terminal_tid, '0' + original_speed);

			Message::Reply::Reply(from, nullptr, 0); // unblock after job is done
			if (train_index == -1) {
				break;
			}

			char original_speed = train_speeds[train_index];
			Train::TrainAdminReq req_to_admin = { Train::RequestHeader::REV, { train_id, original_speed } };
			Message::Send::Send(train_tid, reinterpret_cast<char*>(&req_to_admin), sizeof(req_to_admin), nullptr, 0);
			break;
		}
		default: {
			char exception[50];
			sprintf(exception, "Train Logic: Illegal type: [%d]\r\n", req.header);
			Task::_KernelPrint(exception);
			while (1) {
			}
		}
		}
	}
}
