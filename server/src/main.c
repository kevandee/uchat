#include "../inc/uch_server.h"
#define MAX_LEN 512
#define NAME_LEN 32

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
            cur->login=mx_strdup(login);
        }

        //passwd
        if(recv(cur->cl_socket, passwd, 16, 0) <= 0 || mx_strlen(passwd) <  8 || mx_strlen(passwd) > 16){
            printf("Didn't enter the password.\n");
            free_client(&cur, &users_list);

            
            return NULL;

        } else{
            cur->passwd=mx_strdup(passwd);
        }

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
                    err_msg = false;
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
    
    send(cur->cl_socket, &err_msg,sizeof(bool), 0);
    sprintf(buff_out, "%s has joined with password %s\n", cur->login, cur->passwd);
    printf("%s", buff_out);
    sprintf(buff_out, "%s has joined\n", cur->login);
    //printf("%s", buff_out);
    send_message(buff_out,login);
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
        else if (mes_stat > 0) {
            printf("Message Received from %s\n", login);
		    send_message(message, login);

            clear_message(message, MAX_LEN + NAME_LEN);
        }
        
    }

    // отключение клиента от сервера
    close(cur->cl_socket);
    free_client(&cur, &users_list);
    return NULL;
}


int main(int argc, char *argv[]) {
    printf("%s\n", sqlite3_libversion());
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

    char *weather = get_weather("Kharkov");
    printf("%s\n", weather);
    int client_id = 0;
    while(1) {
        client_fd = accept(serv_fd, (struct sockaddr *) &adr, &adrlen);
        
        t_client *new_client = (t_client *)malloc(sizeof(t_client));
        new_client->adr = adr;
        new_client->cl_socket = client_fd;
        new_client->login = NULL;
        new_client->passwd = NULL;
        new_client->id = client_id;
        printf("id %d\n", client_id);
        
        client_id++;
        mx_push_back(&users_list, new_client);

        pthread_create(&thread, NULL, client_work, (void *)new_client);
        sleep(1);
    }

    close(client_fd);
    close(serv_fd);
    return 0;
}
