

#ifndef X265_OSDEP_H
#define X265_OSDEP_H

#define _LARGEFILE_SOURCE 1
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <inttypes.h>

#ifdef _WIN32
#include <io.h>    // _setmode()
#include <fcntl.h> // _O_BINARY
#endif

#include "config.h"


#ifdef __ICL
#define DECLARE_ALIGNED( var, n ) __declspec(align(n)) var
#else
#define DECLARE_ALIGNED( var, n ) var __attribute__((aligned(n)))
#endif
#define ALIGNED_16( var ) DECLARE_ALIGNED( var, 16 )
#define ALIGNED_8( var )  DECLARE_ALIGNED( var, 8 )
#define ALIGNED_4( var )  DECLARE_ALIGNED( var, 4 )

#if ARCH_ARM
#define ALIGNED_ARRAY_16( ... ) ALIGNED_ARRAY_EMU( 15, __VA_ARGS__ )
#else
#define ALIGNED_ARRAY_16( type, name, sub1, ... )\
    ALIGNED_16( type name sub1 __VA_ARGS__ )
#endif




#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define UNUSED __attribute__((unused))
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define NOINLINE __attribute__((noinline))
#define MAY_ALIAS __attribute__((may_alias))
#define x265_constant_p(x) __builtin_constant_p(x)
#define x265_nonconstant_p(x) (!__builtin_constant_p(x))
#else
#ifdef __ICL
#define ALWAYS_INLINE __forceinline
#define NOINLINE __declspec(noinline)
#else
#define ALWAYS_INLINE inline
#define NOINLINE
#endif
#define UNUSED
#define MAY_ALIAS
#define x265_constant_p(x) 0
#define x265_nonconstant_p(x) 0
#endif

#if HAVE_WIN32THREAD || PTW32_STATIC_LIB
int x265_threading_init( void );
#else
#define x265_threading_init() 0
#endif


#define WORD_SIZE sizeof(void*)

static ALWAYS_INLINE uint16_t endian_fix16( uint16_t x )
{
    return (x<<8)|(x>>8);
}

static ALWAYS_INLINE uint32_t endian_fix32( uint32_t x )
{
    return (x<<24) + ((x<<8)&0xff0000) + ((x>>8)&0xff00) + (x>>24);
}

static ALWAYS_INLINE uint64_t endian_fix64( uint64_t x )
{
    return endian_fix32(x>>32) + ((uint64_t)endian_fix32(x)<<32);
}

static ALWAYS_INLINE intptr_t endian_fix( intptr_t x )
{
    return WORD_SIZE == 8 ? endian_fix64(x) : endian_fix32(x);
}

#if HAVE_POSIXTHREAD
#if SYS_WINDOWS
#else
#include <unistd.h>
#endif /* SYS_WINDOWS */
#elif HAVE_WIN32THREAD
#else
#endif




static inline uint8_t x265_is_regular_file( FILE *filehandle )
{
    struct stat file_stat;
    if( fstat( fileno( filehandle ), &file_stat ) )
    {
        return -1;
    }
    return S_ISREG( file_stat.st_mode );
}


static inline uint8_t x265_is_regular_file_path( const char *filename )
{
    struct stat file_stat;
    if( stat( filename, &file_stat ) )
        return -1;
    return S_ISREG( file_stat.st_mode );
}


#endif

