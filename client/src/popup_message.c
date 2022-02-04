#include "../inc/uch_client.h"

static void edit ( GSimpleAction *action, G_GNUC_UNUSED GVariant *parameter, G_GNUC_UNUSED gpointer data )
{
    g_print ( "You choose `%s` action %s\n", g_action_get_name ( G_ACTION( action ) ), gtk_label_get_text(data) );
}

const GActionEntry entries[] =
    {
        {"edit", edit, NULL, NULL, NULL, { 0, 0, 0 } },
        {"delete", edit, NULL, NULL, NULL, { 0, 0, 0 } }
    };

void show_message_menu(GtkGestureClick *gesture, int n_press, double x, double y, gpointer data) {
    (void)gesture;
    (void)n_press;
    (void)data;
    (void)x;
    (void)y;
    GActionGroup *act = G_ACTION_GROUP (g_simple_action_group_new());
    g_action_map_add_action_entries(G_ACTION_MAP (act), entries, G_N_ELEMENTS (entries), data);

    gtk_widget_insert_action_group(data, "mes", act);
    GMenu * menu = g_menu_new ();

    GMenuItem *item = g_menu_item_new("edit", "mes.edit");

    g_menu_append_item(menu, item);
    g_object_unref(item);
    item = g_menu_item_new("delete", "mes.delete");
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


