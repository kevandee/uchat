#include "../inc/ush.h"

extern char **environ;

char *del_quot(char *src) {
    char *temp = mx_strdup(src);
    int count_quot = 0;
    for (int i = 0; i < mx_strlen(temp); i++) {
        if (temp[i] == '\'' ||temp[i] == '\"') {
            count_quot++;
        }
    }
    if (!count_quot) {
        return temp;
    }
    int size = mx_strlen(temp) - count_quot;
    char *res = mx_strnew(size);
    int k = 0;
    for (int i = 0; i < mx_strlen(temp); i++) {
        if (temp[i] != '\"' && temp[i] != '\'') {
            res[k] = temp[i];
            k++;
        }
    }
    return res;
}

static int env_parser(t_list *argv, bool *i_opt, bool *u_opt, bool *P_opt, t_list **u_param, t_list **p_param, t_list **prog_argv) {
    char **arr = NULL;
    //list_to_arr(argv, &arr);
    t_list *list = argv;
    arr = (char **)malloc((mx_list_size(list) + 1) * sizeof(char *));
    int k = 0;
    while (list) {
        (arr)[k] = mx_strdup(list->data);
        k++;
        list = list -> next;
    }
    arr[k] = NULL;
    for (int i = 0; i < mx_list_size(argv); i++) {
        bool cur_i = false;
        bool cur_u = false;
        bool cur_P = false;
        if (arr[i][0] == '-') {
            for (int j = 1; j < mx_strlen(arr[i]); j++) {
                if (arr[i][j] == 'u') {
                    if (!cur_i) {
                        *u_opt = true;
                        cur_u = true;
                        if (!arr[i + 1]) {
                            *u_opt = false;
                            mx_printerr("env: option requires an argument -- u\n");
                            return -1;
                        }
                        mx_push_back(u_param, mx_strdup(arr[i + 1]));
                        i++;
                        break;
                    }
                }
                else if (arr[i][j] == 'i') {
                    if (!(cur_u)) {
                        *i_opt = true;
                        cur_i = true;
                    }
                    else {
                        *u_opt = false;
                        cur_u = false;
                    }
                }
                else if(arr[i][j] == 'P' && !cur_u) {
                    *P_opt = true;
                    cur_P = true;
                    if (!arr[i + 1]) {
                        *P_opt = false;
                        mx_printerr("env: option requires an argument -- P\n");
                        return -1;
                    }
                    mx_push_back(p_param, mx_strdup(arr[i + 1]));
                    i++;
                    break;
                }
                else {
                    while (i < mx_list_size(argv)) {
                        mx_strdel(&arr[i]);
                        i++;
                    }
                    return -1;
                }
                //mx_strdel(&arr[i]);   
            }
        }
        if (arr[i][0] != '-' && (arr[i - 1][0] != '-' || *i_opt) && i < mx_list_size(argv)) {
            while (i < mx_list_size(argv)) {
                mx_push_back(prog_argv, mx_strdup(arr[i]));
                mx_strdel(&arr[i]);
                i++;
            }
            break;
        }        
    }
    
    //mx_del_strarr(&arr);
    return 0;
}

int launch_proc_env(char **argv, char **env, char *path, t_ush *ush) {
    pid_t pid;
    int status = 0;
    extern char **environ;
    pid = fork();
    mx_disable_canon();
    if (pid == 0) {
        // Дочерний процесс
        sigset_t x;
        sigemptyset (&x);
        sigaddset(&x, SIGINT);
        sigaddset(&x, SIGSTOP);
        sigaddset(&x, SIGTSTP);
        sigprocmask(SIG_UNBLOCK, &x, NULL);
        char *prog = get_prog_path(argv[0], path);

        if (!prog) {
            mx_printerr("env: ");
            mx_printerr(argv[0]);
            mx_printerr(": No such file or directory\n");
            exit(127);
        }
        if (execve(prog, argv, env) == -1) {
            mx_printerr("ush: command not found: ");
            mx_printerr(argv[0]);
            mx_printerr("\n");
            mx_strdel(&prog);
            exit(127);
        }
        mx_strdel(&prog);
        exit(0);
    } else if (pid < 0) {
        // Ошибка при форкинге
        perror("ush");
    } else {
        // Родительский процесс
        t_process *proc = new_process(pid);
        ush->foreground = proc;
        if (waitpid(pid, &status, WUNTRACED) != -1) {
            if (WIFSTOPPED(status)) {
                //add_process(process);
                proc->status = status;
                ush->foreground = NULL;
                add_bg(proc, ush);
                //ush->background = proc;
                
            }
        }
        if (!WIFSTOPPED(status)) {
            tcsetpgrp(STDIN_FILENO, getpgrp());  
            mx_enable_canon();
        }
   }

    return status >> 8;
}

void ush_env(t_ush *ush) {
    bool i_opt = false;
    bool u_opt = false;
    bool P_opt = false;
    t_list *u_param = NULL;
    t_list *p_param = NULL;
    t_list *prog_argv = NULL;
    int res = env_parser(ush->argv, &i_opt, &u_opt, &P_opt, &u_param, &p_param, &prog_argv);

    if (res == -1) {
        mx_printerr("ush: env [-i] [-P utilpath] [-u name]\n");
        ush->err = 1;
        return;
    } 
    char **argv = NULL;
    list_to_arr(ush->argv, &argv);
    char **u_arr = NULL;
    char *path = NULL;
    if (p_param) {
        char **p_arr = NULL;
        list_to_arr(p_param, &p_arr);
        path = mx_strdup(p_arr[0]);
        for (int i = 1; i < mx_list_size(p_param); i++) {
            char *temp1 = mx_strjoin(path, ":");
            mx_strdel(&path);
            path = mx_strjoin(temp1, p_arr[i]);
        }
    }
    if (!path) {
        char *env_p = getenv("PATH");
        if (env_p)
            path = mx_strdup(env_p);
    }
    if (u_param)
        list_to_arr(u_param, &u_arr);

    t_list *list_env = NULL;
    int count_var = 0;
    if (u_opt) {
        int i = 0;
        for (; environ[i]; i++) {
            //if (opt.u) {
                bool cmp = false;
                for (int j = 0; j < mx_list_size(u_param); j++) {
                    if (mx_strncmp(environ[i], u_arr[j], mx_strlen(u_arr[j])) == 0 
                    && environ[i][mx_strlen(u_arr[j])] == '=') {
                        cmp = true;
                    }
                }
            //}
            //printf("%s\n", arr_env[i]);
            if (cmp) {
                continue;
            }
            mx_push_back(&list_env, environ[i]);
        }
        count_var = i;
    }
    char **arr_env = NULL;
    t_list *list = list_env;
    arr_env = (char **)malloc((mx_list_size(list)+1) * sizeof(char *));
    int k = 0;
    while (list) {
        (arr_env)[k] = mx_strdup(list->data);
        k++;
        list = list -> next;
    }
    arr_env[k] = NULL;
    mx_clear_list(&list_env);
    if (prog_argv) {
        char **argv = NULL;
        list_to_arr(prog_argv, &argv);
        if (path) {
            if (i_opt) {
                ush->err = launch_proc_env(argv, NULL, path, ush);
            }
            else {
                ush->err = launch_proc_env(argv, arr_env, path, ush);
            }
        }
        else {
            mx_printerr("env: command not found: ");
            mx_printerr(argv[0]);
            mx_printerr("\n");
            ush->err = 127;
            return;
        }
    }
    else {
        if (count_var) {
            for (int i = 0; i < count_var; i++) {
                if (arr_env[i]) {
                    printf("%s\n", arr_env[i]);
                }
            }
        }
        else {
            if (!i_opt)
                for (int i = 0; environ[i]; i++) { 
                    printf("%s\n", environ[i]);
                }
        }
    }

    mx_strdel(&path);
    ush->err = 0;
    //printf("YYYY\n");
}
