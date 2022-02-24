#ifndef _UTILS_STRING_H
#define _UTILS_STRING_H

int utils_str_compare(char* a, char* b);
void utils_newline2end(char *str);
char utils_int2char(int a);
void utils_int2str_dec(int a,char* str);
void utils_uint2str_dec(unsigned int num,char* str);

void utils_uint2str_hex(unsigned int num,char* str);

#endif

