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

#ifndef __UFOART_ART_H
#define __UFOART_ART_H

#if !defined (__UFOART_H_INSIDE__) && !defined (UFOART_COMPILATION)
#error "Only <ufoart/ufoart.h> can be included directly."
#endif

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <glib.h>
#include <ufo/ufo.h> 

#include <ufoart/ufo-geometry.h>
#include <ufoart/ufo-projector.h>
#include <ufoart/ufo-regularizer.h>
#include <ufoart/ufo-utils.h>

G_BEGIN_DECLS

#define UFO_TYPE_ART                (ufo_art_get_type())
#define UFO_ART(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), UFO_TYPE_ART, UfoART)) 
#define UFO_IS_ART(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), UFO_TYPE_ART))
#define UFO_ART_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST((klass), UFO_TYPE_ART, UfoARTClass))
#define UFO_IS_ART_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE((klass), UFO_TYPE_ART))
#define UFO_ART_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), UFO_TYPE_ART, UfoARTClass))

GQuark ufo_art_error_quark (void);
#define UFO_ART_ERROR               ufo_art_error_quark()

typedef struct _UfoART         UfoART;
typedef struct _UfoARTClass    UfoARTClass;
typedef struct _UfoARTPrivate  UfoARTPrivate;

typedef enum {
    UFO_ART_ERROR_ITERATE,
    UFO_ART_ERROR_INPUT_DATA
} UfoArtError;

/**
 * UfoART:
 *
 * The base class of algebraic reconstruction algorithm.
 * The class resposnsible for setupping resources, geometry,
 * projector, etc. Also it provides an interface that must 
 * be reimplemented by each ART method.
 *
 * Please note that once the ART instance was allocated,
 * you should call #ufo_set_resources, #ufo_map_command_queues
 * and #ufo_set_projector, and only after this you have to call
 * #ufo_art_initialize.
 */
struct _UfoART {
  GObject parent_instance;
  UfoARTPrivate *priv;
};

/**
 * UfoARTClass:
 *
 * #UfoART class
 */
struct _UfoARTClass {
  GObjectClass parent_class;

  void (*set_resources)      (UfoART *art,
                              UfoResources *resources);

  void (*map_command_queues) (UfoART *art,
                              guint command_queue_id);

  void (*set_geometry)       (UfoART *art,
                              const UfoGeometry* geometry,
                              const gfloat *angles);
  
  void (*set_projector)      (UfoART *art,
                              UfoProjector *projector);

  void (*set_regularizer)    (UfoART *art,
                              UfoRegularizer *regularizer);
  
  void (*iterate)            (UfoART *art,
                              UfoBuffer *assumption,
                              UfoBuffer *sinogram,
                              guint n_iterations,
                              GError **error);

  UfoRegularizer *     (*prot_get_regularizer)   (UfoART *art);
  UfoProjector *       (*prot_get_projector)     (UfoART *art);
  UfoBuffer *          (*prot_get_sin_cos_values)(UfoART *art);
  cl_command_queue     (*prot_get_command_queue) (UfoART *art);
  UfoResources *       (*prot_get_resources)     (UfoART *art);
  const UfoGeometry *  (*prot_get_geometry)      (UfoART *art);
};

GType ufo_art_get_type (void);

void 
ufo_art_set_resources      (UfoART *art,
                            UfoResources *resources);
void
ufo_art_map_command_queues (UfoART *art,
                            guint command_queue_id);
void
ufo_art_set_geometry (UfoART *art,
                      const UfoGeometry* geometry,
                      const gfloat *angles);
void
ufo_art_set_projector (UfoART *art,
                       UfoProjector *projector);

void
ufo_art_set_regularizer (UfoART *art,
                         UfoRegularizer *regularizer);

void
ufo_art_iterate (UfoART *art,
                 UfoBuffer *assumption,
                 UfoBuffer *sinogram,
                 guint max_iterations,
                 GError **error);

G_END_DECLS
#endif
