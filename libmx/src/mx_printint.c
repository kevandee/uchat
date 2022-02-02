#include "../inc/libmx.h"

void mx_printint(int n) {
	if (n == -2147483648) {
		write(1, "-2147483648", 11);
		return;
	}	
	
	if(n < 9 && n >= 0) {
		mx_printchar(n + 48);
		return;
	}

	if(n < 0) {
		mx_printchar('-');
                n *= -1;
	}

	int nc = n;
	int length = 0;

	while (nc != 0) {
		nc /= 10;
		length++;
	}

	char str[length];
	int d = 0;

	for (int i = 0; i < length; i++) {
		d = n % 10;
		str[i] = d + 48;
		n /= 10;
	}

	for(int i = length - 1; i >= 0; i--) {
		mx_printchar(str[i]);
	}
}

