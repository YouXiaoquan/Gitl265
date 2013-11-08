
#ifndef X265_FRAME_INPUT_H
#define X265_FRAME_INPUT_H

#include "x265cli.h"

/* options that are used by only some demuxers */
typedef struct
{
	/*
    char *index_file;
    char *format;
    char *colorspace;
    int bit_depth;
    char *timebase;
    int seek;
    int progress;
    int output_csp;
    int output_range;
    int input_range;
    */


    char *resolution;
	int32_t i_input_bit_depth_y;
	int32_t i_input_bit_depth_c;

} cli_frame_input_opt_t;

/* properties of the source given by the demuxer */
typedef struct
{
	int32_t b_is_16_bit ;
    int width;
    int height;
    int num_frames;
} video_info_t;

/* image data type used by x265cli */
typedef struct
{
    int     width;     /* width of the picture */
    int     height;    /* height of the picture */
    pixel  *plane[3]; /* pointers for each plane */
    int     stride[3]; /* strides for each plane */
} cli_image_t;

typedef struct
{
	int64_t pts ;
    cli_image_t img;
} cli_pic_t;

typedef struct
{
    int (*open_file)( char *psz_filename, hnd_t *p_handle, video_info_t *info, cli_frame_input_opt_t *opt );
    int (*picture_alloc)( cli_pic_t *pic, int width, int height );
    int (*read_frame)( cli_pic_t *pic, hnd_t handle, int i_frame );
    int (*release_frame)( cli_pic_t *pic, hnd_t handle );
    void (*picture_clean)( cli_pic_t *pic );
    int (*close_file)( hnd_t handle );
} cli_frame_input_t;

extern const cli_frame_input_t raw_frame_input;

extern cli_frame_input_t cli_frame_input;

int      x265_cli_pic_alloc( cli_pic_t *pic, int width, int height );
void     x265_cli_pic_clean( cli_pic_t *pic );

#endif


