#include "../inc/uch_server.h"

t_client *get_client_by_name(char *name) {
    t_list *temp = users_list;
    while (temp) {
        if (mx_strcmp(name, ((t_client *)(temp->data))->login) == 0) {
            return (t_client *)temp->data;
        }

        temp = temp->next;
    }

    return NULL;
}

int send_all(int sockfd, const char *buf, int len)
{
    ssize_t n;

    while (len > 0)
    {
        n = send(sockfd, buf, len, 0);
        if (n < 0)
            return -1;
        buf += n;
        len -= n;
    }

    return 0;
}

void send_new_chat(t_chat **new_chat) {
    
    t_list *users = (*new_chat)->users;

    while(users) {
        t_client *client = get_client_by_name(users->data);

        if (client) {
            printf("chat sended to %s\n", client->login);
            char msg[512] = {0};
            sprintf(msg, "%s", "add chat");
            send_all(client->cl_socket, msg, 512);
            char buf[256] = {0};
            sprintf(buf, "%s",(*new_chat)->name);
            //int size = 0, total_size = 0;
            /*while (total_size <256) {
                size = send(client->cl_socket, buf + total_size, 256 - total_size, 0);
                total_size+=size;
            }*/
            printf("name: %s\n", (*new_chat)->name);
            send_all(client->cl_socket, (*new_chat)->name, 256);
            printf("count users: %d\n", (*new_chat)->count_users);
            send(client->cl_socket, &(*new_chat)->count_users, sizeof(int), 0);
            t_list *temp_l = (*new_chat)->users;
            while(temp_l) {
                char user_name[32]={0};
                sprintf(user_name, "%s", temp_l->data);
                printf("users: %s\n", temp_l->data);
                send_all(client->cl_socket, user_name, 32);
                temp_l = temp_l->next;
            }
            printf("chat sended to %s\n", client->login);
            //send(client->cl_socket, *new_chat, sizeof(**new_chat), 0);
        }

        users = users->next;
    }
    printf("done\n");
}
