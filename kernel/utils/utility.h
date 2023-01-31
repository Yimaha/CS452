#pragma once
#include "../rpi.h"
#include <stddef.h>
#include <stdint.h>

int min(int a, int b);
int get_length(uint64_t val);
void swap(char s[], int l, int r);
void reverse_arr(char s[], int length);
char* itoa_10(uint64_t val, char* str);
void print(const char s[], int length);
void print_int(uint64_t val);