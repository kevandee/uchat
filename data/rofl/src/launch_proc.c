#include "../inc/ush.h"

int launch_proc(char **argv, t_ush *ush) {
    
    pid_t pid;
    int status = 0;
    extern char **environ;
    pid = fork();
    mx_disable_canon();
    if (pid == 0) {
        sigset_t x;
        sigemptyset (&x);
        sigaddset(&x, SIGINT);
        sigaddset(&x, SIGSTOP);
        sigaddset(&x, SIGTSTP);
        sigprocmask(SIG_UNBLOCK, &x, NULL);

        char *prog = get_prog_path(argv[0], getenv("PATH"));

        bool is_path = false;
        if (mx_strchr(argv[0], '/')) {
            is_path = true;
        }

        if (!prog && is_path) {
            mx_printerr("ush: no such file or directory: ");
            mx_printerr(argv[0]);
            mx_printerr("\n");
            exit(127);
        }

        if (execve(prog, argv, environ) == -1) {
            mx_printerr("ush: command not found: ");
            mx_printerr(argv[0]);
            mx_printerr("\n");
            exit(127);
        }
        exit(0);
    } else if (pid < 0) {
        // Ошибка при форкинге
        perror("ush");
    } else {
        t_process *proc = new_process(pid);
        ush->foreground = proc;
        if (waitpid(pid, &status, WUNTRACED) != -1) {
            if (WIFSTOPPED(status)) {
                proc->status = status;
                ush->foreground = NULL;
                add_bg(proc, ush);
            }
        }
        if (!WIFSTOPPED(status)) {
            tcsetpgrp(STDIN_FILENO, getpgrp());
            mx_enable_canon();
        }
    }
    
    return status >> 8;
}
