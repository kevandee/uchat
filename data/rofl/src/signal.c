#include "../inc/ush.h"

static void handlerC(int signum) {
    (void)signum;
}

/*static void handlerZ(int signum) {
    (void)signum;
}*/

void detecter() {
    signal(SIGINT, handlerC);
    //signal(SIGTSTP, handlerZ);
}

/*static void exit_ush(t_ush *ush) {
    printf("exit\n");
    mx_clear_all(ush);
    exit(0);
}*/

static void reverse_backscape(int *position, char *line)
{
    for (int i = *position; i < mx_strlen(line); i++)
    {
        line[i] = line[i + 1];
    }
}

void signal_detected(int keycode, char **line, int *position, t_ush *ush) {
    if (keycode == MX_CTRL_C) {
        if (ush->foreground) {
            kill(ush->foreground->pid, SIGTERM);
        }
        for (int i = 0; i < mx_strlen(*line); i++) {
            *line[i] = '\0';
        }
    }
    if (keycode == MX_CTRL_D) {
        if (strcmp(*line, "") == 0) {
            printf("\n");
            //exit_ush(ush);
            exit(ush->err);
        }
        else {
            reverse_backscape(position, *line);
        }
    }
    /*if (keycode == MX_CTRL_Z) {
        if (ush->foreground) {
            kill (ush->foreground->pid, SIGKILL);
            waitpid(ush->foreground->pid, NULL, WUNTRACED);
            printf("KILL\n");
        }
    }*/
}
