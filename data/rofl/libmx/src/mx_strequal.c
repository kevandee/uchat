#include "../inc/libmx.h"

int mx_strequal(const char *s1, const char *s2) {
	while (*s1 != '\0' 
	&& (mx_tolower(*s1) == mx_tolower(*s2))) {
			s1++;
        	s2++;
    	}

	return (int)(mx_tolower(*s1) - mx_tolower(*s2));
}
