
#include "../src/etl/unordered_map.h"
#include "../src/kernel.h"
#include <cassert>
#include <iostream>

#define MAX_MAP_SIZE 256

using namespace etl;

int main(void) {
	// Create the unordered map with a custom TKeyEqual
	unordered_map<int, int, MAX_MAP_SIZE> name_server = unordered_map<int, int, MAX_MAP_SIZE>();
	std::cout << sizeof(name_server) << std::endl;
	std::cout << sizeof(int) << std::endl;

	// Let's get some unit tests on this
	name_server[1] = 1;
	name_server[2] = 2;
	name_server[3] = 3;

	assert(name_server[1] == 1);
	assert(name_server[2] == 2);
	assert(name_server[3] == 3);

	// Okay ints worked, let's try Name objects
	unordered_map<MessagePassing::NameServer::Name, int, MAX_MAP_SIZE> name_server2 = unordered_map<MessagePassing::NameServer::Name, int, MAX_MAP_SIZE>();
	std::cout << sizeof(name_server2) << std::endl;
	std::cout << sizeof(MessagePassing::NameServer::Name) << std::endl;

	MessagePassing::NameServer::Name name1 = { "name1" };
	MessagePassing::NameServer::Name name2 = { "name2" };
	MessagePassing::NameServer::Name name3 = { "name3" };
	MessagePassing::NameServer::Name name4 = { "name4" };
	MessagePassing::NameServer::Name name5 = { "namenamenamena" };
	MessagePassing::NameServer::Name name6 = { "namenamenamenab" };
	MessagePassing::NameServer::Name name7 = { "namenamenamenab" };
	MessagePassing::NameServer::Name name8 = { "name4" };
	assert(!(name5 == name6));
	assert(name6 == name7);
	assert(name4 == name8);

	name_server2[name1] = 1;
	name_server2[name2] = 2;
	name_server2[name3] = 3;

	assert(name_server2[name1] == 1);
	assert(name_server2[name2] == 2);
	assert(name_server2[name3] == 3);

	assert(name_server2.find(name4) == name_server2.end());
}
