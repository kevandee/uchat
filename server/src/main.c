#include "../inc/uch_server.h"
#define MAX_LEN 512
#define NAME_LEN 32

#include "../../frameworks/openssl/openssl/ssl.h"

t_list *users_list;

void *client_work(void *param) {
    sqlite3_create_db();

    t_client *cur = (t_client *)param;
    char login[NAME_LEN];
    char passwd[16];
    char buff_out[MAX_LEN + NAME_LEN];
    bool is_run = true;
    char choise;

    bool err_msg = true;
    
    // Proverka svazy
    /*char buf[6];
    SSL_read(cur->ssl, buf, 6);
    SSL_write(cur->ssl, buf, 6);*/
    // Procerka svazy

    while (err_msg) {
        // sign up or sign in
        if(SSL_read(cur->ssl, &choise, 1) <= 0) {
            free_client(&cur, &users_list);
            
            return NULL;
        }
        printf("choise %c\n", choise);
        // login
        if(SSL_read(cur->ssl, login, NAME_LEN) <= 0 || mx_strlen(login) <  2 || mx_strlen(login) >= 32){
            printf("Didn't enter the name.\n");
            free_client(&cur, &users_list);

            return NULL;
        } else{
            cur->login=mx_strtrim(login);
        }

        //passwd
        if(SSL_read(cur->ssl, passwd, 16) <= 0 || mx_strlen(passwd) <  8 || mx_strlen(passwd) > 16){
            printf("Didn't enter the password.\n");
            free_client(&cur, &users_list);

            
            return NULL;

        } else{
            cur->passwd=mx_strtrim(passwd);
        }
        //DB SWITH
        printf("%c || %s || %s\n", choise, cur->login, cur->passwd);
        switch(choise) {
            case 'u': {
                char *query = NULL;
                char *sql_pattern = NULL;
                t_list *list = NULL;
                sql_pattern = "SELECT EXISTS (SELECT id FROM users WHERE login=('%s'));";
                asprintf(&query, sql_pattern, cur->login);
                list = sqlite3_exec_db(query, 1);
                if (strcmp(list->data, "0") == 0) {
                    //REGESTRATION TO DB
                    sql_pattern = "INSERT INTO users (login, password) VALUES ('%s', '%s');";
                    asprintf(&query, sql_pattern, cur->login, cur->passwd);
                    sqlite3_exec_db(query, 2);
                    const bool success_reg = false;
                    SSL_write(cur->ssl, &success_reg, sizeof(bool));
                }
                else {
                    //LOGIN ALREDAY TAKEN
                    SSL_write(cur->ssl, &err_msg, sizeof(bool));
                }
                break;
            }
            case 'i': {
                char *query = NULL;
                char *sql_pattern = NULL;
                t_list *list = NULL;
                
                sql_pattern = "SELECT EXISTS (SELECT id FROM users WHERE login=('%s') AND password=('%s'));";
                asprintf(&query, sql_pattern, cur->login, cur->passwd);
                list = sqlite3_exec_db(query, 1);
                if (strcmp(list->data, "1") == 0) {
                    //USER FOUND
                    err_msg = false;
                    sql_pattern = "SELECT id FROM users WHERE login=('%s') AND password=('%s');";
                    asprintf(&query, sql_pattern, cur->login, cur->passwd);
                    list = sqlite3_exec_db(query, 1);
                }
                else {
                    //USER NOT FOUND
                    SSL_write(cur->ssl, &err_msg, sizeof(bool));
                }
                break;
            }
        }
    }
    
    // auth success

    switch (choise) {
        case 'u':
            cur->chat_count = 0;
            cur->chats = NULL;
            break;
        case 'i':
            //get_client_data(&cur);
            break;
    }
    
    cur->id = get_user_id(cur->login);
    SSL_write(cur->ssl, &err_msg, sizeof(bool));
    
    send_all_user_data(cur);
    sprintf(buff_out, "%s has joined with password %s\n", cur->login, cur->passwd);
    printf("%s", buff_out);
    sprintf(buff_out, "%s has joined\n", cur->login);
    send_message(buff_out,login, NULL, true);
    char message[MAX_LEN + NAME_LEN];
    while (is_run) {
        int mes_stat = SSL_read(cur->ssl, message, MAX_LEN + NAME_LEN);

        if (mes_stat == 0 || (mx_strcmp(message, "exit") == 0)) {
            sprintf(buff_out, "%s has left\n", cur->login);
			printf("%s", buff_out);
            is_run = false;
        }
        else if (mx_strcmp(message, "<users list>") == 0) {
            send_all(cur->ssl, "<users list>", 13);
            t_list *users_l = sqlite3_exec_db("SELECT login FROM users", 1);
            
            int users_count = mx_list_size(users_l);
            printf("users count %d\n", users_count);
            SSL_write(cur->ssl, &users_count, sizeof(int));
            printf("users count %d\n", users_count);
            while (users_l) {
                char buf[20] = {0};
                sprintf(buf, "%s", users_l->data);
                send_all(cur->ssl, buf, 20);
                users_l = users_l->next;
            }
            printf("users count %d\n", users_count);
            clear_message(message, MAX_LEN + NAME_LEN);
            continue;
        }
        else if (mx_strncmp(message,"<add chat, name=",16) == 0) {
            // работа со строкой, будет всё переделано под гтк
            printf("%s\n", message);
            char *temp = message + 16;
            int len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *name = mx_strndup(message + 16, len);
            if (mx_strcmp(name, ".new_dialog") == 0) {
                mx_strdel(&name);
                name = mx_strdup(".dialog");
            }
            char **arr = NULL;
            arr = mx_strsplit(mx_strchr(message, '>') + 1, ' ');
            t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
            new_chat->users=NULL;
            
            mx_strcpy(new_chat->name, name);
            
            int i;
            mx_push_back(&new_chat->users, mx_strdup(cur->login));
            for (i = 0; arr[i]; i++) {
                printf("arr %s\n", arr[i]);
                mx_push_back(&new_chat->users, mx_strdup(arr[i]));
            } 
            if (mx_strcmp(name, ".dialog") == 0) {
                name = mx_strrejoin(name, " ");
                name = mx_strrejoin(name, new_chat->users->next->data);
                name = mx_strrejoin(name, " ");
                name = mx_strrejoin(name, cur->login);
            }
            new_chat->messages = NULL;
            new_chat->count_users = i + 1;
            
            //ADD TO TABLES CHAT AND MEMBERS
            char *query = NULL;
            char *sql_pattern = NULL;
            sql_pattern = "INSERT INTO chats (name, members) VALUES ('%s', %d);";
            asprintf(&query, sql_pattern, name, new_chat->count_users);
            mx_strdel(&name);
            int *data = sqlite3_exec_db(query, 2);
            int c_id = data[0];
            new_chat->id = c_id;
            new_chat->is_new = true;
            t_list *temp_list = new_chat->users;
            int admin = 1;
            while (temp_list) {
                int u_id = get_user_id(temp_list->data);
                if (admin == 1) {
                    sql_pattern = "INSERT INTO members (chat_id, user_id, admin) VALUES (%d, %d, TRUE);";
                    admin = 0;
                }
                else {
                    sql_pattern = "INSERT INTO members (chat_id, user_id) VALUES (%d, %d);";
                }
                asprintf(&query, sql_pattern, c_id, u_id);
                sqlite3_exec_db(query, 2);
                temp_list = temp_list->next;
            }

            // отправка на клиенты
            pthread_mutex_lock(&send_mutex);
            send_new_chat(new_chat);

            mx_push_back(&cur->chats, new_chat);
            printf("added\n");
            pthread_mutex_unlock(&send_mutex);
            clear_message(message, MAX_LEN + NAME_LEN);
        }
        else if (mx_strncmp(message, "change chat", 11) == 0) {
            char *trim = message + 12;
            char *query = NULL;
            char *sql_pattern = NULL;
            t_list *list = NULL;
            sql_pattern = "SELECT EXISTS (SELECT name FROM chats WHERE id=('%d'));";
            asprintf(&query, sql_pattern, trim);
            list = sqlite3_exec_db(query, 1);
            if (strcmp(list->data, "1") == 0) {
                sql_pattern = "SELECT name FROM chats WHERE id=('%s');";
                asprintf(&query, sql_pattern, trim);
                list = sqlite3_exec_db(query, 1);
                char *c_name = mx_strdup(list->data);
                int c_id = get_chat_id(c_name);
                sql_pattern = "SELECT EXISTS (SELECT id FROM members WHERE chat_id=(%d) AND user_id=(%d));";
                asprintf(&query, sql_pattern, c_id, cur->id);
                list = sqlite3_exec_db(query, 1);
                if (strcmp(list->data, "1") == 0) {
                    mx_strcpy(cur->cur_chat.name, c_name);
                    cur->cur_chat.id = c_id;
                    cur->cur_chat.count_users = get_chat_members(cur->cur_chat.id);
                    cur->cur_chat.users = get_chat_users(cur->cur_chat.id);
                    
                }
                else {
                    //NOT CHAT MEMBER
                }
            }
            else {
                //NO SUCH CHAT
            }
        } 
        else if(mx_strncmp(message, "<setting avatar>", 16) == 0){
            char buf[512 + 32] = {0};
            t_avatar recv_avatar;
            recv_all(cur->ssl, buf, 512 + 32);
            recv_avatar.name = mx_strdup(buf);
            clear_message(buf, 512 + 32);
            char *path = mx_strjoin("data/avatars/", cur->login);
            sprintf(buf, "data/avatars/%s/%s", cur->login, recv_avatar.name);
            printf("buf %s\n", buf);
            struct stat st = {0};
            if (stat(path, &st) == -1) {
                mkdir(path, 0777);
            }
            mx_strdel(&path);
            printf("buf %s\n", buf);
            recv_avatar.path = mx_strdup(buf);
            clear_message(buf, 544);
            recv_image(cur->ssl, recv_avatar.path);
        
            sprintf(buf, "<image loaded>");
            send_all(cur->ssl, buf, 512 + 32); 
            clear_message(buf, 512 + 32);

            
            SSL_read(cur->ssl, &recv_avatar.scaled_w, sizeof(double));
            SSL_read(cur->ssl, &recv_avatar.scaled_h, sizeof(double));

            SSL_read(cur->ssl, &recv_avatar.x, sizeof(double));
            SSL_read(cur->ssl, &recv_avatar.y, sizeof(double));

            clear_message(buf, 512 + 32);
            sprintf(buf, "<setting avatar>");
            send_all(cur->ssl, buf, 512+32);
            send_image(cur->ssl, recv_avatar.path);

            sprintf(buf, "path=%s scaled_w=%f scaled_h=%f x=%f y=%f ", recv_avatar.path,recv_avatar.scaled_w, recv_avatar.scaled_h,recv_avatar.x, recv_avatar.y);
            printf("buf %s\n", buf);
            char *query = NULL;
            char *sql_pattern = NULL;
            sql_pattern = "UPDATE users SET avatar = '%s' WHERE id = %d;";
            asprintf(&query, sql_pattern, buf, cur->id);
            sqlite3_exec_db(query, 2);

        }
        else if (mx_strncmp(mx_strtrim(message), "<setting, name=", 15) == 0) {
            char *name = NULL;
            char *surname = NULL;
            char *bio = NULL;
        
            char *temp = mx_strstr(message, "name=") + 5;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }

            name = mx_strndup(temp, len);
            temp = mx_strstr(message, "surname=") + 8;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            surname = mx_strndup(temp, len);

            bio = mx_strdup(mx_strchr(message, '>')+1);

            if (mx_strcmp(name, ".not_changed") != 0) {
                mx_strcpy(cur->name, name);
                
                char *query = NULL;
                char *sql_pattern = NULL;
                sql_pattern = "UPDATE users SET name = '%s' WHERE id = %d;";
                asprintf(&query, sql_pattern, cur->name, cur->id);
                sqlite3_exec_db(query, 2);

                // изменяешь name
            }
            if (mx_strcmp(surname, ".not_changed") != 0) {
                mx_strcpy(cur->surname, surname);

                char *query = NULL;
                char *sql_pattern = NULL;
                sql_pattern = "UPDATE users SET surname = '%s' WHERE id = %d;";
                asprintf(&query, sql_pattern, cur->surname, cur->id);
                sqlite3_exec_db(query, 2);

                // изменяешь surname
            }
            if (mx_strcmp(bio, ".not_changed") != 0) {
                mx_strcpy(cur->bio, bio);
                printf("receive bio: %s\n", bio);
                char *query = NULL;
                char *sql_pattern = NULL;
                sql_pattern = "UPDATE users SET bio = '%s' WHERE id = %d;";
                asprintf(&query, sql_pattern, cur->bio, cur->id);
                sqlite3_exec_db(query, 2);

                // изменяешь bio
            }
        }
        else if (mx_strncmp(message, "<msg, chat_id=", 14) == 0) {
            printf("%s\n", message);
            char *temp = message + 15;
            int len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            printf("%s\n", c_id);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                printf("change chat\n");
                change_chat_by_id(chat_id, cur);
            }

            char *query = NULL;
            char *sql_pattern = NULL;
            char *mess = sql_protection(mx_strchr(message, '>') + 1);
            sql_pattern = "INSERT INTO messages (chat_id, user_id, text) VALUES (%d, %d, '%s');";
            asprintf(&query, sql_pattern, cur->cur_chat.id, cur->id, mess);
            int *mes_id = sqlite3_exec_db(query, 2);
            cur->cur_chat.last_mes_id = *mes_id;
            send_message(mx_strchr(message, '>') + 1, cur->login, &cur->cur_chat, true);
            clear_message(message, MAX_LEN + NAME_LEN);
        }
        else if(mx_strncmp(message, "<file chat_id=", 14) == 0) { //"<file chat_id=%d, name=%s, mode=%s>"
            printf("%s\n", message);
            char *temp = message + 14;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            printf("%s\n", c_id);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                printf("change chat\n");
                change_chat_by_id(chat_id, cur);
            }

            temp = mx_strstr(temp, "name=") + 5;
            len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *name = mx_strndup(temp, len);
            char *path = mx_strjoin("data/", mx_itoa(chat_id));
            struct stat st = {0};
            if (stat(path, &st) == -1) {
                mkdir(path, 0777);
            }
            path = mx_strrejoin(path, "/");
            path = mx_strrejoin(path, name);           
            temp = mx_strstr(temp, "mode=") + 5;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }

            char *mode = mx_strndup(temp, len);
            //printf("name %s\npath %s\nmode %s\n", name, path, mode);
            char *query = NULL;
            char *sql_pattern = NULL;
            sql_pattern = "INSERT INTO messages (chat_id, user_id, text, type) VALUES (%d, %d, '%s', '%s');";
            asprintf(&query, sql_pattern, cur->cur_chat.id, cur->id, path, "file");
            int *mes_id = sqlite3_exec_db(query, 2);
            
            cur->cur_chat.last_mes_id = *mes_id;
            recv_file(cur->ssl, path, mode);

            char buf[544] = {0};
            sprintf(buf, "<file chat_id=%d, mes_id=%d, from=%s, prev=0>%s", chat_id, *mes_id, cur->login, name);

            send_message(buf, cur->login, &cur->cur_chat, false);
        }
        else if (mx_strncmp(message, "<chat users avatars>", 20) == 0){
            printf("%s\n", message);
            char *id_str = message + 20;
            int chat_id = mx_atoi(id_str);
            printf ("%s ,chat id %d\n", id_str, chat_id);
            t_list *users = get_chat_users(chat_id);

            char buf[512+32] = {0};
            sprintf(buf, "<chat users avatars>");
            send_all(cur->ssl,buf, 512+32);
            while (users) {
                if (mx_strcmp(users->data, cur->login) != 0) {
                    printf("%s %s\n", users->data, cur->login);
                    char *avatar_info = get_user_avatar(get_user_id(users->data));
                    t_avatar *avatar = parse_avatar_info(avatar_info);
                    
                    char *temp_str = avatar->path;
                    while (mx_strchr(temp_str,'/')) {
                        temp_str = mx_strchr(temp_str,'/') + 1;
                    }
                    char buf_name[32] = {0};
                    sprintf(buf_name, "%s", temp_str);
                    send_all(cur->ssl, buf_name, 32);
                    clear_message(buf_name, 32);
                    if (mx_strcmp(avatar->path, "default") != 0){
                        send_image(cur->ssl, avatar->path);
                        recv_all(cur->ssl, buf_name, 14);
                        SSL_write(cur->ssl, &avatar->scaled_w, sizeof(double));
                        SSL_write(cur->ssl, &avatar->scaled_h, sizeof(double));
                        SSL_write(cur->ssl, &avatar->x, sizeof(double));
                        SSL_write(cur->ssl, &avatar->y, sizeof(double));
                    }
                }

                users = users->next;
            }
            
        }
        else if (mx_strncmp(message, "<get messages chat_id=", 22) == 0) { //"<get messages chat_id=%d, last_mes=%d>"
            char *temp = message + 22;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            printf("c_id %s\n", c_id);
            mx_strdel(&c_id);

            temp = mx_strstr(message, "last_mes=") + 9;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *m_id = mx_strndup(temp, len);
            int mes_id = mx_atoi(m_id);
            printf("m_id %s\n", m_id);
            mx_strdel(&m_id);

            t_list *mes_list = db_messages_sender(chat_id, mes_id); //DODELAI
            printf("chat_id %d\n", chat_id);
            char buf[512 + 32] = {0};
            while(mes_list) {
                t_message *mes_send = (t_message *)mes_list->data;
                sprintf(buf, "<msg, chat_id=%d, mes_id=%d, from=%s, prev=1>%s", chat_id, mes_send->id, mes_send->sender, mes_send->data);

                send_all(cur->ssl, buf, 512 + 32);

                clear_message(buf, 544);
                int status = 0;
                SSL_read(cur->ssl, &status, sizeof(int));
                if (!status)
                    break;
                mes_list = mes_list->next;
            }

            sprintf(buf, "<last message>");
            send_all(cur->ssl, buf, 512 + 32);
            //send (cur->cl_socket, &count_mes, sizeof(int), 0);
        }
        else if(mx_strncmp(message, "<get user avatar>", 17) == 0) { //<get user avatar>
            char *user_name = message + 17;
            printf("get user avatar %s\n", user_name);
            char *avatar_info = get_user_avatar(get_user_id(user_name));
            t_avatar *avatar = parse_avatar_info(avatar_info);
            char buf[544] = {0};
            sprintf(buf, "<get user avatar>");
            send_all(cur->ssl, buf, 544);
            send_avatar(avatar, cur->ssl);
        }
        else if (mx_strncmp(message, "<delete mes chat_id=", 20) == 0) {
            printf("%s\n", message);
            char *temp = message + 20;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            printf("%s\n", c_id);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                printf("change chat\n");
                change_chat_by_id(chat_id, cur);
            }
            temp = mx_strstr(temp, "mes_id=") + 7;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *m_id = mx_strndup(temp, len);
            printf("%s\n", m_id);
            int mes_id = mx_atoi(m_id);
            mx_strdel(&m_id);
            
            db_delete_message(mes_id);

            send_message(message, cur->login, &cur->cur_chat, false);
        }
        else if (mx_strncmp(message, "<edit msg, chat_id=", 19) == 0) { // "<edit msg, chat_id=%d, mes_id=%d>%s"
            char *temp = message + 19;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            printf("%s\n", c_id);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                printf("change chat\n");
                change_chat_by_id(chat_id, cur);
            }
            temp = mx_strstr(temp, "mes_id=") + 7;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *m_id = mx_strndup(temp, len);
            printf("%s\n", m_id);
            int mes_id = mx_atoi(m_id);
            mx_strdel(&m_id);
            
            message_changer(mes_id, mx_strchr(message, '>') + 1);

            send_message(message, cur->login, &cur->cur_chat, false);
        }
        else if (mx_strncmp(message, "<get last mes id>", 17) == 0) { // "<get last mes id>"
            send_all(cur->ssl, message, 544);
            int mes_id = get_message_max_id(cur->id, cur->cur_chat.id);
            SSL_write(cur->ssl, &mes_id, sizeof(int));
        }
        else if (mes_stat > 0) {
            printf("Message Received from %s | %s |\n", login, message);
		    if(cur->cur_chat.id != 0){
                send_message(message, cur->login, &cur->cur_chat, true);
            }
            else {
                send_message(message, cur->login, NULL, true);
            }
            clear_message(message, MAX_LEN + NAME_LEN);
        }
        
    }

    // отключение клиента от сервера
    close(cur->cl_socket);
    free_client(&cur, &users_list);
    return NULL;
}

t_client *create_new_client(const struct sockaddr_in adr, int client_fd, int *client_id) {
    t_client *new_client = (t_client *)malloc(sizeof(t_client));
    new_client->adr = adr;
    new_client->cl_socket = client_fd;
    new_client->login = NULL;
    new_client->passwd = NULL;
    //new_client->id = client_id;
    new_client->scaled_for_chat = NULL;
    new_client->chats = NULL;
    t_chat init_chat = {0};
    new_client->cur_chat = init_chat;

    printf("id %d\n", *client_id);
    (*client_id)++;
    mx_push_back(&users_list, new_client);
    return new_client;
}

void client_work_wrapper(SSL_CTX *context, int client_fd, pthread_t *thread, t_client *new_client) {
    // threadin
    SSL *ssl;
    ssl = SSL_new(context);             //get new SSL state
    SSL_set_fd(ssl, client_fd);         //set connection socket to SSL state

    if (SSL_accept(ssl) < 0) {
        ERR_print_errors_fp(stderr);
        close_connection(ssl);
        return;
    }

    new_client->ssl = ssl;

    // Certificate check (delete? anyway client has no certs)
    //certificate_ckeck(ssl);

    // nu thread
    pthread_create(thread, NULL, client_work, new_client);
}

int main(int argc, char *argv[]) {
    printf("%s\n", SSLeay_version(SSLEAY_VERSION));
    
    signal(SIGPIPE, SIG_IGN);
    if (argc != 2) {
        mx_printerr("usage: ./uchat_server <port>\n");
        return -1;
    }

    char *weather = get_weather("Kharkov");
    printf("%s\n", weather);

    //  SSLing
    SSL_CTX *context;
    SSL_library_init();
    context = CTX_initialize_server();
    printf("SSL: SSL initialized\n");

    // creaating certificate and pkey
    EVP_PKEY *pkey = create_key();
    printf("SSL: key generated\n");
    X509 *x509 = create_X509(pkey);
    printf("SSL: X509 generated\n");
    write_certs(pkey, x509);
    printf("SSL: certificates saved\n");
    load_certs(context, "certificate.pem", "PEM_privatekey.pem");
    printf("SSL: certificates loaded\n");

    // opening connection
    struct sockaddr_in adr = {0};
    socklen_t adrlen = sizeof(adr);
    int serv_fd = open_server_connection(mx_atoi(argv[1]), &adr, adrlen);
    printf("SSL: connection opened\n");

    // for client serving
    pthread_t thread;
    users_list = NULL;
    pthread_mutex_init(&send_mutex, NULL);

    //char *weather = get_weather("Kharkov");
    //char *facts = get_facts();
    //printf("%s\n", facts);
    int client_id = 0;
    int client_fd;

    while (1) {
        // accept client connection
        client_fd = accept(serv_fd, (struct sockaddr*) &adr, &adrlen);
        printf("SSL: client accepted\n");
        printf("SSL: Connection: %s:%d\n",inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));

        // handle client
        t_client *new_client = create_new_client(adr, client_fd, &client_id);
        client_work_wrapper(context, client_fd, &thread, new_client);
        sleep(1);
    }

    close(client_fd);
    close_server(pkey, x509, context);
    //  SSLing

    return 0;
}

char *sql_protection(char *message) {
    int problem = 0;
    for(int i = 0; message[i]; i++) {
        if (message[i] == '\'') {
            problem++;
        }
    }
    char *temp = mx_strnew(mx_strlen(message) + problem);
    int i = 0;
    int j = 0;
    while (message[i]) {
        if (message[i] =='\'' && message[i-1] !='\'') {
            temp[j] = message[i];
            temp[j+1] = message[i];
            j++;
        }
        else {
            temp[j] = message[i];
        }
        j++;
        i++;
    }
    return temp;
}
