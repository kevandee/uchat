#include "../inc/libmx.h"

char **mx_strsplitlines(char const *s, char c) {
	if (!s) {
		return NULL;
	}
	int lines_count = mx_count_lines(s, c);
	char **lines = (char **)malloc(8 * (lines_count));
	int start = 0;

	for (int i = 0; i < lines_count; i++) {
        if(s[start] == c) {
			lines[i] = mx_strnew(1);
		    lines[i][0] = '\0';
            start++;
            continue;
		}

		int start_word = start;
		int word_len = 0;

		while (s[start_word] != c) {
			if(s[start_word] == '\0') {
				break;
			}
			word_len++;
			start_word++;
		}
		
		lines[i] = mx_strnew(word_len);
		mx_strncpy(lines[i], s + start, word_len);
		start += word_len + 1;
	}

	return lines;
}
