#include "../inc/uch_client.h"

t_client cur_client;

void *sender_func(void *param) {
    t_client *cur_client = (t_client *)param;
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
            send(cur_client->cl_socket, message, mx_strlen(message) - 1, 0);
        }
        else if (mx_strcmp(message, "exit") == 0) {
            send(cur_client->cl_socket, message, 4, 0);
            break;
        }
        else {
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
                mx_push_back(&cur_client.chats, new_chat);

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

static void load_css(GtkCssProvider *provider, GtkWidget *widget, gint widg)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    if(widg == 0)
    {
        gtk_style_context_add_class(context,"LOGIN_window");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 1)
    {
        gtk_style_context_add_class(context,"LOGIN_main_box");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 2)
    {
        gtk_style_context_add_class(context,"LOGIN_logo_box");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 3)
    {
        gtk_style_context_add_class(context,"LOGIN_button");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 4)
    {
        gtk_style_context_add_class(context,"LOGIN_text_under_logo");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 5)
    {
        gtk_style_context_add_class(context,"LOGIN_text_next_logo");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);  
    }
    else if(widg == 6)
    {
        gtk_style_context_add_class(context,"LOGIN_create_account_text");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);  
    }
    else if(widg == 7)
    {
        gtk_style_context_add_class(context,"LOGIN_create_account_button");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);  
    }
    else if(widg == 8)
    {
        gtk_style_context_add_class(context,"LOGIN_button_box");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 9)
    {
        gtk_style_context_add_class(context,"LOGIN_entry_field1");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
}

GtkWidget *LOGIN_window, *LOGIN_entry_field1, *LOGIN_entry_field2;


static void print_LogIn() {
  cur_client.login = mx_strdup(gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (LOGIN_entry_field1))));
  cur_client.passwd = mx_strdup(gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (LOGIN_entry_field2))));

  printf("Hello, %s with password %s\n", cur_client.login, cur_client.passwd);
}

/*static*/ void activate(GtkApplication *application)
{
    GtkWidget *LOGIN_main_box, *LOGIN_logo_box, *LOGIN_button_box, *LOGIN_create_account_box;
    GtkWidget *LOGIN_button, *LOGIN_logo, *LOGIN_text_next_logo, *LOGIN_text_under_logo, /**LOGIN_entry_field1, *LOGIN_entry_field2,*/ *LOGIN_create_account_text, *LOGIN_create_account_button;
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider,"client/style.css");

    LOGIN_window = gtk_application_window_new (application);
    gtk_window_set_title (GTK_WINDOW (LOGIN_window), "Swiftchat");
    gtk_window_set_default_size (GTK_WINDOW (LOGIN_window), 1200, 760);
    gtk_window_set_resizable (GTK_WINDOW (LOGIN_window), FALSE);

    
    LOGIN_main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(LOGIN_main_box), 430, 0);
    gtk_widget_set_halign(GTK_WIDGET(LOGIN_main_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(LOGIN_main_box), GTK_ALIGN_CENTER);
    LOGIN_logo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); //бокс с логотипом и текстом
    gtk_widget_set_halign(GTK_WIDGET(LOGIN_logo_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(LOGIN_logo_box), GTK_ALIGN_CENTER);
    LOGIN_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); //бокс с кнопкой
    gtk_widget_set_halign(GTK_WIDGET(LOGIN_button_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(LOGIN_button_box), GTK_ALIGN_CENTER);
    LOGIN_create_account_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); //бокс с кнопкой
    gtk_widget_set_halign(GTK_WIDGET(LOGIN_create_account_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(LOGIN_create_account_box), GTK_ALIGN_CENTER);


    LOGIN_logo = gtk_image_new_from_file("client/logo2.png");
    gtk_widget_set_size_request(LOGIN_logo, 47, 47);
    gtk_box_append (GTK_BOX(LOGIN_logo_box), LOGIN_logo);
    LOGIN_text_next_logo = gtk_label_new("Swiftchat");
    gtk_widget_set_name(GTK_WIDGET(LOGIN_text_next_logo), "login_label");
    gtk_box_append (GTK_BOX(LOGIN_logo_box), LOGIN_text_next_logo);
    LOGIN_button = gtk_button_new_with_label ("Log in");
    gtk_widget_set_size_request(LOGIN_button, 240, 55);
    gtk_box_append (GTK_BOX(LOGIN_button_box), LOGIN_button);
    g_signal_connect (LOGIN_button, "clicked", G_CALLBACK (print_LogIn), NULL);
    LOGIN_create_account_text = gtk_label_new("New here? ");
    gtk_box_append (GTK_BOX(LOGIN_create_account_box), LOGIN_create_account_text);
    LOGIN_create_account_button = gtk_label_new("Create an account");

    GtkGesture *click_create_acc = gtk_gesture_click_new();
    gtk_gesture_set_state(click_create_acc, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_create_acc, "pressed", G_CALLBACK(gtk_widget_hide), LOGIN_button);
    gtk_widget_add_controller(LOGIN_create_account_button, GTK_EVENT_CONTROLLER(click_create_acc));

    gtk_box_append (GTK_BOX(LOGIN_create_account_box), LOGIN_create_account_button);
    
//-----------------------------------------main_box--------------------------------------------------//
    LOGIN_entry_field1 = gtk_entry_new();
    gtk_widget_set_size_request(LOGIN_entry_field1, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(LOGIN_entry_field1),"Username");
    LOGIN_entry_field2 = gtk_entry_new();
    gtk_widget_set_size_request(LOGIN_entry_field2, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(LOGIN_entry_field2),"Password");
    gtk_entry_set_visibility(GTK_ENTRY(LOGIN_entry_field2),FALSE);
    LOGIN_text_under_logo= gtk_label_new("LOG IN TO YOUR ACCOUNT TO CONTINUE");
    gtk_widget_set_name(GTK_WIDGET(LOGIN_text_under_logo), "login_label");

    gtk_box_append (GTK_BOX(LOGIN_main_box), LOGIN_logo_box);
    gtk_box_append (GTK_BOX(LOGIN_main_box), LOGIN_text_under_logo);
    gtk_box_append (GTK_BOX(LOGIN_main_box), LOGIN_entry_field1);
    gtk_box_append (GTK_BOX(LOGIN_main_box), LOGIN_entry_field2);
    gtk_box_append (GTK_BOX(LOGIN_main_box), LOGIN_button_box);
    gtk_box_append (GTK_BOX(LOGIN_main_box), LOGIN_create_account_box);
    gtk_box_set_spacing (GTK_BOX(LOGIN_main_box), 0);
//-----------------------------------------main_box--------------------------------------------------//
    gtk_window_set_child (GTK_WINDOW (LOGIN_window), LOGIN_main_box);


    load_css(provider, LOGIN_window, 0);
    load_css(provider, LOGIN_main_box, 1);
    load_css(provider, LOGIN_logo_box, 2);
    load_css(provider, LOGIN_button, 3);
    load_css(provider, LOGIN_text_under_logo, 4);
    load_css(provider, LOGIN_text_next_logo, 5);
    load_css(provider, LOGIN_create_account_text, 6);
    load_css(provider, LOGIN_create_account_button, 7);
    load_css(provider, LOGIN_button_box, 8);
    load_css(provider, LOGIN_entry_field1, 9);

    gtk_widget_show (LOGIN_window);

}

int main(int argc, char *argv[]) {
    (void)argv;
    if (argc != 3) {
        mx_printerr("usage: ./uchat <server IP> <port>\n");
        return -1;
    }
    
    GtkApplication *application;
    gint status;
    application = gtk_application_new("my.first.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(application), FALSE, NULL);
    

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
        .login = NULL,
        .passwd = NULL,
        .chat_count = 0,
        .chats = NULL,
        .cur_chat =NULL
    };
    cur_client = cur;
    pthread_create(&sender_th, NULL, sender_func, &cur);
    pthread_create(&rec_th, NULL, rec_func, &fd);





    //ожидание завершения потоков, если нужно добавить код, то добавлять до этих функций
    pthread_join(sender_th, NULL);
    pthread_join(rec_th, NULL);

    close(fd);

    return 0;//status;
}
