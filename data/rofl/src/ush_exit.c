#include "../inc/ush.h"

void ush_exit(t_ush *ush) {
    if (ush -> count_bg) {
        mx_printerr("ush: you have suspended jobs.\n");
        return;
    }
    if (ush -> argv) {
            mx_disable_canon();
            int code = mx_atoi(ush -> argv -> data);
            free_sh (&ush);
            exit(code);
    }
    mx_disable_canon();
    free_sh (&ush);
    exit(0);
}
