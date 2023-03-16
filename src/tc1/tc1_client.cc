#include "tc1_client.h"
#include "../server/global_pathing_server.h"
using namespace Message;
using namespace Planning;
void SystemTask::tc1_dummy() {
	AddressBook addr = getAddressBook();

	Clock::Delay(addr.clock_tid, 100);

	Planning::PlanningServerReq req = { RequestHeader::GLOBAL_LOCATE, Planning::RequestBody { 0x0 } };
	req.body.command.id = 24;
	req.body.command.action = 77;

	Send::Send(addr.global_pathing_tid, (const char*)&req, sizeof(req), nullptr, 0);

	req = { RequestHeader::GLOBAL_CALIBRATE_VELOCITY, Planning::RequestBody { 0x0 } };
	req.body.calibration_request.id = 24;
	req.body.calibration_request.level = SpeedLevel::SPEED_1;
	req.body.calibration_request.from_up = true;

	Send::Send(addr.global_pathing_tid, (const char*)&req, sizeof(req), nullptr, 0);
	Task::Exit();
}
