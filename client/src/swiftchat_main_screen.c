#include "../inc/uch_client.h"

static void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

static void return_to_chatlist() {
    //gtk_widget_hide(gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left)));
    
    t_main.scroll_box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box_left), 323, 0);

    t_list *chats = cur_client.chats;
    while (chats) {
        add_chat_node(chats->data);
        chats = chats->next;
    }
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left),t_main.scroll_box_left);
}

static void send_chat(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkWidget *name_entry = GTK_WIDGET (data);
    cur_client.sender_new_chat = true;
    int count_chats = cur_client.chat_count;
    const char *name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(name_entry)));

    t_list *temp = t_main.check_buttons_user_list;
    t_list *result_users = NULL;
    while (temp) {
        GtkCheckButton *button = GTK_CHECK_BUTTON (temp->data);
        if (gtk_check_button_get_active(button)) {
            mx_push_back(&result_users, mx_strdup(gtk_check_button_get_label(button)));
        }

        temp = temp->next;
    }

    char *users = NULL;
    temp = result_users;
    while (temp) {
        users = mx_strrejoin(users, temp->data);
        users = mx_strrejoin(users, " ");

        temp=temp->next;
    }

    char buf[512 + 32] = {0};
    sprintf(buf, "<add chat, name=%s>%s", name, users);
    send_all(cur_client.serv_fd, buf, 512+32);
    while (count_chats == cur_client.chat_count) {
        usleep(100);
    }
    return_to_chatlist();
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
    printf("gets search list\n");
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
    mx_clear_list(&t_main.check_buttons_user_list);
    t_main.check_buttons_user_list = NULL;
    while (temp) {
        if (mx_strcmp(cur_client.login, temp->data) != 0) {
            GtkWidget *button = gtk_check_button_new_with_label(temp->data);
            gtk_box_append(GTK_BOX (scroll_box), button);
            mx_push_back(&t_main.check_buttons_user_list, button);
        }
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

    g_signal_connect(button_success, "clicked", G_CALLBACK(send_chat), chat_name_entry);

    gtk_box_append(GTK_BOX (buttons_box), button_exit);
    gtk_box_append(GTK_BOX (buttons_box), button_success);

    gtk_box_append(GTK_BOX (add_chat_box), add_chat_label);
    gtk_box_append(GTK_BOX(add_chat_box), chat_name_box);
    gtk_box_append(GTK_BOX(add_chat_box), user_search_entry);
    gtk_box_append(GTK_BOX(add_chat_box), users_list_box);
    gtk_box_append(GTK_BOX(add_chat_box), buttons_box);
    gtk_widget_show(add_chat_box);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left), add_chat_box);
}


static void key_press(GtkWidget *entry) {
    GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY (entry));
    const char *entry_text = gtk_entry_buffer_get_text (buf);
    if (!entry_text) {
        return;
    }
    char message[512 + 32] = {0};

    sprintf(message, "<msg, chat_id= %d>%s", cur_client.cur_chat.id, entry_text);

    send(cur_client.serv_fd, message, 512+32, 0);
    gtk_entry_buffer_set_text(buf, "", 0);
}

void show_chat_history(GtkWidget *widget, gpointer data)
{
    (void)widget;
    printf("id of chat %d\n", ((t_chat *)data)->id);
    cur_client.cur_chat = *((t_chat *)data);

    gtk_grid_remove(GTK_GRID(t_main.grid), t_main.right_panel);
    t_main.right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(GTK_WIDGET(t_main.right_panel), 18);

    t_main.scroll_box_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    t_main.scrolled_window_right = gtk_scrolled_window_new ();
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_right), 730, 680);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_right), t_main.scroll_box_right);
    gtk_widget_set_name(GTK_WIDGET(t_main.scrolled_window_right), "left_scroll");
    load_css_main(t_screen.provider, t_main.scrolled_window_right);

    GtkWidget *write_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(write_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(write_box), GTK_ALIGN_CENTER);
    GtkWidget *write_message = gtk_entry_new();
    gtk_widget_set_size_request(write_message, 700, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(write_message),"Write a message...");
    gtk_box_append(GTK_BOX(write_box), write_message);

    g_signal_connect(write_message, "activate", G_CALLBACK(key_press), NULL);

    gtk_box_append(GTK_BOX(t_main.right_panel), t_main.scrolled_window_right);
    gtk_box_append(GTK_BOX(t_main.right_panel), write_box);
    gtk_box_set_spacing (GTK_BOX(t_main.right_panel), 5);

    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);
}

void chat_show_main_screen(GtkWidget *window) 
{
    gtk_css_provider_load_from_path(t_screen.provider,"client/themes/dark_chat.css");
    t_main.grid = gtk_grid_new();

    t_main.left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.left_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.left_panel), GTK_ALIGN_END);
    gtk_widget_set_name(GTK_WIDGET(t_main.left_panel), "left_panel");
    load_css_main(t_screen.provider, t_main.left_panel);
    t_main.right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.right_panel), 720, 100);
    gtk_widget_set_name(GTK_WIDGET(t_main.right_panel), "right_panel");
    load_css_main(t_screen.provider, t_main.right_panel);
    gtk_widget_set_margin_start(GTK_WIDGET(t_main.right_panel), 18);
    GtkWidget *choose_friend = gtk_label_new("Choose someone to write...");
    gtk_box_append(GTK_BOX(t_main.right_panel), choose_friend);
//-----------------------------------------------SearcPanel---------------------------------------------------------------
    GtkWidget *SearchBox, *Logo, *SearchField, *Home, *Settings;
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

    t_main.scrolled_window_left = gtk_scrolled_window_new ();
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_left), 323, 560);
    t_main.scroll_box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box_left), 323, 0);
    

    t_list *chats = cur_client.chats;
    while (chats) {
        add_chat_node(chats->data);
        chats = chats->next;
    }
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left), t_main.scroll_box_left);

    gtk_box_append(GTK_BOX(t_main.left_panel), GTK_WIDGET(t_main.search_panel));
    gtk_box_append(GTK_BOX(t_main.left_panel), GTK_WIDGET(recent_box));
    gtk_box_append(GTK_BOX(t_main.left_panel), GTK_WIDGET(t_main.scrolled_window_left));
    gtk_box_set_spacing (GTK_BOX(t_main.left_panel), 0);
        
    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.left_panel, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);

    gtk_window_set_child(GTK_WINDOW(window), t_main.grid);

    gtk_widget_show(window);
}
