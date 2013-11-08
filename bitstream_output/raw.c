#include "bitstream_output.h"

static int open_file( char *psz_filename, hnd_t *p_handle, cli_bitstream_output_opt_t *opt )
{
    if( !strcmp( psz_filename, "-" ) )
    {
        *p_handle = stdout;
    }
    else if( !(*p_handle = fopen( psz_filename, "w+b" )) )
    {
        return -1;
    }

    return 0;
}

static int set_param( hnd_t handle, x265_param_t *p_param )
{
    return 0;
}

static int write_headers( hnd_t handle, x265_nal_t *p_nal )
{
    int size = p_nal[0].i_payload + p_nal[1].i_payload + p_nal[2].i_payload;

    if( fwrite( p_nal[0].p_payload, size, 1, (FILE*)handle ) )
    {
        return size;
    }
    return -1;
}

static int write_frame( hnd_t handle, uint8_t *p_nalu, int i_size, x265_picture_t *p_picture )
{
    if( fwrite( p_nalu, i_size, 1, (FILE*)handle ) )
    {
        return i_size;
    }
    return -1;
}

static int close_file( hnd_t handle, int64_t largest_pts, int64_t second_largest_pts )
{
    if( !handle || handle == stdout )
    {
        return 0;
    }

    return fclose( (FILE*)handle );
}

const cli_bitstream_output_t raw_bitstream_output = { open_file, set_param, write_headers, write_frame, close_file };

