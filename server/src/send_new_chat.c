#include "../inc/uch_server.h"

void send_new_chat(t_chat *new_chat) {
    
    t_list *users = (new_chat)->users;

    while(users) {
        t_client *client = get_client_by_name(users->data);

        if (client) {
            printf("chat sended to %s\n", client->login);
            char msg[512 + 32] = {0};
            sprintf(msg, "%s", "<add chat>");
            send_all(client->cl_socket, msg, 512 + 32);
            char buf[256] = {0};
            sprintf(buf, "%s",(new_chat)->name);
            printf("name: %s\n", (new_chat)->name);
            send_all(client->cl_socket, (new_chat)->name, 256);
            send(client->cl_socket, &(new_chat)->id, sizeof(int), 0);
            printf("count users: %d\n", (new_chat)->count_users);
            send(client->cl_socket, &(new_chat)->count_users, sizeof(int), 0);
            t_list *temp_l = (new_chat)->users;
            while(temp_l) {
                char user_name[32]={0};
                sprintf(user_name, "%s", temp_l->data);
                printf("users: %s\n", temp_l->data);
                send_all(client->cl_socket, user_name, 32);
                temp_l = temp_l->next;
            }
            printf("chat sended to %s\n", client->login);
        }

        users = users->next;
    }
    printf("done\n");
}
