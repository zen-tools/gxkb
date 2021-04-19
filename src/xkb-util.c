/* xkb-util.c
 *
 * Copyright (C) 2016 Dmitriy Poltavchenko <poltavchenko.dmitriy@gmail.com>
 *
 * Copyright (C) 2008 Alexander Iliev <sasoiliev@mamul.org>
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

#include <string.h>

#include "xkb-util.h"

gchar*
xkb_util_get_flag_filename( const gchar* group_name, const gchar *variant )
{
    if( group_name == NULL )
        return NULL;

    GSList* flag_names = NULL;
    if(variant && strlen( variant ) > 0)
    {
        flag_names = g_slist_prepend(
            flag_names,
            g_strjoin(
                "_", g_ascii_strdown( group_name, -1 ),
                g_ascii_strdown( variant, -1 ), NULL
            )
        );
    }
    flag_names = g_slist_prepend( flag_names, g_ascii_strdown( group_name, -1 ) );
    flag_names = g_slist_prepend( flag_names, "zz" );
    flag_names = g_slist_reverse( flag_names );

    gchar* file_path = NULL;
    do {
        gchar* file_name = g_strconcat( flag_names->data, ".png", NULL );

        // Try to get image from user data directory
        file_path = g_strjoin( "/", xkb_util_get_data_dir(), "flags", file_name, NULL );
        if( g_file_test( file_path, G_FILE_TEST_EXISTS ) ) {
            g_free( file_name );
            break;
        }
        g_free( file_path );

        // Try to get image from system directory
        file_path = g_strjoin( "/", FLAGSDIR, file_name, NULL );
        if( g_file_test( file_path, G_FILE_TEST_EXISTS ) ) {
            g_free( file_name );
            break;
        }
        g_free( file_path );

        g_free( file_name );
    } while( (flag_names = g_slist_next( flag_names )) );

    g_slist_free( flag_names );
    return file_path;
}

gchar*
xkb_util_get_layout_string( const gchar *group_name, const gchar *variant )
{
    if( group_name == NULL )
        return NULL;

    return ( variant && strlen( variant ) > 0 )
           ? g_strconcat( group_name, " (", variant, ")", NULL )
           : g_strconcat( group_name, NULL );
}

gchar*
xkb_util_get_data_dir( void )
{
    gchar *data_path = (gchar *)(
        ( g_getenv( "XDG_DATA_HOME" ) == NULL )
        ? g_strjoin( "/", g_get_home_dir(), ".local/share", NULL )
        : g_getenv( "XDG_DATA_HOME" )
    );

    data_path = (gchar*)g_strjoin(
        "/",
        data_path,
        PACKAGE_NAME,
        NULL
    );

    if( !g_file_test( data_path, G_FILE_TEST_EXISTS ) )
        g_mkdir_with_parents( data_path, 0700 );

    return data_path;
}

gchar*
xkb_util_get_config_dir( void )
{
    gchar *config_path = (gchar*)(
        ( g_getenv( "XDG_CONFIG_HOME" ) == NULL )
        ? g_strjoin( "/", g_get_home_dir(), ".config", NULL )
        : g_getenv( "XDG_CONFIG_HOME" )
    );

    config_path = g_strjoin(
        "/",
        config_path,
        PACKAGE_NAME,
        NULL
    );

    if( !g_file_test( config_path, G_FILE_TEST_EXISTS ) )
        g_mkdir_with_parents( config_path, 0700 );

    return config_path;
}

gchar*
xkb_util_get_config_file( void )
{
    gchar *config_path = xkb_util_get_config_dir();

    return g_strjoin(
        "/",
        config_path,
        g_strconcat(
            PACKAGE_NAME,
            ".cfg",
            NULL
        ),
        NULL
    );
}
