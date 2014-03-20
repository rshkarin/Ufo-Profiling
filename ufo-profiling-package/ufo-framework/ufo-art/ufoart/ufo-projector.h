#ifndef __UFOART_PROJECTOR_H
#define __UFOART_PROJECTOR_H

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

G_BEGIN_DECLS

#define UFO_TYPE_PROJECTOR            (ufo_projector_get_type())
#define UFO_PROJECTOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), UFO_TYPE_PROJECTOR, UfoProjector)) 
#define UFO_IS_PROJECTOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), UFO_TYPE_PROJECTOR))
#define UFO_PROJECTOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), UFO_TYPE_PROJECTOR, UfoProjectorClass))
#define UFO_IS_PROJECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), UFO_TYPE_PROJECTOR))
#define UFO_PROJECTOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), UFO_TYPE_PROJECTOR, UfoProjectorClass))

GQuark ufo_projector_error_quark (void);
#define UFO_PROJECTOR_ERROR          ufo_projector_error_quark()

typedef struct _UfoProjector         UfoProjector;
typedef struct _UfoProjectorClass    UfoProjectorClass;
typedef struct _UfoProjectorPrivate  UfoProjectorPrivate;

typedef enum {
    UFO_PROJECTOR_ERROR_INITIALIZATION
} UfoProjectorError;

/**
 * UfoProjector:
 *
 * The base class of projectors which are using in ART.
 * The class resposnsible for setupping resources and mapping
 * command queue. Also it provides an interface that must 
 * be reimplemented by each projector.
 *
 * Please note that once the ART instance was allocated,
 * you should call #ufo_set_resources, #ufo_map_command_queues
 * and #ufo_set_projector, and only after this you have to call
 * #ufo_art_initialize.
 */
struct _UfoProjector {
  GObject parent_instance;
  UfoProjectorPrivate *priv;
};

struct _UfoProjectorClass {
  GObjectClass parent_class;

  void (*set_resources) (UfoProjector *projector,
                         UfoResources *resources);

  void (*map_command_queues) (UfoProjector *projector,
                              guint command_queue_id);

  gboolean (*initialized) (UfoProjector *projector,
                           GError **error);
  
  /* Methods to override */
  void (*initialize) (UfoProjector *projector,
                      GError **error);
  
  void (*compute_rays_weights) (UfoProjector *projector,
                                    const UfoGeometry *geometry,
                                    UfoBuffer *angles,
                                    UfoBuffer *ray_weights);

  void (*compute_pixel_weights) (UfoProjector *projector,
                                 const UfoGeometry *geometry,
                                 UfoBuffer *angles,
                                 const guint offset,
                                 const guint n,
                                 UfoBuffer *pixel_weights);

  void (*FP) (UfoProjector *projector,
                  const UfoGeometry *geometry,
                  UfoBuffer *angles,
                  const guint offset,
                  const guint n,
                  UfoBuffer *volume,
                  UfoBuffer *projection,
                  const float output_scale);

  void (*BP) (UfoProjector *projector,
              const UfoGeometry *geometry,
              UfoBuffer *angles,
              const guint offset,
              const guint n,
              UfoBuffer *projection,
              UfoBuffer *volume);

  /*
   * Protected
   */
  UfoResources * (*prot_get_resources) (UfoProjector *projector);
  cl_command_queue (*prot_get_command_queue) (UfoProjector *projector);
};

UfoProjector *ufo_projector_new (void);

void
ufo_projector_set_resources (UfoProjector *projector,
                             UfoResources *resources);

void
ufo_projector_map_command_queues (UfoProjector *projector,
                                  guint command_queue_id);

void
ufo_projector_initialize (UfoProjector *projector,
                          GError **error);

void
ufo_projector_compute_rays_weights (UfoProjector *projector,
                                    const UfoGeometry *geometry,
                                    UfoBuffer *sin_cos_values,
                                    UfoBuffer *ray_weights);

void
ufo_projector_compute_pixel_weights (UfoProjector *projector,
                                     const UfoGeometry *geometry,
                                     UfoBuffer *sin_cos_values,
                                     guint offset,
                                     guint n,
                                     UfoBuffer *pixel_weights);

void
ufo_projector_FP (UfoProjector *projector,
                  const UfoGeometry *geometry,
                  UfoBuffer *sin_cos_values,
                  guint offset,
                  guint n,
                  UfoBuffer *volume,
                  UfoBuffer *projection,
                  float output_scale);

void
ufo_projector_BP (UfoProjector *projector,
                  const UfoGeometry *geometry,
                  UfoBuffer *sin_cos_values,
                  guint offset,
                  guint n,
                  UfoBuffer *projection,
                  UfoBuffer *volume);

GType ufo_projector_get_type (void);
G_END_DECLS
#endif
