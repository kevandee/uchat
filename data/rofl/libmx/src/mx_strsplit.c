#include "../inc/libmx.h"

char **mx_strsplit(char const *s, char c) {
	if (!s) {
		return NULL;
	}
	int word_count = mx_count_words(s, c);
	char **words = (char **)malloc(8 * (word_count + 1));
	int start = 0;

	for (int i = 0; i < word_count; i++) {
		while (s[start] == c) {
			start++;
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
		
		words[i] = mx_strnew(word_len);
		mx_strncpy(words[i], s + start, word_len);
		start += word_len;
	}

	words[word_count] = NULL;

	return words;
}

