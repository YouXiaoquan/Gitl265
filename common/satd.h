


#ifndef X265_SATD_H
#define X265_SATD_H


#include "x86/rd_cost_hads_x86.h"

typedef uint32_t (*x265_satd_func)(pixel *cur,
								uint32_t i_stride_cur,
								pixel *org,
								uint32_t i_stride_org ) ;

typedef struct _x265_satd_t x265_satd_t ;
struct _x265_satd_t
{
	x265_satd_func satd_func[64] ;
};

int PartitionFromSizes(int width, int height) ;
void x265_satd_initialize ( x265_satd_t *satd, unsigned int cpu ) ;

DECLARE_ALL_PIXEL_N_HADS_4x4_FUNCTION(c)
DECLARE_ALL_PIXEL_N_HADS_4x4_FUNCTION(cmp)





#endif

