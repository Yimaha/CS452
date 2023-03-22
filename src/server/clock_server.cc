#include "clock_server.h"
#include "../utils/printf.h"

using namespace Clock;
using namespace Message;

void Clock::clock_server() {
	Name::RegisterAs(CLOCK_SERVER_NAME);
	uint32_t ticks = 0;
	/**
	 * so, we can use a heap, or we can use linear search.
	 * I decided linear search is good enough for our purpose
	 * Since it is very unlikely that we will be hitting more than enough task.
	 * simiarly, we will have potential jittering due to priority queue, which is not something I want
	 * thus, linear is good. (though at the cost of slower insert, but who cares.)
	 *
	 * oh and the list is sorted insert, direct pop
	 */
	etl::list<etl::pair<int, uint32_t>, DELAY_QUEUE_SIZE> delay_queue = etl::list<etl::pair<int, uint32_t>, DELAY_QUEUE_SIZE>();
	while (true) {
		int from;
		ClockServerReq req;
		Message::Receive::Receive(&from, (char*)&req, sizeof(ClockServerReq));
		switch (req.header) {
		case Message::RequestHeader::NOTIFY_TIMER: {
			Message::Reply::EmptyReply(from); // unblock ticker right away
			ticks += 1;
			auto it = delay_queue.begin();
			while (it != delay_queue.end() && it->second == ticks) { // since we never skip any ticks we can do similar process to this
				Message::Reply::Reply(it->first, (const char*)&ticks, sizeof(ticks)); // unblock delayed task
				it = delay_queue.erase(it);
			}
			break;
		}

		case Message::RequestHeader::DELAY: {
			if (req.body.ticks == 0) {											 // instant reply since no delay is requesteds
				Message::Reply::Reply(from, (const char*)&ticks, sizeof(ticks)); // unblock delayed task
			} else {
				auto it = delay_queue.begin();
				while (true) {
					if (it == delay_queue.end() || it->second > (ticks + req.body.ticks)) {
						delay_queue.insert(it, etl::make_pair(from, ticks + req.body.ticks));
						break;
					} else {
						it++;
					}
				}
			}
			break;
		}

		case Message::RequestHeader::DELAY_UNTIL: {
			if (req.body.ticks <= ticks) {										 // instant reply since no delay is requesteds
				Message::Reply::Reply(from, (const char*)&ticks, sizeof(ticks)); // unblock delayed task
			} else {
				auto it = delay_queue.begin();
				while (true) {
					if (it == delay_queue.end() || it->second > req.body.ticks) {
						delay_queue.insert(it, etl::make_pair(from, req.body.ticks));
						break;
					} else {
						it++;
					}
				}
			}
			break;
		}
		case Message::RequestHeader::TIME: {
			Message::Reply::Reply(from, (const char*)&ticks, sizeof(ticks)); // should be 4 bytes
			break;
		}
		default: {
			// invalid request
			Task::_KernelCrash("invalid request for clock server");
			break;
		}
		}
	}
}

void Clock::clock_notifier() {
	// no need to register any name
	AddressBook addr = getAddressBook();
	ClockServerReq req = { Message::RequestHeader::NOTIFY_TIMER, { 0 } };
	while (true) {
		Interrupt::AwaitEvent(TIMER_INTERRUPT_ID);
		Message::Send::SendNoReply(addr.clock_tid, reinterpret_cast<const char*>(&req), sizeof(ClockServerReq));
	}
}
