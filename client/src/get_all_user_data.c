#include "../inc/uch_client.h"

t_client cur_client;

void get_all_user_data() {

    swiftchat_recv(cur_client.ssl, cur_client.name, 32);
    swiftchat_recv(cur_client.ssl, cur_client.surname, 32);
    swiftchat_recv(cur_client.ssl, cur_client.bio, 256);

    char buf[32] = {0};
    swiftchat_recv(cur_client.ssl, buf, 32);
    cur_client.avatar.name = mx_strdup(buf);
    if (mx_strcmp(cur_client.avatar.name, "default") != 0) {
        char *pattern = "client_data/%s";
        asprintf(&cur_client.avatar.path, pattern, cur_client.avatar.name);
        recv_image(cur_client.ssl, cur_client.avatar.path);
        swiftchat_send(cur_client.ssl, "<image loaded>", 14); 
        swiftchat_recv(cur_client.ssl, &cur_client.avatar.scaled_w, sizeof(double));
        swiftchat_recv(cur_client.ssl, &cur_client.avatar.scaled_h, sizeof(double));
        swiftchat_recv(cur_client.ssl, &cur_client.avatar.x, sizeof(double));
        swiftchat_recv(cur_client.ssl, &cur_client.avatar.y, sizeof(double));
    }
    
    swiftchat_recv(cur_client.ssl, &cur_client.chat_count, sizeof(int));

    for (int i = 0; i < cur_client.chat_count; i++) {
        t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
        new_chat->is_new = false;
        new_chat->messages = NULL;
        new_chat->users = NULL;

        char buf_name[256] = {0};
        int receive = swiftchat_recv(cur_client.ssl, buf_name, 256);
        while (receive < 0) {
            receive = swiftchat_recv(cur_client.ssl, buf_name, 256);
        }
        mx_strcpy(new_chat->name, buf_name);
        clear_message(buf_name, 256);
        receive = swiftchat_recv(cur_client.ssl, &new_chat->id, sizeof(int));
        while (receive < 0) {
            receive = swiftchat_recv(cur_client.ssl, &new_chat->id, sizeof(int));
        }
        receive = swiftchat_recv(cur_client.ssl, &new_chat->count_users, sizeof(int));
        while (receive < 0) {
            receive = swiftchat_recv(cur_client.ssl, &new_chat->count_users, sizeof(int));
        }
        for (int i = 0; i < new_chat->count_users; i++) {
            char buf[32] = {0};
            receive = swiftchat_recv(cur_client.ssl, buf, 32);
            while (receive < 0) {
                receive = swiftchat_recv(cur_client.ssl, buf, 32);
            }
            mx_push_back(&new_chat->users,mx_strdup(buf));
            clear_message(buf, 32);
        }

        char mute;
        swiftchat_recv(cur_client.ssl, &mute, 1);
        new_chat->mute = mute == '0' ? false : true;
        // chat avatar

        char buf[32] = {0};
        swiftchat_recv(cur_client.ssl, buf, 32);
        new_chat->avatar.name = mx_strdup(buf);
        struct stat st = {0};
        if (stat("client_data", &st) == -1) {
            mkdir("client_data", 0777);
        }
        if (mx_strcmp(new_chat->avatar.name, "default") != 0) {
            char *pattern = "client_data/%s";
            asprintf(&new_chat->avatar.path, pattern, new_chat->avatar.name);
            recv_image(cur_client.ssl, new_chat->avatar.path);
            swiftchat_send(cur_client.ssl, "<image loaded>", 14); 
            swiftchat_recv(cur_client.ssl, &new_chat->avatar.scaled_w, sizeof(double));
            swiftchat_recv(cur_client.ssl, &new_chat->avatar.scaled_h, sizeof(double));
            swiftchat_recv(cur_client.ssl, &new_chat->avatar.x, sizeof(double));
            swiftchat_recv(cur_client.ssl, &new_chat->avatar.y, sizeof(double));
        }
        else {
            if (mx_strncmp(new_chat->name,".dialog", 7) == 0)
                new_chat->avatar = t_main.default_avatar;
            else 
                new_chat->avatar = t_main.default_group_avatar;
        }

        //////////////
        mx_push_back(&cur_client.chats, new_chat);
    }

    t_main.loaded = true;
}
