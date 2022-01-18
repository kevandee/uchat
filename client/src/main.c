#include "../inc/uch_client.h"

t_client cur_client;

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
    int fd = cur_client.serv_fd;
    (void)param;
    char message[512] = {0};
    while (1) {
		int receive = recv(fd, message, 512, 0);
        
        if (receive > 0) {
            printf("|%s|\n", message);
            if(mx_strcmp(mx_strtrim(message), "add chat") == 0) {
                t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
                new_chat->messages = NULL;
                new_chat->users = NULL;
                //printf("%s geting chat\n", cur_client->login);
                char buf_name[256] = {0};
                receive = recv_all(fd, buf_name, 256);
                while (receive < 0) {
                    receive = recv_all(fd, buf_name, 256);
                }
                mx_strcpy(new_chat->name, buf_name);
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

static void load_css()
{
    t_screen.provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(t_screen.provider,"client/themes/dark.css");
}

static void draw_circle(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data) {
    (void)widget;
    (void)w;
    (void)h;

    cairo_surface_t *image = (cairo_surface_t *)data;

    cairo_set_source_surface (cr, image, 1, 1);
    cairo_arc(cr, 328, 328, 300, 0, 2 * M_PI);
    cairo_clip(cr);
    cairo_paint(cr);
    //cairo_fill(cr);

    //return FALSE;
}

static void main_chat(GtkWidget *window) {
    GtkWidget *main_box = NULL;//, *logo_box = NULL,  *logo = NULL, *text_next_logo = NULL;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(main_box), 1100, 0);
    gtk_widget_set_halign(GTK_WIDGET(main_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(main_box), GTK_ALIGN_CENTER);
    
   // draw circle frow image

    GtkWidget *darea = NULL;
    gint width, height;

    cairo_surface_t *image = cairo_image_surface_create_from_png("test_circle.png");
    width = cairo_image_surface_get_width(image);
    height = cairo_image_surface_get_height(image);
    printf("%d %d\n", width, height);


    darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 700);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 700);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle, image, NULL);
    
    //gtk_box_append (GTK_BOX(main_box), logo_box);
    gtk_box_append (GTK_BOX(main_box), darea);
    gtk_box_set_spacing (GTK_BOX(main_box), 0);

    gtk_window_set_child(GTK_WINDOW(window), main_box);



    gtk_widget_show(window);
}

 void send_login(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkWidget **entry_field = (GtkWidget **)data;

    cur_client.login = mx_strdup(gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (entry_field[0]))));
    cur_client.passwd = mx_strdup(gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (entry_field[1]))));

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
        // ошибка
        mx_printerr("login err\n");
    }
    GtkWidget *child = gtk_window_get_child(GTK_WINDOW (entry_field[2]));
    
    gtk_widget_unparent(child);
    // можем заходить в чат, вход успешен
    main_chat(entry_field[2]);
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
        .chats = NULL,
        .cur_chat =NULL
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

    GtkApplication *application;
    gint status;
    application = gtk_application_new("my.first.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(application), FALSE, NULL);
    

    // Переменные для авторизации
    //char login[32];
    //char password[16];
    //char choise = 'i'; // i - вход, u - регистрация
    /*
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

    

    // Отправка данных для авторизации на сервер
    send(fd, &choise, 1, 0);
    send(fd, login, 32, 0);
    send(fd, password, 16, 0);
    //printf("%c || %s || %s\n", choise, login, password);
    //char *jpeg = "testp.png";//"test.jpg";
    //send_jpeg(fd, jpeg);*/
    /*bool err_aut;
    recv(cur_client.serv_fd, &err_aut, sizeof(bool), 0); // Ожидание ответа от сервера об успешности входа или регистрации

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
        send(cur_client.serv_fd, &choise, 1, 0);
        send(cur_client.serv_fd, login, 32, 0);
        send(cur_client.serv_fd, password, 16, 0);
        recv(cur_client.serv_fd, &err_aut, sizeof(bool), 0);
    }
    */

    // Запуск потоков для приёма и отправки сообщений, будем смотреть. Может, придётся переделать под события из гтк
    pthread_t sender_th;
    pthread_t rec_th;
    
    pthread_create(&sender_th, NULL, sender_func, &cur_client);
    pthread_create(&rec_th, NULL, rec_func, &cur_client.serv_fd);

    //ожидание завершения потоков, если нужно добавить код, то добавлять до этих функций
    pthread_join(sender_th, NULL);
    pthread_join(rec_th, NULL);

    close(cur_client.serv_fd);

    return 0;//status;
}
