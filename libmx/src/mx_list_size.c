#include "../inc/libmx.h"

int mx_list_size(t_list *list) {
	if (list == NULL) {
		return 0;
	}
	
	t_list *temp = list;
	int count = 0;

	while (temp != NULL) {
		count++;
		temp = temp -> next;
	}

	return count;
}

