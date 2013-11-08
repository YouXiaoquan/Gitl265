

#ifndef X265_INPUT_FILTERS_H
#define X265_INPUT_FILTERS_H


#include "frame_input/frame_input.h"


typedef struct cli_input_filter_t cli_input_filter_t;

struct cli_input_filter_t
{
    /* name of the filter */
    const char *name;
    /* help: a short message on what the filter does and how to use it.
     * this should only be implemented by filters directly accessible by the user */
    void (*help)( int longhelp );
    /* init: initializes the filter given the input clip properties and parameter to adjust them as necessary
     * with the given options proinputed by the user.
     * returns 0 on success, nonzero on error. */
    int (*init)( hnd_t *handle, cli_input_filter_t *filter, video_info_t *info, x265_param_t *param );
    /* get_frame: given the storage for the output frame and desired frame number, generate the frame accordingly.
     * the image data returned by get_frame should be treated as const and not be altered.
     * returns 0 on success, nonzero on error. */
    int (*get_frame)( hnd_t handle, cli_pic_t *output, int frame );
    /* release_frame: frame is done being used and is signaled for cleanup.
     * returns 0 on succeess, nonzero on error. */
    int (*release_frame)( hnd_t handle, cli_pic_t *pic );
    /* free: run filter cleanup procedures. */
    void (*free)( hnd_t handle );
    /* next registered filter, unused by filters themselves */
    cli_input_filter_t *next;
};

void x265_register_input_filters( void );
void x265_input_filter_help( int longhelp );
int  x265_init_input_filter( const char *name, hnd_t *handle, cli_input_filter_t *filter,
                           video_info_t *info, x265_param_t *param );



#endif

