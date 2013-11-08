#include "output_filters.h"

#define NAME "resize"
#define FAIL_IF_ERROR( cond, ... ) FAIL_IF_ERR( cond, NAME, __VA_ARGS__ )

cli_output_filter_t resize_output_filter;

typedef struct
{
    hnd_t next_hnd;
    cli_output_filter_t next_filter;
    cli_pic_t buffer;
    int32_t i_width ;
    int32_t i_height ;
    int32_t i_conf_left ;
    int32_t i_conf_right ;
    int32_t i_conf_top ;
    int32_t i_conf_bottom ;
    int32_t b_buffer_allocated ;

} resizer_output_hnd_t;



static int init( hnd_t *handle, cli_output_filter_t *filter, video_info_t *info, x265_param_t *param )
{
	int i_conf_left = param->sps.i_conf_left + param->sps.vui.i_def_disp_win_left_offset ;
	int i_conf_right = param->sps.i_conf_right + param->sps.vui.i_def_disp_win_right_offset ;
	int i_conf_top = param->sps.i_conf_top + param->sps.vui.i_def_disp_win_top_offset ;
	int i_conf_bottom = param->sps.i_conf_bottom + param->sps.vui.i_def_disp_win_bottom_offset ;
	if ( (0 == i_conf_left) && (0 == i_conf_right)
		&& (0 == i_conf_top) && (0 == i_conf_bottom) )
	{
		return 0 ;
	}

    resizer_output_hnd_t *h = calloc( 1, sizeof(resizer_output_hnd_t) );
    if( !h )
    {
        return -1;
    }
    h->i_width = param->i_width - i_conf_left - i_conf_right ;
    h->i_height = param->i_height - i_conf_top - i_conf_bottom ;
    h->i_conf_left = i_conf_left ;
    h->i_conf_right = i_conf_right ;
    h->i_conf_top = i_conf_top ;
    h->i_conf_bottom = i_conf_bottom ;

    if( x265_cli_pic_alloc( &h->buffer, h->i_width, h->i_height ) )
    {
    	free (h) ;
    	return -1;
    }

    h->b_buffer_allocated = 1 ;
    h->next_filter = *filter;
    h->next_hnd = *handle;
    *handle = h;
    *filter = resize_output_filter;

    return 0;
}

static void resize ( pixel *dst, pixel *src,
					int dst_stride, int src_stride,
					int i_width, int i_height )
{
	int x = 0, y = 0 ;

	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			dst[x] = src[x] ;
		}
		dst += dst_stride ;
		src += src_stride ;
	}
}

static int write_frame( hnd_t handle, cli_pic_t *input )
{
    resizer_output_hnd_t *h = handle;
    int loop = 0 ;
    int i_left = 0, i_top = 0, i_plane_offset = 0 ;

    i_left = h->i_conf_left ;
    i_top = h->i_conf_top ;

	for ( loop = 0 ; loop < 1 ; ++ loop )
	{
		i_plane_offset = i_left + i_top * input->img.stride[loop] ;
		resize ( h->buffer.img.plane[loop], input->img.plane[loop] + i_plane_offset,
				h->buffer.img.stride[loop], input->img.stride[loop],
				h->i_width, h->i_height ) ;
	}

	i_left = i_left / 2 ;
	i_top = i_top / 2 ;
	for ( loop = 1 ; loop < 3 ; ++ loop )
	{
		i_plane_offset = i_left + i_top * input->img.stride[loop] ;
		resize ( h->buffer.img.plane[loop], input->img.plane[loop] + i_plane_offset,
				h->buffer.img.stride[loop], input->img.stride[loop],
				h->i_width/2, h->i_height/2 ) ;
	}


    if( h->next_filter.write_frame( h->next_hnd, &h->buffer ) )
    {
        return -1;
    }
    return 0;
}

static int release_frame( hnd_t handle, cli_pic_t *pic )
{
    resizer_output_hnd_t *h = handle;
    return h->next_filter.release_frame( h->next_hnd, pic );
}

static void free_filter( hnd_t handle )
{
    resizer_output_hnd_t *h = handle;
    h->next_filter.free( h->next_hnd );
    if( h->b_buffer_allocated )
    {
        x265_cli_pic_clean( &h->buffer );
    }
    free( h );
}


cli_output_filter_t resize_output_filter = { NAME, NULL, init, write_frame, release_frame, free_filter, NULL };

