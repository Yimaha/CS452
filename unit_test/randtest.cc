#include "../src/etl/random.h"
#include "../src/routing/dijkstra.h"
#include <cassert>
#include <iostream>
using namespace std;

int main() {
	track_node track[TRACK_MAX] = { 0 };
	init_tracka(track);
	Routing::Dijkstra dijkstra = Routing::Dijkstra(track);

	// Try fetching a ton of random sensors
	int source = 64;
	cout << "From " << source << ": ";
	for (int i = 0; i < 10; ++i) {
		cout << dijkstra.random_sensor_dest(source) << ' ';
	}

	cout << endl;
}