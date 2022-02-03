#include "../inc/uch_client.h"


static void load_css_main(GtkCssProvider *provider, GtkWidget *widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

}

static void insert_text_bio(GtkTextBuffer *buffer, GtkTextIter *location)
{
    gint count=gtk_text_buffer_get_char_count(buffer);

    GtkTextIter start, end, offset;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    const char *buf_str = gtk_text_buffer_get_text(buffer, &start, &end, true);
    if(buf_str[count-1] == '\n' && count > 1) {
        gtk_text_buffer_get_iter_at_offset(buffer, &offset, count - 1);
        gtk_text_buffer_delete(buffer, &offset, &end);
        gtk_text_iter_assign(location, &offset);
        return;
    }

    if(count>256) {
        gtk_text_buffer_get_iter_at_offset(buffer, &offset, 256);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_delete(buffer, &offset, &end);
        gtk_text_iter_assign(location, &offset);
    }
}



static void show_circle_range(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data);


gboolean user_function (GtkEventControllerScroll *controller, double dx, double dy,gpointer user_data) {
    (void)controller;
    (void)dx;

    double scaled_w = cur_client.avatar.orig_w+(cur_client.avatar.d_width + 5*dy);
    double scaled_h = cur_client.avatar.orig_h+(cur_client.avatar.d_width + 5*dy) * (cur_client.avatar.orig_h/cur_client.avatar.orig_w);
    if (cur_client.avatar.x + scaled_w > 500 && cur_client.avatar.y + scaled_h > 500) {
        cur_client.avatar.d_width += 5*dy;
    }
    gtk_widget_queue_draw(GTK_WIDGET (user_data));
    return TRUE;
}

GtkWidget *circle_range (cairo_surface_t *file) {
    GtkWidget *darea = NULL;

    darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 700);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 700);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), show_circle_range, file, NULL);

    return GTK_WIDGET (darea);
}

static void show_circle_range(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data) {
    (void)widget;
    (void)w;
    (void)h;
    (void)data;
    cairo_surface_t *image = data;
    gdouble org_width, org_height;
    org_width = cairo_image_surface_get_width(image);
    org_height = cairo_image_surface_get_height(image);
    
    cur_client.avatar.orig_w = org_width;
    cur_client.avatar.orig_h = org_height;

    cairo_surface_t *scaled_image = scale_to_half(image, org_width, org_height, org_width+ cur_client.avatar.d_width, org_height+cur_client.avatar.d_width * (org_height/org_width));
    org_width = cairo_image_surface_get_width(scaled_image);
    org_height = cairo_image_surface_get_height(scaled_image);
    cur_client.avatar.scaled_w = org_width;
    cur_client.avatar.scaled_h = org_height;

    cairo_set_source_surface (cr, scaled_image, cur_client.avatar.x, cur_client.avatar.y); 
    
    cairo_paint(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);

    cairo_rectangle (cr, cur_client.avatar.x, cur_client.avatar.y, cairo_image_surface_get_width(scaled_image), cairo_image_surface_get_height(scaled_image));
    cairo_set_source_rgba (cr, 0.0, 0, 0, 0.8);
    cairo_fill (cr);

    org_width = cairo_image_surface_get_width(image);
    org_height = cairo_image_surface_get_height(image);

    cairo_set_source_surface (cr, scaled_image, cur_client.avatar.x, cur_client.avatar.y); 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

    cairo_arc(cr, w/2, h/2, 150, 0, 2 * M_PI);
    
    cairo_clip(cr);
    
    cairo_paint(cr);
    cairo_fill(cr);
}

static void begin_move_image(GtkWidget *widget, gpointer data) {
    (void)widget;
    t_list *list = data;

    *((double *)list->next->data) = cur_client.avatar.x; 
    *((double *)list->next->next->data) = cur_client.avatar.y;
}

static void move_image (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer data) {
    (void)gesture;
    t_list *list = data;
    GtkWidget *darea = GTK_WIDGET (list->data);
    double prev_x = *((double *)(list->next->data));
    double prev_y = *((double *)(list->next->next->data));

    if (prev_x + offset_x < 200 && prev_x + offset_x + cur_client.avatar.scaled_w > 500) {
        cur_client.avatar.x=prev_x + offset_x;
    }
    if (prev_y + offset_y < 200 && prev_y + offset_y + cur_client.avatar.scaled_h > 500) {
        cur_client.avatar.y=prev_y + offset_y;
    }
    gtk_widget_queue_draw(darea);   

}

static void send_avatar() {
    char buf[512 + 32] = {0};
    sprintf(buf, "%s", "<setting avatar>");
    send_all(cur_client.ssl, buf, 512 + 32);
    clear_message(buf, 512 + 32);
    
    sprintf(buf, "%s", cur_client.avatar.name);
    send_all(cur_client.ssl, buf, 512 + 32);
    clear_message(buf, 512 + 32);
    t_main.loaded = false;
    send_image(cur_client.ssl, cur_client.avatar.path);
    
    while (!t_main.loaded) {
        usleep(50);
    }


    SSL_write(cur_client.ssl, &cur_client.avatar.scaled_w, sizeof(double));
    SSL_write(cur_client.ssl, &cur_client.avatar.scaled_h, sizeof(double));

    SSL_write(cur_client.ssl, &cur_client.avatar.x, sizeof(double));
    SSL_write(cur_client.ssl, &cur_client.avatar.y, sizeof(double));

    t_main.loaded = false;
    while (!t_main.loaded) {
        usleep(50);
    }
    gtk_widget_hide(t_main.logo);
    t_main.logo = get_circle_widget_from_png_avatar(&cur_client.avatar, 45, 45, true);
    gtk_widget_set_name(GTK_WIDGET(t_main.logo), "account_avatar");
    load_css_main(t_screen.provider, t_main.logo);
    gtk_box_prepend(GTK_BOX (t_main.search_panel), t_main.logo);
    show_settings();
}

static void avatar_range(GFile *file) {
    cairo_surface_t *image = get_surface_from_jpg(g_file_get_path(file));

    GtkWidget *darea = circle_range(image);
    gtk_widget_set_size_request(darea , 700, 700);
    
    gtk_grid_remove(GTK_GRID(t_main.grid), t_main.right_panel);
    t_main.right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(t_main.right_panel, 1000,700);
    gtk_widget_set_halign(GTK_WIDGET( t_main.right_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET( t_main.right_panel), GTK_ALIGN_CENTER);

    gtk_widget_set_halign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);
    
    GtkWidget *apply_button = gtk_button_new_with_label("Apply");
    gtk_widget_set_size_request(GTK_WIDGET (apply_button), 200, 0);
    gtk_widget_set_halign(GTK_WIDGET(apply_button), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(apply_button), GTK_ALIGN_CENTER);

    g_signal_connect(apply_button, "clicked", G_CALLBACK(send_avatar), NULL);

    gtk_box_append(GTK_BOX (t_main.right_panel), darea);
    gtk_box_append(GTK_BOX (t_main.right_panel), apply_button);
    
    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);

    GtkEventController *scroll_controller = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    g_signal_connect(scroll_controller, "scroll", G_CALLBACK(user_function), darea);
    gtk_widget_add_controller(t_main.right_panel, GTK_EVENT_CONTROLLER (scroll_controller));

    GtkGesture *drag_controller = gtk_gesture_drag_new();
    double *prev_x = (double *)malloc(sizeof(double)), *prev_y = (double *)malloc(sizeof(double));
    t_list *data = NULL;
    mx_push_back(&data, darea);
    mx_push_back(&data, prev_x);
    mx_push_back(&data, prev_y);
    g_signal_connect(drag_controller, "drag-update", G_CALLBACK(move_image), data);
    g_signal_connect(drag_controller, "drag-begin", G_CALLBACK(begin_move_image), data);
    gtk_widget_add_controller(darea, GTK_EVENT_CONTROLLER(drag_controller));
}

static void on_open_response (GtkDialog *dialog, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

        GFile *file = gtk_file_chooser_get_file (chooser);
        
        cur_client.avatar.name = mx_strdup(g_file_get_basename(file));
        cur_client.avatar.path = mx_strdup(g_file_get_path(file));
    
        avatar_range(file);
    }

    gtk_window_destroy (GTK_WINDOW (dialog));
}

static void choise_photo_file() {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW (t_screen.main_window), action, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    GtkFileFilter *image_filter = gtk_file_filter_new();

    gtk_file_filter_add_pattern(image_filter, "*.png");
    gtk_file_filter_add_pattern(image_filter, "*.jpg");
    gtk_file_filter_add_pattern(image_filter, "*.jpeg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), image_filter);
    gtk_widget_show (dialog);

    g_signal_connect (dialog, "response", G_CALLBACK (on_open_response), NULL);
}

static void send_settings(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkWidget ** entry_arr = data;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (entry_arr[2]));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    const char *tmp_name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (entry_arr[0])));
    const char *tmp_surname = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY (entry_arr[1])));
    const char *tmp_bio = gtk_text_buffer_get_text(buffer, &start, &end, true);;

    if (mx_strcmp(tmp_name, cur_client.name) != 0) {
        clear_message(cur_client.name, 32);
        mx_strcpy(cur_client.name, tmp_name);
        if (mx_strlen(cur_client.name) == 0) {
            tmp_name = ".clear";
            mx_strcpy(cur_client.name, tmp_name);
        }
    }
    else {
        tmp_name = ".not_changed";
    }
    if (mx_strcmp(tmp_surname, cur_client.surname) != 0) {
        clear_message(cur_client.surname, 32);
        mx_strcpy(cur_client.surname, tmp_surname);
        if (mx_strlen(cur_client.surname) == 0) {
            tmp_surname = ".clear";
            mx_strcpy(cur_client.surname, tmp_surname);
        }
    }
    else {
        tmp_surname = ".not_changed";
    }
    if (mx_strcmp(tmp_bio, cur_client.bio) != 0) {
        clear_message(cur_client.bio, 256);
        mx_strcpy(cur_client.bio, tmp_bio);
        if (mx_strlen(cur_client.bio) == 0) {
            tmp_bio = ".clear";
            mx_strcpy(cur_client.bio, tmp_bio);
        }        
    }
    else {
        tmp_bio = ".not_changed";
    }

    char buf[544] = {0};
    sprintf(buf, "<setting, name=%s, surname=%s>%s", tmp_name, tmp_surname, tmp_bio);
    send_all(cur_client.ssl, buf, 544);
    show_settings();
}

void show_settings() 
{
    if(t_main.sticker_panel)
    {
        gtk_widget_hide(t_main.sticker_panel);
    }
    gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.settings);
    t_actives.settings = gtk_image_new_from_file("client/media/setting_active.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.settings), "settings_icon");
    load_css_main(t_screen.provider, t_actives.settings);
    gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.settings);

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
    g_signal_connect(change_photo, "clicked", G_CALLBACK(choise_photo_file), NULL);
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
    if (mx_strcmp(cur_client.name, ".clear") != 0)
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY (entry_field_name)), cur_client.name, mx_strlen(cur_client.name));
    gtk_entry_set_max_length(GTK_ENTRY(entry_field_name), 32);
    gtk_widget_set_name(GTK_WIDGET(entry_field_name), "entry_field_name");
    load_css_main(t_screen.provider, entry_field_name);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_field_name), "First name");
    gtk_widget_set_size_request(entry_field_name, 240, 30);
    gtk_box_append(GTK_BOX(name_entry_box), entry_field_name);
    GtkWidget *entry_field_surname = gtk_entry_new();
    if (mx_strcmp(cur_client.surname, ".clear") != 0)
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY (entry_field_surname)), cur_client.surname, mx_strlen(cur_client.surname));
    gtk_entry_set_max_length(GTK_ENTRY(entry_field_surname), 32);
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
    GtkTextBuffer *bio_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (entry_field));
    if (mx_strcmp(cur_client.bio, ".clear") != 0)
        gtk_text_buffer_set_text(bio_buffer, cur_client.bio, mx_strlen(cur_client.bio));
    g_signal_connect_after(bio_buffer, "insert-text", G_CALLBACK(insert_text_bio), NULL);
    gtk_widget_set_name(GTK_WIDGET(entry_field), "entry_field_bio");
    load_css_main(t_screen.provider, entry_field);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(entry_field), GTK_WRAP_WORD_CHAR);
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

    GtkWidget **entry_arr = (GtkWidget **)malloc(3 * sizeof(GtkWidget *));
    entry_arr[0] = entry_field_name;
    entry_arr[1] = entry_field_surname;
    entry_arr[2] = entry_field;

    g_signal_connect(apply_button, "clicked", G_CALLBACK(send_settings), entry_arr);

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
    GtkWidget *user_image = get_circle_widget_from_png_avatar(&cur_client.avatar, 120, 120, false);
    gtk_box_append(GTK_BOX(user_image_box), user_image);
    GtkWidget *user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_CENTER);
    char *name_surname = NULL;
    char *name = NULL;
    char *surname = NULL;
    if (mx_strcmp(cur_client.name, ".clear") == 0 && mx_strcmp(cur_client.surname, ".clear") == 0) {
        name_surname = mx_strdup(cur_client.login);
    }
    else {
        if (mx_strcmp(cur_client.name, ".clear") != 0) {
            name = cur_client.name;
        }
        if (mx_strcmp(cur_client.surname, ".clear") != 0) {
            surname = cur_client.surname;
        }
        name_surname = mx_strrejoin(name_surname, name);
        name_surname = mx_strrejoin(name_surname, " ");
        name_surname = mx_strrejoin(name_surname, surname);
    }

    GtkWidget *user_name = gtk_label_new(name_surname);
    gtk_widget_set_size_request(user_name, 230, 0);
    mx_strdel(&name_surname);
    gtk_label_set_max_width_chars(GTK_LABEL (user_name), 32);
    gtk_label_set_wrap(GTK_LABEL(user_name), true);
    gtk_label_set_wrap_mode(GTK_LABEL(user_name), PANGO_WRAP_WORD);
    gtk_widget_set_name(user_name, "user_name");
    load_css_main(t_screen.provider, user_name);
    gtk_box_append(GTK_BOX(user_name_box), user_name);
    GtkWidget *user_bio_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(user_bio_box, 230, 0);
    gtk_widget_set_halign(GTK_WIDGET(user_bio_box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(user_bio_box), GTK_ALIGN_CENTER);
    char *bio_str = NULL;
    if (mx_strcmp(cur_client.bio, ".clear") != 0) {
        bio_str = mx_strdup(cur_client.bio);
    }
    GtkWidget *user_bio = gtk_label_new(bio_str);
    gtk_widget_set_size_request(user_bio, 100, 0);
    mx_strdel(&bio_str);
    gtk_label_set_wrap(GTK_LABEL(user_bio), true);
    gtk_label_set_wrap_mode(GTK_LABEL(user_bio), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_max_width_chars(GTK_LABEL (user_bio), 30);
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
