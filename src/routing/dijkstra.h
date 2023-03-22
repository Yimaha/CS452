
#pragma once
#include "../etl/list.h"
#include "../etl/priority_queue.h"
#include "../etl/queue.h"
#include "../etl/stack.h"

#include "track_data_new.h"

namespace Routing
{

const int NO_REVERSE = -1;
const int NO_PREV = -1;
const int PATH_LIMIT = 512;
typedef etl::priority_queue<etl::pair<int, int>, TRACK_MAX> pq_t;

class Dijkstra {
public:
	Dijkstra(track_node* track);
	~Dijkstra();

	int get_prev(int dest);
	int get_dist(int dest);
	bool path(etl::list<int, PATH_LIMIT>* q, const int source, const int dest);
	bool is_path_possible(const int source, const int dest);

private:
	track_node* track;
	int dist[TRACK_MAX]; // distances from source to each node, indexed by source
	int prev[TRACK_MAX]; // previous node in shortest path from source, indexed by source
	pq_t pq = etl::priority_queue<etl::pair<int, int>, TRACK_MAX>();
	etl::stack<int, TRACK_MAX> stack = etl::stack<int, TRACK_MAX>();
	void dijkstra(int source, int rev_cost = NO_REVERSE);
	void dijkstra_update(int curr);
};
}
