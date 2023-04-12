#include "request_header.h"
#include "clock_server.h"
#include "global_pathing_server.h"
#include "local_pathing_server.h"
#include "train_admin.h"
#include "uart_server.h"
#include "track_server.h"

Message::AddressBook Message::getAddressBook() {
	AddressBook book;
	book.clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	book.term_trans_tid = Name::WhoIs(UART::UART_0_TRANSMITTER);
	book.term_receive_tid = Name::WhoIs(UART::UART_0_RECEIVER);
	book.train_trans_tid = Name::WhoIs(UART::UART_1_TRANSMITTER);
	book.train_receive_tid = Name::WhoIs(UART::UART_1_RECEIVER);
	book.train_admin_tid = Name::WhoIs(Train::TRAIN_SERVER_NAME);
	book.sensor_admin_tid = Name::WhoIs(Sensor::SENSOR_ADMIN_NAME);
	book.terminal_admin_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	book.global_pathing_tid = Name::WhoIs(Planning::GLOBAL_PATHING_SERVER_NAME);
	book.track_server_tid = Name::WhoIs(Track::TRACK_SERVER_NAME);

	char buf[Name::MAX_NAME_LENGTH];
	for (int i = 0; i < Train::NUM_TRAINS; ++i) {
		int train_num = Train::TRAIN_NUMBERS[i];
		sprintf(buf, LocalPathing::LOCAL_PATHING_NAME, train_num);
		book.local_pathing_tids[i] = Name::WhoIs(buf);
	}

	return book;
}
