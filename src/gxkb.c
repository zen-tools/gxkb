/* gxkb.c
 *
 * Copyright (C) 2016 Dmitriy Poltavchenko <admin@linuxhub.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <getopt.h>
#include <glib/gstdio.h>

#include "xkb-config.h"
#include "xkb-util.h"
#include "xkb-callbacks.h"
#include "statusicon.h"

/* ----------------------------------------------------------------- *
 *                           XKB Stuff                               *
 * ----------------------------------------------------------------- */

void            xkb_state_changed                   ( gint current_group,
                                                      gboolean config_changed,
                                                      gpointer user_data );

t_xkb_settings *xkb_new                             ( void );

void            xkb_free                            ( t_xkb_settings *xkb );

void            xkb_save_config                     ( t_xkb_settings *xkb,
                                                      const gchar *filename );

gboolean        xkb_load_config                     ( t_xkb_settings *xkb,
                                                      const gchar *filename );

gboolean        xkb_is_config_changed               ( t_xkb_settings *xkb_old,
                                                      t_xkb_settings *xkb_new );

void            xkb_load_default                    ( t_xkb_settings *xkb );

/* ================================================================== *
 *                        Implementation                              *
 * ================================================================== */

void
xkb_state_changed( gint current_group,
                   gboolean config_changed,
                   gpointer user_data )
{
    t_xkb_settings *xkb = (t_xkb_settings*)user_data;
    statusicon_update_current_image();

    if( config_changed )
        statusicon_update_menu();
}

t_xkb_settings *
xkb_new( void )
{
    t_xkb_settings *xkb;
    WnckScreen *wnck_screen;
    wnck_screen = wnck_screen_get_default();

    xkb = g_new0( t_xkb_settings, 1 );

    g_signal_connect( G_OBJECT( wnck_screen ), "active-window-changed",
                      G_CALLBACK (xkb_active_window_changed ), xkb );

    g_signal_connect( G_OBJECT( wnck_screen ), "window-closed",
                      G_CALLBACK (xkb_window_closed ), xkb );

    g_signal_connect( G_OBJECT( wnck_screen ), "application-closed",
                      G_CALLBACK (xkb_application_closed ), xkb );

    return xkb;
}

void
xkb_free( t_xkb_settings *xkb )
{
    xkb_config_finalize();

    if( xkb->kbd_config )
        kbd_config_free( xkb->kbd_config );

    g_free( xkb );
}

void
xkb_save_config( t_xkb_settings *xkb, const gchar *config_file )
{
    GKeyFile *cfg_file = g_key_file_new();
    g_key_file_set_list_separator( cfg_file, ',' );

    g_key_file_set_integer(
        cfg_file,
        "xkb config",
        "group_policy",
        xkb->group_policy
    );

    g_key_file_set_integer(
        cfg_file,
        "xkb config",
        "default_group",
        xkb->default_group
    );

    g_key_file_set_boolean(
        cfg_file,
        "xkb config",
        "never_modify_config",
        xkb->never_modify_config
    );

    if( xkb->kbd_config != NULL )
    {
        g_key_file_set_string(
            cfg_file,
            "xkb config",
            "model",
            xkb->kbd_config->model
        );

        g_key_file_set_string(
            cfg_file,
            "xkb config",
            "layouts",
            xkb->kbd_config->layouts
        );

        g_key_file_set_string(
            cfg_file,
            "xkb config",
            "variants",
            xkb->kbd_config->variants
        );

        if( xkb->kbd_config->toggle_option == NULL )
        {
            g_key_file_set_string(
                cfg_file,
                "xkb config",
                "toggle_option",
                ""
            );
        }
        else
        {
            g_key_file_set_string(
                cfg_file,
                "xkb config",
                "toggle_option",
                xkb->kbd_config->toggle_option
            );
        }

        if( xkb->kbd_config->compose_key_position == NULL )
        {
            g_key_file_set_string(
                cfg_file,
                "xkb config",
                "compose_key_position",
                ""
            );
        }
        else
        {
            g_key_file_set_string(
                cfg_file,
                "xkb config",
                "compose_key_position",
                xkb->kbd_config->compose_key_position
            );
        }
    }

    gsize len;
    gchar *str_data;
    str_data = g_key_file_to_data( cfg_file, &len, NULL );
    g_file_set_contents( config_file, str_data, len, NULL );
    g_free( str_data );
    g_key_file_free( cfg_file );
}

gboolean
xkb_load_config( t_xkb_settings *xkb, const gchar *filename )
{
    GKeyFile *cfg_file = g_key_file_new();
    if( !g_key_file_load_from_file( cfg_file, filename, G_KEY_FILE_KEEP_COMMENTS, NULL ) )
    {
        g_key_file_free( cfg_file );
        return FALSE;
    }

    xkb->group_policy = g_key_file_get_integer(
        cfg_file,
        "xkb config",
        "group_policy",
        NULL
    );

    if( xkb->group_policy != GROUP_POLICY_GLOBAL )
    {
        xkb->default_group = g_key_file_get_integer(
            cfg_file,
            "xkb config",
            "default_group",
            NULL
        );
    }

    xkb->never_modify_config = g_key_file_get_boolean(
        cfg_file,
        "xkb config",
        "never_modify_config",
        NULL
    );

    if( xkb->kbd_config == NULL )
        xkb->kbd_config = g_new0( t_xkb_kbd_config, 1 );


    xkb->kbd_config->model = g_key_file_get_string(
        cfg_file,
        "xkb config",
        "model",
        NULL
    );

    xkb->kbd_config->layouts = g_key_file_get_string(
        cfg_file,
        "xkb config",
        "layouts",
        NULL
    );

    xkb->kbd_config->variants = g_key_file_get_string(
        cfg_file,
        "xkb config",
        "variants",
        NULL
    );

    xkb->kbd_config->toggle_option = g_key_file_get_string(
        cfg_file,
        "xkb config",
        "toggle_option",
        NULL
    );

    xkb->kbd_config->compose_key_position = g_key_file_get_string(
        cfg_file,
        "xkb config",
        "compose_key_position",
        NULL
    );

    g_key_file_free( cfg_file );

    return TRUE;
}

gboolean
xkb_is_config_changed( t_xkb_settings *xkb_old, t_xkb_settings *xkb_new )
{
    if( !xkb_old->kbd_config || !xkb_new->kbd_config )
        return FALSE;

    if( xkb_old->group_policy != xkb_new->group_policy)
        return TRUE;

    if( xkb_old->never_modify_config != xkb_new->never_modify_config )
        return TRUE;

    if( g_strcmp0( xkb_old->kbd_config->model, xkb_new->kbd_config->model ) != 0 )
        return TRUE;

    if( g_strcmp0( xkb_old->kbd_config->layouts, xkb_new->kbd_config->layouts ) != 0 )
        return TRUE;

    if( g_strcmp0( xkb_old->kbd_config->variants, xkb_new->kbd_config->variants ) != 0 )
        return TRUE;

    if( g_strcmp0( xkb_old->kbd_config->toggle_option, xkb_new->kbd_config->toggle_option ) != 0 )
        return TRUE;

    if( g_strcmp0( xkb_old->kbd_config->compose_key_position, xkb_new->kbd_config->compose_key_position ) != 0 )
        return TRUE;

    return FALSE;
}

int main( int argc, char *argv[] )
{
    // Initialize GTK+
    gtk_init( &argc, &argv );
    g_set_application_name( PACKAGE_NAME );
    g_log_set_handler( "Wnck", G_LOG_LEVEL_WARNING, (GLogFunc)gtk_false  , NULL );

    const struct option longopts[] =
    {
        { 0, 0, 0, 'v' },
        { 0, 0, 0, 'h' },
        { 0, 0, 0,  0  },
    };

    int index = 0;
    int iarg  = 0;

    // turn off getopt error message
    opterr = 0;

    while( iarg != -1 )
    {
        iarg = getopt_long( argc, argv, "s:vh", longopts, &index );

        switch( iarg )
        {
        case 'v':
            g_fprintf( stderr, "%s version %s\n", PACKAGE_NAME, PACKAGE_VERSION );
            g_fprintf( stderr, "Features:\n" );
            #ifdef HAVE_APPINDICATOR
                g_fprintf( stderr, "AppIndicator support - Yes\n" );
            #else
                g_fprintf( stderr, "AppIndicator support - No\n" );
            #endif
            return 0;
            break;

        case 'h':
            g_fprintf( stderr, "%s\n\n%s\n%s\n%s\n",
                       "Usage: gxkb [arguments]",
                       "Options:",
                       "-v \t Display gxkb's version number.",
                       "-h \t Show this help." );
            return 0;
            break;
        }
    }

    gchar *config_file = xkb_util_get_config_file();

    gboolean first_run = FALSE;
    t_xkb_settings *xkb = xkb_new();
    if( !xkb_load_config( xkb, config_file ) )
    {
        first_run = TRUE;
        xkb->group_policy = GROUP_POLICY_PER_APPLICATION;
    }

    if( !xkb_config_initialize( xkb, xkb_state_changed, xkb ) )
    {
        g_fprintf( stderr, "Can't get instance of the X display.\n" );
        return 1;
    }

    if( first_run )
        xkb_save_config( xkb, config_file );

    statusicon_new();

    // Save original config
    t_xkb_settings *orig_config = g_new0( t_xkb_settings, 1 );
    xkb_load_config( orig_config, config_file );

    /* Enter the main loop */
    gtk_main();

    // Load config and check if it was not changed
    t_xkb_settings *last_config = g_new0( t_xkb_settings, 1 );
    xkb_load_config( last_config, config_file );
    gboolean is_diff = xkb_is_config_changed( orig_config, last_config );

    if( is_diff )
        g_warning("Config file was changed. Saving skipped.\n");
    else if( xkb->never_modify_config )
        g_warning("Saving skipped by your configuration.\n");
    else
        xkb_save_config( xkb, config_file );

    g_free( orig_config );
    g_free( last_config );
    g_free( config_file );
    xkb_free( xkb );
    statusicon_free();

    return 0;
}
