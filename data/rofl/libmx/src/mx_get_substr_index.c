#include "../inc/libmx.h"

int mx_get_substr_index(const char *str, const char *sub) {
    if (!str || !sub) {
        return -2;
    }

    char *temp_sub = mx_strstr(str, sub);

    if (temp_sub) {
        for (int i = 0; i < mx_strlen(str); i++) {
            if (temp_sub == str + i) {
                return i;
            }
        }
    }

    return -1;

}
