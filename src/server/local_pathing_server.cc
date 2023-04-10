
#include "local_pathing_server.h"
#include "global_pathing_server.h"
using namespace LocalPathing;
using namespace Planning;
using namespace Message;

const int SHORT_DELAY = 50;
const int LONG_DELAY = 300;

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
	PlanningServerReq req_to_global;

	auto send_to_dest_at_speed = [&addr, &req_to_global, internal_train_num](int dest, SpeedLevel speed) {
		req_to_global = { RequestHeader::GLOBAL_PATH, Planning::RequestBody { 0x0 } };
		req_to_global.body.routing_request.id = internal_train_num;
		req_to_global.body.routing_request.dest = dest;
		req_to_global.body.routing_request.speed = speed;
		Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
	};

	auto send_to_b1 = [&](SpeedLevel speed = SPEED_1, int delay = 0) {
		send_to_dest_at_speed(SENSOR_B[0], speed);
		Clock::Delay(addr.clock_tid, delay);
	};

	auto calibrate_velocity = [&](SpeedLevel level, bool from_up, int delay = 0) {
		req_to_global = { RequestHeader::GLOBAL_CALIBRATE_VELOCITY, Planning::RequestBody { 0x0 } };
		req_to_global.body.calibration_request.id = internal_train_num;
		req_to_global.body.calibration_request.level = level;
		req_to_global.body.calibration_request.from_up = from_up;
		Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

		Clock::Delay(addr.clock_tid, delay);
	};

	auto calibrate_starting = [&](SpeedLevel level, bool from_up, int delay = 0) {
		req_to_global = { RequestHeader::GLOBAL_CALIBRATE_STARTING, Planning::RequestBody { 0x0 } };
		req_to_global.body.calibration_request.id = internal_train_num;
		req_to_global.body.calibration_request.level = level;
		req_to_global.body.calibration_request.from_up = from_up;
		Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

		Clock::Delay(addr.clock_tid, delay);
	};

	auto calibrate_acceleration = [&](SpeedLevel from, SpeedLevel to, int delay = 0) {
		req_to_global = { RequestHeader::GLOBAL_CALIBRATE_ACCELERATION, Planning::RequestBody { 0x0 } };
		req_to_global.body.calibration_request_acceleration.id = internal_train_num;
		req_to_global.body.calibration_request_acceleration.from = from;
		req_to_global.body.calibration_request_acceleration.to = to;
		Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
		Clock::Delay(addr.clock_tid, delay);
	};

	auto calibrate_stopping_distance = [&](SpeedLevel level, bool from_up) {
		req_to_global = { RequestHeader::GLOBAL_CALIBRATE_STOPPING_DISTANCE, Planning::RequestBody { 0x0 } };
		req_to_global.body.calibration_request.id = internal_train_num;
		req_to_global.body.calibration_request.level = level;
		req_to_global.body.calibration_request.from_up = from_up;
		Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
	};

	while (true) {
		Receive::Receive(&from, reinterpret_cast<char*>(&req), sizeof(req));
		switch (req.header) {
		case RequestHeader::LOCAL_PATH_SET_PATH: {
			// idk send this to the global pathing server or something
			SpeedLevel speed = static_cast<SpeedLevel>(req.body.command.args[1]);
			send_to_dest_at_speed(req.body.command.args[0], speed);
			Reply::EmptyReply(from);
			break;
		}

		case RequestHeader::LOCAL_PATH_DEST: {
			PlanningServerReq req_to_global = { RequestHeader::GLOBAL_MULTI_PATH, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			req_to_global.body.routing_request.dest = req.body.command.args[0];
			Send::SendNoReply(addr.global_pathing_tid, reinterpret_cast<char*>(&req_to_global), sizeof(req_to_global));
			Reply::EmptyReply(from);
			break;
		}

		case RequestHeader::LOCAL_PATH_RNG: {
			PlanningServerReq req_to_global = { RequestHeader::GLOBAL_RNG, Planning::RequestBody { 0x0 } };
			req_to_global.body.routing_request.id = internal_train_num;
			for (int i = 0; i < 200; i++) {
				Send::SendNoReply(addr.global_pathing_tid, reinterpret_cast<char*>(&req_to_global), sizeof(req_to_global));
			}
			Reply::EmptyReply(from);
			break;
		}

		case RequestHeader::LOCAL_PATH_LOCATE: {

			PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			Reply::EmptyReply(from);
			break;
		}
		case RequestHeader::LOCAL_PATH_CALI: {
			// first locate

			PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// send yourself to sensor 16
			send_to_b1();

			// then start calibration at max speed
			calibrate_velocity(SPEED_MAX, true, SHORT_DELAY);
			send_to_b1();
			debug_print(addr.term_trans_tid, "Cali max speed for train %d finished\r\n", internal_train_num);

			// recalibrate at lower speed
			calibrate_velocity(SPEED_1, true, SHORT_DELAY);
			send_to_b1();
			debug_print(addr.term_trans_tid, "Cali slow speed from up for train %d finished\r\n", internal_train_num);

			// recalibrate at lower speed
			calibrate_velocity(SPEED_1, false, 0);
			debug_print(addr.term_trans_tid, "Cali slow speed from down for train %d finished\r\n", internal_train_num);

			Reply::EmptyReply(from);
			break;
		}

		case RequestHeader::LOCAL_PATH_CALI_ACCELERATION: {
			PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// send yourself to sensor 16
			send_to_b1(SPEED_1, LONG_DELAY);

			// then start calibration
			calibrate_starting(SPEED_1, true, LONG_DELAY);

			// send yourself to sensor 16
			send_to_b1(SPEED_1, LONG_DELAY);

			// then start calibration
			calibrate_starting(SPEED_MAX, true, LONG_DELAY);
			send_to_b1(SPEED_1, LONG_DELAY);
			// 4627,
			calibrate_acceleration(SPEED_1, SPEED_MAX, LONG_DELAY);
			send_to_b1(SPEED_1, LONG_DELAY);
			// 8707
			calibrate_acceleration(SPEED_MAX, SPEED_1, LONG_DELAY);

			break;
		}

		case RequestHeader::LOCAL_PATH_CALI_STOPPING_DISTANCE: {
			// first locate
			PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// go to 16
			send_to_b1(SPEED_1, LONG_DELAY);
			calibrate_stopping_distance(SPEED_MAX, false);

			// go to 16
			send_to_b1(SPEED_1, LONG_DELAY);
			calibrate_stopping_distance(SPEED_1, false);

			// go to 16
			send_to_b1(SPEED_1, LONG_DELAY);
			calibrate_stopping_distance(SPEED_1, true);
			break;
		}
		case RequestHeader::LOCAL_PATH_BUNNY_DIST: {
			// first locate
			PlanningServerReq req_to_global = { RequestHeader::GLOBAL_BUNNY_DIST, Planning::RequestBody { 0x0 } };
			req_to_global.body.pedding_request.id = internal_train_num;
			req_to_global.body.pedding_request.pedding = req.body.command.args[0];
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));
			break;
		}

		case RequestHeader::LOCAL_PATH_CALI_BASE_SPEED: {
			// first locate
			PlanningServerReq req_to_global = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
			req_to_global.body.command.id = internal_train_num;
			Send::SendNoReply(addr.global_pathing_tid, (const char*)&req_to_global, sizeof(req_to_global));

			// go to 16
			send_to_b1(SPEED_1, LONG_DELAY);

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
