#include "tinycthread.h"
#include "bmpfile.h"
#include "imgproc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef WIN32
#include <sys/time.h>
#endif

int pass_count = 1000;
int marker_level = 30;
int blur_level = 5;

uint64_t timestamp();
int thread_proc( void * arg );


int main( int argc, char * argv[] )
{
    int i;
    int arg;
    int thread_count = 1;
    BMP * bmp;
    IMG * img;
    thrd_t * threads;
    char * image_path = NULL;


    for ( arg = 1; arg < argc; arg++ )
    {
        if ( strcmp( argv[ arg ], "-h") == 0 || strcmp( argv[ arg ], "--help") == 0 )
        {
            printf( "usage: imagebm [ -n 1 ] -i image.bmp | <BMP DATA>\n"
                    "Options:\n\t-n thread count (default 1)\n\t-i input file\n\t-p pass count (default 1000)\n" );
            return 0;
        }

        if ( strcmp( argv[ arg ], "-i") == 0 && argc > arg + 1 )
        {
            image_path = argv[ arg + 1 ];
            arg++;
        }
        else if ( strcmp( argv[ arg ], "-p") == 0 && argc > arg + 1 )
        {
            pass_count = atoi( argv[ arg + 1 ] );
            if ( pass_count <= 0 )
            {
                pass_count = 1;
            }
            arg++;
        }
        else if ( strcmp( argv[ arg ], "-n") == 0 && argc > arg + 1 )
        {
            thread_count = atoi( argv[ arg + 1 ] );
            if ( thread_count <= 0 )
            {
                thread_count = 1;
            }
            arg++;
        }
    }

    bmp = bmpfile_read( image_path );
    BMP_CHECK_ERROR( stderr, 1 );

    img = imgproc_create( bmp );
    if ( img == NULL ) {
        perror( "imgproc_create" );
        return 2;
    }

    BMP_Free( bmp );
    threads = ( thrd_t * ) calloc( thread_count, sizeof( thrd_t ) );

    for ( i = 0; i < thread_count; i++ ) {
        arg = thrd_create( threads + i, thread_proc, img );
        if ( arg != thrd_success ) {
            perror( "thrd_create" );
            return 100 + arg;
        }
    }
    for ( i = 0; i < thread_count; i++ ) {
        thrd_join( threads[ i ], & arg );
        if ( arg != 0 ) {
            perror( "thread_proc" );
            return 200 + arg;
        }
    }

    imgproc_destroy( img );

    return 0;
}

uint64_t timestamp()
{
#ifdef WIN32
    LARGE_INTEGER performance_count;
    performance_count.QuadPart = 0;
    QueryPerformanceCounter( & performance_count );
    return performance_count.QuadPart;
#else
    struct timeval now;
    gettimeofday( & now, NULL );
    return now.tv_sec * ( uint64_t ) 1000000 + now.tv_usec;
#endif
}

int thread_proc( void * arg )
{
    int i;
    double center_x;
    double center_y;
    AREA * marker;
    IMG * img;
    uint8_t * blur_pixels;
    uint64_t now;
    uint64_t total_time = 0;

    for ( i = 0; i < pass_count; ++i )
    {
        marker = ( AREA * ) calloc( 1, sizeof( AREA ) );
        img = imgproc_clone( ( IMG * ) arg );
        blur_pixels = ( uint8_t * ) calloc( blur_level * blur_level, sizeof( uint8_t ) );
        now = timestamp();

        if ( imgproc_find_marker( img, marker_level, marker ) != 0 )
        {
            fprintf( stderr, "imgproc_find_marker( %d, ( %d, %d, %d, %d ) )",
                     marker_level, marker->x, marker->y, marker->w, marker->h );
            return 2;
        }

        total_time += timestamp() - now;
        now = timestamp();

        if ( imgproc_find_center( img, marker, blur_level, blur_pixels, & center_x, & center_y ) != 0 )
        {
            fprintf( stderr, "imgproc_find_center( %d, ( %lf, %lf ) )",
                     blur_level, center_x, center_y );
            return 3;
        }
        total_time += timestamp() - now;

        free( blur_pixels );
        imgproc_destroy( img );
        free( marker );
    }

    printf( "Marker: { x: %d, y: %d, w: %d, h: %d }\n", marker->x, marker->y, marker->w, marker->h );
    printf( "Center: { x: %lf, y: %lf }\n", center_x, center_y );
    printf( "Passes: %lu, Total time: %lu, Mean time: %lu\n", pass_count, total_time, total_time / pass_count );

    return 0;
}
