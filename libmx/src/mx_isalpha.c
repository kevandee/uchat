#include "../inc/libmx.h"

bool mx_isalpha(int c) {
	if (c >= 65 && c <= 90) {
		return true;
	}

	if(c >= 97 && c <= 122) {
		return true;
	}

	return false;
}

