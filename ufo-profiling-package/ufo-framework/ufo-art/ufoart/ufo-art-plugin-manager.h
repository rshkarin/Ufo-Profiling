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

#ifndef __UFO_ART_PLUGIN_MANAGER_H
#define __UFO_ART_PLUGIN_MANAGER_H

#if !defined (__UFOART_H_INSIDE__) && !defined (UFOART_COMPILATION)
#error "Only <ufo/art/ufoart.h> can be included directly."
#endif

#include <glib.h>
#include <ufo/ufo.h>
#include <ufoart/ufo-art.h>

UfoART  * ufo_plugin_manager_get_art(UfoPluginManager   *manager,
                                     const gchar        *name,
                                     GError            **error);

UfoProjector  * ufo_plugin_manager_get_projector(UfoPluginManager   *manager,
	                                             const gchar        *name,
                                     			 GError            **error);

UfoRegularizer  * ufo_plugin_manager_get_regularizer(UfoPluginManager   *manager,
		                                             const gchar        *name,
		                                             GError            **error);
#endif
