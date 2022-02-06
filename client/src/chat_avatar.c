#include "../inc/uch_client.h"

static void show_circle_range(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data);


static gboolean user_function (GtkEventControllerScroll *controller, double dx, double dy,gpointer user_data) {
    (void)controller;
    (void)dx;

    double scaled_w = cur_client.cur_chat.avatar.orig_w+(cur_client.cur_chat.avatar.d_width + 5*dy);
    double scaled_h = cur_client.cur_chat.avatar.orig_h+(cur_client.cur_chat.avatar.d_width + 5*dy) * (cur_client.cur_chat.avatar.orig_h/cur_client.cur_chat.avatar.orig_w);
    if (cur_client.cur_chat.avatar.x + scaled_w > 500 && cur_client.cur_chat.avatar.y + scaled_h > 500) {
        cur_client.cur_chat.avatar.d_width += 5*dy;
        cur_client.cur_chat.avatar.scaled_w = scaled_w;
        cur_client.cur_chat.avatar.scaled_h = scaled_h;
    }
    gtk_widget_queue_draw(GTK_WIDGET (user_data));
    
    return TRUE;
}

static GtkWidget *circle_range (cairo_surface_t *file) {
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
    
    cur_client.cur_chat.avatar.orig_w = org_width;
    cur_client.cur_chat.avatar.orig_h = org_height;

    cairo_surface_t *scaled_image = scale_to_half(image, org_width, org_height, org_width+ cur_client.cur_chat.avatar.d_width, org_height+cur_client.cur_chat.avatar.d_width * (org_height/org_width));
    org_width = cairo_image_surface_get_width(scaled_image);
    org_height = cairo_image_surface_get_height(scaled_image);
    cur_client.cur_chat.avatar.scaled_w = org_width;
    cur_client.cur_chat.avatar.scaled_h = org_height;

    cairo_set_source_surface (cr, scaled_image, cur_client.cur_chat.avatar.x, cur_client.cur_chat.avatar.y); 
    
    cairo_paint(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);

    cairo_rectangle (cr, cur_client.cur_chat.avatar.x, cur_client.cur_chat.avatar.y, cairo_image_surface_get_width(scaled_image), cairo_image_surface_get_height(scaled_image));
    cairo_set_source_rgba (cr, 0.0, 0, 0, 0.8);
    cairo_fill (cr);

    org_width = cairo_image_surface_get_width(image);
    org_height = cairo_image_surface_get_height(image);

    cairo_set_source_surface (cr, scaled_image, cur_client.cur_chat.avatar.x, cur_client.cur_chat.avatar.y); 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

    cairo_arc(cr, w/2, h/2, 150, 0, 2 * M_PI);
    
    cairo_clip(cr);
    
    cairo_paint(cr);
    cairo_fill(cr);
}

static void begin_move_image(GtkWidget *widget, gpointer data) {
    (void)widget;
    t_list *list = data;

    *((double *)list->next->data) = cur_client.cur_chat.avatar.x; 
    *((double *)list->next->next->data) = cur_client.cur_chat.avatar.y;
}

static void move_image (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer data) {
    (void)gesture;
    t_list *list = data;
    GtkWidget *darea = GTK_WIDGET (list->data);
    double prev_x = *((double *)(list->next->data));
    double prev_y = *((double *)(list->next->next->data));

    if (prev_x + offset_x < 200 && prev_x + offset_x + cur_client.cur_chat.avatar.scaled_w > 500) {
        cur_client.cur_chat.avatar.x=prev_x + offset_x;
    }
    if (prev_y + offset_y < 200 && prev_y + offset_y + cur_client.cur_chat.avatar.scaled_h > 500) {
        cur_client.cur_chat.avatar.y=prev_y + offset_y;
    }
    gtk_widget_queue_draw(darea);   

}
/*
static void send_avatar() {

    char buf[512 + 32] = {0};
    sprintf(buf, "%s", "<setting avatar>");
    send_all(cur_client.cur_chat.ssl, buf, 512 + 32);
    clear_message(buf, 512 + 32);
    
    sprintf(buf, "%s", cur_client.cur_chat.avatar.name);
    send_all(cur_client.cur_chat.ssl, buf, 512 + 32);
    clear_message(buf, 512 + 32);
    t_main.loaded = false;
    send_image(cur_client.cur_chat.ssl, cur_client.cur_chat.avatar.path);
    
    while (!t_main.loaded) {
        usleep(50);
    }

    SSL_write(cur_client.cur_chat.ssl, &cur_client.cur_chat.avatar.scaled_w, sizeof(double));
    SSL_write(cur_client.cur_chat.ssl, &cur_client.cur_chat.avatar.scaled_h, sizeof(double));

    SSL_write(cur_client.cur_chat.ssl, &cur_client.cur_chat.avatar.x, sizeof(double));
    SSL_write(cur_client.cur_chat.ssl, &cur_client.cur_chat.avatar.y, sizeof(double));
    t_avatar *new = (t_avatar *)malloc(sizeof(t_avatar));
    new->image = cur_client.cur_chat.avatar.image;
    new->path = cur_client.cur_chat.avatar.path;
    new->x = cur_client.cur_chat.avatar.x;
    new->y = cur_client.cur_chat.avatar.y;
    new->scaled_h = cur_client.cur_chat.avatar.scaled_h;
    new->scaled_w = cur_client.cur_chat.avatar.scaled_w;
    t_main.loaded = false;
    while (!t_main.loaded) {
        usleep(50);
    }
    gtk_box_remove(GTK_BOX (t_main.search_panel), t_main.logo);

    t_main.logo = get_circle_widget_from_png_avatar(new, 45, 45, false);

    gtk_widget_set_size_request(t_main.logo, 45, 45);
    gtk_widget_set_name(GTK_WIDGET(t_main.logo), "account_avatar");
    load_css_main(t_screen.provider, t_main.logo);
    gtk_box_prepend(GTK_BOX (t_main.search_panel), t_main.logo);
    show_settings();
}
*/
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
    gtk_widget_set_name(GTK_WIDGET(apply_button), "apply_button_avatar");
    load_css_main(t_screen.provider, apply_button);
    gtk_widget_set_size_request(GTK_WIDGET (apply_button), 160, 0);
    gtk_widget_set_halign(GTK_WIDGET(apply_button), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(apply_button), GTK_ALIGN_CENTER);

    //g_signal_connect(apply_button, "clicked", G_CALLBACK(send_avatar), NULL);

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
        
        cur_client.cur_chat.avatar.name = mx_strdup(g_file_get_basename(file));
        cur_client.cur_chat.avatar.path = mx_strdup(g_file_get_path(file));
    
        avatar_range(file);
    }

    gtk_window_destroy (GTK_WINDOW (dialog));
}

void choise_chatphoto_file() {
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

/*

static void show_circle_range(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data);


static gboolean user_function (GtkEventControllerScroll *controller, double dx, double dy,gpointer user_data) {
    (void)controller;
    (void)dx;
    t_list *list = user_data;

    t_avatar *avatar = list->next->next->next->data;
    printf("scroll\n");
    double scaled_w = avatar->orig_w+(avatar->d_width + 5*dy);
    double scaled_h = avatar->orig_h+(avatar->d_width + 5*dy) * (avatar->orig_h/avatar->orig_w);
    if (avatar->x + scaled_w > 500 && avatar->y + scaled_h > 500) {
        avatar->d_width += 5*dy;
        avatar->scaled_w = scaled_w;
        avatar->scaled_h = scaled_h;
    }
    gtk_widget_queue_draw(GTK_WIDGET (list->data));
    
    return TRUE;
}

static GtkWidget *circle_range (gpointer data) {
    GtkWidget *darea = NULL;
    darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 700);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 700);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), show_circle_range, data, NULL);

    return GTK_WIDGET (darea);
}

static void show_circle_range(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data) {
    (void)widget;
    (void)w;
    (void)h;
    (void)data;
    t_list *list = data;
    cairo_surface_t *image = list->data;
    t_avatar *avatar = list->next->data;
    gdouble org_width, org_height;
    org_width = cairo_image_surface_get_width(image);
    org_height = cairo_image_surface_get_height(image);
    
    avatar->orig_w = org_width;
    avatar->orig_h = org_height;

    cairo_surface_t *scaled_image = scale_to_half(image, org_width, org_height, org_width+ avatar->d_width, org_height+avatar->d_width * (org_height/org_width));
    org_width = cairo_image_surface_get_width(scaled_image);
    org_height = cairo_image_surface_get_height(scaled_image);
    avatar->scaled_w = org_width;
    avatar->scaled_h = org_height;

    cairo_set_source_surface (cr, scaled_image, avatar->x, avatar->y); 
    
    cairo_paint(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);

    cairo_rectangle (cr, avatar->x, avatar->y, cairo_image_surface_get_width(scaled_image), cairo_image_surface_get_height(scaled_image));
    cairo_set_source_rgba (cr, 0.0, 0, 0, 0.8);
    cairo_fill (cr);

    org_width = cairo_image_surface_get_width(image);
    org_height = cairo_image_surface_get_height(image);

    cairo_set_source_surface (cr, scaled_image, avatar->x, avatar->y); 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

    cairo_arc(cr, w/2, h/2, 150, 0, 2 * M_PI);
    
    cairo_clip(cr);
    
    cairo_paint(cr);
    cairo_fill(cr);
}

static void begin_move_image(GtkWidget *widget, gpointer data) {
    (void)widget;
    t_list *list = data;
    t_avatar *avatar = list->next->next->next->data;
    *((double *)list->next->data) = avatar->x; 
    *((double *)list->next->next->data) = avatar->y;
}

static void move_image (GtkGestureDrag *gesture, double offset_x, double offset_y, gpointer data) {
    (void)gesture;
    t_list *list = data;
    GtkWidget *darea = GTK_WIDGET (list->data);
    double prev_x = *((double *)(list->next->data));
    double prev_y = *((double *)(list->next->next->data));
    t_avatar *avatar = list->next->next->next->data;
    if (prev_x + offset_x < 200 && prev_x + offset_x + avatar->scaled_w > 500) {
        avatar->x=prev_x + offset_x;
    }
    if (prev_y + offset_y < 200 && prev_y + offset_y + avatar->scaled_h > 500) {
        avatar->y=prev_y + offset_y;
    }
    gtk_widget_queue_draw(darea);   

}

static void send_avatar() {

    char buf[512 + 32] = {0};
    sprintf(buf, "%s", "<setting avatar>");
    send_all(cur_client.cur_chat.ssl, buf, 512 + 32);
    clear_message(buf, 512 + 32);
    
    sprintf(buf, "%s", cur_client.cur_chat.avatar.name);
    send_all(cur_client.cur_chat.ssl, buf, 512 + 32);
    clear_message(buf, 512 + 32);
    t_main.loaded = false;
    send_image(cur_client.cur_chat.ssl, cur_client.cur_chat.avatar.path);
    
    while (!t_main.loaded) {
        usleep(50);
    }

    SSL_write(cur_client.cur_chat.ssl, &cur_client.cur_chat.avatar.scaled_w, sizeof(double));
    SSL_write(cur_client.cur_chat.ssl, &cur_client.cur_chat.avatar.scaled_h, sizeof(double));

    SSL_write(cur_client.cur_chat.ssl, &cur_client.cur_chat.avatar.x, sizeof(double));
    SSL_write(cur_client.cur_chat.ssl, &cur_client.cur_chat.avatar.y, sizeof(double));
    t_avatar *new = (t_avatar *)malloc(sizeof(t_avatar));
    new->image = cur_client.cur_chat.avatar.image;
    new->path = cur_client.cur_chat.avatar.path;
    new->x = cur_client.cur_chat.avatar.x;
    new->y = cur_client.cur_chat.avatar.y;
    new->scaled_h = cur_client.cur_chat.avatar.scaled_h;
    new->scaled_w = cur_client.cur_chat.avatar.scaled_w;
    t_main.loaded = false;
    while (!t_main.loaded) {
        usleep(50);
    }
    gtk_box_remove(GTK_BOX (t_main.search_panel), t_main.logo);

    t_main.logo = get_circle_widget_from_png_avatar(new, 45, 45, false);

    gtk_widget_set_size_request(t_main.logo, 45, 45);
    gtk_widget_set_name(GTK_WIDGET(t_main.logo), "account_avatar");
    load_css_main(t_screen.provider, t_main.logo);
    gtk_box_prepend(GTK_BOX (t_main.search_panel), t_main.logo);
    show_settings();
}

static void avatar_range(GFile *file, t_avatar *avatar) {
    cairo_surface_t *image = get_surface_from_jpg(g_file_get_path(file));
    t_list *list = NULL;
    mx_push_back(&list, image);
    mx_push_back(&list, avatar);
    GtkWidget *darea = circle_range(list);
    gtk_widget_set_size_request(darea , 700, 700);
    
    gtk_grid_remove(GTK_GRID(t_main.grid), t_main.right_panel);
    t_main.right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(t_main.right_panel, 1000,700);
    gtk_widget_set_halign(GTK_WIDGET( t_main.right_panel), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET( t_main.right_panel), GTK_ALIGN_CENTER);

    gtk_widget_set_halign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(darea), GTK_ALIGN_CENTER);
    
    GtkWidget *apply_button = gtk_button_new_with_label("Apply");
    gtk_widget_set_name(GTK_WIDGET(apply_button), "apply_button_avatar");
    load_css_main(t_screen.provider, apply_button);
    gtk_widget_set_size_request(GTK_WIDGET (apply_button), 160, 0);
    gtk_widget_set_halign(GTK_WIDGET(apply_button), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(apply_button), GTK_ALIGN_CENTER);

    //g_signal_connect(apply_button, "clicked", G_CALLBACK(send_avatar), NULL);

    gtk_box_append(GTK_BOX (t_main.right_panel), darea);
    gtk_box_append(GTK_BOX (t_main.right_panel), apply_button);
    
    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);

    GtkEventController *scroll_controller = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    t_list *data = NULL;
    double *prev_x = (double *)malloc(sizeof(double)), *prev_y = (double *)malloc(sizeof(double));
    mx_push_back(&data, darea);
    mx_push_back(&data, prev_x);
    mx_push_back(&data, prev_y);
    mx_push_back(&data, avatar);
    g_signal_connect(scroll_controller, "scroll", G_CALLBACK(user_function), data);
    gtk_widget_add_controller(t_main.right_panel, GTK_EVENT_CONTROLLER (scroll_controller));

    GtkGesture *drag_controller = gtk_gesture_drag_new();
   
    
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
        t_avatar avatar;
        avatar.name = mx_strdup(g_file_get_basename(file));
        avatar.path = mx_strdup(g_file_get_path(file));
    
        avatar_range(file, &avatar);
    }

    gtk_window_destroy (GTK_WINDOW (dialog));
}

void choise_chatphoto_file() {
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


*/