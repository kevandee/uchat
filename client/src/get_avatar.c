#include "../inc/uch_client.h"

t_avatar *get_avatar(t_avatar *avatar) {
    char buf[32] = {0};
    swiftchat_recv(cur_client.ssl, buf, 32);
    avatar->name = mx_strdup(buf);
    if (mx_strcmp(avatar->name, "default") != 0) {
        char *pattern = "client_data/%s";
        asprintf(&avatar->path, pattern, avatar->name);
        recv_image(cur_client.ssl, avatar->path);
        swiftchat_send(cur_client.ssl, "<image loaded>", 14); 
        swiftchat_recv(cur_client.ssl, &avatar->scaled_w, sizeof(double));
        swiftchat_recv(cur_client.ssl, &avatar->scaled_h, sizeof(double));
        swiftchat_recv(cur_client.ssl, &avatar->x, sizeof(double));
        swiftchat_recv(cur_client.ssl, &avatar->y, sizeof(double));
    }

    return avatar;
}
