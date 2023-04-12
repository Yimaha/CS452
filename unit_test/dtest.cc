
#include "../src/routing/dijkstra.h"
#include <cassert>
#include <iostream>
using namespace Routing;
using namespace std;

void clear_reservations(track_node* track) {
	for (int i = 0; i < TRACK_MAX; i++) {
		track[i].reserved_by = RESERVED_BY_NO_ONE;
	}
}

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
	// cout << endl << "Track A Merge-to-Sensor Distances:" << endl;
	// for (int i = 0; i < TRACK_MAX; ++i) {
	// 	if (track[i].type == NODE_MERGE) {
	// 		cout << "Dist from " << i << " to next sensor: " << dist_to_next_sensor(track, i) << endl;
	// 	}
	// }

	track_node trackb[TRACK_MAX] = { 0 };
	init_trackb(trackb);

	Dijkstra dijkstrab = Dijkstra(trackb);
	cout << "Dijkstra Size: " << sizeof(dijkstra) << endl;

	index = 0;
	int bpath_right[4] = { 119, 122, 120, 49 };

	etl::list<int, PATH_LIMIT> qb = etl::list<int, PATH_LIMIT>();
	// assert(!dijkstrab.path(&qb, 119, 65));
	// assert(!dijkstrab.path(&qb, 119, 127));
	assert(dijkstrab.path(&qb, 119, 49));
	while (!qb.empty()) {
		cout << qb.front() << '|' << trackb[qb.front()].name << ' ';
		assert(qb.front() == bpath_right[index++]);
		qb.pop_front();
	}

	cout << endl;
	// assert(!dijkstrab.path(&qb, 75, 39)); // E12 to C8
	// assert(dijkstrab.path(&qb, 75, 58));  // E12 to D11
	while (!qb.empty()) {
		cout << qb.front() << '|' << trackb[qb.front()].name << ' ';
		qb.pop_front();
	}

	cout << endl;

	assert(dijkstrab.is_path_possible(16, 105));
	cout << dijkstrab.get_dist(105) << endl;

	assert(dijkstrab.is_path_possible(51, 105));
	cout << dijkstrab.get_dist(105) << endl;

	qb.clear();
	assert(dijkstrab.path(&qb, 0, 49, true));
	cout << "Path from 0 to 49 (dist = " << dijkstrab.get_dist(48) << ", cost = " << dijkstrab.get_cost(48) << "):" << endl;
	int wpath[] = { 0, 103, 101, 100, 107, 3, 31, 108, 41, 110, 18, 33, 117, 119, 122, 120, 49 };
	index = 0;
	while (!qb.empty()) {
		cout << qb.front() << '|' << trackb[qb.front()].name << ' ';
		assert(qb.front() == wpath[index++]);
		qb.pop_front();
	}

	cout << endl;

	// Does path to next sensor work?
	etl::list<int, SHORT_PATH_LIMIT> ptns = dijkstrab.path_to_next_sensor(103);
	cout << "Path to next sensor from 103:" << endl;
	while (!ptns.empty()) {
		cout << ptns.front() << '|' << trackb[ptns.front()].name << ' ';
		ptns.pop_front();
	}

	cout << endl;

	// Okay, what about the weighted path?
	qb.clear();
	WeightedPath wp;
	assert(dijkstrab.weighted_path(&wp, 0, 68));
	cout << "Weighted Path from 0 to 68 (dist = " << dijkstrab.get_dist(68) << ", cost = " << dijkstrab.get_cost(68)
		 << ", reverse = " << wp.has_reverse << ", offset = " << wp.rev_offset << "):" << endl;

	int wpath2[] = { 0, 103, 101, 44 };
	index = 0;
	while (!wp.wpath.empty()) {
		cout << wp.wpath.front() << '|' << trackb[wp.wpath.front()].name << ' ';
		assert(wp.wpath.front() == wpath2[index++]);
		wp.wpath.pop_front();
	}

	cout << endl;

	// Let's try weighted path when there are reservations
	trackb[50].reserved_by = 24;
	trackb[51].reserved_by = 24;
	trackb[42].reserved_by = 58;
	trackb[43].reserved_by = 58;
	dijkstrab = Dijkstra(trackb, true);
	qb.clear();
	WeightedPath wp2;
	assert(dijkstrab.weighted_path(&wp2, 0, 68));
	cout << "Weighted Path from 0 to 68 [reserved] (dist = " << dijkstrab.get_dist(68) << ", cost = " << dijkstrab.get_cost(68)
		 << ", reverse = " << wp2.has_reverse << ", offset = " << wp2.rev_offset << "):" << endl;

	while (!wp2.wpath.empty()) {
		cout << wp2.wpath.front() << '|' << trackb[wp2.wpath.front()].name << ' ';
		wp2.wpath.pop_front();
	}

	cout << endl;

	// More random weighted paths
	qb.clear();
	clear_reservations(trackb);
	WeightedPath wp3;
	assert(dijkstrab.weighted_path(&wp3, 81, 133));
	cout << "Weighted Path from 82 to 133 (dist = " << dijkstrab.get_dist(133) << ", cost = " << dijkstrab.get_cost(133)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		cout << wp3.wpath.front() << '|' << trackb[wp3.wpath.front()].name << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;
	clear_reservations(trackb);
	assert(dijkstrab.weighted_path(&wp3, 21, 27));
	cout << "Weighted Path from 21 to 27 (dist = " << dijkstrab.get_dist(27) << ", cost = " << dijkstrab.get_cost(27)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		cout << wp3.wpath.front() << '|' << trackb[wp3.wpath.front()].name << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;
	clear_reservations(trackb);
	trackb[2].reserved_by = 78;
	trackb[3].reserved_by = 78;
	trackb[36].reserved_by = 24;
	trackb[37].reserved_by = 24;
	assert(dijkstrab.weighted_path(&wp3, 21, 27));
	cout << "Weighted Path from 21 to 27 (dist = " << dijkstrab.get_dist(27) << ", cost = " << dijkstrab.get_cost(27)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		cout << wp3.wpath.front() << '|' << trackb[wp3.wpath.front()].name << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;
	clear_reservations(trackb);
	assert(dijkstrab.weighted_path(&wp3, 28, 125));
	cout << "Weighted Path from 28 to 125 (dist = " << dijkstrab.get_dist(125) << ", cost = " << dijkstrab.get_cost(125)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		cout << wp3.wpath.front() << '|' << trackb[wp3.wpath.front()].name << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;
	clear_reservations(trackb);
	assert(dijkstrab.weighted_path(&wp3, 0, 31));
	cout << "Weighted Path from 0 to 31 (dist = " << dijkstrab.get_dist(31) << ", cost = " << dijkstrab.get_cost(31)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		cout << wp3.wpath.front() << '|' << trackb[wp3.wpath.front()].name << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;
	clear_reservations(trackb);

	// Here's a fun thing. Let's try a reverse path that might use the banned sensors.
	assert(dijkstrab.weighted_path(&wp3, 17, 18));
	cout << "Weighted Path from 17 to 18 (dist = " << dijkstrab.get_dist(18) << ", cost = " << dijkstrab.get_cost(18)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		cout << wp3.wpath.front() << '|' << trackb[wp3.wpath.front()].name << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;

	// C14 to B8
	assert(dijkstrab.weighted_path(&wp3, 45, 23));
	cout << "Weighted Path from 45 to 23 (dist = " << dijkstrab.get_dist(23) << ", cost = " << dijkstrab.get_cost(23)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		int curr = wp3.wpath.front();
		cout << curr << '|' << trackb[curr].name << '|' << dijkstrab.get_dist(curr) << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;

	// E12 to B8
	assert(dijkstrab.weighted_path(&wp3, 75, 23));
	cout << "Weighted Path from 75 to 23 (dist = " << dijkstrab.get_dist(23) << ", cost = " << dijkstrab.get_cost(23)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		int curr = wp3.wpath.front();
		cout << curr << '|' << trackb[curr].name << '|' << dijkstrab.get_dist(curr) << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;

	track_node tracka[TRACK_MAX] = { 0 };
	init_tracka(tracka);

	Dijkstra dijkstraa = Dijkstra(tracka, true);
	etl::unordered_set<int, TRACK_MAX> banned = { 20, 21, 104, 105 };

	assert(dijkstraa.weighted_path_with_ban(&wp3, banned, 2, 73));
	cout << "Weighted Path from 2 to 73 (dist = " << dijkstraa.get_dist(73) << ", cost = " << dijkstraa.get_cost(73)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		int curr = wp3.wpath.front();
		cout << curr << '|' << tracka[curr].name << '|' << dijkstraa.get_dist(curr) << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;

	int dummy1[144];
	bool dummy4;
	int dummy2;
	int dummy3;
	assert(dijkstraa.weighted_path_with_ban(dummy1, banned, &dummy4, &dummy2, &dummy3, 2, 73));
	cout << "Weighted Path from 2 to 73 (dist = " << dijkstraa.get_dist(73) << ", cost = " << dijkstraa.get_cost(73) << endl;

	for (int i = 0; i < 144 && dummy1[i] != dummy3; i++) {
		int curr = dummy1[i];
		cout << curr << '|' << tracka[curr].name << '|' << dijkstraa.get_dist(curr) << ' ';
	}

	cout << endl;

	banned = { 72, 133, 134, 15, 2, 38, 81, 53, 65, 95, 30, 26, 19, 137 };

	dijkstraa.weighted_path_with_ban(&wp3, banned, 37, 70);
	cout << "Weighted Path from 37 to 70 (dist = " << dijkstraa.get_dist(73) << ", cost = " << dijkstraa.get_cost(70)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		int curr = wp3.wpath.front();
		cout << curr << '|' << tracka[curr].name << '|' << dijkstraa.get_dist(curr) << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;
	dijkstraa.weighted_path_with_ban(&wp3, banned, 36, 70);
	cout << "Weighted Path from 36 to 70 (dist = " << dijkstraa.get_dist(73) << ", cost = " << dijkstraa.get_cost(70)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		int curr = wp3.wpath.front();
		cout << curr << '|' << tracka[curr].name << '|' << dijkstraa.get_dist(curr) << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;
	dijkstraa.weighted_path_with_ban(&wp3, banned, 36, 71);
	cout << "Weighted Path from 36 to 71 (dist = " << dijkstraa.get_dist(73) << ", cost = " << dijkstraa.get_cost(70)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		int curr = wp3.wpath.front();
		cout << curr << '|' << tracka[curr].name << '|' << dijkstraa.get_dist(curr) << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;
	dijkstraa.weighted_path_with_ban(&wp3, banned, 37, 71);
	cout << "Weighted Path from 37 to 71 (dist = " << dijkstraa.get_dist(73) << ", cost = " << dijkstraa.get_cost(70)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		int curr = wp3.wpath.front();
		cout << curr << '|' << tracka[curr].name << '|' << dijkstraa.get_dist(curr) << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;

	dijkstraa.weighted_path_with_ban(&wp3, banned, 3, 8);
	cout << "Weighted Path from 3 to 8 (dist = " << dijkstraa.get_dist(34) << ", cost = " << dijkstraa.get_cost(34)
		 << ", reverse = " << wp3.has_reverse << ", offset = " << wp3.rev_offset << "):" << endl;

	while (!wp3.wpath.empty()) {
		int curr = wp3.wpath.front();
		cout << curr << '|' << tracka[curr].name << '|' << dijkstraa.get_dist(curr) << ' ';
		wp3.wpath.pop_front();
	}

	cout << endl;

	banned = { 28, 53, 65, 67, 68, 76, 88, 102, 109, 112, 135, 139 };
	WeightedPath wpC;

	assert(!dijkstraa.weighted_path_with_ban(&wpC, banned, 35, 69));
	cout << "Weighted Path from 35 to 69 (dist = " << dijkstraa.get_dist(69) << ", cost = " << dijkstraa.get_cost(69)
		 << ", reverse = " << wpC.has_reverse << ", offset = " << wpC.rev_offset << "):" << endl;

	while (!wpC.wpath.empty()) {
		int curr = wpC.wpath.front();
		cout << curr << '|' << tracka[curr].name << '|' << dijkstraa.get_dist(curr) << ' ';
		wpC.wpath.pop_front();
	}

	cout << endl;
}
