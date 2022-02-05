#include "../inc/libmx.h"

unsigned long mx_hex_to_nbr(const char *hex) {
	if (hex == NULL) {
		return 0;
	}

	unsigned long base = 1;
	unsigned long result = 0;

	int length = 0;
	
	while (hex[length] != '\0') {
		if (!mx_isdigit(hex[length]) 
		&& !mx_isalpha(hex[length])) {
			return 0;
		}

		if (mx_isalpha(hex[length])) {
			if (mx_isupper(hex[length])
			&& (hex[length] < 65 || hex[length] > 70)) {
				return 0;
			}

			if (mx_islower(hex[length])
            && (hex[length] < 97 || hex[length] > 102)) {
				return 0;
			}
		}

		length++;
	}

	for (int i = length - 1; i >= 0; i--) {
		if (mx_isdigit(hex[i])) {
	            result += (hex[i] - 48) * base;
	            base *= 16;
	        }
	        else if (mx_isupper(hex[i])) {
	            result += (hex[i] - 55) * base;
	            base *= 16;
	        }
	        else if (mx_islower(hex[i])) {
	            result += (hex[i] - 87) * base;
	            base *= 16;
	        }
	}
	
	return result;
}

