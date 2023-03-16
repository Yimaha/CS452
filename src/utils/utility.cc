
#include "utility.h"

int min(int a, int b) {
	return a < b ? a : b;
}

bool is_digit(char c) {
	return c >= '0' && c <= '9';
}

char lower(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c + 32;
	}
	return c;
}

int get_length(uint64_t val) {
	int counter = 1;
	uint64_t base = 10;
	if (val == 0) {
		return 2;
	}

	while (val > 0) {
		counter++;
		val = val / base;
	}
	return counter;
}

void swap(char s[], int l, int r) {
	char temp = *(s + l);
	*(s + l) = *(s + r);
	*(s + r) = temp;
}

void reverse_arr(char s[], int length) {
	int l = 0;
	int r = length - 1;
	while (l < r) {
		swap(s, l, r);
		l++;
		r--;
	}
}

/*
 *  Convert a long into readable format string, base 10
 */
char* itoa_10(uint64_t val, char* str) {
	int i = 0;
	uint64_t base = 10;

	if (val == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	while (val != 0) {
		uint64_t remain = val % base;
		str[i++] = remain + '0';
		val = val / base;
	}

	str[i] = '\0';
	reverse_arr(str, i);
	return str;
}

const char INT_TO_HEX[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void ctoh(char c, char* buf) {
	short hi = (c >> 4) & 0xF;
	short lo = c & 0xF;
	buf[0] = INT_TO_HEX[hi];
	buf[1] = INT_TO_HEX[lo];
}

void print(const char s[], int length) {
	uart_puts(0, 0, s, length);
}

void print_int(uint64_t val) {
	char msg[30]; // max of uint64_t goes up to 20 digit, including the /0 you have 21 max
	int length = get_length(val);
	itoa_10(val, msg);
	print(msg, length);
}

// Attempt to read an up-to-n-digit number from a buffer
// Returns the number read, or -1 on failure
int scan_int(const char str[], int* out_len, const int read_len) {
	int i = 0;
	int num = 0;
	bool is_negative = false;
	if (str[i] == '-') {
		i++;
		is_negative = true;
	}

	while (read_len == 0 || i < read_len) {
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

// strncmp for const char*
int strncmp(const char* s1, const char* s2, int n) {
	for (int i = 0; i < n; i++) {
		if (s1[i] != s2[i]) {
			return s1[i] - s2[i];
		} else if (s1[i] == '\0') {
			return 0;
		}
	}

	return 0;
}
