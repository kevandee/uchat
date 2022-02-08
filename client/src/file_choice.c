#include "../inc/uch_client.h"

const char *file_types[] = {"*.txt", "*.mp3", "*.wav", "*.jpeg", "*.jpg", "*.png", "*.bmp", "*.zip", "*.rar", "*.tar", NULL};

static void on_open_response (GtkDialog *dialog, int response)
{
    if (!t_main.connected) {
        return;
    }////!!!!!!!!!

    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

        GFile *file = gtk_file_chooser_get_file (chooser);

        char buf[544] = {0};
        char *name = g_file_get_basename(file);
        if (mx_strstr(name, ".txt")) {
            sprintf(buf, "<file chat_id=%d, name=%s, mode=%s>", cur_client.cur_chat.id,   name, "w");
            swiftchat_send(cur_client.ssl, buf, 544);
            send_file(cur_client.ssl, g_file_get_path(file), "r");
        }
        else {
            sprintf(buf, "<file chat_id=%d, name=%s, mode=%s>", cur_client.cur_chat.id,  name, "wb");
            swiftchat_send(cur_client.ssl, buf, 544);
            send_file(cur_client.ssl, g_file_get_path(file), "rb");
        }

        GtkWidget *file_icon = NULL; 
        

        if (mx_strstr(name, ".txt")) {
            file_icon = gtk_image_new_from_file("client/media/txt.png");
        }
        else if(mx_strstr(name, ".mp3") || mx_strstr(name, ".wav")) {
            file_icon = gtk_image_new_from_file("client/media/music.png");
        }
        else if (mx_strstr(name, ".jpeg") || mx_strstr(name, ".jpg") || mx_strstr(name, ".png") || mx_strstr(name, ".bmp")) {
            file_icon = gtk_image_new_from_file("client/media/image.png");
        }
        else if (mx_strstr(name, ".zip") || mx_strstr(name, ".rar") || mx_strstr(name, ".tar")) {
            file_icon = gtk_image_new_from_file("client/media/zip.png");
        }
        
        gtk_widget_set_size_request(file_icon, 45, 45);

        GtkWidget *file_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_halign(file_box, GTK_ALIGN_END);
        gtk_widget_set_margin_end(file_box, 5);
        gtk_widget_set_margin_bottom(file_box, 5);

        GtkWidget *file_name_label = gtk_label_new(name);
        gtk_widget_set_name(GTK_WIDGET(file_name_label), "message");
        load_css_main(t_screen.provider, file_name_label);

        GtkWidget *file_sender = NULL;
        if(mx_strncmp(cur_client.cur_chat.name, ".dialog", 7)!=0)
            file_sender = get_circle_widget_current_user_avatar();
        time_t t;
        time(&t);
        char *time = mx_strndup(ctime(&t) + 11, 5);
        GtkWidget *file_action_time = gtk_label_new(time);
        gtk_widget_set_name(file_action_time, "user_action");
        load_css_main(t_screen.provider, file_action_time);
        gtk_box_append(GTK_BOX(file_box), file_action_time);

        gtk_box_append(GTK_BOX(file_box), file_icon);
        gtk_box_append(GTK_BOX(file_box), file_name_label);
        if(file_sender)
            gtk_box_append(GTK_BOX(file_box), file_sender);
        gtk_box_append(GTK_BOX (t_main.scroll_box_right), file_box);
    }

    gtk_window_destroy (GTK_WINDOW (dialog));
}

void choise_file() {
    if (!t_main.connected) {
        return;
    }
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW (t_screen.main_window), action, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    GtkFileFilter *file_filter = gtk_file_filter_new();

    for (int i = 0; file_types[i]; i++) {
        gtk_file_filter_add_pattern(file_filter, file_types[i]);
    }
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), file_filter);
    gtk_widget_show (dialog);

    g_signal_connect (dialog, "response", G_CALLBACK (on_open_response), NULL);
}
