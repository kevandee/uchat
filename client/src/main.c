#include "../inc/uch_client.h"

t_client cur_client;
pthread_mutex_t cl_mutex;

static void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

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
            SSL_write(cur_client.ssl, message, mx_strlen(message) - 1);
        }
        else if (mx_strcmp(message, "exit") == 0) {
            SSL_write(cur_client.ssl, message, 4);
            break;
        }
        else {
            SSL_write(cur_client.ssl, message, mx_strlen(message));
        }
        clear_message(message, 512);
        clear_message(buf, 512 + 32);

    }
    return NULL;
}

gboolean add_msg(gpointer data) {
    t_message *message = data;
    char *total_msg = message->data;
    GtkWidget *incoming_msg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *incoming_msg = gtk_label_new(total_msg);
    bool is_sender = false;
    if (mx_strcmp(message->sender, cur_client.login) != 0){
        gtk_widget_set_halign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_START);
    }   
    else {
        gtk_widget_set_halign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_END);
        gtk_widget_set_valign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_END);

        GtkGesture *gesture = gtk_gesture_click_new();
        gtk_gesture_set_state(gesture, GTK_EVENT_SEQUENCE_CLAIMED);
        gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture), 3);
        GtkWidget **arr = (GtkWidget **)malloc(2*sizeof(GtkWidget *));
        arr[0] = incoming_msg;
        arr[1] = incoming_msg_box;
        int *mes_id = (int *)malloc(sizeof(int));
        *mes_id = message->id;
        t_list *gesture_data = NULL;
        mx_push_back(&gesture_data, arr);
        mx_push_back(&gesture_data, mes_id);
        g_signal_connect_after(gesture, "pressed", G_CALLBACK(show_message_menu), gesture_data);
        gtk_widget_add_controller(incoming_msg_box, GTK_EVENT_CONTROLLER(gesture));

        
        is_sender = true;
    }
    gtk_widget_set_margin_end(incoming_msg_box, 5);
    gtk_widget_set_margin_bottom(incoming_msg_box, 5);
   
    if (!is_sender)
        gtk_widget_set_name(GTK_WIDGET(incoming_msg), "incoming-message");
    else 
        gtk_widget_set_name(GTK_WIDGET(incoming_msg), "message");
    load_css_main(t_screen.provider, incoming_msg);
    gtk_label_set_wrap(GTK_LABEL(incoming_msg), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(incoming_msg), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_max_width_chars(GTK_LABEL(incoming_msg), 50);
    gtk_label_set_selectable(GTK_LABEL(incoming_msg), FALSE);
    GtkWidget *User_logo = NULL;
    if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0) {
        if (!is_sender) {
            t_list *avatars = cur_client.cur_chat.users_avatars;
            t_list *user_list = cur_client.cur_chat.users;
            while (mx_strcmp(user_list->data, message->sender) != 0 && user_list) {
                if (mx_strcmp(user_list->data, cur_client.login) == 0) {
                    user_list = user_list->next;
                    continue;
                }
                avatars = avatars->next;
                user_list = user_list->next;
            }

            if (avatars) {
                t_avatar *draw = avatars->data;
                if (mx_strcmp (draw->name, "default") == 0) {
                    draw = &t_main.default_avatar;
                }
                User_logo = get_circle_widget_from_png_avatar(draw, 45, 45, false);
                gtk_box_append(GTK_BOX(incoming_msg_box), User_logo);
            }
        }
        else {
            User_logo = get_circle_widget_current_user_avatar();
        }
    }

    gtk_box_append(GTK_BOX(incoming_msg_box), incoming_msg);
    if (is_sender && mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0)
        gtk_box_append(GTK_BOX(incoming_msg_box), User_logo);
    if (!message->prev) {
        gtk_box_append(GTK_BOX(t_main.scroll_box_right), incoming_msg_box);
        mx_push_front(&t_main.message_widgets_list, incoming_msg);
    }
    else {
        gtk_box_prepend(GTK_BOX(t_main.scroll_box_right), incoming_msg_box);
        mx_push_back(&t_main.message_widgets_list, incoming_msg);
    }
    t_main.last_mes = incoming_msg_box;
    
    pthread_mutex_unlock(&cl_mutex);

    return FALSE;
}

void *rec_func(void *param) {
    while(!t_main.loaded) {
        sleep(1);
    }
    SSL *fd = cur_client.ssl;
    (void)param;
    char message[512 + 32] = {0};
    while (1) {
		int receive = SSL_read(fd, message, 512 + 32);

        if (receive > 0) {
            printf("|%s|\n", message);
            if(mx_strcmp(mx_strtrim(message), "<add chat>") == 0) {
                t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
                new_chat->is_new = true;
                new_chat->messages = NULL;
                new_chat->users = NULL;

                char buf_name[256] = {0};
                receive = recv_all(fd, buf_name, 256);
                while (receive < 0) {
                    receive = recv_all(fd, buf_name, 256);
                }
                mx_strcpy(new_chat->name, buf_name);
                printf("recv chatname %s\n", new_chat->name);
                receive = SSL_read(fd, &new_chat->id, sizeof(int));
                while (receive < 0) {
                    receive = SSL_read(fd, &new_chat->id, sizeof(int));
                }
                receive = SSL_read(fd, &new_chat->count_users, sizeof(int));
                while (receive < 0) {
                    receive = SSL_read(fd, &new_chat->count_users, sizeof(int));
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
                /////// аватарка чата
                if (mx_strncmp(new_chat->name, ".dialog", 7) !=0) {
                    printf("a\n");
                    t_main.loaded_avatar = (t_avatar *)malloc(sizeof(t_avatar));
                    get_avatar(t_main.loaded_avatar);
                    printf("gets avatar\n");
                    new_chat->avatar=*t_main.loaded_avatar;
                    if (mx_strcmp(new_chat->avatar.name, "default") == 0) {
                        if (mx_strncmp(new_chat->name, ".dialog", 7) != 0)
                            new_chat->avatar = t_main.default_group_avatar;
                        else
                            new_chat->avatar = t_main.default_avatar;
                    }
                    new_chat->is_avatar = true;
                }
                else {
                    new_chat->is_avatar = false;
                }

                //////
                mx_push_back(&cur_client.chats, new_chat);
                if (!cur_client.sender_new_chat){
                    g_idle_add(add_chat_node, new_chat);
                }
                else {
                    cur_client.sender_new_chat = false;
                }
                cur_client.chat_count++;
                t_main.loaded = true;
                printf("chat added\n");
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
                printf("reseive\n");
                SSL_read(cur_client.ssl, &count_users, sizeof(int));
                for (int i = 0; i < count_users; i++) {
                    printf("reseive\n");
                    char buf[20] = {0};
                    recv_all(cur_client.ssl, buf, 20);
                    mx_push_back(&users_list, mx_strtrim(buf));
                }
                t_main.search_users_list = users_list;
                pthread_mutex_unlock(&cl_mutex);
                t_main.loaded = true;

            }
            else if (mx_strncmp(message, "<msg, chat_id=", 14) == 0){ // "<msg, chat_id=%d, mes_id=%d, from=%s, prev=1>%s"
                char *temp = message + 14;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                printf("%s\n", c_id);
                int chat_id = mx_atoi(c_id);                        // ид чата, в который надо вставить сообщение
                (void)chat_id; // избавляюсь от unused variable
                mx_strdel(&c_id);
                temp = mx_strstr(message, "mes_id=") + 7;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *mes_id = mx_strndup(temp, len);
                printf("%s\n", mes_id);
                int message_id = mx_atoi(mes_id);
                mx_strdel(&c_id);
                temp = mx_strstr(message, "from=") + 5;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *sender = mx_strndup(temp, len);              // отправитель
                printf("%s\n", sender);
                temp = mx_strstr(message, "prev=") + 5;
                bool prev = *temp == '0' ? false : true; 
                printf("bool %i %c\n", prev, *temp);

                char *total_msg = mx_strdup(mx_strchr(message, '>') + 1);     // сообщение
                                                                   // время надо получить локально на клиенте
                printf("%s\n", total_msg);
                
                if (cur_client.cur_chat.id == chat_id) {
                    t_message *mes = (t_message *)malloc(sizeof(t_message));
                    mes->c_id = chat_id;
                    mes->prev = prev;
                    mes->id = message_id;
                    mx_strcpy(mes->sender, sender);
                    mx_strcpy(mes->data, total_msg);
                    pthread_mutex_lock(&cl_mutex);
                    g_idle_add(add_msg, mes);
                    pthread_mutex_lock(&cl_mutex);
                    if (prev) {
                        cur_client.cur_chat.last_mes_id = mes->id;
                        int status = 1;
                        SSL_write(cur_client.ssl, &status, sizeof(int));
                        mx_push_back(&cur_client.cur_chat.messages, mes);
                    }
                    else {
                        mx_push_front(&cur_client.cur_chat.messages, mes);
                    }
                    
                    
                    pthread_mutex_unlock(&cl_mutex);
                }
                else if (prev) {
                    int status = 0;
                    SSL_write(cur_client.ssl, &status, sizeof(int));        
                }
                
                if (!prev || t_main.scroll_mes) {           
                    pthread_t display_thread = NULL;
                    pthread_create(&display_thread, NULL, scroll_func, NULL);  
                }
        
                printf("%s\n", total_msg);
                printf("> ");
                fflush(stdout);
            }
            else if(mx_strncmp(message, "<last message>", 14) == 0) {
                if (!t_main.first_load_mes){
                    pthread_t display_thread = NULL;
                    pthread_create(&display_thread, NULL, save_scroll_func, NULL);
                }
                else {
                    printf ("down\n");
                    pthread_t display_thread = NULL;
                    pthread_create(&display_thread, NULL, scroll_func, NULL);  
                    t_main.first_load_mes = false;
                }  
            } // "<get last mes id>"
            else if (mx_strncmp(message, "<get last mes id>", 17) == 0) {
                SSL_read(cur_client.ssl, &t_main.send_mes_id, sizeof(int));
                t_main.loaded = true;
            }
            else if(mx_strncmp(message, "<delete mes chat_id=", 20) == 0) { // "<delete mes chat_id=%d, mes_id=%d>"
                char *temp = message + 20;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                printf("%s\n", c_id);
                int chat_id = mx_atoi(c_id);
                mx_strdel(&c_id);
                if (cur_client.cur_chat.id == chat_id) {
                    // найти и удалить
                    temp = mx_strstr(temp, "mes_id=") + 7;
                    len = 0;
                    while (*(temp + len) != '>') {
                        len++;
                    }
                    char *m_id = mx_strndup(temp, len);
                    printf("%s\n", m_id);
                    int mes_id = mx_atoi(m_id);
                    mx_strdel(&m_id);

                    t_list *temp_mes = cur_client.cur_chat.messages;
                    t_list *temp_widgets = t_main.message_widgets_list;
                    t_list *prev_m = NULL;
                    t_list *prev_w = NULL;

                    while(temp_mes) {
                        t_message *mes = (t_message *)temp_mes->data;
                        if (mes->id == mes_id) {
                            break;
                        }
                        prev_m = temp_mes;
                        prev_w = temp_widgets;

                        temp_widgets = temp_widgets->next;
                        temp_mes = temp_mes->next;
                    }

                    GtkWidget *box = gtk_widget_get_parent(GTK_WIDGET (temp_widgets->data));
                    gtk_widget_hide(box);
    
                    if (temp_mes && prev_m){
                        prev_m->next = temp_mes->next;
                        free(temp_mes->data);
                        free(temp_mes);
                        temp_mes = NULL;
                    }
                    if (temp_widgets && prev_w) {
                        prev_w->next = temp_widgets->next;
                        free(temp_widgets);
                        temp_widgets = NULL;
                    }
                    if (!prev_w) {
                        mx_pop_front(&t_main.message_widgets_list);
                    }
                    if(!prev_m) {
                        mx_pop_front(&cur_client.cur_chat.messages);

                    }

                } 
            }
            if(mx_strncmp(message, "<edit msg, chat_id=", 19) == 0) { // "<edit msg, chat_id=%d, mes_id=%d>%s"
                char *temp = message + 19;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                printf("%s\n", c_id);
                int chat_id = mx_atoi(c_id);
                mx_strdel(&c_id);
                if (cur_client.cur_chat.id == chat_id) {
                    // найти и изменить
                    temp = mx_strstr(temp, "mes_id=") + 7;
                    len = 0;
                    while (*(temp + len) != '>') {
                        len++;
                    }
                    char *m_id = mx_strndup(temp, len);
                    printf("%s\n", m_id);
                    int mes_id = mx_atoi(m_id);
                    mx_strdel(&m_id);

                    t_list *temp_mes = cur_client.cur_chat.messages;
                    t_list *temp_widgets = t_main.message_widgets_list;

                    while(temp_mes) {
                        t_message *mes = (t_message *)temp_mes->data;
                        if (mes->id == mes_id) {
                            break;
                        }

                        temp_widgets = temp_widgets->next;
                        temp_mes = temp_mes->next;
                    }

                    const char *text = mx_strchr(message, '>') + 1;
                    gtk_label_set_text(GTK_LABEL (temp_widgets->data), text);
                } 
            }
            else if(mx_strcmp(mx_strtrim(message), "<setting avatar>") == 0) {
                printf("a\n");
                char buf[544] = {0};
                sprintf(buf, "client_data/%s", cur_client.avatar.name);
                recv_image(cur_client.ssl, buf);
                mx_strdel(&cur_client.avatar.path);
                cur_client.avatar.path = mx_strdup(buf);

                printf("setts avatar\n");
                t_main.loaded = true;
            }
            if(mx_strncmp(mx_strtrim(message), "<get user avatar>",17) == 0) {
                printf("a\n");
                t_main.loaded_avatar = (t_avatar *)malloc(sizeof(t_avatar));
                get_avatar(t_main.loaded_avatar);
                printf("gets avatar %s\n", t_main.loaded_avatar->name);
                t_main.loaded = true;
            }
            else if(mx_strcmp(mx_strtrim(message), "<image loaded>") == 0) {

                t_main.loaded = true;
            }
            else if (mx_strcmp(mx_strtrim(message), "<chat users avatars>") == 0) {
                cur_client.cur_chat.users_avatars = get_chat_users_avatars(&cur_client.cur_chat);
                t_main.loaded = true;
            }
        }  
        if (receive == 0) {
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
    SSL_write(cur_client.ssl, "i", 1);
    sprintf(message, "%s", cur_client.login);
    SSL_write(cur_client.ssl, message, 32);
    clear_message(message, 32);
    sprintf(message, "%s", cur_client.passwd);
    SSL_write(cur_client.ssl, message, 16);

    bool err_aut;
    SSL_read(cur_client.ssl, &err_aut, sizeof(bool)); // Ожидание ответа от сервера об успешности входа или регистрации

    if (err_aut) {
        mx_strdel(&cur_client.login);
        mx_strdel(&cur_client.passwd);
        gtk_label_set_label(GTK_LABEL(t_auth.ErrorMessageLogin), "INCORRECT LOGIN OR PASSWORD");
        gtk_widget_show(t_auth.ErrorMessageLogin);
        mx_printerr("login err\n");
        return;
    }

    get_all_user_data();
    create_user_db(cur_client);

    GtkWidget *child = gtk_window_get_child(GTK_WINDOW (t_screen.main_window));
    
    gtk_widget_unparent(child);
    // можем заходить в чат, вход успешен
    chat_show_main_screen(t_screen.main_window);
}

static void activate(GtkApplication *application) {
    t_screen.main_window = gtk_application_window_new (application);
    gtk_window_set_title (GTK_WINDOW ( t_screen.main_window), "Swiftchat");
    gtk_window_set_default_size (GTK_WINDOW ( t_screen.main_window), 1200, 760);
    gtk_window_set_resizable (GTK_WINDOW ( t_screen.main_window), FALSE);

    /*struct stat c_buffer;
    int exist = stat("client_data/client.db", &c_buffer);
    if (exist == 0) {
        t_list *temp = user_exec_db("SELECT login, password FROM user;", 1);
    }*/
    //РОМА ТУТ НАДО ЗАПУСТИТЬ ГЛАВНЫЙ ЭКРАН ПО КОНКРЕТНОМУ ПОЛЬЗОВАТЕЛЮ (ДАННЫЕ В temp) ИГНОРИРУЮ АВТОРИЗАЦИЮ/РЕГИСТРАЦИЮ
    //else {
        load_css();
        chat_show_auth_screen();
        chat_decorate_headerbar();
        gtk_widget_show(t_screen.main_window);
    //}
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
        .ssl = NULL,
        .avatar = {
            .orig_w = 512,
            .orig_h = 512,
            .scaled_w = 300,
            .scaled_h = 300,
            .image = NULL,
            .path = "client/media/default_user.png",
            .name = "default_user.png", 
            .x = 200,
            .y = 200
        },
    };
    t_avatar default_avatar = {
        .orig_w = 512,
        .orig_h = 512,
        .scaled_w = 300,
        .scaled_h = 300,
        .image = NULL,
        .path = "client/media/default_user.png",
        .name = "default_user.png", 
        .x = 200,
        .y = 200
    };
    t_main.default_avatar = default_avatar;
    t_avatar default_group_avatar = {
        .orig_w = 512,
        .orig_h = 512,
        .scaled_w = 300,
        .scaled_h = 300,
        .image = NULL,
        .path = "client/media/default_groupchat.png",
        .name = "default_groupchat.png", 
        .x = 200,
        .y = 200
    };
    t_main.default_group_avatar = default_group_avatar;
    cur_client = cur;;

    //      =====   SSLing    =====
    SSL_CTX *context;
    SSL *ssl;
    SSL_library_init();

    open_client_connection(argv[1], mx_atoi(argv[2]));

    context = CTX_initialize_client();
    ssl = SSL_new(context);
    if (SSL_set_fd(ssl, cur_client.serv_fd) == 0) {
        perror("ERROR: socket descriptor attachment failed!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    cur_client.ssl = ssl;

    if (SSL_connect(ssl) == -1) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    printf("SSL: chipher: %s\n", SSL_get_cipher(ssl));
    X509 *cert = SSL_get_peer_certificate(ssl);
    if (cert == NULL) {
        printf("SSL: No certificates configured.\n");
    }
    else {
        printf("SSL: Server certificates:\n");
        char *line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("SSL: Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("SSL: Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }

    //      echo server check
    /*const char *check_request = "Pablo";
    int check_len = mx_strlen(check_request);
    SSL_write(ssl, check_request, check_len);
    char check_reply[6] = {0};
    SSL_read(ssl, &tempbool, sizeof(bool));
    printf("SSL: request - %s\nSSL: reply   - %d\n", check_request, tempbool);*/
    //      =====   SSLing    =====

    // Запуск потоков для приёма и отправки сообщений, будем смотреть. Может, придётся переделать под события из гтк
    t_main.loaded = false;
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
    SSL_free(ssl);
    SSL_CTX_free(context);

    return 0;//status;
}
