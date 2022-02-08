#include "../inc/uch_client.h"

static void draw_circle_avatar(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data) {
    (void)widget;
    (void)w;
    (void)h;
    // lutiy kostil'
    if (h != w) {
        h = w;
    }

    t_avatar *avatar = data;
    cairo_surface_t *image = avatar->image;
    double width = cairo_image_surface_get_width(image);
    double height = cairo_image_surface_get_height(image);

    cairo_set_source_surface (cr, image, (avatar->x - 200)* width/avatar->scaled_w , (avatar->y - 200)* height/avatar->scaled_h); 

    cairo_arc(cr, w/2, h/2, w/2, 0, 2 * M_PI);
    cairo_clip(cr);
    cairo_paint(cr);
}

static void draw_circle(GtkDrawingArea *widget, cairo_t *cr, int w, int h, gpointer data) {
    (void)widget;
    (void)w;
    (void)h;

    cairo_surface_t *image = (cairo_surface_t *)data;
    cairo_set_source_surface (cr, image, 0, 0); 
    
    cairo_arc(cr, w/2, h/2, w/2, 0, 2 * M_PI);
    cairo_clip(cr);
    cairo_paint(cr);
}

GtkWidget *get_circle_widget_from_png(const char *filename) {
    GtkWidget *darea = NULL;
    gint width, height;
    
    cairo_surface_t *image = cairo_image_surface_create_from_png(filename);
    width = cairo_image_surface_get_width(image);
    height = cairo_image_surface_get_height(image);
    
    cairo_surface_t *scaled_image = scale_to_half(image, width, height, 60, 60);
    width = cairo_image_surface_get_width(scaled_image);
    height = cairo_image_surface_get_height(scaled_image);

    darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 60);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 60);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle, scaled_image, NULL);

    return GTK_WIDGET (darea);
}

GtkWidget *get_circle_widget_from_png_avatar(t_avatar *avatar, gint width, gint height, bool must_update){
    GtkWidget *darea = NULL;
    gint org_width, org_height;

    if (must_update || !avatar->image || (avatar->prev_w != width && avatar->prev_h != height)) {
        cairo_surface_t *image = get_surface_from_jpg(avatar->path);
        org_width = cairo_image_surface_get_width(image);
        org_height = cairo_image_surface_get_height(image);

        cairo_surface_t *scaled_image = scale_to_half(image, org_width, org_height, avatar->scaled_w * width/300, avatar->scaled_h * height/300);
        avatar->image=scaled_image;
        avatar->prev_w = width;
        avatar->prev_h = height;
    }

    darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), width);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), height);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle_avatar, avatar, NULL);

    return GTK_WIDGET (darea);

}

GtkWidget *get_circle_widget_current_user_avatar(){
    GtkWidget * darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), 45);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), 45);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle_avatar, &cur_client.avatar, NULL);

    return GTK_WIDGET (darea);

}

GtkWidget *get_circle_widget_from_png_custom(const char *filename, gint width, gint height){
    GtkWidget *darea = NULL;
    gint org_width, org_height;

    cairo_surface_t *image = cairo_image_surface_create_from_png(filename);
    org_width = cairo_image_surface_get_width(image);
    org_height = cairo_image_surface_get_height(image);
    
    cairo_surface_t *scaled_image = scale_to_half(image, org_width, org_height, width, height);
    org_width = cairo_image_surface_get_width(scaled_image);
    org_height = cairo_image_surface_get_height(scaled_image);

    darea = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA (darea), width);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA (darea), height);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (darea), draw_circle, scaled_image, NULL);

    return GTK_WIDGET (darea);

}

cairo_surface_t *get_surface_from_jpg(const char *filename) {
        GdkPixbuf *pixbuf;
        gint width;
        gint height;
        cairo_format_t format;
        cairo_surface_t *surface;
        cairo_t *cr;
        pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
        g_assert (pixbuf != NULL);
        format = (gdk_pixbuf_get_has_alpha (pixbuf)) ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;
        width = gdk_pixbuf_get_width (pixbuf);
        height = gdk_pixbuf_get_height (pixbuf);
        surface = cairo_image_surface_create (format, width, height);
        g_assert (surface != NULL);
        cr = cairo_create (surface);
        /* Draw the pixbuf */
        gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
        cairo_paint (cr);
       
        cairo_destroy (cr);
        return surface;
}

cairo_surface_t *scale_to_half(cairo_surface_t *s, int orig_width, int orig_height, int scaled_width, int scaled_height) { 
    if (orig_height == scaled_height && orig_width == scaled_width)
        return s;
    double param1 = (double)(orig_width)/((double)scaled_width);

    double param2 = (double)orig_height/((double)scaled_height);

    cairo_surface_t *result = cairo_surface_create_similar(s, cairo_surface_get_content(s), orig_width * (1/param1), orig_height*(1/param2)); 
    cairo_t *cr = cairo_create(result); 
    cairo_scale(cr, 1/param1, 1/param2); 
    cairo_set_source_surface(cr, s, 0, 0); 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE); 
    cairo_paint(cr); 
    cairo_destroy(cr); 
    return result; 
}
