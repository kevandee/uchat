#include "../inc/uch_client.h"

t_avatar *get_avatar(t_avatar *avatar) {
    char buf[32] = {0};
    recv_all(cur_client.serv_fd, buf, 32);
    avatar->name = mx_strdup(buf);
    if (mx_strcmp(avatar->name, "default") != 0) {
        char *pattern = "client_data/%s";
        asprintf(&avatar->path, pattern, avatar->name);
        recv_image(cur_client.serv_fd, avatar->path);
        send_all(cur_client.serv_fd, "<image loaded>", 14); 
        printf("a\n");
        recv(cur_client.serv_fd, &avatar->scaled_w, sizeof(double), 0);
        recv(cur_client.serv_fd, &avatar->scaled_h, sizeof(double), 0);
        recv(cur_client.serv_fd, &avatar->x, sizeof(double), 0);
        recv(cur_client.serv_fd, &avatar->y, sizeof(double), 0);
    }

    return avatar;
}
