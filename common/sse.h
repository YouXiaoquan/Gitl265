



#ifndef X265_SSE_H
#define X265_SSE_H


#include "x86/rd_cost_sses_x86.h"


typedef uint32_t (*x265_sses_p_p_func)(pixel *cur,
										uint32_t i_stride_cur,
										pixel *org,
										uint32_t i_stride_org,
										int32_t i_bit_depth ) ;

typedef uint32_t (*x265_sses_p_s_func)(pixel *cur,
										uint32_t i_stride_cur,
										short_pixel *org,
										uint32_t i_stride_org,
										int32_t i_bit_depth ) ;

typedef uint32_t (*x265_sses_s_s_func)(short_pixel *cur,
										uint32_t i_stride_cur,
										short_pixel *org,
										uint32_t i_stride_org,
										int32_t i_bit_depth ) ;


void x265_sse_initialize ( x265_sses_p_p_func *sse_p_p_func,
							x265_sses_p_s_func *sse_p_s_func,
							x265_sses_s_s_func *sse_s_s_func,
							unsigned int cpu ) ;

DECLARE_ALL_PIXEL_N_SSES_FUNCTION(pixel,p,pixel,p,c)
DECLARE_ALL_PIXEL_N_SSES_FUNCTION(pixel,p,short_pixel,s,c)
DECLARE_ALL_PIXEL_N_SSES_FUNCTION(short_pixel,s,short_pixel,s,c)

DECLARE_ALL_PIXEL_N_SSES_FUNCTION(pixel,p,pixel,p,cmp)
DECLARE_ALL_PIXEL_N_SSES_FUNCTION(pixel,p,short_pixel,s,cmp)
DECLARE_ALL_PIXEL_N_SSES_FUNCTION(short_pixel,s,short_pixel,s,cmp)

#endif

