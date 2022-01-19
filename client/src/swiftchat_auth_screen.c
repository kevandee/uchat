#include "../inc/uch_client.h"

t_client cur_client;

void load_css_auth(GtkCssProvider *provider, GtkWidget *widget, gint widg)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    if(widg == 0)
    {
        gtk_style_context_add_class(context,"AUTH_window");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 1)
    {
        gtk_style_context_add_class(context,"AUTH_main_box");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 2)
    {
        gtk_style_context_add_class(context,"AUTH_logo_box");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 3)
    {
        gtk_style_context_add_class(context,"AUTH_button");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 4)
    {
        gtk_style_context_add_class(context,"AUTH_text_under_logo");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 5)
    {
        gtk_style_context_add_class(context,"AUTH_text_next_logo");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);  
    }
    else if(widg == 6)
    {
        gtk_style_context_add_class(context,"AUTH_create_account_text");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);  
    }
    else if(widg == 7)
    {
        gtk_style_context_add_class(context,"AUTH_create_account_button");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);  
    }
    else if(widg == 8)
    {
        gtk_style_context_add_class(context,"AUTH_button_box");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else if(widg == 9)
    {
        gtk_style_context_add_class(context,"AUTH_entry_field1");
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    else
    {
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
}

static void register_button_click(GtkWidget *widget, gpointer data)
{
    GtkWidget **entry_field = data;
    GtkEntryBuffer *login_field_buf = gtk_entry_get_buffer(GTK_ENTRY (entry_field[0]));
    GtkEntryBuffer *password_field_buf1 = gtk_entry_get_buffer(GTK_ENTRY (entry_field[1]));
    GtkEntryBuffer *password_field_buf2 = gtk_entry_get_buffer(GTK_ENTRY (entry_field[2]));

    int login_len = gtk_entry_buffer_get_length(login_field_buf);
    int password_len = gtk_entry_buffer_get_length(password_field_buf1);

    if (login_len < 6 || login_len > 20) {
        gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "LOGIN MUST CONTAIN 6-20 SYMBOLS");
        gtk_widget_show(t_auth.ErrorMessage);
        return;
    }

    if (password_len < 8 || password_len > 16) {
        gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "LOGIN MUST CONSIST OF 8-16 SYMBOLS");
        gtk_widget_show(t_auth.ErrorMessage);
        return;
    }
    
    cur_client.login = mx_strdup(gtk_entry_buffer_get_text(login_field_buf));
    const char *password_str_1 = gtk_entry_buffer_get_text(password_field_buf1);
    const char *password_str_2 = gtk_entry_buffer_get_text(password_field_buf2);

    if (mx_strcmp(password_str_1, password_str_2) != 0) {
        gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "PASSWORDS DOESN'T MATCH");
        gtk_widget_show(t_auth.ErrorMessage);
        return;
    }

    cur_client.passwd = mx_strdup(password_str_1);
    int status = check_auth_input(cur_client.login);
    switch(status) {
        case 0:
            break;
        case -1:{
            // несколько точек подряд
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "LOGIN: A LOT OF '.' SYMBOLS NEAR");
            gtk_widget_show(t_auth.ErrorMessage); 
            return;
        }
        case -2:{
            // точка в начале или конце
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "LOGIN: '.' SYMBOL WHETHER AT THE START OR AT THE END");
            gtk_widget_show(t_auth.ErrorMessage); 
            return;
        }
        default:{
            // запрещённый символ, переменная status == этому символу
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "LOGIN: FORBIDDEN SYMBOL");
            gtk_widget_show(t_auth.ErrorMessage); 
            return;
        }
    }

    status = check_auth_input(cur_client.passwd);
    switch(status) {
        case 0:
            break;
        case -1:{
            // несколько точек подряд
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "PASSWORDS: A LOT OF '.' SYMBOLS NEAR");
            gtk_widget_show(t_auth.ErrorMessage); 
            return;
        }
        case -2:{
            // точка в начале или конце
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "PASSWORDS: '.' SYMBOL WHETHER AT THE START OR AT THE END");
            gtk_widget_show(t_auth.ErrorMessage); 
            return;
        }
        default:{
            // запрещённый символ, переменная status == этому символу
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "PASSWORDS: FORBIDDEN SYMBOL");
            gtk_widget_show(t_auth.ErrorMessage); 
            return;
        }
    }
    
    char message[32] = {0};
    // Отправка данных для регистрации на сервер
    send(cur_client.serv_fd, "u", 1, 0);
    sprintf(message, "%s", cur_client.login);
    send(cur_client.serv_fd, message, 32, 0);
    clear_message(message, 32);
    sprintf(message, "%s", cur_client.passwd);
    send(cur_client.serv_fd, message, 16, 0);

    bool err_aut;
    recv(cur_client.serv_fd, &err_aut, sizeof(bool), 0); // Ожидание ответа от сервера об успешности регистрации
    
    if (err_aut) {
        gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessage), "REGISTRATION ERROR");
        gtk_widget_show(t_auth.ErrorMessage); 
        mx_printerr("reg err\n");
        return;
    }
    gtk_widget_unparent(widget);
    chat_show_auth_screen();
}

static void register_signin_click(GtkWidget *widget) {
    gtk_widget_unparent(widget);
    chat_show_auth_screen();
}

void chat_show_reg_screen()
{
    GtkWidget* REGISTER_logo_box, *REGISTER_button_box, *REGISTER_back_box;
    GtkWidget* REGISTER_logo, *REGISTER_text_under_logo, * REGISTER_Swiftchat_text, *REGISTER_entry_field1, *REGISTER_entry_field2, *REGISTER_entry_field3, *REGISTER_button, *REGISTER_signin_text, *REGISTER_signin_button;
    gtk_widget_set_name(t_auth.ErrorMessage, "error");
    gtk_widget_set_margin_bottom(t_auth.ErrorMessage, 5);

    t_auth.REGISTRATION_menu = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_auth.REGISTRATION_menu ), 430, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_auth.REGISTRATION_menu), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(t_auth.REGISTRATION_menu ), GTK_ALIGN_CENTER);
    REGISTER_logo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(REGISTER_logo_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(REGISTER_logo_box), GTK_ALIGN_CENTER);
    REGISTER_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(REGISTER_button_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(REGISTER_button_box), GTK_ALIGN_CENTER);
    REGISTER_back_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(REGISTER_back_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(REGISTER_back_box), GTK_ALIGN_CENTER);

    REGISTER_logo = gtk_image_new_from_file("client/media/Logo.png");
    gtk_widget_set_size_request(REGISTER_logo, 47, 47);
    gtk_box_append (GTK_BOX(REGISTER_logo_box), REGISTER_logo);
    REGISTER_Swiftchat_text = gtk_label_new("Swiftchat");
    gtk_widget_set_name(GTK_WIDGET(REGISTER_Swiftchat_text), "login_label");
    gtk_box_append (GTK_BOX(REGISTER_logo_box), REGISTER_Swiftchat_text);
    REGISTER_button = gtk_button_new_with_label ("Sign Up");
    gtk_widget_set_size_request(REGISTER_button, 240, 50);
    gtk_box_append (GTK_BOX(REGISTER_button_box), REGISTER_button);
    REGISTER_signin_text = gtk_label_new("Already have an account? ");
    gtk_box_append (GTK_BOX(REGISTER_back_box), REGISTER_signin_text);
    REGISTER_signin_button = gtk_label_new("Sign in");
    gtk_box_append (GTK_BOX(REGISTER_back_box), REGISTER_signin_button);

    REGISTER_text_under_logo = gtk_label_new("CREATE AN ACCOUNT TO CONTINUE");
    gtk_widget_set_name(GTK_WIDGET(REGISTER_text_under_logo), "login_label");
    gtk_widget_set_margin_bottom(REGISTER_text_under_logo, 40);
    REGISTER_entry_field1 = gtk_entry_new();
    gtk_widget_set_size_request(REGISTER_entry_field1, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(REGISTER_entry_field1),"Enter your username");
    gtk_widget_set_margin_bottom (REGISTER_entry_field1, 10);
    REGISTER_entry_field2 = gtk_entry_new();
    gtk_widget_set_size_request(REGISTER_entry_field2, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(REGISTER_entry_field2),"Enter your password");
    gtk_entry_set_visibility(GTK_ENTRY(REGISTER_entry_field2),FALSE);
    gtk_widget_set_margin_bottom (REGISTER_entry_field2, 10);
    REGISTER_entry_field3 = gtk_entry_new();
    gtk_widget_set_size_request(REGISTER_entry_field3, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(REGISTER_entry_field3),"Repeat your password");
    gtk_entry_set_visibility(GTK_ENTRY(REGISTER_entry_field3),FALSE);

    GtkWidget **entry_arr = (GtkWidget **)malloc(3 * sizeof(GtkWidget *));//{LOGIN_entry_field1, LOGIN_entry_field2};
    entry_arr[0] = REGISTER_entry_field1;
    entry_arr[1] = REGISTER_entry_field2;
    entry_arr[2] = REGISTER_entry_field3;    

    g_signal_connect(REGISTER_button, "clicked", G_CALLBACK(register_button_click), entry_arr);

    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_logo_box);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_text_under_logo);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), t_auth.ErrorMessage);
    gtk_widget_hide(t_auth.ErrorMessage);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_entry_field1);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_entry_field2);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_entry_field3);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_button_box);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_back_box);
    gtk_box_set_spacing (GTK_BOX(t_auth.REGISTRATION_menu ), 0);

    load_css_auth(t_screen.provider, t_screen.main_window, 0);
    load_css_auth(t_screen.provider, t_auth.REGISTRATION_menu , 1);
    load_css_auth(t_screen.provider, REGISTER_logo_box, 2);
    load_css_auth(t_screen.provider, REGISTER_button, 3);
    load_css_auth(t_screen.provider, REGISTER_text_under_logo, 4);
    load_css_auth(t_screen.provider, REGISTER_Swiftchat_text, 5);
    load_css_auth(t_screen.provider, REGISTER_signin_text, 6);
    load_css_auth(t_screen.provider, REGISTER_signin_button, 7);
    load_css_auth(t_screen.provider, REGISTER_button_box, 8);
    load_css_auth(t_screen.provider, t_auth.ErrorMessage, 127);


    gtk_window_set_child(GTK_WINDOW(t_screen.main_window), t_auth.REGISTRATION_menu);
    gtk_widget_show(t_auth.REGISTRATION_menu);

    GtkGesture *click_signin = gtk_gesture_click_new();
    gtk_gesture_set_state(click_signin, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_signin, "pressed", G_CALLBACK(register_signin_click), t_auth.REGISTRATION_menu);
    gtk_widget_add_controller(REGISTER_signin_button, GTK_EVENT_CONTROLLER(click_signin));

}


static void account_button_click(GtkWidget *widget)
{
    if(widget){}
    gtk_widget_unparent(widget);
    chat_show_reg_screen();
}

void chat_show_auth_screen()
{
    GtkWidget *LOGIN_logo_box, *LOGIN_button_box, *LOGIN_create_account_box;
    GtkWidget *LOGIN_button, *LOGIN_logo, *LOGIN_text_next_logo, *LOGIN_text_under_logo, *LOGIN_entry_field1, *LOGIN_entry_field2, *LOGIN_create_account_text, *LOGIN_create_account_button;
    t_auth.ErrorMessage = gtk_label_new("ERROR!");
    
    t_auth.LOGIN_menu = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_auth.LOGIN_menu), 430, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_auth.LOGIN_menu), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(t_auth.LOGIN_menu ), GTK_ALIGN_CENTER);
    LOGIN_logo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); //бокс с логотипом и текстом
    gtk_widget_set_halign(GTK_WIDGET(LOGIN_logo_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(LOGIN_logo_box), GTK_ALIGN_CENTER);
    LOGIN_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); //бокс с кнопкой
    gtk_widget_set_halign(GTK_WIDGET(LOGIN_button_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(LOGIN_button_box), GTK_ALIGN_CENTER);
    LOGIN_create_account_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); //бокс с кнопкой
    gtk_widget_set_halign(GTK_WIDGET(LOGIN_create_account_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(LOGIN_create_account_box), GTK_ALIGN_CENTER);


    LOGIN_logo = gtk_image_new_from_file("client/media/Logo.png");
    gtk_widget_set_size_request(LOGIN_logo, 47, 47);
    gtk_box_append (GTK_BOX(LOGIN_logo_box), LOGIN_logo);
    LOGIN_text_next_logo = gtk_label_new("Swiftchat");
    gtk_widget_set_name(GTK_WIDGET(LOGIN_text_next_logo), "login_label");
    gtk_box_append (GTK_BOX(LOGIN_logo_box), LOGIN_text_next_logo);
    LOGIN_button = gtk_button_new_with_label ("Log in");
    gtk_widget_set_size_request(LOGIN_button, 240, 50);
    gtk_box_append (GTK_BOX(LOGIN_button_box), LOGIN_button);
    LOGIN_create_account_text = gtk_label_new("New here? ");
    gtk_box_append (GTK_BOX(LOGIN_create_account_box), LOGIN_create_account_text);
    LOGIN_create_account_button = gtk_label_new("Create an account");
    gtk_box_append (GTK_BOX(LOGIN_create_account_box), LOGIN_create_account_button);

//-----------------------------------------main_box--------------------------------------------------//
    LOGIN_entry_field1 = gtk_entry_new();
    gtk_widget_set_size_request(LOGIN_entry_field1, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(LOGIN_entry_field1),"Username");
    gtk_widget_set_margin_bottom (LOGIN_entry_field1, 10);
    gtk_widget_set_margin_top (LOGIN_entry_field1, 40);
    LOGIN_entry_field2 = gtk_entry_new();
    gtk_widget_set_size_request(LOGIN_entry_field2, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(LOGIN_entry_field2),"Password");
    gtk_entry_set_visibility(GTK_ENTRY(LOGIN_entry_field2),FALSE);
    LOGIN_text_under_logo= gtk_label_new("LOG IN TO YOUR ACCOUNT TO CONTINUE");
    gtk_widget_set_name(GTK_WIDGET(LOGIN_text_under_logo), "login_label");

    GtkWidget **entry_arr = (GtkWidget **)malloc(2 * sizeof(GtkWidget *));//{LOGIN_entry_field1, LOGIN_entry_field2};
    entry_arr[0] = LOGIN_entry_field1;
    entry_arr[1] = LOGIN_entry_field2;

    g_signal_connect(LOGIN_button, "clicked", G_CALLBACK (send_login), entry_arr);

    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_logo_box);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_text_under_logo);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_entry_field1);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_entry_field2);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_button_box);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_create_account_box);
    gtk_box_set_spacing (GTK_BOX(t_auth.LOGIN_menu ), 0);

//-----------------------------------------main_box--------------------------------------------------//

    load_css_auth(t_screen.provider, t_screen.main_window, 0);
    load_css_auth(t_screen.provider, t_auth.LOGIN_menu, 1);
    load_css_auth(t_screen.provider, LOGIN_logo_box, 2);
    load_css_auth(t_screen.provider, LOGIN_button, 3);
    load_css_auth(t_screen.provider, LOGIN_text_under_logo, 4);
    load_css_auth(t_screen.provider, LOGIN_text_next_logo, 5);
    load_css_auth(t_screen.provider, LOGIN_create_account_text, 6);
    load_css_auth(t_screen.provider, LOGIN_create_account_button, 7);
    load_css_auth(t_screen.provider, LOGIN_button_box, 8);

    gtk_window_set_child (GTK_WINDOW(t_screen.main_window), t_auth.LOGIN_menu);
    GtkGesture *click_create_acc = gtk_gesture_click_new();
    gtk_gesture_set_state(click_create_acc, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_create_acc, "pressed", G_CALLBACK(account_button_click), t_auth.LOGIN_menu);
    gtk_widget_add_controller(LOGIN_create_account_button, GTK_EVENT_CONTROLLER(click_create_acc));

}
