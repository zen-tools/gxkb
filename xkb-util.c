/* vim: set backspace=2 ts=4 softtabstop=4 sw=4 cinoptions=>4 expandtab autoindent smartindent: */
/* xkb-util.c
 *
 * Copyright (C) 2013 Dmitriy Poltavchenko <zen@root.ua>
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
xkb_util_get_flag_filename (const gchar* group_name)
{
    gchar* filename;
    gchar* filepath;

    if (!group_name)
        return NULL;

    gchar *flag_name = g_ascii_strdown(group_name, -1);
    flag_name = g_strstrip(flag_name);

    filename = g_strconcat(flag_name, ".png", NULL);
    g_free(flag_name);

    // Try to get image from user config directory
    filepath = g_build_filename(xkb_util_get_config_dir(), 
                                "flags", filename, NULL);

    if (g_file_test(filepath, G_FILE_TEST_EXISTS)) {
        g_free(filename);
        return filepath;
    }

    // Try to get image from system directory
    filepath = g_build_filename(FLAGSDIR, filename, NULL);

    g_free(filename);

    if (!g_file_test(filepath, G_FILE_TEST_EXISTS))
    {
        filepath = g_build_filename(FLAGSDIR, "zz.png", NULL);
        if (!g_file_test(filepath, G_FILE_TEST_EXISTS))
            return NULL;
    }

    return filepath;
}

gchar*
xkb_util_get_layout_string (const gchar *group_name, const gchar *variant)
{
    if (!group_name)
        return NULL;

    return (variant && strlen (variant) > 0)
           ? g_strconcat (group_name, " (", variant, ")", NULL)
           : g_strconcat (group_name, NULL);
}

gchar*
xkb_util_normalize_group_name (const gchar* group_name)
{
    gchar *c;
    gchar *result;
    gint cut_length;
    gint index_of_na = -1;
    gint index_tmp = -1;

    if (!group_name)
        return NULL;

    if (strlen (group_name) <= 3)
        return g_strdup (group_name);

    c = g_strdup (group_name);

    while (*c)
    {
        index_tmp++;

        if (!((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z')))
        {
            index_of_na = index_tmp;
            break;
        }

        c++;
    }

    cut_length = (index_of_na != -1 && index_of_na <= 3) ? index_of_na : 3;

    result = g_strndup (group_name, cut_length);

    g_free (c);

    return result;
}

gchar*
xkb_util_get_config_dir (void)
{
    gchar *config_path =  (g_getenv("XDG_CONFIG_HOME") == NULL
            ? g_build_filename(g_get_home_dir(), ".config", NULL)
            : g_strdup(g_getenv("XDG_CONFIG_HOME"))
    );

    config_path = g_build_path(
        "/",
        config_path,
        g_get_application_name(),
        NULL
    );

    if (!g_file_test(config_path, G_FILE_TEST_EXISTS)) {
        g_mkdir_with_parents(config_path, 0700);
    }

    return config_path;
}

gchar*
xkb_util_get_config_file (void)
{
    char *config_path = xkb_util_get_config_dir();
    return g_strconcat(
        g_build_filename(
            config_path,
            g_get_application_name(),
            NULL
        ),
        ".cfg",
        NULL
    );
}

