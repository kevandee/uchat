#include "../inc/libmx.h"

char *mx_nbr_to_hex(unsigned long nbr) {
	unsigned long tmp_nbr = nbr;
	int count = 0;

	while(tmp_nbr > 0) {
		tmp_nbr /= 16;
		count++;
	}
	char *result = mx_strnew(count);
	if (nbr == 0) {
		return mx_strcpy(result, "0");
	}
	for(int i = count - 1; nbr > 0; i--) {
		if(nbr % 16 < 10) {
			result[i] = nbr % 16 + 48;
		}
		else {
			result[i] = nbr % 16 + 87;
		}
		nbr /= 16;
	}

	return result;
}

