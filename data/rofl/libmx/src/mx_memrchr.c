#include "../inc/libmx.h"

void *mx_memrchr(const void *s, int c, size_t n) {
    unsigned char *temp = (unsigned char *)s + mx_strlen(s);
    for (;n > 0; n--) {
        if (*temp == (unsigned char)c) {
            return temp;
        }
        temp--;
    }

    return NULL;
}
