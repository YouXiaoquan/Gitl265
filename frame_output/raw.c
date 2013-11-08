


#include "frame_output.h"


typedef struct
{
    FILE *fh;
    int      next_frame ;
    uint8_t *output_buffers ;
    uint8_t *output_buffer[3] ;
    uint64_t plane_size[4];
    uint64_t frame_size;
	int32_t i_output_bit_depth_y;
	int32_t i_output_bit_depth_c;
	int32_t b_is_16_bit ;

} raw_hnd_t;

static int open_file( char *psz_filename, hnd_t *p_handle, cli_frame_output_opt_t *opt )
{
	int pixel_depth = 0 ;
    raw_hnd_t *h = calloc( 1, sizeof(raw_hnd_t) );
    if( !h )
    {
        return -1;
    }

#define FAIL_IF_ERROR( cond, ... ) FAIL_IF_ERR( cond, "raw", __VA_ARGS__ )
    FAIL_IF_ERROR( !opt->i_width || !opt->i_height, "raw output requires a resolution.\n" )

    h->i_output_bit_depth_y = opt->i_output_bit_depth_y;
    h->i_output_bit_depth_c = opt->i_output_bit_depth_c;
    FAIL_IF_ERROR( h->i_output_bit_depth_y < 8 || h->i_output_bit_depth_y > 16,
    				"unsupported luma bit depth `%d'\n", h->i_output_bit_depth_y );
    FAIL_IF_ERROR( h->i_output_bit_depth_c < 8 || h->i_output_bit_depth_c > 16,
    				"unsupported chroma bit depth `%d'\n", h->i_output_bit_depth_c );
#undef FAIL_IF_ERROR

    h->b_is_16_bit = h->i_output_bit_depth_y > 8 || h->i_output_bit_depth_c > 8 ;
    h->plane_size[0] = opt->i_width * opt->i_height ;
    h->plane_size[1] = h->plane_size[2] = h->plane_size[0] / 4 ;
    for( int i = 0; i < 3 ; i++ )
    {
        h->frame_size += h->plane_size[i];
    }

    pixel_depth = (h->b_is_16_bit) ? 2 : 1 ;
    h->output_buffers = calloc ( opt->i_width * opt->i_height * pixel_depth * 3 / 2,
    							sizeof (uint8_t) ) ;
    if ( 0 == h->output_buffer )
    {
    	free ( h ) ;
    	return -1 ;
    }
    h->output_buffer[0] = h->output_buffers ;
    h->output_buffer[1] = h->output_buffer[0] + h->plane_size[0] * pixel_depth ;
    h->output_buffer[2] = h->output_buffer[1] + h->plane_size[1] * pixel_depth ;

    if( !strcmp( psz_filename, "-" ) )
    {
        h->fh = stdin;
    }
    else
    {
        h->fh = fopen( psz_filename, "wb" );
    }
    if( h->fh == NULL )
    {
        return -1;
    }

    *p_handle = h;
    return 0;
}

static int write_frame_internal( cli_pic_t *pic, raw_hnd_t *h )
{
    int error = 0;
    int32_t width = 0 ;
    int32_t height = 0 ;
    int32_t stride = 0 ;
    int pixel_depth = h->b_is_16_bit ? 2 : 1;
    uint8_t *dst = NULL;
    pixel *src = NULL ;
    for( int i = 0; i < 3 && !error; i++ )
    {
        dst = h->output_buffer[i] ;
        src = pic->img.plane[i] ;
        if ( i )
        {
        	width = (pic->img.width>>1) ;
        	height = (pic->img.height>>1) ;
        }
        else
        {
        	width = pic->img.width ;
        	height = pic->img.height ;
        }
        stride = pic->img.stride[i] ;
        if( h->b_is_16_bit )
        {
            for( int32_t j = 0; j < height; j++ )
            {
            	for ( int32_t k = 0 ; k < width ; ++ k )
            	{
            		dst[2 * k] = ((uint8_t)(src[k] & 0xFF)) ;
            		dst[2 * k + 1] = ((uint8_t)((src[k] >> 8) & 0xFF)) ;
            	}
            	src += stride ;
            	dst += width * 2 ;
            }
        }
        else
        {
            for( int32_t j = 0; j < height; j++ )
            {
            	for ( int32_t k = 0 ; k < width ; ++ k )
            	{
                    dst[k] = (uint8_t)src[k] ;
            	}
            	src += stride ;
            	dst += width ;
            }
        }
        error |= fwrite( h->output_buffer[i], 1, h->plane_size[i] * pixel_depth, h->fh ) != h->plane_size[i];
    }

    return error;
}

static int write_frame( cli_pic_t *pic, hnd_t handle )
{
    raw_hnd_t *h = handle;

    if( write_frame_internal( pic, h ) )
    {
        return -1;
    }
    return 0;
}

static int close_file( hnd_t handle )
{
    raw_hnd_t *h = handle;
    if( !h || !h->fh )
    {
        return 0;
    }
    fclose( h->fh );
    free ( h->output_buffers ) ;
    free( h );
    return 0;
}


const cli_frame_output_t raw_frame_output = { open_file, x265_cli_pic_alloc, write_frame, NULL, x265_cli_pic_clean, close_file };


