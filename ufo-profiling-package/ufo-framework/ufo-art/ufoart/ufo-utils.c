#include <string.h>
#include <ufoart/ufo-utils.h>

/**
 * ufo_allocate_volume:
 * @geometry: (in): An #UfoGeometry object
 * @context: (in): An pointer on cl_context
 *
 * Allocate memory to store a slice, according with geometry.
 *
 * Returns: A new #UfoBuffer
 */
UfoBuffer *
ufo_allocate_volume (const UfoGeometry *geometry, 
                     gpointer context)
{
    g_return_val_if_fail (geometry && context, NULL);
    UfoRequisition requisition;
    requisition.n_dims = 2;
    requisition.dims[0] = geometry->vol_width;
    requisition.dims[1] = geometry->vol_height;

    UfoBuffer *buffer = ufo_buffer_new (&requisition, context);
    return buffer;
}

/**
 * ufo_allocate_sinogram:
 * @geometry: (in): An #UfoGeometry object
 * @context: (in): An pointer on cl_context
 *
 * Allocate memory to store a sinogram, according with geometry.
 *
 * Returns: A new #UfoBuffer
 */
UfoBuffer *
ufo_allocate_sinogram (const UfoGeometry *geometry,
                       gpointer context)
{
    g_return_val_if_fail (geometry && context, NULL);
    UfoRequisition requisition;
    requisition.n_dims = 2;
    requisition.dims[0] = geometry->proj_dets;
    requisition.dims[1] = geometry->proj_angles;

    UfoBuffer *buffer = ufo_buffer_new (&requisition, context);
    return buffer;
}

/**
 * ufo_fill_volume:
 * @volume: (in): An #UfoBuffer object storing the volume (slice)
 * @data: (in): The data which will be used to fill the volume
 * @command_queue: (in):  An pointer on cl_command_queue
 *
 * Fills the volume with respective values from the data array.
 */
void
ufo_fill_volume (UfoBuffer *volume,
                 const gfloat *data,
                 gpointer command_queue)
{
    g_return_if_fail (UFO_IS_BUFFER(volume) && data);
    gfloat *h_buffer = ufo_buffer_get_host_array (volume, command_queue);
    g_memmove (h_buffer, data, ufo_buffer_get_size (volume));
}

/**
 * ufo_fill_sinogram:
 * @sino: (in): An #UfoBuffer object storing the sinogram
 * @data: (in): The data which will be used to fill the sinogram
 * @command_queue: (in):  An pointer on cl_command_queue
 *
 * Fills the sinogram with respective values from the data array.
 */
void
ufo_fill_sinogram (UfoBuffer *sino,
                   const gfloat *data,
                   gpointer command_queue)
{
    g_return_if_fail (UFO_IS_BUFFER(sino) && data);
    gfloat *h_buffer = ufo_buffer_get_host_array (sino, command_queue);
    g_memmove (h_buffer, data, ufo_buffer_get_size (sino));
}

/**
 * ufo_allocate_volume:
 * @geometry: (in): An #UfoGeometry object
 * @context: (in): An pointer on cl_context
 *
 * Allocate memory to store a slice, according with geometry.
 *
 * Returns: A new #UfoBuffer
 */
UfoBuffer *
ufo_allocate_volume0 (const UfoGeometry *geometry,
                      gpointer context,
                      gpointer command_queue)
{
    g_return_val_if_fail (geometry && context && command_queue, NULL);
    UfoBuffer *buffer = ufo_allocate_volume (geometry, context);
    gfloat *zero_data = (gfloat *) g_malloc0 (ufo_buffer_get_size(buffer));
    ufo_fill_volume (buffer, zero_data, command_queue);
    return buffer;
}

/**
 * ufo_allocate_sinogram0:
 * @geometry: (in): An #UfoGeometry object
 * @context: (in): An pointer on cl_context
 *
 * Allocate memory to store a sinogram, according with geometry and
 * fills it with zeros.
 *
 * Returns: A new #UfoBuffer
 */
UfoBuffer *
ufo_allocate_sinogram0 (const UfoGeometry *geometry,
                        gpointer context,
                        gpointer command_queue)
{
    g_return_val_if_fail (geometry && context && command_queue, NULL);
    UfoBuffer *buffer = ufo_allocate_sinogram (geometry, context);
    gfloat *zero_data = (gfloat *) g_malloc0 (ufo_buffer_get_size(buffer));
    ufo_fill_sinogram (buffer, zero_data, command_queue);
    return buffer;
}

/**
 * ufo_copy_sinogram_to_sinogram:
 * @sino: (in): An #UfoBuffer object storing the source sinogram
 * @dst_sino: (in): An #UfoBuffer object storing the destination sinogram
 * @first_src_projection_index: (in): An index of projection (row) in source sinogram
 * @command_queue: (in):  An pointer on cl_command_queue
 *
 * Copy projections from the specified projection of source sinogram to the destination one,
 * the number of copied projections depends on size of destination sinogram.
 *
 * Returns: A cl_event object
 */
gpointer
ufo_copy_sinogram_to_sinogram (UfoBuffer *sino, 
                               UfoBuffer *dst_sino,
                               guint first_src_projection_index,
                               gpointer command_queue)
{
    g_return_val_if_fail (UFO_IS_BUFFER(sino) &&
                          UFO_IS_BUFFER(dst_sino) &&
                          command_queue, NULL);
    cl_event event;
    cl_mem src_data = ufo_buffer_get_device_image (sino, command_queue);
    cl_mem dst_data = ufo_buffer_get_device_image (dst_sino, command_queue);

    UfoRequisition src_requisition;
    ufo_buffer_get_requisition (sino, &src_requisition);

    UfoRequisition dst_requisition;
    ufo_buffer_get_requisition (dst_sino, &dst_requisition);

    size_t src_origin[3] = {0, first_src_projection_index, 0};
    size_t dst_origin[3] = {0, 0, 0};

    size_t region[3];
    region[0] = src_requisition.dims[0] > dst_requisition.dims[0] ? 
                dst_requisition.dims[0] : src_requisition.dims[0];

    size_t rest = src_requisition.dims[1] - first_src_projection_index;
    region[1] = rest < dst_requisition.dims[1] ? rest : dst_requisition.dims[1];
    region[2] = 1;

    UFO_CHECK_CLERR (clEnqueueCopyImage(command_queue,
                                        src_data,
                                        dst_data,
                                        src_origin,
                                        dst_origin,
                                        region,
                                        0, NULL, &event));

    return event;
}

/**
 * ufo_get_sin_value:
 * @sin_cos_values: (in): An array with precomputes sin or cos values
 * @n_angles: (in): Number of angles used
 * @angle_id: (in): An index of access
 *
 * The function to address the respective sin precomputed values.
 *
 * Returns: Value for sin.
 */
float
ufo_get_sin_value (const gfloat *sin_cos_values, guint n_angles, guint angle_id) {
    if (sin_cos_values == NULL) {
      g_error("The NULL array has been passed.");
    }
    if (angle_id >= n_angles) {
      g_error("Out of range.");
    }
    return sin_cos_values[angle_id];
}

/**
 * ufo_get_cos_value:
 * @sin_cos_values: (in): An array with precomputes sin or cos values
 * @n_angles: (in): Number of angles used
 * @angle_id: (in): An index of access
 *
 * The function to address the respective cos precomputed values.
 *
 * Returns: Value for cos.
 */
float
ufo_get_cos_value (const gfloat *sin_cos_values, 
                   guint n_angles, 
                   guint angle_id)
{
    if (sin_cos_values == NULL) {
      g_error("The NULL array has been passed.");
    }
    if (angle_id >= n_angles) {
      g_error("Out of range.");
    }
    return sin_cos_values[n_angles + angle_id];
}


/**
 * ufo_geometry_valid:
 * @geometry: (in): An #UfoGeometry object
 *
 * Check whether the geometry is valid.
 *
 * Returns: TRUE if geometry is valid, otherwise FALSE.
 */
gboolean
ufo_geometry_valid (const UfoGeometry *geometry)
{
    g_return_val_if_fail (geometry, FALSE);
    return (geometry->vol_width > 1 &&
            geometry->vol_height > 1 &&
            geometry->proj_angles >= 1 &&
            geometry->proj_dets >= 1 &&
            geometry->det_scale > 0.0f);
}

/**
 * ufo_geometry_valid:
 * @geometry: (in): An #UfoGeometry object
 *
 * Reset geometry.
 *
 */
void
ufo_geometry_reset (UfoGeometry *geometry)
{
    g_return_if_fail (geometry);
    geometry->vol_width = 0;
    geometry->vol_height = 0;
    geometry->proj_angles = 0;
    geometry->proj_dets = 0;
    geometry->det_scale = 1;
}

/**
 * ufo_print_profiling:
 * @events: (in): An array of cl_event objects
 * @n_events: (in): number of cl_event objects within array
 * @method: (in): Textual description for each cl_event
 *
 * Print duration for each cl_event in the array and total time.
 *
 */
void
ufo_print_profiling (gpointer *events,
                     guint n_events,
                     const char *method)
{
    #if PROFILING
    float result_time = 0;
    gsize start, end;
    clWaitForEvents (n_events, events);

    guint i;
    for (i = 0; i < n_events; ++i) {
       UFO_CHECK_CLERR (clGetEventProfilingInfo (events[i],
                                                 CL_PROFILING_COMMAND_START,
                                                 sizeof(cl_ulong),
                                                 &start,
                                                 NULL));
       UFO_CHECK_CLERR (clGetEventProfilingInfo (events[i],
                                                 CL_PROFILING_COMMAND_END,
                                                 sizeof(cl_ulong),
                                                 &end,
                                                 NULL));
       result_time += end - start;
    }
    result_time *= 1E-6;
    g_message ("METHOD: %s  taken %.2f ms.", method, result_time);
    #endif
}
