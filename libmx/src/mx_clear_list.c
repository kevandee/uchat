#include "../inc/libmx.h"

void mx_clear_list(t_list **list) {
	if(*list == NULL || list == NULL) {
        return;
    }

    t_list *current = (*list);
    t_list *next;

    while(current != NULL){
        next = current->next;
        if (malloc_size(current))
            free(current);
        current = next;
    }
}

void mx_clear_ldata(t_list **list) {
	if(*list == NULL || list == NULL) {
        return;
    }

    t_list *current = (*list);

    while(current != NULL){
        if (malloc_size(current -> data)) {
            free(current -> data);
            current -> data = NULL;
        }
        current = current -> next;
    }
}
