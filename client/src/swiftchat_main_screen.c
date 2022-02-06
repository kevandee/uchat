#include "../inc/uch_client.h"
 
void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
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
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box_left), 200, 0);
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_left), 200, 600);

    t_list *chats = cur_client.chats;
    while (chats) {
        
        add_chat_node(chats->data);
        chats = chats->next;
    }
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left),t_main.scroll_box_left);
}



static void show_group_settings(GtkWidget *widget, gpointer data)
{
    (void)widget;
    cur_client.cur_chat.id = -1;
    if(t_main.sticker_panel)
    {
        gtk_widget_hide(t_main.sticker_panel);
    }

    gtk_grid_remove(GTK_GRID(t_main.grid), t_main.right_panel);
    t_main.right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_START);
    gtk_widget_set_margin_start(GTK_WIDGET(t_main.right_panel), 50);

    GtkWidget *group_sett_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 550);//<----------
    gtk_widget_set_halign(GTK_WIDGET(group_sett_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(group_sett_box), GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(group_sett_box, 20);
    gtk_widget_set_margin_top(group_sett_box, 28);
    GtkWidget *group_sett_label_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(group_sett_label_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(group_sett_label_box), GTK_ALIGN_CENTER);
    GtkWidget *group_sett_label = gtk_label_new("Group settings");
    gtk_widget_set_name(GTK_WIDGET(group_sett_label), "account_sett_text");
    load_css_main(t_screen.provider, group_sett_label);
    gtk_box_append(GTK_BOX(group_sett_label_box), group_sett_label);
    GtkWidget *return_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(return_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(return_box), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(return_box, 5);
    GtkWidget *return_button = NULL;
    if(cur_client.theme == DARK_THEME) 
        return_button = gtk_image_new_from_file("client/media/return_settings.png");
    else
        return_button = gtk_image_new_from_file("client/media/return_settings_light.png");

    GtkGesture *click_back = gtk_gesture_click_new();
    gtk_gesture_set_state(click_back, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_back, "pressed", G_CALLBACK(show_chat_history), data);
    gtk_widget_add_controller(return_button, GTK_EVENT_CONTROLLER(click_back));

    gtk_widget_set_size_request(return_button, 25, 25);
    gtk_box_append(GTK_BOX(return_box), return_button);

    gtk_box_append(GTK_BOX(group_sett_box), group_sett_label_box);
    gtk_box_append(GTK_BOX(group_sett_box), return_box);

    GtkWidget *сhange_data_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);//<---------
    gtk_box_set_spacing(GTK_BOX(сhange_data_box), 100);
    gtk_widget_set_halign(GTK_WIDGET(сhange_data_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(сhange_data_box), GTK_ALIGN_START);

    GtkWidget *change_data_left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(change_data_left_box), 20);
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
    //g_signal_connect(change_photo, "clicked", G_CALLBACK(choise_photo_file), NULL);
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
    gtk_entry_set_max_length(GTK_ENTRY(entry_field_name), 32);
    gtk_widget_set_name(GTK_WIDGET(entry_field_name), "entry_field_name");
    load_css_main(t_screen.provider, entry_field_name);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_field_name), "New gruup name");
    gtk_widget_set_size_request(entry_field_name, 240, 30);
    gtk_box_append(GTK_BOX(name_entry_box), entry_field_name);
    GtkWidget *apply_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(apply_button_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(apply_button_box), GTK_ALIGN_CENTER);
    GtkWidget *apply_button = gtk_button_new_with_label("Apply");
    gtk_widget_set_name(GTK_WIDGET(apply_button), "apply_btn");
    load_css_main(t_screen.provider, apply_button);
    gtk_widget_set_size_request(apply_button, 160, 30);
    gtk_widget_set_margin_start(apply_button, 0);
    gtk_box_append(GTK_BOX(apply_button_box), apply_button);

    gtk_box_append(GTK_BOX(name_box), name_label_box);
    gtk_box_append(GTK_BOX(name_box), name_entry_box);
    gtk_box_append(GTK_BOX(name_box), apply_button_box);

    gtk_box_append(GTK_BOX(change_data_left_box), name_box);

    GtkWidget *change_data_right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(change_data_right_box, "change_data_right_box");
    gtk_widget_set_size_request(change_data_right_box, 230, 230);
    load_css_main(t_screen.provider, change_data_right_box);
    gtk_box_set_spacing(GTK_BOX(change_data_right_box), 10);
    gtk_widget_set_halign(GTK_WIDGET(change_data_right_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(change_data_right_box), GTK_ALIGN_CENTER);

    GtkWidget *group_image_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(group_image_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(group_image_box), GTK_ALIGN_CENTER);
    GtkWidget *group_image = get_circle_widget_from_png_avatar(&cur_client.avatar, 120, 120,  true);
    gtk_box_append(GTK_BOX(group_image_box), group_image);
    GtkWidget *group_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(group_name_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(group_name_box), GTK_ALIGN_CENTER);
    GtkWidget *group_name = gtk_label_new("BEBRA LOVERS");
    gtk_widget_set_size_request(group_name, 230, 0);
    gtk_label_set_max_width_chars(GTK_LABEL (group_name), 32);
    gtk_label_set_wrap(GTK_LABEL(group_name), true);
    gtk_label_set_wrap_mode(GTK_LABEL(group_name), PANGO_WRAP_WORD);
    gtk_widget_set_name(group_name, "user_name");
    load_css_main(t_screen.provider, group_name);
    gtk_box_append(GTK_BOX(group_name_box), group_name);

    gtk_box_append(GTK_BOX(change_data_right_box), group_image_box);
    gtk_box_append(GTK_BOX(change_data_right_box), group_name_box);

    gtk_box_append(GTK_BOX(сhange_data_box), change_data_left_box);
    gtk_box_append(GTK_BOX(сhange_data_box), change_data_right_box);

    //gtk_widget_set_margin_bottom(сhange_data_box, 45);

    GtkWidget *members_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(members_box), 15);
    gtk_widget_set_halign(GTK_WIDGET(members_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(members_box), GTK_ALIGN_START);

    GtkWidget *members_label_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(members_label_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(members_label_box), GTK_ALIGN_START);
    GtkWidget *members_label = gtk_label_new("MEMBERS LIST");
    gtk_widget_set_name(GTK_WIDGET(members_label), "name_label");
    load_css_main(t_screen.provider, members_label);
    gtk_box_append(GTK_BOX(members_label_box), members_label);

    GtkWidget *members_list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(members_list_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(members_list_box), GTK_ALIGN_START);
    GtkWidget *members_scroll_window = gtk_scrolled_window_new();
    gtk_widget_set_size_request(GTK_WIDGET(members_scroll_window), 320, 350);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(members_scroll_window), members_list_box);
    gtk_widget_set_name(members_scroll_window, "test");
    load_css_main(t_screen.provider, members_scroll_window);

    gtk_box_append(GTK_BOX(members_box), members_label_box);
    gtk_box_append(GTK_BOX(members_box), members_scroll_window);


    gtk_box_append(GTK_BOX(t_main.right_panel), group_sett_box);
    gtk_box_append(GTK_BOX(t_main.right_panel), сhange_data_box);
    gtk_box_append(GTK_BOX(t_main.right_panel), members_box);

    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);
    
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
    send_all(cur_client.ssl, buf, 512+32);
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
    SSL_write(cur_client.ssl, message, 512+32);
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
            char buf[544] = {0};
            t_main.loaded = false;
            sprintf(buf, "<get user avatar>%s",temp->data);
            printf("buf %s\n", buf);
            send_all(cur_client.ssl, buf, 544);
            while(!t_main.loaded) {
                usleep(50);
            }
            t_avatar *avatar = t_main.loaded_avatar;
            //printf("%s\n", avatar.name);
            if (mx_strcmp(avatar->name, "default") == 0) {
                *avatar = t_main.default_avatar;
            }
            GtkWidget *user_img_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_widget_set_size_request(user_img_box, 40, 40);
            GtkWidget *user_img = get_circle_widget_from_png_avatar(avatar, 40, 40, false);
            gtk_widget_set_size_request(user_img, 40, 40);
            gtk_box_append(GTK_BOX(user_img_box), user_img);
            GtkWidget *user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_widget_set_margin_start(user_name_box, 10);
            GtkWidget *user_name = gtk_label_new(temp->data);
            gtk_widget_set_name(user_name, "users_check_boxes");
            load_css_main(t_screen.provider, user_name);
            gtk_box_append(GTK_BOX(user_name_box), user_name);
            GtkWidget *user_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_widget_set_margin_start(user_button_box, 180);
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
    t_main.add_users_grid = users_grid;
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

    gtk_widget_set_margin_start(add_chat_box, 5);
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
        bool is_edit = false;
        const char *buf_str = gtk_text_buffer_get_text(buffer, &start, &end, true);

        if (!buf_str || buf_str[0] == '\n') {
            return;
        }
        char *str = mx_strnew(1024);
        str = mx_strncpy(str, buf_str, mx_strlen(buf_str) - 1);
        char message[512 + 32] = {0};
        GtkWidget *my_msg_box = NULL;
        GtkWidget *msg = NULL;
        if (!t_main.message_change_id) 
        {
            sprintf(message, "<msg, chat_id= %d>%s", cur_client.cur_chat.id, str);
            my_msg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_widget_set_halign(GTK_WIDGET(my_msg_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(my_msg_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(my_msg_box, 5);
            gtk_widget_set_margin_bottom(my_msg_box, 5);
            msg = gtk_label_new(str);
            gtk_widget_set_name(GTK_WIDGET(msg), "message");
            load_css_main(t_screen.provider, msg);
            gtk_label_set_wrap(GTK_LABEL(msg), TRUE);
            gtk_label_set_wrap_mode(GTK_LABEL(msg), PANGO_WRAP_WORD_CHAR);
            gtk_label_set_max_width_chars(GTK_LABEL(msg), 50);
            gtk_label_set_selectable(GTK_LABEL(msg), FALSE);
            mx_push_front(&t_main.message_widgets_list, msg);

            if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0) 
            {
                GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
                gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_END);
                gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_END);
                gtk_widget_set_margin_bottom(message_box, 5);
                gtk_widget_set_margin_end(message_box, 5);
                gtk_box_set_spacing(GTK_BOX(message_box), 10);

                GtkWidget* message_content = gtk_grid_new();
                gtk_grid_set_row_spacing(GTK_GRID(message_content), 3);
                gtk_grid_set_column_spacing(GTK_GRID(message_content), 5);

                GtkWidget *user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_START);
                gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_START);

                GtkWidget *user_name = gtk_label_new("Dimas");
                gtk_widget_set_name(user_name, "user_name_chat");
                load_css_main(t_screen.provider, user_name);
                gtk_box_append(GTK_BOX(user_name_box), user_name);

                my_msg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                gtk_widget_set_halign(GTK_WIDGET(my_msg_box), GTK_ALIGN_CENTER);
                gtk_widget_set_valign(GTK_WIDGET(my_msg_box), GTK_ALIGN_CENTER);
                gtk_box_append(GTK_BOX(my_msg_box), msg);

                GtkWidget *user_logo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                gtk_widget_set_halign(GTK_WIDGET(user_logo_box), GTK_ALIGN_CENTER);
                gtk_widget_set_valign(GTK_WIDGET(user_logo_box), GTK_ALIGN_CENTER);
                GtkWidget *user_logo = get_circle_widget_current_user_avatar();
                gtk_box_append(GTK_BOX(user_logo_box), user_logo);

                GtkWidget *user_action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                gtk_widget_set_halign(GTK_WIDGET(user_action_box), GTK_ALIGN_END);
                gtk_widget_set_valign(GTK_WIDGET(user_action_box), GTK_ALIGN_END);
                GtkWidget *user_action = gtk_label_new("edited");
                gtk_widget_set_name(user_action, "user_action");
                load_css_main(t_screen.provider, user_action);
                gtk_box_append(GTK_BOX(user_action_box), user_action);


                gtk_grid_attach(GTK_GRID(message_content), user_name_box, 1, 1, 1, 1);
                gtk_grid_attach(GTK_GRID(message_content), my_msg_box, 1, 2, 1, 1);
                gtk_grid_attach(GTK_GRID(message_content), user_logo_box, 2, 1, 1, 3);
                gtk_grid_attach(GTK_GRID(message_content), user_action_box, 1, 3, 1, 1);

                gtk_box_append(GTK_BOX(message_box), message_content);
                gtk_box_append(GTK_BOX(t_main.scroll_box_right), message_box);

            }

            else
            {
                gtk_box_append(GTK_BOX(my_msg_box), msg);
                gtk_box_append(GTK_BOX(t_main.scroll_box_right), my_msg_box);
            }

            pthread_t display_thread = NULL;
            pthread_create(&display_thread, NULL, scroll_func, NULL);
        }

        else {
            sprintf(message, "<edit msg, chat_id=%d, mes_id=%d>%s", cur_client.cur_chat.id, t_main.message_change_id, str);
            is_edit = true;

            // найти и изменить
            t_list *temp_mes = cur_client.cur_chat.messages;
            t_list *temp_widgets = t_main.message_widgets_list;

            while(temp_mes) {
                t_message *mes = (t_message *)temp_mes->data;
                if (mes->id == t_main.message_change_id) {
                    break;
                }

                temp_widgets = temp_widgets->next;
                temp_mes = temp_mes->next;
            }

            gtk_label_set_text(GTK_LABEL (temp_widgets->data), str);

            t_main.message_change_id = 0;
        }           

        send_all(cur_client.ssl, message, 512+32);
        gtk_text_buffer_set_text (buffer, "", 0);

        ////////////////
        if (!is_edit) {
            GtkGesture *gesture = gtk_gesture_click_new();
            gtk_gesture_set_state(gesture, GTK_EVENT_SEQUENCE_CLAIMED);
            gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture), 3);
            GtkWidget **arr = (GtkWidget **)malloc(2*sizeof(GtkWidget *));
            arr[0] = msg;
            arr[1] = my_msg_box;
            int *mes_id = (int *)malloc(sizeof(int));
            char buf[544] = {0};
            sprintf(buf, "<get last mes id>");
            send_all(cur_client.ssl, buf, 544);
            t_main.loaded = false;
            while (!t_main.loaded) {
                usleep(50);
            }
            *mes_id = t_main.send_mes_id;
            t_message *mes = (t_message *)malloc(sizeof(t_message));
            mes->id = *mes_id;
            mx_push_front(&cur_client.cur_chat.messages, mes);
            t_list *gesture_data = NULL;
            mx_push_back(&gesture_data, arr);
            mx_push_back(&gesture_data, mes_id);
            g_signal_connect_after(gesture, "pressed", G_CALLBACK(show_message_menu), gesture_data);
            gtk_widget_add_controller(my_msg_box, GTK_EVENT_CONTROLLER(gesture));
        }
        /////////////// 
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

static void show_stickers(gpointer data);

static void hide_stickers(gpointer data)
{
    GtkWidget **change = data;
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_right), 818, 588);
    gtk_widget_set_size_request(change[0], 750, 0);
    gtk_grid_set_column_spacing (GTK_GRID(change[1]), 535);
    gtk_box_remove(GTK_BOX(change[2]), change[3]);
    change[3] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    if(cur_client.theme == DARK_THEME)
        change[4] = gtk_image_new_from_file("client/media/sticker_dark.png");
    else change[4] = gtk_image_new_from_file("client/media/sticker_light.png");
    gtk_widget_set_size_request(change[4], 27, 27);
    gtk_box_append(GTK_BOX(change[3]), change[4]);

    GtkGesture *click_stickers = gtk_gesture_click_new();
    gtk_gesture_set_state(click_stickers, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_stickers, "pressed", G_CALLBACK(show_stickers), (gpointer)change);
    gtk_widget_add_controller(change[4], GTK_EVENT_CONTROLLER(click_stickers));
    gtk_box_insert_child_after(GTK_BOX(change[2]), change[3], change[6]);

    gtk_widget_hide(t_main.sticker_panel);
}

static void send_sticker(GtkGestureClick *gesture, int n_press, double x, double y, gpointer data) {
    (void)gesture;
    (void)n_press;
    (void)x;
    (void)y;

    int sticker_num = *(int *)data;

    char buf[544] = {0};
    sprintf(buf, "<sticker chat_id=%d, sticker_num=%d>", cur_client.cur_chat.id, sticker_num);
    send_all(cur_client.ssl, buf, 544);

    GtkWidget *sticker = gtk_image_new_from_file(mx_strjoin(mx_strjoin("client/media/stickers/", mx_itoa(sticker_num)), ".png"));
    gtk_widget_set_halign(sticker, GTK_ALIGN_END);
    gtk_widget_set_margin_end(sticker, 5);
    gtk_widget_set_margin_bottom(sticker, 5);
    gtk_widget_set_size_request(GTK_WIDGET(sticker), 130, 130);
    gtk_box_append(GTK_BOX (t_main.scroll_box_right), sticker);
}

static void show_stickers(gpointer data)
{
    GtkWidget **change = data;
    gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_right), 543, 588);
    gtk_widget_set_size_request(change[0], 525, 0);
    gtk_grid_set_column_spacing (GTK_GRID(change[1]), 260);
    gtk_box_remove(GTK_BOX(change[2]), change[3]);
    change[3] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    change[4] = gtk_image_new_from_file("client/media/sticker_active.png");
    gtk_widget_set_size_request(change[4], 27, 27);
    gtk_box_append(GTK_BOX(change[3]), change[4]);
    gtk_box_insert_child_after(GTK_BOX(change[2]), change[3], change[6]);

    t_main.sticker_scroll_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(GTK_WIDGET(t_main.sticker_scroll_box), GTK_ALIGN_END);
    gtk_widget_set_name(GTK_WIDGET(t_main.sticker_scroll_box), "stickerbox_scroll");
    load_css_main(t_screen.provider, t_main.sticker_scroll_box);

    GtkWidget *stickers = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(stickers), 5);
    gtk_grid_set_column_spacing(GTK_GRID(stickers), 5);

    int sticker_num = 1;
    int i = 0;
    int j = 0;
    GtkWidget *single;
    for(i = 1; i <= 15; i++)    // Columns
    {
        for(j = 1; j <= 4; j++) // Rows
        {
            if(sticker_num > 60) 
                break;
            single = gtk_image_new_from_file(mx_strjoin(mx_strjoin("client/media/stickers/", mx_itoa(sticker_num)), ".png"));

            gtk_widget_set_size_request(GTK_WIDGET(single), 70, 70);
            gtk_grid_attach(GTK_GRID(stickers), single, j, i, 1, 1);
            gtk_widget_set_name(GTK_WIDGET(single), "stickers");

            GtkGesture *click_sticker = gtk_gesture_click_new();
            gtk_gesture_set_state(click_sticker, GTK_EVENT_SEQUENCE_CLAIMED);
            int *num = (int *)malloc(sizeof(int));
            *num = sticker_num;
            g_signal_connect_after(click_sticker, "pressed", G_CALLBACK(send_sticker), num);
            gtk_widget_add_controller(single, GTK_EVENT_CONTROLLER(click_sticker));

            
            sticker_num++;
        }
    }

    gtk_box_append(GTK_BOX(t_main.sticker_scroll_box), stickers);


    t_main.sticker_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.sticker_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(t_main.sticker_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(t_main.sticker_panel, 27);
    gtk_box_set_spacing(GTK_BOX(t_main.sticker_panel), 10);
    GtkWidget *stickers_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(stickers_label_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(stickers_label_box), GTK_ALIGN_CENTER);
    GtkWidget *stickers_label = gtk_label_new("Stickers");
    gtk_widget_set_name(stickers_label, "recent_text");
    load_css_main(t_screen.provider, stickers_label);
    gtk_box_append(GTK_BOX(stickers_label_box), stickers_label);
/**/
    t_main.sticker_scroll_window = gtk_scrolled_window_new();
    gtk_widget_set_size_request(GTK_WIDGET(t_main.sticker_scroll_window), 295, 655);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.sticker_scroll_window), t_main.sticker_scroll_box);
    gtk_widget_set_name(GTK_WIDGET(t_main.sticker_scroll_window), "sticker_panel");
    load_css_main(t_screen.provider, t_main.sticker_scroll_window);

    gtk_box_append(GTK_BOX(t_main.sticker_panel),  stickers_label_box);
    gtk_box_append(GTK_BOX(t_main.sticker_panel),  t_main.sticker_scroll_window);

    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.sticker_panel, 2, 0, 1, 2);

    GtkGesture *click_stickers = gtk_gesture_click_new();
    gtk_gesture_set_state(click_stickers, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_stickers, "pressed", G_CALLBACK(hide_stickers), (gpointer)change);
    gtk_widget_add_controller(change[4], GTK_EVENT_CONTROLLER(click_stickers));

}

void load_more_messages (GtkScrolledWindow *scrolled_window, GtkPositionType pos, gpointer data) {
    (void)scrolled_window;
    t_chat *chat = data;
    if (pos == GTK_POS_BOTTOM || chat->last_mes_id == 1) {
        return;
    }
    t_main.scroll_mes = false;
    

    GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW(t_main.scrolled_window_right));
    t_main.adj_pos = gtk_adjustment_get_upper(GTK_ADJUSTMENT(adj));//gtk_adjustment_get_value(GTK_ADJUSTMENT(adj));
    
    get_messages_from_server(chat->id, chat->last_mes_id);

}

void show_chat_history(GtkWidget *widget, gpointer data)
{
    (void)widget;
    if (cur_client.cur_chat.id == ((t_chat *)data)->id) {
        return;
    }
    mx_clear_ldata(&cur_client.cur_chat.messages);
    mx_clear_list(&cur_client.cur_chat.messages);
    cur_client.cur_chat.messages = NULL;
    mx_clear_list(&t_main.message_widgets_list);
    t_main.message_widgets_list = NULL;

    printf("id of chat %d\n", ((t_chat *)data)->id);
    cur_client.cur_chat = *((t_chat *)data);
    printf("1\n");
    t_main.scroll_mes = true;
    if(t_main.sticker_panel)
    {
        gtk_widget_hide(t_main.sticker_panel);
    }

    printf("2\n");

    gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.home);
    if(cur_client.theme == DARK_THEME)
        t_actives.home = gtk_image_new_from_file("client/media/home.png");
    else 
        t_actives.home = gtk_image_new_from_file("client/media/home_light.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.home), "home_icon");
    load_css_main(t_screen.provider, t_actives.home);
    GtkGesture *click_home = gtk_gesture_click_new();
    gtk_gesture_set_state(click_home, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_home, "pressed", G_CALLBACK(show_home), NULL);
    gtk_widget_add_controller(t_actives.home, GTK_EVENT_CONTROLLER(click_home));
    gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.home);

    gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.settings);
    if(cur_client.theme == DARK_THEME)
        t_actives.settings = gtk_image_new_from_file("client/media/settings.png");
    else 
        t_actives.settings = gtk_image_new_from_file("client/media/setting_light.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.settings), "settings_icon");
    load_css_main(t_screen.provider, t_actives.settings);
    GtkGesture *click_settings = gtk_gesture_click_new();
    gtk_gesture_set_state(click_settings, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_settings, "pressed", G_CALLBACK(show_settings), NULL);
    gtk_widget_add_controller(t_actives.settings, GTK_EVENT_CONTROLLER(click_settings));
    gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.settings);


    if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0) {
        char buf[512+32] = {0};
        sprintf(buf, "<chat users avatars>%d", cur_client.cur_chat.id);
        send_all(cur_client.ssl,buf, 512+32);
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
    gtk_grid_set_column_spacing (GTK_GRID(chat_headerbar), 515); //535

    GtkWidget *chat_headerbar_left = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(GTK_WIDGET(chat_headerbar_left), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(chat_headerbar_left), GTK_ALIGN_CENTER);
    GtkWidget *photo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *photo =  NULL;
   
    if(mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) == 0) {
        char buf[544] = {0};
        t_main.loaded = false;
        t_list *temp = cur_client.cur_chat.users;
        if (mx_strcmp (cur_client.login, temp->data) != 0)
            sprintf(buf, "<get user avatar>%s",temp->data);
        else 
            sprintf(buf, "<get user avatar>%s",temp->next->data);
        send_all(cur_client.ssl, buf, 544);
        while(!t_main.loaded) {
            usleep(50);
        }
        t_avatar *avatar = t_main.loaded_avatar;
        if (mx_strcmp(avatar->name, "default") == 0) {
            *avatar = t_main.default_avatar;
        }
        photo = get_circle_widget_from_png_avatar(avatar, 50, 50, true);
    }
    else {
        photo = get_circle_widget_from_png_avatar(&cur_client.cur_chat.avatar, 50, 50, true);
    }

    gtk_box_append(GTK_BOX(photo_box), photo);
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(info_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(info_box), GTK_ALIGN_CENTER);
    GtkWidget *group_or_user_name = NULL;
    if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) == 0) {
        t_list *logins = cur_client.cur_chat.users;
        while (logins) {
            if (mx_strcmp(logins->data, cur_client.login) != 0) {
                group_or_user_name = gtk_label_new(logins->data);
                break;
            }
            logins = logins->next;
        }
    }
    else {
        group_or_user_name = gtk_label_new(cur_client.cur_chat.name);
    }

    gtk_widget_set_name(GTK_WIDGET(group_or_user_name), "chat_name_inside_label");
    load_css_main(t_screen.provider, group_or_user_name);
    GtkWidget *status = gtk_label_new("last seen 13 minutes ago");
    gtk_widget_set_name(GTK_WIDGET(status), "last_online");
    load_css_main(t_screen.provider, status);
    gtk_box_append(GTK_BOX(info_box), group_or_user_name);
    gtk_box_append(GTK_BOX(info_box), status);

    gtk_box_append(GTK_BOX(chat_headerbar_left), photo_box);
    gtk_box_append(GTK_BOX(chat_headerbar_left), info_box);

    GtkWidget *chat_headerbar_right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(GTK_WIDGET(chat_headerbar_right), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(chat_headerbar_right), GTK_ALIGN_CENTER);
    //GtkWidget *block_img_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *block_img = NULL;
    if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) == 0) 
    {
        if(cur_client.theme == DARK_THEME)
            block_img = gtk_image_new_from_file("client/media/block_btn.png");
        else block_img = gtk_image_new_from_file("client/media/block_btn_light.png");
        gtk_widget_set_size_request(block_img, 35, 35);
    }
    else
    {
        if(cur_client.theme == DARK_THEME)
            block_img = gtk_image_new_from_file("client/media/settings_group.png");
        else block_img = gtk_image_new_from_file("client/media/settings_group_light.png");
        gtk_widget_set_size_request(block_img, 35, 35);
        GtkGesture *settings_group = gtk_gesture_click_new();
        gtk_gesture_set_state(settings_group, GTK_EVENT_SEQUENCE_CLAIMED);
        g_signal_connect_swapped(settings_group, "pressed", G_CALLBACK(show_group_settings), data);
        gtk_widget_add_controller(block_img, GTK_EVENT_CONTROLLER(settings_group));
    }

    //GtkWidget *mute_img_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *mute_img = NULL;
    if(cur_client.theme == DARK_THEME)
        mute_img = gtk_image_new_from_file("client/media/mute_btn.png");
    else mute_img = gtk_image_new_from_file("client/media/mute_btn_light.png");
    //GtkWidget *mute_img = gtk_image_new_from_file("client/media/mute_inactive.png");
    gtk_widget_set_size_request(mute_img, 35, 35);
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

    g_signal_connect(t_main.scrolled_window_right, "edge-reached", G_CALLBACK(load_more_messages), &cur_client.cur_chat);

    GtkWidget *write_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);////findme
    gtk_widget_set_name(GTK_WIDGET(write_box), "write_message_box");
    load_css_main(t_screen.provider, write_box);
    gtk_widget_set_halign(GTK_WIDGET(write_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(write_box), GTK_ALIGN_START);
    gtk_box_set_spacing(GTK_BOX(write_box), 5);
    GtkWidget *write_message = gtk_text_view_new();
    gtk_widget_set_name(GTK_WIDGET(write_message), "write_message");
    gtk_widget_set_size_request(write_message, 750, 0);
    load_css_main(t_screen.provider, write_message);
    GtkTextBuffer *bio_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (write_message));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(write_message), GTK_WRAP_WORD_CHAR);
    t_main.message_input_view = write_message;

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
    GtkWidget *stickers = NULL;
    if(cur_client.theme == DARK_THEME)
        stickers = gtk_image_new_from_file("client/media/sticker_dark.png");
    else 
        stickers = gtk_image_new_from_file("client/media/sticker_light.png");
    //GtkWidget *stickers = gtk_image_new_from_file("client/media/sticker.png");
    gtk_widget_set_size_request(stickers, 27, 27);
    GtkWidget *attach_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *attach = NULL;
    if(cur_client.theme == DARK_THEME)
        attach = gtk_image_new_from_file("client/media/attach_img_dark.png");
    else 
        attach = gtk_image_new_from_file("client/media/attach_img_light.png");
    gtk_widget_set_size_request(attach, 27, 27);
    gtk_box_append(GTK_BOX(attach_box), attach);
    
    GtkGesture *click_attach = gtk_gesture_click_new();
    gtk_gesture_set_state(click_attach, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_attach, "pressed", G_CALLBACK(choise_file), NULL);
    gtk_widget_add_controller(attach, GTK_EVENT_CONTROLLER(click_attach));

    GtkWidget **resize = (GtkWidget **)malloc(7 * sizeof(GtkWidget *));
    resize[0] = write_message;
    resize[1] = chat_headerbar;
    resize[2] = write_box;
    resize[3] = stickers_box;
    resize[4] = stickers;
    resize[5] = data;
    resize[6] = write_message_scroll;

    GtkGesture *click_sstickers = gtk_gesture_click_new();
    gtk_gesture_set_state(click_sstickers, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_sstickers, "pressed", G_CALLBACK(show_stickers), (gpointer)resize);
    gtk_widget_add_controller(stickers, GTK_EVENT_CONTROLLER(click_sstickers));
    gtk_box_append(GTK_BOX(stickers_box), stickers);

    gtk_box_append(GTK_BOX(write_box), write_message_scroll);
    gtk_box_append(GTK_BOX(write_box), stickers_box);
    gtk_box_append(GTK_BOX(write_box), attach_box);

    gtk_box_append(GTK_BOX(t_main.right_panel), chat_headerbar);
    gtk_box_append(GTK_BOX(t_main.right_panel), t_main.scrolled_window_right);
    gtk_box_append(GTK_BOX(t_main.right_panel), write_box);

    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);
    
    get_messages_from_server(cur_client.cur_chat.id, -1);
    t_main.first_load_mes = true;
}

void chat_show_main_screen(GtkWidget *window) 
{
    switch(cur_client.theme) {
        case DARK_THEME:
            on_dark_theme();
            break;
        case LIGHT_THEME:
            on_light_theme();
            break;
        default:
            on_dark_theme();
            break;
    }
    //gtk_css_provider_load_from_path(t_screen.provider,"client/themes/dark_chat.css");
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
    GtkWidget *SearchField;
    t_main.search_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(t_main.search_panel, 200, 45);
    gtk_widget_set_halign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.search_panel), GTK_ALIGN_CENTER);
    //gtk_widget_set_margin_bottom(GTK_WIDGET(t_main.search_panel), 27);
    gtk_widget_set_margin_top(GTK_WIDGET(t_main.search_panel), 15);
    t_actives.search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_actives.search_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(t_actives.search_box), GTK_ALIGN_CENTER);
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
    //gtk_widget_add_controller(t_actives.search_box, GTK_EVENT_CONTROLLER(click_search_field));
    g_signal_connect (SearchField, "notify::text", G_CALLBACK (text_changed_main_screen), NULL);
    
    ///////////

    load_css_main(t_screen.provider, SearchField);
    gtk_box_append(GTK_BOX(t_actives.search_box), GTK_WIDGET(SearchField));
    GFile *path = g_file_new_for_path("client/media/search_ico.png"); 
    GIcon *icon = g_file_icon_new(path);
    gtk_entry_set_icon_from_gicon(GTK_ENTRY(SearchField),GTK_ENTRY_ICON_SECONDARY, icon);
    
    t_actives.home = gtk_image_new_from_file("client/media/home.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.home ), "home_icon");
    load_css_main(t_screen.provider, t_actives.home );
    GtkGesture *click_home = gtk_gesture_click_new();
    gtk_gesture_set_state(click_home, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_home, "pressed", G_CALLBACK(show_home), NULL);
    gtk_widget_add_controller(t_actives.home, GTK_EVENT_CONTROLLER(click_home));
    t_actives.settings = gtk_image_new_from_file("client/media/settings.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.settings), "settings_icon");
    load_css_main(t_screen.provider, t_actives.settings);
    GtkGesture *click_settings = gtk_gesture_click_new();
    gtk_gesture_set_state(click_settings, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_settings, "pressed", G_CALLBACK(show_settings), NULL);
    gtk_widget_add_controller(t_actives.settings, GTK_EVENT_CONTROLLER(click_settings));

    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(t_main.logo));
    gtk_box_append(GTK_BOX(t_main.search_panel), GTK_WIDGET(t_actives.search_box));
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
