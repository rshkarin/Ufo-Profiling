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

#include <ufoart/ufo-art.h>
#include <ufoart/ufo-utils.h>
#include <math.h>

G_DEFINE_TYPE (UfoART, ufo_art, G_TYPE_OBJECT)
#define UFO_ART_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UFO_TYPE_ART, UfoARTPrivate))

static gboolean ready_for_iterating (UfoART *self, GError **error);
static gboolean art_type_error (UfoART *self, GError **error);

GQuark
ufo_art_error_quark (void)
{
  return g_quark_from_static_string ("ufo-art-error-quark");
}

struct _UfoARTPrivate {
    UfoResources   *resources;
    UfoProjector   *projector;
    UfoRegularizer *regularizer;
    UfoBuffer      *sin_cos_values;
    gboolean        posc_enabled;

    UfoGeometry geometry;
    guint command_queue_id;
};

enum {
    PROP_0,
    PROP_POSC,
    N_PROPERTIES
};
static GParamSpec *properties[N_PROPERTIES] = { NULL, };

static void
ufo_art_init (UfoART *self)
{
    UfoARTPrivate *priv = NULL;
    self->priv = priv = UFO_ART_GET_PRIVATE (self);
    priv->resources = NULL;
    priv->projector = NULL;
    priv->regularizer = NULL;

    priv->sin_cos_values = NULL;

    ufo_geometry_reset(&priv->geometry);
    priv->command_queue_id = 0;
}

static void
ufo_art_set_property (GObject *object,
                      guint property_id,
                      const GValue *value,
                      GParamSpec *pspec)
{
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (object);

    switch (property_id) {
        case PROP_POSC:
            priv->posc_enabled = g_value_get_boolean (value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
ufo_art_get_property (GObject *object,
                      guint property_id,
                      GValue *value,
                      GParamSpec *pspec)
{
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (object);

    switch (property_id) {
        case PROP_POSC:
            g_value_set_boolean (value, priv->posc_enabled);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
ufo_art_dispose (GObject *object)
{
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (object);

    if (priv->resources != NULL) {
        g_object_unref (priv->resources);
        priv->resources = NULL;
    }

    if (priv->projector != NULL) {
        g_object_unref (priv->projector);
        priv->projector = NULL;
    }

    if (priv->regularizer != NULL) {
        g_object_unref (priv->regularizer);
        priv->regularizer = NULL;
    }

    if (priv->sin_cos_values != NULL) {
        g_object_unref (priv->sin_cos_values);
        priv->sin_cos_values = NULL;
    }

    G_OBJECT_CLASS(ufo_art_parent_class)->dispose (object);
}

static void
ufo_art_finalize (GObject *object)
{
    //UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (object);
    G_OBJECT_CLASS(ufo_art_parent_class)->finalize (object);
    g_message ("UfoART: finalized");
}

static void
_ufo_art_map_command_queues (UfoART *self,
                             guint command_queue_id)
{
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    priv->command_queue_id = command_queue_id;
}

static void 
_ufo_art_set_resources (UfoART *self,
                        UfoResources *resources)
{
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    priv->resources = g_object_ref (resources);
}

static void 
_ufo_art_set_geometry (UfoART *self,
                       const UfoGeometry *geometry,
                       const gfloat *angles)
{
    UfoARTClass   *klass = UFO_ART_GET_CLASS (self);
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);

    if (priv->resources == NULL) {
        g_error ("Resources must be set before geometry.");
    }

    priv->geometry = *geometry;

    UfoRequisition requisition;
    requisition.n_dims = 2; // 0 - sin , 1 - cos
    requisition.dims[0] = priv->geometry.proj_angles;
    requisition.dims[1] = 2;

    cl_context context = ufo_resources_get_context (priv->resources);
    cl_command_queue command_queue = klass->prot_get_command_queue (self);

    if (!priv->sin_cos_values) {
        priv->sin_cos_values = ufo_buffer_new (&requisition, context);
    } else {
        ufo_buffer_resize(priv->sin_cos_values, &requisition);
    }

    // precompute sin and cos values
    gfloat *h_sin_cos_values = ufo_buffer_get_host_array (priv->sin_cos_values, command_queue);
    guint i = 0, n = geometry->proj_angles;
    for (i = 0; i < n; ++i) {
        h_sin_cos_values[i] = sinf(angles[i]);
        h_sin_cos_values[i+n] = cosf(angles[i]);
    }
}

static void
_ufo_art_set_projector (UfoART *self,
                        UfoProjector *projector)
{
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    if (priv->projector != NULL) {
        g_object_unref (priv->projector);
    }
    priv->projector = g_object_ref (projector);
}

static void
_ufo_art_set_regularizer (UfoART *self,
                          UfoRegularizer *regularizer)
{
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    if (priv->regularizer != NULL) {
        g_object_unref (priv->regularizer);
    }
    priv->regularizer = g_object_ref (regularizer);
}
  
static void 
_ufo_art_iterate (UfoART *art,
                  UfoBuffer *assumption,
                  UfoBuffer *sinogram,
                  guint n_iterations,
                  GError **error)
{
    g_error ("The 'iterate' method should be reimplemented.");
}


static UfoRegularizer *
prot_ufo_art_get_regularizer (UfoART *self)
{
    g_return_val_if_fail (UFO_IS_ART(self), NULL);
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    return priv->regularizer;
}

static UfoProjector *
prot_ufo_art_get_projector (UfoART *self)
{
    g_return_val_if_fail (UFO_IS_ART(self), NULL);
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    return priv->projector;
}

static UfoBuffer *
prot_ufo_art_get_sin_cos_values (UfoART *self)
{
    g_return_val_if_fail (UFO_IS_ART(self), NULL);
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    return priv->sin_cos_values;
}

static cl_command_queue
prot_ufo_art_get_command_queue (UfoART *self)
{
    g_return_val_if_fail (UFO_IS_ART(self), NULL);
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    GList *command_queues = ufo_resources_get_cmd_queues (priv->resources);
    return g_list_nth_data (command_queues, priv->command_queue_id);
}

static UfoResources *
prot_ufo_art_get_resources (UfoART *self)
{
    g_return_val_if_fail (UFO_IS_ART(self), NULL);
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    return priv->resources;
}

static const UfoGeometry *
prot_ufo_art_get_geometry (UfoART *self)
{
    g_return_val_if_fail (UFO_IS_ART(self), NULL);
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    return &(priv->geometry);
}

static void
ufo_art_class_init (UfoARTClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->set_property = ufo_art_set_property;
    gobject_class->get_property = ufo_art_get_property;
    gobject_class->finalize = ufo_art_finalize;
    gobject_class->dispose = ufo_art_dispose;

    properties[PROP_POSC] =
        g_param_spec_boolean ("posc",
                              "Positive constraint",
                              "Positive constraint",
                              FALSE,
                              G_PARAM_READWRITE);

    guint i;
    for (i = PROP_0 + 1; i < N_PROPERTIES; i++)
        g_object_class_install_property (gobject_class, i, properties[i]);

    g_type_class_add_private (gobject_class, sizeof(UfoARTPrivate));

    klass->set_resources = _ufo_art_set_resources;
    klass->set_geometry = _ufo_art_set_geometry;
    klass->set_projector = _ufo_art_set_projector;
    klass->set_regularizer = _ufo_art_set_regularizer;
    klass->map_command_queues = _ufo_art_map_command_queues;
    klass->iterate = _ufo_art_iterate;

    klass->prot_get_regularizer = prot_ufo_art_get_regularizer;
    klass->prot_get_projector = prot_ufo_art_get_projector;
    klass->prot_get_sin_cos_values = prot_ufo_art_get_sin_cos_values;

    klass->prot_get_command_queue = prot_ufo_art_get_command_queue;
    klass->prot_get_resources = prot_ufo_art_get_resources;
    klass->prot_get_geometry = prot_ufo_art_get_geometry;
}

/**
 * ufo_art_map_command_queues:
 * @art: (in): An #UfoART child class instance.
 * @command_queue_id: (in): An index of command queue that will be used for
 * whole processing.
 *
 * Set the index of command queue to @art for the further selection
 * the related OpenCL command queue from the #UfoResources.
 */
void
ufo_art_map_command_queues (UfoART *art,
                            guint command_queue_id)
{
    g_return_if_fail (UFO_IS_ART(art));
    UfoARTClass *klass = UFO_ART_GET_CLASS (art);
    klass->map_command_queues (art, command_queue_id);
}

/**
 * ufo_art_set_resources:
 * @art: (in): An #UfoART child class instance.
 * @resources: (in): An #UfoResources.
 *
 * Set the #UfoResources instance to @art.
 */
void 
ufo_art_set_resources (UfoART *art,
                       UfoResources *resources)
{
    g_return_if_fail (UFO_IS_ART(art) && UFO_IS_RESOURCES(resources));
    UfoARTClass *klass = UFO_ART_GET_CLASS (art);
    klass->set_resources (art, resources);
}

/**
 * ufo_art_set_geometry:
 * @art: (in): An #UfoART child class instance.
 * @geometry: (in): An #UfoGeometry.
 * @angles: (in): An array of projections angles in radians.
 *
 * Set the @geometry and @angles to @art.
 */
void
ufo_art_set_geometry (UfoART *art,
                      const UfoGeometry* geometry,
                      const gfloat *angles)
{
    g_return_if_fail (UFO_IS_ART(art) && geometry && angles);
    UfoARTClass *klass = UFO_ART_GET_CLASS (art);
    klass->set_geometry (art, geometry, angles);
}

/**
 * ufo_art_set_projector:
 * @art: (in): An #UfoART child class instance.
 * @projector: (in): An #UfoProjector.
 *
 * Set the @projector to @art. This could be
 * called only after #ufo_art_set_resources.
 */
void
ufo_art_set_projector (UfoART *art,
                       UfoProjector *projector)
{
    g_return_if_fail (UFO_IS_ART(art) && UFO_IS_PROJECTOR(projector));
    UfoARTClass *klass = UFO_ART_GET_CLASS (art);
    klass->set_projector (art, projector);
}

/**
 * ufo_art_set_projector:
 * @art: (in): An #UfoART child class instance.
 * @regularizer: (in): An #UfoRegularizer.
 *
 * Set the @regularizer to @art. This could be
 * called only after #ufo_art_set_resources.
 */
void
ufo_art_set_regularizer (UfoART *art,
                         UfoRegularizer *regularizer)
{
    g_return_if_fail (UFO_IS_ART(art) && UFO_IS_REGULARIZER(regularizer));
    UfoARTClass *klass = UFO_ART_GET_CLASS (art);
    klass->set_regularizer (art, regularizer);
}

gboolean
art_type_error (UfoART *self,
                GError **error)
{
    if(!UFO_IS_ART(self)) {
        g_return_val_if_fail (*error == NULL, TRUE);
        g_set_error (error,
                     UFO_REGULARIZER_ERROR,
                     UFO_REGULARIZER_ERROR_INITIALIZATION,
                     "The passed object is not ART.");
        return TRUE;
    }
    return FALSE;
}

/**
 * ufo_art_initialize:
 * @art: (in): An #UfoART child class instance.
 * @initial_assumption: (in, out) An #UfoBuffer that allocated as volume and stores initial assumption.
 * @sinogram: (in) An #UfoBuffer that stores sinogram.
 * @max_iterations: (in): Max number of iterations over all data.
 * @error: (in): An #GError.
 */
void
ufo_art_iterate (UfoART *art,
                 UfoBuffer *assumption,
                 UfoBuffer *sinogram,
                 guint max_iterations,
                 GError **error)
{
    g_return_if_fail (!art_type_error (art, error) &&
                      UFO_IS_BUFFER (assumption) &&
                      UFO_IS_BUFFER (sinogram) &&
                      max_iterations);

    UfoARTClass *klass = UFO_ART_GET_CLASS (art);

    if (ready_for_iterating(art, error)) {
        klass->iterate (art, assumption, sinogram, max_iterations, error);
    }
}

gboolean
ready_for_iterating (UfoART *self,
                     GError **error)
{
    g_return_val_if_fail (!art_type_error (self, error), FALSE);
    UfoARTPrivate *priv = UFO_ART_GET_PRIVATE (self);
    
    if (!UFO_IS_RESOURCES(priv->resources)) {
        g_set_error (error,
                     UFO_ART_ERROR,
                     UFO_ART_ERROR_ITERATE,
                     "Algorithm is not ready: resources did not set.");
        return FALSE;
    }

    if (!ufo_geometry_valid(&priv->geometry)) {
        g_set_error (error,
                     UFO_ART_ERROR,
                     UFO_ART_ERROR_ITERATE,
                     "Algorithm is not ready: the geometry is invalid.");

        return FALSE;
    }

    if (!UFO_IS_PROJECTOR(priv->projector)) {
        g_set_error (error,
                     UFO_ART_ERROR,
                     UFO_ART_ERROR_ITERATE,
                     "Algorithm is not ready: the projector is not assigned.");
        return FALSE;
    } else {
        ufo_projector_map_command_queues (priv->projector, priv->command_queue_id);
    }

    if (UFO_IS_REGULARIZER(priv->regularizer)) {
        ufo_regularizer_map_command_queues (priv->regularizer, priv->command_queue_id);
    }

    return TRUE;
}
