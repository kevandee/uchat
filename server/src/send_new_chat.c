#include "../inc/uch_server.h"

void send_new_chat(t_chat *new_chat) {
    
    t_list *users = (new_chat)->users;

    while(users) {
        t_client *client = get_client_by_name(users->data);

        if (client) {
            char msg[512 + 32] = {0};
            sprintf(msg, "%s", "<add chat>");
            send_all(client->ssl, msg, 512 + 32);
            char buf[256] = {0};
            sprintf(buf, "%s",(new_chat)->name);
            send_all(client->ssl, (new_chat)->name, 256);
            SSL_write(client->ssl, &(new_chat)->id, sizeof(int));
            SSL_write(client->ssl, &(new_chat)->count_users, sizeof(int));
            t_list *temp_l = (new_chat)->users;
            while(temp_l) {
                char user_name[32]={0};
                sprintf(user_name, "%s", temp_l->data);
                send_all(client->ssl, user_name, 32);
                temp_l = temp_l->next;
            }

            if (new_chat->is_new && mx_strncmp(new_chat->name, ".dialog", 7) != 0) {
                t_avatar default_avatar = {.name = "default", .path = "default"};
                send_avatar(&default_avatar, client->ssl);
            }
            printf("chat sended to %s\n", client->login);
        }

        users = users->next;
    }
}
