#include "../inc/ush.h"

void free_sh(t_ush **ush) {
    mx_strdel(&((*ush)->pwd));
    (*ush)->pwd = NULL;
    mx_strdel(&((*ush)->prev_pwd));
    (*ush)->prev_pwd = NULL;
    mx_clear_ldata(&(*ush)->argv);
    mx_clear_list(&(*ush)->argv);

    mx_del_strarr (&(*ush)->argv_arr);

    free(*ush);
    *ush = NULL;
}
