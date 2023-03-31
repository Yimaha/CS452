
#pragma once
#include <stdint.h>
#include "../etl/list.h"

namespace Kinematic
{
int64_t calculate_expected_ticks(int64_t V_i, int64_t d, int64_t a);
etl::pair<int64_t, int64_t> calculate_bunny_hop(int64_t A_1, int64_t A_2, int64_t D_t);
int64_t calculate_traversal_min_dist(int64_t A_1, int64_t A_2, int64_t V_f);

}
