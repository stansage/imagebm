#include <qdbmp/qdbmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <time.h>

#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

uint64_t timestamp();
int comparator(void* a, void* b);
char* read_stdin(void);
char* read_file(FILE* in);
void analyse_image(char* image);
int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}
size_t image_width = 640;
size_t image_height = 480;
size_t marker_width = 20;
size_t marker_height = 20;
int threshold_level = 150;
size_t marker_height = 20;

int main(int argc, char *argv[])
{
    int arg;
    int thread_count = 1;
    BMP* bmp;
    char * image_path;
//    uint8_t * image_data;
//    uint8_t * image_copy;


    for(arg = 1; arg < argc; arg++)
    {
        if(strcmp(argv[i], "-t") == 0 && argc > arg + 1)
        {
            thread_count = atoi(argv[arg + 1]);
            arg++;
        }
        else if(strcmp(argv[i], "-f") == 0 && argc > arg + 1)
        {
            image_path = argv[i + 1];
            arg++;
        }
    }

    if(image_path == NULL)
    {
        perror("usage: imagebm [-t 1] -f image.bmp");
        return 1;
//        image_data = read_stdin();
    }


    bmp = BMP_ReadFile(image_path);
    BMP_CHECK_ERROR( stdout, -1 );

        /* Get image's dimensions */
//        width = BMP_GetWidth( bmp );
//        height = BMP_GetHeight( bmp );

//    printf("Image path %s\n", image_path);
    printf("Thread count %d\n", thread_count);

//    image_copy = copy_image(image_data);
//    analyse_image(image_data);

    BMP_Free( bmp );
//    free(image_data);
//    free(image_copy);

    return 0;
}

uint64_t timestamp()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * (uint64_t)1000000 + now.tv_usec;
}

int comparator(void* a, void* b)
{
    return *(int*)a - *(int*)b;
}

char* read_stdin(void)
{
    assert("not implemented" == NULL);
    return NULL;
}


//char* read_file(FILE* in)
//{
//    size_t size;
//    size_t count;
//    char* result;

//    assert(in != NULL);
//    assert(feof(in) == 0);

//    fseek(in, 0L, SEEK_END);
//    size = ftell(in);
//    fseek(in, 0L, SEEK_SET);

//    result = (char*)malloc(size);
//    count = fread(buf, 1, size, in);

//    if(count != size)
//    {
//        fprintf(stderr, "%u != %u", count, size);
//    }

//    return result;
//}

char* copy_image(uint8_t* image)
{
    char * result = (char*)malloc(image_width * image_height);

    memcpy(result, image);

    return result;
}

void analyse_image(uint8_t* image)
{
    int r;
    int c;
    int p;
    int r;
    int n = 5;
    int rr;
    int cc;
    int32_t * pixel;
    uint8_t* pixel_values = (uint8_t*)calloc(n * n, sizeof(uint8_t));

    for(r = 0; r < image_height; r++)
    {
        for(c = 0; c < image_width; c++)
        {
            p = 0;
            for(rr = (r - (n / 2)); rr < (r - (n / 2) + n) ; rr++)
            {
                for(cc = (c - (n / 2)); cc < (c - (n / 2) + n); cc++)
                {
                    if((rr >= 0) && (rr < image_height) && (cc >= 0) && (cc < image_width))
                    {
                        pixel = (int32_t*)image;
                        pixel += rr * image_width;
                        pixel += cc;

                        if(*pixel < threshold_level)
                        {
                            pixel_values[p] = 0;
                        }
                        else
                        {
                            pixel_values[p] = *pixel;
                        }
                        p++;
                    }
                }
            }

            qsort(pixel_values, p, sizeof(uint8_t), comarator);

            pixel = (int32_t*)image;
            pixel += r * image_width;
            pixel += c;
            *pixel = pixel_values[p / 2];
        }
    }



}
