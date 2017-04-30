#include "bmpfile.h"
#include "qdbmp/qdbmp.c"
#include <stdlib.h>

static void close_file( FILE * file, const char * file_name )
{
    if ( file_name != NULL )
    {
        fclose( file );
    }
}

BMP * bmpfile_read( const char * file_name )
{
    BMP * bmp;
    FILE * file;

    /* Allocate */
    bmp = calloc( 1, sizeof( BMP ) );
    if ( bmp == NULL )
    {
        BMP_LAST_ERROR_CODE = BMP_OUT_OF_MEMORY;
        return NULL;
    }

    if ( file_name == NULL )
    {
//		BMP_LAST_ERROR_CODE = BMP_INVALID_ARGUMENT;
//		return NULL;
        file = stdin;
    } else {
        /* Open file */
        file = fopen( file_name, "rb" );
        if ( file == NULL )
        {
            BMP_LAST_ERROR_CODE = BMP_FILE_NOT_FOUND;
            free( bmp );
            return NULL;
        }
    }


    /* Read header */
    if ( ReadHeader( bmp, file ) != BMP_OK || bmp->Header.Magic != 0x4D42 )
    {
        BMP_LAST_ERROR_CODE = BMP_FILE_INVALID;
        close_file( file, file_name );
        free( bmp );
        return NULL;
    }


    /* Verify that the bitmap variant is supported */
    if ( ( bmp->Header.BitsPerPixel != 32 && bmp->Header.BitsPerPixel != 24 && bmp->Header.BitsPerPixel != 8 )
        || bmp->Header.CompressionType != 0 || bmp->Header.HeaderSize != 40 )
    {
        BMP_LAST_ERROR_CODE = BMP_FILE_NOT_SUPPORTED;
        close_file( file, file_name );
        free( bmp );
        return NULL;
    }


    /* Allocate and read palette */
    if ( bmp->Header.BitsPerPixel == 8 )
    {
        bmp->Palette = ( UCHAR * ) malloc( BMP_PALETTE_SIZE * sizeof( UCHAR ) );
        if ( bmp->Palette == NULL )
        {
            BMP_LAST_ERROR_CODE = BMP_OUT_OF_MEMORY;
            close_file( file, file_name );
            free( bmp );
            return NULL;
        }

        if ( fread( bmp->Palette, sizeof( UCHAR ), BMP_PALETTE_SIZE, file ) != BMP_PALETTE_SIZE )
        {
            BMP_LAST_ERROR_CODE = BMP_FILE_INVALID;
            close_file( file, file_name );
            free( bmp->Palette );
            free( bmp );
            return NULL;
        }
    }
    else	/* Not an indexed image */
    {
        bmp->Palette = NULL;
    }

    if ( bmp->Header.ImageDataSize == 0 && bmp->Header.CompressionType == 0 ) {
        bmp->Header.ImageDataSize = bmp->Header.FileSize - bmp->Header.DataOffset;
    }

    /* Allocate memory for image data */
    bmp->Data = ( UCHAR * ) malloc( bmp->Header.ImageDataSize );
    if ( bmp->Data == NULL )
    {
        BMP_LAST_ERROR_CODE = BMP_OUT_OF_MEMORY;
        close_file( file, file_name );
        free( bmp->Palette );
        free( bmp );
        return NULL;
    }


    /* Read image data */
    if ( fread( bmp->Data, sizeof( UCHAR ), bmp->Header.ImageDataSize, file ) != bmp->Header.ImageDataSize )
    {
        BMP_LAST_ERROR_CODE = BMP_FILE_INVALID;
        close_file( file, file_name );
        free( bmp->Data );
        free( bmp->Palette );
        free( bmp );
        return NULL;
    }

    close_file( file, file_name );

    BMP_LAST_ERROR_CODE = BMP_OK;

    return bmp;

}
