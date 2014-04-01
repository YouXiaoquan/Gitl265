

#include "common.h"



#define DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_C(i_cols, i_rows) \
uint32_t x265_pixel_hads_##i_cols##x##i_rows##_c(pixel *cur, \
											uint32_t i_stride_cur, \
											pixel *org, \
											uint32_t i_stride_org )	\
{ \
	uint32_t i_sum = 0 ; \
	int32_t x = 0 ; \
	int32_t y = 0 ; \
	int32_t i_offset_org = 0 ; \
	int32_t i_offset_cur = 0; \
\
	i_offset_org = i_stride_org<<2; \
    i_offset_cur = i_stride_cur<<2; \
\
    for ( y=0; y<i_rows; y+= 4 ) \
    { \
    	for ( x=0; x<i_cols; x+= 4 ) \
    	{ \
    		i_sum += x265_pixel_hads_4x4_c(&cur[x], \
    										i_stride_cur, \
    										&org[x], \
    										i_stride_org) ; \
    	} \
    	org += i_offset_org; \
    	cur += i_offset_cur; \
    } \
    \
	return i_sum ; \
}

#define DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_C(i_cols, i_rows) \
uint32_t x265_pixel_hads_##i_cols##x##i_rows##_c(pixel *cur, \
											uint32_t i_stride_cur, \
											pixel *org, \
											uint32_t i_stride_org )	\
{ \
	uint32_t i_sum = 0 ; \
	int32_t x = 0 ; \
	int32_t y = 0 ; \
	int32_t i_offset_org = 0 ; \
	int32_t i_offset_cur = 0; \
\
	i_offset_org = i_stride_org<<3; \
    i_offset_cur = i_stride_cur<<3; \
\
    for ( y=0; y<i_rows; y+= 8 ) \
    { \
    	for ( x=0; x<i_cols; x+= 8 ) \
    	{ \
    		i_sum += x265_pixel_hads_8x8_c(&cur[x], \
    										i_stride_cur, \
    										&org[x], \
    										i_stride_org) ; \
    	} \
    	org += i_offset_org; \
    	cur += i_offset_cur; \
    } \
    \
	return i_sum ; \
}

#define DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(i_cols, i_rows, extent, extent1,extent2) \
uint32_t x265_pixel_hads_##i_cols##x##i_rows##_##extent(pixel *cur, \
											uint32_t i_stride_cur, \
											pixel *org, \
											uint32_t i_stride_org ) \
{ \
	uint32_t i_satd1 = 0 ; \
	uint32_t i_satd2 = 0 ; \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
\
	i_start_time = x265_get_timer_state () ; \
	i_satd1 = x265_pixel_hads_##i_cols##x##i_rows##_##extent1( cur, \
																i_stride_cur, \
																org, \
																i_stride_org) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
\
	i_start_time = x265_get_timer_state () ; \
	i_satd2 = x265_pixel_hads_##i_cols##x##i_rows##_##extent2( cur, \
																i_stride_cur, \
																org, \
																i_stride_org) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
\
	if ( i_satd1 != i_satd2 ) \
	{ \
		fprintf ( stderr, "x265_pixel_hads_%dx%d\n", i_cols, i_rows ) ; \
		exit(0) ; \
	} \
	\
	return i_satd2 ; \
}

#define DEFINE_ALL_PIXEL_N_HADS_4x4_FUNCTION(extent) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 4,  8) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 4, 12) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 4, 16) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 4, 24) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 4, 32) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 4, 48) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 4, 64) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 8,  4) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent( 8, 12) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent( 8, 16) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent( 8, 24) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent( 8, 32) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent( 8, 48) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent( 8, 64) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(12,  4) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(12,  8) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(12, 12) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(12, 16) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(12, 24) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(12, 32) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(12, 48) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(12, 64) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(16,  4) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(16,  8) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(16, 12) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(16, 16) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(16, 24) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(16, 32) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(16, 48) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(16, 64) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(24,  4) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(24,  8) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(24, 12) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(24, 16) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(24, 24) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(24, 32) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(24, 48) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(24, 64) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(32,  4) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(32,  8) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(32, 12) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(32, 16) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(32, 24) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(32, 32) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(32, 48) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(32, 64) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(48,  4) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(48,  8) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(48, 12) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(48, 16) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(48, 24) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(48, 32) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(48, 48) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(48, 64) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(64,  4) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(64,  8) \
	DEFINE_ONE_PIXEL_N_HADS_4x4_FUNCTION_##extent(64, 12) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(64, 16) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(64, 24) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(64, 32) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(64, 48) \
	DEFINE_ONE_PIXEL_N_HADS_8x8_FUNCTION_##extent(64, 64) \


#define DEFINE_ALL_PIXEL_HADS_FUNCTION_CMP(extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(4, 4, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(4, 8, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(4, 12, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(4, 16, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(4, 24, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(4, 32, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(4, 48, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(4, 64, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(8, 4, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(8, 8, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(8, 12, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(8, 16, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(8, 24, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(8, 32, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(8, 48, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(8, 64, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(12, 4, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(12, 8, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(12, 12, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(12, 16, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(12, 24, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(12, 32, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(12, 48, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(12, 64, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(16, 4, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(16, 8, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(16, 12, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(16, 16, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(16, 24, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(16, 32, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(16, 48, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(16, 64, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(24, 4, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(24, 8, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(24, 12, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(24, 16, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(24, 24, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(24, 32, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(24, 48, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(24, 64, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(32, 4, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(32, 8, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(32, 12, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(32, 16, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(32, 24, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(32, 32, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(32, 48, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(32, 64, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(48, 4, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(48, 8, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(48, 12, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(48, 16, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(48, 24, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(48, 32, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(48, 48, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(48, 64, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(64, 4, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(64, 8, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(64, 12, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(64, 16, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(64, 24, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(64, 32, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(64, 48, extent, extent1, extent2) \
		DEFINE_ONE_PIXEL_HADS_FUNCTION_CMP(64, 64, extent, extent1, extent2) \

#define X265_SATD_INITIALIZE_ALL(satd_func,extent) \
	satd_func[0] = x265_pixel_hads_4x4_##extent ; \
	satd_func[1] = x265_pixel_hads_4x8_##extent ; \
	satd_func[2] = x265_pixel_hads_4x12_##extent ; \
	satd_func[3] = x265_pixel_hads_4x16_##extent ; \
	satd_func[4] = x265_pixel_hads_4x24_##extent ; \
	satd_func[5] = x265_pixel_hads_4x32_##extent ; \
	satd_func[6] = x265_pixel_hads_4x48_##extent ; \
	satd_func[7] = x265_pixel_hads_4x64_##extent ; \
	satd_func[8] = x265_pixel_hads_8x4_##extent ; \
	satd_func[9] = x265_pixel_hads_8x8_##extent ; \
	satd_func[10] = x265_pixel_hads_8x12_##extent ; \
	satd_func[11] = x265_pixel_hads_8x16_##extent ; \
	satd_func[12] = x265_pixel_hads_8x24_##extent ; \
	satd_func[13] = x265_pixel_hads_8x32_##extent ; \
	satd_func[14] = x265_pixel_hads_8x48_##extent ; \
	satd_func[15] = x265_pixel_hads_8x64_##extent ; \
	satd_func[16] = x265_pixel_hads_12x4_##extent ; \
	satd_func[17] = x265_pixel_hads_12x8_##extent ; \
	satd_func[18] = x265_pixel_hads_12x12_##extent ; \
	satd_func[19] = x265_pixel_hads_12x16_##extent ; \
	satd_func[20] = x265_pixel_hads_12x24_##extent ; \
	satd_func[21] = x265_pixel_hads_12x32_##extent ; \
	satd_func[22] = x265_pixel_hads_12x48_##extent ; \
	satd_func[23] = x265_pixel_hads_12x64_##extent ; \
	satd_func[24] = x265_pixel_hads_16x4_##extent ; \
	satd_func[25] = x265_pixel_hads_16x8_##extent ; \
	satd_func[26] = x265_pixel_hads_16x12_##extent ; \
	satd_func[27] = x265_pixel_hads_16x16_##extent ; \
	satd_func[28] = x265_pixel_hads_16x24_##extent ; \
	satd_func[29] = x265_pixel_hads_16x32_##extent ; \
	satd_func[30] = x265_pixel_hads_16x48_##extent ; \
	satd_func[31] = x265_pixel_hads_16x64_##extent ; \
	satd_func[32] = x265_pixel_hads_24x4_##extent ; \
	satd_func[33] = x265_pixel_hads_24x8_##extent ; \
	satd_func[34] = x265_pixel_hads_24x12_##extent ; \
	satd_func[35] = x265_pixel_hads_24x16_##extent ; \
	satd_func[36] = x265_pixel_hads_24x24_##extent ; \
	satd_func[37] = x265_pixel_hads_24x32_##extent ; \
	satd_func[38] = x265_pixel_hads_24x48_##extent ; \
	satd_func[39] = x265_pixel_hads_24x64_##extent ; \
	satd_func[40] = x265_pixel_hads_32x4_##extent ; \
	satd_func[41] = x265_pixel_hads_32x8_##extent ; \
	satd_func[42] = x265_pixel_hads_32x12_##extent ; \
	satd_func[43] = x265_pixel_hads_32x16_##extent ; \
	satd_func[44] = x265_pixel_hads_32x24_##extent ; \
	satd_func[45] = x265_pixel_hads_32x32_##extent ; \
	satd_func[46] = x265_pixel_hads_32x48_##extent ; \
	satd_func[47] = x265_pixel_hads_32x64_##extent ; \
	satd_func[48] = x265_pixel_hads_48x4_##extent ; \
	satd_func[49] = x265_pixel_hads_48x8_##extent ; \
	satd_func[50] = x265_pixel_hads_48x12_##extent ; \
	satd_func[51] = x265_pixel_hads_48x16_##extent ; \
	satd_func[52] = x265_pixel_hads_48x24_##extent ; \
	satd_func[53] = x265_pixel_hads_48x32_##extent ; \
	satd_func[54] = x265_pixel_hads_48x48_##extent ; \
	satd_func[55] = x265_pixel_hads_48x64_##extent ; \
	satd_func[56] = x265_pixel_hads_64x4_##extent ; \
	satd_func[57] = x265_pixel_hads_64x8_##extent ; \
	satd_func[58] = x265_pixel_hads_64x12_##extent ; \
	satd_func[59] = x265_pixel_hads_64x16_##extent ; \
	satd_func[60] = x265_pixel_hads_64x24_##extent ; \
	satd_func[61] = x265_pixel_hads_64x32_##extent ; \
	satd_func[62] = x265_pixel_hads_64x48_##extent ; \
	satd_func[63] = x265_pixel_hads_64x64_##extent ; \

static int8_t psize[16] = {  0,  1,  2,  3, -1,  4, -1, 5,
                            -1, -1, -1,  6, -1, -1, -1, 7 };

int PartitionFromSizes(int width, int height)
{
    int8_t w = psize[(width >> 2) - 1];
    int8_t h = psize[(height >> 2) - 1];

    return (w << 3) + h;
}


void x265_satd_initialize ( x265_satd_func *satd_func, unsigned int cpu )
{
	X265_SATD_INITIALIZE_ALL(satd_func,c)

	if ( cpu & X265_CPU_SSSE3 )
	{
		X265_SATD_INITIALIZE_ALL(satd_func,ssse3)
	}
}

uint32_t x265_pixel_hads_4x4_c(pixel *cur,
								uint32_t i_stride_cur,
								pixel *org,
								uint32_t i_stride_org )
{
	int32_t k, satd = 0, diff[16], m[16], d[16];

	for( k = 0; k < 16; k+=4 )
	{
		diff[k+0] = org[0] - cur[0];
		diff[k+1] = org[1] - cur[1];
		diff[k+2] = org[2] - cur[2];
		diff[k+3] = org[3] - cur[3];

		cur += i_stride_cur;
		org += i_stride_org;
	}

	/*===== hadamard transform =====*/
	m[ 0] = diff[ 0] + diff[12];
	m[ 1] = diff[ 1] + diff[13];
	m[ 2] = diff[ 2] + diff[14];
	m[ 3] = diff[ 3] + diff[15];
	m[ 4] = diff[ 4] + diff[ 8];
	m[ 5] = diff[ 5] + diff[ 9];
	m[ 6] = diff[ 6] + diff[10];
	m[ 7] = diff[ 7] + diff[11];
	m[ 8] = diff[ 4] - diff[ 8];
	m[ 9] = diff[ 5] - diff[ 9];
	m[10] = diff[ 6] - diff[10];
	m[11] = diff[ 7] - diff[11];
	m[12] = diff[ 0] - diff[12];
	m[13] = diff[ 1] - diff[13];
	m[14] = diff[ 2] - diff[14];
	m[15] = diff[ 3] - diff[15];

	d[ 0] = m[ 0] + m[ 4];
	d[ 1] = m[ 1] + m[ 5];
	d[ 2] = m[ 2] + m[ 6];
	d[ 3] = m[ 3] + m[ 7];
	d[ 4] = m[ 8] + m[12];
	d[ 5] = m[ 9] + m[13];
	d[ 6] = m[10] + m[14];
	d[ 7] = m[11] + m[15];
	d[ 8] = m[ 0] - m[ 4];
	d[ 9] = m[ 1] - m[ 5];
	d[10] = m[ 2] - m[ 6];
	d[11] = m[ 3] - m[ 7];
	d[12] = m[12] - m[ 8];
	d[13] = m[13] - m[ 9];
	d[14] = m[14] - m[10];
	d[15] = m[15] - m[11];

	m[ 0] = d[ 0] + d[ 3];
	m[ 1] = d[ 1] + d[ 2];
	m[ 2] = d[ 1] - d[ 2];
	m[ 3] = d[ 0] - d[ 3];
	m[ 4] = d[ 4] + d[ 7];
	m[ 5] = d[ 5] + d[ 6];
	m[ 6] = d[ 5] - d[ 6];
	m[ 7] = d[ 4] - d[ 7];
	m[ 8] = d[ 8] + d[11];
	m[ 9] = d[ 9] + d[10];
	m[10] = d[ 9] - d[10];
	m[11] = d[ 8] - d[11];
	m[12] = d[12] + d[15];
	m[13] = d[13] + d[14];
	m[14] = d[13] - d[14];
	m[15] = d[12] - d[15];

	d[ 0] = m[ 0] + m[ 1];
	d[ 1] = m[ 0] - m[ 1];
	d[ 2] = m[ 2] + m[ 3];
	d[ 3] = m[ 3] - m[ 2];
	d[ 4] = m[ 4] + m[ 5];
	d[ 5] = m[ 4] - m[ 5];
	d[ 6] = m[ 6] + m[ 7];
	d[ 7] = m[ 7] - m[ 6];
	d[ 8] = m[ 8] + m[ 9];
	d[ 9] = m[ 8] - m[ 9];
	d[10] = m[10] + m[11];
	d[11] = m[11] - m[10];
	d[12] = m[12] + m[13];
	d[13] = m[12] - m[13];
	d[14] = m[14] + m[15];
	d[15] = m[15] - m[14];

	for (k=0; k<16; ++k)
	{
		satd += abs(d[k]);
	}
	satd = ((satd+1)>>1);

	return satd;
}

uint32_t x265_pixel_hads_8x8_c(pixel *cur,
								uint32_t i_stride_cur,
								pixel *org,
								uint32_t i_stride_org )
{
	int32_t k, i, j, jj, sad=0;
	int32_t diff[64], m1[8][8], m2[8][8], m3[8][8];

	for( k = 0; k < 64; k += 8 )
	{
		diff[k+0] = org[0] - cur[0];
		diff[k+1] = org[1] - cur[1];
		diff[k+2] = org[2] - cur[2];
		diff[k+3] = org[3] - cur[3];
		diff[k+4] = org[4] - cur[4];
		diff[k+5] = org[5] - cur[5];
		diff[k+6] = org[6] - cur[6];
		diff[k+7] = org[7] - cur[7];

		cur += i_stride_cur;
		org += i_stride_org;
	}

	//horizontal
	for (j=0; j < 8; j++)
	{
		jj = j << 3;
		m2[j][0] = diff[jj  ] + diff[jj+4];
		m2[j][1] = diff[jj+1] + diff[jj+5];
		m2[j][2] = diff[jj+2] + diff[jj+6];
		m2[j][3] = diff[jj+3] + diff[jj+7];
		m2[j][4] = diff[jj  ] - diff[jj+4];
		m2[j][5] = diff[jj+1] - diff[jj+5];
		m2[j][6] = diff[jj+2] - diff[jj+6];
		m2[j][7] = diff[jj+3] - diff[jj+7];

		m1[j][0] = m2[j][0] + m2[j][2];
		m1[j][1] = m2[j][1] + m2[j][3];
		m1[j][2] = m2[j][0] - m2[j][2];
		m1[j][3] = m2[j][1] - m2[j][3];
		m1[j][4] = m2[j][4] + m2[j][6];
		m1[j][5] = m2[j][5] + m2[j][7];
		m1[j][6] = m2[j][4] - m2[j][6];
		m1[j][7] = m2[j][5] - m2[j][7];

		m2[j][0] = m1[j][0] + m1[j][1];
		m2[j][1] = m1[j][0] - m1[j][1];
		m2[j][2] = m1[j][2] + m1[j][3];
		m2[j][3] = m1[j][2] - m1[j][3];
		m2[j][4] = m1[j][4] + m1[j][5];
		m2[j][5] = m1[j][4] - m1[j][5];
		m2[j][6] = m1[j][6] + m1[j][7];
		m2[j][7] = m1[j][6] - m1[j][7];
	}

	//vertical
	for (i=0; i < 8; i++)
	{
		m3[0][i] = m2[0][i] + m2[4][i];
		m3[1][i] = m2[1][i] + m2[5][i];
		m3[2][i] = m2[2][i] + m2[6][i];
		m3[3][i] = m2[3][i] + m2[7][i];
		m3[4][i] = m2[0][i] - m2[4][i];
		m3[5][i] = m2[1][i] - m2[5][i];
		m3[6][i] = m2[2][i] - m2[6][i];
		m3[7][i] = m2[3][i] - m2[7][i];

		m1[0][i] = m3[0][i] + m3[2][i];
		m1[1][i] = m3[1][i] + m3[3][i];
		m1[2][i] = m3[0][i] - m3[2][i];
		m1[3][i] = m3[1][i] - m3[3][i];
		m1[4][i] = m3[4][i] + m3[6][i];
		m1[5][i] = m3[5][i] + m3[7][i];
		m1[6][i] = m3[4][i] - m3[6][i];
		m1[7][i] = m3[5][i] - m3[7][i];

		m2[0][i] = m1[0][i] + m1[1][i];
		m2[1][i] = m1[0][i] - m1[1][i];
		m2[2][i] = m1[2][i] + m1[3][i];
		m2[3][i] = m1[2][i] - m1[3][i];
		m2[4][i] = m1[4][i] + m1[5][i];
		m2[5][i] = m1[4][i] - m1[5][i];
		m2[6][i] = m1[6][i] + m1[7][i];
		m2[7][i] = m1[6][i] - m1[7][i];
	}

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			sad += abs(m2[i][j]);
		}
	}

	sad=((sad+2)>>2);
	return sad;
}


DEFINE_ALL_PIXEL_N_HADS_4x4_FUNCTION(C)
DEFINE_ALL_PIXEL_HADS_FUNCTION_CMP(cmp, c, ssse3)

