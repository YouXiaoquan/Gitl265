

#include "common.h"

#define DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,mode_id,extent,blk_size) \
void x265_intra_pred_mode_##mode_id##_##name##_##extent(pixel *left, \
													pixel *top, \
													pixel* dst, \
													int i_dst_stride, \
													int i_chroma_idx, \
													int32_t i_bit_depth) \
{ \
	int32_t i_blk_size = 0 ; \
	int32_t k,l; \
	int32_t b_mode_dc = 0; \
	int32_t b_mode_hor = 0; \
	int32_t b_mode_ver = 0; \
	int32_t i_intra_pred_angle = 0; \
	int32_t i_abs_ang = 0; \
	int32_t i_sign_ang = 0; \
	int32_t i_inv_angle = 0; \
	int32_t i_inv_angle_sum = 0; \
	int32_t i_delta_pos = 0; \
	int32_t i_delta_int = 0; \
	int32_t i_delta_fract = 0; \
	int32_t i_ref_main_index = 0; \
	pixel i_tmp = 0; \
	int32_t ang_table[9]    = {0,    2,    5,   9,  13,  17,  21,  26,  32}; \
	int32_t inv_ang_table[9] = {0, 4096, 1638, 910, 630, 482, 390, 315, 256}; \
	pixel *ref_main; \
	pixel *ref_side; \
	pixel ref_above[2*X265_MAX_CU_SIZE+1]; \
	pixel ref_left[2*X265_MAX_CU_SIZE+1]; \
	i_blk_size = blk_size ; \
	assert( mode_id > 0 ); \
	b_mode_dc = mode_id < 2; \
	b_mode_hor = !b_mode_dc && (mode_id < 18); \
	b_mode_ver = !b_mode_dc && !b_mode_hor; \
	i_intra_pred_angle = b_mode_ver ? (int32_t)mode_id - X265_VER_IDX : b_mode_hor ? -((int32_t)mode_id - X265_HOR_IDX) : 0; \
	i_abs_ang = abs(i_intra_pred_angle); \
	i_sign_ang = i_intra_pred_angle < 0 ? -1 : 1; \
	i_inv_angle = inv_ang_table[i_abs_ang]; \
	i_abs_ang = ang_table[i_abs_ang]; \
	i_intra_pred_angle = i_sign_ang * i_abs_ang; \
\
	if (i_intra_pred_angle < 0) \
	{ \
		memcpy (ref_above + i_blk_size - 1, top-1, sizeof(pixel) * (i_blk_size + 1)); \
		memcpy (ref_left + i_blk_size - 1, left-1, sizeof(pixel) * (i_blk_size + 1)); \
		ref_main = (b_mode_ver ? ref_above : ref_left) + (i_blk_size-1); \
		ref_side = (b_mode_ver ? ref_left : ref_above) + (i_blk_size-1); \
		i_inv_angle_sum    = 128; \
		for (k=-1; k>i_blk_size*i_intra_pred_angle>>5; k--) \
		{ \
			i_inv_angle_sum += i_inv_angle; \
			ref_main[k] = ref_side[i_inv_angle_sum>>8]; \
		} \
	} \
	else \
	{ \
		memcpy (ref_above, top-1, sizeof(pixel) * (2 * i_blk_size + 1)); \
		memcpy (ref_left, left-1, sizeof(pixel) * (2 * i_blk_size + 1)); \
		ref_main = b_mode_ver ? ref_above : ref_left; \
		ref_side = b_mode_ver ? ref_left  : ref_above; \
	} \
\
	if (i_intra_pred_angle == 0) \
	{ \
		for (k=0;k<i_blk_size;k++) \
		{ \
			for (l=0;l<i_blk_size;l++) \
			{ \
				dst[k*i_dst_stride+l] = ref_main[l+1]; \
			} \
		} \
\
		if ( 0 == i_chroma_idx && i_blk_size < 32 ) \
		{ \
			for (k=0;k<i_blk_size;k++) \
			{ \
				dst[k*i_dst_stride] = x265_clip3_int32(dst[k*i_dst_stride] + (( ref_side[k+1] - ref_side[0] ) >> 1), \
														0, \
														(1<<8)-1); \
			} \
		} \
	} \
	else \
	{ \
		for (k=0;k<i_blk_size;k++) \
		{ \
			i_delta_pos += i_intra_pred_angle; \
			i_delta_int   = i_delta_pos >> 5; \
			i_delta_fract = i_delta_pos & (32 - 1); \
\
			if (i_delta_fract) \
			{ \
				for (l=0;l<i_blk_size;l++) \
				{ \
					i_ref_main_index        = l+i_delta_int+1; \
					dst[k*i_dst_stride+l] = (pixel) ( ((32-i_delta_fract)*ref_main[i_ref_main_index]+i_delta_fract*ref_main[i_ref_main_index+1]+16) >> 5 ); \
				} \
			} \
			else \
			{ \
				for (l=0;l<i_blk_size;l++) \
				{ \
					dst[k*i_dst_stride+l] = ref_main[l+i_delta_int+1]; \
				} \
			} \
		} \
	} \
\
	if (b_mode_hor) \
	{ \
		for (k=0;k<i_blk_size-1;k++) \
		{ \
			for (l=k+1;l<i_blk_size;l++) \
			{ \
				i_tmp = dst[k*i_dst_stride+l]; \
				dst[k*i_dst_stride+l] = dst[l*i_dst_stride+k]; \
				dst[l*i_dst_stride+k] = i_tmp; \
			} \
		} \
	} \
}

#define DEFINE_ONE_INTRA_PRED_PLANAR_FUNCTION_C(name,extent,blk_size,shift) \
void x265_intra_pred_mode_0_##name##_##extent(pixel *left, \
													pixel *top, \
													pixel* dst, \
													int i_dst_stride, \
													int i_chroma_idx, \
													int32_t i_bit_depth) \
{ \
	int32_t k, l, i_bottom_left, i_top_right; \
	int32_t i_hor_pred; \
	int16_t left_column[blk_size+1], top_row[blk_size+1], bottom_row[blk_size], right_column[blk_size]; \
	for ( k = 0 ; k < blk_size+1 ; ++ k ) \
	{ \
		left_column[k] = left[k] ; \
		top_row[k] = top[k] ; \
	} \
	i_bottom_left = left_column[blk_size]; \
	i_top_right   = top_row[blk_size]; \
	for (k=0;k<blk_size;k++) \
	{ \
		bottom_row[k]   = i_bottom_left - top_row[k]; \
		right_column[k] = i_top_right   - left_column[k]; \
		top_row[k]      <<= shift; \
		left_column[k]  <<= shift; \
	} \
	for (k=0;k<blk_size;k++) \
	{ \
		i_hor_pred = left_column[k] + blk_size; \
		for (l=0;l<blk_size;l++) \
		{ \
			i_hor_pred += right_column[k]; \
			top_row[l] += bottom_row[l]; \
			dst[k*i_dst_stride+l] = ( (i_hor_pred + top_row[l]) >> (shift+1) ); \
		} \
	} \
}

#define DEFINE_ONE_INTRA_PRED_DC_FUNCTION_0_2_C(name,extent,blk_size,shift) \
void x265_intra_pred_mode_1_##name##_##extent(pixel *left, \
													pixel *top, \
													pixel* dst, \
													int i_dst_stride, \
													int i_chroma_idx, \
													int32_t i_bit_depth) \
{ \
	int32_t loop = 0 ; \
	int32_t i = 0, j = 0 ; \
	int32_t i_sum = 0 ; \
	pixel i_dc = 0 ; \
	pixel *temp_dst = NULL ; \
\
	for ( loop = 0 ; loop < blk_size ; ++ loop ) \
	{ \
		i_sum += left[loop] ; \
	} \
	for ( loop = 0 ; loop < blk_size ; ++ loop ) \
	{ \
		i_sum += top[loop] ; \
	} \
	i_dc = ((pixel)((i_sum + blk_size) >> (shift+1))) ; \
\
	temp_dst = dst ; \
	for ( i = 0 ; i < blk_size ; ++ i ) \
	{ \
		for ( j = 0 ; j < blk_size ; ++ j ) \
		{ \
			temp_dst[j] = i_dc ; \
		} \
		temp_dst += i_dst_stride ; \
	} \
	if ( 0 == i_chroma_idx ) \
	{ \
		dst[0] = (left[0] + 2 * dst[0] + top[0] + 2) >> 2 ; \
		for ( loop = 1 ; loop < blk_size ; ++ loop ) \
		{ \
			dst[loop] = (top[loop] + 3 * dst[loop] + 2) >> 2 ; \
		} \
		temp_dst = dst + i_dst_stride ; \
		for ( loop = 1 ; loop < blk_size ; ++ loop ) \
		{ \
			temp_dst[0] = (left[loop] + 3 * temp_dst[0] + 2) >> 2 ; \
			temp_dst += i_dst_stride ; \
		} \
	} \
}

#define DEFINE_ONE_INTRA_PRED_DC_FUNCTION_3_4_C(name,extent,blk_size,shift) \
void x265_intra_pred_mode_1_##name##_##extent(pixel *left, \
													pixel *top, \
													pixel* dst, \
													int i_dst_stride, \
													int i_chroma_idx, \
													int32_t i_bit_depth) \
{ \
	int32_t loop = 0 ; \
	int32_t i = 0, j = 0 ; \
	int32_t i_sum = 0 ; \
	pixel i_dc = 0 ; \
	pixel *temp_dst = NULL ; \
\
	for ( loop = 0 ; loop < blk_size ; ++ loop ) \
	{ \
		i_sum += left[loop] ; \
	} \
	for ( loop = 0 ; loop < blk_size ; ++ loop ) \
	{ \
		i_sum += top[loop] ; \
	} \
	i_dc = ((pixel)((i_sum + blk_size) >> (shift+1))) ; \
\
	temp_dst = dst ; \
	for ( i = 0 ; i < blk_size ; ++ i ) \
	{ \
		for ( j = 0 ; j < blk_size ; ++ j ) \
		{ \
			temp_dst[j] = i_dc ; \
		} \
		temp_dst += i_dst_stride ; \
	} \
}

#define DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,mode_id,extent,extent1,extent2,blk_size) \
void x265_intra_pred_mode_##mode_id##_##name##_##extent(pixel *left, \
													pixel *top, \
													pixel* dst, \
													int i_dst_stride, \
													int i_chroma_idx, \
													int32_t i_bit_depth) \
{ \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
	pixel dst1[blk_size*blk_size] ; \
\
	i_start_time = x265_get_timer_state () ; \
	x265_intra_pred_mode_##mode_id##_##name##_##extent1( left, \
														top, \
														dst1, \
														blk_size, \
														i_chroma_idx, \
														i_bit_depth ) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
\
	i_start_time = x265_get_timer_state () ; \
	x265_intra_pred_mode_##mode_id##_##name##_##extent2( left, \
														top, \
														dst, \
														i_dst_stride, \
														i_chroma_idx, \
														i_bit_depth ) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
\
	if ( memory_compare(dst, i_dst_stride, dst1, blk_size, blk_size, blk_size ) ) \
	{ \
		fprintf ( stderr, "Error:\t%d\t%d\t%d\n", i_chroma_idx, mode_id, blk_size ) ; \
		exit(0) ; \
	} \
}




void x265_intra_pred_dc_0_8( pixel *left,
								pixel *top,
								pixel *dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_bit_depth )
{
	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;
	pixel dst1[16] ;


	static int ww = 0 ;
	ww ++ ;
	if ( 28 == ww )
	{
		//	fprintf ( stderr, "Good\n" ) ;
	}

	if ( i_dst_stride % 4 )
	{
		fprintf ( stderr, "Error\n" ) ;
	}

	i_start_time = x265_get_timer_state () ;
	x265_intra_pred_mode_1_0_8_c( left,
								top,
								dst1,
								4,
								i_chroma_idx,
								i_bit_depth ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_1 ( i_end_time - i_start_time ) ;

	i_start_time = x265_get_timer_state () ;
	x265_intra_pred_mode_1_0_8_ssse3( left,
									top,
									dst,
									i_dst_stride,
									i_chroma_idx,
									i_bit_depth ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_2 ( i_end_time - i_start_time ) ;

	if ( memory_compare(dst, i_dst_stride, dst1, 4, 4, 4 ) )
	{
		fprintf ( stderr, "Error\t%d\t%d\t%d\n", i_chroma_idx, 1, 4 ) ; ;
		exit (0) ;
	}
}

void x265_intra_pred_dc_1_8( pixel *left,
								pixel *top,
								pixel *dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_bit_depth )
{
	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;
	pixel dst1[64] ;


	static int ww = 0 ;
	ww ++ ;
	if ( 37 == ww )
	{
		//	fprintf ( stderr, "Good\n" ) ;
	}

	if ( i_dst_stride % 4 )
	{
		fprintf ( stderr, "Error\n" ) ;
	}

	i_start_time = x265_get_timer_state () ;
	x265_intra_pred_mode_1_1_8_c( left,
								top,
								dst1,
								8,
								i_chroma_idx,
								i_bit_depth ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_1 ( i_end_time - i_start_time ) ;

	i_start_time = x265_get_timer_state () ;
	x265_intra_pred_mode_1_1_8_ssse3( left,
									top,
									dst,
									i_dst_stride,
									i_chroma_idx,
									i_bit_depth ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_2 ( i_end_time - i_start_time ) ;

	if ( memory_compare(dst, i_dst_stride, dst1, 8, 8, 8 ) )
	{
		fprintf ( stderr, "Error\t%d\t%d\t%d\n", i_chroma_idx, 1, 8 ) ;
		exit(0) ;
	}
}

void x265_intra_pred_dc_2_8( pixel *left,
								pixel *top,
								pixel *dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_bit_depth )
{
	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;
	pixel dst1[256] ;


	static int ww = 0 ;
	ww ++ ;
	if ( 63 == ww )
	{
		//	fprintf ( stderr, "Good\n" ) ;
	}

	if ( i_dst_stride % 4 )
	{
		fprintf ( stderr, "Error\n" ) ;
	}

	i_start_time = x265_get_timer_state () ;
	x265_intra_pred_mode_1_2_8_c( left,
								top,
								dst1,
								16,
								i_chroma_idx,
								i_bit_depth ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_1 ( i_end_time - i_start_time ) ;

	i_start_time = x265_get_timer_state () ;
	x265_intra_pred_mode_1_2_8_ssse3( left,
									top,
									dst,
									i_dst_stride,
									i_chroma_idx,
									i_bit_depth ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_2 ( i_end_time - i_start_time ) ;

	if ( memory_compare(dst, i_dst_stride, dst1, 16, 16, 16 ) )
	{
		fprintf ( stderr, "Error\t%d\t%d\t%d\n", i_chroma_idx, 1, 16 ) ; ;
		exit (0) ;
	}
}

void x265_intra_pred_dc_4_8( pixel *left,
								pixel *top,
								pixel *dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_bit_depth )
{
	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;
	pixel dst1[4096] ;


	static int ww = 0 ;
	ww ++ ;
	if ( 0 != i_chroma_idx )
	{
		//	fprintf ( stderr, "Good\n" ) ;
	}

	if ( i_dst_stride % 4 )
	{
		fprintf ( stderr, "Error\n" ) ;
	}

	i_start_time = x265_get_timer_state () ;
	x265_intra_pred_mode_1_4_8_c( left,
								top,
								dst1,
								64,
								i_chroma_idx,
								i_bit_depth ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_1 ( i_end_time - i_start_time ) ;

	i_start_time = x265_get_timer_state () ;
	x265_intra_pred_mode_1_4_8_ssse3( left,
									top,
									dst,
									i_dst_stride,
									i_chroma_idx,
									i_bit_depth ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_2 ( i_end_time - i_start_time ) ;

	if ( memory_compare(dst, i_dst_stride, dst1, 64, 64, 64 ) )
	{
		fprintf ( stderr, "Error\t%d\t%d\n", 1, 64 ) ;
		exit (0) ;
	}
}


void x265_intra_pred_initialize ( x265_intra_pred_t *ip, unsigned int cpu )
{
	X265_INTRA_PRED_INITIALIZE_ALL(ip->intra_pred_func,c)

	if ( cpu & X265_CPU_SSSE3 )
	{
		X265_INTRA_PRED_INITIALIZE_ALL(ip->intra_pred_func,ssse3)
	}
}

void x265_intra_pred_planar_0_8_c( pixel *left,
									pixel *top,
									pixel *dst,
									int32_t i_dst_stride )
{
	int32_t k, l, i_bottom_left, i_top_right;
	int32_t i_hor_pred;
	int16_t left_column[5], top_row[5], bottom_row[4], right_column[4];

	// get left and above reference column and row
	for ( k = 0 ; k < 5 ; ++ k )
	{
		left_column[k] = left[k] ;
		top_row[k] = top[k] ;
	}

	// prepare intermediate variables used in interpolation
	i_bottom_left = left_column[4];
	i_top_right   = top_row[4];
	for (k=0;k<4;k++)
	{
		bottom_row[k]   = i_bottom_left - top_row[k];
		right_column[k] = i_top_right   - left_column[k];
		top_row[k]      <<= 2;
		left_column[k]  <<= 2;
	}

	// generate prediction signal
	for (k=0;k<4;k++)
	{
		i_hor_pred = left_column[k] + 4;
		for (l=0;l<4;l++)
		{
			i_hor_pred += right_column[k];
			top_row[l] += bottom_row[l];
			dst[k*i_dst_stride+l] = ( (i_hor_pred + top_row[l]) >> 3 );
		}
	}
}

void x265_intra_pred_planar_1_8_c( pixel *left,
									pixel *top,
									pixel *dst,
									int32_t i_dst_stride )
{
	int32_t k, l, i_bottom_left, i_top_right;
	int32_t i_hor_pred;
	int16_t left_column[9], top_row[9], bottom_row[8], right_column[8];

	// get left and above reference column and row
	for ( k = 0 ; k < 9 ; ++ k )
	{
		left_column[k] = left[k] ;
		top_row[k] = top[k] ;
	}

	// prepare intermediate variables used in interpolation
	i_bottom_left = left_column[8];
	i_top_right   = top_row[8];
	for (k=0;k<8;k++)
	{
		bottom_row[k]   = i_bottom_left - top_row[k];
		right_column[k] = i_top_right   - left_column[k];
		top_row[k]      <<= 3;
		left_column[k]  <<= 3;
	}

	// generate prediction signal
	for (k=0;k<8;k++)
	{
		i_hor_pred = left_column[k] + 8;
		for (l=0;l<8;l++)
		{
			i_hor_pred += right_column[k];
			top_row[l] += bottom_row[l];
			dst[k*i_dst_stride+l] = ( (i_hor_pred + top_row[l]) >> 4 );
		}
	}
}

void x265_intra_pred_planar_2_8_c( pixel *left,
									pixel *top,
									pixel *dst,
									int32_t i_dst_stride )
{
	int32_t k, l, i_bottom_left, i_top_right;
	int32_t i_hor_pred;
	int16_t left_column[17], top_row[17], bottom_row[16], right_column[16];

	// get left and above reference column and row
	for ( k = 0 ; k < 17 ; ++ k )
	{
		left_column[k] = left[k] ;
		top_row[k] = top[k] ;
	}

	// prepare intermediate variables used in interpolation
	i_bottom_left = left_column[16];
	i_top_right   = top_row[16];
	for (k=0;k<16;k++)
	{
		bottom_row[k]   = i_bottom_left - top_row[k];
		right_column[k] = i_top_right   - left_column[k];
		top_row[k]      <<= 4;
		left_column[k]  <<= 4;
	}

	// generate prediction signal
	for (k=0;k<16;k++)
	{
		i_hor_pred = left_column[k] + 16;
		for (l=0;l<16;l++)
		{
			i_hor_pred += right_column[k];
			top_row[l] += bottom_row[l];
			dst[k*i_dst_stride+l] = ( (i_hor_pred + top_row[l]) >> 5 );
		}
	}
}

void x265_intra_pred_planar_3_8_c( pixel *left,
									pixel *top,
									pixel *dst,
									int32_t i_dst_stride )
{
	int32_t k, l, i_bottom_left, i_top_right;
	int32_t i_hor_pred;
	int16_t left_column[33], top_row[33], bottom_row[32], right_column[32];

	// get left and above reference column and row
	for ( k = 0 ; k < 33 ; ++ k )
	{
		left_column[k] = left[k] ;
		top_row[k] = top[k] ;
	}

	// prepare intermediate variables used in interpolation
	i_bottom_left = left_column[32];
	i_top_right   = top_row[32];
	for (k=0;k<32;k++)
	{
		bottom_row[k]   = i_bottom_left - top_row[k];
		right_column[k] = i_top_right   - left_column[k];
		top_row[k]      <<= 5;
		left_column[k]  <<= 5;
	}

	// generate prediction signal
	for (k=0;k<32;k++)
	{
		i_hor_pred = left_column[k] + 32;
		for (l=0;l<32;l++)
		{
			i_hor_pred += right_column[k];
			top_row[l] += bottom_row[l];
			dst[k*i_dst_stride+l] = ( (i_hor_pred + top_row[l]) >> 6 );
		}
	}
}

void x265_intra_pred_planar_4_8_c( pixel *left,
									pixel *top,
									pixel *dst,
									int32_t i_dst_stride )
{
	int32_t k, l, i_bottom_left, i_top_right;
	int32_t i_hor_pred;
	int16_t left_column[65], top_row[65], bottom_row[64], right_column[64];

	// get left and above reference column and row
	for ( k = 0 ; k < 65 ; ++ k )
	{
		left_column[k] = left[k] ;
		top_row[k] = top[k] ;
	}

	// prepare intermediate variables used in interpolation
	i_bottom_left = left_column[64];
	i_top_right   = top_row[64];
	for (k=0;k<64;k++)
	{
		bottom_row[k]   = i_bottom_left - top_row[k];
		right_column[k] = i_top_right   - left_column[k];
		top_row[k]      <<= 6;
		left_column[k]  <<= 6;
	}

	// generate prediction signal
	for (k=0;k<64;k++)
	{
		i_hor_pred = left_column[k] + 64;
		for (l=0;l<64;l++)
		{
			i_hor_pred += right_column[k];
			top_row[l] += bottom_row[l];
			dst[k*i_dst_stride+l] = ( (i_hor_pred + top_row[l]) >> 7 );
		}
	}
}

/*
void x265_intra_pred_mode_1_0_8_ssse3( pixel *left,
										pixel *top,
										pixel* dst,
										int i_dst_stride,
										int i_chroma_idx,
										int32_t i_bit_depth)
{
	int32_t loop = 0 ;
	int32_t i = 0, j = 0 ;
	int32_t i_sum = 0 ;
	pixel i_dc = 0 ;
	pixel *temp_dst = NULL ;

	for ( loop = 0 ; loop < 4 ; ++ loop )
	{
		i_sum += left[loop] ;
	}
	for ( loop = 0 ; loop < 4 ; ++ loop )
	{
		i_sum += top[loop] ;
	}
	i_dc = ((pixel)((i_sum + 4) >> 3)) ;

	temp_dst = dst ;
	for ( i = 0 ; i < 4 ; ++ i )
	{
		for ( j = 0 ; j < 4 ; ++ j )
		{
			temp_dst[j] = i_dc ;
		}
		temp_dst += i_dst_stride ;
	}
	if ( 0 == i_chroma_idx )
	{
		dst[0] = (left[0] + 2 * dst[0] + top[0] + 2) >> 2 ;
		for ( loop = 1 ; loop < 4 ; ++ loop )
		{
			dst[loop] = (top[loop] + 3 * dst[loop] + 2) >> 2 ;
		}
		temp_dst = dst + i_dst_stride ;
		for ( loop = 1 ; loop < 4 ; ++ loop )
		{
			temp_dst[0] = (left[loop] + 3 * temp_dst[0] + 2) >> 2 ;
			temp_dst += i_dst_stride ;
		}
	}
}

void x265_intra_pred_mode_1_1_8_ssse3( pixel *left,
										pixel *top,
										pixel* dst,
										int i_dst_stride,
										int i_chroma_idx,
										int32_t i_bit_depth)

{
	int32_t loop = 0 ;
	int32_t i = 0, j = 0 ;
	int32_t i_sum = 0 ;
	pixel i_dc = 0 ;
	pixel *temp_dst = NULL ;

	for ( loop = 0 ; loop < 8 ; ++ loop )
	{
		i_sum += left[loop] ;
	}
	for ( loop = 0 ; loop < 8 ; ++ loop )
	{
		i_sum += top[loop] ;
	}
	i_dc = ((pixel)((i_sum + 8) >> 4)) ;

	temp_dst = dst ;
	for ( i = 0 ; i < 8 ; ++ i )
	{
		for ( j = 0 ; j < 8 ; ++ j )
		{
			temp_dst[j] = i_dc ;
		}
		temp_dst += i_dst_stride ;
	}
	if ( 0 == i_chroma_idx )
	{
		dst[0] = (left[0] + 2 * dst[0] + top[0] + 2) >> 2 ;
		for ( loop = 1 ; loop < 8 ; ++ loop )
		{
			dst[loop] = (top[loop] + 3 * dst[loop] + 2) >> 2 ;
		}
		temp_dst = dst + i_dst_stride ;
		for ( loop = 1 ; loop < 8 ; ++ loop )
		{
			temp_dst[0] = (left[loop] + 3 * temp_dst[0] + 2) >> 2 ;
			temp_dst += i_dst_stride ;
		}
	}
}


void x265_intra_pred_mode_1_3_8_ssse3( pixel *left,
										pixel *top,
										pixel* dst,
										int i_dst_stride,
										int i_chroma_idx,
										int32_t i_bit_depth)
{
	int32_t loop = 0 ;
	int32_t i = 0, j = 0 ;
	int32_t i_sum = 0 ;
	pixel i_dc = 0 ;
	pixel *temp_dst = NULL ;

	for ( loop = 0 ; loop < 32 ; ++ loop )
	{
		i_sum += left[loop] ;
	}
	for ( loop = 0 ; loop < 32 ; ++ loop )
	{
		i_sum += top[loop] ;
	}
	i_dc = ((pixel)((i_sum + 32) >> 6)) ;

	temp_dst = dst ;
	for ( i = 0 ; i < 32 ; ++ i )
	{
		for ( j = 0 ; j < 32 ; ++ j )
		{
			temp_dst[j] = i_dc ;
		}
		temp_dst += i_dst_stride ;
	}
}

void x265_intra_pred_mode_1_4_8_ssse3( pixel *left,
										pixel *top,
										pixel* dst,
										int i_dst_stride,
										int i_chroma_idx,
										int32_t i_bit_depth)
{
	int32_t loop = 0 ;
	int32_t i = 0, j = 0 ;
	int32_t i_sum = 0 ;
	pixel i_dc = 0 ;
	pixel *temp_dst = NULL ;

	for ( loop = 0 ; loop < 64 ; ++ loop )
	{
		i_sum += left[loop] ;
	}
	for ( loop = 0 ; loop < 64 ; ++ loop )
	{
		i_sum += top[loop] ;
	}
	i_dc = ((pixel)((i_sum + 64) >> 7)) ;

	temp_dst = dst ;
	for ( i = 0 ; i < 64 ; ++ i )
	{
		for ( j = 0 ; j < 64 ; ++ j )
		{
			temp_dst[j] = i_dc ;
		}
		temp_dst += i_dst_stride ;
	}
}
*/

void x265_intra_pred_angle_0_8_c(pixel *left,
								pixel *top,
								pixel* dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_mode,
								int32_t i_bit_depth)
{
	int32_t i_blk_size = 0 ;
	int32_t k,l;
	// map the mode index to main prediction direction and angle

	int32_t b_mode_dc = 0;
	int32_t b_mode_hor = 0;
	int32_t b_mode_ver = 0;
	int32_t i_intra_pred_angle = 0;
	int32_t i_abs_ang = 0;
	int32_t i_sign_ang = 0;
	int32_t i_inv_angle = 0;
	int32_t i_inv_angle_sum = 0;       // rounding for (shift by 8)
	int32_t i_delta_pos = 0;
	int32_t i_delta_int = 0;
	int32_t i_delta_fract = 0;
	int32_t i_ref_main_index = 0;
	pixel i_tmp = 0;
	// set bitshifts and scale the angle parameter to block size
	int32_t ang_table[9]    = {0,    2,    5,   9,  13,  17,  21,  26,  32};
	int32_t inv_ang_table[9] = {0, 4096, 1638, 910, 630, 482, 390, 315, 256}; // (256 * 32) / angle
	pixel *ref_main;
	pixel *ref_side;
	pixel ref_above[2*X265_MAX_CU_SIZE+1];
	pixel ref_left[2*X265_MAX_CU_SIZE+1];

	// map the mode index to main prediction direction and angle
	i_blk_size = 4 ;
	assert( i_mode > 0 ); //no planar
	b_mode_dc = i_mode < 2;
	b_mode_hor = !b_mode_dc && (i_mode < 18);
	b_mode_ver = !b_mode_dc && !b_mode_hor;
	i_intra_pred_angle = b_mode_ver ? (int32_t)i_mode - X265_VER_IDX : b_mode_hor ? -((int32_t)i_mode - X265_HOR_IDX) : 0;
	i_abs_ang = abs(i_intra_pred_angle);
	i_sign_ang = i_intra_pred_angle < 0 ? -1 : 1;

	// set bitshifts and scale the angle parameter to block size
	i_inv_angle = inv_ang_table[i_abs_ang];
	i_abs_ang = ang_table[i_abs_ang];
	i_intra_pred_angle = i_sign_ang * i_abs_ang;

	if (i_intra_pred_angle < 0)
	{
		memcpy (ref_above + i_blk_size - 1, top-1, sizeof(pixel) * (i_blk_size + 1));
		memcpy (ref_left + i_blk_size - 1, left-1, sizeof(pixel) * (i_blk_size + 1));
		ref_main = (b_mode_ver ? ref_above : ref_left) + (i_blk_size-1);
		ref_side = (b_mode_ver ? ref_left : ref_above) + (i_blk_size-1);

		// extend the main reference to the left.
		i_inv_angle_sum    = 128;       // rounding for (shift by 8)
		for (k=-1; k>i_blk_size*i_intra_pred_angle>>5; k--)
		{
			i_inv_angle_sum += i_inv_angle;
			ref_main[k] = ref_side[i_inv_angle_sum>>8];
		}
	}
	else
	{
		memcpy (ref_above, top-1, sizeof(pixel) * (2 * i_blk_size + 1));
		memcpy (ref_left, left-1, sizeof(pixel) * (2 * i_blk_size + 1));
		ref_main = b_mode_ver ? ref_above : ref_left;
		ref_side = b_mode_ver ? ref_left  : ref_above;
	}

	if (i_intra_pred_angle == 0)
	{
		for (k=0;k<i_blk_size;k++)
		{
			for (l=0;l<i_blk_size;l++)
			{
				dst[k*i_dst_stride+l] = ref_main[l+1];
			}
		}

		if ( 0 == i_chroma_idx )
		{
			for (k=0;k<i_blk_size;k++)
			{
				dst[k*i_dst_stride] = x265_clip3_int32(dst[k*i_dst_stride] + (( ref_side[k+1] - ref_side[0] ) >> 1),
														0,
														(1<<8)-1);
			}
		}
	}
	else
	{
		for (k=0;k<i_blk_size;k++)
		{
			i_delta_pos += i_intra_pred_angle;
			i_delta_int   = i_delta_pos >> 5;
			i_delta_fract = i_delta_pos & (32 - 1);

			if (i_delta_fract)
			{
				// do linear filtering
				for (l=0;l<i_blk_size;l++)
				{
					i_ref_main_index        = l+i_delta_int+1;
					dst[k*i_dst_stride+l] = (pixel) ( ((32-i_delta_fract)*ref_main[i_ref_main_index]+i_delta_fract*ref_main[i_ref_main_index+1]+16) >> 5 );
				}
			}
			else
			{
				// just copy the integer samples
				for (l=0;l<i_blk_size;l++)
				{
					dst[k*i_dst_stride+l] = ref_main[l+i_delta_int+1];
				}
			}
		}
	}

	// flip the block if this is the horizontal mode
	if (b_mode_hor)
	{
		for (k=0;k<i_blk_size-1;k++)
		{
			for (l=k+1;l<i_blk_size;l++)
			{
				i_tmp = dst[k*i_dst_stride+l];
				dst[k*i_dst_stride+l] = dst[l*i_dst_stride+k];
				dst[l*i_dst_stride+k] = i_tmp;
			}
		}
	}
}

void x265_intra_pred_angle_1_8_c(pixel *left,
								pixel *top,
								pixel* dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_mode,
								int32_t i_bit_depth)
{
	int32_t i_blk_size = 0 ;
	int32_t k,l;
	// map the mode index to main prediction direction and angle

	int32_t b_mode_dc = 0;
	int32_t b_mode_hor = 0;
	int32_t b_mode_ver = 0;
	int32_t i_intra_pred_angle = 0;
	int32_t i_abs_ang = 0;
	int32_t i_sign_ang = 0;
	int32_t i_inv_angle = 0;
	int32_t i_inv_angle_sum = 0;       // rounding for (shift by 8)
	int32_t i_delta_pos = 0;
	int32_t i_delta_int = 0;
	int32_t i_delta_fract = 0;
	int32_t i_ref_main_index = 0;
	pixel i_tmp = 0;
	// set bitshifts and scale the angle parameter to block size
	int32_t ang_table[9]    = {0,    2,    5,   9,  13,  17,  21,  26,  32};
	int32_t inv_ang_table[9] = {0, 4096, 1638, 910, 630, 482, 390, 315, 256}; // (256 * 32) / angle
	pixel *ref_main;
	pixel *ref_side;
	pixel ref_above[2*X265_MAX_CU_SIZE+1];
	pixel ref_left[2*X265_MAX_CU_SIZE+1];

	// map the mode index to main prediction direction and angle
	i_blk_size = 8 ;
	assert( i_mode > 0 ); //no planar
	b_mode_dc = i_mode < 2;
	b_mode_hor = !b_mode_dc && (i_mode < 18);
	b_mode_ver = !b_mode_dc && !b_mode_hor;
	i_intra_pred_angle = b_mode_ver ? (int32_t)i_mode - X265_VER_IDX : b_mode_hor ? -((int32_t)i_mode - X265_HOR_IDX) : 0;
	i_abs_ang = abs(i_intra_pred_angle);
	i_sign_ang = i_intra_pred_angle < 0 ? -1 : 1;

	// set bitshifts and scale the angle parameter to block size
	i_inv_angle = inv_ang_table[i_abs_ang];
	i_abs_ang = ang_table[i_abs_ang];
	i_intra_pred_angle = i_sign_ang * i_abs_ang;

	if (i_intra_pred_angle < 0)
	{
		memcpy (ref_above + i_blk_size - 1, top-1, sizeof(pixel) * (i_blk_size + 1));
		memcpy (ref_left + i_blk_size - 1, left-1, sizeof(pixel) * (i_blk_size + 1));
		ref_main = (b_mode_ver ? ref_above : ref_left) + (i_blk_size-1);
		ref_side = (b_mode_ver ? ref_left : ref_above) + (i_blk_size-1);

		// extend the main reference to the left.
		i_inv_angle_sum    = 128;       // rounding for (shift by 8)
		for (k=-1; k>i_blk_size*i_intra_pred_angle>>5; k--)
		{
			i_inv_angle_sum += i_inv_angle;
			ref_main[k] = ref_side[i_inv_angle_sum>>8];
		}
	}
	else
	{
		memcpy (ref_above, top-1, sizeof(pixel) * (2 * i_blk_size + 1));
		memcpy (ref_left, left-1, sizeof(pixel) * (2 * i_blk_size + 1));
		ref_main = b_mode_ver ? ref_above : ref_left;
		ref_side = b_mode_ver ? ref_left  : ref_above;
	}

	if (i_intra_pred_angle == 0)
	{
		for (k=0;k<i_blk_size;k++)
		{
			for (l=0;l<i_blk_size;l++)
			{
				dst[k*i_dst_stride+l] = ref_main[l+1];
			}
		}

		if ( 0 == i_chroma_idx )
		{
			for (k=0;k<i_blk_size;k++)
			{
				dst[k*i_dst_stride] = x265_clip3_int32(dst[k*i_dst_stride] + (( ref_side[k+1] - ref_side[0] ) >> 1),
														0,
														(1<<8)-1);
			}
		}
	}
	else
	{
		for (k=0;k<i_blk_size;k++)
		{
			i_delta_pos += i_intra_pred_angle;
			i_delta_int   = i_delta_pos >> 5;
			i_delta_fract = i_delta_pos & (32 - 1);

			if (i_delta_fract)
			{
				// do linear filtering
				for (l=0;l<i_blk_size;l++)
				{
					i_ref_main_index        = l+i_delta_int+1;
					dst[k*i_dst_stride+l] = (pixel) ( ((32-i_delta_fract)*ref_main[i_ref_main_index]+i_delta_fract*ref_main[i_ref_main_index+1]+16) >> 5 );
				}
			}
			else
			{
				// just copy the integer samples
				for (l=0;l<i_blk_size;l++)
				{
					dst[k*i_dst_stride+l] = ref_main[l+i_delta_int+1];
				}
			}
		}
	}

	// flip the block if this is the horizontal mode
	if (b_mode_hor)
	{
		for (k=0;k<i_blk_size-1;k++)
		{
			for (l=k+1;l<i_blk_size;l++)
			{
				i_tmp = dst[k*i_dst_stride+l];
				dst[k*i_dst_stride+l] = dst[l*i_dst_stride+k];
				dst[l*i_dst_stride+k] = i_tmp;
			}
		}
	}
}

void x265_intra_pred_angle_2_8_c(pixel *left,
								pixel *top,
								pixel* dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_mode,
								int32_t i_bit_depth)
{
	int32_t i_blk_size = 0 ;
	int32_t k,l;
	// map the mode index to main prediction direction and angle

	int32_t b_mode_dc = 0;
	int32_t b_mode_hor = 0;
	int32_t b_mode_ver = 0;
	int32_t i_intra_pred_angle = 0;
	int32_t i_abs_ang = 0;
	int32_t i_sign_ang = 0;
	int32_t i_inv_angle = 0;
	int32_t i_inv_angle_sum = 0;       // rounding for (shift by 8)
	int32_t i_delta_pos = 0;
	int32_t i_delta_int = 0;
	int32_t i_delta_fract = 0;
	int32_t i_ref_main_index = 0;
	pixel i_tmp = 0;
	// set bitshifts and scale the angle parameter to block size
	int32_t ang_table[9]    = {0,    2,    5,   9,  13,  17,  21,  26,  32};
	int32_t inv_ang_table[9] = {0, 4096, 1638, 910, 630, 482, 390, 315, 256}; // (256 * 32) / angle
	pixel *ref_main;
	pixel *ref_side;
	pixel ref_above[2*X265_MAX_CU_SIZE+1];
	pixel ref_left[2*X265_MAX_CU_SIZE+1];

	// map the mode index to main prediction direction and angle
	i_blk_size = 16 ;
	assert( i_mode > 0 ); //no planar
	b_mode_dc = i_mode < 2;
	b_mode_hor = !b_mode_dc && (i_mode < 18);
	b_mode_ver = !b_mode_dc && !b_mode_hor;
	i_intra_pred_angle = b_mode_ver ? (int32_t)i_mode - X265_VER_IDX : b_mode_hor ? -((int32_t)i_mode - X265_HOR_IDX) : 0;
	i_abs_ang = abs(i_intra_pred_angle);
	i_sign_ang = i_intra_pred_angle < 0 ? -1 : 1;

	// set bitshifts and scale the angle parameter to block size
	i_inv_angle = inv_ang_table[i_abs_ang];
	i_abs_ang = ang_table[i_abs_ang];
	i_intra_pred_angle = i_sign_ang * i_abs_ang;

	if (i_intra_pred_angle < 0)
	{
		memcpy (ref_above + i_blk_size - 1, top-1, sizeof(pixel) * (i_blk_size + 1));
		memcpy (ref_left + i_blk_size - 1, left-1, sizeof(pixel) * (i_blk_size + 1));
		ref_main = (b_mode_ver ? ref_above : ref_left) + (i_blk_size-1);
		ref_side = (b_mode_ver ? ref_left : ref_above) + (i_blk_size-1);

		// extend the main reference to the left.
		i_inv_angle_sum    = 128;       // rounding for (shift by 8)
		for (k=-1; k>i_blk_size*i_intra_pred_angle>>5; k--)
		{
			i_inv_angle_sum += i_inv_angle;
			ref_main[k] = ref_side[i_inv_angle_sum>>8];
		}
	}
	else
	{
		memcpy (ref_above, top-1, sizeof(pixel) * (2 * i_blk_size + 1));
		memcpy (ref_left, left-1, sizeof(pixel) * (2 * i_blk_size + 1));
		ref_main = b_mode_ver ? ref_above : ref_left;
		ref_side = b_mode_ver ? ref_left  : ref_above;
	}

	if (i_intra_pred_angle == 0)
	{
		for (k=0;k<i_blk_size;k++)
		{
			for (l=0;l<i_blk_size;l++)
			{
				dst[k*i_dst_stride+l] = ref_main[l+1];
			}
		}

		if ( 0 == i_chroma_idx )
		{
			for (k=0;k<i_blk_size;k++)
			{
				dst[k*i_dst_stride] = x265_clip3_int32(dst[k*i_dst_stride] + (( ref_side[k+1] - ref_side[0] ) >> 1),
														0,
														(1<<8)-1);
			}
		}
	}
	else
	{
		for (k=0;k<i_blk_size;k++)
		{
			i_delta_pos += i_intra_pred_angle;
			i_delta_int   = i_delta_pos >> 5;
			i_delta_fract = i_delta_pos & (32 - 1);

			if (i_delta_fract)
			{
				// do linear filtering
				for (l=0;l<i_blk_size;l++)
				{
					i_ref_main_index        = l+i_delta_int+1;
					dst[k*i_dst_stride+l] = (pixel) ( ((32-i_delta_fract)*ref_main[i_ref_main_index]+i_delta_fract*ref_main[i_ref_main_index+1]+16) >> 5 );
				}
			}
			else
			{
				// just copy the integer samples
				for (l=0;l<i_blk_size;l++)
				{
					dst[k*i_dst_stride+l] = ref_main[l+i_delta_int+1];
				}
			}
		}
	}

	// flip the block if this is the horizontal mode
	if (b_mode_hor)
	{
		for (k=0;k<i_blk_size-1;k++)
		{
			for (l=k+1;l<i_blk_size;l++)
			{
				i_tmp = dst[k*i_dst_stride+l];
				dst[k*i_dst_stride+l] = dst[l*i_dst_stride+k];
				dst[l*i_dst_stride+k] = i_tmp;
			}
		}
	}
}

void x265_intra_pred_angle_3_8_c(pixel *left,
								pixel *top,
								pixel* dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_mode,
								int32_t i_bit_depth)
{
	int32_t i_blk_size = 0 ;
	int32_t k,l;
	// map the mode index to main prediction direction and angle

	int32_t b_mode_dc = 0;
	int32_t b_mode_hor = 0;
	int32_t b_mode_ver = 0;
	int32_t i_intra_pred_angle = 0;
	int32_t i_abs_ang = 0;
	int32_t i_sign_ang = 0;
	int32_t i_inv_angle = 0;
	int32_t i_inv_angle_sum = 0;       // rounding for (shift by 8)
	int32_t i_delta_pos = 0;
	int32_t i_delta_int = 0;
	int32_t i_delta_fract = 0;
	int32_t i_ref_main_index = 0;
	pixel i_tmp = 0;
	// set bitshifts and scale the angle parameter to block size
	int32_t ang_table[9]    = {0,    2,    5,   9,  13,  17,  21,  26,  32};
	int32_t inv_ang_table[9] = {0, 4096, 1638, 910, 630, 482, 390, 315, 256}; // (256 * 32) / angle
	pixel *ref_main;
	pixel *ref_side;
	pixel ref_above[2*X265_MAX_CU_SIZE+1];
	pixel ref_left[2*X265_MAX_CU_SIZE+1];

	// map the mode index to main prediction direction and angle
	i_blk_size = 32 ;
	assert( i_mode > 0 ); //no planar
	b_mode_dc = i_mode < 2;
	b_mode_hor = !b_mode_dc && (i_mode < 18);
	b_mode_ver = !b_mode_dc && !b_mode_hor;
	i_intra_pred_angle = b_mode_ver ? (int32_t)i_mode - X265_VER_IDX : b_mode_hor ? -((int32_t)i_mode - X265_HOR_IDX) : 0;
	i_abs_ang = abs(i_intra_pred_angle);
	i_sign_ang = i_intra_pred_angle < 0 ? -1 : 1;

	// set bitshifts and scale the angle parameter to block size
	i_inv_angle = inv_ang_table[i_abs_ang];
	i_abs_ang = ang_table[i_abs_ang];
	i_intra_pred_angle = i_sign_ang * i_abs_ang;

	if (i_intra_pred_angle < 0)
	{
		memcpy (ref_above + i_blk_size - 1, top-1, sizeof(pixel) * (i_blk_size + 1));
		memcpy (ref_left + i_blk_size - 1, left-1, sizeof(pixel) * (i_blk_size + 1));
		ref_main = (b_mode_ver ? ref_above : ref_left) + (i_blk_size-1);
		ref_side = (b_mode_ver ? ref_left : ref_above) + (i_blk_size-1);

		// extend the main reference to the left.
		i_inv_angle_sum    = 128;       // rounding for (shift by 8)
		for (k=-1; k>i_blk_size*i_intra_pred_angle>>5; k--)
		{
			i_inv_angle_sum += i_inv_angle;
			ref_main[k] = ref_side[i_inv_angle_sum>>8];
		}
	}
	else
	{
		memcpy (ref_above, top-1, sizeof(pixel) * (2 * i_blk_size + 1));
		memcpy (ref_left, left-1, sizeof(pixel) * (2 * i_blk_size + 1));
		ref_main = b_mode_ver ? ref_above : ref_left;
		ref_side = b_mode_ver ? ref_left  : ref_above;
	}

	if (i_intra_pred_angle == 0)
	{
		for (k=0;k<i_blk_size;k++)
		{
			for (l=0;l<i_blk_size;l++)
			{
				dst[k*i_dst_stride+l] = ref_main[l+1];
			}
		}
	}
	else
	{
		for (k=0;k<i_blk_size;k++)
		{
			i_delta_pos += i_intra_pred_angle;
			i_delta_int   = i_delta_pos >> 5;
			i_delta_fract = i_delta_pos & (32 - 1);

			if (i_delta_fract)
			{
				// do linear filtering
				for (l=0;l<i_blk_size;l++)
				{
					i_ref_main_index        = l+i_delta_int+1;
					dst[k*i_dst_stride+l] = (pixel) ( ((32-i_delta_fract)*ref_main[i_ref_main_index]+i_delta_fract*ref_main[i_ref_main_index+1]+16) >> 5 );
				}
			}
			else
			{
				// just copy the integer samples
				for (l=0;l<i_blk_size;l++)
				{
					dst[k*i_dst_stride+l] = ref_main[l+i_delta_int+1];
				}
			}
		}
	}

	// flip the block if this is the horizontal mode
	if (b_mode_hor)
	{
		for (k=0;k<i_blk_size-1;k++)
		{
			for (l=k+1;l<i_blk_size;l++)
			{
				i_tmp = dst[k*i_dst_stride+l];
				dst[k*i_dst_stride+l] = dst[l*i_dst_stride+k];
				dst[l*i_dst_stride+k] = i_tmp;
			}
		}
	}
}

void x265_intra_pred_angle_4_8_c(pixel *left,
								pixel *top,
								pixel* dst,
								int32_t i_dst_stride,
								int32_t i_chroma_idx,
								int32_t i_mode,
								int32_t i_bit_depth)
{
	int32_t i_blk_size = 0 ;
	int32_t k,l;
	// map the mode index to main prediction direction and angle

	int32_t b_mode_dc = 0;
	int32_t b_mode_hor = 0;
	int32_t b_mode_ver = 0;
	int32_t i_intra_pred_angle = 0;
	int32_t i_abs_ang = 0;
	int32_t i_sign_ang = 0;
	int32_t i_inv_angle = 0;
	int32_t i_inv_angle_sum = 0;       // rounding for (shift by 8)
	int32_t i_delta_pos = 0;
	int32_t i_delta_int = 0;
	int32_t i_delta_fract = 0;
	int32_t i_ref_main_index = 0;
	pixel i_tmp = 0;
	// set bitshifts and scale the angle parameter to block size
	int32_t ang_table[9]    = {0,    2,    5,   9,  13,  17,  21,  26,  32};
	int32_t inv_ang_table[9] = {0, 4096, 1638, 910, 630, 482, 390, 315, 256}; // (256 * 32) / angle
	pixel *ref_main;
	pixel *ref_side;
	pixel ref_above[2*X265_MAX_CU_SIZE+1];
	pixel ref_left[2*X265_MAX_CU_SIZE+1];

	// map the mode index to main prediction direction and angle
	i_blk_size = 64 ;
	assert( i_mode > 0 ); //no planar
	b_mode_dc = i_mode < 2;
	b_mode_hor = !b_mode_dc && (i_mode < 18);
	b_mode_ver = !b_mode_dc && !b_mode_hor;
	i_intra_pred_angle = b_mode_ver ? (int32_t)i_mode - X265_VER_IDX : b_mode_hor ? -((int32_t)i_mode - X265_HOR_IDX) : 0;
	i_abs_ang = abs(i_intra_pred_angle);
	i_sign_ang = i_intra_pred_angle < 0 ? -1 : 1;

	// set bitshifts and scale the angle parameter to block size
	i_inv_angle = inv_ang_table[i_abs_ang];
	i_abs_ang = ang_table[i_abs_ang];
	i_intra_pred_angle = i_sign_ang * i_abs_ang;

	if (i_intra_pred_angle < 0)
	{
		memcpy (ref_above + i_blk_size - 1, top-1, sizeof(pixel) * (i_blk_size + 1));
		memcpy (ref_left + i_blk_size - 1, left-1, sizeof(pixel) * (i_blk_size + 1));
		ref_main = (b_mode_ver ? ref_above : ref_left) + (i_blk_size-1);
		ref_side = (b_mode_ver ? ref_left : ref_above) + (i_blk_size-1);

		// extend the main reference to the left.
		i_inv_angle_sum    = 128;       // rounding for (shift by 8)
		for (k=-1; k>i_blk_size*i_intra_pred_angle>>5; k--)
		{
			i_inv_angle_sum += i_inv_angle;
			ref_main[k] = ref_side[i_inv_angle_sum>>8];
		}
	}
	else
	{
		memcpy (ref_above, top-1, sizeof(pixel) * (2 * i_blk_size + 1));
		memcpy (ref_left, left-1, sizeof(pixel) * (2 * i_blk_size + 1));
		ref_main = b_mode_ver ? ref_above : ref_left;
		ref_side = b_mode_ver ? ref_left  : ref_above;
	}

	if (i_intra_pred_angle == 0)
	{
		for (k=0;k<i_blk_size;k++)
		{
			for (l=0;l<i_blk_size;l++)
			{
				dst[k*i_dst_stride+l] = ref_main[l+1];
			}
		}
	}
	else
	{
		for (k=0;k<i_blk_size;k++)
		{
			i_delta_pos += i_intra_pred_angle;
			i_delta_int   = i_delta_pos >> 5;
			i_delta_fract = i_delta_pos & (32 - 1);

			if (i_delta_fract)
			{
				// do linear filtering
				for (l=0;l<i_blk_size;l++)
				{
					i_ref_main_index        = l+i_delta_int+1;
					dst[k*i_dst_stride+l] = (pixel) ( ((32-i_delta_fract)*ref_main[i_ref_main_index]+i_delta_fract*ref_main[i_ref_main_index+1]+16) >> 5 );
				}
			}
			else
			{
				// just copy the integer samples
				for (l=0;l<i_blk_size;l++)
				{
					dst[k*i_dst_stride+l] = ref_main[l+i_delta_int+1];
				}
			}
		}
	}

	// flip the block if this is the horizontal mode
	if (b_mode_hor)
	{
		for (k=0;k<i_blk_size-1;k++)
		{
			for (l=k+1;l<i_blk_size;l++)
			{
				i_tmp = dst[k*i_dst_stride+l];
				dst[k*i_dst_stride+l] = dst[l*i_dst_stride+k];
				dst[l*i_dst_stride+k] = i_tmp;
			}
		}
	}
}

DEFINE_5_INTRA_PRED_PLANAR_FUNCTION_C(c)
DEFINE_5_INTRA_PRED_DC_FUNCTION_C(c)

DEFINE_33_INTRA_PRED_MODE_FUNCTION_C(0_8,c, 4)
DEFINE_33_INTRA_PRED_MODE_FUNCTION_C(1_8,c, 8)
DEFINE_33_INTRA_PRED_MODE_FUNCTION_C(2_8,c,16)
DEFINE_33_INTRA_PRED_MODE_FUNCTION_C(3_8,c,32)
DEFINE_33_INTRA_PRED_MODE_FUNCTION_C(4_8,c,64)

DEFINE_35_INTRA_PRED_MODE_FUNCTION_CMP(0_8,cmp,c,ssse3,4)
DEFINE_35_INTRA_PRED_MODE_FUNCTION_CMP(1_8,cmp,c,ssse3,8)
DEFINE_35_INTRA_PRED_MODE_FUNCTION_CMP(2_8,cmp,c,ssse3,16)
DEFINE_35_INTRA_PRED_MODE_FUNCTION_CMP(3_8,cmp,c,ssse3,32)
DEFINE_35_INTRA_PRED_MODE_FUNCTION_CMP(4_8,cmp,c,ssse3,64)

