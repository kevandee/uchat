#include "../inc/uch_client.h"

void load_css_auth(GtkCssProvider *provider, GtkWidget *widget, gint widg)
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

static void register_button_click(GtkWidget *widget)
{
    if(widget){}

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider,"client/style.css");
    gtk_widget_unparent(widget);
    chat_show_auth_screen();
    gtk_window_set_child(GTK_WINDOW(t_screen.main_window), t_auth.LOGIN_menu);
}

void chat_show_reg_screen()
{
    GtkWidget* REGISTER_logo_box, *REGISTER_button_box, *REGISTER_back_box;
    GtkWidget* REGISTER_logo, *REGISTER_text_under_logo, * REGISTER_Swiftchat_text, *REGISTER_entry_field1, *REGISTER_entry_field2, *REGISTER_entry_field3, *REGISTER_button, *REGISTER_signin_text, *REGISTER_signin_button;

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

    REGISTER_logo = gtk_image_new_from_file("client/Logo.png");
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
    REGISTER_entry_field1 = gtk_entry_new();
    gtk_widget_set_size_request(REGISTER_entry_field1, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(REGISTER_entry_field1),"Enter your username");
    gtk_widget_set_margin_bottom (REGISTER_entry_field1, 10);
    gtk_widget_set_margin_top (REGISTER_entry_field1, 40);
    REGISTER_entry_field2 = gtk_entry_new();
    gtk_widget_set_size_request(REGISTER_entry_field2, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(REGISTER_entry_field2),"Enter your password");
    gtk_entry_set_visibility(GTK_ENTRY(REGISTER_entry_field2),FALSE);
    gtk_widget_set_margin_bottom (REGISTER_entry_field2, 10);
    REGISTER_entry_field3 = gtk_entry_new();
    gtk_widget_set_size_request(REGISTER_entry_field3, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(REGISTER_entry_field3),"Repeat your password");
    gtk_entry_set_visibility(GTK_ENTRY(REGISTER_entry_field3),FALSE);

    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_logo_box);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_text_under_logo);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_entry_field1);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_entry_field2);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_entry_field3);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_button_box);
    gtk_box_append (GTK_BOX(t_auth.REGISTRATION_menu ), REGISTER_back_box);
    gtk_box_set_spacing (GTK_BOX(t_auth.REGISTRATION_menu ), 0);

    load_css_auth(t_screen.provider, t_auth.REGISTRATION_menu , 1);
    load_css_auth(t_screen.provider, REGISTER_logo_box, 2);
    load_css_auth(t_screen.provider, REGISTER_button, 3);
    load_css_auth(t_screen.provider, REGISTER_text_under_logo, 4);
    load_css_auth(t_screen.provider, REGISTER_Swiftchat_text, 5);
    load_css_auth(t_screen.provider, REGISTER_signin_text, 6);
    load_css_auth(t_screen.provider, REGISTER_signin_button, 7);
    load_css_auth(t_screen.provider, REGISTER_button_box, 8);


    gtk_window_set_child(GTK_WINDOW(t_screen.main_window), t_auth.REGISTRATION_menu);
    gtk_widget_show(t_auth.REGISTRATION_menu);

    GtkGesture *click_signin = gtk_gesture_click_new();
    gtk_gesture_set_state(click_signin, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_signin, "pressed", G_CALLBACK(register_button_click), t_auth.REGISTRATION_menu);
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


    LOGIN_logo = gtk_image_new_from_file("client/Logo.png");
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
    gtk_entry_set_placeholder_text(GTK_ENTRY(LOGIN_entry_field1),"Your login...");
    gtk_widget_set_margin_bottom (LOGIN_entry_field1, 10);
    gtk_widget_set_margin_top (LOGIN_entry_field1, 40);
    LOGIN_entry_field2 = gtk_entry_new();
    gtk_widget_set_size_request(LOGIN_entry_field2, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(LOGIN_entry_field2),"Your password...");
    gtk_entry_set_visibility(GTK_ENTRY(LOGIN_entry_field2),FALSE);
    LOGIN_text_under_logo= gtk_label_new("LOG IN TO YOUR ACCOUNT TO CONTINUE");
    gtk_widget_set_name(GTK_WIDGET(LOGIN_text_under_logo), "login_label");

    GtkWidget **entry_arr = (GtkWidget **)malloc(3 * sizeof(GtkWidget *));//{LOGIN_entry_field1, LOGIN_entry_field2};
    entry_arr[0] = LOGIN_entry_field1;
    entry_arr[1] = LOGIN_entry_field2;
    entry_arr[2] = t_screen.main_window;

    g_signal_connect(LOGIN_button, "clicked", G_CALLBACK (send_login), entry_arr);

    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_logo_box);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_text_under_logo);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_entry_field1);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_entry_field2);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_button_box);
    gtk_box_append (GTK_BOX(t_auth.LOGIN_menu ), LOGIN_create_account_box);
    gtk_box_set_spacing (GTK_BOX(t_auth.LOGIN_menu ), 0);

//-----------------------------------------main_box--------------------------------------------------//

    load_css_auth(t_screen.provider, t_auth.LOGIN_menu, 1);
    load_css_auth(t_screen.provider, LOGIN_logo_box, 2);
    load_css_auth(t_screen.provider, LOGIN_button, 3);
    load_css_auth(t_screen.provider, LOGIN_text_under_logo, 4);
    load_css_auth(t_screen.provider, LOGIN_text_next_logo, 5);
    load_css_auth(t_screen.provider, LOGIN_create_account_text, 6);
    load_css_auth(t_screen.provider, LOGIN_create_account_button, 7);
    load_css_auth(t_screen.provider, LOGIN_button_box, 8);

    GtkGesture *click_create_acc = gtk_gesture_click_new();
    gtk_gesture_set_state(click_create_acc, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_create_acc, "pressed", G_CALLBACK(account_button_click), t_auth.LOGIN_menu);
    gtk_widget_add_controller(LOGIN_create_account_button, GTK_EVENT_CONTROLLER(click_create_acc));

}
