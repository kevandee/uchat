#include "../inc/ush.h"

static void split_first_eq(char *str, int *exit_err) {
    int eq_index = -1;
	char **words = (char **)malloc(8 * 2);

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '=') {
            eq_index = i;
            break;
        }
    }
    
    if (eq_index == 0) {
        char *name = mx_strndup(str + 1, mx_strlen(str) - 1);
        mx_printerr("ush: ");
        mx_printerr(name);
        mx_printerr(" not found\n");
        *exit_err = 1;
        return;
    }

    words[0] = mx_strndup(str, eq_index);
    words[1] = mx_strndup(str + eq_index + 1, mx_strlen(str) - eq_index);

    for (int i = 0; words[0][i] != '\0'; i++) {
        if (mx_isspace(words[0][i]) 
        || (!mx_isalpha(words[0][i]) && !mx_isdigit(words[0][i]))) {
            mx_printerr("export: not valid in this context: ");
            mx_printerr(words[0]);
            mx_printerr("\n");
            *exit_err = 1;
            return;
        }
    }
    setenv(words[0], words[1], 1);
    return;
}

static void exporter(char *str, int *exit_err) {
    int eq_index = -1;
    char *del_quot_str = del_quot(str);
    for (int i = 0; del_quot_str[i] != '\0'; i++) {
        if (del_quot_str[i] == '=' && eq_index == -1) {
            eq_index = i;
        }
    }

    if (eq_index == -1) {
        for (int i = 0; del_quot_str[i] != '\0'; i++) {
            if (mx_isspace(del_quot_str[i]) 
            || ((!mx_isalpha(del_quot_str[i])) && (!mx_isdigit(del_quot_str[i])) && (del_quot_str[i] != '_'))) {
                mx_printerr("export: not an identifier: ");
                mx_printerr(del_quot_str);
                mx_printerr("\n");
                *exit_err = 1;
                return;
            }
        }
        setenv(del_quot_str, "\'\'", 1);
        return;
    }
    else if (eq_index > -1) {
        split_first_eq(del_quot_str, exit_err);
        return;
    }

}

void ush_export(t_ush *ush) {
    if (!ush->argv) {
        extern char **environ;
        for (int i = 0; environ[i]; i++) {
            printf("%s\n", environ[i]);
        }
        return;
    }
    t_list *temp = ush->argv;
    int exit_err = 0;
    for (int i = 0; i < mx_list_size(ush->argv) && exit_err == 0; i++) {
        exporter(temp->data, &exit_err);
        temp = temp->next;
    }
    if (exit_err == 1) {
        ush->err = 1;
    }
    else {
        ush->err = 0;
    }
    return;
}
