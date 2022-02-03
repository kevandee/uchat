#include "../inc/libmx.h"

int mx_binary_search(char **arr, int size, const char *s, int *count) {
	int start;
	int end;
	int index;

	start = 0;
	end = size - 1;

	while (start <= end) {
		index = (start + end) / 2;
		(*count)++;
		
		if (mx_strcmp(arr[index], s) > 0)
			end = index - 1;
		else if (mx_strcmp(arr[index], s) < 0)
			start = index + 1;
		else
			return index;
	}

	*count = 0;
	return -1;
}

