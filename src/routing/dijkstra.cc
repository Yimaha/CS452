
#include "dijkstra.h"
#include "../etl/list.h"
#include "../etl/stack.h"
#include <climits>
using namespace Routing;

etl::list<int, SHORT_PATH_LIMIT> Dijkstra::path_to_next_sensor(const int src) const {
	int node = src;
	etl::list<int, SHORT_PATH_LIMIT> path = etl::list<int, SHORT_PATH_LIMIT>();
	while (track[node].type != NODE_SENSOR || banned_sensors.find(node) != banned_sensors.end()) {
		if (track[node].type == NODE_EXIT) {
			return path;
		}

		int dir = DIR_STRAIGHT; // aka DIR_AHEAD, same difference
		if (track[node].type == NODE_BRANCH && track[node].edge[DIR_AHEAD].broken) {
			dir = DIR_CURVED;
		} else if (track[node].edge[dir].dest->type == NODE_EXIT) {
			dir = DIR_CURVED;
		}

		path.push_back(node);
		node = track[node].edge[dir].dest - track;
	}

	path.push_back(node);
	return path;
}

void Dijkstra::dijkstra(const int source, const bool enable_reverse, const bool use_reservations) {
	etl::unordered_set<int, TRACK_MAX> empty;
	dijkstra(source, empty, enable_reverse, use_reservations);
}

void Dijkstra::dijkstra_update(const int curr,
							   etl::unordered_set<int, TRACK_MAX>& banned_node,
							   const bool enable_reverse,
							   const bool use_reservations) {
	if (track[curr].type == NODE_EXIT)
		return;

	track_edge edge = track[curr].edge[DIR_AHEAD];
	int v = edge.dest - track;
	int w = edge.dist;
	int cw = w;
	if (weight_brms && (track[v].type == NODE_BRANCH || track[v].type == NODE_MERGE)) {
		cw *= BRANCH_MULTIPLIER;
	}

	if (use_reservations && track[v].reserved_by != RESERVED_BY_NO_ONE) {
		// Double cost for reserved nodes
		cw = RESERVATION_MULTIPLIER * cw + RESERVED_FLAT_COST;
	}

	if (cost[v] > cost[curr] + cw && !edge.broken && banned_node.count(v) == 0) {
		cost[v] = cost[curr] + cw;
		dist[v] = dist[curr] + w;
		prev[v] = curr;
		pq.emplace(cost[v], v);
	}

	if (track[curr].type == NODE_BRANCH) {
		edge = track[curr].edge[DIR_CURVED];
		v = edge.dest - track;
		w = edge.dist;
		cw = w;
		if (weight_brms && (track[v].type == NODE_BRANCH || track[v].type == NODE_MERGE)) {
			cw *= 15;
			cw /= 10;
		}

		if (use_reservations && track[v].reserved_by != RESERVED_BY_NO_ONE) {
			// Double cost for reserved nodes
			cw = 2 * cw + RESERVED_FLAT_COST;
		}

		if (cost[v] > cost[curr] + cw && !edge.broken && banned_node.count(v) == 0) {
			cost[v] = cost[curr] + cw;
			dist[v] = dist[curr] + w;
			prev[v] = curr;
			pq.emplace(cost[v], v);
		}
	}

	if (track[curr].type == NODE_MERGE && enable_reverse) {
		v = track[curr].reverse - track;
		w = track[curr].rev_cost;

		// For reversing, cost and distance are different, since you have to drive to the next sensor
		// (with potentially some amount of offset) and then drive back to the branch.
		// By factoring in both the sensor distance and the offset, we obtain the real distance.
		// Doubling the resulting amount produces the correct path distance.
		int rdist = dist_to_next_sensor(track, curr) + track[curr].rev_offset;
		if (cost[v] > cost[curr] + w && banned_node.count(v) == 0) {
			cost[v] = cost[curr] + w;
			dist[v] = dist[curr] + 2 * rdist;
			prev[v] = curr;
			pq.emplace(cost[v], v);
		}
	}
}

Dijkstra::Dijkstra(track_node* track, bool weight_brms)
	: track(track)
	, weight_brms(weight_brms) {

	for (int i = 0; i < NUM_BANNED_SENSORS; i++) {
		banned_sensors.insert(BANNED_SENSORS[i]);
	}
}

Dijkstra::~Dijkstra() { }

void Routing::Dijkstra::dijkstra(const int source,
								 etl::unordered_set<int, TRACK_MAX>& banned_node,
								 const bool enable_reverse,
								 const bool use_reservations) {
	for (int i = 0; i < TRACK_MAX; i++) {
		for (int j = 0; j < TRACK_MAX; j++) {
			cost[j] = INT_MAX;
			dist[j] = INT_MAX;
			prev[j] = NO_PREV;
		}
	}

	cost[source] = 0;
	dist[source] = 0;
	pq.emplace(0, source);
	while (!pq.empty()) {
		etl::pair<int, int> p = pq.top();
		pq.pop();

		int u = p.second;
		if (banned_node.count(u) == 0) {
			dijkstra_update(u, banned_node, enable_reverse, use_reservations);
		}
	}
}

int Dijkstra::get_prev(const int dest) const {
	return prev[dest];
}

int Dijkstra::get_dist(const int dest) const {
	return dist[dest];
}

int Dijkstra::get_cost(const int dest) const {
	return cost[dest];
}

bool Dijkstra::is_path_possible(const int source, const int dest) {
	dijkstra(source);
	return prev[dest] != NO_PREV;
}

bool Dijkstra::path(etl::list<int, PATH_LIMIT>* q, const int source, const int dest, const bool enable_reverse) {
	dijkstra(source, enable_reverse);
	int curr = dest;
	if (prev[curr] == NO_PREV) {
		return false; // nothing you can do, dead end
	}

	while (curr != NO_PREV) {
		stack.push(curr);
		curr = prev[curr];
	}

	// Pop nodes off stack and push onto queue
	while (!stack.empty()) {
		q->push_back(stack.top());
		stack.pop();
	}

	return true;
}

bool Dijkstra::path_with_ban(etl::list<int, PATH_LIMIT>* q,
							 etl::unordered_set<int, TRACK_MAX>& banned_node,
							 const int source,
							 const int dest,
							 const bool enable_reverse,
							 const bool using_weight) {
	dijkstra(source, banned_node, enable_reverse, using_weight);
	int curr = dest;
	if (prev[curr] == NO_PREV) {
		return false; // nothing you can do, dead end
	}

	while (curr != NO_PREV) {
		stack.push(curr);
		curr = prev[curr];
	}

	// Pop nodes off stack and push onto queue
	while (!stack.empty()) {
		q->push_back(stack.top());
		stack.pop();
	}

	return true;
}

bool Dijkstra::path(int* q, const int source, const int dest) {
	/* q is an int array with size at least PATH_LIMIT */
	dijkstra(source);
	int curr = dest;
	while (curr != NO_PREV) {
		stack.push(curr);
		curr = prev[curr];
	}

	// Pop nodes off stack and push onto queue
	for (int i = 0; i < PATH_LIMIT && !stack.empty(); i++) {
		q[i] = stack.top();
		stack.pop();
	}

	return true;
}

bool Dijkstra::weighted_path(WeightedPath* q, const int source, const int dest) {
	etl::unordered_set<int, TRACK_MAX> banned_node = {};
	return weighted_path_with_ban(q, banned_node, source, dest);
}

bool Dijkstra::weighted_path_with_ban(WeightedPath* q, etl::unordered_set<int, TRACK_MAX>& banned_node, const int source, const int dest) {
	q->has_reverse = false;
	q->rev_offset = 0;
	dijkstra(source, banned_node, true, true);
	int curr = dest;
	if (prev[curr] == NO_PREV) {
		return false; // nothing you can do, dead end
	}

	// First, check if there are any reverses in the path
	int end = dest;
	while (curr != NO_PREV) {
		if (prev[curr] == track[curr].reverse - track) {
			// Reverse detected
			q->has_reverse = true;
			end = track[curr].reverse - track;
		}

		curr = prev[curr];
	}

	// So now we build the path
	if (!q->has_reverse) {
		// No reverse detected
		return path_with_ban(&q->wpath, banned_node, source, dest, false, true);
	} else {
		// Reverse detected. Where do we actually need to go?
		etl::list<int, SHORT_PATH_LIMIT> path_to_sensor = path_to_next_sensor(end);
		int actual_end = path_to_sensor.back();
		if (get_dist(actual_end) == INT_MAX) {
			// No path to sensor
			return false;
		}

		// Also modify the offset
		q->rev_offset = track[end].rev_offset;

		// Now we just go back to the beginning as usual
		curr = actual_end;
		while (curr != NO_PREV) {
			q->wpath.push_front(curr);
			curr = prev[curr];
		}

		// And we're done!
		return true;
	}
}

bool Dijkstra::weighted_path_with_ban(int wpath[],
									  etl::unordered_set<int, TRACK_MAX>& banned_node,
									  bool* has_reverse,
									  int* rev_offset,
									  int* new_dest,
									  const int source,
									  const int dest) {
	WeightedPath wp;
	bool res = weighted_path_with_ban(&wp, banned_node, source, dest);

	if (!res) {
		return false;
	} else {
		int psize = wp.wpath.size();
		*new_dest = wp.wpath.back();
		for (int i = 0; i < psize; ++i) {
			wpath[i] = wp.wpath.front();
			wp.wpath.pop_front();
		}

		*has_reverse = wp.has_reverse;
		*rev_offset = wp.rev_offset;
		return true;
	}
}

void Dijkstra::determine_range_sensors(const int source) {
	dijkstra(source, true);
	sensors_within_range.clear();
	for (int i = 0; i < TRACK_NUM_SENSORS; i++) {
		if (dist[i] >= MIN_RANDOM_DEST_DIST && dist[i] <= MAX_RANDOM_DEST_DIST) {
			sensors_within_range.push(i);
		}
	}

	int rev = track[source].reverse - track;
	dijkstra(rev, true);
	for (int i = 0; i < TRACK_NUM_SENSORS; i++) {
		if (dist[i] >= MIN_RANDOM_DEST_DIST && dist[i] <= MAX_RANDOM_DEST_DIST) {
			sensors_within_range.push(i);
		}
	}
}

int Dijkstra::num_sensors_within_dist_range(const int source) {
	determine_range_sensors(source);
	return sensors_within_range.size();
}

int Dijkstra::random_sensor_dest(const int source) {
	determine_range_sensors(source);
	if (sensors_within_range.size() == 0) {
		return NO_SENSOR;
	}

	int rand_idx = rngesus.range(0, sensors_within_range.size() - 1);
	return sensors_within_range[rand_idx];
}
