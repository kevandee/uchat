#include "../inc/uch_client.h"

static void send_and_choice_new_dialog(GtkWidget *widget, gpointer data) {
    char *users = NULL;
    t_chat *chat = (t_chat *)data;
    t_list *temp = chat->users;
    while (temp) {
        users = mx_strrejoin(users, temp->data);
        users = mx_strrejoin(users, " ");

        temp=temp->next;
    }

    t_main.loaded = false;
    char buf[512 + 32] = {0};
    cur_client.sender_new_chat = true;
    sprintf(buf, "<add chat, name=.dialog>%s", users);
    
    swiftchat_send(cur_client.ssl, buf, 512+32);
    while (!t_main.loaded) {
        usleep(50);
    }
    temp = cur_client.chats;
    while (temp->next) {
        temp = temp->next;
    }
    
    gtk_widget_hide(widget);
    show_chat_history(widget, temp->data);
    //return_to_chatlist(NULL, NULL);
}

void redraw_actives_chats(GtkWidget *widget, gpointer data)
{
    (void)widget;
    int point;
    if(data == NULL)
        point = 0;
    else point = 1;

    t_list *temp_widgets = t_main.chat_nodes_info;
    t_list *temp_ch = cur_client.chats;
    if(point == 1)
    {
        while (temp_ch)
        {
            gtk_widget_set_name (gtk_widget_get_parent(temp_widgets->data), "scroll_buttons_border");
            temp_widgets = temp_widgets->next;
            temp_ch = temp_ch->next;
        }
        return;
    }

    while (temp_ch && temp_widgets) 
    {
        if (((t_chat *)temp_ch->data)->id == cur_client.cur_chat.id) 
        {
            gtk_widget_set_name (gtk_widget_get_parent(temp_widgets->data), "scroll_button_active");
            if(((t_chat *)temp_ch->data)->is_enter == false && ((t_chat *)temp_ch->data)->notify)
            {
                ((t_chat *)temp_ch->data)->is_enter = true;
                GtkWidget *notify_circle = gtk_widget_get_last_child(temp_widgets->data);
                gtk_box_remove(GTK_BOX(temp_widgets->data), notify_circle);
            }
            temp_widgets = temp_widgets->next;
            temp_ch = temp_ch->next;
            
        }
        else
        {
            gtk_widget_set_name (gtk_widget_get_parent(temp_widgets->data), "scroll_buttons_border");
            temp_widgets = temp_widgets->next;
            temp_ch = temp_ch->next;
        }
    }
}


///gboolean add_msg(gpointer data)
gboolean add_chat_node(gpointer data) {
    t_chat *chat = data;
    if (!chat) {
        perror("unexpected error!\n");
    }

    GtkWidget *child_widget = gtk_button_new ();
    gtk_widget_set_size_request(child_widget, 200, 54);
    gtk_widget_set_name(GTK_WIDGET(child_widget), "scroll_buttons_border");
    load_css_main(t_screen.provider, child_widget);
    GtkWidget *chat_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(chat_info), "scroll_buttons");
    load_css_main(t_screen.provider, chat_info);

    if (mx_strcmp(chat->name, ".new_dialog") == 0) {
        char buf[544] = {0};
        t_main.loaded = false;
        sprintf(buf, "<get user avatar>%s", chat->users->data);
        swiftchat_send(cur_client.ssl, buf, 544);
        while(!t_main.loaded) {
            usleep(50);
        }
        chat->avatar = *t_main.loaded_avatar;
        if (mx_strcmp(chat->avatar.name, "default") == 0) {
            chat->avatar = t_main.default_avatar;
        }
    }
    else if (mx_strncmp(chat->name, ".dialog", 7) == 0 && chat->is_new && !chat->is_avatar) {
        chat->is_new = false;
        char buf[544] = {0};
        t_main.loaded = false;
        if (mx_strcmp(cur_client.login, chat->users->data) != 0)
            sprintf(buf, "<get user avatar>%s", chat->users->data);
        else 
            sprintf(buf, "<get user avatar>%s", chat->users->next->data);
        swiftchat_send(cur_client.ssl, buf, 544);
        while(!t_main.loaded) {
            usleep(50);
        }
        chat->avatar = *t_main.loaded_avatar;
        if (mx_strcmp(chat->avatar.name, "default") == 0) {
            chat->avatar = t_main.default_avatar;
        }
    }
    else if (chat->is_new) {
        chat->avatar = t_main.default_group_avatar;
    }

    GtkWidget *chat_image = get_circle_widget_from_png_avatar(&chat->avatar, 57, 57, false);
    gtk_widget_set_size_request(GTK_WIDGET(chat_image),  57, 0);
    gtk_widget_set_halign(GTK_WIDGET(chat_image), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(chat_image), GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX(chat_info), chat_image);

    GtkWidget *chat_name = NULL;

    if (mx_strcmp(chat->name, ".new_dialog") == 0){
        chat_name = gtk_label_new(chat->users->data);
    }
    else if (mx_strncmp(chat->name, ".dialog", 7) == 0) {
        t_list *logins = chat->users;
        

        while (logins) {
            if (mx_strcmp(logins->data, cur_client.login) != 0) {
                chat_name = gtk_label_new(logins->data);
                break;
            }
            logins = logins->next;
        }
    }
    else {
        chat_name = gtk_label_new(chat->name);
    }

    gtk_widget_set_name(GTK_WIDGET(chat_name), "chat_name");
    load_css_main(t_screen.provider, chat_name);

    gtk_widget_set_size_request(GTK_WIDGET(chat_name), 0, 0);
    gtk_widget_set_halign(GTK_WIDGET(chat_name), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(chat_name), GTK_ALIGN_CENTER);
    
    gtk_box_append (GTK_BOX(chat_info), chat_name);

    gtk_button_set_child(GTK_BUTTON (child_widget), chat_info);

    //переход в историю чатов, в файле мейн скрин
    if (mx_strcmp(chat->name, ".new_dialog") == 0) {
        g_signal_connect(child_widget, "clicked", G_CALLBACK(send_and_choice_new_dialog), chat);
        g_signal_connect(child_widget, "clicked", G_CALLBACK(redraw_actives_chats), NULL);
    }
    else { 
        g_signal_connect(child_widget, "clicked", G_CALLBACK(show_chat_history), chat);
        g_signal_connect(child_widget, "clicked", G_CALLBACK(redraw_actives_chats), NULL);
        mx_push_back(&t_main.chat_nodes_info, chat_info);
    }
    gtk_box_append(GTK_BOX(t_main.scroll_box_left), child_widget);
    return FALSE;
}
