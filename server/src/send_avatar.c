#include "../inc/uch_server.h"

void send_avatar(t_avatar *avatar, SSL *ssl) {
    char *temp_str = avatar->path;
    while (mx_strchr(temp_str,'/')) {
        temp_str = mx_strchr(temp_str,'/') + 1;
    }
    char buf_name[32] = {0};
    sprintf(buf_name, "%s", temp_str);
    send_all(ssl, buf_name, 32);
    clear_message(buf_name, 32);
    if (mx_strcmp(avatar->path, "default") != 0){
        send_image(ssl, avatar->path);
        recv_all(ssl, buf_name, 14);
        SSL_write(ssl, &avatar->scaled_w, sizeof(double));
        SSL_write(ssl, &avatar->scaled_h, sizeof(double));
        SSL_write(ssl, &avatar->x, sizeof(double));
        SSL_write(ssl, &avatar->y, sizeof(double));
    }
}
