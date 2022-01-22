#include "../inc/uch_client.h"

t_client cur_client;
pthread_mutex_t cl_mutex;

void *sender_func(void *param) {
    (void)param;
    char *message = (char *)malloc(512);
    char buf[512 + 32];
    fseek(stdin,0,SEEK_END);
    while(1) {
        printf("> ");
        fflush(stdout);
        size_t len;
        getline(&message, &len, stdin);
        message = mx_strtrim(message);
        if (mx_strcmp(message, "users") == 0) {
            send(cur_client.serv_fd, message, mx_strlen(message) - 1, 0);
        }
        else if (mx_strcmp(message, "exit") == 0) {
            send(cur_client.serv_fd, message, 4, 0);
            break;
        }
        else {
            send(cur_client.serv_fd, message, mx_strlen(message), 0);
        }
        clear_message(message, 512);
        clear_message(buf, 512 + 32);

    }
    return NULL;
}

void *rec_func(void *param) {
    while(!cur_client.login) {
        sleep(1);
    }
    int fd = cur_client.serv_fd;
    (void)param;
    char message[512 + 32] = {0};
    while (1) {
		int receive = recv(fd, message, 512 + 32, 0);

        if (receive > 0) {
            printf("|%s|\n", message);
            if(mx_strcmp(mx_strtrim(message), "<add chat>") == 0) {
                t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
                new_chat->messages = NULL;
                new_chat->users = NULL;

                char buf_name[256] = {0};
                receive = recv_all(fd, buf_name, 256);
                while (receive < 0) {
                    receive = recv_all(fd, buf_name, 256);
                }
                mx_strcpy(new_chat->name, buf_name);
                receive = recv(fd, &new_chat->id, sizeof(int), 0);
                while (receive < 0) {
                    receive = recv(fd, &new_chat->id, sizeof(int), 0);
                }
                receive = recv(fd, &new_chat->count_users, sizeof(int), 0);
                while (receive < 0) {
                    receive = recv(fd, &new_chat->count_users, sizeof(int), 0);
                }
                for (int i = 0; i < new_chat->count_users; i++) {
                    char buf[32] = {0};
                    receive = recv_all(fd, buf, 32);
                    while (receive < 0) {
                        receive = recv_all(fd, buf, 32);
                    }
                    mx_push_back(&new_chat->users,mx_strdup(buf));
                    clear_message(buf, 32);
                }
                mx_push_back(&cur_client.chats, new_chat);

                add_chat_node(new_chat);

                /*
                Дим, тут данные о новом чате приняты на клиент, добавляй на локальную бд
                */
                
            
                //printf("%s gets chat %s\n", cur_client->login, new_chat->name);
                printf("> ");
                fflush(stdout);
            }
            else if (mx_strcmp(mx_strtrim(message), "<users list>") == 0) {
                pthread_mutex_lock(&cl_mutex);
                t_list *users_list = NULL;
                int count_users;
                recv(cur_client.serv_fd, &count_users, sizeof(int), 0);
                for (int i = 0; i < count_users; i++) {
                    printf("reseive\n");
                    char buf[20] = {0};
                    recv_all(cur_client.serv_fd, buf, 20);
                    mx_push_back(&users_list, mx_strtrim(buf));
                }
                t_main.search_users_list = users_list;
                pthread_mutex_unlock(&cl_mutex);

            }
            else{
                printf("%s\n", message);
                printf("> ");
                fflush(stdout);
            }
        } else if (receive == 0) {
            break;
        } else {
                // -1
		}
		clear_message(message, 512);
    }
    return NULL;

}
static void load_css() {
    t_screen.provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(t_screen.provider,"client/themes/dark.css");
}

 void send_login(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkWidget **entry_field = (GtkWidget **)data;
    GtkEntryBuffer *login_field_buf = gtk_entry_get_buffer(GTK_ENTRY (entry_field[0]));
    GtkEntryBuffer *password_field_buf = gtk_entry_get_buffer(GTK_ENTRY (entry_field[1]));
    
    int login_len = gtk_entry_buffer_get_length(login_field_buf);
    int password_len = gtk_entry_buffer_get_length(password_field_buf);

    if (login_len < 6 || login_len > 20) {
        gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessageLogin), "LOGIN MUST CONTAIN MORE THAN 3 CHARACTERS");
        gtk_widget_show(t_auth.ErrorMessageLogin);
        return;
    }

    if (password_len < 8 || password_len > 16) {
        gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessageLogin), "PASSWORD MUST CONTAIN MORE THAN 8 CHARACTERS");
        gtk_widget_show(t_auth.ErrorMessageLogin);
        return;
    }
    
    cur_client.login = mx_strdup(gtk_entry_buffer_get_text(login_field_buf));
    cur_client.passwd = mx_strdup(gtk_entry_buffer_get_text(password_field_buf));
    int status = check_auth_input(cur_client.login);
    switch(status) {
        case 0:
            break;
        default:{
            mx_strdel(&cur_client.login);
            mx_strdel(&cur_client.passwd);   
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessageLogin), "INCORRECT LOGIN OR PASSWORD");
            gtk_widget_show(t_auth.ErrorMessageLogin); 
            return;
        }
    }

    status = check_auth_input(cur_client.passwd);
    switch(status) {
        case 0:
            break;
        default:{
            mx_strdel(&cur_client.login);
            mx_strdel(&cur_client.passwd);
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessageLogin), "INCORRECT LOGIN OR PASSWORD");
            gtk_widget_show(t_auth.ErrorMessageLogin); 
            return;
        }
    }

    char message[32] = {0};
    // Отправка данных для авторизации на сервер
    send(cur_client.serv_fd, "i", 1, 0);
    sprintf(message, "%s", cur_client.login);
    send(cur_client.serv_fd, message, 32, 0);
    clear_message(message, 32);
    sprintf(message, "%s", cur_client.passwd);
    send(cur_client.serv_fd, message, 16, 0);

    bool err_aut;
    recv(cur_client.serv_fd, &err_aut, sizeof(bool), 0); // Ожидание ответа от сервера об успешности входа или регистрации
    
    if (err_aut) {
        mx_strdel(&cur_client.login);
        mx_strdel(&cur_client.passwd);
        gtk_label_set_label(GTK_LABEL(t_auth.ErrorMessageLogin), "INCORRECT LOGIN OR PASSWORD");
        gtk_widget_show(t_auth.ErrorMessageLogin);
        mx_printerr("login err\n");
        return;
    }
    //create_user_db(cur_client.login, cur_client.passwd);
    
    get_all_user_data();

    GtkWidget *child = gtk_window_get_child(GTK_WINDOW (t_screen.main_window));
    
    gtk_widget_unparent(child);
    // можем заходить в чат, вход успешен
    chat_show_main_screen(t_screen.main_window);
}

static void activate(GtkApplication *application)
{
    t_screen.main_window = gtk_application_window_new (application);
    gtk_window_set_title (GTK_WINDOW ( t_screen.main_window), "Swiftchat");
    gtk_window_set_default_size (GTK_WINDOW ( t_screen.main_window), 1200, 760);
    gtk_window_set_resizable (GTK_WINDOW ( t_screen.main_window), FALSE);

    load_css();
    chat_show_auth_screen();
    chat_decorate_headerbar();

    gtk_widget_show(t_screen.main_window);
}

int main(int argc, char *argv[]) {
    (void)argv;
    if (argc != 3) {
        mx_printerr("usage: ./uchat <server IP> <port>\n");
        return -1;
    }
    t_client cur = {
        .login = NULL,
        .passwd = NULL,
        .chat_count = 0,
        .chats = NULL
    };
    cur_client = cur;
    
    // Подключение к серверу, тут ничего менять не надо
    cur_client.serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(mx_atoi(argv[2]));
    connect(cur_client.serv_fd, (struct sockaddr *)&adr, sizeof(adr));
    inet_pton(AF_INET, argv[1], &adr.sin_addr); //"127.0.0.1"
    cur_client.adr = adr;
    // Запуск потоков для приёма и отправки сообщений, будем смотреть. Может, придётся переделать под события из гтк
    pthread_t sender_th;
    pthread_t rec_th;
    pthread_mutex_init(&cl_mutex, NULL);
    pthread_create(&sender_th, NULL, sender_func, &cur_client);
    pthread_create(&rec_th, NULL, rec_func, &cur_client.serv_fd);

    GtkApplication *application;
    gint status;
    application = gtk_application_new("my.first.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(application), FALSE, NULL);


    //ожидание завершения потоков, если нужно добавить код, то добавлять до этих функций
    pthread_join(sender_th, NULL);
    pthread_join(rec_th, NULL);



    close(cur_client.serv_fd);

    return 0;//status;
}
