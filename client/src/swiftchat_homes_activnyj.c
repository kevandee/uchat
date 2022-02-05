#include "../inc/uch_client.h"

static void insert_text_note(GtkTextBuffer *buffer, GtkTextIter *location)
{
    gint count=gtk_text_buffer_get_char_count(buffer);

    GtkTextIter start, end, offset;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    const char *buf_str = gtk_text_buffer_get_text(buffer, &start, &end, true);

    if(count>512) {
        gtk_text_buffer_get_iter_at_offset(buffer, &offset, 256);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_delete(buffer, &offset, &end);
        gtk_text_iter_assign(location, &offset);
    }

    /////// VVV delete this VVV
    printf("\nnote : %s\n", buf_str);
    
}


void show_home() {
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

    GtkWidget *note_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(GTK_WIDGET(note_box), "note_box");
    gtk_widget_set_halign(GTK_WIDGET(note_box), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(note_box), GTK_ALIGN_START);
    gtk_box_set_spacing(GTK_BOX(note_box), 10);

    GtkWidget *notes_label = gtk_label_new("Your notes:");
    gtk_box_append(GTK_BOX(note_box), notes_label);

    GtkWidget *note_box_inner = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(GTK_WIDGET(note_box_inner), "note_box_inner");
    load_css_main(t_screen.provider, note_box_inner);
    /*gtk_widget_set_halign(GTK_WIDGET(note_box_inner), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(note_box_inner), GTK_ALIGN_START);
    gtk_box_set_spacing(GTK_BOX(note_box_inner), 10);*/
    gtk_widget_set_size_request(note_box_inner, 0, 300);

    GtkWidget *note_text_writer = gtk_text_view_new();
    gtk_widget_set_name(GTK_WIDGET(note_text_writer), "write_note");
    load_css_main(t_screen.provider, note_text_writer);
    gtk_widget_set_size_request(note_text_writer, 300, 0);
    GtkTextBuffer *note_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (note_text_writer));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(note_text_writer), GTK_WRAP_WORD_CHAR);
    t_main.note_input = note_text_writer;
    g_signal_connect_after(note_text_buffer, "insert-text", G_CALLBACK(insert_text_note), NULL);
    //gtk_text_buffer_set_text();

    GtkWidget *note_scroll = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(note_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (note_scroll), note_text_writer);
    gtk_scrolled_window_set_propagate_natural_height (GTK_SCROLLED_WINDOW(note_scroll),false);
    gtk_scrolled_window_set_propagate_natural_width (GTK_SCROLLED_WINDOW(note_scroll),true);
    load_css_main(t_screen.provider, note_scroll);
    
    gtk_box_append(GTK_BOX(note_box_inner), note_scroll);
    gtk_box_append(GTK_BOX(note_box), note_box_inner);
    gtk_box_append(GTK_BOX(t_main.right_panel), note_box);


    gtk_grid_attach(GTK_GRID(t_main.grid), t_main.right_panel, 1, 0, 1, 2);
}
