#include "../inc/uch_client.h"

t_client *cur_client;

int recv_all(int sockfd, char * buf, int len)
{
    ssize_t n;

    while (len > 0)
    {
        n = recv(sockfd, buf, len, 0);
        if (n <= 0)
            return n;
        buf += n;
        len -= n;
    }

    return 1;
}

void *sender_func(void *param) {
    t_client *cur_client = (t_client *)param;
    //char message[512];
    char *message = (char *)malloc(512);
    char buf[512 + 32];
    fseek(stdin,0,SEEK_END);
    while(1) {
        printf("> ");
        fflush(stdout);
        size_t len;
        getline(&message, &len, stdin);
        //scanf("%s", message);
        message = mx_strtrim(message);
        if (mx_strcmp(message, "users") == 0) {
            send(cur_client->cl_socket, message, mx_strlen(message) - 1, 0);
        }
        else if (mx_strcmp(message, "exit") == 0) {
            send(cur_client->cl_socket, message, 4, 0);
            break;
        }
        else {
            //sprintf(buf, "%s: %s\n", cur_client->login, message);
            send(cur_client->cl_socket, message, mx_strlen(message), 0);
        }
        clear_message(message, 512);
        clear_message(buf, 512 + 32);

    }
    return NULL;
}

void *rec_func(void *param) {
    int fd = *(int *)param;
    char message[512] = {0};
    while (1) {
		int receive = recv(fd, message, 512, 0);
        
        if (receive > 0) {
            printf("|%s|\n", message);
            if(mx_strcmp(mx_strtrim(message), "add chat") == 0) {
                t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
                //printf("%s geting chat\n", cur_client->login);
                char buf_name[256] = {0};
                receive = recv_all(fd, buf_name, 256);
                while (receive < 0) {
                    receive = recv_all(fd, buf_name, 256);
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
                mx_push_back(&cur_client->chats, new_chat);

                /*
                Дим, тут данные о новом чате приняты на клиент, добавляй на локальную бд
                */

                //printf("%s gets chat %s\n", cur_client->login, new_chat->name);
                printf("> ");
                fflush(stdout);
            }
            else {
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

static void print_LogIn()
{
  g_print ("Welcome, user!\n");
}

static void load_css(GtkCssProvider *provider, GtkWidget *widget, gint widg)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    if(widg == 0)
    {
        gtk_style_context_add_class(context,"window_login");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 1)
    {
        gtk_style_context_add_class(context,"button_login");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else
    {
        gtk_style_context_add_class(context,"fixed_login");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
}

/*static*/ void activate(GtkApplication *application)
{
    GtkWidget *window;
    GtkWidget *button1, *entry_field1, *entry_field2, *logo;
    GtkWidget *fixed;
    GtkCssProvider *provider;

    window = gtk_application_window_new (application);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 1200, 760);
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

    fixed = gtk_fixed_new();
    button1 = gtk_button_new_with_label ("Log in");
    g_signal_connect (button1, "clicked", G_CALLBACK (print_LogIn), NULL);
    g_signal_connect_swapped (button1, "clicked", G_CALLBACK (gtk_window_destroy), window);
    entry_field1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_field1),"Your login...");
    entry_field2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_field2),"Your password...");
    gtk_entry_set_visibility(GTK_ENTRY(entry_field2),FALSE);
    gtk_button_set_has_frame(GTK_BUTTON(button1), FALSE);
    logo = gtk_image_new_from_file("Logo.png");

    gtk_fixed_put(GTK_FIXED(fixed), logo, 489, 107);
    gtk_fixed_put(GTK_FIXED(fixed), entry_field1, 445, 338);
    gtk_widget_set_size_request(entry_field1, 310, 36);
    gtk_fixed_put(GTK_FIXED(fixed), entry_field2, 445, 384);
    gtk_widget_set_size_request(entry_field2, 310, 36);
    gtk_fixed_put(GTK_FIXED(fixed), button1, 480, 450);
    gtk_widget_set_size_request(button1, 240, 55);
    gtk_window_set_child (GTK_WINDOW (window), fixed);


    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(provider),"client/style.css");
    load_css(provider, window, 0);
    load_css(provider, button1, 1);
    load_css(provider, fixed, 2);
    g_object_unref(provider);
    gtk_widget_show (window);

}


int main(int argc, char *argv[]) {
    (void)argv;
    if (argc != 3) {
        mx_printerr("usage: ./uchat <server IP> <port>\n");
        return -1;
    }
    
    /*GtkApplication *application;
    gint status;
    application = gtk_application_new("my.first.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(application), FALSE, NULL);
    */

    // Переменные для авторизации
    char login[32];
    char password[16];
    char choise; // i - вход, u - регистрация

    printf("Sign in or sign up? (i / u)\n");

    scanf("%c", &choise); // все сканфы убрать, оставил для наглядности, как считывает в терминале
    switch(choise) {
        case 'u':
        case 'i':
            printf("Enter login: ");
            scanf("%s",login);

            printf("Enter password: ");
            scanf("%s",password);

            break;
        default:
            mx_printerr("Invalid choise\n");
            exit(-1);
            break;
    }

    // Подключение к серверу, тут ничего менять не надо
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(mx_atoi(argv[2]));
    connect(fd, (struct sockaddr *)&adr, sizeof(adr));
    inet_pton(AF_INET, argv[1], &adr.sin_addr); //"127.0.0.1"

    // Отправка данных для авторизации на сервер
    send(fd, &choise, 1, 0);
    send(fd, login, 32, 0);
    send(fd, password, 16, 0);
    //printf("%c || %s || %s\n", choise, login, password);
    //char *jpeg = "testp.png";//"test.jpg";
    //send_jpeg(fd, jpeg);
    bool err_aut;
    recv(fd, &err_aut, sizeof(bool), 0); // Ожидание ответа от сервера об успешности входа или регистрации

    // Обработка ошибок от сервера, нужно ввести заново поля
    while (err_aut) {
        switch (choise) {
            case 'u':
                printf("user with this login already exists\n");
                printf("Enter login: ");
                scanf("%s",login);

                printf("Enter password: ");
                scanf("%s",password);
                break;
            
            case 'i':
                printf("login or password is invalid\n");
                printf("Enter login: ");
                scanf("%s",login);

                printf("Enter password: ");
                scanf("%s",password);
                break;
        }
        // Снова отправляем данные на сервер
        send(fd, &choise, 1, 0);
        send(fd, login, 32, 0);
        send(fd, password, 16, 0);
        recv(fd, &err_aut, sizeof(bool), 0);
    }


    // Запуск потоков для приёма и отправки сообщений, будем смотреть. Может, придётся переделать под события из гтк
    pthread_t sender_th;
    pthread_t rec_th;
    t_client cur = {
        .adr = adr,
        .cl_socket = fd,
        .login = login,
        .chat_count = 0,
        .chats = NULL,
        .cur_chat =NULL
    };
    cur_client = &cur;
    pthread_create(&sender_th, NULL, sender_func, &cur);
    pthread_create(&rec_th, NULL, rec_func, &fd);





    //ожидание завершения потоков, если нужно добавить код, то добавлять до этих функций
    pthread_join(sender_th, NULL);
    pthread_join(rec_th, NULL);

    close(fd);

    return 0;//status;
}
