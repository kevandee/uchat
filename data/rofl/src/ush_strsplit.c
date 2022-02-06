#include "../inc/ush.h"

char **ush_strsplit(char const *s, t_ush *ush) {
	if (!s) {
		return NULL;
	}
    if (check_input(s)) {
        return NULL;
    }
	int word_count = 0;
	bool is_mark = false;
    if (s[0] == '\'' || s[0] == '\"') {
        is_mark = true;
    }
    int i = 1;
    for (; i < mx_strlen(s); i++) {
        if (s[i] == '(') {
            if (s[i - 1] == ' ') {
                word_count++;
            }
            while (s[i] != ')') {
                i++;
            } 
        }
        if ((s[i] == '\'' || s[i] == '\"') ) {
            if (!is_mark) {
                if (s[i - 1] == ' ')
                    is_mark = true;
            }
            else {
                if (s[i-1] != '\\' && i != mx_strlen(s) - 1) {
                    is_mark = false;
                    word_count++;
                }
                else if (i == mx_strlen(s) - 1) {
                    is_mark = false;
                    word_count++;
                }
            }
        }

        if (!is_mark && s[i - 1] != '\\' && s[i] == ' ' && s[i + 1] != ' ' && s[i + 1] != '\'' && s[i + 1] != '\"' && i < mx_strlen(s) - 1) {
            word_count++;
        }
    }
    if (s[0] != '\'' && s[0] != '\0' && s[0]!= '\"') {
        word_count++;
    }
    //printf("wc %d\n", word_count);
    char **words = (char **)malloc(8 * (word_count + 1));
	int start = 0;
    is_mark = false;
    char delim = ' ';
	for (int i = 0; i < word_count; i++) {
        delim = ' ';
        while (s[start] == delim) {
			start++;
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
            if (s[start + word_len] == '(' && s[start_word + word_len - 1] != '\\') {
                delim = ')';
            }
            if (!is_mark ) {
                if (s[start_word + word_len] == '\'' 
                || s[start_word + word_len] == '\"') {
                    is_mark = true;
                    quote = s[start_word + word_len] == '\"' ? '\"' : '\'';
                }
                if (s[start_word + word_len] == delim && s[start_word + word_len - 1] != '\\') {
                    while (delim == ')' && s[start_word + word_len] == delim) {
                        word_len++;
                        //continue;
                    }
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
                if (delim == ')') {
                    //word_len++;
                    continue;
                }
                
                break;
            }
        }
        if(s[start_word] == '$'){
            ush->quote = true;
        }
        if (i == 1) {
            char *cmd_table[] = {"export", "unset", "fg","env", 
                         "cd", "pwd", "which", "echo", 
                         "exit"};
            bool check = false;
            for (int j = 0; j < 9; j++) {
                if (mx_strcmp(cmd_table[j], words[0]) == 0) {
                    check = true;
                    ush->cmd_num = j;
                }
            }
            if (!check) {
                ush->cmd_num = -1;
                //return 127;
            }
        }
		char *temp1 = mx_strnew(word_len);
		mx_strncpy(temp1, s + start, word_len);
        if (ush->cmd_num == 7 && ush->cmd_num == -1) {
            char *q = del_quot(temp1);
            mx_strdel(&temp1);
            char *temp2 = mx_replace_substr(q, "\\\"", "\"");
            mx_strdel(&q);
            temp1 = mx_replace_substr(temp2, "\\\'", "\'");
            mx_strdel(&temp2);
            temp2 = mx_replace_substr(temp1, "\\ ", " ");
            mx_strdel(&temp1);
            temp1 = mx_replace_substr(temp2, "\\`", "`");
            mx_strdel(&temp2);
            temp2 = mx_replace_substr(temp1, "\\$", "$");
            mx_strdel(&temp1);
            temp1 = mx_replace_substr(temp2, "\\(", "(");
            mx_strdel(&temp2);
            temp2 = mx_replace_substr(temp1, "\\)", ")");
            mx_strdel(&temp1);
            words[i] = mx_replace_substr(temp2, "\\\\", "\\");
            mx_strdel(&temp2);
            //printf("%s\n", words[i]);
            //words[i] = mx_replace_substr(temp1, "\\\\", "\\");
            //mx_strdel(&temp1);
        }
        else {
            words[i] = temp1;
        }
        //printf("%s\n", words[i]);
        if (words[i][0] == '~'&& words[i][1] != '-'&& words[i][1] != '+') {
            char *repl = mx_strdup(getenv("HOME"));
            char *usr = NULL;
            if (words[i][1] != '/' && words[i][1] != '\0' 
            && words[i][1] != '-'&& words[i][1] != '+') {
                usr = mx_strndup((words[i] + 1), mx_strlen(words[i]) - 1);
            }
            int split_len = 1;
            if (usr) {
                while (repl[split_len] != '\0') {
                    if (repl[split_len] == '/') {
                        break;
                    }
                    split_len++;
                }
                char *temp = mx_strndup(repl, split_len + 1);
                mx_strdel(&repl);
                repl = mx_strjoin(temp, usr);
                mx_strdel(&temp);
            }
            char *path = NULL;
            if (!usr) {
                path = mx_replace_substr((words[i]),"~",repl);
                mx_strdel(&repl);
            }
            else {
                path = repl;
            }
            free(words[i]);
            mx_strdel(&usr);
            words[i] = path;
        }
        if (mx_strstr((words[i]), "~+")) {
            char *repl = getenv("PWD");
            char *path = mx_replace_substr((words[i]),"~+",repl);
            free(words[i]);
            words[i] = path;
        }
        if (mx_strstr((words[i]), "~-")) {
            char *repl = getenv("OLDPWD");
            char *path = mx_replace_substr((words[i]),"~-",repl);
            free(words[i]);
            words[i] = path;
        }

        start += word_len;
	}

	words[word_count] = NULL;

	return words;
}
