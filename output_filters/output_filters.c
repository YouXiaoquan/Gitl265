

#include "output_filters.h"


static cli_output_filter_t *first_output_filter = NULL;

static void register_output_filter( cli_output_filter_t *new_filter )
{
    cli_output_filter_t *filter_i = first_output_filter;
    while( filter_i->next )
    {
        filter_i = filter_i->next;
    }
    filter_i->next = new_filter;
    new_filter->next = NULL;
}

#define REGISTER_OUTPUT_FILTER(name)\
{\
    extern cli_output_filter_t name##_output_filter;\
    register_output_filter( &name##_output_filter );\
}

void x265_register_output_filters( void )
{
    extern cli_output_filter_t source_output_filter;
    first_output_filter = &source_output_filter;
    REGISTER_OUTPUT_FILTER( resize );
    REGISTER_OUTPUT_FILTER( depth );
}

int x265_init_output_filter( const char *name, hnd_t *handle, cli_output_filter_t *filter,
                          video_info_t *info, x265_param_t *param )
{
    cli_output_filter_t *filter_i = first_output_filter;
    while( filter_i && strcasecmp( name, filter_i->name ) )
    {
        filter_i = filter_i->next;
    }
    FAIL_IF_ERR( !filter_i, "x265", "invalid filter `%s'\n", name );
    if( filter_i->init( handle, filter, info, param ) )
    {
        return -1;
    }

    return 0;
}

void x265_output_filter_help( int longhelp )
{
    for( cli_output_filter_t *filter_i = first_output_filter; filter_i; filter_i = filter_i->next )
    {
        if( filter_i->help )
        {
            filter_i->help( longhelp );
        }
    }
}
