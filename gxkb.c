/*
 * Copyright (C) 2013 Dmitriy Poltavchenko <zen@root.ua>
 */
#include <stdlib.h>
#include <gtk/gtk.h>
#include <ctype.h>

#include <libxklavier/xklavier.h>
#include <libwnck/libwnck.h>
#include <cairo/cairo.h>
#include "common.h"
#include "xkb-config.h"
#include "xkb-util.h"
#include "xkb-callbacks.h"


#include <stdio.h>

/* ----------------------------------------------------------------- *
 *                           XKB Stuff                               *
 * ----------------------------------------------------------------- */

t_xkb *      xkb_new                             (GtkStatusIcon *plugin);

void         xkb_free                            (t_xkb *xkb);

gboolean     xkb_load_config                     (t_xkb *xkb, const gchar *filename);

void         xkb_save_config                     (t_xkb *xkb, const gchar *filename);

void         xkb_load_default                    (t_xkb *xkb);

void         xkb_initialize_menu                 (t_xkb *xkb);

void         xkb_refresh_gui                     (t_xkb *xkb);

GdkPixbuf *  draw_group_icon                     (gchar *text);

/* ================================================================== *
 *                        Implementation                              *
 * ================================================================== */

void
xkb_state_changed (gint current_group, gboolean config_changed,
                   gpointer user_data)
{
    t_xkb *xkb = (t_xkb*) user_data;
    xkb_refresh_gui (xkb);

    if (config_changed)
    {
        xkb_initialize_menu (xkb);
    }

}

void
xkb_set_group (GtkStatusIcon *item,
                      gpointer data)
{
    gint group = GPOINTER_TO_INT (data);
    xkb_config_set_group (group);
}

t_xkb *
xkb_new (GtkStatusIcon *tray)
{
    t_xkb *xkb;
    WnckScreen *wnck_screen;

    xkb = g_slice_new0 (t_xkb);
    xkb->settings = NULL;
    xkb->settings = g_new0 (t_xkb_settings, 1);
    xkb->tray = tray;

    g_signal_connect (G_OBJECT(xkb->tray), "activate", G_CALLBACK (xkb_tray_icon_clicked), xkb);
    g_signal_connect (G_OBJECT(xkb->tray), "scroll-event", G_CALLBACK (xkb_tray_icon_scrolled), xkb);
    g_signal_connect(G_OBJECT(xkb->tray), "popup-menu", G_CALLBACK(xkb_tray_icon_popup_menu), xkb);

    //g_object_set (G_OBJECT (xkb->tray), "has-tooltip", FALSE, NULL);

    wnck_screen = wnck_screen_get_default ();
    g_signal_connect (G_OBJECT (wnck_screen), "active-window-changed",
                      G_CALLBACK (xkb_active_window_changed), xkb);
    g_signal_connect (G_OBJECT (wnck_screen), "window-closed",
                      G_CALLBACK (xkb_window_closed), xkb);
    g_signal_connect (G_OBJECT (wnck_screen), "application-closed",
                      G_CALLBACK (xkb_application_closed), xkb);

    return xkb;
}

void
xkb_free (t_xkb *xkb)
{
    xkb_config_finalize ();

    if (xkb->settings->kbd_config)
        kbd_config_free (xkb->settings->kbd_config);

    g_free (xkb->settings);

    gtk_widget_destroy (xkb->popup);
}

void
xkb_save_config (t_xkb *xkb, const gchar *config_file)
{
    gchar *str_data;
    gsize len;
    GKeyFile *cfg_file = g_key_file_new();
    g_key_file_set_list_separator (cfg_file, ',');

    g_key_file_set_integer(cfg_file, "xkb config", "display_type", xkb->display_type);
    g_key_file_set_integer(cfg_file, "xkb config", "group_policy", xkb->settings->group_policy);
    g_key_file_set_integer(cfg_file, "xkb config", "default_group", xkb->settings->default_group);
    g_key_file_set_boolean(cfg_file, "xkb config", "never_modify_config", xkb->settings->never_modify_config);

    if (xkb->settings->kbd_config != NULL)
    {
        g_key_file_set_string(cfg_file, "xkb config", "model", xkb->settings->kbd_config->model);
        g_key_file_set_string(cfg_file, "xkb config", "layouts", xkb->settings->kbd_config->layouts);
        g_key_file_set_string(cfg_file, "xkb config", "variants", xkb->settings->kbd_config->variants);
        if (xkb->settings->kbd_config->toggle_option == NULL)
            g_key_file_set_string(cfg_file, "xkb config", "toggle_option", "");
        else g_key_file_set_string(cfg_file, "xkb config", "toggle_option", xkb->settings->kbd_config->toggle_option);

        if (xkb->settings->kbd_config->compose_key_position == NULL)
            g_key_file_set_string(cfg_file, "xkb config", "compose_key_position", "");
        else g_key_file_set_string(cfg_file, "xkb config", "compose_key_position", xkb->settings->kbd_config->compose_key_position);
    }

    str_data = g_key_file_to_data (cfg_file, &len, NULL);
    g_file_set_contents (config_file, str_data, len, NULL);
    g_free (str_data);

    g_key_file_free(cfg_file);
}

gboolean
xkb_load_config (t_xkb *xkb, const gchar *filename)
{
    GError *error;
    GKeyFile *cfg_file = g_key_file_new();
    if (!g_key_file_load_from_file(cfg_file, filename, G_KEY_FILE_KEEP_COMMENTS, &error))
    {
        g_key_file_free(cfg_file);
        return FALSE;
    }

    xkb->display_type = g_key_file_get_integer(cfg_file, "xkb config", "display_type", &error);
    error = NULL;
    xkb->settings->group_policy = g_key_file_get_integer(cfg_file, "xkb config", "group_policy",  &error);
    error = NULL;
    if (xkb->settings->group_policy != GROUP_POLICY_GLOBAL) {
        xkb->settings->default_group = g_key_file_get_integer(cfg_file, "xkb config", "default_group",  &error);
        error = NULL;
    }

    xkb->settings->never_modify_config = g_key_file_get_boolean(cfg_file, "xkb config", "never_modify_config", &error);
    error = NULL;

    if (xkb->settings->kbd_config == NULL)
    {
        xkb->settings->kbd_config = g_new0 (t_xkb_kbd_config, 1);
    }

    xkb->settings->kbd_config->model = g_key_file_get_string(cfg_file, "xkb config", "model", &error);
    error = NULL;
    xkb->settings->kbd_config->layouts = g_key_file_get_string(cfg_file, "xkb config", "layouts", &error);
    error = NULL;
    xkb->settings->kbd_config->variants = g_key_file_get_string(cfg_file, "xkb config", "variants", &error);
    error = NULL;
    xkb->settings->kbd_config->toggle_option = g_key_file_get_string(cfg_file, "xkb config", "toggle_option", &error);
    error = NULL;
    xkb->settings->kbd_config->compose_key_position = g_key_file_get_string(cfg_file, "xkb config", "compose_key_position", &error);
    error = NULL;

    g_key_file_free(cfg_file);

    return TRUE;
}

void
xkb_load_default (t_xkb *xkb)
{
    if (xkb->settings->kbd_config == NULL)
    {
        xkb->settings->kbd_config = g_new0 (t_xkb_kbd_config, 1);
    }
    xkb->display_type = DISPLAY_TYPE_TEXT;
    xkb->settings->group_policy = GROUP_POLICY_PER_APPLICATION;
    xkb->settings->never_modify_config = FALSE;
    xkb->settings->kbd_config->model = g_strdup("pc105");
    xkb->settings->kbd_config->layouts = g_strdup("us,ru");
    xkb->settings->kbd_config->variants = g_strdup(",");
    xkb->settings->kbd_config->toggle_option = g_strdup("grp:alt_shift_toggle,grp_led:scroll,terminate:ctrl_alt_bksp");
    xkb->settings->kbd_config->compose_key_position = g_strdup("");
}

void
xkb_initialize_menu (t_xkb *xkb)
{
    ///FIXME: Add menu pixmap

    gint i;
    GdkPixbuf *handle = NULL;
    GdkPixbuf *pixbuf;
    //gchar *imgfilename;
    GtkWidget *image;
    GtkWidget *menu_item;

    if (G_UNLIKELY (xkb == NULL)) return;

    if (xkb->popup)
        gtk_widget_destroy (xkb->popup);

    xkb->popup = gtk_menu_new ();

    gint width, height;
    gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, &height);

    for (i = 0; i < xkb_config_get_group_count (); i++)
    {
        gchar *layout_string;

        //imgfilename = xkb_util_get_flag_filename (xkb_config_get_group_name (i));
        //handle = rsvg_handle_new_from_file (imgfilename, NULL);
        //g_free (imgfilename);

        layout_string =
            xkb_util_get_layout_string (xkb_config_get_group_name (i),
                                        xkb_config_get_variant (i));
        layout_string =  g_ascii_strup (layout_string, -1);

        handle = draw_group_icon(layout_string);

        menu_item = gtk_image_menu_item_new_with_label (layout_string);
        g_free (layout_string);

        g_signal_connect (G_OBJECT (menu_item), "activate",
                          G_CALLBACK (xkb_set_group), GINT_TO_POINTER (i));

        if (handle)
        {
            image = gtk_image_new ();

            pixbuf = gdk_pixbuf_scale_simple (handle, width, height, GDK_INTERP_BILINEAR);
            gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbuf);
            gtk_widget_show (image);
            gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_item), image);

            g_object_unref (G_OBJECT (pixbuf));
            g_object_unref (handle);
        }

        gtk_widget_show (menu_item);

        gtk_menu_shell_append (GTK_MENU_SHELL (xkb->popup), menu_item);
    }

    GtkWidget *sep = gtk_separator_menu_item_new();
    gtk_menu_shell_append (GTK_MENU_SHELL (xkb->popup), sep);
    gtk_widget_show (sep);
    menu_item = gtk_image_menu_item_new_with_label ("Quit");

    g_signal_connect (G_OBJECT (menu_item), "activate",
                      G_CALLBACK (gtk_main_quit), NULL);

    image = gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_MENU);

    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_item), image);
    gtk_widget_show (menu_item);

    gtk_menu_shell_append (GTK_MENU_SHELL (xkb->popup), menu_item);
}

void
xkb_refresh_gui (t_xkb *xkb)
{
    gchar *text =  g_ascii_strup(xkb_util_get_layout_string (xkb_config_get_group_name (-1),
                                 xkb_config_get_variant (-1)), -1);

    GdkPixbuf * pixmap = draw_group_icon(text);
    gtk_status_icon_set_from_pixbuf(xkb->tray, pixmap);

    g_free(text);
}

GdkPixbuf *
draw_group_icon(gchar *text)
{

    cairo_t *cr;

    int width = 26;
    int height = 26;

    GdkPixmap *pixmap = gdk_pixmap_new(NULL, width, height, 24);
    GError *error;
    cr = gdk_cairo_create(pixmap);

    cairo_rectangle(cr, 0.0, 0.0, width, height);
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
    cairo_rectangle(cr, 0.0, 3.0, width, height-6);
    cairo_set_source_rgb(cr, 0.5, 0.4, 0.7);
    cairo_fill(cr);
    //cairo_paint(cr);

    cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, 20.0);
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    cairo_move_to (cr, 1, 20);
    cairo_show_text (cr, text);
    cairo_destroy(cr);

    GdkPixbuf *pixbuf_new = gdk_pixbuf_get_from_drawable(NULL, pixmap, NULL,
                            0, 0, 0, 0, width, height);

    g_object_unref(pixmap);

    return pixbuf_new;
}

int main (int argc, char *argv[])
{
    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);
    g_set_application_name ("gxkb");

    GtkStatusIcon *tray_icon;
    tray_icon = gtk_status_icon_new();

    t_xkb *xkb = xkb_new (tray_icon);
xkb_load_default(xkb);
    char *config_path = (g_getenv("XDG_CONFIG_HOME") == NULL
                         ? g_build_filename(g_get_home_dir(), ".config", NULL)
                         : g_strdup(g_getenv("XDG_CONFIG_HOME")));

    char *config_file = g_strconcat(g_build_filename(config_path,
                                    g_get_application_name(), NULL), ".cfg");

    if (!g_file_test(config_path, G_FILE_TEST_EXISTS))
    {
        g_mkdir_with_parents(config_path, 0700);
        xkb_load_default(xkb);
    }
    else
    {
        if(!xkb_load_config(xkb, config_file)) {
            xkb_load_default(xkb);
        }
    }

    if (xkb_config_initialize (xkb->settings, xkb_state_changed, xkb))
    {
        xkb_config_update_settings(xkb->settings);
        xkb_refresh_gui (xkb);
        xkb_initialize_menu (xkb);
    }

    g_free(config_path);

    gtk_status_icon_set_visible(tray_icon, TRUE);

    /* Enter the main loop */
    gtk_main ();
    if (!xkb->settings->never_modify_config)
        xkb_save_config(xkb, config_file);

    g_free(config_file);
    xkb_free(xkb);
    return 0;
}

