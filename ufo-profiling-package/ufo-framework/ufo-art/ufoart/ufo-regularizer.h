#ifndef __UFOART_REGULARIZER_H
#define __UFOART_REGULARIZER_H

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

G_BEGIN_DECLS

#define UFO_TYPE_REGULARIZER                (ufo_regularizer_get_type())
#define UFO_REGULARIZER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), UFO_TYPE_REGULARIZER, UfoRegularizer))
#define UFO_IS_REGULARIZER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), UFO_TYPE_REGULARIZER))
#define UFO_REGULARIZER_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST((klass), UFO_TYPE_REGULARIZER, UfoRegularizerClass))
#define UFO_IS_REGULARIZER_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE((klass), UFO_TYPE_REGULARIZER))
#define UFO_REGULARIZER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), UFO_TYPE_REGULARIZER, UfoRegularizerClass))

GQuark ufo_regularizer_error_quark (void);
#define UFO_REGULARIZER_ERROR               ufo_regularizer_error_quark()

typedef gfloat (*NormaFunct) (UfoBuffer *, UfoResources *, gpointer);

typedef struct _UfoRegularizer         UfoRegularizer;
typedef struct _UfoRegularizerClass    UfoRegularizerClass;
typedef struct _UfoRegularizerPrivate  UfoRegularizerPrivate;


typedef enum {
    UFO_REGULARIZER_ERROR_INITIALIZATION,
    UFO_REGULARIZER_ERROR_INPUT_DATA
} UfoRegularizerError;

struct _UfoRegularizer {
  GObject parent_instance;
  UfoRegularizerPrivate *priv;
};


struct _UfoRegularizerClass {
  GObjectClass parent_class;

  void (*initialize) (UfoRegularizer *regularizer,
                          GError **error);

  void (*set_resources) (UfoRegularizer *regularizer,
                         UfoResources *resources);

  void (*map_command_queues) (UfoRegularizer *regularizer,
                              guint command_queue_id);

  cl_event (*process) (UfoRegularizer * regularizer,
                       UfoBuffer *problem,
                       NormaFunct f_norma);

  /*
   * Protected */
  cl_command_queue     (*prot_get_command_queue) (UfoRegularizer *regularizer);
  UfoResources *       (*prot_get_resources)     (UfoRegularizer *regularizer);
  gboolean             (*prot_initialized)       (UfoRegularizer *regularizer,
                                                  GError **error);
};
UfoRegularizer *ufo_regularizer_new (void);

void
ufo_regularizer_initialize (UfoRegularizer *self,
                            GError **error);

void
ufo_regularizer_set_resources (UfoRegularizer *self,
                               UfoResources *resources);

void
ufo_regularizer_map_command_queues (UfoRegularizer *self,
                                    guint command_queue_id);

cl_event
ufo_regularizer_process (UfoRegularizer *self,
                         UfoBuffer *problem,
                         NormaFunct f_norma);

GType ufo_regularizer_get_type (void);
G_END_DECLS
#endif
