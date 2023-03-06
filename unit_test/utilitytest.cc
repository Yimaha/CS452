
#include <cassert>

bool is_digit(char c) {
	return c >= '0' && c <= '9';
}

int scan_int(const char str[], const int read_len, int* out_len) {
	int i = 0;
	int num = 0;
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
		return -1;
	}

	*out_len = i;
	return num;
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
}