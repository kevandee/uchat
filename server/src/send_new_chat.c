#include "../inc/uch_server.h"

void send_new_chat(t_chat *new_chat) {
    
    t_list *users = (new_chat)->users;

    while(users) {
        t_client *client = get_client_by_name(users->data);

        if (client) {
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

            /*
            t_avatar *avatar = NULL;
            char *temp_str = NULL;
            if (mx_strncmp(new_chat->name, ".dialog", 7) == 0) {
                t_list *find_user = new_chat->users;

                char *avatar_info = get_user_avatar(get_user_id(users->data));
                avatar = parse_avatar_info(avatar_info);
                
                temp_str = avatar->path;
                while (mx_strchr(temp_str,'/')) {
                    temp_str = mx_strchr(temp_str,'/') + 1;
                }
            }

            char buf_name[32] = {0};
            sprintf(buf_name, "%s", temp_str);
            send_all(client->cl_socket, buf_name, 32);
            clear_message(buf_name, 32);
            if (mx_strcmp(avatar->path, "default") != 0){
                send_image(client->cl_socket, avatar->path);
                recv_all(client->cl_socket, buf_name, 14);
                send(client->cl_socket, &avatar->scaled_w, sizeof(double), 0);
                send(client->cl_socket, &avatar->scaled_h, sizeof(double), 0);
                send(client->cl_socket, &avatar->x, sizeof(double), 0);
                send(client->cl_socket, &avatar->y, sizeof(double), 0);
            }
            */
            printf("chat sended to %s\n", client->login);
        }

        users = users->next;
    }
    printf("done\n");
}
