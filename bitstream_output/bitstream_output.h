

#ifndef X265_BITSTREAM_OUTPUT_H
#define X265_BITSTREAM_OUTPUT_H

#include "x265cli.h"

typedef struct
{
    int use_dts_compress;
} cli_bitstream_output_opt_t;

typedef struct
{
    int (*open_file)( char *psz_filename, hnd_t *p_handle, cli_bitstream_output_opt_t *opt );
    int (*set_param)( hnd_t handle, x265_param_t *p_param );
    int (*write_headers)( hnd_t handle, x265_nal_t *p_nal );
    int (*write_frame)( hnd_t handle, uint8_t *p_nal, int i_size, x265_picture_t *p_picture );
    int (*close_file)( hnd_t handle, int64_t largest_pts, int64_t second_largest_pts );
} cli_bitstream_output_t;

extern const cli_bitstream_output_t raw_bitstream_output;

#endif

