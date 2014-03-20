#ifndef __UFOART_GEOMETRY_H
#define __UFOART_GEOMETRY_H

/**
 * SECTION:ufo-art-geometry
 * @Short_description: Represents geometry for ART methods.
 * @Title: UfoGeometry
 */

#if !defined (__UFOART_H_INSIDE__) && !defined (UFOART_COMPILATION)
#error "Only <ufoart/ufoart.h> can be included directly."
#endif

G_BEGIN_DECLS

typedef struct _UfoGeometry UfoGeometry;

/**
 * UfoGeometry:
 * @vol_width: Reconstruction volume width.
 * @vol_height: Reconstruction volume height.
 * @proj_angles: Number of angles in sinogram.
 * @proj_dets: Number of detectorsю
 * @det_scale: Size of a detector compared to volume pixels
 *
 * Used to specify a projection geometry.
 */
struct _UfoGeometry {
    unsigned int vol_width;
    unsigned int vol_height;
    unsigned int proj_angles;
    unsigned int proj_dets;
    float det_scale;
};

G_END_DECLS

#endif
