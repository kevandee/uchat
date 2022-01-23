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
    printf("1\n");

    while (err_msg) {
        // sign up or sign in
        if(recv(cur->cl_socket, &choise, 1, 0) <= 0) {
            free_client(&cur, &users_list);
            
            return NULL;
        }
        printf("choise %c\n", choise);
        // login
        if(recv(cur->cl_socket, login, NAME_LEN, 0) <= 0 || mx_strlen(login) <  2 || mx_strlen(login) >= 32){
            printf("Didn't enter the name.\n");
            free_client(&cur, &users_list);

            return NULL;
        } else{
            cur->login=mx_strtrim(login);
        }

        //passwd
        if(recv(cur->cl_socket, passwd, 16, 0) <= 0 || mx_strlen(passwd) <  8 || mx_strlen(passwd) > 16){
            printf("Didn't enter the password.\n");
            free_client(&cur, &users_list);

            
            return NULL;

        } else{
            cur->passwd=mx_strtrim(passwd);
        }
        //recv_jpeg(cur->cl_socket, "received.jpg");
        //DB SWITH
        printf("%c || %s || %s\n", choise, cur->login, cur->passwd);
        switch(choise) {
            case 'u': {
                char *query = NULL;
                char *sql_pattern = NULL;
                t_list *list = NULL;
                sql_pattern = "SELECT EXISTS (SELECT id FROM users WHERE login=('%s'));";
                // 1 = nachol
                // 0 = ne nachol
                asprintf(&query, sql_pattern, cur->login);
                list = sqlite3_exec_db(query, 1);
                printf("list head = '%s' \n", list->data);
                if (strcmp(list->data, "0") == 0) {
                    //REGESTRATION TO DB
                    sql_pattern = "INSERT INTO users (login, password) VALUES ('%s', '%s');";
                    asprintf(&query, sql_pattern, cur->login, cur->passwd);
                    sqlite3_exec_db(query, 2);
                    const bool success_reg = false;
                    send(cur->cl_socket, &success_reg, sizeof(bool), 0);
                }
                else {
                    //LOGIN ALREDAY TAKEN
                    send(cur->cl_socket, &err_msg, sizeof(bool), 0);
                }
                /*mx_clear_list(&list);
                mx_strdel(&query);
                mx_strdel(&sql_pattern);*/
                break;
            }
            case 'i': {
                char *query = NULL;
                char *sql_pattern = NULL;
                t_list *list = NULL;
                
                sql_pattern = "SELECT EXISTS (SELECT id FROM users WHERE login=('%s') AND password=('%s'));";
                // 1 = nachol
                // 0 = ne nachol
                asprintf(&query, sql_pattern, cur->login, cur->passwd);
                list = sqlite3_exec_db(query, 1);
                printf("list head = '%s' \n", list->data);
                if (strcmp(list->data, "1") == 0) {
                    //USER FOUND
                    err_msg = false;

                    sql_pattern = "SELECT id FROM users WHERE login=('%s') AND password=('%s');";
                    asprintf(&query, sql_pattern, cur->login, cur->passwd);
                    list = sqlite3_exec_db(query, 1);
                    cur->id = mx_atoi(list->data);
                }
                else {
                    //USER NOT FOUND
                    send(cur->cl_socket, &err_msg, sizeof(bool), 0);
                }
                /*mx_clear_list(&list);
                mx_strdel(&query);
                mx_strdel(&sql_pattern);*/
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
    
    send(cur->cl_socket, &err_msg,sizeof(bool), 0);
    
    send_all_user_data(cur);
    sprintf(buff_out, "%s has joined with password %s\n", cur->login, cur->passwd);
    printf("%s", buff_out);
    sprintf(buff_out, "%s has joined\n", cur->login);
    //printf("%s", buff_out);
    send_message(buff_out,login, NULL);
    char message[MAX_LEN + NAME_LEN];
    while (is_run) {
        int mes_stat = recv(cur->cl_socket, message, MAX_LEN + NAME_LEN, 0);

        if (mes_stat == 0 || (mx_strcmp(message, "exit") == 0)) {
            sprintf(buff_out, "%s has left\n", cur->login);
			printf("%s", buff_out);
            is_run = false;
        }
        else if (mx_strcmp(message, "<users list>") == 0) {
            send_all(cur->cl_socket, "<users list>", 13);
            t_list *users_l = sqlite3_exec_db("SELECT login FROM users", 1);
            
            int users_count = mx_list_size(users_l);
            printf("users count %d\n", users_count);
            send(cur->cl_socket, &users_count, sizeof(int), 0);
            while (users_l) {
                char buf[20] = {0};
                sprintf(buf, "%s", users_l->data);
                send_all(cur->cl_socket, buf, 20);
                users_l = users_l->next;
            }
            clear_message(message, MAX_LEN + NAME_LEN);
            continue;
        }
        else if (mx_strncmp(message,"<add chat>",10) == 0) {
            // работа со строкой, будет всё переделано под гтк
            char *trim = message + 10;

            char **arr = NULL;
            arr = mx_strsplit(trim, ' ');
            t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
            new_chat->users=NULL;
            
            mx_strcpy(new_chat->name, arr[0]);
            int i;
            mx_push_back(&new_chat->users, mx_strdup(cur->login));
            for (i = 1; arr[i]; i++) {
                printf("arr %s\n", arr[i]);
                mx_push_back(&new_chat->users, mx_strdup(arr[i]));
            } 
            new_chat->messages = NULL;
            new_chat->count_users = i;
            
            //ADD TO TABLES CHAT AND MEMBERS
            char *query = NULL;
            char *sql_pattern = NULL;
            sql_pattern = "INSERT INTO chats (name, members) VALUES ('%s', %d);";
            asprintf(&query, sql_pattern, new_chat->name, new_chat->count_users);
            int *data = sqlite3_exec_db(query, 2);
            int c_id = data[0];
            new_chat->id = c_id;
            t_list *temp_list = new_chat->users;
            int admin = 1;
            while (temp_list) {
                int u_id = get_user_id ((char *)temp_list->data);
                //printf("User: %s\n", temp_list->data);
                //printf("User id: %s\n", u_id->data);
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
            /*
            Дим, а тут нужно поиск данных по чату достать из бд и заполнить структуру,
            пока что тут идёт поиск по линкед листу, но надо из бд всё взять
            */
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
                int u_id = get_user_id(cur->login);
                sql_pattern = "SELECT EXISTS (SELECT id FROM members WHERE chat_id=(%d) AND user_id=(%d));";
                asprintf(&query, sql_pattern, c_id, u_id);
                list = sqlite3_exec_db(query, 1);
                printf("check %s\n", list->data);
                if (strcmp(list->data, "1") == 0) {
                    //
                    mx_strcpy(cur->cur_chat.name, c_name);
                    cur->cur_chat.id = c_id;
                    cur->cur_chat.count_users = get_chat_members(cur->cur_chat.id);
                    cur->cur_chat.users = get_chat_users(cur->cur_chat.id);
                    //
                    printf("cur name: %s\n", cur->cur_chat.name);
                    printf("1 user: %s\n", cur->cur_chat.users->data);
                    printf("2 user: %s\n", cur->cur_chat.users->next->data);
                }
                else {
                    //NOT CHAT MEMBER
                }
            }
            else {
                //NO SUCH CHAT
            }
        } 
        else if (mx_strncmp(message, "<msg, chat_id=", 13) == 0) {
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

            send_message(mx_strchr(message, '>') + 1, cur->login, &cur->cur_chat);
            clear_message(message, MAX_LEN + NAME_LEN);
        }
        else if (mes_stat > 0) {
            printf("Message Received from %s | %s |\n", login, message);
		    if(cur->cur_chat.id != 0){
                send_message(message, cur->login, &cur->cur_chat);
            }
            else {
                send_message(message, cur->login, NULL);
            }
            clear_message(message, MAX_LEN + NAME_LEN);
        }
        
    }

    // отключение клиента от сервера
    close(cur->cl_socket);
    free_client(&cur, &users_list);
    return NULL;
}


int main(int argc, char *argv[]) {
    printf("%s\n", SSLeay_version(SSLEAY_VERSION));
    
    signal(SIGPIPE, SIG_IGN);
    if (argc != 2) {
        mx_printerr("usage: ./uchat_server <port>\n");
        return -1;
    }
    
    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(mx_atoi(argv[1]));
    if (bind(serv_fd, (struct sockaddr *) &adr, sizeof(struct sockaddr_in)) < 0) {
        perror("ERROR: Socket binding failed");
        return EXIT_FAILURE;
    }

    if (listen(serv_fd, 5) < 0) {
        perror("ERROR: Socket listening failed");
        return EXIT_FAILURE;
	}

    socklen_t adrlen = sizeof(struct sockaddr_in);
    int client_fd;
    pthread_t thread;
    users_list = NULL;
    pthread_mutex_init(&send_mutex, NULL);

    //char *weather = get_weather("Kharkov");
    //printf("%s\n", weather);
    int client_id = 0;
    while(1) {
        client_fd = accept(serv_fd, (struct sockaddr *) &adr, &adrlen);
        
        t_client *new_client = (t_client *)malloc(sizeof(t_client));
        new_client->adr = adr;
        new_client->cl_socket = client_fd;
        new_client->login = NULL;
        new_client->passwd = NULL;
        //new_client->id = client_id;
        new_client->chats = NULL;
        t_chat init_chat = {0};
        new_client->cur_chat = init_chat;
    
        printf("id %d\n", client_id);
        
        client_id++;
        mx_push_back(&users_list, new_client);

        pthread_create(&thread, NULL, client_work, new_client);
        sleep(1);
    }

    close(client_fd);
    close(serv_fd);
    return 0;
}
