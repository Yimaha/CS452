
#include <cassert>
#include <climits>

const int READ_INT_FAIL = INT_MIN;

bool is_digit(char c) {
	return c >= '0' && c <= '9';
}

// Attempt to read an up-to-n-digit number from a buffer
// Returns the number read, or -1 on failure
int scan_int(const char str[], const int read_len, int* out_len) {
	int i = 0;
	int num = 0;
	bool is_negative = false;
	if (str[i] == '-') {
		i++;
		is_negative = true;
	}

	while (i < read_len) {
		char c = str[i];
		if (is_digit(c)) {
			num = num * 10 + (c - '0');
		} else {
			break;
		}
		i++;
	}

	if (i == 0) {
		return READ_INT_FAIL;
	}

	*out_len = i;
	return num * (is_negative ? -1 : 1);
}

int main() {
	char cmd[] = "tr 2 10\r\n";
	int out_len = 0;

	int train = scan_int(cmd + 3, 2, &out_len);
	assert(train == 2);
	assert(out_len == 1);

	int speed = scan_int(cmd + 5, 2, &out_len);
	assert(speed == 10);
	assert(out_len == 2);

	char cmd2[] = "go 10 -3\r\n";
	int out_len2 = 0;

	int sdest = scan_int(cmd2 + 3, 2, &out_len2);
	assert(sdest == 10);
	assert(out_len2 == 2);

	int offset = scan_int(cmd2 + 6, 2, &out_len2);
	assert(offset == -3);
	assert(out_len2 == 2);
}