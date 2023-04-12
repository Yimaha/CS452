#include "kinematic.h"
using namespace Kinematic;

int64_t floorSqrt(int64_t x) {
	// Base Cases
	if (x == 0 || x == 1)
		return x;

	// Do Binary Search for floor(sqrt(x))
	int64_t start = 1, end = 3037000500, ans = 0;
	while (start <= end) {
		int64_t mid = (start + end) / 2;
		;

		// If x is a perfect square
		if (mid * mid == x)
			return mid;

		// Since we need floor, we update answer when
		// mid*mid is smaller than x, and move closer to
		// sqrt(x)u
		if (mid * mid < x) {
			start = mid + 1;
			ans = mid;
		} else // If mid*mid is greater than x
			end = mid - 1;
	}
	return ans;
}

int64_t quadratic_solution(int64_t a, int64_t b, int64_t c) {

	int64_t discriminant = (b * b - 4 * a * c);
	discriminant = floorSqrt(discriminant);
	return (-b + discriminant) * 100 / (2 * a);
}
/*
A1 is assumed positive
A2 is assumed negative
*/
etl::pair<int64_t, int64_t> Kinematic::calculate_bunny_hop(int64_t A_1, int64_t A_2, int64_t D_t) {
	int64_t t_2 = floorSqrt((2 * D_t) * 1000000 / (-A_2) / (100 + -A_2*100/A_1));
	int64_t t_1 = -A_2 * t_2 / A_1;
	return etl::make_pair(t_1, t_2);
}

int64_t Kinematic::calculate_traversal_min_dist(int64_t A_1, int64_t A_2, int64_t V_f) {
	int64_t t_1 = V_f * 100 / A_1;
	int64_t t_2 = V_f * 100 / -A_2;
	return (t_1 + t_2) * V_f / 200;
}

int64_t Kinematic::calculate_expected_ticks(int64_t V_i, int64_t d, int64_t a) {
	return quadratic_solution(a, 2 * V_i, -2 * d);
}
