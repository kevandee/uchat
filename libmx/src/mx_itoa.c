#include "../inc/libmx.h"

char *mx_itoa(int number) {
    bool flag = false;

    if (number < 0) {
        flag = true;
		number = -number;
	}

    int tmp_nbr = number;
	int len = 0;
    char *num_str = NULL;

	while(tmp_nbr > 0) {
        tmp_nbr /= 10;
		len++;
	}

    num_str = mx_strnew(len + 1);

    if (number == 0)
		return mx_strcpy(num_str, "0");
	if (number == -2147483648)
		return mx_strcpy(num_str, "-2147483648");
    if (flag) {
		num_str[0] = '-';
	}
    if(flag) {
        for (int i = len; i > 0; i--) {
            num_str[i] = number % 10 + 48;
            number /= 10;
        }
    }
    else {
        for (int i = len-1; i >= 0; i--) {
            num_str[i] = number % 10 + 48;
            number /= 10;
        }
    }
    return num_str;
}
