#include "../inc/ush.h"

int bg_new_id(t_list *background) {
    if (!background) {
        return 1;
    }
    t_list *first = background;
    while (first->next) {
        if (((t_process *)(first->data))->id != ((t_process *)(first->next->data))->id - 1) {
            return ((t_process *)(first->data))->id + 1;
        }
        first = first->next;
    }
    return mx_list_size(background);
}

t_process *new_process(pid_t pid) {
    t_process *new = (t_process *)malloc(sizeof(t_process));
    new->pid = pid;
    new->pgid = getpgid(pid);
    new->status = 1;
    new->foregrd = 1;
    return new;
}

void add_bg(t_process *proc, t_ush *ush) {
    tcsetpgrp(STDIN_FILENO, getpgrp());
    mx_enable_canon();
    proc->id = bg_new_id(ush->background);
    mx_push_back(&ush->background, proc);
    ush->count_bg++;
    printf("[%d]    %d suspended  %s\n", ush->count_bg, proc->pid, ush->argv_arr[0]);
}
