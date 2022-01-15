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
