/*
 * Copyright (C) 2013 Dmitriy Poltavchenko <zen@root.ua>
 */
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <getopt.h>
#include <glib/gstdio.h>

#include "common.h"
#include "xkb-config.h"
#include "xkb-util.h"
#include "xkb-callbacks.h"

/* ----------------------------------------------------------------- *
 *                           XKB Stuff                               *
 * ----------------------------------------------------------------- */

void         xkb_state_changed                   (gint current_group,
        gboolean config_changed,
        gpointer user_data);

void         xkb_set_group                       (GtkStatusIcon *item,
        gpointer data);

t_xkb *      xkb_new                             ();

void         xkb_free                            (t_xkb *xkb);

void         xkb_save_config                     (t_xkb *xkb, const gchar *filename);

gboolean     xkb_load_config                     (t_xkb *xkb, const gchar *filename);

void         xkb_load_default                    (t_xkb *xkb);

void         xkb_initialize_menu                 (t_xkb *xkb);

void         xkb_refresh                         (t_xkb *xkb);

void         xkb_about                           (t_xkb *xkb);
/* ================================================================== *
 *                        Implementation                              *
 * ================================================================== */

void
xkb_state_changed (gint current_group, gboolean config_changed,
                   gpointer user_data)
{
    t_xkb *xkb = (t_xkb*) user_data;
    xkb_refresh(xkb);

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
xkb_new ()
{
    t_xkb *xkb;
    WnckScreen *wnck_screen;

    xkb = g_slice_new0 (t_xkb);
    xkb->settings = NULL;
    xkb->settings = g_new0 (t_xkb_settings, 1);
    xkb->tray = gtk_status_icon_new();

    g_signal_connect(G_OBJECT(xkb->tray), "activate", G_CALLBACK (xkb_tray_icon_clicked), xkb);
    g_signal_connect(G_OBJECT(xkb->tray), "scroll-event", G_CALLBACK (xkb_tray_icon_scrolled), xkb);
    g_signal_connect(G_OBJECT(xkb->tray), "popup-menu", G_CALLBACK(xkb_tray_icon_popup_menu), xkb);

    wnck_screen = wnck_screen_get_default();
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

    gtk_widget_destroy (xkb->rb_mouse_popup);
    gtk_widget_destroy (xkb->lb_mouse_popup);

    g_object_unref (xkb->tray);
}

void
xkb_save_config (t_xkb *xkb, const gchar *config_file)
{
    gchar *str_data;
    gsize len;
    GKeyFile *cfg_file = g_key_file_new();
    g_key_file_set_list_separator (cfg_file, ',');

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
    GKeyFile *cfg_file = g_key_file_new();
    if (!g_key_file_load_from_file(cfg_file, filename, G_KEY_FILE_KEEP_COMMENTS, NULL))
    {
        g_key_file_free(cfg_file);
        return FALSE;
    }

    xkb->settings->group_policy = g_key_file_get_integer(cfg_file, "xkb config", "group_policy",  NULL);
    if (xkb->settings->group_policy != GROUP_POLICY_GLOBAL)
    {
        xkb->settings->default_group = g_key_file_get_integer(cfg_file, "xkb config", "default_group",  NULL);
    }

    xkb->settings->never_modify_config = g_key_file_get_boolean(cfg_file, "xkb config", "never_modify_config", NULL);

    if (xkb->settings->kbd_config == NULL)
    {
        xkb->settings->kbd_config = g_new0 (t_xkb_kbd_config, 1);
    }

    xkb->settings->kbd_config->model = g_key_file_get_string(cfg_file, "xkb config", "model", NULL);
    xkb->settings->kbd_config->layouts = g_key_file_get_string(cfg_file, "xkb config", "layouts", NULL);
    xkb->settings->kbd_config->variants = g_key_file_get_string(cfg_file, "xkb config", "variants", NULL);
    xkb->settings->kbd_config->toggle_option = g_key_file_get_string(cfg_file, "xkb config", "toggle_option", NULL);
    xkb->settings->kbd_config->compose_key_position = g_key_file_get_string(cfg_file, "xkb config", "compose_key_position", NULL);

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
    if (G_UNLIKELY (xkb == NULL)) return;

    // Right button click menu
    GtkWidget *mi;

    if (xkb->rb_mouse_popup)
        gtk_widget_destroy (xkb->rb_mouse_popup);

    xkb->rb_mouse_popup = gtk_menu_new();

    mi = gtk_image_menu_item_new_from_stock(GTK_STOCK_DIALOG_INFO, NULL);
    g_signal_connect(G_OBJECT(mi), "activate", (GCallback)xkb_about, NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL (xkb->rb_mouse_popup), mi);
    gtk_widget_show (mi);

    mi = gtk_menu_item_new ();
    gtk_widget_show (mi);
    gtk_menu_shell_append (GTK_MENU_SHELL (xkb->rb_mouse_popup), mi);
    gtk_widget_set_sensitive (mi, FALSE);

    mi = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    g_signal_connect(G_OBJECT(mi), "activate", (GCallback)gtk_main_quit, NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL (xkb->rb_mouse_popup), mi);
    gtk_widget_show (mi);

    // Left button click menu
    gint i;
    GdkPixbuf *handle = NULL;
    gchar *imgfilename;
    GtkWidget *image;
    GtkWidget *menu_item;

    if (xkb->lb_mouse_popup)
        gtk_widget_destroy (xkb->lb_mouse_popup);

    xkb->lb_mouse_popup = gtk_menu_new ();

    gint width, height;
    gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, &height);

    for (i = 0; i < xkb_config_get_group_count (); i++)
    {
        gchar *layout_string;

        imgfilename = xkb_util_get_flag_filename (xkb_config_get_group_name (i));
        handle = gdk_pixbuf_new_from_file_at_scale(imgfilename, width, height, TRUE, NULL);
        g_free (imgfilename);

        layout_string =
            xkb_util_get_layout_string (xkb_config_get_group_name (i),
                                        xkb_config_get_variant (i));

        layout_string =  g_ascii_strup (layout_string, -1);

        menu_item = gtk_image_menu_item_new_with_label (layout_string);
        g_free (layout_string);

        g_signal_connect (G_OBJECT (menu_item), "activate",
                          G_CALLBACK (xkb_set_group), GINT_TO_POINTER (i));

        if (handle)
        {
            image = gtk_image_new ();

            gtk_image_set_from_pixbuf (GTK_IMAGE (image), handle);
            gtk_widget_show (image);
            gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_item), image);

            g_object_unref (handle);
        }

        gtk_widget_show (menu_item);

        gtk_menu_shell_append (GTK_MENU_SHELL (xkb->lb_mouse_popup), menu_item);
    }
}

void
xkb_refresh(t_xkb *xkb)
{
    gchar *text = g_strdup(xkb_config_get_group_name (-1));
    gchar *filepath = g_strdup(xkb_util_get_flag_filename(text));

    GdkPixbuf * pixmap = gdk_pixbuf_new_from_file_at_scale(filepath, 24, 24, FALSE, NULL);

    g_free(text);
    g_free(filepath);

    if (!pixmap)
        return;

    gtk_status_icon_set_from_pixbuf(xkb->tray, pixmap);
}

void
xkb_about(t_xkb *xkb)
{
    GtkWidget *about_dialog = gtk_message_dialog_new (NULL,
                              GTK_DIALOG_DESTROY_WITH_PARENT,
                              GTK_MESSAGE_INFO,
                              GTK_BUTTONS_CLOSE,
                              "%s\nX11 Keyboard switcher\nAuthor: Dmitriy Poltavchenko <%s>", PACKAGE_STRING, PACKAGE_BUGREPORT);
    /* Destroy the dialog when the user responds to it (e.g. clicks a button) */
    g_signal_connect_swapped (about_dialog, "response",
                              G_CALLBACK (gtk_widget_hide),
                              about_dialog);
    gtk_widget_show (about_dialog);
}

int main (int argc, char *argv[])
{
    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);
    g_set_application_name ("gxkb");

    const struct option longopts[] =
    {
        {0, 0, 0, 'v'},
        {0, 0, 0, 'h'},
        {0, 0, 0,  0 },
    };

    int index;
    int iarg=0;

    //turn off getopt error message
    opterr=0;

    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "s:vh", longopts, &index);

        switch (iarg)
        {
        case 'v':
            g_fprintf(stderr, "%s version %s\n", PACKAGE_TARNAME, PACKAGE_VERSION);
            return 0;
            break;

        case 'h':
            g_fprintf(stderr, "%s\n\n%s\n%s\n%s\n",
                      "Usage: gxkb [arguments]",
                      "Options:",
                      "-v \t Display gxkb's version number.",
                      "-h \t Show this help.");
            return 0;
            break;
        }
    }

    t_xkb *xkb = xkb_new ();

    char *config_path = (g_getenv("XDG_CONFIG_HOME") == NULL
                         ? g_build_filename(g_get_home_dir(), ".config", NULL)
                         : g_strdup(g_getenv("XDG_CONFIG_HOME")));

    char *config_file = g_strconcat(g_build_filename(config_path,
                                    g_get_application_name(), NULL), ".cfg", NULL);

    if (!g_file_test(config_path, G_FILE_TEST_EXISTS))
    {
        g_mkdir_with_parents(config_path, 0700);
        xkb_load_default(xkb);
    }
    else
    {
        if(!xkb_load_config(xkb, config_file))
        {
            xkb_load_default(xkb);
            xkb_save_config(xkb, config_file);
        }
    }

    if (xkb_config_initialize(xkb->settings, xkb_state_changed, xkb))
    {
        xkb_refresh(xkb);
        xkb_initialize_menu (xkb);
    }

    g_free(config_path);

    /* Enter the main loop */
    gtk_main ();

    if (!xkb->settings->never_modify_config)
        xkb_save_config(xkb, config_file);

    g_free(config_file);
    xkb_free(xkb);

    return 0;
}



