#ifndef __UFOART_UTILS_H
#define __UFOART_UTILS_H

#if !defined (__UFOART_H_INSIDE__) && !defined (UFOART_COMPILATION)
#error "Only <ufoart/ufoart.h> can be included directly."
#endif

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <ufo/ufo.h>
#include <ufoart/ufo-geometry.h>

#define UFO_CHECK_CLERR(err) UFO_RESOURCES_CHECK_CLERR (err)
#define PRINT_METHOD //g_print("call: %s\n", __func__);
#define PROFILING 0

UfoBuffer *
ufo_allocate_volume (const UfoGeometry *geometry, gpointer context);

UfoBuffer *
ufo_allocate_sinogram (const UfoGeometry *geometry, gpointer context);

UfoBuffer *
ufo_allocate_volume0 (const UfoGeometry *geometry, gpointer context, gpointer command_queue);

UfoBuffer *
ufo_allocate_sinogram0 (const UfoGeometry *geometry, gpointer context, gpointer command_queue);

void
ufo_fill_volume (UfoBuffer *volume, const gfloat *data, gpointer command_queue);

void
ufo_fill_sinogram (UfoBuffer *sino, const gfloat *data, gpointer command_queue);

gpointer
ufo_copy_sinogram_to_sinogram (UfoBuffer *sino, 
		                       UfoBuffer *dst_sino,
		                       guint first_src_projection_index,
		                       gpointer command_queue);



float
ufo_get_sin_value (const gfloat *sin_cos_values, guint n_angles, guint angle_id);

float
ufo_get_cos_value (const gfloat *sin_cos_values, guint n_angles, guint angle_id);

gboolean
ufo_geometry_valid (const UfoGeometry *geometry);

void
ufo_geometry_reset (UfoGeometry *geometry);

void
ufo_print_profiling (gpointer *events, guint n_events, const char *method);

#endif
