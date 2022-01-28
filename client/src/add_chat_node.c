#include "../inc/uch_client.h"

static void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

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
    send_all(cur_client.serv_fd, buf, 512+32);
    while (!t_main.loaded) {
        usleep(50);
    }
    temp = cur_client.chats;
    while (temp->next) {
        temp = temp->next;
    }

    show_chat_history(widget, temp->data);
}

void add_chat_node(t_chat *chat) {
    GtkWidget *child_widget = gtk_button_new ();
    gtk_widget_set_size_request(child_widget, 200, 54);
    gtk_widget_set_name(GTK_WIDGET(child_widget), "scroll_buttons_border");
    load_css_main(t_screen.provider, child_widget);
    GtkWidget *chat_info = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(chat_info), "scroll_buttons");
    load_css_main(t_screen.provider, chat_info);

    GtkWidget *chat_image = get_circle_widget_from_png_custom("test_circle.png", 57, 57);
    gtk_widget_set_size_request(GTK_WIDGET(chat_image),  57, 0);
    gtk_widget_set_halign(GTK_WIDGET(chat_image), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(chat_image), GTK_ALIGN_CENTER);
    gtk_box_append (GTK_BOX(chat_info), chat_image);

    GtkWidget *chat_name = NULL;

    if (mx_strcmp(chat->name, ".new_dialog") == 0){
        chat_name = gtk_label_new(chat->users->data);
    }
    else if (mx_strcmp(chat->name, ".dialog") == 0) {
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
    }
    else { 
        g_signal_connect(child_widget, "clicked", G_CALLBACK(show_chat_history), chat);
    }
    gtk_box_append(GTK_BOX(t_main.scroll_box_left), child_widget);
}
