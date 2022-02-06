#include "../inc/uch_client.h"

GtkWidget *get_icon_from_filename(const char *name) {
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

    return file_icon;
}
