


#ifndef X265_CLI_H
#define X265_CLI_H

#include "common/common.h"

/* In microseconds */
#define UPDATE_INTERVAL 250000

typedef void *hnd_t;


static inline char *get_filename_extension( char *filename )
{
    char *ext = filename + strlen( filename );
    while( *ext != '.' && ext > filename )
    {
        ext--;
    }
    ext += *ext == '.';
    return ext;
}


void x265_cli_log( const char *name, int i_level, const char *fmt, ... );
void x265_cli_printf( int i_level, const char *fmt, ... );

#define RETURN_IF_ERR( cond, name, ret, ... )\
if( cond )\
{\
    x265_cli_log( name, X265_LOG_ERROR, __VA_ARGS__ );\
    return ret;\
}

#define FAIL_IF_ERR( cond, name, ... ) RETURN_IF_ERR( cond, name, -1, __VA_ARGS__ )


#endif



