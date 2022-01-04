#include "../inc/libmx.h"

int mx_count_substr(const char *str, const char *sub) {
	if (!str || !sub) {
		return -1;
	}

	int length_sub = mx_strlen(sub);
	int count = 0;

	for (str = mx_strstr(str, sub); str; str = mx_strstr(str + length_sub, sub)) {
		count++;
	}

	return count;
}

