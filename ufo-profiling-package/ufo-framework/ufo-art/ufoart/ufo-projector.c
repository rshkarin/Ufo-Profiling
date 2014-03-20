#include <ufoart/ufo-projector.h>
#include <ufoart/ufo-utils.h>

G_DEFINE_TYPE (UfoProjector, ufo_projector, G_TYPE_OBJECT)
#define UFO_PROJECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UFO_TYPE_PROJECTOR, UfoProjectorPrivate))
gboolean projector_type_error (UfoProjector *self, GError **error);

GQuark
ufo_projector_error_quark (void)
{
    return g_quark_from_static_string ("ufo-projector-error-quark");
}

struct _UfoProjectorPrivate {
    UfoResources *resources;
    guint command_queue_id;
};

UfoProjector *
ufo_projector_new (void)
{ 
    UfoProjector *projector = g_object_new (UFO_TYPE_PROJECTOR, NULL);
    return projector;
}

static void
ufo_projector_init (UfoProjector *self)
{ 
    UfoProjectorPrivate *priv = NULL;
    self->priv = priv = UFO_PROJECTOR_GET_PRIVATE (self);
    priv->resources = NULL;
    priv->command_queue_id = 0;
}

static void
ufo_projector_dispose (GObject *object)
{
    UfoProjectorPrivate *priv = UFO_PROJECTOR_GET_PRIVATE (object);
  
    if (priv->resources != NULL) {
        g_object_unref (priv->resources);
        priv->resources = NULL;
    }
  
    G_OBJECT_CLASS (ufo_projector_parent_class)->finalize (object);
}

static void
ufo_projector_finalize (GObject *object)
{ 
    //UfoProjectorPrivate *priv = UFO_PROJECTOR_GET_PRIVATE (object);
    G_OBJECT_CLASS (ufo_projector_parent_class)->finalize (object);
    g_message ("UfoProjector: finalized");
}

static void
_ufo_projector_set_resources (UfoProjector *self,
                              UfoResources *resources)
{
    g_return_if_fail (UFO_IS_PROJECTOR(self) && UFO_IS_RESOURCES(resources));
    UfoProjectorPrivate *priv = UFO_PROJECTOR_GET_PRIVATE (self);
    priv->resources = g_object_ref (resources);
}

static void
_ufo_projector_map_command_queues (UfoProjector *self,
                                   guint command_queue_id)
{
    g_return_if_fail (UFO_IS_PROJECTOR(self)); 
    UfoProjectorPrivate *priv = UFO_PROJECTOR_GET_PRIVATE (self);
    priv->command_queue_id = command_queue_id;
}

static void
_ufo_projector_initialize (UfoProjector *self,
                           GError **error)
{ 
    g_error ("Method initialize is not implemented.");
}

static void
_ufo_projector_compute_rays_weights (UfoProjector *projector,
                                     const UfoGeometry *geometry,
                                     UfoBuffer *angles,
                                     UfoBuffer *ray_weights)
{ 
    g_error ("Method compute_weights is not implemented.");
}

static void
_ufo_projector_compute_pixel_weights (UfoProjector *projector,
                                      const UfoGeometry *geometry,
                                      UfoBuffer *angles,
                                      const guint offset,
                                      const guint n,
                                      UfoBuffer *pixel_weights)
{ 
    g_error ("Method compute_pixel_weights is not implemented.");
}

static void
_ufo_projector_FP (UfoProjector *projector,
                   const UfoGeometry *geometry,
                   UfoBuffer *angles,
                   const guint offset,
                   const guint n,
                   UfoBuffer *volume,
                   UfoBuffer *projection,
                   const float output_scale)
{ 
    g_error ("Method FP is not implemented.");
}

static void
_ufo_projector_BP (UfoProjector *projector,
                  const UfoGeometry *geometry,
                  UfoBuffer *angles,
                  const guint offset,
                  const guint n,
                  UfoBuffer *projection,
                  UfoBuffer *volume)
{ 
    g_error ("Method BP is not implemented.");
}

gboolean
projector_type_error (UfoProjector *self,
                      GError **error)
{
    if(!UFO_IS_PROJECTOR(self)) {
        g_return_val_if_fail (*error == NULL, TRUE);
        g_set_error (error,
                     UFO_PROJECTOR_ERROR,
                     UFO_PROJECTOR_ERROR_INITIALIZATION,
                     "The passed object is not Projector.");
        return TRUE;
    }
    return FALSE;
}

static gboolean
_ufo_projector_initialized (UfoProjector *self,
                            GError **error)
{
    if (!projector_type_error (self, error)) { 
        UfoProjectorPrivate *priv = UFO_PROJECTOR_GET_PRIVATE (self);
        gboolean ok = (priv->resources != NULL);
        if (!ok) {
            g_set_error (error,
                         UFO_PROJECTOR_ERROR,
                         UFO_PROJECTOR_ERROR_INITIALIZATION,
                         "Projector is not inititalized: the UfoResources did not set and/or OpenCL.");
        }
        return ok;
    } else {
        return FALSE;
    }
}

static UfoResources * 
prot_ufo_projector_get_resources (UfoProjector *self)
{
     g_return_val_if_fail (UFO_IS_PROJECTOR (self), NULL);
     UfoProjectorPrivate *priv = UFO_PROJECTOR_GET_PRIVATE (self);
     return priv->resources; 
}

static cl_command_queue
prot_ufo_projector_get_command_queue (UfoProjector *self)
{
     g_return_val_if_fail (UFO_IS_PROJECTOR (self), NULL);
     UfoProjectorPrivate *priv = UFO_PROJECTOR_GET_PRIVATE (self);
     GList *command_queues = ufo_resources_get_cmd_queues (priv->resources);
     return g_list_nth_data(command_queues, priv->command_queue_id); 
}

static void
ufo_projector_class_init (UfoProjectorClass *klass)
{ 
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ufo_projector_finalize;
    gobject_class->dispose = ufo_projector_dispose;
    g_type_class_add_private (gobject_class, sizeof(UfoProjectorPrivate));

    klass->set_resources = _ufo_projector_set_resources;
    klass->map_command_queues = _ufo_projector_map_command_queues;
    klass->initialized = _ufo_projector_initialized;
    klass->initialize = _ufo_projector_initialize;

    klass->compute_rays_weights = _ufo_projector_compute_rays_weights;
    klass->compute_pixel_weights = _ufo_projector_compute_pixel_weights;
    klass->FP = _ufo_projector_FP;
    klass->BP = _ufo_projector_BP;

    klass->prot_get_resources = prot_ufo_projector_get_resources;
    klass->prot_get_command_queue = prot_ufo_projector_get_command_queue;
}

/**
 * ufo_projector_set_resources:
 * @projector: (in): An #UfoProjector child class instance.
 * @resources: (in): An #UfoProjector.
 *
 * Set the #UfoResources.
 */
void
ufo_projector_set_resources (UfoProjector *projector,
                             UfoResources *resources)
{
    g_return_if_fail (UFO_IS_PROJECTOR(projector) && UFO_IS_RESOURCES(resources)); 
    UfoProjectorClass *parent_class_part = UFO_PROJECTOR_GET_CLASS (projector);
    parent_class_part->set_resources(projector, resources);
}

/**
 * ufo_projector_map_command_queues:
 * @projector: (in): An #UfoProjector child class instance.
 * @command_queue_id: (in): An index of command queue that will be used for
 * whole processing.
 *
 * Set the index of command queue to @projector for the further selection
 * the related OpenCL command queue from the #UfoResources.
 */
void
ufo_projector_map_command_queues (UfoProjector *projector,
                                  guint command_queue_id)
{
    g_return_if_fail (UFO_IS_PROJECTOR(projector)); 
    UfoProjectorClass *parent_class_part = UFO_PROJECTOR_GET_CLASS (projector);
    parent_class_part->map_command_queues(projector, command_queue_id);
}

/**
 * ufo_projector_initialize:
 * @projector: (in): An #UfoProjector child class instance.
 * @error: return location for a GError or %NULL.
 *
 * Inititalize @projector.
 */
void
ufo_projector_initialize (UfoProjector *projector,
                          GError **error)
{
    if (!projector_type_error (projector, error)) {
        UfoProjectorClass *parent_class_part = UFO_PROJECTOR_GET_CLASS (projector);
        parent_class_part->initialize(projector, error);
    }
}

/**
 * ufo_projector_compute_rays_weights:
 * @projector: (in): An #UfoProjector child class instance.
 * @geometry: (in): @geometry: (in): An #UfoGeometry instance.
 * @angles: (in): An #UfoBuffer that stores angles in radians.
 * @ray_weights: (out): An #UfoBuffer where the weights will be written, it should be allocated as sinogram.
 *
 * Compute the weights of rays using @projector.
 */
void
ufo_projector_compute_rays_weights (UfoProjector *projector,
                                    const UfoGeometry *geometry,
                                    UfoBuffer *angles,
                                    UfoBuffer *ray_weights)
{
    g_return_if_fail (UFO_IS_PROJECTOR(projector) && 
                      geometry &&
                      UFO_IS_BUFFER(angles) &&
                      UFO_IS_BUFFER(ray_weights));
    UfoProjectorClass *parent_class_part = UFO_PROJECTOR_GET_CLASS (projector);
    parent_class_part->compute_rays_weights(projector, geometry, angles, ray_weights);
}

/**
 * ufo_projector_compute_pixel_weights:
 * @projector: (in): An #UfoProjector child class instance.
 * @geometry: (in): @geometry: (in): An #UfoGeometry instance.
 * @angles: (in): An #UfoBuffer that stores angles in radians.
 * @offset: (in): An index of angle in @angles.
 * @n: (in): Number of angles to be processed from @offset.
 * @pixel_weights: (out): An #UfoBuffer where the weights will be written, it should be allocated as volume.
 *
 * Compute the weights of pixel using @projector.
 */
void
ufo_projector_compute_pixel_weights (UfoProjector *projector,
                                     const UfoGeometry *geometry,
                                     UfoBuffer *angles,
                                     guint offset,
                                     guint n,
                                     UfoBuffer *pixel_weights)
{
    g_return_if_fail (UFO_IS_PROJECTOR(projector) &&
                      geometry &&
                      UFO_IS_BUFFER(angles) &&
                      UFO_IS_BUFFER(pixel_weights));
    UfoProjectorClass *parent_class_part = UFO_PROJECTOR_GET_CLASS (projector);
    parent_class_part->compute_pixel_weights(projector, geometry, angles, offset, n, pixel_weights);
}

/**
 * ufo_projector_FP:
 * @projector: (in): An #UfoProjector child class instance.
 * @geometry: (in): @geometry: (in): An #UfoGeometry instance.
 * @sin_cos_values: (in): An #UfoBuffer that stores precomputed sin and cos values.
 * @offset: (in): The first index of angles range to be processed.
 * @n: (in): Number of angles to be processed.
 * @volume: (in): An #UfoBuffer that represents a volume to be projected.
 * @projection: (out): An #UfoBuffer to which the result multiplied by @output_scale will be added.
 * @output_scale: (in): w.
 *
 * Perform forward projection using @projector.
 */
void
ufo_projector_FP (UfoProjector *self,
                  const UfoGeometry *geometry,
                  UfoBuffer *sin_cos_values,
                  guint offset,
                  guint n,
                  UfoBuffer *volume,
                  UfoBuffer *projection,
                  float output_scale)
{
    g_return_if_fail (UFO_IS_PROJECTOR(self) &&
                      geometry &&
                      UFO_IS_BUFFER(sin_cos_values) &&
                      UFO_IS_BUFFER(volume) &&
                      UFO_IS_BUFFER(projection) &&
                      n > 0);
    UfoProjectorClass *parent_class_part = UFO_PROJECTOR_GET_CLASS (self);
    parent_class_part->FP(self, geometry, sin_cos_values, offset, n, volume, projection, output_scale);
}

/**
 * ufo_projector_BP:
 * @projector: (in): An #UfoProjector child class instance.
 * @geometry: (in): @geometry: (in): An #UfoGeometry instance.
 * @sin_cos_values: (in): An #UfoBuffer that stores precomputed sin and cos values.
 * @offset: (in): The first index of angles range to be processed.
 * @n: (in): Number of angles to be processed.
 * @projection: (in): An #UfoBuffer that represents a sinogram (projection) to be back-projected.
 * @volume: (out): An #UfoBuffer onto which the back-projection will be performed.
 *
 * Perform back-projection using @projector.
 */
void
ufo_projector_BP (UfoProjector *self,
                  const UfoGeometry *geometry,
                  UfoBuffer *sin_cos_values,
                  guint offset,
                  guint n,
                  UfoBuffer *projection,
                  UfoBuffer *volume)
{ 
    g_return_if_fail (UFO_IS_PROJECTOR(self) &&
                      geometry &&
                      UFO_IS_BUFFER(sin_cos_values) &&
                      UFO_IS_BUFFER(volume) &&
                      UFO_IS_BUFFER(projection) &&
                      n > 0);
    UfoProjectorClass *parent_class_part = UFO_PROJECTOR_GET_CLASS (self);
    parent_class_part->BP(self, geometry, sin_cos_values, offset, n, projection, volume);
}
