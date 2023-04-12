
#include "name_server.h"
#include "../etl/unordered_map.h"

using namespace Name;

void Name::name_server() {
	// Create the unordered map with a custom TKeyEqual
	etl::unordered_map<RequestBody, int, MAX_NAME_SERVER_SIZE> name_server = etl::unordered_map<RequestBody, int, MAX_NAME_SERVER_SIZE>();

	while (true) {
		// Receive
		int from;
		NameServerReq req;
		Message::Receive::Receive(&from, (char*)&req, NAME_REQ_LENGTH);

		// Check if the request is a register or whois
		if (req.header == Message::RequestHeader::REGISTER_AS) {
			// Add the name to the name server
			name_server[req.name] = from;
			Message::Reply::Reply(from, (char*)&from, sizeof(int));
		} else if (req.header == Message::RequestHeader::WHO_IS) {
			// Whois
			if (name_server.find(req.name) != name_server.end()) {
				// Send the tid
				Message::Reply::Reply(from, (char*)&name_server[req.name], sizeof(int));
			} else {
				// Name does not exist
				int reply = Exception::NAME_NOT_REGISTERED;
				Message::Reply::Reply(from, (char*)&reply, sizeof(int));
			}

		} else {
			// Invalid request
			Task::_KernelCrash("received invalid request of type %d at Name Server", req.header);
		}
	}
}
