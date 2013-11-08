


#ifndef X265_RD_COST_HADS_X86_H
#define X265_RD_COST_HADS_X86_H

uint32_t x265_pixel_hads_4x4_ssse3(pixel *cur,
									uint32_t i_stride_cur,
									pixel *org,
									uint32_t i_stride_org )	;

uint32_t x265_pixel_hads_8x8_ssse3(pixel *cur,
									uint32_t i_stride_cur,
									pixel *org,
									uint32_t i_stride_org )	;
uint32_t x265_pixel_hads_32x32_ssse3(pixel *cur,
									uint32_t i_stride_cur,
									pixel *org,
									uint32_t i_stride_org )	;









#endif



