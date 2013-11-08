#include "frame_input.h"

typedef struct
{
    FILE *fh;
    int      next_frame ;
    uint8_t *input_buffers ;
    uint8_t *input_buffer[3] ;
    uint64_t plane_size[4];
    uint64_t frame_size;
	int32_t i_input_bit_depth_y;
	int32_t i_input_bit_depth_c;
	int32_t b_is_16_bit ;

} raw_hnd_t;

static int open_file( char *psz_filename, hnd_t *p_handle, video_info_t *info, cli_frame_input_opt_t *opt )
{
	int pixel_depth = 0 ;
    raw_hnd_t *h = calloc( 1, sizeof(raw_hnd_t) );
    if( !h )
    {
        return -1;
    }

    if( !opt->resolution )
    {
        /* try to parse the file name */
        for( char *p = psz_filename; *p; p++ )
        {
            if( *p >= '0' && *p <= '9' && sscanf( p, "%dx%d", &info->width, &info->height ) == 2 )
            {
                break;
            }
        }
    }
    else
    {
        sscanf( opt->resolution, "%dx%d", &info->width, &info->height );
    }
#define FAIL_IF_ERROR( cond, ... ) FAIL_IF_ERR( cond, "raw", __VA_ARGS__ )
    FAIL_IF_ERROR( !info->width || !info->height, "raw input requires a resolution.\n" )

    h->i_input_bit_depth_y = opt->i_input_bit_depth_y;
    h->i_input_bit_depth_c = opt->i_input_bit_depth_c;
    FAIL_IF_ERROR( h->i_input_bit_depth_y < 8 || h->i_input_bit_depth_y > 16,
    				"unsupported luma bit depth `%d'\n", h->i_input_bit_depth_y );
    FAIL_IF_ERROR( h->i_input_bit_depth_c < 8 || h->i_input_bit_depth_c > 16,
    				"unsupported chroma bit depth `%d'\n", h->i_input_bit_depth_c );
#undef FAIL_IF_ERROR

    h->b_is_16_bit = h->i_input_bit_depth_y > 8 || h->i_input_bit_depth_c > 8 ;
    info->b_is_16_bit = h->b_is_16_bit ;
    h->plane_size[0] = info->width * info->height ;
    h->plane_size[1] = h->plane_size[2] = h->plane_size[0] / 4 ;
    for( int i = 0; i < 3 ; i++ )
    {
        h->frame_size += h->plane_size[i];
    }

    pixel_depth = (h->b_is_16_bit) ? 2 : 1 ;
    h->input_buffers = calloc ( info->width * info->height * pixel_depth * 3 / 2,
    							sizeof (uint8_t) ) ;
    if ( 0 == h->input_buffer )
    {
    	free ( h ) ;
    	return -1 ;
    }
    h->input_buffer[0] = h->input_buffers ;
    h->input_buffer[1] = h->input_buffer[0] + h->plane_size[0] * pixel_depth ;
    h->input_buffer[2] = h->input_buffer[1] + h->plane_size[1] * pixel_depth ;

    if( !strcmp( psz_filename, "-" ) )
    {
        h->fh = stdin;
    }
    else
    {
        h->fh = fopen( psz_filename, "rb" );
    }
    if( h->fh == NULL )
    {
        return -1;
    }


    if( x265_is_regular_file( h->fh ) )
    {
        fseek( h->fh, 0, SEEK_END );
        uint64_t size = ftell( h->fh );
        fseek( h->fh, 0, SEEK_SET );
        info->num_frames = size / h->frame_size;
    }

    *p_handle = h;
    return 0;
}

static int read_frame_internal( cli_pic_t *pic, raw_hnd_t *h )
{
    int error = 0;
    int pixel_depth = h->b_is_16_bit ? 2 : 1;
    pixel *dst = NULL;
    uint8_t *src = NULL ;
    uint64_t pixel_count = 0 ;
    for( int i = 0; i < 3 && !error; i++ )
    {
        error |= fread( h->input_buffer[i], 1, h->plane_size[i] * pixel_depth, h->fh ) != h->plane_size[i];
        dst = pic->img.plane[i];
        src = h->input_buffer[i] ;
        pixel_count = h->plane_size[i];
        if( h->b_is_16_bit )
        {
            for( uint64_t j = 0; j < pixel_count; j++ )
            {
                dst[j] = (src[2 * j + 1] << 8) | src [2 * j] ;
            }
        }
        else
        {
            for( uint64_t j = 0; j < pixel_count; j++ )
            {
                dst[j] = src[j] ;
            }
        }
    }
    return error;
}

static int read_frame( cli_pic_t *pic, hnd_t handle, int i_frame )
{
    raw_hnd_t *h = handle;

    if( i_frame > h->next_frame )
    {
        if( x265_is_regular_file( h->fh ) )
        {
            fseek( h->fh, i_frame * h->frame_size, SEEK_SET );
        }
        else
        {
            while( i_frame > h->next_frame )
            {
                if( read_frame_internal( pic, h ) )
                    return -1;
                h->next_frame++;
            }
        }
    }


    if( read_frame_internal( pic, h ) )
    {
        return -1;
    }

    h->next_frame = i_frame+1;
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
    free ( h->input_buffers ) ;
    free( h );
    return 0;
}


const cli_frame_input_t raw_frame_input = { open_file, x265_cli_pic_alloc, read_frame, NULL, x265_cli_pic_clean, close_file };


