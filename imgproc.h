#ifndef IMAGEBM_IMGPROC_H
#define IMAGEBM_IMGPROC_H

#include <inttypes.h>

typedef struct _BMP BMP;

typedef struct
{
//    uint8_t * red;
//    uint8_t * green;
//    uint8_t * blue;
    uint8_t * grey;
    unsigned long int width;
    unsigned long int height;
} IMG;

typedef struct
{
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
} AREA;

IMG * imgproc_create( BMP * bmp );
void imgproc_destroy( IMG * img );
IMG * imgproc_clone( const IMG *src );
int imgproc_find_marker( const IMG * img, int marker_level, AREA * marker_area );
int imgproc_find_center( const IMG * img, const AREA * area, int blur_level, uint8_t * blur_pixels, double * center_x, double * center_y );

#endif
