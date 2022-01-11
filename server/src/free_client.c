#include "../inc/uch_server.h"

void free_client(t_client **client, t_list **users_list) {
    t_client *cur = *client;
    mx_strdel(&cur->login);
    t_list *del = *users_list;
    t_list *prev = NULL;
    while (del) {
        if (((t_client *)del->data)->id == cur->id) {
            break;
        }
        prev = del;
        del = del->next;
    }
    if (!prev) {
        //printf("front\n");
        mx_pop_front(users_list);
        t_list *temp = *users_list;
        while (temp) {
            ((t_client *)temp->data)->id--;
            temp = temp->next;
        }
        free(cur);
        cur = NULL;
    }
    else if (!del->next) {
        //printf("back\n");
        mx_pop_back(users_list);
        free(cur);
        cur = NULL;
    }
    else {
        //printf("prev = %d; del = %d;\n", ((t_client *)prev->data)->id, ((t_client *)del->data)->id);
        
        if (del->next) {
            prev->next = del -> next;
        }
        else {
            prev->next = NULL;
        }
        free(del);
        del = NULL;
        free(cur);
        cur = NULL;
    }

    pthread_detach(pthread_self());
}
