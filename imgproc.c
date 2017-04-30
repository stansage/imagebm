#include "imgproc.h"
#include "qdbmp/qdbmp.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

size_t image_size( IMG * img )
{
    return img->width * img->height;
}

IMG * image_create( UINT width, UINT height )
{
    IMG * img = calloc( 1, sizeof( IMG ) );

    if ( img != NULL )
    {
        img->width = width;
        img->height = height;

//        img->red = ( uint8_t * ) malloc( image_size( img ) );
//        img->green = ( uint8_t * ) malloc( image_size( img ) );
//        img->blue = ( uint8_t * ) malloc( image_size( img ) );
        img->grey = ( uint8_t * ) malloc( image_size( img ) );
    }

    return img;
}

int pixel_comparator( const void * a, const void * b )
{
    return * ( int * ) a - * ( int * ) b;
}

int marker_valid( RECT * marker )
{
    if ( marker->x == 0 || marker->y == 0  )
    {
        return 1;
    }
    return 0;
}


IMG * imgproc_create( BMP * bmp )
{
    UINT x;
    UINT y;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    UINT offset = 0;
    double grey;
    IMG * img = image_create( BMP_GetWidth( bmp ), BMP_GetHeight( bmp ) );

    if ( img != NULL ) {
        for ( y = 0; y < img->height; y++ )
        {
            for ( x = 0; x < img->width; x++ )
            {
//                BMP_GetPixelRGB( bmp, x, y, img->red + offset, img->green + offset, img->blue + offset );
                BMP_GetPixelRGB( bmp, x, y, & red, & green, & blue );
                if ( BMP_GetError() != BMP_OK )
                {
                    imgproc_destroy( img );
                    return NULL;
                }

//                grey = 0.21 * ( double ) * ( img->red + offset )
//                     + 0.72 * ( double ) * ( img->green + offset )
//                     + 0.07 * ( double ) * ( img->blue + offset );
                grey = 0.21 * red
                     + 0.72 * green
                     + 0.07 * blue;
                * ( img->grey + offset ) = grey;

                offset++;
            }
        }
    }

    return img;
}

void imgproc_destroy(  IMG * img  )
{
    assert( img != NULL );

//    free( img->red );
//    free( img->green );
//    free( img->blue );
    free( img->grey );

    memset( img, 0, sizeof( IMG ) );
}

IMG * imgproc_clone( const IMG * src )
{
    assert( src != NULL );

    IMG * img = image_create( src->width, src->height );

    if ( img != NULL ) {
//        memcpy( img->red, src->red, image_size( img ) );
//        memcpy( img->green, src->green, image_size( img ) );
//        memcpy( img->blue, src->blue, image_size( img ) );
        memcpy( img->grey, src->grey, image_size( img ) );
    }

    return img;
}


int imgproc_find_marker( const IMG * img, int marker_level, RECT * marker_area )
{
    int x;
    int y;
    int offset = 0;

    assert( marker_area != NULL );
    memset( marker_area, 0, sizeof( RECT ) );

    marker_area->x = img->width;
    marker_area->y = img->height;

    for ( y = 0; y < img->height; y++ )
    {
        for ( x = 0; x < img->width; x++ )
        {
            if ( * ( img->grey + offset ) >= marker_level )
            {
                if ( marker_area->x > x ) {
                    marker_area->x = x;
                }
                if ( marker_area->y > y ) {
                    marker_area->y = y;
                }
                if ( marker_area->w < x ) {
                    marker_area->w = x;
                }
                if ( marker_area->h < y ) {
                    marker_area->h = y;
                }
            }

            offset++;
        }
    }

    if ( marker_area->x == 0 ||
         marker_area->y == 0 ||
         marker_area->w == 0 ||
         marker_area->h == 0 )
    {
        return 1;
    }

    marker_area->w -= marker_area->x;
    marker_area->h -= marker_area->y;

    return 0;
}


int imgproc_find_center( const IMG * img, const RECT * area, int blur_level, uint8_t * blur_pixels, double * center_x, double * center_y )
{
    int i;
    int x;
    int y;
    int xx;
    int yy;
    uint64_t pixel_sum = 0;
    uint64_t pixel_sum_x = 0;
    uint64_t pixel_sum_y = 0;
    uint8_t * pixel;

    assert( img != NULL );
    assert( area != NULL );
    assert( blur_pixels != NULL );
    assert( center_x != NULL );
    assert( center_y != NULL );

    for ( x = area->x - blur_level; x < area->x + area->w + blur_level; x++ )
    {
        for ( y = area->y - blur_level; y < area->y + area->h + blur_level; y++ )
        {
            i = 0;
            for ( xx = x - blur_level / 2; xx < x - blur_level / 2 + blur_level; xx++ )
            {
                for ( yy = y - blur_level / 2; yy < y - blur_level / 2 + blur_level; yy++ )
                {
                    if ( ( xx >= 0 ) && ( xx < img->height ) && ( yy >= 0 ) && ( yy < img->width ) )
                    {
                        blur_pixels[ i ] =  * ( img->grey + img->width * yy + xx );
                        i++;
                    }
                }
            }

            qsort( blur_pixels, i, sizeof( uint8_t ), pixel_comparator );

            pixel = blur_pixels + i / 2;
            pixel_sum += * pixel;
            pixel_sum_x += * pixel * x;
            pixel_sum_y += * pixel * y;
        }
    }

    * center_x = 1.0 * pixel_sum_x / pixel_sum;
    * center_y = 1.0 * pixel_sum_y / pixel_sum;

    return 0;
}
