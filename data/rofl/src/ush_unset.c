#include "../inc/ush.h"

static void unseter(char *str, int *exit_err) {
    char *del_quot_str = del_quot(str);
    if (del_quot_str[0] == '=') {
        char *name = mx_strndup(str + 1, mx_strlen(str) - 1);
        mx_printerr("ush: ");
        mx_printerr(name);
        mx_printerr(" not found\n");
        *exit_err = 1;
        return;
    }
    
    for (int i = 0; del_quot_str[i] != '\0'; i++) {
        if (mx_isspace(del_quot_str[i]) 
        || ((!mx_isalpha(del_quot_str[i])) && (!mx_isdigit(del_quot_str[i])) && (del_quot_str[i] != '_'))) {
            mx_printerr("unset: ");
            mx_printerr(del_quot_str);
            mx_printerr(" invalid parameter name\n");
            *exit_err = 1;
            return;
        }
    }
    unsetenv(del_quot_str);

}

void ush_unset(t_ush *ush) {
    if (!ush->argv) {
        mx_printerr("unset: not enough arguments\n");
        ush->err = 1;
        return;
    }
    t_list *temp = ush->argv;
    int exit_err = 0;
    for (int i = 0; i < mx_list_size(ush->argv) && exit_err == 0; i++) {
        unseter(temp->data, &exit_err);
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
