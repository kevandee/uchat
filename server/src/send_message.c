#include "../inc/uch_server.h"

t_list *users_list;
pthread_mutex_t send_mutex;


void send_message(char *mes, char *sender, t_chat *chat) {
    if (mx_list_size(users_list) == 1) {
        return;
    }
    char buf[512 + 32] = {0};
    if (chat)
        sprintf(buf, "<msg, chat_id=%d, from=%s>%s", chat->id, sender, mes);
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
                send_all(cl->cl_socket, buf, 512 + 32);
            }
        }
        else {
            t_client *cl = get_client_by_name(users_tmp->data);
            if (cl->login && mx_strcmp(cl->login, sender) != 0) {
                send_all(cl->cl_socket, buf, 512 + 32);
            }
        }

        users_tmp = users_tmp->next;
    }
    pthread_mutex_unlock(&send_mutex);
    printf("%d users\n", count);
}
