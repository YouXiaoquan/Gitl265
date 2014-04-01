

#include "common.h"

#define DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(x) \
	i_temp = org[x] - cur[x]; i_sum += ( i_temp * i_temp ) >> uiShift; \

#define DEFINE_ONE_PIXEL_N_SSES_COMMON_4_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 0) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 1) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 2) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 3) \

#define DEFINE_ONE_PIXEL_N_SSES_COMMON_8_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SSES_COMMON_4_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 4) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 5) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 6) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 7) \

#define DEFINE_ONE_PIXEL_N_SSES_COMMON_12_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SSES_COMMON_8_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 8) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C( 9) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(10) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(11) \

#define DEFINE_ONE_PIXEL_N_SSES_COMMON_16_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SSES_COMMON_12_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(12) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(13) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(14) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(15) \

#define DEFINE_ONE_PIXEL_N_SSES_COMMON_24_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SSES_COMMON_16_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(16) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(17) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(18) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(19) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(20) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(21) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(22) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(23) \

#define DEFINE_ONE_PIXEL_N_SSES_COMMON_32_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SSES_COMMON_24_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(24) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(25) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(26) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(27) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(28) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(29) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(30) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(31) \

#define DEFINE_ONE_PIXEL_N_SSES_COMMON_48_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SSES_COMMON_32_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(32) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(33) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(34) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(35) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(36) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(37) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(38) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(39) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(40) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(41) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(42) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(43) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(44) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(45) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(46) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(47) \

#define DEFINE_ONE_PIXEL_N_SSES_COMMON_64_FUNCTION_C() \
	DEFINE_ONE_PIXEL_N_SSES_COMMON_48_FUNCTION_C() \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(48) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(49) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(50) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(51) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(52) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(53) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(54) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(55) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(56) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(57) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(58) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(59) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(60) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(61) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(62) \
	DEFINE_ONE_PIXEL_ONE_SSES_FUNCTION_C(63) \

#define DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(i_cols, i_rows, src_type, src_type_name, dst_type, dst_type_name) \
uint32_t x265_pixel_sses##_##src_type_name##_##dst_type_name##_##i_cols##x##i_rows##_c(src_type *cur, \
																					uint32_t i_stride_cur, \
																					dst_type *org, \
																					uint32_t i_stride_org, \
																					int32_t i_bit_depth ) \
{ \
	int32_t loop = 0 ; \
	int32_t i_temp = 0 ; \
	uint32_t uiShift = 0 ; \
	uint32_t i_sum = 0; \
\
	uiShift = X265_DISTORTION_PRECISION_ADJUSTMENT((i_bit_depth-8) << 1); \
	for( loop = i_rows; loop != 0; -- loop ) \
	{ \
		DEFINE_ONE_PIXEL_N_SSES_COMMON_##i_cols##_FUNCTION_C() \
\
		org += i_stride_org; \
		cur += i_stride_cur; \
	} \
\
	return i_sum ; \
}


#define DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(i_cols, i_rows, src_type, src_type_name, dst_type, dst_type_name, extent, extent1,extent2) \
uint32_t x265_pixel_sses_##src_type_name##_##dst_type_name##_##i_cols##x##i_rows##_##extent(src_type *cur, \
																					uint32_t i_stride_cur, \
																					dst_type *org, \
																					uint32_t i_stride_org, \
																					int32_t i_bit_depth ) \
{ \
	uint32_t i_sse1 = 0 ; \
	uint32_t i_sse2 = 0 ; \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
\
	i_start_time = x265_get_timer_state () ; \
	i_sse1 = x265_pixel_sses_##src_type_name##_##dst_type_name##_##i_cols##x##i_rows##_##extent1( cur, \
																								i_stride_cur, \
																								org, \
																								i_stride_org, \
																								i_bit_depth ) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
\
	i_start_time = x265_get_timer_state () ; \
	i_sse2 = x265_pixel_sses_##src_type_name##_##dst_type_name##_##i_cols##x##i_rows##_##extent2( cur, \
																								i_stride_cur, \
																								org, \
																								i_stride_org, \
																								i_bit_depth ) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
\
	if ( i_sse1 != i_sse2 ) \
	{ \
		fprintf ( stderr, "x265_pixel_sses" \
				"_" #src_type_name \
				"_" #dst_type_name \
				"_%dx%d:\t%d != %d\n", \
				i_cols, i_rows, i_sse1, i_sse2 ) ; \
		exit(0) ; \
	} \
	\
	return i_sse2 ; \
}

#define DEFINE_ALL_PIXEL_N_SSES_FUNCTION_C(src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 4, 4, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 4, 8, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 4,12, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 4,16, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 4,24, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 4,32, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 4,48, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 4,64, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 8, 4, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 8, 8, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 8,12, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 8,16, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 8,24, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 8,32, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 8,48, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C( 8,64, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(12, 4, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(12, 8, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(12,12, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(12,16, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(12,24, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(12,32, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(12,48, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(12,64, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(16, 4, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(16, 8, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(16,12, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(16,16, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(16,24, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(16,32, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(16,48, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(16,64, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(24, 4, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(24, 8, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(24,12, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(24,16, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(24,24, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(24,32, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(24,48, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(24,64, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(32, 4, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(32, 8, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(32,12, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(32,16, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(32,24, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(32,32, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(32,48, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(32,64, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(48, 4, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(48, 8, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(48,12, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(48,16, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(48,24, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(48,32, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(48,48, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(48,64, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(64, 4, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(64, 8, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(64,12, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(64,16, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(64,24, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(64,32, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(64,48, src_type, src_type_name, dst_type, dst_type_name) \
	DEFINE_ONE_PIXEL_N_SSES_FUNCTION_C(64,64, src_type, src_type_name, dst_type, dst_type_name) \

#define DEFINE_ALL_PIXEL_SSES_FUNCTION_CMP(src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(4, 4, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(4, 8, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(4,12, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(4,16, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(4,24, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(4,32, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(4,48, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(4,64, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(8, 4, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(8, 8, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(8,12, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(8,16, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(8,24, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(8,32, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(8,48, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(8,64, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(12, 4, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(12, 8, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(12,12, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(12,16, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(12,24, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(12,32, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(12,48, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(12,64, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(16, 4, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(16, 8, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(16,12, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(16,16, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(16,24, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(16,32, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(16,48, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(16,64, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(24, 4, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(24, 8, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(24,12, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(24,16, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(24,24, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(24,32, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(24,48, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(24,64, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(32, 4, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(32, 8, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(32,12, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(32,16, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(32,24, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(32,32, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(32,48, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(32,64, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(48, 4, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(48, 8, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(48,12, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(48,16, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(48,24, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(48,32, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(48,48, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(48,64, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(64, 4, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(64, 8, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(64,12, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(64,16, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(64,24, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(64,32, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(64,48, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \
	DEFINE_ONE_PIXEL_SSES_FUNCTION_CMP(64,64, src_type, src_type_name, dst_type, dst_type_name, extent, extent1, extent2) \

#define X265_SSES_INITIALIZE_ALL(sses_func,extent,src_type_name, dst_type_name ) \
	sses_func[ 0] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##4x4_##extent ; \
	sses_func[ 1] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##4x8_##extent ; \
	sses_func[ 2] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##4x12_##extent ; \
	sses_func[ 3] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##4x16_##extent ; \
	sses_func[ 4] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##4x24_##extent ; \
	sses_func[ 5] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##4x32_##extent ; \
	sses_func[ 6] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##4x48_##extent ; \
	sses_func[ 7] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##4x64_##extent ; \
	sses_func[ 8] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##8x4_##extent ; \
	sses_func[ 9] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##8x8_##extent ; \
	sses_func[ 10] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##8x12_##extent ; \
	sses_func[ 11] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##8x16_##extent ; \
	sses_func[ 12] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##8x24_##extent ; \
	sses_func[ 13] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##8x32_##extent ; \
	sses_func[ 14] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##8x48_##extent ; \
	sses_func[ 15] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##8x64_##extent ; \
	sses_func[ 16] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##12x4_##extent ; \
	sses_func[ 17] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##12x8_##extent ; \
	sses_func[ 18] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##12x12_##extent ; \
	sses_func[ 19] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##12x16_##extent ; \
	sses_func[ 20] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##12x24_##extent ; \
	sses_func[ 21] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##12x32_##extent ; \
	sses_func[ 22] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##12x48_##extent ; \
	sses_func[ 23] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##12x64_##extent ; \
	sses_func[ 24] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##16x4_##extent ; \
	sses_func[ 25] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##16x8_##extent ; \
	sses_func[ 26] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##16x12_##extent ; \
	sses_func[ 27] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##16x16_##extent ; \
	sses_func[ 28] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##16x24_##extent ; \
	sses_func[ 29] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##16x32_##extent ; \
	sses_func[ 30] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##16x48_##extent ; \
	sses_func[ 31] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##16x64_##extent ; \
	sses_func[ 32] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##24x4_##extent ; \
	sses_func[ 33] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##24x8_##extent ; \
	sses_func[ 34] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##24x12_##extent ; \
	sses_func[ 35] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##24x16_##extent ; \
	sses_func[ 36] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##24x24_##extent ; \
	sses_func[ 37] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##24x32_##extent ; \
	sses_func[ 38] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##24x48_##extent ; \
	sses_func[ 39] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##24x64_##extent ; \
	sses_func[ 40] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##32x4_##extent ; \
	sses_func[ 41] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##32x8_##extent ; \
	sses_func[ 42] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##32x12_##extent ; \
	sses_func[ 43] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##32x16_##extent ; \
	sses_func[ 44] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##32x24_##extent ; \
	sses_func[ 45] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##32x32_##extent ; \
	sses_func[ 46] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##32x48_##extent ; \
	sses_func[ 47] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##32x64_##extent ; \
	sses_func[ 48] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##48x4_##extent ; \
	sses_func[ 49] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##48x8_##extent ; \
	sses_func[ 50] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##48x12_##extent ; \
	sses_func[ 51] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##48x16_##extent ; \
	sses_func[ 52] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##48x24_##extent ; \
	sses_func[ 53] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##48x32_##extent ; \
	sses_func[ 54] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##48x48_##extent ; \
	sses_func[ 55] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##48x64_##extent ; \
	sses_func[ 56] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##64x4_##extent ; \
	sses_func[ 57] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##64x8_##extent ; \
	sses_func[ 58] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##64x12_##extent ; \
	sses_func[ 59] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##64x16_##extent ; \
	sses_func[ 60] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##64x24_##extent ; \
	sses_func[ 61] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##64x32_##extent ; \
	sses_func[ 62] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##64x48_##extent ; \
	sses_func[ 63] = x265_pixel_sses_##src_type_name##_##dst_type_name##_##64x64_##extent ; \


void x265_sse_initialize ( x265_sses_p_p_func *sse_p_p_func,
							x265_sses_p_s_func *sse_p_s_func,
							x265_sses_s_s_func *sse_s_s_func,
							unsigned int cpu )
{
	X265_SSES_INITIALIZE_ALL(sse_p_p_func, c, p, p)
	X265_SSES_INITIALIZE_ALL(sse_p_s_func, c, p, s)
	X265_SSES_INITIALIZE_ALL(sse_s_s_func, c, s, s)

	if ( cpu & X265_CPU_SSSE3 )
	{
		X265_SSES_INITIALIZE_ALL(sse_p_p_func, ssse3, p, p)
		X265_SSES_INITIALIZE_ALL(sse_p_s_func, ssse3, p, s)
		X265_SSES_INITIALIZE_ALL(sse_s_s_func, ssse3, s, s)
	}
	//	X265_SSES_INITIALIZE_ALL(sse_p_p_func, cmp, p, p)
	//	X265_SSES_INITIALIZE_ALL(sse_p_s_func, cmp, p, s)
	//	X265_SSES_INITIALIZE_ALL(sse_s_s_func, cmp, s, s)
}

DEFINE_ALL_PIXEL_N_SSES_FUNCTION_C(pixel, p, pixel, p)
DEFINE_ALL_PIXEL_N_SSES_FUNCTION_C(pixel, p, short_pixel, s)
DEFINE_ALL_PIXEL_N_SSES_FUNCTION_C(short_pixel, s, short_pixel, s)
DEFINE_ALL_PIXEL_SSES_FUNCTION_CMP(pixel, p, pixel, p, cmp, c, ssse3)
DEFINE_ALL_PIXEL_SSES_FUNCTION_CMP(pixel, p, short_pixel, s, cmp, c, ssse3)
DEFINE_ALL_PIXEL_SSES_FUNCTION_CMP(short_pixel, s, short_pixel, s, cmp, c, ssse3)


