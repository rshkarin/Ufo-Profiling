#include <ufoart/ufo-regularizer.h>
#include <ufoart/ufo-utils.h>

G_DEFINE_TYPE (UfoRegularizer, ufo_regularizer, G_TYPE_OBJECT)
#define UFO_REGULARIZER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UFO_TYPE_REGULARIZER, UfoRegularizerPrivate))
static gboolean regularizer_type_error (UfoRegularizer *self, GError **error);

GQuark
ufo_regularizer_error_quark (void)
{
    return g_quark_from_static_string ("ufo-regularizer-error-quark");
}

struct _UfoRegularizerPrivate {
    UfoResources *resources;
    guint command_queue_id;
    guint max_iterations;
};

enum {
    PROP_0,
    PROP_MAX_ITERATIONS,
    N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

UfoRegularizer *
ufo_regularizer_new (void)
{
    UfoRegularizer *regularizer = g_object_new (UFO_TYPE_REGULARIZER, NULL);
    return regularizer;
}

static void
ufo_regularizer_init (UfoRegularizer *self)
{
    UfoRegularizerPrivate *priv = NULL;
    self->priv = priv = UFO_REGULARIZER_GET_PRIVATE (self);
    priv->resources = NULL;
    priv->command_queue_id = 0;
}

static void
ufo_regularizer_set_property (GObject *object,
                              guint property_id,
                              const GValue *value,
                              GParamSpec *pspec)
{
    UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (object);

    switch (property_id) {
      case PROP_MAX_ITERATIONS:
          priv->max_iterations = g_value_get_uint (value);
          break;
      default:
          G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
          break;
    }
}

static void
ufo_regularizer_get_property (GObject *object,
                              guint property_id,
                              GValue *value,
                              GParamSpec *pspec)
{
    UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (object);

    switch (property_id) {
      case PROP_MAX_ITERATIONS:
          g_value_set_uint (value, priv->max_iterations);
          break;
      default:
          G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
          break;
    }
}

static void
ufo_regularizer_dispose (GObject *object)
{
    UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (object);

    if (priv->resources != NULL) {
        g_object_unref (priv->resources);
        priv->resources = NULL;
    }

    G_OBJECT_CLASS(ufo_regularizer_parent_class)->dispose (object);
}

static void
ufo_regularizer_finalize (GObject *object)
{
    //UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (object);
    G_OBJECT_CLASS(ufo_regularizer_parent_class)->finalize (object);
    g_message ("UfoRegularizer: finalized");
}

static void
_ufo_regularizer_initialize (UfoRegularizer *self,
                             GError **error)
{
    /* Call overriden method. */
    UfoRegularizerClass *parent_class_part = UFO_REGULARIZER_GET_CLASS (self);
    parent_class_part->prot_initialized(self, error);
}

static void
_ufo_regularizer_map_command_queues (UfoRegularizer *self,
                                     guint command_queue_id)
{
    UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (self);
    priv->command_queue_id = command_queue_id;
}

static void 
_ufo_regularizer_set_resources (UfoRegularizer *self,
                                UfoResources *resources)
{
    UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (self);
    priv->resources = g_object_ref (resources);
}

static cl_event
_ufo_regularizer_process (UfoRegularizer *self,
                          UfoBuffer *problem,
                          NormaFunct f_norma)
{
    g_error ("The 'process' method should be reimplemented.");
    return NULL;
}

static cl_command_queue
prot_ufo_regularizer_get_command_queue (UfoRegularizer *self)
{
    g_return_val_if_fail (UFO_IS_REGULARIZER(self), NULL);
    UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (self);
    GList *command_queues = ufo_resources_get_cmd_queues (priv->resources);
    return g_list_nth_data (command_queues, priv->command_queue_id);
}

static UfoResources *
prot_ufo_regularizer_get_resources (UfoRegularizer *self)
{
    g_return_val_if_fail (UFO_IS_REGULARIZER(self), NULL);
    UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (self);
    return priv->resources;
}

gboolean
regularizer_type_error (UfoRegularizer *self,
                        GError **error)
{
    if(!UFO_IS_REGULARIZER(self)) {
        g_return_val_if_fail (*error == NULL, TRUE);
        g_set_error (error,
                     UFO_REGULARIZER_ERROR,
                     UFO_REGULARIZER_ERROR_INITIALIZATION,
                     "The passed object is not Regularizer.");
        return TRUE;
    }
    return FALSE;
}

static gboolean
prot_ufo_regularizer_initialized (UfoRegularizer *self,
                                  GError **error)
{
    g_return_val_if_fail (!regularizer_type_error (self, error), FALSE);
    UfoRegularizerPrivate *priv = UFO_REGULARIZER_GET_PRIVATE (self);
    gboolean ok = ( priv->resources != NULL);

    if (!ok) {
        g_set_error (error,
                     UFO_REGULARIZER_ERROR,
                     UFO_REGULARIZER_ERROR_INITIALIZATION,
                     "Regularizer is not inititalized: resources did not set.");
    }
    return ok;
}

static void
ufo_regularizer_class_init (UfoRegularizerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->set_property = ufo_regularizer_set_property;
    gobject_class->get_property = ufo_regularizer_get_property;
    gobject_class->finalize = ufo_regularizer_finalize;
    gobject_class->dispose = ufo_regularizer_dispose;

    properties[PROP_MAX_ITERATIONS] =
        g_param_spec_uint ("max-iterations",
              "Maximum number of iterations over all data",
              "Maximum number of iterations over all data",
              1, G_MAXUINT, 20,
              G_PARAM_READWRITE);

    guint i;
    for (i = PROP_0 + 1; i < N_PROPERTIES; i++)
        g_object_class_install_property (gobject_class, i, properties[i]);

    g_type_class_add_private (gobject_class, sizeof(UfoRegularizerPrivate));

    klass->initialize = _ufo_regularizer_initialize;
    klass->set_resources = _ufo_regularizer_set_resources;
    klass->map_command_queues = _ufo_regularizer_map_command_queues;
    klass->process = _ufo_regularizer_process;

    klass->prot_get_command_queue = prot_ufo_regularizer_get_command_queue;
    klass->prot_get_resources = prot_ufo_regularizer_get_resources;
    klass->prot_initialized = prot_ufo_regularizer_initialized;
}

/**
 * ufo_regularizer_initialize:
 * @regularizer: (in): An #UfoRegularizer child class instance.
 * @error: return location for a GError or %NULL.
 *
 * Inititalize @regularizer.
 */
void
ufo_regularizer_initialize (UfoRegularizer *regularizer,
                            GError **error)
{
    if (!regularizer_type_error (regularizer, error)) {
      //UfoRegularizerClass *klass = UFO_REGULARIZER_GET_CLASS (regularizer);
    }
}

/**
 * ufo_regularizer_set_resources:
 * @regularizer: (in): An #UfoRegularizer child class instance.
 * @resources: (in): An #UfoResources.
 *
 * Set the #UfoResources instance to @regularizer.
 */
void 
ufo_regularizer_set_resources (UfoRegularizer *regularizer,
                               UfoResources *resources)
{
    g_return_if_fail (UFO_IS_REGULARIZER(regularizer) && UFO_IS_RESOURCES(resources));
    UfoRegularizerClass *klass = UFO_REGULARIZER_GET_CLASS (regularizer);
    klass->set_resources (regularizer, resources);
}

/**
 * ufo_regularizer_map_command_queues:
 * @regularizer: (in): An #UfoRegularizer child class instance.
 * @command_queue_id: (in): An index of command queue that will be used for
 * whole processing.
 *
 * Set the index of command queue to @regularizer for the further selection
 * the related OpenCL command queue from the #UfoResources.
 */
void
ufo_regularizer_map_command_queues (UfoRegularizer *regularizer,
                                    guint command_queue_id)
{
    g_return_if_fail (UFO_IS_REGULARIZER(regularizer));
    UfoRegularizerClass *klass = UFO_REGULARIZER_GET_CLASS (regularizer);
    klass->map_command_queues (regularizer, command_queue_id);
}


/**
 * ufo_regularizer_process:
 * @problem: (in): An #UfoBuffer that will be processed.
 * @f_norma: (in): A pointer onto the function which will be used for norm computation.
 *
 * Perform regularization on @problem with using norm @f_norma.
 */
cl_event
ufo_regularizer_process (UfoRegularizer *self,
                         UfoBuffer *problem,
                         NormaFunct f_norma)
{
    g_return_val_if_fail (UFO_IS_REGULARIZER(self) &&
                          UFO_IS_BUFFER (problem) &&
                          f_norma,
                          NULL);
    UfoRegularizerClass *klass = UFO_REGULARIZER_GET_CLASS (self);
    return klass->process (self, problem, f_norma);
}
