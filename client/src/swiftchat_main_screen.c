#include "../inc/uch_client.h"

static void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

static void return_to_chatlist() {
    //gtk_widget_hide(gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window)));
    t_main.scroll_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box), 323, 0);

    t_list *chats = cur_client.chats;
    while (chats) {
        add_chat_node(chats->data);
        chats = chats->next;
    }
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window),t_main.scroll_box);
}

static void add_chat_dialog(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    mx_clear_list(&t_main.search_users_list);
    t_main.search_users_list = NULL;
    char message[512+32] = {0};
    sprintf(message, "<users list>");
    send(cur_client.serv_fd, message, 512+32, 0);
    while (!t_main.search_users_list)
        usleep(100);
    //gtk_widget_hide(t_main.scroll_box);
    GtkWidget *add_chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(add_chat_box, 300, 20);

    GtkWidget *add_chat_label = gtk_label_new("New Chat");
    
    GtkWidget *chat_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 30);
    gtk_widget_set_halign(GTK_WIDGET(chat_name_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(chat_name_box), GTK_ALIGN_CENTER);
    GtkWidget *chat_name_label = gtk_label_new("Name of new chat: ");
    GtkWidget *chat_name_entry = gtk_entry_new();
    
    gtk_box_append(GTK_BOX (chat_name_box), chat_name_label);
    gtk_box_append(GTK_BOX (chat_name_box), chat_name_entry);

    GtkWidget *user_search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(user_search_entry), "Who would you like to add?");


    //------users list-------
    
    GtkWidget *users_list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(users_list_box, 300, 400);
    GtkWidget *scroll_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_halign(GTK_WIDGET(scroll_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(scroll_box), GTK_ALIGN_CENTER);
    
    pthread_mutex_lock(&cl_mutex);

    t_list *temp = t_main.search_users_list;
    while (temp) {
        GtkWidget *button = gtk_check_button_new_with_label(temp->data);
        gtk_box_append(GTK_BOX (scroll_box), button);
        temp = temp->next;
    }
    
    pthread_mutex_unlock(&cl_mutex);
    
    GtkWidget *scrolled_tree = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_tree), scroll_box);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_tree), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_set_size_request(scrolled_tree, 300, 400);
    gtk_box_append(GTK_BOX(users_list_box), scrolled_tree);
    //-----------------------

    GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);
    gtk_widget_set_size_request(buttons_box, 150, 20);
    gtk_widget_set_halign(GTK_WIDGET(buttons_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(buttons_box), GTK_ALIGN_CENTER);
    GtkWidget *button_exit = gtk_button_new_with_label("exit");
    g_signal_connect(button_exit, "clicked", G_CALLBACK(return_to_chatlist), NULL);
    GtkWidget *button_success = gtk_button_new_with_label("add chat");
    gtk_box_append(GTK_BOX (buttons_box), button_exit);
    gtk_box_append(GTK_BOX (buttons_box), button_success);

    gtk_box_append(GTK_BOX (add_chat_box), add_chat_label);
    gtk_box_append(GTK_BOX(add_chat_box), chat_name_box);
    gtk_box_append(GTK_BOX(add_chat_box), user_search_entry);
    gtk_box_append(GTK_BOX(add_chat_box), users_list_box);
    gtk_box_append(GTK_BOX(add_chat_box), buttons_box);
    gtk_widget_show(add_chat_box);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window), add_chat_box);
}

void chat_show_main_screen(GtkWidget *window) 
{
    GtkWidget *main_box;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(main_box), 323, 700);
    gtk_widget_set_halign(GTK_WIDGET(main_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(main_box), GTK_ALIGN_END);
    gtk_widget_set_name(GTK_WIDGET(main_box), "left_panel");
    load_css_main(t_screen.provider, main_box);
    GtkWidget *Logo, *SearchBox, *SearchField, *Home, *Settings;
//-----------------------------------------------SearcPanel---------------------------------------------------------------
    t_main.search_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(GTK_WIDGET(t_main.search_panel), 40);
    SearchBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(SearchBox), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(SearchBox), GTK_ALIGN_CENTER);

    Logo = get_circle_widget_from_png("test_circle.png");
    gtk_widget_set_size_request(GTK_WIDGET(Logo), 10, 10);
    SearchField = gtk_entry_new();
    gtk_widget_set_name(GTK_WIDGET(SearchField), "search_field");
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER (gtk_entry_get_buffer(GTK_ENTRY(SearchField))), 20);
    gtk_widget_set_size_request(SearchField, 310, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(SearchField), "Search");
    load_css_main(t_screen.provider, SearchField);
    gtk_box_append(GTK_BOX(SearchBox), GTK_WIDGET(SearchField));
    GFile *path = g_file_new_for_path("client/media/search_ico.png"); 
    GIcon *icon = g_file_icon_new(path);
    gtk_entry_set_icon_from_gicon(GTK_ENTRY(SearchField),GTK_ENTRY_ICON_SECONDARY, icon);
    Home = gtk_image_new_from_file("client/media/home.png");
    gtk_widget_set_size_request(GTK_WIDGET(Home), 10, 10);
    Settings = gtk_image_new_from_file("client/media/settings.png");
    gtk_widget_set_size_request(GTK_WIDGET(Settings), 10, 10);
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Logo));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(SearchBox));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Home));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Settings));
    gtk_box_set_spacing (GTK_BOX(t_main.search_panel), 8);
//------------------------------------------------------------------------------------------------------------------------
    
    GtkWidget *recent_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(recent_box, GTK_ALIGN_START);
    gtk_widget_set_valign(recent_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(recent_box, 40);

    GtkWidget *recent_label = gtk_label_new("Recent");
    gtk_widget_set_size_request(recent_label, 300, 0);
    gtk_widget_set_halign(recent_label, GTK_ALIGN_START);
    gtk_widget_set_valign(recent_label, GTK_ALIGN_CENTER);

    GtkWidget *add_chat_button = gtk_button_new_with_label("+");
    gtk_widget_set_margin_start(GTK_WIDGET(add_chat_button), 80);
    gtk_widget_set_halign(add_chat_button, GTK_ALIGN_END);
    gtk_widget_set_valign(add_chat_button, GTK_ALIGN_CENTER);

    g_signal_connect(add_chat_button, "clicked", G_CALLBACK (add_chat_dialog), NULL);

    gtk_box_append(GTK_BOX(recent_box), GTK_WIDGET(recent_label));
    gtk_box_append(GTK_BOX(recent_box), GTK_WIDGET(add_chat_button));

//------------------------------------------------------------------------------------------------------------------------

    t_main.scrolled_window = gtk_scrolled_window_new ();
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window), 323, 600);
    t_main.scroll_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box), 323, 0);
    

    t_list *chats = cur_client.chats;
    while (chats) {
        add_chat_node(chats->data);
        chats = chats->next;
    }
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window),t_main.scroll_box);

    gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(t_main.search_panel));
    gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(recent_box));
    gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(t_main.scrolled_window));
    gtk_box_set_spacing (GTK_BOX(main_box), 0);

    gtk_window_set_child(GTK_WINDOW(window), main_box);

    gtk_widget_show(window);
}
