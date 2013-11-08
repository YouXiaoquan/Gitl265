#include "input_filters.h"

#define NAME "resize"
#define FAIL_IF_ERROR( cond, ... ) FAIL_IF_ERR( cond, NAME, __VA_ARGS__ )

cli_input_filter_t resize_input_filter;

typedef struct
{
    hnd_t prev_hnd;
    cli_input_filter_t prev_filter;
    cli_pic_t buffer;
    int32_t i_width ;
    int32_t i_height ;
    int32_t i_pad[2] ;
    int32_t b_buffer_allocated ;

} resizer_input_hnd_t;



static int init( hnd_t *handle, cli_input_filter_t *filter, video_info_t *info, x265_param_t *param )
{
	if ( (0 == param->i_pad[0]) && (0 == param->i_pad[1]) )
	{
		return 0 ;
	}

    resizer_input_hnd_t *h = calloc( 1, sizeof(resizer_input_hnd_t) );
    if( !h )
    {
        return -1;
    }
    h->i_width = info->width ;
    h->i_height = info->height ;
    h->i_pad[0] = param->i_pad[0] ;
    h->i_pad[1] = param->i_pad[1] ;

    if( x265_cli_pic_alloc( &h->buffer,
    						h->i_width + h->i_pad[0],
    						h->i_height + h->i_pad[1] ) )
    {
    	free (h) ;
    	return -1;
    }

    h->b_buffer_allocated = 1 ;
    h->prev_filter = *filter;
    h->prev_hnd = *handle;
    *handle = h;
    *filter = resize_input_filter;

    return 0;
}

static void resize_type_1 ( pixel *dst, pixel *src,
							int dst_stride, int src_stride,
							int i_width, int i_height,
							int *i_pad
							)
{
	int x = 0, y = 0 ;
	int i_width_full = i_width + i_pad[0] ;
	int i_height_full = i_height + i_pad[1] ;
	int i_width_minux1 = i_width - 1 ;

	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			dst[x] = src[x] ;
		}

		for ( x = i_width ; x < i_width_full ; ++ x )
		{
			dst[x] = src[i_width_minux1] ;
		}

		dst += dst_stride ;
		src += src_stride ;
	}
	for ( y = i_height ; y < i_height_full ; ++ y )
	{
		for ( x = 0 ; x < i_width_full ; ++ x )
		{
			dst[x] = (dst-dst_stride)[x] ;
		}
		dst += dst_stride ;
	}
}

static void resize_type_2 ( pixel *dst, pixel *src,
							int dst_stride, int src_stride,
							int i_width, int i_height,
							int i_pad
							)
{
	int x = 0, y = 0 ;
	int i_width_full = i_width + i_pad ;
	int i_width_minux1 = i_width - 1 ;

	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			dst[x] = src[x] ;
		}

		for ( x = i_width ; x < i_width_full ; ++ x )
		{
			dst[x] = src[i_width_minux1] ;
		}

		dst += dst_stride ;
		src += src_stride ;
	}
}

static void resize_type_3 ( pixel *dst, pixel *src,
							int dst_stride, int src_stride,
							int i_width, int i_height,
							int i_pad
							)
{
	int x = 0, y = 0 ;
	int i_height_full = i_height + i_pad ;

	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			dst[x] = src[x] ;
		}

		dst += dst_stride ;
		src += src_stride ;
	}
	for ( y = i_height ; y < i_height_full ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			dst[x] = (dst-dst_stride)[x] ;
		}
		dst += dst_stride ;
	}
}

static int get_frame( hnd_t handle, cli_pic_t *output, int frame )
{
    resizer_input_hnd_t *h = handle;
    int i_pad[2] ;
    int loop = 0 ;

    if( h->prev_filter.get_frame( h->prev_hnd, output, frame ) )
    {
        return -1;
    }

    if ( h->i_pad[0] > 0 && h->i_pad[1] > 0 )
    {
    	i_pad[0] = h->i_pad[0] ;
    	i_pad[1] = h->i_pad[1] ;
    	for ( loop = 0 ; loop < 1 ; ++ loop )
    	{
    		resize_type_1 ( h->buffer.img.plane[loop], output->img.plane[loop],
    						h->buffer.img.stride[loop], output->img.stride[loop],
    						h->i_width, h->i_height, i_pad ) ;
    	}
    	i_pad[0] = i_pad[0] / 2 ;
    	i_pad[1] = i_pad[1] / 2 ;
    	for ( loop = 1 ; loop < 3 ; ++ loop )
    	{
    		resize_type_1 ( h->buffer.img.plane[loop], output->img.plane[loop],
    						h->buffer.img.stride[loop], output->img.stride[loop],
    						h->i_width/2, h->i_height/2, i_pad ) ;
    	}
    }
    else if ( h->i_pad[0] > 0 )
    {
    	for ( loop = 0 ; loop < 1 ; ++ loop )
    	{
    		resize_type_2 ( h->buffer.img.plane[loop], output->img.plane[loop],
    						h->buffer.img.stride[loop], output->img.stride[loop],
    						h->i_width, h->i_height, h->i_pad[0] ) ;
    	}
    	for ( loop = 1 ; loop < 3 ; ++ loop )
    	{
    		resize_type_2 ( h->buffer.img.plane[loop], output->img.plane[loop],
    						h->buffer.img.stride[loop], output->img.stride[loop],
    						h->i_width/2, h->i_height/2, h->i_pad[0]/2 ) ;
    	}
    }
    else if ( h->i_pad[1] > 0 )
    {
    	for ( loop = 0 ; loop < 1 ; ++ loop )
    	{
    		resize_type_3 ( h->buffer.img.plane[loop], output->img.plane[loop],
    						h->buffer.img.stride[loop], output->img.stride[loop],
    						h->i_width, h->i_height, h->i_pad[1] ) ;
    	}
    	for ( loop = 1 ; loop < 3 ; ++ loop )
    	{
    		resize_type_3 ( h->buffer.img.plane[loop], output->img.plane[loop],
    						h->buffer.img.stride[loop], output->img.stride[loop],
    						h->i_width/2, h->i_height/2, h->i_pad[1]/2 ) ;
    	}
    }


    output->img = h->buffer.img ;
    return 0;
}

static int release_frame( hnd_t handle, cli_pic_t *pic )
{
    resizer_input_hnd_t *h = handle;
    return h->prev_filter.release_frame( h->prev_hnd, pic );
}

static void free_filter( hnd_t handle )
{
    resizer_input_hnd_t *h = handle;
    h->prev_filter.free( h->prev_hnd );
    if( h->b_buffer_allocated )
    {
        x265_cli_pic_clean( &h->buffer );
    }
    free( h );
}


cli_input_filter_t resize_input_filter = { NAME, NULL, init, get_frame, release_frame, free_filter, NULL };

