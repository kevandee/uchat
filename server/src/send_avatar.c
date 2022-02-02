#include "../inc/uch_server.h"

void send_avatar(t_avatar *avatar, int client_fd) {
    char *temp_str = avatar->path;
    while (mx_strchr(temp_str,'/')) {
        temp_str = mx_strchr(temp_str,'/') + 1;
    }
    char buf_name[32] = {0};
    sprintf(buf_name, "%s", temp_str);
    send_all(client_fd, buf_name, 32);
    clear_message(buf_name, 32);
    if (mx_strcmp(avatar->path, "default") != 0){
        send_image(client_fd, avatar->path);
        recv_all(client_fd, buf_name, 14);
        send(client_fd, &avatar->scaled_w, sizeof(double), 0);
        send(client_fd, &avatar->scaled_h, sizeof(double), 0);
        send(client_fd, &avatar->x, sizeof(double), 0);
        send(client_fd, &avatar->y, sizeof(double), 0);
    }
}
