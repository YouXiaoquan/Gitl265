

#include "input_filters.h"


static cli_input_filter_t *first_input_filter = NULL;

static void register_input_filter( cli_input_filter_t *new_filter )
{
    cli_input_filter_t *filter_i = first_input_filter;
    while( filter_i->next )
    {
        filter_i = filter_i->next;
    }
    filter_i->next = new_filter;
    new_filter->next = NULL;
}

#define REGISTER_INPUT_FILTER(name)\
{\
    extern cli_input_filter_t name##_input_filter;\
    register_input_filter( &name##_input_filter );\
}

void x265_register_input_filters( void )
{
    extern cli_input_filter_t source_input_filter;
    first_input_filter = &source_input_filter;
    REGISTER_INPUT_FILTER( resize );
    REGISTER_INPUT_FILTER( depth );
}

int x265_init_input_filter( const char *name, hnd_t *handle, cli_input_filter_t *filter,
                          video_info_t *info, x265_param_t *param )
{
    cli_input_filter_t *filter_i = first_input_filter;
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

void x265_input_filter_help( int longhelp )
{
    for( cli_input_filter_t *filter_i = first_input_filter; filter_i; filter_i = filter_i->next )
    {
        if( filter_i->help )
        {
            filter_i->help( longhelp );
        }
    }
}
