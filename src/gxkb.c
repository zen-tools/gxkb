/*
 * Copyright (C) 2014 Dmitriy Poltavchenko <admin@linuxhub.ru>
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

void            xkb_load_default                    ( t_xkb_settings *xkb );

void            xkb_initialize_menu                 ( t_xkb_settings *xkb );

void            xkb_refresh                         ( t_xkb_settings *xkb );

/* ================================================================== *
 *                        Implementation                              *
 * ================================================================== */

void
xkb_state_changed( gint current_group,
                   gboolean config_changed,
                   gpointer user_data )
{
    t_xkb_settings *xkb = (t_xkb_settings*)user_data;
    xkb_refresh( xkb );

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

    statusicon_free();
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

void
xkb_load_default( t_xkb_settings *xkb )
{
    if( xkb->kbd_config == NULL )
    {
        xkb->kbd_config = g_new0( t_xkb_kbd_config, 1 );
    }
    xkb->group_policy = GROUP_POLICY_PER_APPLICATION;
    xkb->never_modify_config = FALSE;
    xkb->kbd_config->model = g_strdup( "pc105" );
    xkb->kbd_config->layouts = g_strdup( "us,ru" );
    xkb->kbd_config->variants = g_strdup( "," );
    xkb->kbd_config->toggle_option = g_strdup( "grp:alt_shift_toggle,grp_led:scroll,terminate:ctrl_alt_bksp" );
    xkb->kbd_config->compose_key_position = g_strdup( "" );
}

void
xkb_refresh( t_xkb_settings *xkb )
{
    statusicon_update_current_image();
}

int main( int argc, char *argv[] )
{
    /* Initialize GTK+ */
    g_log_set_handler( "Gtk", G_LOG_LEVEL_WARNING, (GLogFunc)gtk_false  , NULL );
    gtk_init( &argc, &argv );
    g_log_set_handler( "Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL );
    g_set_application_name( PACKAGE_NAME );

    const struct option longopts[] =
    {
        { 0, 0, 0, 'v' },
        { 0, 0, 0, 'h' },
        { 0, 0, 0,  0  },
    };

    int index = 0;
    int iarg  = 0;

    //turn off getopt error message
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

    t_xkb_settings *xkb = xkb_new();

    char *config_file = xkb_util_get_config_file();

    if( !xkb_load_config( xkb, config_file ) )
    {
        xkb_load_default( xkb );
        xkb_save_config( xkb, config_file );
    }

    if( xkb_config_initialize( xkb, xkb_state_changed, xkb ) )
        xkb_refresh( xkb );

    status_icon_new();

    /* Enter the main loop */
    gtk_main();

    if( !xkb->never_modify_config )
        xkb_save_config( xkb, config_file );

    g_free( config_file );
    xkb_free( xkb );

    return 0;
}



