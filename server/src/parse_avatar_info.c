#include "../inc/uch_server.h"

t_avatar *parse_avatar_info(char *avatar_info) {
    t_avatar *avatar = (t_avatar *)malloc(sizeof(t_avatar));
    avatar->name = NULL;
    avatar->path = NULL;
    if (mx_strcmp (avatar_info, "default") == 0) {
        avatar->path = mx_strdup(avatar_info);
    } 
    else {
        char *temp = avatar_info + 5;
        int len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        avatar->path = mx_strndup(avatar_info + 5, len);
        temp = mx_strstr(avatar_info, "scaled_w=") + 9;
        len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        //char *num = mx_strndup(temp, len);
        char *end = temp + len;
        avatar->scaled_w = strtod(temp, &end);
        //mx_strdel(&num);

        temp = mx_strstr(avatar_info, "scaled_h=") + 9;
        len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        char *num = mx_strndup(temp, len);
        end = temp + len;
        avatar->scaled_h = strtod(temp, &end);
        mx_strdel(&num);

        temp = mx_strstr(avatar_info, "x=") + 2;
        len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        num = mx_strndup(temp, len);
        end = temp + len;
        avatar->x = strtod(temp, &end);
        mx_strdel(&num);

        temp = mx_strstr(avatar_info, "y=") + 2;
        len = 0;
        while (*(temp + len) != ' ') {
            len++;
        }
        num = mx_strndup(temp, len);
        end = temp + len;
        avatar->y = strtod(temp, &end);
        mx_strdel(&num);
    }
    return avatar;
}
