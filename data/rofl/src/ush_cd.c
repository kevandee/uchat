#include "../inc/ush.h"

void ush_cd(t_ush *ush) {
    bool P_opt = false;
    bool s_opt = false;
    t_list *temp = ush->argv;
    char **argv = NULL;
    list_to_arr(temp, &argv);
    for (int i = mx_list_size(temp) - 1; i >= 0; i--) {
        if (argv[i][0] == '-' && argv[i][1] != '/'){
            for (int j = mx_strlen(argv[i]) - 1; j > 0; j--) {
                if (argv[i][0] == '-' && argv[i][j] != 's' && argv[i][j] != 'P') {
                    mx_printerr("u$h: cd: -");
                    mx_printerr(&argv[i][j]);
                    mx_printerr(": invalid option\n");
                    mx_printerr("cd: usage: cd [-s] [-P] [dir]\n");
                    ush->err = 1;
                    return;
                }
                if (argv[i][j] == 'P' && argv[i][0] == '-') {
                    P_opt = true;
                }
                if (argv[i][j] == 's' && argv[i][0] == '-') {
                    s_opt = true;
                }
            }
            mx_strdel(&argv[i]);
            argv[i] = NULL;
        }
    }
    free(argv);
    argv = NULL;

    if (!ush->argv) {
        if (malloc_size(ush -> prev_pwd) != 0)
            mx_strdel(&ush->prev_pwd);
        ush->prev_pwd = mx_strdup(ush->pwd);
        ush->pwd = getenv("HOME");
        chdir(ush->pwd);
        return;
    }

    if (mx_list_size(ush->argv) > 2) {
        mx_printerr("cd: too many arguments\n");
        ush -> err = 1;
        return;
    }

    while (((char *)ush->argv->data)[0] == '-' && ((char *)ush->argv->data)[1] != '/' && mx_strcmp(ush->argv->data, "-") != 0) {
        mx_pop_front(&ush->argv);
    }
    
    if (s_opt) {
        char *check = realpath(ush->argv->data, NULL);
        if (check && mx_strcmp(check, ush->argv->data) != 0) {
            mx_printerr("ush: cd: ");
            mx_printerr(ush->argv->data);
            mx_printerr(": Not a directory\n");
        }
    }

    if (mx_strcmp(ush->argv->data,"-") == 0) {
        char *temp = ush->prev_pwd;
        ush->prev_pwd = ush->pwd;
        ush->pwd = temp;
        chdir(ush->pwd);
        if (mx_strstr(temp, getenv("HOME"))) {
            temp = mx_replace_substr(temp, getenv("HOME"), "~");
            mx_printstr(temp);
            mx_printchar('\n');
            mx_strdel(&temp);
        }
        else {
            mx_printstr(temp);
            mx_printchar('\n');
        }
        return;
    }

    if (((char *)ush->argv->data)[0] == '-' && ((char *)ush->argv->data)[1] == '/') {
       char *temp = mx_strjoin(getenv("OLDPWD") ,((char *)ush->argv->data) + 1);
        char *del = ush->argv->data;
        mx_strdel(&(del));
        ush->argv->data = temp;
    }
    char *res_pwd = mx_strdup(ush->argv->data);
    char *temp_pwd = mx_strdup(ush -> pwd);
    if (((char *)(ush->argv->data))[0] != '/') {
        mx_strdel(&res_pwd);
        char *str_path = ush->argv->data;
        char **path = mx_strsplit(str_path,'/');

        for (int i = 0; path[i]; i++) {
            if (mx_strcmp(path[i], "..") == 0) {
                char *temp = temp_pwd;
                while (mx_strchr(temp, '/')) {
                    temp = mx_strchr(temp, '/') + 1;
                }
                res_pwd = mx_strndup(temp_pwd, mx_strlen(temp_pwd) - mx_strlen(temp) - 1);
            }
            else if (mx_strcmp(path[i], ".") == 0){
                continue;
            }
            else {
                char *temp = mx_strdup(temp_pwd);
                if (temp_pwd[mx_strlen(temp_pwd) - 1] != '/' && mx_strlen(temp_pwd) != 0) {
                    mx_strdel(&temp);
                    temp = mx_strjoin (temp_pwd, "/");
                }
                res_pwd = mx_strjoin(temp, path[i]);
                mx_strdel(&temp);
            }
            mx_strdel(&temp_pwd);
            temp_pwd = mx_strdup(res_pwd);
            mx_strdel(&path[i]);
            path[i] = NULL;
        }
        free(path);
        path=NULL;

        if (mx_strlen(res_pwd) == 0) {
            mx_strdel(&res_pwd);
            res_pwd = mx_strdup("/");
        }
    }
    struct stat buf;
    if (stat(res_pwd, &buf) == 0) {
        if (!S_ISDIR(buf.st_mode)) {
            mx_printerr("cd: not a directory: ");
            mx_printerr(ush->argv->data);
            mx_printerr("\n");
            ush -> err = 1;
            return;
        }
    }
    else {
        mx_printerr("cd: no such file or directory: ");
        mx_printerr(ush->argv->data);
        mx_printerr("\n");
        ush -> err = 1;
        return;
    }
    char *temp_path = mx_strjoin(res_pwd, "/.");
    if(lstat(temp_path, &buf) == -1) {
        if (errno == 13) {
            mx_printerr("cd: permission denied: ");
            mx_printerr(ush->argv->data);
            mx_printerr("\n");
            errno = 0;
            ush -> err = 1;
            return;
        }
    }
    mx_strdel(&temp_path);
    mx_strdel(&temp_pwd);
    if (malloc_size(ush -> prev_pwd) != 0)
        mx_strdel(&ush->prev_pwd);
    ush->prev_pwd = mx_strdup(ush->pwd);
    setenv("OLDPWD",ush->pwd,1);
    if (malloc_size(ush -> pwd) != 0)
        mx_strdel(&ush -> pwd);
    ush->pwd = res_pwd;
    setenv("PWD",res_pwd,1);
    chdir(res_pwd);
    if (P_opt) {
        char *dir = getcwd(NULL, 1024);
        if (dir) {
            if (malloc_size(ush -> pwd) != 0)
                mx_strdel(&ush->pwd);
            ush->pwd = dir;
            setenv("PWD",dir,1);
            chdir(dir);
        }
    }
    ush->err = 0;
}
