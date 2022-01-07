#include "../inc/uch_client.h"

GtkWidget *login_label, *login_entry;
GtkWidget *password_label, *password_entry;

const gchar *glogin, *gpasswd;
char choise;

void set_button_clicked(GtkWidget *button, gpointer data)
{
    (void)button; (void)data;
    glogin = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) login_entry));
    gpasswd = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) password_entry));
}

void set_button_clicked_reg(GtkWidget *button, gpointer data)
{
    set_button_clicked(button, data);
    choise = 'u'; // sign up
    gtk_main_quit();
}

void set_button_clicked_log(GtkWidget *button, gpointer data)
{
    set_button_clicked(button, data);
    choise = 'i'; // sign in
    gtk_main_quit();
}
 
void del_button_clicked(GtkWidget *button, gpointer data)
{
    (void)button; (void)data;
    gtk_entry_set_text(GTK_ENTRY((GtkWidget*) login_entry), "");
    gtk_entry_set_text(GTK_ENTRY((GtkWidget*) password_entry), "");
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
            sprintf(buf, "%s: %s\n", cur_client->name, message);
            send(cur_client->cl_socket, buf, mx_strlen(buf) - 1, 0);
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
            printf("%s\n", message);
            printf("> ");
            fflush(stdout);
        } else if (receive == 0) {
            break;
        } else {
                // -1
		}
		clear_message(message, 512);
    }
    return NULL;

}



int main(int argc, char *argv[]) {
    if (argc != 3) {
        mx_printerr("usage: ./uchat <server IP> <port>\n");
        return -1;
    }
    
    GtkWidget *window;
    GtkWidget *hbox1, *hbox2, *hbox3;
    GtkWidget *vbox;
    GtkWidget *reg_button, *log_button, *del_button;
 
    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 
    gtk_window_set_title (GTK_WINDOW (window), "podroChill");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
 
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
 
    // Создаем ярлык и поле ввода логина
    login_label = gtk_label_new("Введите логин:");
    login_entry = gtk_entry_new();
 
    // Создаем ярлык и поле ввода пароля
    password_label = gtk_label_new("Введите пароль:");
    password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
 
    // Создаем кнопки
    log_button = gtk_button_new_with_label("Войти");
    reg_button = gtk_button_new_with_label("Зарегистрироваться");
    del_button = gtk_button_new_with_label("Удалить");
 
    // Задаем функции кнопок
    g_signal_connect(GTK_BUTTON(log_button), "clicked", G_CALLBACK(set_button_clicked_log), login_entry);
    g_signal_connect(GTK_BUTTON(reg_button), "clicked", G_CALLBACK(set_button_clicked_reg), login_entry);
    g_signal_connect(GTK_BUTTON(del_button), "clicked", G_CALLBACK(del_button_clicked), password_entry);
 
    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), login_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), login_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, FALSE, 5);
 
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), password_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), password_entry, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, FALSE, 5);
 
    hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox3), log_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox3), reg_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox3), del_button, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox3, TRUE, FALSE, 5);
 
    gtk_container_add(GTK_CONTAINER(window), vbox);
 
 
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
 
    gtk_main ();

    char login[32];
    char password[16];
    mx_strcpy(login, glogin);
    mx_strcpy(password, gpasswd);
   /* printf("Sign in or sign up? (i / u)\n");

    //scanf("%c", &choise);
    switch(choise) {
        case 'u':
        case 'i':
            //printf("Enter login: ");
            //scanf("%s",login);

           // printf("Enter password: ");
            //scanf("%s",password);

            break;
        default:
            mx_printerr("Invalid choise\n");
            exit(-1);
            break;
    }*/
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(mx_atoi(argv[2]));
    connect(fd, (struct sockaddr *)&adr, sizeof(adr));
    inet_pton(AF_INET, argv[1], &adr.sin_addr); //"127.0.0.1"

    send(fd, &choise, 1, 0);
    send(fd, login, 32, 0);
    send(fd, password, 16, 0);

    pthread_t sender_th;
    pthread_t rec_th;
    t_client cur_client = {
        .adr = adr,
        .cl_socket = fd,
        .name = login
    };
    pthread_create(&sender_th, NULL, sender_func, &cur_client);
    pthread_create(&rec_th, NULL, rec_func, &fd);
    pthread_join(sender_th, NULL);
    pthread_join(rec_th, NULL);

    close(fd);

    return 0;
}
