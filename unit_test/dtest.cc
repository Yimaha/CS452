
#include "../src/routing/dijkstra.h"
#include <cassert>
#include <iostream>
using namespace Routing;
using namespace std;

int main() {
	track_node track[TRACK_MAX] = { 0 };
	init_tracka(track);

	Dijkstra dijkstra = Dijkstra(track);
	cout << "Dijkstra Size: " << sizeof(dijkstra) << endl;

	int index = 0;
	int curr = 95;
	int ppath[9] = { 108, 41, 110, 16, 61, 113, 77, 72, 95 };

	etl::list<int, PATH_LIMIT> q = etl::list<int, PATH_LIMIT>();
	dijkstra.path(&q, 108, 95);
	while (!q.empty()) {
		cout << q.front() << '|' << track[q.front()].name << ' ';
		assert(q.front() == ppath[index++]);
		q.pop_front();
	}

	assert(dijkstra.get_dist(95) == 1986);
	cout << endl;

	track_node trackb[TRACK_MAX] = { 0 };
	init_trackb(trackb);

	Dijkstra dijkstrab = Dijkstra(trackb);
	cout << "Dijkstra Size: " << sizeof(dijkstra) << endl;

	index = 0;
	int bpath_right[4] = { 119, 122, 120, 49 };

	etl::list<int, PATH_LIMIT> qb = etl::list<int, PATH_LIMIT>();
	assert(!dijkstrab.path(&qb, 119, 65));
	assert(!dijkstrab.path(&qb, 119, 127));
	assert(dijkstrab.path(&qb, 119, 49));
	while (!qb.empty()) {
		cout << qb.front() << '|' << trackb[qb.front()].name << ' ';
		assert(qb.front() == bpath_right[index++]);
		qb.pop_front();
	}

	cout << endl;
	assert(!dijkstrab.path(&qb, 75, 39)); // E12 to C8
	assert(dijkstrab.path(&qb, 75, 58));  // E12 to D11
	while (!qb.empty()) {
		cout << qb.front() << '|' << trackb[qb.front()].name << ' ';
		qb.pop_front();
	}

	cout << endl;

	assert(dijkstrab.is_path_possible(16, 105));
	cout << dijkstrab.get_dist(105) << endl;

	assert(dijkstrab.is_path_possible(51, 105));
	cout << dijkstrab.get_dist(105) << endl;
}