#pragma once
#include "../rpi.h"
#include <climits>
#include <cstddef>
#include <cstdint>

int min(int a, int b);
bool is_digit(char c);
char lower(char c);
int get_length(uint64_t val);
void swap(char s[], int l, int r);
void reverse_arr(char s[], int length);
char* itoa_10(uint64_t val, char* str);
void ctoh(char c, char buf[]);
void print(const char s[], int length);
void print_int(uint64_t val);

const int READ_INT_FAIL = INT_MIN;
// Returns READ_INT_FAIL if no integer is found.
int scan_int(const char str[], int* out_len, const int read_len = 0);

// N-length string compare
int strncmp(const char* s1, const char* s2, int n);
