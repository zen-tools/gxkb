/* vim: set backspace=2 ts=4 softtabstop=4 sw=4 cinoptions=>4 expandtab autoindent smartindent: */
/* xkb-util.c
 *
 * Copyright (C) 2014 Dmitriy Poltavchenko <admin@linuxhub.ru>
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
xkb_util_get_flag_filename( const gchar* group_name )
{
    gchar* filepath;

    if( group_name == NULL )
        return NULL;

    gchar *flag_name = g_ascii_strdown( group_name, -1 );
    flag_name = g_strstrip( flag_name );

    gchar* filename = g_strconcat( flag_name, ".png", NULL );
    g_free( flag_name );

    // Try to get image from user config directory
    filepath = g_strjoin( "/", xkb_util_get_config_dir(),
                                 "flags", filename, NULL );

    if( g_file_test( filepath, G_FILE_TEST_EXISTS ) )
    {
        g_free( filename );
        return filepath;
    }

    // Try to get image from system directory
    filepath = g_strjoin( "/", FLAGSDIR, filename, NULL );

    g_free( filename );

    if( !g_file_test( filepath, G_FILE_TEST_EXISTS ) )
    {
        filepath = g_strjoin( "/", FLAGSDIR, "zz.png", NULL );
        if( !g_file_test( filepath, G_FILE_TEST_EXISTS ) )
            return NULL;
    }

    return filepath;
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
xkb_util_normalize_group_name( const gchar* group_name )
{
    const gchar *c;
    gchar *result;
    gint cut_length;
    gint index_of_na = -1;

    if( group_name == NULL )
        return NULL;

    if( strlen( group_name ) <= 3 )
        return g_strdup( group_name );

    for( c = group_name; *c; c++ )
    {
        if( !( ( *c >= 'a' && *c <= 'z' ) || ( *c >= 'A' && *c <= 'Z' ) ) )
        {
            index_of_na = c - group_name;
            break;
        }
    }

    cut_length = ( index_of_na != -1 && index_of_na <= 3 ) ? index_of_na : 3;

    result = g_strndup( group_name, cut_length );

    return result;
}

gchar*
xkb_util_get_config_dir( void )
{
    const gchar *config_path = (
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
    char *config_path = xkb_util_get_config_dir();

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

