#include "../inc/libmx.h"

void mx_push_back(t_list **list, void *data) {	
	if(!list) {
		return;
	}
	t_list *new = mx_create_node(data);

	t_list *temp = NULL;
    if (!*list) {
        *list = new;
	}
    else {
        temp = *list;
        while (temp->next != NULL) {
            temp = temp->next;
		}
        temp->next = new;
    }
}

