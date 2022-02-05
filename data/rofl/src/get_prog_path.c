#include "../inc/ush.h"

char *get_prog_path(char *name, const char *PATH) {
    struct stat buf;
    if (stat(name, &buf) == 0) {
        return name;
    }
    
    char **arr_path = mx_strsplit(PATH, ':');
    if (!arr_path) {
        return NULL;
    }
    for (int i = 0; arr_path[i]; i++) {
        DIR *cur_dir = opendir(arr_path[i]);
        if (cur_dir) {
            struct dirent *temp = readdir(cur_dir);
            while (temp) {
                //////////////
                if (mx_strcmp(name, temp->d_name) == 0) {
                    char *temp_path = mx_strjoin(arr_path[i], "/");
                    char *path = mx_strjoin(temp_path, temp->d_name);
                    mx_strdel(&temp_path);
                    closedir(cur_dir);
                    return path;
                }
                /////////////////
                temp = readdir(cur_dir);
            }
        }    
    }

    return NULL;
}
