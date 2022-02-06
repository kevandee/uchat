#include "../inc/libmx.h"

int mx_atoi(const char *str) {
	int i = 0;
	int space_count = 0;
	bool flag = false;
	
	while(str[space_count] == ' ') {
		space_count++;
	}

	if(str[space_count] == '-' 
	|| str[space_count] == '+') {
		flag = true;
	}
	
	while(str[i] != '\0') {
		if(i > 0) {
			if (!mx_isdigit(str[i])) {
				return 0;
			}
		}
		
		if (i == 0 && !flag) {
			if(!mx_isdigit(str[i])) {
				return 0;
			}
		}

		i++;
	}

	long number = 0;	

	for (int i = 0; str[i] != '\0'; i++) {
		if(mx_isdigit(str[i])) {
			number *= 10;

			if(str[0] == '-'){
				number -= str[i] - 48;
			}
			else {
				number += str[i] - 48;
			}
		}

		if(mx_isspace(str[i])) {
			break;
		}
	}

	return (int)number;
}

