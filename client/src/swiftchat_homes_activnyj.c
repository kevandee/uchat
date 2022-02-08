#include "../inc/uch_client.h"

static void insert_text_note(GtkTextBuffer *buffer, GtkTextIter *location)
{
    gint count=gtk_text_buffer_get_char_count(buffer);

    GtkTextIter start, end, offset;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    //const char *buf_str = gtk_text_buffer_get_text(buffer, &start, &end, true);

    if(count>512) {
        gtk_text_buffer_get_iter_at_offset(buffer, &offset, 256);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_delete(buffer, &offset, &end);
        gtk_text_iter_assign(location, &offset);
    }
}

static void save_note(GtkGestureClick *gesture, int n_press, double x, double y, gpointer buff) {
    (void) gesture;
    (void) n_press;
    (void) x;
    (void) y;

    GtkWidget *entry = buff;
    GtkTextBuffer *gtk_text_buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(gtk_text_buff, &start);
    gtk_text_buffer_get_end_iter(gtk_text_buff, &end);

    const char *text_buff = gtk_text_buffer_get_text(gtk_text_buff, &start, &end, true);

    char *query = NULL;
    char *sql_pattern = "UPDATE user SET note = '%s';";
    asprintf(&query, sql_pattern, text_buff);
    user_exec_db(cur_client.login, query, 2);
}

void show_home() 
{
    if(!t_main.connected) {
        return;
    }
    cur_client.cur_chat.id = -1;
    int point = 1;
    redraw_actives_chats(NULL, &point);
    gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.settings);
    if(cur_client.theme == DARK_THEME)
        t_actives.settings = gtk_image_new_from_file("client/media/settings.png");
    else t_actives.settings = gtk_image_new_from_file("client/media/setting_light.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.settings), "settings_icon");
    load_css_main(t_screen.provider, t_actives.settings);
    GtkGesture *click_settings = gtk_gesture_click_new();
    gtk_gesture_set_state(click_settings, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_swapped(click_settings, "pressed", G_CALLBACK(show_settings), NULL);
    gtk_widget_add_controller(t_actives.settings, GTK_EVENT_CONTROLLER(click_settings));
    gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.settings);
    if(t_main.sticker_panel)
    {
        gtk_widget_hide(t_main.sticker_panel);
    }
    gtk_box_remove(GTK_BOX(t_main.search_panel), t_actives.home);
    t_actives.home = gtk_image_new_from_file("client/media/home_active.png");
    gtk_widget_set_name(GTK_WIDGET(t_actives.home), "home_icon");
    load_css_main(t_screen.provider, t_actives.home);
    //gtk_box_append(GTK_BOX(t_main.search_panel), t_actives.home);
    gtk_box_insert_child_after(GTK_BOX(t_main.search_panel), t_actives.home, t_actives.search_box);

    gtk_grid_remove(GTK_GRID(t_main.grid), t_main.right_panel);
    t_main.right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(t_main.right_panel), GTK_ALIGN_START);
    gtk_widget_set_margin_start(GTK_WIDGET(t_main.right_panel), 50);

    char *hello = mx_strjoin("Hello, ", cur_client.login);

    GtkWidget *hello_user_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(hello_user_box, GTK_ALIGN_START);
    gtk_widget_set_valign(hello_user_box, GTK_ALIGN_CENTER);

    GtkWidget *hello_user = gtk_label_new(hello);
    gtk_widget_set_name(hello_user, "hello_user");
    gtk_box_append(GTK_BOX(hello_user_box), hello_user);
    load_css_main(t_screen.provider, hello_user);

    GtkWidget *home_grid = gtk_grid_new();
    //gtk_widget_set_name(GTK_WIDGET(home_grid, "home_grid");
    gtk_grid_set_row_spacing(GTK_GRID(home_grid), 50);
    gtk_grid_set_column_spacing(GTK_GRID(home_grid), 36);
    
    //NOTES
    GtkWidget *note_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(GTK_WIDGET(note_box), "note_box");
    load_css_main(t_screen.provider, note_box);
    gtk_widget_set_halign(GTK_WIDGET(note_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(note_box), GTK_ALIGN_START);
    gtk_widget_set_size_request(note_box, 0, 0);

    GtkWidget *note_box_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(note_box_header, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(note_box_header, GTK_ALIGN_CENTER);
    gtk_widget_set_name(GTK_WIDGET(note_box_header), "note_box_header");
    load_css_main(t_screen.provider, note_box_header);
    gtk_box_append(GTK_BOX(note_box), note_box_header);
    gtk_widget_set_size_request(note_box_header, 300, 0);

    GtkWidget *note_box_label = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(note_box_label, GTK_ALIGN_START);
    gtk_widget_set_valign(note_box_label, GTK_ALIGN_CENTER);
    gtk_widget_set_name(GTK_WIDGET(note_box_label), "note_box_label");
    GtkWidget *note_label = gtk_label_new("Notes");

    gtk_box_append(GTK_BOX(note_box_label), note_label);
    gtk_box_append(GTK_BOX(note_box_header), note_box_label);

    GtkWidget *note_box_saver = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(note_box_saver), "note_box_saver");
    gtk_widget_set_halign(note_box_saver, GTK_ALIGN_END);
    gtk_widget_set_valign(note_box_saver, GTK_ALIGN_CENTER);
    load_css_main(t_screen.provider, note_box_saver);

    GtkWidget *note_saver = gtk_label_new("Save");
    gtk_widget_set_name(GTK_WIDGET(note_saver), "notes_saver");
    gtk_box_set_spacing(GTK_BOX(note_box_header), 321);         // distance between label and button

    gtk_box_append(GTK_BOX(note_box_saver), note_saver);
    gtk_box_append(GTK_BOX(note_box_header), note_box_saver);

    GtkWidget *note_box_inner = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(note_box_inner), "note_box_inner");
    load_css_main(t_screen.provider, note_box_inner);
    gtk_widget_set_size_request(note_box_inner, 0, 190);

    GtkWidget *note_text_writer = gtk_text_view_new();
    gtk_widget_set_name(GTK_WIDGET(note_text_writer), "note_text_writer");
    load_css_main(t_screen.provider, note_text_writer);
    gtk_widget_set_size_request(note_text_writer, 500, 0);

    GtkGesture *click_save = gtk_gesture_click_new();
    gtk_gesture_set_state(click_save, GTK_EVENT_SEQUENCE_CLAIMED);
    g_signal_connect_after(click_save, "pressed", G_CALLBACK(save_note), note_text_writer);
    gtk_widget_add_controller(note_saver, GTK_EVENT_CONTROLLER(click_save));

    GtkTextBuffer *note_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (note_text_writer));
    //gtk_widget_set_name(GTK_WIDGET(note_text_buffer), "note_text_buffer");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(note_text_writer), GTK_WRAP_WORD_CHAR);
    t_main.note_input = note_text_writer;
    g_signal_connect_after(note_text_buffer, "insert-text", G_CALLBACK(insert_text_note), NULL);
    // notes buffer from data base
    t_list *temp = user_exec_db(cur_client.login, "SELECT note FROM user;", 1);
    if (mx_strcmp((char *)temp->data, ".clear") == 0) {
        temp->data = "";
    }
    gtk_text_buffer_set_text(note_text_buffer, (char *)temp->data, mx_strlen((char *)temp->data));

    GtkWidget *note_scroll = gtk_scrolled_window_new ();
    gtk_widget_set_name(GTK_WIDGET(note_scroll), "note_scroll");
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(note_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (note_scroll), note_text_writer);
    gtk_scrolled_window_set_propagate_natural_height (GTK_SCROLLED_WINDOW(note_scroll),false);
    gtk_scrolled_window_set_propagate_natural_width (GTK_SCROLLED_WINDOW(note_scroll),true);
    load_css_main(t_screen.provider, note_scroll);
    
    gtk_box_append(GTK_BOX(note_box_inner), note_scroll);
    gtk_box_append(GTK_BOX(note_box), note_box_inner);
    gtk_grid_attach(GTK_GRID(home_grid), note_box, 0, 0, 1, 1);
    // NOTES End

    GtkWidget *weather_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(weather_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(weather_box, GTK_ALIGN_END);
    gtk_widget_set_name(GTK_WIDGET(weather_box), "weather_box");
    load_css_main(t_screen.provider, weather_box);
    GtkWidget *weather_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(weather_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(weather_grid), 10);

    char *city = "харків";
    char **weather_label_array = weather_parse(city);

    char *weather_day = weather_label_array[0];
    char *weather_month = weather_label_array[1];
    char *weather_location = weather_label_array[2];
    char *weather_temperature = weather_label_array[3];
    char *weather_image_path = weather_label_array[4];

    GtkWidget *weather_date_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(GTK_WIDGET(weather_date_box), "weather_date_box");
    load_css_main(t_screen.provider, weather_date_box);

    GtkWidget *weather_day_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(weather_day_box), "weather_day_box");
    load_css_main(t_screen.provider, weather_day_box);
    GtkWidget *weather_day_label = gtk_label_new(weather_day);
    gtk_widget_set_name(GTK_WIDGET(weather_day_label), "weather_day_label");
    load_css_main(t_screen.provider, weather_day_label);
    gtk_box_append(GTK_BOX(weather_day_box), weather_day_label);
    gtk_box_append(GTK_BOX(weather_date_box), weather_day_box);

    GtkWidget *weather_month_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(weather_month_box), "weather_month_box");
    load_css_main(t_screen.provider, weather_month_box);
    GtkWidget *weather_month_label = gtk_label_new(weather_month);
    gtk_widget_set_name(GTK_WIDGET(weather_month_label), "weather_month_label");
    load_css_main(t_screen.provider, weather_month_label);
    gtk_box_append(GTK_BOX(weather_month_box), weather_month_label);
    gtk_box_append(GTK_BOX(weather_date_box), weather_month_box);

    GtkWidget *weather_image_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(weather_image_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(weather_image_box, GTK_ALIGN_CENTER);
    gtk_widget_set_name(GTK_WIDGET(weather_image_box), "weather_image_box");
    load_css_main(t_screen.provider, weather_image_box);
    GtkWidget *weather_image = gtk_image_new_from_file(weather_image_path);
    gtk_widget_set_size_request(weather_image, 100, 100);
    gtk_box_append(GTK_BOX(weather_image_box), weather_image);

    GtkWidget *weather_bottom_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(GTK_WIDGET(weather_bottom_box), "weather_bottom_box");
    load_css_main(t_screen.provider, weather_bottom_box);

    GtkWidget *weather_location_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(weather_location_box), "weather_location_box");
    load_css_main(t_screen.provider, weather_location_box);
    GtkWidget *weather_location_label = gtk_label_new(weather_location);
    gtk_widget_set_name(GTK_WIDGET(weather_location_label), "weather_location_label");
    load_css_main(t_screen.provider, weather_location_label);
    gtk_box_append(GTK_BOX(weather_location_box), weather_location_label);
    gtk_box_append(GTK_BOX(weather_bottom_box), weather_location_box);

    GtkWidget *weather_temperature_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(weather_temperature_box), "weather_temperature_box");
    load_css_main(t_screen.provider, weather_temperature_box);
    GtkWidget *weather_temperature_label = gtk_label_new(weather_temperature);
    gtk_widget_set_name(GTK_WIDGET(weather_temperature_label), "weather_temperature_label");
    load_css_main(t_screen.provider, weather_temperature_label);
    gtk_box_append(GTK_BOX(weather_temperature_box), weather_temperature_label);
    gtk_box_append(GTK_BOX(weather_bottom_box), weather_temperature_box);

    GtkWidget *united_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_append(GTK_BOX(united_box), weather_date_box);
    gtk_box_append(GTK_BOX(united_box), weather_image_box);
    gtk_widget_set_margin_top(weather_image_box, 20);
    
    gtk_grid_attach(GTK_GRID(weather_grid), united_box, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(weather_grid), weather_bottom_box, 0, 1, 1, 1);
    gtk_box_append(GTK_BOX(weather_box), weather_grid);
    gtk_widget_set_margin_bottom(weather_box, 4);

    gtk_grid_attach(GTK_GRID(home_grid), weather_box, 1, 0, 1, 1);

    gtk_box_append(GTK_BOX(t_main.right_panel), hello_user_box);
    gtk_box_append(GTK_BOX(t_main.right_panel), home_grid);
    gtk_box_set_spacing(GTK_BOX(t_main.right_panel), 23);
    gtk_widget_set_margin_top(t_main.right_panel, 20);

    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);
}
