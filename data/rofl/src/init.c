#include "../inc/ush.h"

void init(t_ush **ush) {

    *ush = (t_ush *)malloc(sizeof(t_ush));
    (*ush) -> pwd = mx_strdup(getenv("PWD"));
    (*ush) -> prev_pwd = mx_strdup(getenv("PWD"));
    char *shlvl = mx_itoa(mx_atoi(getenv("SHLVL")) + 1);
    setenv("SHLVL", shlvl, 1);
    mx_strdel(&shlvl);
    (*ush)->cmd_num = -2;
    (*ush)->argv = NULL;
    (*ush)->err = 0;
    (*ush)->quote = false;
    (*ush)->pid = getpid();
    (*ush)->pgid = getpgid((*ush)->pid);
    (*ush)->count_bg = 0;

    sigfillset(&(*ush)->mask);

    sigprocmask(SIG_SETMASK, &(*ush)->mask, NULL);
    tcgetattr(STDIN_FILENO, mx_get_tty());
    setvbuf(stdout, NULL, _IONBF, 0);
    mx_enable_canon();
}
