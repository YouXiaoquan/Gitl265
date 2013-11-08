#include "output_filters.h"

/* This filter converts the demuxer API into the filtering API for video frames.
 * Backseeking is prohibited here as not all demuxers are capable of doing so. */

typedef struct
{
    hnd_t hin;
} source_output_hnd_t;

cli_output_filter_t source_output_filter;

static int init( hnd_t *handle, cli_output_filter_t *filter, video_info_t *info, x265_param_t *param )
{
    source_output_hnd_t *h = calloc( 1, sizeof(source_output_hnd_t) );
    if( !h )
    {
        return -1;
    }

    h->hin = *handle;
    *handle = h;
    *filter = source_output_filter;

    return 0;
}

static int write_frame( hnd_t handle, cli_pic_t *input )
{
	source_output_hnd_t *h = handle;
    /* do not allow requesting of frames from before the current position */
    cli_frame_output.write_frame( input, h->hin ) ;
    return 0;
}

static int release_frame( hnd_t handle, cli_pic_t *pic )
{
    return 0;
}

static void free_filter( hnd_t handle )
{
	source_output_hnd_t *h = handle;
    cli_frame_output.close_file( h->hin );
    free( h );
}

cli_output_filter_t source_output_filter = { "source", NULL, init, write_frame, release_frame, free_filter, NULL };
