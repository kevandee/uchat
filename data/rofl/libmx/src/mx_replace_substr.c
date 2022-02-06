#include "../inc/libmx.h"

char *mx_replace_substr(const char *str, const char *sub, const char *replace) {
    if (!str || !sub || !replace) {
        return NULL;
    }
    if (mx_strlen(str) < mx_strlen(sub)) {
        //return mx_strdup(str);
    }
    
    int count_sub = mx_count_substr(str, sub);
    int len_new = mx_strlen(str) - count_sub * mx_strlen(sub) + count_sub * mx_strlen(replace);
    char *temp_str = mx_strnew(len_new);
    int k = 0;
    for (int i = 0; i < len_new;) {
        int index_sub = mx_get_substr_index(str + k, sub) + k;
        if (k == index_sub) {
            for (int j = 0; j < mx_strlen(replace); j++) {
                temp_str[i + j] = replace[j];
            }
            i += mx_strlen(replace);
            k += mx_strlen(sub);
        }
        else {
            temp_str[i] = str[k];
            k++;
            i++;
        }
    }

    return temp_str;
}
