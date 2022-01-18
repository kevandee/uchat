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
        printf("%c || %s || %s\n", choise, login, passwd);
        switch(choise) {
            case 'u': {
                char *query = NULL;
                char *sql_pattern = NULL;
                t_list *list = NULL;
                sql_pattern = "SELECT EXISTS (SELECT id FROM users WHERE name=('%s'));";
                // 1 = nachol
                // 0 = ne nachol
                asprintf(&query, sql_pattern, cur->login);
                list = sqlite3_exec_db(query, 1);
                printf("list head = '%s' \n", list->data);
                if (strcmp(list->data, "0") == 0) {
                    //REGESTRATION TO DB
                    sql_pattern = "INSERT INTO users (name, password) VALUES ('%s', '%s');";
                    asprintf(&query, sql_pattern, cur->login, cur->passwd);
                    sqlite3_exec_db(query, 2);
                    const bool success_reg = false;
                    send(cur->cl_socket, &success_reg, sizeof(bool), 0);
                }
                else {
                    //NAME ALREDAY TAKEN
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
                sql_pattern = "SELECT EXISTS (SELECT id FROM users WHERE name=('%s') AND password=('%s'));";
                // 1 = nachol
                // 0 = ne nachol
                asprintf(&query, sql_pattern, cur->login, cur->passwd);
                list = sqlite3_exec_db(query, 1);
                printf("list head = '%s' \n", list->data);
                if (strcmp(list->data, "1") == 0) {
                    //USER FOUND
                    err_msg = false;
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
        else if (mx_strcmp(message, "users") == 0) {
            char *table = NULL;
            t_list *users_tmp = users_list;
            int i = 1;
            while (users_tmp) {
                t_client *cl_tmp = (t_client *)(users_tmp->data);
                if (cl_tmp->login) {
                    char *num = mx_itoa(i);
                    table = mx_strjoin(table, num);
                    mx_strdel(&num);
                    table = mx_strjoin(table, ". ");
                    table = mx_strjoin(table, cl_tmp->login);
                    table = mx_strjoin(table, "\n");
                    i++;
                }

                users_tmp = users_tmp->next;
            }
            send(cur->cl_socket, table, mx_strlen(table), 0);
            clear_message(message, MAX_LEN + NAME_LEN);
            continue;
        }
        else if (mx_strncmp(message,"add chat",8) == 0) {
            // работа со строкой, будет всё переделано под гтк
            char *trim = message + 9;

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
            printf("c_id: %i\n", c_id);
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
            send_new_chat(&new_chat);

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
            t_list *temp_chat_l = cur->chats;
            char *find_chat = NULL;
            while(temp_chat_l) {
                if (mx_strcmp(trim, ((t_chat *)(temp_chat_l->data))->name ) == 0) {
                    find_chat = ((t_chat *)(temp_chat_l->data))->name;
                    break;
                }
                temp_chat_l = temp_chat_l->next;
            }
            if (temp_chat_l) {
                cur->cur_chat = ((t_chat *)(temp_chat_l->data));
                printf("cur name: %s\n", cur->cur_chat->name);
                printf("1 user: %s\n", cur->cur_chat->users->data);
                printf("2 user: %s\n", cur->cur_chat->users->next->data);
            }
        }
        else if (mes_stat > 0) {
            printf("Message Received from %s | %s |\n", login, message);
		    if(cur->cur_chat){
                send_message(message, cur->login, cur->cur_chat);
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
        new_client->id = client_id;
        new_client->chats = NULL;
        new_client->cur_chat = NULL;

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
