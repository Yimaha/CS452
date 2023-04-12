
#pragma once
#include "../etl/circular_buffer.h"
#include "../etl/list.h"
#include "../etl/priority_queue.h"
#include "../etl/queue.h"
#include "../etl/random.h"
#include "../etl/stack.h"
#include "../etl/unordered_set.h"
#include "track_data_new.h"

namespace Routing
{

const int NO_REVERSE = -1;
const int NO_PREV = -1;
const int PATH_LIMIT = 150;
const int BRANCH_MULTIPLIER = 2;
const int RESERVATION_MULTIPLIER= 4;
const int NO_SWITCH = -1;
const int NO_SENSOR = -1;
const int SHORT_PATH_LIMIT = 128;
const int NUM_BRANCHES = 22;
const int MAX_MULTI_PATH = 8;
const int RESERVED_FLAT_COST = 200;
typedef etl::priority_queue<etl::pair<int, int>, TRACK_MAX> pq_t;

const int MIN_RANDOM_DEST_DIST = 750;
const int MAX_RANDOM_DEST_DIST = 200000;
const int RNG_SEED = 314159;
// 55, 37 landed wayy to often on a dead zone, dismissed
const int BANNED_SENSORS[] = { 38, 40, 55, 37 };
const int NUM_BANNED_SENSORS = sizeof(BANNED_SENSORS) / sizeof(int);

struct WeightedPath {
	etl::list<int, PATH_LIMIT> wpath;
	bool has_reverse = false;
	int rev_offset = 0;
};

class Dijkstra {
public:
	Dijkstra(track_node* track, bool weight_brms = false);
	~Dijkstra();

	int get_prev(const int dest) const;
	int get_dist(const int dest) const;
	int get_cost(const int dest) const;
	etl::list<int, SHORT_PATH_LIMIT> path_to_next_sensor(const int src) const;
	bool path(etl::list<int, PATH_LIMIT>* q, const int source, const int dest, const bool enable_reverse = false);
	bool path_with_ban(etl::list<int, PATH_LIMIT>* q, etl::unordered_set<int, TRACK_MAX>& banned_node, const int source, const int dest, const bool enable_revers, const bool enable_weight);

	bool path(int* q, const int source, const int dest);
	bool path_with_ban(int* q, etl::unordered_set<int, TRACK_MAX>& banned_node, const int source, const int dest);

	// Weighted-path: used to calculate a full shortest path to a destination, which keeps track of reversing.
	// This has a complicated type because I need to have two pieces of extra information:
	// 1. Whether the path requires reversing
	// 2. How much of an offset the end of the path requires (may be necessary to move past merges)

	bool weighted_path(WeightedPath* q, const int source, const int dest);

	bool weighted_path_with_ban(WeightedPath* q, etl::unordered_set<int, TRACK_MAX>& banned_node, const int source, const int dest);

	bool weighted_path_with_ban(int wpath[],
								etl::unordered_set<int, TRACK_MAX>& banned_node,
								bool* has_reverse,
								int* rev_offset,
								int* new_dest,
								const int source,
								const int dest);
	bool is_path_possible(const int source, const int dest);

	// How many sensors are within a certain distance of a source?
	int num_sensors_within_dist_range(const int source);

	// Obtain a random destination from a source
	int random_sensor_dest(const int source);

private:
	track_node* track;
	bool weight_brms;
	int dist[TRACK_MAX]; // distances from source to each node, indexed by source
	int cost[TRACK_MAX]; // cost of each node. What is actually used to calculate path lengths
	int prev[TRACK_MAX]; // previous node in shortest path from source, indexed by source
	pq_t pq = etl::priority_queue<etl::pair<int, int>, TRACK_MAX>();
	etl::stack<int, TRACK_MAX> stack = etl::stack<int, TRACK_MAX>();
	void dijkstra(const int source, const bool enable_reverse = false, const bool use_reservations = false);

	void dijkstra(const int source,
				  etl::unordered_set<int, TRACK_MAX>& banned_node,
				  const bool enable_reverse = false,
				  const bool use_reservations = false);
	void dijkstra_update(const int curr, etl::unordered_set<int, TRACK_MAX>& banned_node, const bool enable_reverse, const bool use_reservations);

	etl::unordered_set<int, TRACK_MAX> banned_sensors = etl::unordered_set<int, TRACK_MAX>();

	etl::random_xorshift rngesus = etl::random_xorshift(RNG_SEED);
	etl::circular_buffer<int, TRACK_MAX> sensors_within_range = etl::circular_buffer<int, TRACK_MAX>();
	void determine_range_sensors(const int source);
};
}
