#include "bmpfile.h"
#include "imgproc.h"
#include "tinycthread.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

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
        if ( strcmp( argv[ arg ], "-t") == 0 && argc > arg + 1 )
        {
            thread_count = atoi( argv[ arg + 1 ] );
            arg++;
        }
        else if ( strcmp( argv[ arg ], "-f") == 0 && argc > arg + 1 )
        {
            image_path = argv[ arg + 1 ];
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
    double center_x = 0;
    double center_y = 0;
    AREA * marker = ( AREA * ) calloc( 1, sizeof( AREA ) );
    IMG * img = imgproc_clone( ( IMG * ) arg );
    uint8_t * blur_pixels = ( uint8_t * ) calloc( blur_level * blur_level, sizeof( uint8_t ) );
    uint64_t now = timestamp();

    if ( img == NULL ) {
        return 1;
    }

    printf( "Finding marker..." );
    if ( imgproc_find_marker( img, marker_level, marker ) != 0 )
    {
        fprintf( stderr, "imgproc_find_marker( %d, ( %d, %d, %d, %d ) )",
                 marker_level, marker->x, marker->y, marker->w, marker->h );
        return 2;
    }
    printf( "Found marker for %llu microseconds\n", timestamp() - now );
    now = timestamp();

    printf( "Finding marker..." );
    if ( imgproc_find_center( img, marker, blur_level, blur_pixels, & center_x, & center_y ) != 0 )
    {
        fprintf( stderr, "imgproc_find_center( %d, ( %d, %d ) )",
                 blur_level, center_x, center_y );
        return 3;
    }
    printf( "Found center for %llu microseconds\n", timestamp() - now );
    now = timestamp();

    printf( "Marker: { x: %d, y: %d, w: %d, h: %d }\n", marker->x, marker->y, marker->w, marker->h );
    printf( "Center: { x: %lf, y: %lf }\n", center_x, center_y );

    imgproc_destroy( img );
    free( marker );

    return 0;
}
