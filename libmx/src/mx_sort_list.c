#include "../inc/libmx.h"

t_list *mx_sort_list(t_list *list, bool (*cmp)(void *a, void *b)) {
	if(list == NULL || cmp == NULL) {
		return NULL;
	}

	for (t_list *temp_i = list; temp_i != NULL; temp_i = temp_i -> next) {
		for (t_list *temp_j = list; temp_j -> next != NULL; temp_j = temp_j -> next) {
			if (cmp(temp_j -> data, temp_j -> next -> data)) {
				void *b = temp_j -> data;
				temp_j -> data = temp_j -> next -> data; 
				temp_j -> next -> data = b; 
			}
		}
	}

	return list;
}

