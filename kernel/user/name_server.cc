
#include "name_server.h"
#include "../etl/unordered_map.h"

using namespace Name;

extern "C" void name_server() {
	// Create the unordered map with a custom TKeyEqual
	etl::unordered_map<NameContainer, int, MAX_NAME_SERVER_SIZE> name_server = etl::unordered_map<NameContainer, int, MAX_NAME_SERVER_SIZE>();

	while (true) {
		// Receive
		int from;
		NameServerReq req;
		MessagePassing::Receive::Receive(&from, (char*)&req, NAME_REQ_LENGTH);

		// Check if the request is a register or whois
		if (req.iden == Iden::REGISTER_AS) {
			// Add the name to the name server
			name_server[req.name] = from;
			MessagePassing::Reply::Reply(from, (char*)&from, sizeof(int));
		} else if (req.iden == Iden::WHO_IS) {
			// Whois
			if (name_server.find(req.name) != name_server.end()) {
				// Send the tid
				MessagePassing::Reply::Reply(from, (char*)&name_server[req.name], sizeof(int));
			} else {
				// Name does not exist
				int reply = Exception::NAME_NOT_REGISTERED;
				MessagePassing::Reply::Reply(from, (char*)&reply, sizeof(int));
			}

		} else {
			// Invalid request
			int reply = Exception::INVALID_IDEN;
			MessagePassing::Reply::Reply(from, (char*)&reply, sizeof(int));
		}
	}
}
