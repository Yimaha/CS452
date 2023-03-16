
#include "dijkstra.h"
#include "../etl/list.h"
#include "../etl/stack.h"
#include <climits>
using namespace Routing;

void Dijkstra::dijkstra_update(const int curr) {
	if (track[curr].type == NODE_EXIT)
		return;

	track_edge edge = track[curr].edge[DIR_AHEAD];
	int v = edge.dest - track;
	int w = edge.dist;
	if (dist[v] > dist[curr] + w && !edge.broken) {
		dist[v] = dist[curr] + w;
		prev[v] = curr;
		pq.emplace(dist[v], v);
	}

	if (track[curr].type == NODE_BRANCH) {
		edge = track[curr].edge[DIR_CURVED];
		v = edge.dest - track;
		w = edge.dist;

		if (dist[v] > dist[curr] + w && !edge.broken) {
			dist[v] = dist[curr] + w;
			prev[v] = curr;
			pq.emplace(dist[v], v);
		}
	}
}

Dijkstra::Dijkstra(track_node* track)
	: track(track) { }

Dijkstra::~Dijkstra() { }

void Routing::Dijkstra::dijkstra(const int source, const int rev_cost) {
	for (int i = 0; i < TRACK_MAX; i++) {
		for (int j = 0; j < TRACK_MAX; j++) {
			dist[j] = INT_MAX;
			prev[j] = NO_PREV;
		}
	}

	dist[source] = 0;
	pq.emplace(0, source);
	while (!pq.empty()) {
		etl::pair<int, int> p = pq.top();
		pq.pop();

		int u = p.second;
		dijkstra_update(u);
		if (rev_cost != NO_REVERSE) {
			int v = track[u].reverse - track;
			int w = rev_cost;
			if (dist[v] > dist[u] + w) {
				dist[v] = dist[u] + w;
				prev[v] = u;
				pq.emplace(dist[v], v);
			}
		}
	}
}

int Dijkstra::get_prev(const int dest) {
	return prev[dest];
}

int Dijkstra::get_dist(const int dest) {
	return dist[dest];
}

bool Dijkstra::is_path_possible(const int source, const int dest) {
	dijkstra(source);
	return prev[dest] != NO_PREV;
}

bool Dijkstra::path(etl::list<int, PATH_LIMIT>* q, const int source, const int dest) {
	dijkstra(source);
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
