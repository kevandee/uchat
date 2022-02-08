#include "../inc/uch_client.h"

static void edit ( GSimpleAction *action, G_GNUC_UNUSED GVariant *parameter, G_GNUC_UNUSED gpointer data )
{   
    (void)action;
    t_list *gesture_list = data;
    GtkWidget **arr = gesture_list->data;

    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW (t_main.message_input_view));
    const char *text = gtk_label_get_text(GTK_LABEL (arr[0]));
    gtk_text_buffer_set_text(buf, text, mx_strlen(text));
    t_main.message_change_id = *((int *)gesture_list->next->data);

}

static void delete ( GSimpleAction *action, G_GNUC_UNUSED GVariant *parameter, G_GNUC_UNUSED gpointer data )
{
    (void)action;
    t_list *gesture_list = data;
    GtkWidget **arr = gesture_list->data;
    GtkWidget * pop = gesture_list->next->next->data;

    // <delete message>
    gtk_popover_popdown(GTK_POPOVER (pop));
    if (mx_strncmp(cur_client.name, ".dialog", 7) == 0)
        gtk_widget_hide(arr[1]);
    else 
        gtk_widget_hide(gtk_widget_get_parent (arr[1]));
    char buf[544] = {0};
    sprintf(buf, "<delete mes chat_id=%d, mes_id=%d>", cur_client.cur_chat.id, *((int *)gesture_list->next->data));
    swiftchat_send(cur_client.ssl, buf, 544);
}

const GActionEntry entries[] =
    {
        {"edit", edit, NULL, NULL, NULL, {0} },
        {"delete", delete, NULL, NULL, NULL, {0} }
    };

void show_message_menu(GtkGestureClick *gesture, int n_press, double x, double y, gpointer data) {
    (void)gesture;
    (void)n_press;
    (void)data;
    (void)x;
    (void)y;
    if (!t_main.connected) {
        return;
    }
    t_list *gesture_list = data;
    GtkWidget **arr = gesture_list->data;
    GActionGroup *act = G_ACTION_GROUP (g_simple_action_group_new());
    g_action_map_add_action_entries(G_ACTION_MAP (act), entries, G_N_ELEMENTS (entries), data);

    gtk_widget_insert_action_group(t_main.scrolled_window_right, "mes", act);
    g_object_unref(act);
    GMenu * menu = g_menu_new ();

    GMenuItem *item = g_menu_item_new("edit", "mes.edit");

    g_menu_append_item(menu, item);
    g_object_unref(item);
    item = g_menu_item_new("delete", "mes.delete");
    g_menu_append_item(menu, item);
    g_object_unref(item);

    GtkPopoverMenu * popover_menu = GTK_POPOVER_MENU (gtk_popover_menu_new_from_model (G_MENU_MODEL (menu)));
    g_object_unref(menu);
    gtk_popover_set_has_arrow (GTK_POPOVER (popover_menu), false);
    gtk_popover_set_position (GTK_POPOVER (popover_menu), GTK_POS_BOTTOM);
    gtk_widget_set_parent (GTK_WIDGET (popover_menu), GTK_WIDGET (arr[1]));

    if (x != -1 && y != -1)
    {
      GdkRectangle rect = { x, y + 5, 1, 1 };
      gtk_popover_set_pointing_to (GTK_POPOVER (popover_menu), &rect);
    }
  else
    gtk_popover_set_pointing_to (GTK_POPOVER (popover_menu), NULL);

    gtk_popover_popup (GTK_POPOVER (popover_menu));
    gtk_popover_present (GTK_POPOVER (popover_menu));
    mx_push_back(&gesture_list, popover_menu);
}


