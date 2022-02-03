#include "../inc/libmx.h"

char *mx_strndup(const char *s1, size_t n) {
    if (n < (size_t)mx_strlen(s1)) {
        return mx_strncpy(mx_strnew(n), s1, n);
    }
    else {
        return mx_strdup(s1);
    }
}
