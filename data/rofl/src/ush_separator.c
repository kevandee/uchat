#include "../inc/ush.h"

void ush_separator(char *s, t_ush *ush) {
    if (!s) {
		return;
	}
    if (check_input(s)) {
        return;
    }
	int word_count = 0;
	bool is_mark = false;
    if (s[0] == '\'' || s[0] == '\"') {
        is_mark = true;
    }
    
    int i = 1;
    for (; i < mx_strlen(s); i++) {
        if ((s[i] == '\'' || s[i] == '\"') 
        && s[i - 1] != '\\') {
            if (!is_mark) {
                if (s[i - 1] == ' ')
                    is_mark = true;
            }
            else {
                is_mark = false;
                //word_count++;
            }
        }

        if (!is_mark && s[i - 1] != '\\'&& s[i] == ';' && s[i + 1] != ';' && s[i + 1] != '\'' && s[i + 1] != '\"' && s[i + 1] != '\0' && i < mx_strlen(s) - 1) {
            word_count++;
        }
    }
    if (s[0] != '\'' && s[0] != '\0' && s[0]!= '\"') {
        word_count++;
    }
    if (!word_count) {
        mx_push_back(&ush->cmd_queue, mx_strdup(s));
        return;
    }
	int start = 0;
    is_mark = false;
    char delim = ';';
    for (int i = 0; i < word_count; i++) {
        delim = ';';
        while (s[start] == delim) {
			start++;
		}
        if (start == mx_strlen(s) - 1) {
            break;
        }
		int start_word = start;
		int word_len = 0;
        
        char quote;
        if (s[start] == '\'' || s[start] == '\"') {
            is_mark = is_mark == true ? false : true;
            quote = s[start] == '\"' ? '\"' : '\'';
            word_len++;
        }

        while (s[start_word + word_len] != '\0') {
            if (!is_mark ) {
                if (s[start_word + word_len] == '\'' 
                || s[start_word + word_len] == '\"') {
                    is_mark = true;
                    quote = s[start_word + word_len] == '\"' ? '\"' : '\'';
                }
                if (s[start_word + word_len] == delim && s[start_word + word_len - 1] != '\\') {
                    break;
                }
            }
            else if (is_mark) {
                if (s[start_word + word_len] == quote) {
                    //word_len++;
                    is_mark = false;
                }
            }
            word_len++;
            if (s[start_word + word_len] == delim && !is_mark && s[start_word + word_len - 1] != '\\') {
                break;
            }
        }
        if(s[start_word] == '$'){
            ush->quote = true;
        }

		char *temp1 = mx_strnew(word_len);
		mx_strncpy(temp1, s + start, word_len);
        mx_push_back(&ush->cmd_queue, temp1);
        
        start += word_len;
	}
}
