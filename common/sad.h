


#ifndef X265_SAD_H
#define X265_SAD_H


#include "x86/rd_cost_sads_x86.h"

typedef uint32_t (*x265_sads_func)(pixel *cur,
								uint32_t i_stride_cur,
								pixel *org,
								uint32_t i_stride_org,
								int32_t i_bit_depth ) ;

void x265_sad_initialize (x265_sads_func *sads_func,
						x265_sads_func *sads_half_func,
						unsigned int cpu ) ;

DECLARE_ALL_PIXEL_N_SADS_FUNCTION(_,c)
DECLARE_ALL_PIXEL_N_SADS_FUNCTION(_,cmp)

DECLARE_ALL_PIXEL_N_SADS_FUNCTION(_half_,c)
DECLARE_ALL_PIXEL_N_SADS_FUNCTION(_half_,cmp)





#endif

