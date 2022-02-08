#include "../inc/uch_client.h"

static void on_open_response (GtkDialog *dialog, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

        GFile *file = gtk_file_chooser_get_file (chooser);
        
        t_main.choosed_dir = g_file_get_path(file);

        char buf[544] = {0};
        sprintf(buf, "<get file chat_id=%d, mes_id=%d>", cur_client.cur_chat.id, t_main.choosed_file->id);
        swiftchat_send(cur_client.ssl, buf, 544);
    }

    gtk_window_destroy (GTK_WINDOW (dialog));
}


void choise_dir(GtkGestureClick *gesture, int n_press, double x, double y, gpointer data) {
    (void)gesture;
    (void)n_press;
    (void)data;
    (void)x;
    (void)y;
    t_main.choosed_file = data;
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;

    dialog = gtk_file_chooser_dialog_new ("Choose dir", GTK_WINDOW (t_screen.main_window), action, "Cancel", GTK_RESPONSE_CANCEL, "Choose", GTK_RESPONSE_ACCEPT, NULL);
    gtk_widget_show (dialog);

    g_signal_connect (dialog, "response", G_CALLBACK (on_open_response), NULL);
}
