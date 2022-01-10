#include "../inc/uch_server.h"
#define MAX_LEN 512
#define NAME_LEN 32

t_list *users_list;

void *client_work(void *param) {
    char *err;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    sqlite3_open("my.db", &db);
    int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT NOT NULL, password TEXT NOT NULL);", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        mx_printerr("db creation error");
        mx_printerr("\n");
        sqlite3_close(db);
    }
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

        switch(choise) {
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
    close(cur->cl_socket);
    mx_strdel(&cur->login);
    /*printf("1\n");
    t_list *del = users_list;
    for (int i = 0; i < cur->id - 1; i++) {
        del = del->next;
    }
    printf("2\n");*/
    free(cur);
    cur = NULL;

    /*t_list *next = del->next->next;
    free(del->next);
    del->next = next;
    printf("3\n");
    del = del->next;
    while (del) {
        ((t_client *)del->data)->id--;
        del = del->next;
    }
    printf("4\n");
    del = users_list;
    while(del) {
        printf("id %d\n",  ((t_client *)del->data)->id);
        del = del->next;
    }*/
    pthread_detach(pthread_self());
    return NULL;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

char *get_weather(char *city) {
    FILE *fp = fopen("weather.txt","wb");
    CURL *curl;
    (void)city;
  CURLcode res;
    
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://wttr.in/Kharkov");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.47.1");
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);

  }
  fclose(fp);
  return mx_file_to_str("weather.txt");
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

        mx_push_back(&users_list, new_client);

        pthread_create(&thread, NULL, client_work, (void *)new_client);
        sleep(1);
    }

    close(client_fd);
    close(serv_fd);
    return 0;
}
