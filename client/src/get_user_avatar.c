#include "../inc/uch_client.h"

t_list *get_chat_users_avatars(t_chat *chat) {
    int count_users = chat->count_users - 1;
    t_list *avatar_list = NULL;
    printf("count %d\n", count_users);
    for (int i = 0; i < count_users; i++) {
        t_avatar *new_avatar = (t_avatar *)malloc(sizeof(t_avatar));
        char buf[32] = {0};
        recv_all(cur_client.serv_fd, buf, 32);
        new_avatar->name = mx_strdup(buf);
        clear_message(buf, 32);
        printf("a\n");
        if (mx_strcmp(new_avatar->name, "default") != 0) {
            char *pattern = "client_data/%s";
            asprintf(&new_avatar->path, pattern, new_avatar->name);
            printf("b\n");
            recv_image(cur_client.serv_fd, new_avatar->path);
            send_all(cur_client.serv_fd, "<image loaded>", 14); 
            recv(cur_client.serv_fd, &new_avatar->scaled_w, sizeof(double), 0);
            recv(cur_client.serv_fd, &new_avatar->scaled_h, sizeof(double), 0);
            recv(cur_client.serv_fd, &new_avatar->x, sizeof(double), 0);
            recv(cur_client.serv_fd, &new_avatar->y, sizeof(double), 0);
        }

        mx_push_back(&avatar_list, new_avatar);
    }

    return avatar_list;
}
