#pragma once
typedef enum {
	NODE_NONE,
	NODE_SENSOR,
	NODE_BRANCH,
	NODE_MERGE,
	NODE_ENTER,
	NODE_EXIT,
} node_type;

#define DIR_AHEAD 0
#define DIR_STRAIGHT 0
#define DIR_CURVED 1
#define RESERVED_BY_NO_ONE -1

#define DIRECT_RESERVE 1
#define REVERSE_RESERVE 2

struct track_node;
typedef struct track_node track_node;
typedef struct track_edge track_edge;

struct track_edge {
	track_edge* reverse;
	track_node *src, *dest;
	int dist;	 /* in millimetres */
	bool broken; /* false if functioning */
};

struct track_node {
	const char* name;
	node_type type;
	int num;							  /* sensor or switch number */
	track_node* reverse;				  /* same location, but opposite direction */
	int reserved_by = RESERVED_BY_NO_ONE; /* who is the node currently reserved by */
	int reserve_dir = RESERVED_BY_NO_ONE;
	track_edge edge[2]; /* one or two edges to other nodes */
	int rev_cost;		/* for merges, specifies how expensive reversing is */
	int rev_offset;		/* for merges, specifies how far off the sensor you need to park */
	int index;			/* Inverted index*/
};
