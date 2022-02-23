#include"string_utils.h"
#include <stddef.h>

int str_compare(char* a, char* b) {
    char aa, bb;

    do {
        aa = (char)*a++;
        bb = (char)*b++;
        if (aa == '\0' || bb == '\0') {
            return aa - bb;
        }
    } while (aa == bb);
    return aa - bb;
}
void str_newline2end(char* str) {
    while (*str != '\0') {
        if (*str == '\n') {
            *str = '\0';
            return;
        }
        ++str;
    }
}