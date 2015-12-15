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

    if (!group_name)
        return NULL;

    gchar *flag_name = g_ascii_strdown(group_name, -1);
    flag_name = g_strstrip(flag_name);

    filename = g_strconcat (FLAGSDIR, "/", flag_name, ".png", NULL);

    g_free(flag_name);

    if (!g_file_test(filename, G_FILE_TEST_EXISTS))
    {
        filename = g_strconcat(FLAGSDIR, "/", "zz.png", NULL);
        if (!g_file_test(filename, G_FILE_TEST_EXISTS))
            return NULL;
    }

    return filename;
}

gchar*
xkb_util_get_layout_string (const gchar *group_name, const gchar *variant)
{
    gchar *layout;

    if (!group_name)
        return NULL;

    if (variant && strlen (variant) > 0)
    {
        layout = g_strconcat (group_name, " (", variant, ")", NULL);
    }
    else
    {
        layout = g_strconcat (group_name, NULL);
    }

    return layout;
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

