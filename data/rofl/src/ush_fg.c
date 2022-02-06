#include "../inc/ush.h"

void fger(char *str, t_ush *ush) {
    if (str[0] != '%') {
        mx_printerr("fg: job not found: ");
        mx_printerr(str);
        mx_printerr("\n");
        ush->err = 1;
        return;
    }
    int id = atoi(str + 1);

    t_list *temp = ush->background;
    for (int i = 0; i < mx_list_size(ush->background); i++) {
        if (id == ((t_process *)(temp->data))->id) {
            pid_t child_pid = ((t_process *)(temp->data))->pid;
            ush->foreground = ((t_process *)(ush->background->data));
            
            mx_disable_canon();
            tcsetpgrp(STDIN_FILENO, child_pid);
            kill(child_pid, SIGCONT);
            int status = 0;
            waitpid(child_pid, &status, WUNTRACED);
            if (!WIFSTOPPED(status)) {
                mx_pop_back(&ush->background);
                ush->count_bg--;
            }

            tcsetpgrp(STDIN_FILENO, getpgrp());
            mx_enable_canon();
            ush->err = 0;
        }
        temp = temp->next;
    }
    mx_printerr("fg: ");
    mx_printerr(str);
    mx_printerr(": no such job\n");
    ush->err = 1;
}

void ush_fg(t_ush *ush) {
    //t_process *current = ush->foreground;
    
    if (!ush->argv) {
        if (!ush->count_bg) {
            mx_printerr("fg: no current job\n");
            ush->err = 1;
            return;
        }
        t_list *last_proc = ush->background;
        while (last_proc->next) {
            last_proc = last_proc->next;
        }
        pid_t child_pid = ((t_process *)(last_proc->data))->pid;
        ush->foreground = ((t_process *)(ush->background->data));
        
        mx_disable_canon();
        tcsetpgrp(STDIN_FILENO, child_pid);
        kill(child_pid, SIGCONT);
        int status = 0;
        waitpid(child_pid, &status, WUNTRACED);
        if (!WIFSTOPPED(status)) {
            mx_pop_back(&ush->background);
            ush->count_bg--;
        }

        tcsetpgrp(STDIN_FILENO, getpgrp());
        mx_enable_canon();
        ush->err = 0;
        return;
    }

    t_list *temp = ush->argv;

    for (int i = 0; i < mx_list_size(ush->argv); i++) {
        fger(temp->data, ush);
        temp = temp->next;
    }
    return;
}
