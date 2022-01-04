#include "../inc/libmx.h"

void *mx_memset(void *b, int c, size_t len) {
    unsigned char *mem_str = (unsigned char *)b;

    for (size_t i = 0; i < len; i++) {
        mem_str[i] = (unsigned char)c;
    }
    
    return b;
}
