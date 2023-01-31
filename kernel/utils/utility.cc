
#include "utility.h"

int min(int a, int b) {
	return a < b ? a : b;
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

void print(const char s[], int length) {
	uart_puts(0, 0, s, length);
}

void print_int(uint64_t val) {
	char msg[30]; // max of uint64_t goes up to 20 digit, including the /0 you have 21 max
	int length = get_length(val);
	itoa_10(val, msg);
	print(msg, length);
}
