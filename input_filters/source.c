#include "input_filters.h"

/* This filter converts the demuxer API into the filtering API for video frames.
 * Backseeking is prohibited here as not all demuxers are capable of doing so. */

typedef struct
{
    cli_pic_t pic;
    hnd_t hin;
    int cur_frame;
} source_input_hnd_t;

cli_input_filter_t source_input_filter;

static int init( hnd_t *handle, cli_input_filter_t *filter, video_info_t *info, x265_param_t *param )
{
    source_input_hnd_t *h = calloc( 1, sizeof(source_input_hnd_t) );
    if( !h )
    {
        return -1;
    }
    h->cur_frame = -1;

    if( cli_frame_input.picture_alloc( &h->pic, info->width, info->height ) )
    {
        return -1;
    }

    h->hin = *handle;
    *handle = h;
    *filter = source_input_filter;

    return 0;
}

static int get_frame( hnd_t handle, cli_pic_t *output, int frame )
{
	source_input_hnd_t *h = handle;
    /* do not allow requesting of frames from before the current position */
    if( frame <= h->cur_frame || cli_frame_input.read_frame( &h->pic, h->hin, frame ) )
    {
        return -1;
    }
    h->cur_frame = frame;
    *output = h->pic;
    return 0;
}

static int release_frame( hnd_t handle, cli_pic_t *pic )
{
	source_input_hnd_t *h = handle;
    if( cli_frame_input.release_frame && cli_frame_input.release_frame( &h->pic, h->hin ) )
    {
        return -1;
    }
    return 0;
}

static void free_filter( hnd_t handle )
{
	source_input_hnd_t *h = handle;
    cli_frame_input.picture_clean( &h->pic );
    cli_frame_input.close_file( h->hin );
    free( h );
}

cli_input_filter_t source_input_filter = { "source", NULL, init, get_frame, release_frame, free_filter, NULL };
