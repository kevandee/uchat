#include "../inc/uch_server.h"

void send_all_user_data(t_client *client) {
    t_client *send_cl = get_user_info(client->id);
    char buf_name[32] = {0};
    sprintf(buf_name, "%s", send_cl->name);
    send_all(client->cl_socket, buf_name, 32);
    clear_message(buf_name, 32);

    sprintf(buf_name, "%s", send_cl->surname);
    send_all(client->cl_socket, buf_name, 32);
    clear_message(buf_name, 32);
    char buf[256] = {0};
    sprintf(buf, "%s",send_cl->bio);

    send_all(client->cl_socket, buf, 256);
    clear_message(buf, 256);

    char *temp_str = send_cl->avatar.path;
    while (mx_strchr(temp_str,'/')) {
        temp_str = mx_strchr(temp_str,'/') + 1;
    }

    sprintf(buf_name, "%s", temp_str);
    send_all(client->cl_socket, buf_name, 32);
    clear_message(buf_name, 32);
    if (mx_strcmp(send_cl->avatar.path, "default") != 0){
        send_image(client->cl_socket, send_cl->avatar.path);
        recv_all(client->cl_socket, buf_name, 14);
        send(client->cl_socket, &send_cl->avatar.scaled_w, sizeof(double), 0);
        send(client->cl_socket, &send_cl->avatar.scaled_h, sizeof(double), 0);
        send(client->cl_socket, &send_cl->avatar.x, sizeof(double), 0);
        send(client->cl_socket, &send_cl->avatar.y, sizeof(double), 0);
    }

    send(client->cl_socket, &send_cl->chat_count, sizeof(int), 0);

    t_list *temp = send_cl->chats;

    while(temp) {
        t_chat *chat = (t_chat *)temp->data;
        
        sprintf(buf, "%s",(chat)->name);
        printf("name: %s\n", (chat)->name);
        send_all(client->cl_socket, (chat)->name, 256);
        send(client->cl_socket, &(chat)->id, sizeof(int), 0);
        printf("id: %d\n", (chat)->id); 
        printf("count users: %d\n", (chat)->count_users);
        send(client->cl_socket, &(chat)->count_users, sizeof(int), 0);
        t_list *temp_l = (chat)->users;
        while(temp_l) {
            char user_name[32]={0};
            sprintf(user_name, "%s", temp_l->data);
            printf("users: %s\n", temp_l->data);
            send_all(client->cl_socket, user_name, 32);
            temp_l = temp_l->next;
        }
        printf("chat sended to %s\n", client->login);

        temp = temp->next;
    }
}
