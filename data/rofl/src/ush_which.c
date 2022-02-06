#include "../inc/ush.h"

static int input_check_reserved(t_ush *ush){
    char *shell_reserved[] = {"do", "done", "esac", "then", "elif", "else", "fi",
                            "for", "case", "if", "while", "function", "repeat",
                            "time","until", "select", "coproc", "nocorrect",
                            "foreach", "end", "!", "[", "{", "}"};
    
    for(int i = 0; i < 24; i++){
        if(mx_strcmp(ush->argv->data, shell_reserved[i]) == 0){
            mx_printerr(ush->argv->data);
            mx_printerr(": ush reserved word\n");
            return 1;
        }
    }
    return 0;
}

static int input_check_builtin(t_ush *ush){
    char *builtIn_commands[] = {":", ".", "[", "alias", "bg", "bind", "break", "builtin", "case",
 "cd", "command", "compgen", "complete", "continue", "declare", "dirs", "disown", "echo",
  "enable", "eval", "exec", "exit", "export", "fc", "fg", "getopts", "hash", "help", "history",
   "if", "jobs", "kill", "let", "local", "logout", "popd", "printf", "pushd", "pwd", "read",
    "readonly", "return", "set", "shift", "shopt", "source", "suspend", "test", "times", "trap",
  "type", "typeset", "ulimit", "umask", "unalias", "unset", "until", "wait", "while"};

    for(int i = 0; i < 59; i++){
        if(mx_strcmp(ush->argv->data, builtIn_commands[i]) == 0){
            mx_printerr(ush->argv->data);
            mx_printerr(": ush built-in command\n");
            return 1;
        }
    }
    return 0;    
}

static int which_path(char *buff, char **path, bool flag_a, bool flag_s, bool is_reserved, bool is_builtin){
    char *env_path = mx_strdup(getenv("PATH"));
    bool exist = false;
    
    path = mx_strsplit(env_path, ':');

    DIR *dir;
    struct dirent *dirent;

    for(int j = 0; path[j]; j++){
        dir = opendir(path[j]);
        if(!dir){
            continue;
        }
        while((dirent = readdir(dir)) != NULL){
            if(mx_strcmp(dirent->d_name, buff) == 0 && !is_reserved && !is_builtin){
                if(flag_s && !flag_a){
                    return 0;
                }
                else{
                    exist = true;
                    mx_printstr(path[j]);
                    mx_printchar('/');
                    mx_printstr(dirent->d_name);
                    mx_printchar('\n');
                    break;
                }
            }
            else if(mx_strcmp(dirent->d_name, buff) == 0 && flag_a){
                if(flag_s && !flag_a){
                    return 0;
                }
                else{
                    exist = true;
                    mx_printstr(path[j]);
                    mx_printchar('/');
                    mx_printstr(dirent->d_name);
                    mx_printchar('\n');
                }
                if(mx_strcmp(dirent->d_name, buff) == 0 && !is_reserved && exist && flag_a && is_builtin){
                    continue;
                }
                else{
                    break;
                }
            }
        }
        closedir(dir);
    }
    if(!exist && !is_reserved && !is_builtin && !flag_s){
        mx_printerr(buff);
        mx_printerr(" not found\n");
        return 1;
    }
    else if(flag_s){
        return 1;
    }

    mx_strdel(&env_path);
    return 0;
}


int ush_which(t_ush *ush){
    char *buff = NULL;
    char **path = NULL;
    bool flag_s = false;
    bool flag_a = false;

    if (!ush->argv || mx_list_size(ush->argv) < 1) {
        return 1;
    }

    if(mx_strncmp(ush->argv->data, "-*", 1) == 0){
        if(mx_strcmp(ush->argv->data, "-s") == 0){
            flag_s = true;
        }
        else if(mx_strcmp(ush->argv->data, "-a") == 0){
            flag_a = true;
        }
        else if(mx_strcmp(ush->argv->data, "--") == 0){
            return 1;
        }
        else if(mx_strcmp(ush->argv->data, "-sa") == 0|| mx_strcmp(ush->argv->data, "-as") == 0){
            flag_a = true;
            flag_s = true;
        }
        else{
            mx_printerr("which: bad option: ");
            mx_printerr(ush->argv->data);
            mx_printerr("\n");
            return 1;
        }
        ush->argv = ush->argv->next;
    }

    while(ush->argv){
        bool is_reserved = false;
        bool is_builtin = false;

        buff = ush->argv->data;
        if(input_check_reserved(ush) == 1){
            is_reserved = true;
        }
        if(input_check_builtin(ush) == 1){
            is_builtin = true;
        }
        if(which_path(buff, path, flag_a, flag_s, is_reserved, is_builtin) == 1){
            return 1;
        }
        ush->argv = ush->argv->next;
    }

    return 0;
}
