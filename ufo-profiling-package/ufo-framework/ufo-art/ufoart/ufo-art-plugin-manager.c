/*
 * Copyright (C) 2011-2013 Karlsruhe Institute of Technology
 *
 * This file is part of Ufo.
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <ufoart/ufo-art-plugin-manager.h>

static gchar *
transform_string (const gchar *pattern,
                  const gchar *s,
                  const gchar *separator)
{
    gchar **sv;
    gchar *transformed;
    gchar *result;

    sv = g_strsplit_set (s, "-_ ", -1);
    transformed = g_strjoinv (separator, sv);
    result = g_strdup_printf (pattern, transformed);

    g_strfreev (sv);
    g_free (transformed);
    return result;
}

/**
 * ufo_plugin_manager_get_art:
 * @manager: A #UfoART
 * @name: Name of the algebraic reconstruction technique.
 * @error: return location for a GError or %NULL
 *
 * Load a #UfoART module and return an instance. The shared object name must
 * be * constructed as "libufoartsolver@name.so".
 *
 * Returns: (transfer full): (allow-none): #UfoART or %NULL if module cannot be found
 */
UfoART *
ufo_plugin_manager_get_art(UfoPluginManager   *manager,
                           const gchar        *name,
                           GError            **error)
{
    g_return_val_if_fail (UFO_IS_PLUGIN_MANAGER (manager) && name != NULL, NULL);
    UfoART *art;

    gchar *module_name = transform_string ("libufoartsolver%s.so", name, NULL);
    gchar *func_name = transform_string ("ufo_%s_new", name, "_");
    

    art = ufo_plugin_manager_get_plugin (manager,
                                         func_name,
                                         module_name,
                                         error);
    g_free (func_name);
    g_free (module_name);

    g_message ("UfoPluginManager: Created UfoART's %s-%p", name, (void*)art);
    return UFO_ART(art);
}

/**
 * ufo_plugin_manager_get_projector:
 * @manager: A #UfoPluginManager
 * @name: Name of the projector.
 * @error: return location for a GError or %NULL
 *
 * Load a #UfoProjector module and return an instance. The shared object name must
 * be * constructed as "libufoartprojector@name.so".
 *
 * Returns: (transfer full): (allow-none): #UfoProjector or %NULL if module cannot be found
 */
UfoProjector *
ufo_plugin_manager_get_projector(UfoPluginManager   *manager,
                                 const gchar        *name,
                                 GError            **error)
{
    g_return_val_if_fail (UFO_IS_PLUGIN_MANAGER (manager) && name != NULL, NULL);
    UfoProjector *projector;

    gchar *module_name = transform_string ("libufoartprojector%s.so", name, NULL);
    gchar *func_name = transform_string ("ufo_%s_projector_new", name, "_");
    

    projector = ufo_plugin_manager_get_plugin (manager,
                                               func_name,
                                               module_name,
                                               error);
    g_free (func_name);
    g_free (module_name);

    g_message ("UfoPluginManager: Created UfoART's %s-%p", name, (void*)projector);
    return UFO_PROJECTOR(projector);
}

/**
 * ufo_plugin_manager_get_regularizer:
 * @manager: A #UfoRegularizer
 * @name: Name of the regularizer.
 * @error: return location for a GError or %NULL
 *
 * Load a #UfoRegularizer module and return an instance. The shared object name must
 * be * constructed as "libufoartregularizer@name.so".
 *
 * Returns: (transfer full): (allow-none): #UfoRegularizer or %NULL if module cannot be found
 */
UfoRegularizer *
ufo_plugin_manager_get_regularizer(UfoPluginManager   *manager,
                                   const gchar        *name,
                                   GError            **error)
{
    g_return_val_if_fail (UFO_IS_PLUGIN_MANAGER (manager) && name != NULL, NULL);
    UfoRegularizer *regularizer;

    gchar *module_name = transform_string ("libufoartregularizer%s.so", name, NULL);
    gchar *func_name = transform_string ("ufo_%s_new", name, "_");
    

    regularizer = ufo_plugin_manager_get_plugin (manager,
                                                 func_name,
                                                 module_name,
                                                 error);
    g_free (func_name);
    g_free (module_name);

    g_message ("UfoPluginManager: Created UfoART's %s-%p", name, (void*)regularizer);
    return UFO_REGULARIZER(regularizer);
}
