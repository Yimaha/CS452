/* THIS FILE IS GENERATED CODE -- DO NOT EDIT */

#pragma once
#include "track_node.h"

// The track initialization functions expect an array of this size.
#define TRACK_MAX 144
#define TRACK_NUM_BRANCHES 22
#define TRACK_NUM_SENSORS 80

const int TRACK_DATA_NO_SWITCH = -1;
const int TRACK_DATA_NO_SENSOR = -1;

// To reverse at a merge, you have to exceed the merge by some minimum amount
// or else you run the risk of having the train get swiped.
const int TRACK_MIN_REV_COST = 210;

// Also, reversing should incur some flat cost from having to slow down,
// stop, reverse, then re-accelerate.
const int TRACK_REV_FLAT_COST = 0;

// Gets the distance to the next sensor in the track, or -1 if there is no next sensor.
int dist_to_next_sensor(const track_node* track, int node);

// Gets the ID of the next branch in the track, or -1 if there is no next branch.
int next_branch_id(const track_node* track, int node);

void init_tracka(track_node* track);
void init_trackb(track_node* track);
