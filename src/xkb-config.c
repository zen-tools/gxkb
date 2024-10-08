/* xkb-config.c
 *
 * Copyright (C) 2016 Dmytro Poltavchenko <dmytro.poltavchenko@gmail.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gdk/gdkx.h>
#include <glib.h>

#include "xkb-config.h"

#ifndef DEBUG
#define G_DISABLE_ASSERT
#endif

typedef struct {
  gchar *group_name;
  gchar *variant;
  gchar *pretty_layout_name;
} t_group_data;

typedef struct {
  XklEngine *engine;

  t_group_data *group_data;
  t_xkb_settings *settings;

  GHashTable *application_map;
  GHashTable *window_map;

  guint current_window_id;
  guint current_application_id;

  gint group_count;
  gint current_group;

  XkbCallback callback;
  gpointer callback_data;

  guint config_changed_timeout_id;

  XklConfigRec *config_rec;
} t_xkb_config;

t_xkb_config *config;

void xkb_config_xkl_state_changed(XklEngine *engine,
                                  XklEngineStateChange *change, gint group,
                                  gboolean restore);

void xkb_config_xkl_config_changed(XklEngine *engine, t_xkb_config *config);

void xkb_config_xkl_new_device(XklEngine *engine);

GdkFilterReturn handle_xevent(GdkXEvent *xev, GdkEvent *event);

void xkb_config_free(void);

void xkb_config_initialize_xkb_options(t_xkb_settings *settings);

void xkb_config_backup_maps(GHashTable *window_map,
                            GHashTable *application_map);

void xkb_config_restore_maps(GHashTable *window_map,
                             GHashTable *application_map);

/* ---------------------- implementation ------------------------- */
gchar *xkb_config_xkb_description(XklConfigItem *config_item) {
  gchar *ci_description = g_strstrip(config_item->description);
  if (ci_description[0] == 0)
    return g_strdup(config_item->name);

  return g_strdup(ci_description);
}

gchar *xkb_config_create_pretty_layout_name(XklConfigRegistry *registry,
                                            XklConfigItem *config_item,
                                            gchar *layout_name,
                                            gchar *layout_variant) {
  if (g_strcmp0(layout_name, "cz_qwerty") == 0) {
    // cz_qwerty = pc+cz(qwerty)
    // https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=514409
    layout_name = "cz";
    layout_variant = "qwerty";
  }

  g_snprintf(config_item->name, sizeof(config_item->name), "%s",
             layout_variant);
  if (xkl_config_registry_find_variant(registry, layout_name, config_item))
    return g_strdup(xkb_config_xkb_description(config_item));

  g_snprintf(config_item->name, sizeof(config_item->name), "%s", layout_name);
  if (xkl_config_registry_find_layout(registry, config_item))
    return g_strdup(xkb_config_xkb_description(config_item));

  return g_strdup(g_ascii_strup(
      xkb_util_get_layout_string(layout_name, layout_variant), -1));
}

gboolean xkb_config_initialize(t_xkb_settings *settings, XkbCallback callback,
                               gpointer callback_data) {
  g_assert(settings != NULL);

  config = g_new0(t_xkb_config, 1);

  config->settings = settings;

  config->callback = callback;
  config->callback_data = callback_data;

  config->engine =
      xkl_engine_get_instance(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()));

  if (!config->engine)
    return FALSE;

  xkb_config_update_settings(settings, config->engine);

  xkl_engine_set_group_per_toplevel_window(config->engine, FALSE);

  xkl_engine_start_listen(config->engine, XKLL_TRACK_KEYBOARD_STATE);

  g_signal_connect(config->engine, "X-state-changed",
                   G_CALLBACK(xkb_config_xkl_state_changed), NULL);

  g_signal_connect(config->engine, "X-config-changed",
                   G_CALLBACK(xkb_config_xkl_config_changed), config);

  g_signal_connect(config->engine, "X-new-device",
                   G_CALLBACK(xkb_config_xkl_new_device), NULL);

  gdk_window_add_filter(NULL, (GdkFilterFunc)handle_xevent, NULL);

  return TRUE;
}

void xkb_config_initialize_xkb_options(t_xkb_settings *settings) {
  XklConfigRegistry *registry;
  XklConfigItem *config_item;
  gchar **group;
  gint val, i;
  gpointer pval;

  xkb_config_free();

  group = config->config_rec->layouts;
  config->group_count = 0;
  while (*group) {
    group++;
    config->group_count++;
  }

  config->window_map = g_hash_table_new(g_direct_hash, NULL);
  config->application_map = g_hash_table_new(g_direct_hash, NULL);
  config->group_data =
      (t_group_data *)g_new0(typeof(t_group_data), config->group_count);

  registry = xkl_config_registry_get_instance(config->engine);
  xkl_config_registry_load(registry, FALSE);
  config_item = xkl_config_item_new();

  for (i = 0; i < config->group_count; i++) {
    t_group_data *group_data = &config->group_data[i];
    group_data->group_name = g_strdup(config->config_rec->layouts[i]);
    group_data->variant = (config->config_rec->variants[i] == NULL)
                              ? g_strdup("")
                              : g_strdup(config->config_rec->variants[i]);
    group_data->pretty_layout_name = xkb_config_create_pretty_layout_name(
        registry, config_item, group_data->group_name, group_data->variant);
  }
  g_object_unref(config_item);
  g_object_unref(registry);
}

void kbd_config_free(t_xkb_kbd_config *kbd_config) {
  g_free(kbd_config->model);
  g_free(kbd_config->layouts);
  g_free(kbd_config->variants);
  g_free(kbd_config->toggle_option);
  g_free(kbd_config->compose_key_position);
  g_free(kbd_config);
}

void xkb_config_free(void) {
  gint i;

  g_assert(config != NULL);

  if (config->window_map)
    g_hash_table_destroy(config->window_map);

  if (config->application_map)
    g_hash_table_destroy(config->application_map);

  if (config->group_data) {
    for (i = 0; i < config->group_count; i++) {
      t_group_data *group_data = &config->group_data[i];
      g_free(group_data->group_name);
      g_free(group_data->variant);
      g_free(group_data->pretty_layout_name);
    }
    g_free(config->group_data);
  }
}

void xkb_config_finalize(void) {
  xkl_engine_stop_listen(config->engine, XKLL_TRACK_KEYBOARD_STATE);

  g_object_unref(config->config_rec);

  xkb_config_free();
  g_free(config);

  gdk_window_remove_filter(NULL, (GdkFilterFunc)handle_xevent, NULL);
}

gint xkb_config_get_current_group(void) { return config->current_group; }

gboolean xkb_config_set_group(gint group) {
  g_assert(config != NULL);

  if (G_UNLIKELY(group < 0 || group >= config->group_count))
    return FALSE;

  xkl_engine_lock_group(config->engine, group);
  config->current_group = group;

  return TRUE;
}

gboolean xkb_config_next_group(void) {
  xkl_engine_lock_group(config->engine,
                        xkl_engine_get_next_group(config->engine));

  return TRUE;
}

gboolean xkb_config_prev_group(void) {
  xkl_engine_lock_group(config->engine,
                        xkl_engine_get_prev_group(config->engine));

  return TRUE;
}

void xkb_config_backup_maps(GHashTable *window_map,
                            GHashTable *application_map) {
  g_assert(config != NULL);

  GHashTableIter iter;
  gpointer key, value;

  if (config->window_map) {
    g_hash_table_iter_init(&iter, config->window_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
      g_hash_table_insert(
          window_map, key,
          strdup(xkb_config_get_group_name(GPOINTER_TO_INT(value))));
    }
  }

  if (config->application_map) {
    g_hash_table_iter_init(&iter, config->application_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
      g_hash_table_replace(
          application_map, key,
          strdup(xkb_config_get_group_name((GPOINTER_TO_INT(value)))));
    }
  }
}

void xkb_config_restore_maps(GHashTable *window_map,
                             GHashTable *application_map) {
  g_assert(config != NULL);

  gpointer pval;
  GHashTable *index_variants = g_hash_table_new(g_str_hash, g_str_equal);
  gint i;
  for (i = 0; i < config->group_count; i++) {
    gchar *group_name = config->group_data[i].group_name;

    g_hash_table_replace(index_variants, group_name, GINT_TO_POINTER(i));
  }

  GHashTableIter iter;
  gpointer key, value;

  if (config->window_map && window_map) {
    g_hash_table_iter_init(&iter, window_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
      gint pid = GPOINTER_TO_INT(key);
      pval = g_hash_table_lookup(index_variants, value);

      if (pval == NULL)
        continue;

      g_hash_table_replace(config->window_map, GINT_TO_POINTER(pid), pval);
    }
  }

  if (config->application_map && application_map) {
    g_hash_table_iter_init(&iter, application_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
      gint pid = GPOINTER_TO_INT(key);
      pval = g_hash_table_lookup(index_variants, value);

      if (pval == NULL)
        continue;

      g_hash_table_replace(config->application_map, GINT_TO_POINTER(pid), pval);
    }
  }

  g_hash_table_destroy(index_variants);
}

gboolean xkb_config_update_settings(t_xkb_settings *settings,
                                    XklEngine *engine) {
  gboolean activate_settings = FALSE;

  g_assert(config != NULL);
  g_assert(settings != NULL);

  GHashTable *window_map_old = g_hash_table_new(g_direct_hash, NULL);
  GHashTable *application_map_old = g_hash_table_new(g_direct_hash, NULL);

  xkb_config_backup_maps(window_map_old, application_map_old);

  config->settings = settings;

  if (config->config_rec == NULL)
    config->config_rec = xkl_config_rec_new();

  if (settings->kbd_config == NULL || settings->never_modify_config) {
    xkl_config_rec_get_from_server(config->config_rec, engine);
    if (settings->kbd_config == NULL)
      settings->kbd_config = g_new0(t_xkb_kbd_config, 1);

    g_free(settings->kbd_config->model);
    settings->kbd_config->model = g_strdup(config->config_rec->model);
    g_free(settings->kbd_config->layouts);
    settings->kbd_config->layouts =
        g_strjoinv(",", config->config_rec->layouts);

    // Next code is hack to populate settings->kbd_config->variants
    // with valid config string, because XklConfigRec like to return NULL
    // if variant was not set, what can lead to wrong keyboard settings
    gchar *tmp1 = g_strdup("");
    gchar *tmp2 = NULL;
    gint i;
    for (i = 0; config->config_rec->layouts[i]; i++) {
      tmp2 = g_strconcat(tmp1,
                         (config->config_rec->variants[i])
                             ? config->config_rec->variants[i]
                             : "",
                         NULL);
      g_free(tmp1);
      tmp1 = tmp2;
      if (config->config_rec->layouts[i + 1]) {
        tmp2 = g_strconcat(tmp1, ",", NULL);
        g_free(tmp1);
        tmp1 = tmp2;
      }
    }
    g_free(settings->kbd_config->variants);
    settings->kbd_config->variants = tmp2;
  } else {
    gchar *options;

    activate_settings = TRUE;

    g_free(config->config_rec->model);
    config->config_rec->model = g_strdup(settings->kbd_config->model);

    g_strfreev(config->config_rec->layouts);

    config->config_rec->layouts =
        g_strsplit_set(settings->kbd_config->layouts, ",", 0);

    g_strfreev(config->config_rec->variants);
    config->config_rec->variants =
        g_strsplit_set(settings->kbd_config->variants, ",", 0);

    if (settings->kbd_config->toggle_option &&
        strlen(settings->kbd_config->toggle_option) > 0) {
      options = g_strdup(settings->kbd_config->toggle_option);
    } else {
      options = g_strdup("");
    }

    if (settings->kbd_config->compose_key_position &&
        strlen(settings->kbd_config->compose_key_position) > 0) {
      gchar *tmp = options;
      options = g_strconcat(tmp, ",",
                            settings->kbd_config->compose_key_position, NULL);
      g_free(tmp);
    }

    g_strfreev(config->config_rec->options);
    config->config_rec->options = g_strsplit_set(options, ",", 0);
    g_free(options);
  }

  g_free(settings->kbd_config->toggle_option);
  settings->kbd_config->toggle_option = NULL;
  g_free(settings->kbd_config->compose_key_position);
  settings->kbd_config->compose_key_position = g_strdup("");
  gchar **opt = config->config_rec->options;
  while (opt && *opt) {
    gchar **prefix = g_strsplit(*opt, ":", 2);
    if (prefix && *prefix && strcmp(*prefix, "compose") != 0) {
      if (settings->kbd_config->toggle_option == NULL) {
        settings->kbd_config->toggle_option = g_strdup(*opt);
      } else {
        settings->kbd_config->toggle_option = g_strconcat(
            settings->kbd_config->toggle_option, ",", g_strdup(*opt), NULL);
      }
    } else if (prefix && *prefix && strcmp(*prefix, "compose") == 0) {
      settings->kbd_config->compose_key_position = g_strdup(*opt);
    }

    g_strfreev(prefix);
    opt++;
  }

  if (activate_settings && !settings->never_modify_config)
    xkl_config_rec_activate(config->config_rec, engine);

  xkb_config_initialize_xkb_options(settings);

  xkb_config_restore_maps(window_map_old, application_map_old);

  g_hash_table_destroy(window_map_old);
  g_hash_table_destroy(application_map_old);

  return TRUE;
}

void xkb_config_window_changed(guint new_window_id, guint application_id) {
  gint group;
  gpointer key, value;
  GHashTable *hashtable;
  guint id;
  g_assert(config != NULL);

  id = 0;
  hashtable = NULL;

  switch (config->settings->group_policy) {
  case GROUP_POLICY_GLOBAL:
    return;

  case GROUP_POLICY_PER_WINDOW:
    hashtable = config->window_map;
    id = new_window_id;
    config->current_window_id = id;
    break;

  case GROUP_POLICY_PER_APPLICATION:
    hashtable = config->application_map;
    id = application_id;
    config->current_application_id = id;
    break;
  }

  group = config->settings->default_group;

  if (g_hash_table_lookup_extended(hashtable, GINT_TO_POINTER(id), &key,
                                   &value))
    group = GPOINTER_TO_INT(value);
  else {
    g_hash_table_insert(hashtable, GINT_TO_POINTER(id), GINT_TO_POINTER(group));
  }

  xkb_config_set_group(group);
}

void xkb_config_application_closed(guint application_id) {
  g_assert(config != NULL);

  switch (config->settings->group_policy) {
  case GROUP_POLICY_GLOBAL:
  case GROUP_POLICY_PER_WINDOW:
    return;

  case GROUP_POLICY_PER_APPLICATION:
    g_hash_table_remove(config->application_map,
                        GINT_TO_POINTER(application_id));
    break;
  }
}

void xkb_config_window_closed(guint window_id) {
  g_assert(config != NULL);

  switch (config->settings->group_policy) {
  case GROUP_POLICY_GLOBAL:
  case GROUP_POLICY_PER_APPLICATION:
    return;

  case GROUP_POLICY_PER_WINDOW:
    g_hash_table_remove(config->window_map, GINT_TO_POINTER(window_id));
    break;
  }
}

gint xkb_config_get_group_count(void) {
  g_assert(config != NULL);

  return config->group_count;
}

const gchar *xkb_config_get_group_name(gint group) {
  g_assert(config != NULL);

  if (group == -1)
    group = xkb_config_get_current_group();

  if (G_UNLIKELY(group < 0 || group >= config->group_count))
    return NULL;

  return g_strdup(config->group_data[group].group_name);
}

const gchar *xkb_config_get_variant(gint group) {
  g_assert(config != NULL);

  if (group == -1)
    group = xkb_config_get_current_group();

  if (G_UNLIKELY(group < 0 || group >= config->group_count))
    return NULL;

  return g_strdup(config->group_data[group].variant);
}

const gchar *xkb_config_get_pretty_layout_name(gint group) {
  g_assert(config != NULL);

  if (group == -1)
    group = xkb_config_get_current_group();

  if (G_UNLIKELY(group < 0 || group >= config->group_count))
    return NULL;

  return g_strdup(config->group_data[group].pretty_layout_name);
}

void xkb_config_xkl_state_changed(XklEngine *engine,
                                  XklEngineStateChange *change, gint group,
                                  gboolean restore) {
  if (change == GROUP_CHANGED) {
    config->current_group = group;
    switch (config->settings->group_policy) {
    case GROUP_POLICY_GLOBAL:
      break;

    case GROUP_POLICY_PER_WINDOW:
      g_hash_table_replace(config->window_map,
                           GINT_TO_POINTER(config->current_window_id),
                           GINT_TO_POINTER(group));
      break;

    case GROUP_POLICY_PER_APPLICATION:
      g_hash_table_replace(config->application_map,
                           GINT_TO_POINTER(config->current_application_id),
                           GINT_TO_POINTER(group));
      break;
    }

    if (config->callback != NULL)
      config->callback(group, FALSE, config->callback_data);
  }
}

static gboolean xkb_keyboard_xkl_config_changed_timeout (gpointer user_data) {
  t_xkb_config *config = user_data;
  gchar *previous_active_group_name = strdup(xkb_config_get_group_name(-1));

  kbd_config_free(config->settings->kbd_config);
  config->settings->kbd_config = NULL;
  xkb_config_update_settings(config->settings, config->engine);

  // If group name for current window is mismatch, then it means previous
  // group name is not exist in new configuration, let's reset it
  if (g_strcmp0(previous_active_group_name, xkb_config_get_group_name(-1)))
    xkb_config_set_group(0);

  g_free(previous_active_group_name);

  if (config->callback != NULL) {
    // Notify main application that configuration was changed
    config->callback(xkb_config_get_current_group(), TRUE,
                     config->callback_data);

    // Make main application think that current layout "changed"
    // to force redraw current keyboard layout icon
    config->callback(xkb_config_get_current_group(), FALSE,
                     config->callback_data);
  }

  config->config_changed_timeout_id = 0;
  return G_SOURCE_REMOVE;
}

void xkb_config_xkl_config_changed(XklEngine *engine, t_xkb_config *config) {
  // libxklavier may send default Xorg settings first,
  // which leads to losing the actual keyboard layout settings.
  // To avoid this, we run the callback with a delay.
  // If there was already one scheduled, then we drop it.

  if (config->config_changed_timeout_id != 0)
    g_source_remove(config->config_changed_timeout_id);

  config->config_changed_timeout_id = g_timeout_add(
    100, xkb_keyboard_xkl_config_changed_timeout, config);
}

void xkb_config_xkl_new_device(XklEngine *engine) {
  if (!config->settings->never_modify_config)
    xkl_config_rec_activate(config->config_rec, engine);
}

GdkFilterReturn handle_xevent(GdkXEvent *xev, GdkEvent *event) {
  XEvent *xevent = (XEvent *)xev;

  xkl_engine_filter_events(config->engine, xevent);

  return GDK_FILTER_CONTINUE;
}
