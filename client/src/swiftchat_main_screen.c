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

    t_main.loaded = false;
    char buf[512 + 32] = {0};
    sprintf(buf, "<add chat, name=%s>%s", name, users);
    send_all(cur_client.serv_fd, buf, 512+32);
    while (!t_main.loaded) {
        usleep(50);
    }
    return_to_chatlist(widget, swapped);
    //return_to_chatlist(NULL, сюда передать первые 3 элемента gpointer data, так как 4 не нужен);

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
    gtk_widget_set_size_request(chat_name_entry, 300,20);
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
    gtk_widget_set_size_request(add_member_input, 300,20);
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

    GtkWidget *chat_image1 = get_circle_widget_from_png_custom("test_circle.png", 54, 54);
    gtk_widget_set_size_request(GTK_WIDGET(chat_image1),  54, 0);
    gtk_widget_set_halign(GTK_WIDGET(chat_image1), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(chat_image1), GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX(scroll_box), chat_image1);
    
    pthread_mutex_lock(&cl_mutex);

    t_list *temp = t_main.search_users_list;
    mx_clear_list(&t_main.check_buttons_user_list);
    t_main.check_buttons_user_list = NULL;
    while (temp) {
        if (mx_strcmp(cur_client.login, temp->data) != 0) {
            GtkWidget *button = gtk_check_button_new_with_label(temp->data);
            gtk_widget_set_name(GTK_WIDGET(button), "check_button");
            load_css_main(t_screen.provider, button);
            gtk_box_append(GTK_BOX (scroll_box), button);
            mx_push_back(&t_main.check_buttons_user_list, button);
        }
        temp = temp->next;
    }
    
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
    gtk_widget_set_size_request(button_success, 15, 15);
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
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_right), 828, 680);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_right), t_main.scroll_box_right);
    gtk_widget_set_name(GTK_WIDGET(t_main.scrolled_window_right), "right_scroll");
    load_css_main(t_screen.provider, t_main.scrolled_window_right);

    GtkWidget *write_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(write_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(write_box), GTK_ALIGN_CENTER);
    GtkWidget *write_message = gtk_entry_new();
    gtk_widget_set_size_request(write_message, 800, 36);
    gtk_entry_set_placeholder_text(GTK_ENTRY(write_message),"Write a message...");
    gtk_box_append(GTK_BOX(write_box), write_message);

    g_signal_connect(write_message, "activate", G_CALLBACK(key_press), NULL);

    gtk_box_append(GTK_BOX(t_main.right_panel), t_main.scrolled_window_right);
    gtk_box_append(GTK_BOX(t_main.right_panel), write_box);
    gtk_box_set_spacing (GTK_BOX(t_main.right_panel), 5);

    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);
}

static void get_list_users() {
    t_main.loaded = false;
    char message[512+32] = {0};
    sprintf(message, "<users list>");
    send(cur_client.serv_fd, message, 512+32, 0);
    while (!t_main.loaded)
        usleep(50);
}

void show_settings()
{
    gtk_grid_remove(GTK_GRID(t_main.grid), t_main.right_panel);
    t_main.right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_START);
    gtk_widget_set_margin_start(GTK_WIDGET(t_main.right_panel), 50);

    GtkWidget *acc_sett_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//<----------
    gtk_widget_set_halign(GTK_WIDGET(acc_sett_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(acc_sett_box), GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(acc_sett_box, 20);
    GtkWidget *acc_sett_label = gtk_label_new("Account settings");
    gtk_widget_set_name(GTK_WIDGET(acc_sett_label), "account_sett_text");
    load_css_main(t_screen.provider, acc_sett_label);
    gtk_box_append(GTK_BOX(acc_sett_box), acc_sett_label);

    GtkWidget *сhange_data_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//<---------
    gtk_box_set_spacing(GTK_BOX(сhange_data_box), 100);
    gtk_widget_set_halign(GTK_WIDGET(сhange_data_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(сhange_data_box), GTK_ALIGN_START);

    GtkWidget *change_data_left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(change_data_left_box), 10);
    gtk_widget_set_halign(GTK_WIDGET(change_data_left_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(change_data_left_box), GTK_ALIGN_START);
    GtkWidget *photo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(photo_box), 8);
    gtk_widget_set_halign(GTK_WIDGET(photo_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(photo_box), GTK_ALIGN_START);
    GtkWidget *photo_label_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(photo_label_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(photo_label_box), GTK_ALIGN_START);
    GtkWidget *photo_label = gtk_label_new("PHOTO");
    gtk_widget_set_name(GTK_WIDGET(photo_label), "photo_label");
    load_css_main(t_screen.provider, photo_label);
    gtk_box_append(GTK_BOX(photo_label_box), photo_label);
    GtkWidget *photo_buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(photo_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(photo_box), GTK_ALIGN_START);
    GtkWidget *change_photo = gtk_button_new_with_label("Change photo");
    gtk_widget_set_name(GTK_WIDGET(change_photo), "change_photo_btn");
    load_css_main(t_screen.provider, change_photo);
    gtk_box_append(GTK_BOX(photo_buttons_box), change_photo);
    GtkWidget *delete = gtk_button_new_with_label("Delete");
    gtk_widget_set_name(GTK_WIDGET(delete), "delete_btn");
    load_css_main(t_screen.provider, delete);
    gtk_box_append(GTK_BOX(photo_buttons_box), delete);

    gtk_box_append(GTK_BOX(photo_box), photo_label_box);
    gtk_box_append(GTK_BOX(photo_box), photo_buttons_box);

    gtk_box_append(GTK_BOX(change_data_left_box), photo_box);


    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(name_box), 8);
    gtk_widget_set_halign(GTK_WIDGET(name_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(name_box), GTK_ALIGN_START);
    GtkWidget *name_label_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(name_label_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(name_label_box), GTK_ALIGN_START);
    GtkWidget *name_label = gtk_label_new("NAME");
    gtk_widget_set_name(GTK_WIDGET(name_label), "name_label");
    load_css_main(t_screen.provider, name_label);
    gtk_box_append(GTK_BOX(name_label_box), name_label);
    GtkWidget *name_entry_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(name_entry_box), 8);
    gtk_widget_set_halign(GTK_WIDGET(name_entry_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(name_entry_box), GTK_ALIGN_START);
    GtkWidget *entry_field_name = gtk_entry_new();
    gtk_widget_set_name(GTK_WIDGET(entry_field_name), "entry_field_name");
    load_css_main(t_screen.provider, entry_field_name);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_field_name), "First name");
    gtk_widget_set_size_request(entry_field_name, 240, 30);
    gtk_box_append(GTK_BOX(name_entry_box), entry_field_name);
    GtkWidget *entry_field_surname = gtk_entry_new();
    gtk_widget_set_name(GTK_WIDGET(entry_field_surname), "entry_field_surname");
    load_css_main(t_screen.provider, entry_field_surname);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_field_surname), "Second Name");
    gtk_widget_set_size_request(entry_field_surname, 240, 30);
    gtk_box_append(GTK_BOX(name_entry_box), entry_field_surname);

    gtk_box_append(GTK_BOX(name_box), name_label_box);
    gtk_box_append(GTK_BOX(name_box), name_entry_box);

    gtk_box_append(GTK_BOX(change_data_left_box), name_box);

    GtkWidget *bio_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(bio_box), 13);
    gtk_widget_set_halign(GTK_WIDGET(bio_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(bio_box), GTK_ALIGN_START);
    GtkWidget *bio_label_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(bio_label_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(bio_label_box), GTK_ALIGN_START);
    GtkWidget *bio_label = gtk_label_new("BIO");
    gtk_widget_set_name(GTK_WIDGET(bio_label), "bio_label");
    load_css_main(t_screen.provider, bio_label);
    gtk_box_append(GTK_BOX(bio_label_box), bio_label);
    GtkWidget *bio_entry_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(bio_entry_box), 8);
    gtk_widget_set_halign(GTK_WIDGET(bio_entry_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(bio_entry_box), GTK_ALIGN_START);
    GtkWidget *entry_field = gtk_text_view_new();
    gtk_widget_set_name(GTK_WIDGET(entry_field), "entry_field_bio");
    load_css_main(t_screen.provider, entry_field);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(entry_field), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(entry_field), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(entry_field), 10);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(entry_field), 10);

    gtk_widget_set_size_request(entry_field, 240, 120);
    gtk_box_append(GTK_BOX(bio_entry_box), entry_field);
    GtkWidget *apply_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(apply_button_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(apply_button_box), GTK_ALIGN_CENTER);
    GtkWidget *apply_button = gtk_button_new_with_label("Apply");
    gtk_widget_set_name(GTK_WIDGET(apply_button), "apply_btn");
    load_css_main(t_screen.provider, apply_button);
    gtk_widget_set_size_request(apply_button, 160, 30);
    gtk_widget_set_margin_start(apply_button, 0);
    gtk_box_append(GTK_BOX(apply_button_box), apply_button);

    gtk_box_append(GTK_BOX(bio_box), bio_label_box);
    gtk_box_append(GTK_BOX(bio_box), bio_entry_box);
    gtk_box_append(GTK_BOX(bio_box), apply_button_box);

    gtk_box_append(GTK_BOX(change_data_left_box), bio_box);


    GtkWidget *change_data_right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(change_data_right_box, "change_data_right_box");
    gtk_widget_set_size_request(change_data_right_box, 230, 270);
    load_css_main(t_screen.provider, change_data_right_box);
    gtk_box_set_spacing(GTK_BOX(change_data_right_box), 10);
    gtk_widget_set_halign(GTK_WIDGET(change_data_right_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(change_data_right_box), GTK_ALIGN_CENTER);
    GtkWidget *user_image_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(user_image_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(user_image_box), GTK_ALIGN_CENTER);
    GtkWidget *user_image = get_circle_widget_from_png_custom("test_circle.png", 120, 120);
    gtk_box_append(GTK_BOX(user_image_box), user_image);
    GtkWidget *user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_CENTER);
    GtkWidget *user_name = gtk_label_new("Name Surname");
    gtk_widget_set_name(user_name, "user_name");
    load_css_main(t_screen.provider, user_name);
    gtk_box_append(GTK_BOX(user_name_box), user_name);
    GtkWidget *user_bio_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(user_bio_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(user_bio_box), GTK_ALIGN_CENTER);
    GtkWidget *user_bio = gtk_label_new("User bio vjifdjvifdjvidfjvidfojvdfovjdfo");
    gtk_widget_set_name(user_bio, "user_bio");
    load_css_main(t_screen.provider, user_bio);
    gtk_box_append(GTK_BOX(user_bio_box), user_bio);

    gtk_box_append(GTK_BOX(change_data_right_box), user_image_box);
    gtk_box_append(GTK_BOX(change_data_right_box), user_name_box);
    gtk_box_append(GTK_BOX(change_data_right_box), user_bio_box);

    gtk_box_append(GTK_BOX(сhange_data_box), change_data_left_box);
    gtk_box_append(GTK_BOX(сhange_data_box), change_data_right_box);

    gtk_widget_set_margin_bottom(сhange_data_box, 45);

    GtkWidget *themes_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(themes_box), 15);
    gtk_widget_set_halign(GTK_WIDGET(themes_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(themes_box), GTK_ALIGN_START);
    GtkWidget *themes_label_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(themes_label_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(themes_label_box), GTK_ALIGN_START);
    GtkWidget *themes_label = gtk_label_new("Themes");
    gtk_widget_set_name(GTK_WIDGET(themes_label), "themes_label");
    load_css_main(t_screen.provider, themes_label);
    gtk_box_append(GTK_BOX(themes_label_box), themes_label);
    GtkWidget *radio_buttons_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(radio_buttons_box), 6);
    gtk_widget_set_halign(GTK_WIDGET(radio_buttons_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(radio_buttons_box), GTK_ALIGN_START);
    GtkWidget *radio_button_dark = gtk_check_button_new_with_label("Dark");
    gtk_widget_set_name(GTK_WIDGET(radio_button_dark), "radio_button_dark");
    load_css_main(t_screen.provider, radio_button_dark);
    gtk_box_append(GTK_BOX(radio_buttons_box), radio_button_dark);
    GtkWidget *radio_button_light = gtk_check_button_new_with_label("Light");
    gtk_widget_set_name(GTK_WIDGET(radio_button_light), "radio_button_light");
    load_css_main(t_screen.provider, radio_button_light);
    gtk_box_append(GTK_BOX(radio_buttons_box), radio_button_light);
    gtk_check_button_set_group(GTK_CHECK_BUTTON(radio_button_dark), GTK_CHECK_BUTTON(radio_button_light));
    GtkWidget *delete_box_and_exit = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_spacing(GTK_BOX(delete_box_and_exit), 20);
    gtk_widget_set_halign(GTK_WIDGET(delete_box_and_exit), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(delete_box_and_exit), GTK_ALIGN_CENTER);
    GtkWidget *delete_button = gtk_button_new_with_label("DELETE ACCOUNT");
    gtk_widget_set_name(GTK_WIDGET(delete_button), "delete_account_btn");
    load_css_main(t_screen.provider, delete_button);
    gtk_box_append(GTK_BOX(delete_box_and_exit), delete_button);
    GtkWidget *exit_button = gtk_button_new_with_label("Log out");
    gtk_widget_set_name(GTK_WIDGET(exit_button), "logOut_btn");
    load_css_main(t_screen.provider, exit_button);
    GtkGesture *click_exit = gtk_gesture_click_new();
    gtk_gesture_set_state(click_exit, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_exit, "pressed", G_CALLBACK(gtk_window_destroy), t_screen.main_window);
    gtk_widget_add_controller(exit_button, GTK_EVENT_CONTROLLER(click_exit));
    gtk_box_append(GTK_BOX(delete_box_and_exit), exit_button);

    gtk_box_append(GTK_BOX(themes_box), themes_label_box);
    gtk_box_append(GTK_BOX(themes_box), radio_buttons_box);
    gtk_box_append(GTK_BOX(themes_box), delete_box_and_exit);


    gtk_box_append(GTK_BOX(t_main.right_panel), acc_sett_box);
    gtk_box_append(GTK_BOX(t_main.right_panel), сhange_data_box);
    gtk_box_append(GTK_BOX(t_main.right_panel), themes_box);

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
    //gtk_widget_set_size_request(GTK_WIDGET(t_main.right_panel), 720, 100);
    gtk_widget_set_name(GTK_WIDGET(t_main.right_panel), "right_panel");
    load_css_main(t_screen.provider, t_main.right_panel);
    gtk_widget_set_margin_start(GTK_WIDGET(t_main.right_panel), 20);
    GtkWidget *choose_friend = gtk_label_new("Choose someone to write...");
    gtk_box_append(GTK_BOX(t_main.right_panel), choose_friend);
    gtk_widget_set_margin_top(choose_friend, 40);
//-----------------------------------------------SearchPanel---------------------------------------------------------------
    GtkWidget *SearchBox, *Logo, *SearchField, *Home, *Settings;
    t_main.search_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_CENTER);
    //gtk_widget_set_margin_bottom(GTK_WIDGET(t_main.search_panel), 27);
    gtk_widget_set_margin_top(GTK_WIDGET(t_main.search_panel), 15);
    SearchBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(SearchBox), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(SearchBox), GTK_ALIGN_CENTER);
    Logo = get_circle_widget_from_png_custom("test_circle.png", 45, 45);
    gtk_widget_set_name(GTK_WIDGET(Logo), "account_avatar");
    load_css_main(t_screen.provider, Logo);
    SearchField = gtk_entry_new();
    gtk_widget_set_name(GTK_WIDGET(SearchField), "search_field");
    gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER (gtk_entry_get_buffer(GTK_ENTRY(SearchField))), 20);
    gtk_entry_set_placeholder_text(GTK_ENTRY(SearchField), "Search");
    
    ///////////

    GtkGesture *click_search_field = gtk_gesture_click_new();
    gtk_gesture_set_state(click_search_field, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_search_field, "pressed", G_CALLBACK(get_list_users), t_auth.LOGIN_menu);
    gtk_widget_add_controller(SearchField, GTK_EVENT_CONTROLLER(click_search_field));
    g_signal_connect (SearchField, "notify::text", G_CALLBACK (text_changed_main_screen), NULL);

    ///////////

    load_css_main(t_screen.provider, SearchField);
    gtk_box_append(GTK_BOX(SearchBox), GTK_WIDGET(SearchField));
    GFile *path = g_file_new_for_path("client/media/search_ico.png"); 
    GIcon *icon = g_file_icon_new(path);
    gtk_entry_set_icon_from_gicon(GTK_ENTRY(SearchField),GTK_ENTRY_ICON_SECONDARY, icon);
    Home = gtk_image_new_from_file("client/media/home.png");
    gtk_widget_set_name(GTK_WIDGET(Home), "home_icon");
    load_css_main(t_screen.provider, Home);
    Settings = gtk_image_new_from_file("client/media/settings.png");
    gtk_widget_set_name(GTK_WIDGET(Settings), "settings_icon");
    load_css_main(t_screen.provider, Settings);

    GtkGesture *click_settings = gtk_gesture_click_new();
    gtk_gesture_set_state(click_settings, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_settings, "pressed", G_CALLBACK(show_settings), NULL);
    gtk_widget_add_controller(Settings, GTK_EVENT_CONTROLLER(click_settings));

    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Logo));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(SearchBox));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Home));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(Settings));
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
