


#ifndef X265_INTERPOLATION_FILTER_H
#define X265_INTERPOLATION_FILTER_H

#include "ip.h"

#define X265_NTAPS_LUMA        8 ///< Number of taps for luma
#define X265_NTAPS_CHROMA      4 ///< Number of taps for chroma
#define X265_IF_INTERNAL_PREC 14 ///< Number of bits for internal precision
#define X265_IF_FILTER_PREC    6 ///< Log2 of sum of filter taps
#define X265_IF_INTERNAL_OFFS (1<<(X265_IF_INTERNAL_PREC-1)) ///< Offset used internally


typedef struct
{
	x265_ip_t ip ;
	int16_t i_luma_filter[4][X265_NTAPS_LUMA];     ///< Luma filter taps
	int16_t i_chroma_filter[8][X265_NTAPS_CHROMA]; ///< Chroma filter taps

} x265_interpolation_filter_t ;

extern int16_t luma_filter[4][X265_NTAPS_LUMA] ;
extern int16_t chroma_filter[8][X265_NTAPS_CHROMA] ;

x265_interpolation_filter_t *x265_interpolation_filter_new (x265_t *h) ;
void x265_interpolation_filter_delete ( x265_interpolation_filter_t *interpolation_filter ) ;

int x265_interpolation_filter_init ( x265_t *h, x265_interpolation_filter_t *interpolation_filter );
void x265_interpolation_filter_deinit ( x265_interpolation_filter_t *interpolation_filter );


void x265_interpolation_filter_filter_copy_p_p(x265_interpolation_filter_t *interpolation_filter,
											int32_t i_bit_depth,
											pixel *src,
											int32_t i_src_stride,
											pixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last) ;
void x265_interpolation_filter_filter_copy_p_s(x265_interpolation_filter_t *interpolation_filter,
											int32_t i_bit_depth,
											pixel *src,
											int32_t i_src_stride,
											spixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last) ;
void x265_interpolation_filter_filter_copy_s_p(x265_interpolation_filter_t *interpolation_filter,
											int32_t i_bit_depth,
											spixel *src,
											int32_t i_src_stride,
											pixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last) ;
void x265_interpolation_filter_filter_copy_s_s(x265_interpolation_filter_t *interpolation_filter,
											int32_t i_bit_depth,
											spixel *src,
											int32_t i_src_stride,
											spixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last) ;
void x265_interpolation_filter_filter_p_p(x265_interpolation_filter_t *interpolation_filter,
									int32_t n,
									int32_t b_is_vertical,
									int32_t b_is_first,
									int32_t b_is_last,
									int32_t i_bit_depth,
									pixel *src,
									int32_t i_src_stride,
									pixel *dst,
									int32_t i_dst_stride,
									int32_t i_width,
									int32_t i_height,
									int16_t *coeff) ;
void x265_interpolation_filter_filter_p_s(x265_interpolation_filter_t *interpolation_filter,
									int32_t n,
									int32_t b_is_vertical,
									int32_t b_is_first,
									int32_t b_is_last,
									int32_t i_bit_depth,
									pixel *src,
									int32_t i_src_stride,
									spixel *dst,
									int32_t i_dst_stride,
									int32_t i_width,
									int32_t i_height,
									int16_t *coeff) ;
void x265_interpolation_filter_filter_s_p(x265_interpolation_filter_t *interpolation_filter,
									int32_t n,
									int32_t b_is_vertical,
									int32_t b_is_first,
									int32_t b_is_last,
									int32_t i_bit_depth,
									spixel *src,
									int32_t i_src_stride,
									pixel *dst,
									int32_t i_dst_stride,
									int32_t i_width,
									int32_t i_height,
									int16_t *coeff) ;
void x265_interpolation_filter_filter_s_s(x265_interpolation_filter_t *interpolation_filter,
									int32_t n,
									int32_t b_is_vertical,
									int32_t b_is_first,
									int32_t b_is_last,
									int32_t i_bit_depth,
									spixel *src,
									int32_t i_src_stride,
									spixel *dst,
									int32_t i_dst_stride,
									int32_t i_width,
									int32_t i_height,
									int16_t *coeff) ;
void x265_interpolation_filter_filter_hor_p_p(x265_interpolation_filter_t *interpolation_filter,
										int32_t n,
										int32_t i_bit_depth,
										pixel *src,
										int32_t i_src_stride,
										pixel *dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t b_is_last,
										int16_t *coeff) ;
void x265_interpolation_filter_filter_hor_p_s(x265_interpolation_filter_t *interpolation_filter,
										int32_t n,
										int32_t i_bit_depth,
										pixel *src,
										int32_t i_src_stride,
										spixel *dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t b_is_last,
										int16_t *coeff) ;
void x265_interpolation_filter_filter_ver_p_p(x265_interpolation_filter_t *interpolation_filter,
											int32_t n,
											int32_t i_bit_depth,
											pixel *src,
											int32_t i_src_stride,
											pixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last,
											int16_t *coeff) ;
void x265_interpolation_filter_filter_ver_p_s(x265_interpolation_filter_t *interpolation_filter,
											int32_t n,
											int32_t i_bit_depth,
											pixel *src,
											int32_t i_src_stride,
											spixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last,
											int16_t *coeff) ;
void x265_interpolation_filter_filter_ver_s_p(x265_interpolation_filter_t *interpolation_filter,
											int32_t n,
											int32_t i_bit_depth,
											spixel *src,
											int32_t i_src_stride,
											pixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last,
											int16_t *coeff) ;
void x265_interpolation_filter_filter_ver_s_s(x265_interpolation_filter_t *interpolation_filter,
											int32_t n,
											int32_t i_bit_depth,
											spixel *src,
											int32_t i_src_stride,
											spixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last,
											int16_t *coeff) ;

void x265_interpolation_filter_filter_hor_luma_p_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_hor_luma_p_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_ver_luma_p_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_ver_luma_p_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_ver_luma_s_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												spixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_ver_luma_s_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												spixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_hor_chroma_p_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_hor_chroma_p_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_ver_chroma_p_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_ver_chroma_p_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_ver_chroma_s_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												spixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last ) ;
void x265_interpolation_filter_filter_ver_chroma_s_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												spixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last ) ;



#endif

