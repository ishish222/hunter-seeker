#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <emul_x86.h>

void print_location(modrm_ptr a);
int write_region(FILE* f, BYTE_t* src, unsigned int len);
int write_t_region(FILE* f, BYTE_t* src, unsigned int len);
int filter_str(char* str);
int filter_str_2(char* str, char unwanted);
void print_usage();
void _pause();
int strcmpi(char const*, char const*);

#endif
