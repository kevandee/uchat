#include "../inc/uch_server.h"

void send_all_user_data(t_client *client) {
    t_client *send_cl = get_user_info(client->id);
    char buf_name[32] = {0};
    sprintf(buf_name, "%s", send_cl->name);
    send_all(client->ssl, buf_name, 32);
    clear_message(buf_name, 32);

    sprintf(buf_name, "%s", send_cl->surname);
    send_all(client->ssl, buf_name, 32);
    clear_message(buf_name, 32);
    char buf[256] = {0};
    sprintf(buf, "%s",send_cl->bio);

    send_all(client->ssl, buf, 256);
    clear_message(buf, 256);

    char *temp_str = send_cl->avatar.path;
    while (mx_strchr(temp_str,'/')) {
        temp_str = mx_strchr(temp_str,'/') + 1;
    }

    sprintf(buf_name, "%s", temp_str);
    send_all(client->ssl, buf_name, 32);
    clear_message(buf_name, 32);
    if (mx_strcmp(send_cl->avatar.path, "default") != 0){
        send_image(client->ssl, send_cl->avatar.path);
        recv_all(client->ssl, buf_name, 14);
        SSL_write(client->ssl, &send_cl->avatar.scaled_w, sizeof(double));
        SSL_write(client->ssl, &send_cl->avatar.scaled_h, sizeof(double));
        SSL_write(client->ssl, &send_cl->avatar.x, sizeof(double));
        SSL_write(client->ssl, &send_cl->avatar.y, sizeof(double));
    }

    SSL_write(client->ssl, &send_cl->chat_count, sizeof(int));

    t_list *temp = send_cl->chats;

    while(temp) {
        t_chat *chat = (t_chat *)temp->data;
        
        sprintf(buf, "%s",(chat)->name);
        send_all(client->ssl, (chat)->name, 256);
        SSL_write(client->ssl, &(chat)->id, sizeof(int));
        SSL_write(client->ssl, &(chat)->count_users, sizeof(int));
        t_list *temp_l = (chat)->users;
        while(temp_l) {
            char user_name[32]={0};
            sprintf(user_name, "%s", temp_l->data);
            send_all(client->ssl, user_name, 32);
            temp_l = temp_l->next;
        }

        char *query = NULL;
        char *sql_pattern = "SELECT mute FROM members WHERE chat_id = %d AND user_id = %d;";
        asprintf(&query, sql_pattern, chat->id, client->id);
        t_list *temp_m = sqlite3_exec_db(query, 1);
        if (strcmp(temp_m->data, "0") == 0) {
            send_all(client->ssl, "0", 1);
        }
        else {
            send_all(client->ssl, "1", 1);
        }

        temp = temp->next;
        char *avatar_info = NULL;
        if (mx_strncmp(chat->name, ".dialog", 7) != 0){
            avatar_info = get_chat_avatar(chat->id);
        }
        else {
            t_list *find_user = chat->users;
            while (find_user) {
                if (mx_strcmp(client->login, find_user->data) != 0) {
                    break;
                }
        
                find_user = find_user->next;
            }
            avatar_info = get_user_avatar(get_user_id(find_user->data));
        }

        t_avatar *chat_avatar = parse_avatar_info(avatar_info);

        temp_str = chat_avatar->path;
        while (mx_strchr(temp_str,'/')) {
            temp_str = mx_strchr(temp_str,'/') + 1;
        }
        sprintf(buf_name, "%s", temp_str);
        send_all(client->ssl, buf_name, 32);
        clear_message(buf_name, 32);
        if (mx_strcmp(chat_avatar->path, "default") != 0){
            send_image(client->ssl, chat_avatar->path);
            recv_all(client->ssl, buf_name, 14);
            SSL_write(client->ssl, &chat_avatar->scaled_w, sizeof(double));
            SSL_write(client->ssl, &chat_avatar->scaled_h, sizeof(double));
            SSL_write(client->ssl, &chat_avatar->x, sizeof(double));
            SSL_write(client->ssl, &chat_avatar->y, sizeof(double));
        }       
    }
}
