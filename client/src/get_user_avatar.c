#include "../inc/uch_client.h"

t_list *get_chat_users_avatars(t_chat *chat) {
    int count_users = chat->count_users - 1;
    t_list *avatar_list = NULL;
    for (int i = 0; i < count_users; i++) {
        t_avatar *new_avatar = (t_avatar *)malloc(sizeof(t_avatar));
        char buf[32] = {0};
        swiftchat_recv(cur_client.ssl, buf, 32);
        new_avatar->name = mx_strdup(buf);
        clear_message(buf, 32);
        if (mx_strcmp(new_avatar->name, "default") != 0) {
            char *pattern = "client_data/%s";
            asprintf(&new_avatar->path, pattern, new_avatar->name);
            recv_image(cur_client.ssl, new_avatar->path);
            swiftchat_send(cur_client.ssl, "<image loaded>", 14); 
            swiftchat_recv(cur_client.ssl, &new_avatar->scaled_w, sizeof(double));
            swiftchat_recv(cur_client.ssl, &new_avatar->scaled_h, sizeof(double));
            swiftchat_recv(cur_client.ssl, &new_avatar->x, sizeof(double));
            swiftchat_recv(cur_client.ssl, &new_avatar->y, sizeof(double));
        }

        mx_push_back(&avatar_list, new_avatar);
    }

    return avatar_list;
}
