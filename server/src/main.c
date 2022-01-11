#include "../inc/uch_server.h"
#define MAX_LEN 512
#define NAME_LEN 32

t_list *users_list;

void *client_work(void *param) {
    //char *err;
    //sqlite3 *db;
    //sqlite3_stmt *stmt;
    //sqlite3_config();
    //sqlite3_open("my.db", &db);
    //int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT NOT NULL, password TEXT NOT NULL);", NULL, NULL, &err);
    /*if (rc != SQLITE_OK) {
        mx_printerr("db creation error");
        mx_printerr("\n");
        sqlite3_close(db);
    }*/
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
    pthread_detach(pthread_self());
            
            return NULL;
        }
        printf("choise %c\n", choise);
        // login
        if(recv(cur->cl_socket, login, NAME_LEN, 0) <= 0 || mx_strlen(login) <  2 || mx_strlen(login) >= 32){
            printf("Didn't enter the name.\n");
            pthread_detach(pthread_self());

            return NULL;
        } else{
            cur->login=mx_strdup(login);
        }

        //passwd
        if(recv(cur->cl_socket, passwd, 16, 0) <= 0 || mx_strlen(passwd) <  8 || mx_strlen(passwd) > 16){
            printf("Didn't enter the password.\n");
            pthread_detach(pthread_self());
            
            return NULL;

        } else{
            cur->passwd=mx_strdup(passwd);
        }

        // Дима, вот тут твоя работа с бд. Проверил или добавил по бд и нужно ответ на клиент сделать
        printf("%c || %s || %s\n", choise, login, passwd);
        /*switch(choise) {
            case 'u':
                rc = sqlite3_prepare_v2(db, "SELECT name FROM users WHERE name = ?", -1, &stmt, NULL);
                if (rc != SQLITE_OK) {
                    mx_printerr("db selection error");
                    mx_printerr("\n");
                    sqlite3_close(db);
                }
                rc = sqlite3_bind_text(stmt, 1, cur->login, mx_strlen(cur->login), NULL);
                if (rc != SQLITE_OK) {
                    mx_printerr("db binding error");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
                    sqlite3_close(db);
                }
                rc = sqlite3_step(stmt);
                if (rc == SQLITE_DONE) {
                    printf("db evolution error => this name not finded. GOOD!!!\n");
                    //REGISTRATION
                    char* query = sqlite3_mprintf("INSERT INTO users (name, password) VALUES ('%q', '%q');", cur->login, cur->passwd);
                    rc = sqlite3_exec(db, query, NULL, NULL, &err);
                    if (rc != SQLITE_OK) {
                        mx_printerr("err: ");
                        //mx_printerr(rc);
                    }
                    err_msg = false;
                }
                else {
                    //NAME ALREADY TAKEN
                    send(cur->cl_socket, &err_msg,sizeof(bool), 0);
                }
                break;
            case 'i':
                rc = sqlite3_prepare_v2(db, "SELECT * FROM users WHERE name = ? AND password = ?", -1, &stmt, NULL);
                if (rc != SQLITE_OK) {
                    mx_printerr("db selection error");
                    mx_printerr("\n");
                    sqlite3_close(db);
                }
                rc = sqlite3_bind_text(stmt, 1, cur->login, mx_strlen(cur->login), NULL);
                if (rc != SQLITE_OK) {
                    mx_printerr("db binding error1");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
                    sqlite3_close(db);
                }
                rc = sqlite3_bind_text(stmt, 2, cur->passwd, mx_strlen(cur->passwd), NULL);
                if (rc != SQLITE_OK) {
                    mx_printerr("db binding error2");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
                    sqlite3_close(db);
                }
                printf("Login = %s\n", cur->login);
                printf("Password = %s\n", cur->passwd);
                rc = sqlite3_step(stmt);
                printf("rc = %i\n", rc);
                if (rc != SQLITE_DONE) {
                    printf("db evolution error => this user finded. GOOD!!!\n");
                    err_msg = false;
                }
                else {
                    //USER NOT FOUND
                    send(cur->cl_socket, &err_msg,sizeof(bool), 0);
                }
                break;
        }
    */
   err_msg = false;
    }
    //sqlite3_close(db);
    
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
    mx_strdel(&cur->login);
    t_list *del = users_list;
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
        mx_pop_front(&users_list);
        t_list *temp = users_list;
        while (temp) {
            ((t_client *)temp->data)->id--;
            temp = temp->next;
        }
        free(cur);
        cur = NULL;
    }
    else if (!del->next) {
        //printf("back\n");
        mx_pop_back(&users_list);
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
