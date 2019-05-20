#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <taint_emul_x86.h>

int write_region(FILE* f, BYTE_t* src, unsigned int len);
int write_t_region(FILE* f, BYTE_t* src, unsigned int len);
int filter_str(char* str);
int filter_str_2(char* str, char unwanted);

#endif
