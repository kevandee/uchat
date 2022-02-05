#include "../inc/ush.h"

void ush_execute(t_ush *ush) {
    if (ush->cmd_num == 8) {
            return;
    }

    switch (ush->cmd_num) {
        case 0:
                ush_export(ush);
                break;
        case 1:
                ush_unset(ush);
                break;
        case 2:
                ush_fg(ush);
                break;
        case 3:
                ush_env(ush);
                break;
        case 4:
                ush_cd(ush);
                break;
        case 5:             //pwd
                ush_pwd(ush);
                break;
        case 6:
                ush -> err = ush_which(ush);
                break;
        case 7: //echo
                ush_echo(ush);
                break;
        case 8: //exit
                return;
        case -1:
                ush -> err = launch_proc(ush->argv_arr, ush);
                break;
    }
    ush -> cmd_num = -2;
}
