#include "../inc/uch_client.h"

static void on_open_response (GtkDialog *dialog, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

        GFile *file = gtk_file_chooser_get_file (chooser);
        
        t_main.choosed_dir = g_file_get_path(file);
        printf("%s\n", t_main.choosed_dir);
    }

    gtk_window_destroy (GTK_WINDOW (dialog));
}


void choise_dir() {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;

    dialog = gtk_file_chooser_dialog_new ("Choose dir", GTK_WINDOW (t_screen.main_window), action, "Cancel", GTK_RESPONSE_CANCEL, "Choose", GTK_RESPONSE_ACCEPT, NULL);
    gtk_widget_show (dialog);

    g_signal_connect (dialog, "response", G_CALLBACK (on_open_response), NULL);
}
