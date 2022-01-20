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



int x = 0,y = 0;

gboolean key_press_event(GtkEventControllerKey *controller,
               guint                  keyval,
               guint                  keycode,
               GdkModifierType        state,
               gpointer               user_data) {
    (void)controller;
    (void)keycode;
    (void)state;
    (void)user_data;
    switch(keyval) {
        case GDK_KEY_Down:
            y++;
            break;
        case GDK_KEY_Up:
            y--;
            break;
        case GDK_KEY_Left:
            x--;
            break;
        case GDK_KEY_Right:
            x++;
            break;
    }
    
    gtk_widget_queue_draw(GTK_WIDGET(user_data));

    return TRUE;
}


static void draw_circle(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data) {
    (void)widget;
    (void)w;
    (void)h;

    cairo_surface_t *image = (cairo_surface_t *)data;
    cairo_surface_t *target = cairo_image_surface_create(cairo_image_surface_get_format(image), 128, 128); //gdk_surface_create_similar_surface(GDK_SURFACE (image), cairo_surface_get_content(image), cairo_image_surface_get_width(image), cairo_image_surface_get_height(image));
    cairo_t *cr_new = cairo_create(target);    
    cairo_set_source_surface (cr, image, x, y);
    
    cairo_arc(cr, 64, 64, 60, 0, 2 * M_PI);
    cairo_clip(cr);
    //cairo_fill(cr);
    cairo_paint(cr);

    cairo_set_source_surface (cr_new, image, 1, 1);

    
    cairo_arc(cr_new, x, y, 60, 0, 2 * M_PI);
    cairo_clip(cr_new);
    cairo_paint(cr_new);

    //cairo_fill(cr);
    cairo_surface_write_to_png (target, "output.png");
    //return FALSE;
}

static void main_chat(GtkWidget *window) {
    GtkWidget *main_box = NULL;//, *logo_box = NULL,  *logo = NULL, *text_next_logo = NULL;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(main_box), 1100, 0);
    gtk_widget_set_halign(GTK_WIDGET(main_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(main_box), GTK_ALIGN_CENTER);
    
    GtkEventController *keys_controller = gtk_event_controller_key_new();
    gtk_widget_add_controller(t_screen.main_window, keys_controller);
    
   // draw circle frow image

    GtkWidget *darea = NULL;
    gint width, height;
    
    cairo_surface_t *image = get_surface_from_jpg("test.jpeg");//cairo_image_surface_create_from_png("test_circle.png");
    //cairo_surface_t *scaled_image = cairo_image_surface_create_for_data(cairo_image_surface_get_data(image), CAIRO_FORMAT_RGB24, 64, 64, cairo_image_surface_get_stride(image));
    width = cairo_image_surface_get_width(image);
    height = cairo_image_surface_get_height(image);
    printf("%d %d\n", width, height);
    
    cairo_surface_t *scaled_image = scale_to_half(image, width, height, 128, 128);
    width = cairo_image_surface_get_width(scaled_image);
    height = cairo_image_surface_get_height(scaled_image);
    printf("%d %d\n", width, height);
    

    darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 256);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 256);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle, scaled_image, NULL);
    gtk_widget_set_halign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);

    g_signal_connect(keys_controller, "key-pressed", G_CALLBACK(key_press_event), darea);
    /*GtkWidget *grid = gtk_grid_new();

    for (int i = 0; i < 9; i++) {
        //gtk_widget_set_valign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);
        for (int j = 0; j < 5; j++) {
            darea = gtk_drawing_area_new();
            gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 128);
            gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 128);
            gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle, scaled_image, NULL);
            
            gtk_grid_attach(GTK_GRID (grid), darea, i, j, 1, 1);   
        }
    }
    */
    gtk_box_append (GTK_BOX(main_box), darea);
    
    //gtk_box_append (GTK_BOX(main_box), grid);
    gtk_box_set_spacing (GTK_BOX(main_box), 0);

    gtk_window_set_child(GTK_WINDOW(window), main_box);



    gtk_widget_show(window);
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
        gtk_label_set_label(GTK_LABEL(t_auth.ErrorMessageLogin), "INCORRECT LOGIN OR PASSWORD");
        gtk_widget_show(t_auth.ErrorMessageLogin);
        mx_printerr("login err\n");
        return;
    }
    GtkWidget *child = gtk_window_get_child(GTK_WINDOW (t_screen.main_window));
    
    gtk_widget_unparent(child);
    // можем заходить в чат, вход успешен
    main_chat(t_screen.main_window);
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
