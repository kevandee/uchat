#include "../inc/uch_server.h"

t_list *users_list;
pthread_mutex_t send_mutex;


void send_message(char *mes, char *sender, t_chat *chat) {
    pthread_mutex_lock(&send_mutex);
    int count = 0;
    t_list *users_tmp = NULL;
    if (!chat) {
        users_tmp = users_list; 
    }
    else {
        users_tmp = chat->users;
    }
    while (users_tmp) {
        count++;
        if (!chat) {
            if (((t_client *)(users_tmp->data))->login && mx_strcmp(((t_client *)(users_tmp->data))->login, sender) != 0) {
                t_client *cl = ((t_client *)(users_tmp->data));
                send(cl->cl_socket, mes, mx_strlen(mes), 0);
            }
        }
        else {
            t_client *cl = get_client_by_name(users_tmp->data);
            if (cl->login && mx_strcmp(cl->login, sender) != 0) {
                send(cl->cl_socket, mes, mx_strlen(mes), 0);
            }
        }

        users_tmp = users_tmp->next;
    }
    pthread_mutex_unlock(&send_mutex);
    printf("%d users\n", count);
}
