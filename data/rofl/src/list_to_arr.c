#include "../inc/ush.h"

void list_to_arr(t_list *list, char ***arr) {
    *arr = (char **)malloc((mx_list_size(list)) * sizeof(char *));
    int i = 0;
    while (list) {
        (*arr)[i] = mx_strdup(list->data);
        i++;
        list = list -> next;
    }
}
