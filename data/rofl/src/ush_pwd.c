#include "../inc/ush.h"

void ush_pwd(t_ush *ush) {
    t_list *temp = ush->argv;
    char **argv = NULL;
    list_to_arr(temp, &argv);

    bool P_opt = false;
    bool L_opt = false;
    for (int i = mx_list_size(temp) - 1; i >= 0; i--) {
        for (int j = mx_strlen(argv[i]) - 1; j > 0; j--) {
            if (argv[i][0] == '-' && argv[i][j] != 'L' && argv[i][j] != 'P') {
                mx_printerr("u$h: pwd: -");
                mx_printerr(&argv[i][j]);
                mx_printerr(": invalid option\n");
                mx_printerr("pwd: usage: pwd [-LP]\n");
                ush->err = 1;
                return;
            }
            if (argv[i][j] == 'L' && !P_opt) {
                L_opt = true;
            }
            if (argv[i][j] == 'P' && !L_opt) {
                P_opt = true;
            }
        }
        mx_strdel(&argv[i]);
        argv[i] = NULL;
    }
    free(argv);
    argv = NULL;
    if (!P_opt) {
        mx_printstr(ush->pwd);
        mx_printstr("\n");
    }
    else {
        char *dir = getcwd(NULL, 1024);
        if (dir) {
            mx_printstr(dir);
            mx_printstr("\n");
        }
        mx_strdel(&dir);
    }
    ush->err = 0;
}
