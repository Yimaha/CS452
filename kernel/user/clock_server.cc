#include "clock_server.h"

using namespace Clock;

extern "C" void clock_server() {
	Name::RegisterAs(CLOCK_SERVER_NAME);
	uint32_t ticks = 0;
	/**
	 * so, we can use a heap, or we can use linear search.
	 * I decided linear search is good enough for our purpose
	 * Since it is very unlikely that we will be hitting more than enough task.
	 * simiarly, we will have potential jitterring due to priority queue, which is not something I want
	 * thus, linear is good. (though at the cost of slower insert, but who cares.)
	 *
	 * oh and the list is sorted insert, direct pop
	 */
	etl::list<etl::pair<int, uint32_t>, DELAY_QUEUE_SIZE> delay_queue;
	while (1) {
		int from;
		ClockServerReq req;
		Message::Receive::Receive(&from, (char*)&req, sizeof(ClockServerReq));
		switch (req.header) {
		case RequestHeader::NOTIFY: {
			Message::Reply::Reply(from, nullptr, 0); // unblock ticker right away
			ticks += 1;
			auto it = delay_queue.begin();
			while (it != delay_queue.end() && it->second == ticks) {						 // since we never skip any ticks we can do similar process to this
				Message::Reply::Reply(it->first, (const char*)&ticks, sizeof(ticks)); // unblock delayed task
				it = delay_queue.erase(it);
			}
			break;
		}
		case RequestHeader::DELAY: {
			if (req.body.ticks == 0) {													// instant reply since no delay is requesteds
				Message::Reply::Reply(from, (const char*)&ticks, sizeof(ticks)); // unblock delayed task
			} else {
				auto it = delay_queue.begin();
				while (1) {
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
		case RequestHeader::DELAY_UNTIL: {
			if (req.body.ticks == ticks) {												// instant reply since no delay is requesteds
				Message::Reply::Reply(from, (const char*)&ticks, sizeof(ticks)); // unblock delayed task
			} else {
				auto it = delay_queue.begin();
				while (1) {
					if (it == delay_queue.end() || it->second > (ticks + req.body.ticks)) {
						delay_queue.insert(it, etl::make_pair(from, req.body.ticks));
						break;
					} else {
						it++;
					}
				}
			}
			break;
		}
		case RequestHeader::TIME: {
			Message::Reply::Reply(from, (const char*)&ticks, sizeof(ticks)); // should be 4 bytes
			break;
		}
		}
	}
}

extern "C" void clock_notifier() {
	// no need to register any name
	int clock_tid = Name::WhoIs(CLOCK_SERVER_NAME);
	while (1) {
		Interrupt::AwaitEvent(Clock::TIMER_INTERRUPT_ID); // temporarily 1
		Message::Send::Send(clock_tid, nullptr, 0, nullptr, 0);
	}	
}
