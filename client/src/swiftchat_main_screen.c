#include "../inc/uch_client.h"

static void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void return_to_chatlist(GtkWidget *widget, gpointer data) {
    //gtk_widget_hide(gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left)));
    (void)widget;
    if(data != NULL)
    {
        GtkWidget** swapped = data;
        gtk_label_set_label(GTK_LABEL(swapped[0]), "Recent");
        gtk_box_remove (GTK_BOX(swapped[2]), swapped[1]);
        swapped[1] = NULL;
        GtkWidget *goback = gtk_button_new_with_label("+");
        gtk_widget_set_name(goback, "add_chat");
        load_css_main(t_screen.provider, goback);
        gtk_widget_set_margin_start(GTK_WIDGET(goback), 210);
        gtk_widget_set_halign(goback, GTK_ALIGN_END);
        gtk_widget_set_valign(goback, GTK_ALIGN_CENTER);
        swapped[1] = GTK_WIDGET(goback);
        gtk_box_append(GTK_BOX(swapped[2]), goback);
        g_signal_connect(goback, "clicked", G_CALLBACK(add_chat_dialog), (gpointer)swapped);
    }
    t_main.scroll_box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box_left), 310, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_left), 310, 590);

    t_list *chats = cur_client.chats;
    while (chats) {
        add_chat_node(chats->data);
        chats = chats->next;
    }
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left),t_main.scroll_box_left);
}

static void send_chat(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkWidget **swapped = data;
    GtkWidget *name_entry = GTK_WIDGET (swapped[3]);
    cur_client.sender_new_chat = true;
    const char *name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(name_entry)));

    t_list *temp = t_main.check_buttons_user_list;
    t_list *result_users = NULL;
    while (temp) {
        GtkCheckButton *button = GTK_CHECK_BUTTON (temp->data);
        if (gtk_check_button_get_active(button)) {
            mx_push_back(&result_users, mx_strdup(gtk_widget_get_name(GTK_WIDGET(button))));
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

    t_main.loaded = false;
    char buf[512 + 32] = {0};
    sprintf(buf, "<add chat, name=%s>%s", name, users);
    send_all(cur_client.serv_fd, buf, 512+32);
    while (!t_main.loaded) {
        usleep(50);
    }
    return_to_chatlist(widget, swapped);
}

void add_chat_dialog(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkWidget** swapped = data;
    gtk_label_set_label(GTK_LABEL(swapped[0]), "New Group");
    gtk_box_remove (GTK_BOX(swapped[2]), swapped[1]);
    swapped[1] = NULL;
    GtkWidget *close = gtk_button_new_with_label("×");
    gtk_widget_set_name(close, "add_chat");
    load_css_main(t_screen.provider, close);
    gtk_widget_set_margin_start(GTK_WIDGET(close), 160);
    gtk_widget_set_halign(close, GTK_ALIGN_END);
    gtk_widget_set_valign(close, GTK_ALIGN_CENTER);
    swapped[1] = GTK_WIDGET(close);
    gtk_box_append(GTK_BOX(swapped[2]), close);
    g_signal_connect(close, "clicked", G_CALLBACK(return_to_chatlist), (gpointer)swapped);

    mx_clear_list(&t_main.search_users_list);
    t_main.search_users_list = NULL;
    t_main.loaded = false;
    char message[512+32] = {0};
    sprintf(message, "<users list>");
    send(cur_client.serv_fd, message, 512+32, 0);
    while (!t_main.loaded)
        usleep(50);
    printf("gets search list\n");
    //gtk_widget_hide(t_main.scroll_box);
    GtkWidget *add_chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_name(add_chat_box, "chat_box");
    load_css_main(t_screen.provider, add_chat_box);
    gtk_widget_set_size_request(add_chat_box, 300, 20);
    gtk_widget_set_halign(add_chat_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(add_chat_box, GTK_ALIGN_CENTER);
    
    GtkWidget *chat_name_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(chat_name_box, GTK_ALIGN_START);
    gtk_widget_set_valign(chat_name_box, GTK_ALIGN_START);
    gtk_box_set_spacing(GTK_BOX(chat_name_box), 5);
    GtkWidget *chat_name_label = gtk_label_new("GROUP NAME");
    gtk_widget_set_name(GTK_WIDGET(chat_name_label), "create_chat_group_name_text");
    load_css_main(t_screen.provider, chat_name_label);
    GtkWidget *chat_name_entry_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(chat_name_entry_box, GTK_ALIGN_START);
    gtk_widget_set_valign(chat_name_entry_box, GTK_ALIGN_START);
    GtkWidget *chat_name_entry = gtk_entry_new();
    gtk_widget_set_size_request(chat_name_entry, 308,20);
    gtk_widget_set_name(GTK_WIDGET(chat_name_entry), "input_field_add_group");
    load_css_main(t_screen.provider, chat_name_entry);
    //gtk_box_append(GTK_BOX(chat_name_entry_box), chat_name_entry);
    
    gtk_box_append(GTK_BOX (chat_name_box), chat_name_label);
    gtk_box_append(GTK_BOX (chat_name_entry_box), chat_name_entry);

    GtkWidget *add_member_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(add_member_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(add_member_box), GTK_ALIGN_START);
    gtk_box_set_spacing(GTK_BOX(add_member_box), 5);
    GtkWidget *add_member_name = gtk_label_new("ADD MEMEBER");
    gtk_widget_set_name(GTK_WIDGET(add_member_name), "add_member_text");
    load_css_main(t_screen.provider, add_member_name);
    GtkWidget *add_member_input_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(add_member_input_box, GTK_ALIGN_START);
    gtk_widget_set_valign(add_member_input_box, GTK_ALIGN_START);
    GtkWidget *add_member_input = gtk_entry_new();
    gtk_widget_set_size_request(add_member_input, 308,20);
    gtk_widget_set_name(GTK_WIDGET(add_member_input), "input_field_add_group");
    load_css_main(t_screen.provider, add_member_input);
    GFile *path = g_file_new_for_path("client/media/search_ico_grey.png"); 
    GIcon *icon = g_file_icon_new(path);
    gtk_entry_set_icon_from_gicon(GTK_ENTRY(add_member_input),GTK_ENTRY_ICON_SECONDARY, icon);

    gtk_box_append(GTK_BOX (add_member_box), add_member_name);
    gtk_box_append(GTK_BOX(add_member_input_box), add_member_input);

    //gtk_entry_set_placeholder_text(GTK_ENTRY(user_search_entry), "Who would you like to add?");

    //------users list-------
    
    GtkWidget *users_list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(users_list_box, 300, 200);
    GtkWidget *scroll_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_name(GTK_WIDGET(scroll_box), "scroll_add_member_box");
    load_css_main(t_screen.provider, scroll_box);
    gtk_widget_set_halign(GTK_WIDGET(scroll_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(scroll_box), GTK_ALIGN_START);

    /*GtkWidget *chat_image1 = get_circle_widget_from_png_custom("test_circle.png", 54, 54);
    gtk_widget_set_size_request(GTK_WIDGET(chat_image1),  54, 0);
    gtk_widget_set_halign(GTK_WIDGET(chat_image1), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(chat_image1), GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX(scroll_box), chat_image1);*/
    
    pthread_mutex_lock(&cl_mutex);

    t_list *temp = t_main.search_users_list;
    mx_clear_list(&t_main.check_buttons_user_list);
    t_main.check_buttons_user_list = NULL;
    GtkWidget *users_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(users_grid), 5);

    int row = 0;
    while (temp) {
        if (mx_strcmp(cur_client.login, temp->data) != 0) 
        {
            GtkWidget *user_img_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            GtkWidget *user_img = get_circle_widget_from_png_custom("test_circle.png", 45, 45);
            gtk_box_append(GTK_BOX(user_img_box), user_img);
            GtkWidget *user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_widget_set_margin_start(user_name_box, 10);
            GtkWidget *user_name = gtk_label_new(temp->data);
            gtk_widget_set_name(user_name, "users_check_boxes");
            load_css_main(t_screen.provider, user_name);
            gtk_box_append(GTK_BOX(user_name_box), user_name);
            GtkWidget *user_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_widget_set_margin_start(user_button_box, 150);
            GtkWidget *user_button = gtk_check_button_new();
            gtk_widget_set_name(GTK_WIDGET(user_button), gtk_label_get_text(GTK_LABEL(user_name)));
            load_css_main(t_screen.provider, user_button);
            gtk_box_append(GTK_BOX (user_button_box), user_button);

            gtk_grid_attach(GTK_GRID(users_grid), user_img_box, 0, row, 1, 1);
            gtk_grid_attach(GTK_GRID(users_grid), user_name_box, 1, row, 1, 1);
            gtk_grid_attach(GTK_GRID(users_grid), user_button_box, 2, row, 1, 1);
            
            mx_push_back(&t_main.check_buttons_user_list, user_button);
            row++;
        }
        temp = temp->next;
    }
    gtk_box_append(GTK_BOX (scroll_box), users_grid);
    /*while (temp) {
        if (mx_strcmp(cur_client.login, temp->data) != 0) {
            GtkWidget *button = gtk_check_button_new_with_label(temp->data);
            gtk_widget_set_name(GTK_WIDGET(button), "check_button");
            load_css_main(t_screen.provider, button);
            gtk_box_append(GTK_BOX (scroll_box), button);
            mx_push_back(&t_main.check_buttons_user_list, button);
        }
        temp = temp->next;
    }*/
    
    pthread_mutex_unlock(&cl_mutex);

   
    
    GtkWidget *scrolled_tree = gtk_scrolled_window_new();
    gtk_widget_set_name(GTK_WIDGET(scrolled_tree), "add_member_scrolled");
    load_css_main(t_screen.provider, scrolled_tree);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (scrolled_tree), scroll_box);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_tree), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_set_size_request(scrolled_tree, 300, 350);
    gtk_box_append(GTK_BOX(users_list_box), scrolled_tree);
    //-----------------------
    g_signal_connect(add_member_input, "notify::text", G_CALLBACK (text_changed_add_chat), scrolled_tree);

    GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(buttons_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(buttons_box), GTK_ALIGN_CENTER);    
    gtk_widget_set_margin_start(buttons_box, 7);
    GtkWidget *button_success = gtk_button_new_with_label("Submit");
    gtk_widget_set_name(button_success, "submit_button");
    load_css_main(t_screen.provider, button_success);
    gtk_widget_set_size_request(button_success, 180, 26);
    swapped[3] = chat_name_entry;
    g_signal_connect(button_success, "clicked", G_CALLBACK(send_chat), swapped);
    gtk_box_append(GTK_BOX (buttons_box), button_success);

    gtk_box_append(GTK_BOX (add_chat_box), chat_name_box);
    gtk_box_append(GTK_BOX (add_chat_box), chat_name_entry_box);
    gtk_box_append(GTK_BOX (add_chat_box), add_member_box);
    gtk_box_append(GTK_BOX (add_chat_box), add_member_input_box);
    gtk_box_append(GTK_BOX(add_chat_box), users_list_box);
    gtk_box_append(GTK_BOX(add_chat_box), buttons_box);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left), add_chat_box);
}


static void return_controll_func(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    (void)controller;
    (void)keycode;
    (void)state;

    if (keyval == GDK_KEY_Return) 
    {
        GtkWidget *entry = user_data;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry));
        GtkTextIter start, end ;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        //gint n_chars = gtk_text_buffer_get_char_count (buffer);

        const char *buf_str = gtk_text_buffer_get_text(buffer, &start, &end, true);

        if (!buf_str || buf_str[0] == '\n') {
            return;
        }
        char *str = mx_strnew(1024);
        str = mx_strncpy(str, buf_str, mx_strlen(buf_str) - 1);
        char message[512 + 32] = {0};
        sprintf(message, "<msg, chat_id= %d>%s", cur_client.cur_chat.id, str);

        GtkWidget *my_msg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        
        gtk_widget_set_halign(GTK_WIDGET(my_msg_box), GTK_ALIGN_END);
        gtk_widget_set_valign(GTK_WIDGET(my_msg_box), GTK_ALIGN_END);
        gtk_widget_set_margin_end(my_msg_box, 5);
        gtk_widget_set_margin_bottom(my_msg_box, 5);
        GtkWidget *msg = gtk_label_new(str);
        gtk_widget_set_name(GTK_WIDGET(msg), "message");
        load_css_main(t_screen.provider, msg);
        gtk_label_set_wrap(GTK_LABEL(msg), TRUE);
        gtk_label_set_wrap_mode(GTK_LABEL(msg), PANGO_WRAP_WORD_CHAR);
        gtk_label_set_max_width_chars(GTK_LABEL(msg), 50);
        gtk_label_set_selectable(GTK_LABEL(msg), TRUE);

        gtk_box_append(GTK_BOX(my_msg_box), msg);
        if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0) {
            GtkWidget *user_logo = get_circle_widget_current_user_avatar();
            gtk_box_append(GTK_BOX(my_msg_box), user_logo);
        }
        gtk_box_append(GTK_BOX(t_main.scroll_box_right), my_msg_box);

        GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW(t_main.scrolled_window_right));
        //double to_this_pos = gtk_adjustment_get_upper(GTK_ADJUSTMENT(adj)) - gtk_adjustment_get_page_size(GTK_ADJUSTMENT(adj));
        gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), gtk_adjustment_get_upper(GTK_ADJUSTMENT(adj)) + 40);
        gtk_scrolled_window_set_vadjustment (GTK_SCROLLED_WINDOW(t_main.scrolled_window_right),GTK_ADJUSTMENT(adj));



        send(cur_client.serv_fd, message, 512+32, 0);
        gtk_text_buffer_set_text (buffer, "", 0);
    }
}

static void insert_text_bio(GtkTextBuffer *buffer, GtkTextIter *location)
{
    gint count=gtk_text_buffer_get_char_count(buffer);

    GtkTextIter  start, end, offset;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    //const char *buf_str = gtk_text_buffer_get_text(buffer, &start, &end, true);
    //gtk_text_buffer_get_start_iter(buffer, &start);
    //gtk_text_buffer_get_end_iter(buffer, &end);

    
    if(count>512) {
        gtk_text_buffer_get_iter_at_offset(buffer, &offset, 512);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_delete(buffer, &offset, &end);
        gtk_text_iter_assign(location, &offset);
    }
}

static void show_stickers()
{

}

void show_chat_history(GtkWidget *widget, gpointer data)
{
    cairo_surface_t *image = get_surface_from_jpg(cur_client.avatar.path);
    int org_width = cairo_image_surface_get_width(image);
    int org_height = cairo_image_surface_get_height(image);
    
    cur_client.avatar.image = scale_to_half(image, org_width, org_height, cur_client.avatar.scaled_w * 45/300, cur_client.avatar.scaled_h * 45/300);

    gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.settings);
    t_actives.settings = gtk_image_new_from_file("client/media/settings.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.settings), "settings_icon");
    load_css_main(t_screen.provider, t_actives.settings);
    GtkGesture *click_settings = gtk_gesture_click_new();
    gtk_gesture_set_state(click_settings, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_settings, "pressed", G_CALLBACK(show_settings), NULL);
    gtk_widget_add_controller(t_actives.settings, GTK_EVENT_CONTROLLER(click_settings));
    gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.settings);

    (void)widget;

    printf("id of chat %d\n", ((t_chat *)data)->id);
    cur_client.cur_chat = *((t_chat *)data);

    if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0) {
        char buf[512+32] = {0};
        sprintf(buf, "<chat users avatars>%d", cur_client.cur_chat.id);
        send_all(cur_client.serv_fd,buf, 512+32);
        t_main.loaded = false;
        while (!t_main.loaded) {
            usleep(50);
        }
    }
    gtk_grid_remove(GTK_GRID(t_main.grid), t_main.right_panel);
    t_main.right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(GTK_WIDGET(t_main.right_panel), 30);
    gtk_widget_set_margin_top(GTK_WIDGET(t_main.right_panel), 20);
    gtk_box_set_spacing (GTK_BOX(t_main.right_panel), 8);

    GtkWidget *chat_headerbar = gtk_grid_new();
    gtk_grid_set_column_spacing (GTK_GRID(chat_headerbar), 535);


    GtkWidget *chat_headerbar_left = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(GTK_WIDGET(chat_headerbar_left), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(chat_headerbar_left), GTK_ALIGN_CENTER);
    GtkWidget *photo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *photo = get_circle_widget_from_png_custom("client/media/default_groupchat.png", 50, 50);
    gtk_box_append(GTK_BOX(photo_box), photo);
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_halign(GTK_WIDGET(info_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(info_box), GTK_ALIGN_CENTER);
    GtkWidget *group_or_user_name = gtk_label_new("Name Surname");
    GtkWidget *status = gtk_label_new("last seen 13 minutes ago");
    gtk_box_append(GTK_BOX(info_box), group_or_user_name);
    gtk_box_append(GTK_BOX(info_box), status);

    gtk_box_append(GTK_BOX(chat_headerbar_left), photo_box);
    gtk_box_append(GTK_BOX(chat_headerbar_left), info_box);

    GtkWidget *chat_headerbar_right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_widget_set_halign(GTK_WIDGET(chat_headerbar_right), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(chat_headerbar_right), GTK_ALIGN_CENTER);
    GtkWidget *block_img = gtk_image_new_from_file("client/media/settings.png"); 
    GtkWidget *mute_img = gtk_image_new_from_file("client/media/settings.png");
    gtk_box_append(GTK_BOX(chat_headerbar_right), block_img);
    gtk_box_append(GTK_BOX(chat_headerbar_right), mute_img); 

    gtk_grid_attach (GTK_GRID(chat_headerbar), chat_headerbar_left, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID(chat_headerbar), chat_headerbar_right, chat_headerbar_left, GTK_POS_RIGHT, 1, 1);

    t_main.scroll_box_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(GTK_WIDGET(t_main.scroll_box_right), GTK_ALIGN_END);
    gtk_widget_set_name(GTK_WIDGET(t_main.scroll_box_right), "messagebox_scroll");
    gtk_widget_set_margin_end(t_main.scroll_box_right, 30);
    load_css_main(t_screen.provider, t_main.scroll_box_right);
    t_main.scrolled_window_right = gtk_scrolled_window_new ();
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_right), 818, 588);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_right), t_main.scroll_box_right);
    gtk_widget_set_name(GTK_WIDGET(t_main.scrolled_window_right), "message_scroll");
    load_css_main(t_screen.provider, t_main.scrolled_window_right);

    GtkWidget *write_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(write_box), "write_message_box");
    load_css_main(t_screen.provider, write_box);
    gtk_widget_set_halign(GTK_WIDGET(write_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(write_box), GTK_ALIGN_START);
    gtk_box_set_spacing(GTK_BOX(write_box), 10);
    GtkWidget *write_message = gtk_text_view_new();
    gtk_widget_set_name(GTK_WIDGET(write_message), "write_message");
    gtk_widget_set_size_request(write_message, 800, 0);
    load_css_main(t_screen.provider, write_message);
    GtkTextBuffer *bio_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (write_message));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(write_message), GTK_WRAP_WORD_CHAR);
    g_signal_connect_after(bio_buffer, "insert-text", G_CALLBACK(insert_text_bio), NULL);
    GtkEventController *return_controller = gtk_event_controller_key_new();
    g_signal_connect_after(return_controller, "key-released", G_CALLBACK(return_controll_func), write_message);
    gtk_widget_add_controller(write_message,  GTK_EVENT_CONTROLLER(return_controller));
    GtkWidget *write_message_scroll = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(write_message_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (write_message_scroll), write_message);
    gtk_scrolled_window_set_propagate_natural_height (GTK_SCROLLED_WINDOW(write_message_scroll),false);
    gtk_scrolled_window_set_propagate_natural_width (GTK_SCROLLED_WINDOW(write_message_scroll),true);
    load_css_main(t_screen.provider, write_message_scroll);
    GtkWidget *stickers_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *stickers = gtk_image_new_from_file("client/media/search_ico.png");
    GtkGesture *click_sstickers = gtk_gesture_click_new();
    gtk_gesture_set_state(click_sstickers, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_sstickers, "pressed", G_CALLBACK(show_stickers), NULL);
    gtk_widget_add_controller(stickers, GTK_EVENT_CONTROLLER(click_sstickers));
    gtk_box_append(GTK_BOX(stickers_box), stickers);

    gtk_box_append(GTK_BOX(write_box), write_message_scroll);
    gtk_box_append(GTK_BOX(write_box), stickers_box);

    gtk_box_append(GTK_BOX(t_main.right_panel), chat_headerbar);
    gtk_box_append(GTK_BOX(t_main.right_panel), t_main.scrolled_window_right);
    gtk_box_append(GTK_BOX(t_main.right_panel), write_box);

    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);
    
    get_messages_from_server(cur_client.cur_chat.id, -1);
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
    //gtk_widget_set_size_request(GTK_WIDGET(t_main.right_panel), 720, 100);
    gtk_widget_set_name(GTK_WIDGET(t_main.right_panel), "right_panel");
    load_css_main(t_screen.provider, t_main.right_panel);
    gtk_widget_set_margin_start(GTK_WIDGET(t_main.right_panel), 20);
    GtkWidget *choose_friend = gtk_label_new("Choose someone to write...");
    gtk_box_append(GTK_BOX(t_main.right_panel), choose_friend);
    gtk_widget_set_margin_top(choose_friend, 40);
//-----------------------------------------------SearchPanel---------------------------------------------------------------
    GtkWidget *SearchBox, *SearchField;
    t_main.search_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(t_main.search_panel, 200, 45);
    gtk_widget_set_halign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_CENTER);
    //gtk_widget_set_margin_bottom(GTK_WIDGET(t_main.search_panel), 27);
    gtk_widget_set_margin_top(GTK_WIDGET(t_main.search_panel), 15);
    SearchBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(SearchBox), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(SearchBox), GTK_ALIGN_CENTER);
    t_main.logo = get_circle_widget_from_png_avatar(&cur_client.avatar, 45, 45, false);
    gtk_widget_set_name(GTK_WIDGET(t_main.logo), "account_avatar");
    load_css_main(t_screen.provider, t_main.logo);
    SearchField = gtk_entry_new();
    gtk_widget_set_name(GTK_WIDGET(SearchField), "search_field");
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER (gtk_entry_get_buffer(GTK_ENTRY(SearchField))), 20);
    gtk_entry_set_placeholder_text(GTK_ENTRY(SearchField), "Search");
    
    ///////////

    //GtkGesture *click_search_field = gtk_gesture_click_new();
    //gtk_gesture_set_state(click_search_field, GTK_EVENT_SEQUENCE_CLAIMED);
    //g_signal_connect_after(click_search_field, "pressed", G_CALLBACK(get_list_users), t_auth.LOGIN_menu);
    //gtk_widget_add_controller(SearchBox, GTK_EVENT_CONTROLLER(click_search_field));
    g_signal_connect (SearchField, "notify::text", G_CALLBACK (text_changed_main_screen), NULL);
    
    ///////////

    load_css_main(t_screen.provider, SearchField);
    gtk_box_append(GTK_BOX(SearchBox), GTK_WIDGET(SearchField));
    GFile *path = g_file_new_for_path("client/media/search_ico.png"); 
    GIcon *icon = g_file_icon_new(path);
    gtk_entry_set_icon_from_gicon(GTK_ENTRY(SearchField),GTK_ENTRY_ICON_SECONDARY, icon);
    
    t_actives.home = gtk_image_new_from_file("client/media/home.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.home ), "home_icon");
    load_css_main(t_screen.provider, t_actives.home );
    t_actives.settings = gtk_image_new_from_file("client/media/settings.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.settings), "settings_icon");
    load_css_main(t_screen.provider, t_actives.settings);
    GtkGesture *click_settings = gtk_gesture_click_new();
    gtk_gesture_set_state(click_settings, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_settings, "pressed", G_CALLBACK(show_settings), NULL);
    gtk_widget_add_controller(t_actives.settings, GTK_EVENT_CONTROLLER(click_settings));

    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(t_main.logo));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(SearchBox));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(t_actives.home));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(t_actives.settings));
    gtk_box_set_spacing (GTK_BOX(t_main.search_panel), 0);
//------------------------------------------------------------------------------------------------------------------------
    
    GtkWidget *recent_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(recent_box, GTK_ALIGN_START);
    gtk_widget_set_valign(recent_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(recent_box, 0);
    gtk_widget_set_margin_top(recent_box, 20);

    GtkWidget *recent_label = gtk_label_new("Recent");
    gtk_widget_set_name(GTK_WIDGET(recent_label), "recent_text");
    load_css_main(t_screen.provider, recent_label);
    //gtk_widget_set_size_request(recent_label, 20, 0);
    gtk_widget_set_halign(recent_label, GTK_ALIGN_START);
    gtk_widget_set_valign(recent_label, GTK_ALIGN_CENTER);

    GtkWidget *add_chat_button = gtk_button_new_with_label("+");
    gtk_widget_set_name(add_chat_button, "add_chat");
    load_css_main(t_screen.provider, add_chat_button);
    gtk_widget_set_margin_start(GTK_WIDGET(add_chat_button), 210);
    gtk_widget_set_halign(add_chat_button, GTK_ALIGN_END);
    gtk_widget_set_valign(add_chat_button, GTK_ALIGN_CENTER);

    gtk_box_append(GTK_BOX(recent_box), GTK_WIDGET(recent_label));
    gtk_box_append(GTK_BOX(recent_box), GTK_WIDGET(add_chat_button));

    GtkWidget **swapped = (GtkWidget **)malloc(4 * sizeof(GtkWidget *));
    swapped[0] = recent_label;
    swapped[1] = add_chat_button;
    swapped[2] = recent_box;
    g_signal_connect(add_chat_button, "clicked", G_CALLBACK (add_chat_dialog), (gpointer)swapped);
    

//------------------------------------------------------------------------------------------------------------------------

    t_main.scrolled_window_left = gtk_scrolled_window_new ();
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_left), 200, 600);
    t_main.scroll_box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box_left), 200, 0);
    

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
