
#include "local_pathing_server.h"
#include "global_pathing_server.h"
using namespace LocalPathing;
using namespace Message;

void LocalPathing::local_pathing_worker() {
	int from;
	int internal_train_num;
	char buf[LOCAL_PATHING_BUFLEN];
	LocalPathingReq req;
	Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(req));

	while (req.header != Message::RequestHeader::LOCAL_PATH_SET_TRAIN) {
		Reply::EmptyReply(from);
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(req));
	}

	internal_train_num = req.body.train_num;
	Reply::EmptyReply(from);
	sprintf(buf, LOCAL_PATHING_NAME, internal_train_num);
	Name::RegisterAs(buf);

	AddressBook addr = Message::getAddressBook();

	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(req));
		switch (req.header) {
		case RequestHeader::LOCAL_PATH_SET_PATH: {
			// idk send this to the global pathing server or something

			Planning::PlanningServerReq req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = req.body.command.args[0];
			req_to_global.body.routing_request.speed = static_cast<Planning::SpeedLevel>(req.body.command.args[1]);
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Reply::EmptyReply(from);
			break;
		}
		case RequestHeader::LOCAL_PATH_LOOP: {
			// first locate
			Planning::PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, reinterpret_cast<char*>(&req_to_global), sizeof(req_to_global));

			// send yourself to sensor 16, aka B1
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_MAX;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// then loop
			req_to_global = { RequestHeader::GLOBAL_LOOP, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			Planning::SpeedLevel speed
				= (req.body.command.num_args == 0) ? Planning::SpeedLevel::SPEED_MAX : static_cast<Planning::SpeedLevel>(req.body.command.args[0]);
			req_to_global.body.routing_request.speed = speed;

			Send::SendNoReply(addr.global_pathing_tid, reinterpret_cast<char*>(&req_to_global), sizeof(req_to_global));
			Reply::EmptyReply(from);
			break;
		}
		case RequestHeader::LOCAL_PATH_EXLOOP: {
			Planning::PlanningServerReq req_to_global = { RequestHeader::GLOBAL_EXIT_LOOP, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = req.body.command.args[0];

			int offset = (req.body.command.num_args >= 2) ? req.body.command.args[1] : 0;
			debug_print(addr.term_trans_tid, "Offset: %d\r\n", offset);
			req_to_global.body.routing_request.offset = offset;

			Send::SendNoReply(addr.global_pathing_tid, reinterpret_cast<char*>(&req_to_global), sizeof(req_to_global));
			Reply::EmptyReply(from);
			break;
		}
		case RequestHeader::LOCAL_PATH_LOCATE: {

			Planning::PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Reply::EmptyReply(from);
			break;
		}
		case RequestHeader::LOCAL_PATH_INIT: {
			debug_print(addr.term_trans_tid, "LOCAL_PATH_INIT_%d", internal_train_num);
			for (uint32_t i = 0; i < req.body.command.num_args; ++i) {
				debug_print(addr.term_trans_tid, " %d", req.body.command.args[i]);
			}

			debug_print(addr.term_trans_tid, "\r\n");
			Reply::EmptyReply(from);
			break;
		}
		case RequestHeader::LOCAL_PATH_CALI: {
			// first locate

			Planning::PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// send your self to sensor 16
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// then start calibration at max speed
			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_VELOCITY, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request.id = internal_train_num;
			req_to_global.body.calibration_request.level = Planning::SpeedLevel::SPEED_MAX;
			req_to_global.body.calibration_request.from_up = true;
			Send::Send(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global), nullptr, 0);

			Clock::Delay(addr.clock_tid, 50);
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// recalibrate at lower speed
			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_VELOCITY, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request.id = internal_train_num;
			req_to_global.body.calibration_request.level = Planning::SpeedLevel::SPEED_1;
			req_to_global.body.calibration_request.from_up = true;
			Send::Send(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global), nullptr, 0);

			Clock::Delay(addr.clock_tid, 50);
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// recalibrate at lower speed
			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_VELOCITY, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request.id = internal_train_num;
			req_to_global.body.calibration_request.level = Planning::SpeedLevel::SPEED_1;
			req_to_global.body.calibration_request.from_up = false;
			Send::Send(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global), nullptr, 0);

			Reply::EmptyReply(from);
			break;
		}

		case RequestHeader::LOCAL_PATH_CALI_ACCELERATION: {
			Planning::PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// send your self to sensor 16
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Clock::Delay(addr.clock_tid, 300);

			// then start calibration
			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_STARTING, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request.id = internal_train_num;
			req_to_global.body.calibration_request.level = Planning::SpeedLevel::SPEED_1;
			req_to_global.body.calibration_request.from_up = true;
			Send::Send(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global), nullptr, 0);
			Clock::Delay(addr.clock_tid, 300);

			// send your self to sensor 16
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Clock::Delay(addr.clock_tid, 300);

			// then start calibration
			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_STARTING, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request.id = internal_train_num;
			req_to_global.body.calibration_request.level = Planning::SpeedLevel::SPEED_MAX;
			req_to_global.body.calibration_request.from_up = true;
			Send::Send(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global), nullptr, 0);
			Clock::Delay(addr.clock_tid, 300);

			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Clock::Delay(addr.clock_tid, 300);

			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_ACCELERATION, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request_acceleration.id = internal_train_num;
			req_to_global.body.calibration_request_acceleration.from = Planning::SpeedLevel::SPEED_1;
			req_to_global.body.calibration_request_acceleration.to = Planning::SpeedLevel::SPEED_MAX;
			Send::Send(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global), nullptr, 0);
			Clock::Delay(addr.clock_tid, 300);

			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Clock::Delay(addr.clock_tid, 300);

			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_ACCELERATION, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request_acceleration.id = internal_train_num;
			req_to_global.body.calibration_request_acceleration.from = Planning::SpeedLevel::SPEED_MAX;
			req_to_global.body.calibration_request_acceleration.to = Planning::SpeedLevel::SPEED_1;
			Send::Send(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global), nullptr, 0);
			Clock::Delay(addr.clock_tid, 300);

			break;
		}

		case RequestHeader::LOCAL_PATH_CALI_STOPPING_DISTANCE: {
			// first locate
			Planning::PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// go to 16
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Clock::Delay(addr.clock_tid, 300);

			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_STOPPING_DISTANCE, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request.id = internal_train_num;
			req_to_global.body.calibration_request.level = Planning::SpeedLevel::SPEED_MAX;
			req_to_global.body.calibration_request.from_up = false;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// go to 16
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Clock::Delay(addr.clock_tid, 300);

			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_STOPPING_DISTANCE, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request.id = internal_train_num;
			req_to_global.body.calibration_request.level = Planning::SpeedLevel::SPEED_1;
			req_to_global.body.calibration_request.from_up = false;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// go to 16
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Clock::Delay(addr.clock_tid, 300);

			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_STOPPING_DISTANCE, Planning::RequestBody { 0x0 } };
			req_to_global.body.calibration_request.id = internal_train_num;
			req_to_global.body.calibration_request.level = Planning::SpeedLevel::SPEED_1;
			req_to_global.body.calibration_request.from_up = true;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			break;
		}

		case RequestHeader::LOCAL_PATH_CALI_BASE_SPEED: {
			// first locate
			Planning::PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// go to 16
			req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = 16;
			req_to_global.body.routing_request.speed = Planning::SpeedLevel::SPEED_1;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Clock::Delay(addr.clock_tid, 300);

			req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			req_to_global = { RequestHeader::GLOBAL_CALIBRATE_BASE_VELOCITY, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			Reply::EmptyReply(from);
			break;
		}
		default:
			Task::_KernelCrash("Error in %s at line %d, msg: %s, header: %d", __FILE__, __LINE__, "Unknown request header", req.header);
		} // switch
	}
}