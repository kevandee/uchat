#include "../inc/libmx.h"

int mx_count_lines(const char *str, char delimiter) {
	if (!str) {
		return -1;
	}
	
	int count = 0;
	int i = 0;

	for(; str[i] != '\0'; i++) {		
		if(str[i] == delimiter) {
			count++;
		}
	}
	if(str[0] != '\0') {
		count++;
	}

	return count;
}
