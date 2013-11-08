

#ifndef X265_FRAME_OUTPUT_H
#define X265_FRAME_OUTPUT_H


#include "x265cli.h"
#include "frame_input/frame_input.h"

/* options that are used by only some demuxers */
typedef struct
{
	int32_t i_width ;
	int32_t i_height ;
	int32_t i_output_bit_depth_y;
	int32_t i_output_bit_depth_c;

} cli_frame_output_opt_t;

typedef struct
{
    int (*open_file)( char *psz_filename, hnd_t *p_handle, cli_frame_output_opt_t *opt );
    int (*picture_alloc)( cli_pic_t *pic, int width, int height );
    int (*write_frame)( cli_pic_t *pic, hnd_t handle );
    int (*release_frame)( cli_pic_t *pic, hnd_t handle );
    void (*picture_clean)( cli_pic_t *pic );
    int (*close_file)( hnd_t handle );
} cli_frame_output_t;

extern const cli_frame_output_t raw_frame_output;

extern cli_frame_output_t cli_frame_output;




#endif

