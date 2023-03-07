#pragma once
#include "../rpi.h"
#include <stddef.h>
#include <stdint.h>

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

int scan_int(const char str[], const int read_len, int* out_len);
