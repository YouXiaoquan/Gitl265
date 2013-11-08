
#include "frame_input.h"


int x265_cli_pic_alloc( cli_pic_t *pic, int width, int height )
{
    pic->img.width  = width;
    pic->img.height = height;
    for( int i = 0; i < 1; i++ )
    {
         pic->img.plane[i] = x265_malloc( width * height * sizeof(pixel) );
         if( !pic->img.plane[i] )
         {
             return -1;
         }
         pic->img.stride[i] = width ;
    }
    for( int i = 1; i < 3; i++ )
    {
         pic->img.plane[i] = x265_malloc( width * height * sizeof(pixel) / 4 );
         if( !pic->img.plane[i] )
         {
             return -1;
         }
         pic->img.stride[i] = width / 2 ;
    }

    return 0;
}

void x265_cli_pic_clean( cli_pic_t *pic )
{
    for( int i = 0; i < 3; i++ )
    {
        x265_free( pic->img.plane[i] );
    }
    memset( pic, 0, sizeof(cli_pic_t) );
}

