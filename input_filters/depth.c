#include "input_filters.h"

#define NAME "depth"
#define FAIL_IF_ERROR( cond, ... ) FAIL_IF_ERR( cond, NAME, __VA_ARGS__ )

cli_input_filter_t depth_input_filter;

typedef struct
{
    hnd_t prev_hnd;
    cli_input_filter_t prev_filter;
    cli_pic_t buffer;
    int32_t i_width ;
    int32_t i_height ;
    int32_t i_bit_depth_y ;
    int32_t i_bit_depth_c ;
    int32_t i_shift_y ;
    int32_t i_shift_c ;
    int32_t b_buffer_allocated ;

} depth_input_hnd_t;



static int init( hnd_t *handle, cli_input_filter_t *filter, video_info_t *info, x265_param_t *param )
{
	if ( (param->i_input_bit_depth_y == param->sps.i_bit_depth_y)
		&& (param->i_input_bit_depth_c == param->sps.i_bit_depth_c)
		)
	{
		return 0 ;
	}

    depth_input_hnd_t *h = calloc( 1, sizeof(depth_input_hnd_t) );
    if( !h )
    {
        return -1;
    }
    h->i_width = param->i_width ;
    h->i_height = param->i_height ;
    h->i_bit_depth_y = param->sps.i_bit_depth_y ;
    h->i_bit_depth_c = param->sps.i_bit_depth_c ;
    h->i_shift_y = param->sps.i_bit_depth_y - param->i_input_bit_depth_y ;
    h->i_shift_c = param->sps.i_bit_depth_c - param->i_input_bit_depth_c ;

    if( x265_cli_pic_alloc( &h->buffer, h->i_width, h->i_height ) )
    {
    	free (h) ;
    	return -1;
    }

    h->b_buffer_allocated = 1 ;
    h->prev_filter = *filter;
    h->prev_hnd = *handle;
    *handle = h;
    *filter = depth_input_filter;

    return 0;
}

static void scale_plane ( pixel *dst, pixel *src,
							int dst_stride, int src_stride,
							int i_width, int i_height,
							int i_shift, int i_min_value, int i_max_value
							)
{
	int x = 0, y = 0 ;
	pixel i_offset = 0 ;
	pixel i_value = 0 ;

	if ( i_shift > 0 )
	{
		for ( y = 0 ; y < i_height ; ++ y )
		{
			for ( x = 0 ; x < i_width ; ++ x )
			{
				dst[x] = (src[x] << i_shift) ;
			}
			dst += dst_stride ;
			src += src_stride ;
		}
	}
	else
	{
		i_shift = - i_shift ;
		i_offset = (1 << (i_shift - 1) ) ;
		for ( y = 0 ; y < i_height ; ++ y )
		{
			for ( x = 0 ; x < i_width ; ++ x )
			{
				i_value = ((src[x]+i_offset) >> i_shift) ;
				dst[x] = x265_clip3_pixel ( i_value, i_min_value, i_max_value ) ;
			}
			dst += dst_stride ;
			src += src_stride ;
		}
	}
}


static int get_frame( hnd_t handle, cli_pic_t *output, int frame )
{
    depth_input_hnd_t *h = handle;
    int i_min_value = 0 ;
    int i_max_value = 0 ;
    int loop = 0 ;

    if( h->prev_filter.get_frame( h->prev_hnd, output, frame ) )
    {
        return -1;
    }

    if ( h->i_shift_y != 0 )
    {
    	i_max_value = (1 << h->i_bit_depth_y) - 1 ;
    	for ( loop = 0 ; loop < 1 ; ++ loop )
    	{
    		scale_plane ( h->buffer.img.plane[loop], output->img.plane[loop],
    						h->buffer.img.stride[loop], output->img.stride[loop],
    						h->i_width, h->i_height,
    						h->i_shift_y, i_min_value, i_max_value ) ;
    	}
    }

    if ( h->i_shift_c != 0 )
    {
    	i_max_value = (1 << h->i_bit_depth_c) - 1 ;
    	for ( loop = 1 ; loop < 3 ; ++ loop )
    	{
    		scale_plane ( h->buffer.img.plane[loop], output->img.plane[loop],
    						h->buffer.img.stride[loop], output->img.stride[loop],
    						h->i_width/2, h->i_height/2,
    						h->i_shift_c, i_min_value, i_max_value ) ;
    	}
    }


    output->img = h->buffer.img ;
    return 0;
}

static int release_frame( hnd_t handle, cli_pic_t *pic )
{
    depth_input_hnd_t *h = handle;
    return h->prev_filter.release_frame( h->prev_hnd, pic );
}

static void free_filter( hnd_t handle )
{
    depth_input_hnd_t *h = handle;
    h->prev_filter.free( h->prev_hnd );
    if( h->b_buffer_allocated )
    {
        x265_cli_pic_clean( &h->buffer );
    }
    free( h );
}


cli_input_filter_t depth_input_filter = { NAME, NULL, init, get_frame, release_frame, free_filter, NULL };

