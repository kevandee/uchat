#include "../inc/uch_client.h"

static bool is_dialog(char *name) {
    t_list *temp = cur_client.chats;
    while (temp) {
        t_chat *chat = ((t_chat *)(temp->data));
        if (mx_strncmp(".dialog", chat->name, 7) == 0 
        && mx_list_size(chat->users) == 2
        && (mx_strcmp(chat->users->data, name) == 0 
        || mx_strcmp(chat->users->next->data, name) == 0 )) {
            return true;
        } 

        temp = temp->next;
    }

    return false;
}

static void get_list_users() {
    t_main.loaded = false;
    char message[512+32] = {0};
    sprintf(message, "<users list>");
    swiftchat_send(cur_client.ssl, message, 512+32);
    while (!t_main.loaded)
        usleep(50);
}

void text_changed_main_screen(GObject *object, GParamSpec *pspec, gpointer data) {
    (void)data;
    (void)pspec;
    if (!t_main.connected) {
        return;
    }
    
    GtkEntry *entry = GTK_ENTRY (object);
    gboolean has_text;

    has_text = gtk_entry_get_text_length (entry) > 0;
    
    if (gtk_entry_get_text_length (entry) == 1 && !t_main.search_users_list) {
        get_list_users();
    }

    if(has_text) {
        t_main.scroll_box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_size_request(GTK_WIDGET(t_main.scroll_box_left), 200, 0);
        gtk_widget_set_size_request(GTK_WIDGET(t_main.scrolled_window_left), 200, 600);
        const char *entry_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
        t_list *res = NULL;
        t_list *search = t_main.search_users_list;
        while (search) {
            if (mx_strncmp(entry_text, search->data, mx_strlen(entry_text)) == 0 && !is_dialog(search->data)) {
                if (mx_strcmp(search->data, cur_client.login) != 0)
                    mx_push_back(&res, search->data);
            }

            search = search -> next;
        }

        t_list *chats = res;
        while (chats) {
            t_chat *new_c = (t_chat *)malloc(sizeof (t_chat));
            new_c->count_users = 1;
            new_c->messages = NULL;
            new_c->users = NULL;
            mx_strcpy(new_c->name,".new_dialog");
            mx_push_back(&new_c->users, mx_strdup(chats->data));
            add_chat_node(new_c);

            chats = chats->next;
        }

        mx_clear_list(&res);
        res = NULL;
        search = cur_client.chats;
        while (search) {
            if (mx_strncmp(entry_text, ((t_chat *)(search->data))->name, mx_strlen(entry_text)) == 0){
                 mx_push_front(&res, search->data);
            }
            else if (mx_strncmp(".dialog", ((t_chat *)(search->data))->name, 7) == 0){
                if (mx_strncmp(entry_text, ((t_chat *)(search->data))->users->data, mx_strlen(entry_text)) == 0
                || mx_strncmp(entry_text, ((t_chat *)(search->data))->users->next->data, mx_strlen(entry_text)) == 0)
                    mx_push_front(&res, search->data);
            }

            search = search->next;
        }

        chats = res;
        mx_clear_list(&t_main.chat_nodes_info);
        t_main.chat_nodes_info = NULL;
        while (chats) {
            add_chat_node(chats->data);
            chats = chats->next;
        }
        
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW (t_main.scrolled_window_left),t_main.scroll_box_left);
    }
    else {
        return_to_chatlist(NULL, NULL);
    }
}

void text_changed_add_chat(GObject *object, GParamSpec *pspec, gpointer data) {
    (void)pspec;
    (void)data;
    if (!t_main.connected) {
        return;
    }
    GtkEntry *entry = GTK_ENTRY (object);
    //GtkWidget *scroll_window = data;
    gboolean has_text;

    has_text = gtk_entry_get_text_length (entry) > 0;
    
    if(has_text) {
        const char *entry_text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));

        t_list *temp = t_main.check_buttons_user_list;
        int row = 0;
        while (temp) {

            
            GtkWidget *parent = gtk_grid_get_child_at(GTK_GRID (t_main.add_users_grid), 1, row);
            GtkWidget *cur_button = gtk_widget_get_last_child (parent);
            const char *label = gtk_label_get_text (GTK_LABEL (cur_button));
            if (mx_strcmp(cur_client.login, label) != 0) {
                if (mx_strncmp(entry_text, label, mx_strlen(entry_text)) == 0) {
                    if (!gtk_widget_get_visible(GTK_WIDGET (parent))) {
                        gtk_widget_show(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID (t_main.add_users_grid), 0, row)));
                        gtk_widget_show(GTK_WIDGET (parent));
                        gtk_widget_show(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID (t_main.add_users_grid), 2, row)));
                    }
                }
                else {
                    if (gtk_widget_get_visible(GTK_WIDGET (parent))) {
                        gtk_widget_hide(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID (t_main.add_users_grid), 0, row)));
                        gtk_widget_hide(GTK_WIDGET (parent));
                        gtk_widget_hide(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID (t_main.add_users_grid), 2, row)));
                    }
                }
            }
            row++;
            temp = temp->next;
        }
    }
    else {
        t_list *temp = t_main.check_buttons_user_list;
        int row = 0;
        while (temp) {
            GtkWidget *parent = gtk_grid_get_child_at(GTK_GRID (t_main.add_users_grid), 1, row);
            GtkWidget *cur_button = gtk_widget_get_last_child (parent);
            const char *label = gtk_label_get_text (GTK_LABEL (cur_button));
            if (mx_strcmp(cur_client.login, label) != 0) {
                if (!gtk_widget_get_visible(GTK_WIDGET (parent))) {
                    gtk_widget_show(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID (t_main.add_users_grid), 0, row)));
                    gtk_widget_show(GTK_WIDGET (parent));
                    gtk_widget_show(GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID (t_main.add_users_grid), 2, row)));
                }
            }
            row++;
            temp = temp->next;
        }
    }
}
