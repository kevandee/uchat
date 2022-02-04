#include "../inc/uch_client.h"

static void quit ( GSimpleAction *action, G_GNUC_UNUSED GVariant *parameter, G_GNUC_UNUSED gpointer data )
{
    g_print ( "You choose `%s` action\n", g_action_get_name ( G_ACTION( action ) ) );
}

const GActionEntry entries[] =
    {
        {"close", quit, NULL, NULL, NULL, { 0, 0, 0 } }
    };

void show_message_menu(GtkGestureClick *gesture, int n_press, double x, double y, gpointer data) {
    (void)gesture;
    (void)n_press;
    (void)data;
    (void)x;
    (void)y;
    printf("bebra\n");
     g_action_map_add_action_entries ( G_ACTION_MAP ( gtk_window_get_application(GTK_WINDOW (t_screen.main_window)) ), entries, G_N_ELEMENTS ( entries ), NULL );
    GMenu * menu = g_menu_new ();
    //char tmp[700];
    //sprintf (tmp, "app.delete-cc-binding::%d", 1);
    GMenuItem *item = g_menu_item_new("edit", "app.close");
    //g_menu_append (menu, "_Delete", tmp);
    g_menu_append_item(menu, item);
    g_object_unref(item);
    item = g_menu_item_new("delete", "app.open");
    g_menu_append_item(menu, item);
    g_object_unref(item);

    GtkPopoverMenu * popover_menu = GTK_POPOVER_MENU (gtk_popover_menu_new_from_model (G_MENU_MODEL (menu)));
    gtk_popover_set_has_arrow (GTK_POPOVER (popover_menu), false);
    gtk_popover_set_position (GTK_POPOVER (popover_menu), GTK_POS_BOTTOM);
    gtk_widget_set_parent (GTK_WIDGET (popover_menu), GTK_WIDGET (data));

    if (x != -1 && y != -1)
    {
      GdkRectangle rect = { x, y + 5, 1, 1 };
      gtk_popover_set_pointing_to (GTK_POPOVER (popover_menu), &rect);
    }
  else
    gtk_popover_set_pointing_to (GTK_POPOVER (popover_menu), NULL);
    
    gtk_popover_popup (GTK_POPOVER (popover_menu));
    gtk_popover_present (GTK_POPOVER (popover_menu));
}


