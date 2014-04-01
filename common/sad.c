

#include "common.h"

#define DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(x) \
	i_sum += abs( org[x] - cur[x] ); \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_4_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 0) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 1) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 2) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 3) \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_8_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_4_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 4) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 5) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 6) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 7) \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_12_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_8_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 8) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C( 9) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(10) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(11) \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_16_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_12_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(12) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(13) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(14) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(15) \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_24_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_16_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(16) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(17) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(18) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(19) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(20) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(21) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(22) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(23) \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_32_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_24_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(24) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(25) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(26) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(27) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(28) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(29) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(30) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(31) \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_48_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_32_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(32) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(33) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(34) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(35) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(36) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(37) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(38) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(39) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(40) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(41) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(42) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(43) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(44) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(45) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(46) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(47) \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_64_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_48_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(48) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(49) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(50) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(51) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(52) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(53) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(54) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(55) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(56) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(57) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(58) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(59) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(60) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(61) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(62) \
	DEFINE_ONE_PIXEL_ONE_SADS_FUNCTION_C(63) \

#define DEFINE_ONE_PIXEL_N_SADS_COMMON_FUNCTION_C(i_cols, i_rows, sub_step) \
	int32_t loop = 0 ; \
	int32_t  i_sub_step   = ( 1 << sub_step ); \
	int32_t  stride_cur = i_stride_cur*i_sub_step; \
	int32_t  stride_org = i_stride_org*i_sub_step; \
\
	uint32_t i_sum = 0; \
\
	for( loop = i_rows; loop != 0; loop -= i_sub_step ) \
	{ \
		DEFINE_ONE_PIXEL_N_SADS_COMMON_##i_cols##_FUNCTION_C() \
\
		org += stride_org; \
		cur += stride_cur; \
	} \
\
	i_sum <<= sub_step; \
	return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(i_bit_depth-8); \


#define DEFINE_ONE_PIXEL_N_SADS_FUNCTION_C(i_cols, i_rows) \
uint32_t x265_pixel_sads_##i_cols##x##i_rows##_c(pixel *cur, \
											uint32_t i_stride_cur, \
											pixel *org, \
											uint32_t i_stride_org, \
											int32_t i_bit_depth ) \
{ \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_FUNCTION_C(i_cols, i_rows, 0) \
}

#define DEFINE_ONE_PIXEL_N_SADS_HALF_FUNCTION_C(i_cols, i_rows) \
uint32_t x265_pixel_sads_half_##i_cols##x##i_rows##_c(pixel *cur, \
											uint32_t i_stride_cur, \
											pixel *org, \
											uint32_t i_stride_org, \
											int32_t i_bit_depth ) \
{ \
	DEFINE_ONE_PIXEL_N_SADS_COMMON_FUNCTION_C(i_cols, i_rows, 1) \
}

#define DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(i_cols, i_rows, type, extent, extent1,extent2) \
uint32_t x265_pixel_sads##type##i_cols##x##i_rows##_##extent(pixel *cur, \
															uint32_t i_stride_cur, \
															pixel *org, \
															uint32_t i_stride_org, \
															int32_t i_bit_depth ) \
{ \
	uint32_t i_sad1 = 0 ; \
	uint32_t i_sad2 = 0 ; \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
\
	i_start_time = x265_get_timer_state () ; \
	i_sad1 = x265_pixel_sads##type##i_cols##x##i_rows##_##extent1( cur, \
																i_stride_cur, \
																org, \
																i_stride_org, \
																i_bit_depth ) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
\
	i_start_time = x265_get_timer_state () ; \
	i_sad2 = x265_pixel_sads##type##i_cols##x##i_rows##_##extent2( cur, \
																i_stride_cur, \
																org, \
																i_stride_org, \
																i_bit_depth ) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
\
	if ( i_sad1 != i_sad2 ) \
	{ \
		fprintf ( stderr, "x265_pixel_sads_half_%dx%d:\t%d != %d\n", \
				i_cols, i_rows, i_sad1, i_sad2 ) ; \
		exit(0) ; \
	} \
	\
	return i_sad2 ; \
}

#define DEFINE_ALL_PIXEL_N_SADS_FUNCTION_C(TYPE) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 4, 4) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 4, 8) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 4,12) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 4,16) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 4,24) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 4,32) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 4,48) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 4,64) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 8, 4) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 8, 8) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 8,12) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 8,16) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 8,24) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 8,32) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 8,48) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C( 8,64) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(12, 4) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(12, 8) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(12,12) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(12,16) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(12,24) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(12,32) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(12,48) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(12,64) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(16, 4) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(16, 8) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(16,12) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(16,16) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(16,24) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(16,32) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(16,48) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(16,64) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(24, 4) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(24, 8) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(24,12) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(24,16) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(24,24) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(24,32) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(24,48) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(24,64) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(32, 4) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(32, 8) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(32,12) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(32,16) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(32,24) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(32,32) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(32,48) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(32,64) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(48, 4) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(48, 8) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(48,12) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(48,16) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(48,24) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(48,32) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(48,48) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(48,64) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(64, 4) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(64, 8) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(64,12) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(64,16) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(64,24) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(64,32) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(64,48) \
	DEFINE_ONE_PIXEL_N_SADS##TYPE##FUNCTION_C(64,64) \

#define DEFINE_ALL_PIXEL_SADS_FUNCTION_CMP(type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(4, 4, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(4, 8, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(4,12, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(4,16, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(4,24, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(4,32, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(4,48, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(4,64, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(8, 4, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(8, 8, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(8,12, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(8,16, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(8,24, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(8,32, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(8,48, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(8,64, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(12, 4, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(12, 8, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(12,12, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(12,16, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(12,24, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(12,32, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(12,48, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(12,64, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(16, 4, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(16, 8, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(16,12, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(16,16, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(16,24, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(16,32, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(16,48, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(16,64, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(24, 4, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(24, 8, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(24,12, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(24,16, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(24,24, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(24,32, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(24,48, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(24,64, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(32, 4, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(32, 8, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(32,12, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(32,16, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(32,24, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(32,32, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(32,48, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(32,64, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(48, 4, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(48, 8, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(48,12, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(48,16, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(48,24, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(48,32, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(48,48, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(48,64, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(64, 4, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(64, 8, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(64,12, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(64,16, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(64,24, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(64,32, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(64,48, type, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SADS_FUNCTION_CMP(64,64, type, extent, extent1, extent2) \

#define X265_SADS_INITIALIZE_ALL(sads_func,extent,type) \
	sads_func[ 0] = x265_pixel_sads##type##4x4_##extent ; \
	sads_func[ 1] = x265_pixel_sads##type##4x8_##extent ; \
	sads_func[ 2] = x265_pixel_sads##type##4x12_##extent ; \
	sads_func[ 3] = x265_pixel_sads##type##4x16_##extent ; \
	sads_func[ 4] = x265_pixel_sads##type##4x24_##extent ; \
	sads_func[ 5] = x265_pixel_sads##type##4x32_##extent ; \
	sads_func[ 6] = x265_pixel_sads##type##4x48_##extent ; \
	sads_func[ 7] = x265_pixel_sads##type##4x64_##extent ; \
	sads_func[ 8] = x265_pixel_sads##type##8x4_##extent ; \
	sads_func[ 9] = x265_pixel_sads##type##8x8_##extent ; \
	sads_func[ 10] = x265_pixel_sads##type##8x12_##extent ; \
	sads_func[ 11] = x265_pixel_sads##type##8x16_##extent ; \
	sads_func[ 12] = x265_pixel_sads##type##8x24_##extent ; \
	sads_func[ 13] = x265_pixel_sads##type##8x32_##extent ; \
	sads_func[ 14] = x265_pixel_sads##type##8x48_##extent ; \
	sads_func[ 15] = x265_pixel_sads##type##8x64_##extent ; \
	sads_func[ 16] = x265_pixel_sads##type##12x4_##extent ; \
	sads_func[ 17] = x265_pixel_sads##type##12x8_##extent ; \
	sads_func[ 18] = x265_pixel_sads##type##12x12_##extent ; \
	sads_func[ 19] = x265_pixel_sads##type##12x16_##extent ; \
	sads_func[ 20] = x265_pixel_sads##type##12x24_##extent ; \
	sads_func[ 21] = x265_pixel_sads##type##12x32_##extent ; \
	sads_func[ 22] = x265_pixel_sads##type##12x48_##extent ; \
	sads_func[ 23] = x265_pixel_sads##type##12x64_##extent ; \
	sads_func[ 24] = x265_pixel_sads##type##16x4_##extent ; \
	sads_func[ 25] = x265_pixel_sads##type##16x8_##extent ; \
	sads_func[ 26] = x265_pixel_sads##type##16x12_##extent ; \
	sads_func[ 27] = x265_pixel_sads##type##16x16_##extent ; \
	sads_func[ 28] = x265_pixel_sads##type##16x24_##extent ; \
	sads_func[ 29] = x265_pixel_sads##type##16x32_##extent ; \
	sads_func[ 30] = x265_pixel_sads##type##16x48_##extent ; \
	sads_func[ 31] = x265_pixel_sads##type##16x64_##extent ; \
	sads_func[ 32] = x265_pixel_sads##type##24x4_##extent ; \
	sads_func[ 33] = x265_pixel_sads##type##24x8_##extent ; \
	sads_func[ 34] = x265_pixel_sads##type##24x12_##extent ; \
	sads_func[ 35] = x265_pixel_sads##type##24x16_##extent ; \
	sads_func[ 36] = x265_pixel_sads##type##24x24_##extent ; \
	sads_func[ 37] = x265_pixel_sads##type##24x32_##extent ; \
	sads_func[ 38] = x265_pixel_sads##type##24x48_##extent ; \
	sads_func[ 39] = x265_pixel_sads##type##24x64_##extent ; \
	sads_func[ 40] = x265_pixel_sads##type##32x4_##extent ; \
	sads_func[ 41] = x265_pixel_sads##type##32x8_##extent ; \
	sads_func[ 42] = x265_pixel_sads##type##32x12_##extent ; \
	sads_func[ 43] = x265_pixel_sads##type##32x16_##extent ; \
	sads_func[ 44] = x265_pixel_sads##type##32x24_##extent ; \
	sads_func[ 45] = x265_pixel_sads##type##32x32_##extent ; \
	sads_func[ 46] = x265_pixel_sads##type##32x48_##extent ; \
	sads_func[ 47] = x265_pixel_sads##type##32x64_##extent ; \
	sads_func[ 48] = x265_pixel_sads##type##48x4_##extent ; \
	sads_func[ 49] = x265_pixel_sads##type##48x8_##extent ; \
	sads_func[ 50] = x265_pixel_sads##type##48x12_##extent ; \
	sads_func[ 51] = x265_pixel_sads##type##48x16_##extent ; \
	sads_func[ 52] = x265_pixel_sads##type##48x24_##extent ; \
	sads_func[ 53] = x265_pixel_sads##type##48x32_##extent ; \
	sads_func[ 54] = x265_pixel_sads##type##48x48_##extent ; \
	sads_func[ 55] = x265_pixel_sads##type##48x64_##extent ; \
	sads_func[ 56] = x265_pixel_sads##type##64x4_##extent ; \
	sads_func[ 57] = x265_pixel_sads##type##64x8_##extent ; \
	sads_func[ 58] = x265_pixel_sads##type##64x12_##extent ; \
	sads_func[ 59] = x265_pixel_sads##type##64x16_##extent ; \
	sads_func[ 60] = x265_pixel_sads##type##64x24_##extent ; \
	sads_func[ 61] = x265_pixel_sads##type##64x32_##extent ; \
	sads_func[ 62] = x265_pixel_sads##type##64x48_##extent ; \
	sads_func[ 63] = x265_pixel_sads##type##64x64_##extent ; \

void x265_sad_initialize(x265_sads_func *sads_func,
						x265_sads_func *sads_half_func,
						unsigned int cpu )
{
	X265_SADS_INITIALIZE_ALL(sads_func,      c,      _)
	X265_SADS_INITIALIZE_ALL(sads_half_func, c, _half_)

	if ( cpu & X265_CPU_SSSE3 )
	{
		X265_SADS_INITIALIZE_ALL(sads_func,      ssse3,      _)
		X265_SADS_INITIALIZE_ALL(sads_half_func, ssse3, _half_)
	}
	//	X265_SADS_INITIALIZE_ALL(sads_func,      cmp,      _)
	//	X265_SADS_INITIALIZE_ALL(sads_half_func, cmp, _half_)
}

DEFINE_ALL_PIXEL_SADS_FUNCTION_CMP(_half_, cmp, c, ssse3)
DEFINE_ALL_PIXEL_SADS_FUNCTION_CMP(_, cmp, c, ssse3)
DEFINE_ALL_PIXEL_N_SADS_FUNCTION_C(_)
DEFINE_ALL_PIXEL_N_SADS_FUNCTION_C(_HALF_)
