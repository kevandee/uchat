#include "../inc/uch_client.h"

static void on_open_response (GtkDialog *dialog, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

        GFile *file = gtk_file_chooser_get_file (chooser);
        
        printf("file %s\n", g_file_get_basename(file));
        char buf[544] = {0};
        char *name = g_file_get_basename(file);
        if (mx_strstr(name, ".txt")) {
            sprintf(buf, "<file chat_id=%d, name=%s, mode=%s>", cur_client.cur_chat.id,   name, "w");
            send_all(cur_client.ssl, buf, 544);
            send_file(cur_client.ssl, g_file_get_path(file), "r");
        }
        else {
            sprintf(buf, "<file chat_id=%d, name=%s, mode=%s>", cur_client.cur_chat.id,  name, "wb");
            send_all(cur_client.ssl, buf, 544);
            send_file(cur_client.ssl, g_file_get_path(file), "rb");
        }

        GtkWidget *file_icon = gtk_image_new(); 
        gtk_widget_set_size_request(file_icon, 45, 45);

        

        gtk_box_append(GTK_BOX (t_main.scroll_box_right), file_icon);
    }

    gtk_window_destroy (GTK_WINDOW (dialog));
}

void choise_file() {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW (t_screen.main_window), action, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    //GtkFileFilter *image_filter = gtk_file_filter_new();

    //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), image_filter);
    gtk_widget_show (dialog);

    g_signal_connect (dialog, "response", G_CALLBACK (on_open_response), NULL);
}
