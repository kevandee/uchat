#include "../inc/uch_client.h"

t_avatar *get_avatar(t_avatar *avatar) {
    char buf[32] = {0};
    recv_all(cur_client.ssl, buf, 32);
    avatar->name = mx_strdup(buf);
    if (mx_strcmp(avatar->name, "default") != 0) {
        char *pattern = "client_data/%s";
        asprintf(&avatar->path, pattern, avatar->name);
        recv_image(cur_client.ssl, avatar->path);
        send_all(cur_client.ssl, "<image loaded>", 14); 
        printf("a\n");
        SSL_read(cur_client.ssl, &avatar->scaled_w, sizeof(double));
        SSL_read(cur_client.ssl, &avatar->scaled_h, sizeof(double));
        SSL_read(cur_client.ssl, &avatar->x, sizeof(double));
        SSL_read(cur_client.ssl, &avatar->y, sizeof(double));
    }

    return avatar;
}
