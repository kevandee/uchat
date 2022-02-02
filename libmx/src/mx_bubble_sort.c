#include "../inc/libmx.h"

int mx_bubble_sort(char **arr, int size) {
	if (size > 1) {
		int swap_count = 0;

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size - 1; j++) {
				if (mx_strcmp(arr[j],arr[j + 1]) > 0) {
					char *temp = arr[j];
					arr[j] = arr[j + 1];
					arr[j + 1] = temp;
					swap_count++;
				}
			}
		}

		return swap_count;
	}

	return 0;
}

