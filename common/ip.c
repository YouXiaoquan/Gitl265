

#include "common.h"
#include "ip.h"


//! \ingroup tlib_common
//! \{

// ====================================================================================================================
// tables
// ====================================================================================================================

int16_t luma_filter[4][X265_NTAPS_LUMA] =
{
	{  0, 0,   0, 64,  0,   0, 0,  0 },
	{ -1, 4, -10, 58, 17,  -5, 1,  0 },
	{ -1, 4, -11, 40, 40, -11, 4, -1 },
	{  0, 1,  -5, 17, 58, -10, 4, -1 }
};

int16_t chroma_filter[8][X265_NTAPS_CHROMA] =
{
	{  0, 64,  0,  0 },
	{ -2, 58, 10, -2 },
	{ -4, 54, 16, -2 },
	{ -6, 46, 28, -4 },
	{ -4, 36, 36, -4 },
	{ -4, 28, 46, -6 },
	{ -2, 16, 54, -4 },
	{ -2, 10, 58, -2 }
};



#define X265_IP_FILTER_LUMA_INITIALIZE(ip,dir,src_type_name,dst_type_name,extent) \
	ip->ip_filter_##dir##_luma_##src_type_name##_##dst_type_name[0] = x265_ip_filter_##dir##_luma_##src_type_name##_##dst_type_name##_0_##extent ; \
	ip->ip_filter_##dir##_luma_##src_type_name##_##dst_type_name[1] = x265_ip_filter_##dir##_luma_##src_type_name##_##dst_type_name##_1_##extent ; \
	ip->ip_filter_##dir##_luma_##src_type_name##_##dst_type_name[2] = x265_ip_filter_##dir##_luma_##src_type_name##_##dst_type_name##_2_##extent ; \
	ip->ip_filter_##dir##_luma_##src_type_name##_##dst_type_name[3] = x265_ip_filter_##dir##_luma_##src_type_name##_##dst_type_name##_3_##extent ; \

#define X265_IP_FILTER_CHROMA_INITIALIZE(ip,dir,src_type_name,dst_type_name,extent) \
	ip->ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name[0] = x265_ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name##_0_##extent ; \
	ip->ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name[1] = x265_ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name##_1_##extent ; \
	ip->ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name[2] = x265_ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name##_2_##extent ; \
	ip->ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name[3] = x265_ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name##_3_##extent ; \
	ip->ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name[4] = x265_ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name##_4_##extent ; \
	ip->ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name[5] = x265_ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name##_5_##extent ; \
	ip->ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name[6] = x265_ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name##_6_##extent ; \
	ip->ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name[7] = x265_ip_filter_##dir##_chroma_##src_type_name##_##dst_type_name##_7_##extent ; \

#define DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,frac,extent,extent1,extent2) \
	void x265_ip_filter_##dir##_##chroma##_##src_type_name##_##dst_type_name##_##frac##_##extent(\
														src_type *ref, \
														int32_t i_ref_stride, \
														dst_type *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
	dst_type *dst1 = NULL ; \
	dst_type *dst2 = NULL ; \
\
	dst1 = dst ; \
	NOCHECKED_MALLOCZERO(dst2, sizeof(dst_type)*i_width*i_height ) ; \
\
	i_start_time = x265_get_timer_state () ; \
	x265_ip_filter_##dir##_##chroma##_##src_type_name##_##dst_type_name##_##frac##_##extent1(\
										ref, \
										i_ref_stride, \
										dst1, \
										i_dst_stride, \
										i_width, \
										i_height, \
										i_bit_depth) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
\
	i_start_time = x265_get_timer_state () ; \
	x265_ip_filter_##dir##_##chroma##_##src_type_name##_##dst_type_name##_##frac##_##extent2( \
										ref, \
										i_ref_stride, \
										dst2, \
										i_width, \
										i_width, \
										i_height, \
										i_bit_depth) ; \
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
\
	if (memory_compare_##dst_type_name(dst1, i_dst_stride, dst2, i_width, i_width, i_height ) ) \
	{ \
		fprintf ( stderr, "x265_ip_filter_" #dir \
					"_" #chroma \
					"_" #src_type_name \
					"_" #dst_type_name \
					"_%d_ssse3 error: Pixel \t%d\t%d", \
				frac, \
				i_width, \
				i_height ) ; \
		exit(0) ; \
	} \
	x265_free (dst2) ; \
}


#define DECLARE_IP_FILTER_COMMON_DATA \
	int32_t i_row, i_col; \
	int32_t i_stride = 0; \
	int32_t i_shift = 0; \
	int32_t i_sum = 0; \
	int16_t i_val = 0; \

#define INIT_IP_FILTER_LUMA_COMMON_DATA(frac) \
	c[0] = luma_filter[frac][0]; \
	c[1] = luma_filter[frac][1]; \
	c[2] = luma_filter[frac][2]; \
	c[3] = luma_filter[frac][3]; \
	c[4] = luma_filter[frac][4]; \
	c[5] = luma_filter[frac][5]; \
	c[6] = luma_filter[frac][6]; \
	c[7] = luma_filter[frac][7]; \
\
	ref -= 3 * i_stride; \

#define INIT_IP_FILTER_CHROMA_COMMON_DATA(frac) \
	c[0] = chroma_filter[frac][0]; \
	c[1] = chroma_filter[frac][1]; \
	c[2] = chroma_filter[frac][2]; \
	c[3] = chroma_filter[frac][3]; \
\
	ref -= i_stride; \

#define IP_FILTER_LUMA \
	i_sum = ref[ i_col + 0 * i_stride] * c[0]; \
	i_sum += ref[ i_col + 1 * i_stride] * c[1]; \
	i_sum += ref[ i_col + 2 * i_stride] * c[2]; \
	i_sum += ref[ i_col + 3 * i_stride] * c[3]; \
	i_sum += ref[ i_col + 4 * i_stride] * c[4]; \
	i_sum += ref[ i_col + 5 * i_stride] * c[5]; \
	i_sum += ref[ i_col + 6 * i_stride] * c[6]; \
	i_sum += ref[ i_col + 7 * i_stride] * c[7]; \

#define IP_FILTER_CHROMA \
	i_sum = ref[ i_col + 0 * i_stride] * c[0]; \
	i_sum += ref[ i_col + 1 * i_stride] * c[1]; \
	i_sum += ref[ i_col + 2 * i_stride] * c[2]; \
	i_sum += ref[ i_col + 3 * i_stride] * c[3]; \

#define DEFINE_IP_FILTER_COPY_P_P_C_FUNC(ver,chroma,frac,extent) \
	void x265_ip_filter_##ver##_##chroma##_p_p_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	int32_t i_row, i_col; \
\
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			dst[i_col] = ref[i_col]; \
		} \
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
}

#define DEFINE_IP_FILTER_COPY_P_S_C_FUNC(ver,chroma,frac,extent) \
	void x265_ip_filter_##ver##_##chroma##_p_s_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														short_pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	int32_t i_row, i_col; \
    int16_t i_val = 0; \
	int32_t i_shift = 0; \
\
	i_shift = X265_IF_INTERNAL_PREC - i_bit_depth; \
\
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			i_val = ref[i_col] << i_shift; \
			dst[i_col] = i_val - (int16_t)X265_IF_INTERNAL_OFFS; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
}


#define DEFINE_IP_FILTER_COPY_S_P_C_FUNC(ver,chroma,frac,extent) \
	void x265_ip_filter_##ver##_##chroma##_s_p_##frac##_##extent(short_pixel *ref, \
														int32_t i_ref_stride, \
														pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	int32_t i_row, i_col; \
    int16_t i_val = 0; \
	int32_t i_shift = 0; \
	int16_t i_offset = 0; \
	int16_t i_max_val = 0; \
	int16_t i_min_val = 0; \
\
	i_shift = X265_IF_INTERNAL_PREC - i_bit_depth; \
	i_offset = X265_IF_INTERNAL_OFFS; \
	i_offset += i_shift ? (1 << (i_shift - 1)):0; \
	i_max_val = (1 << i_bit_depth) - 1; \
	i_min_val = 0; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			i_val = ref[ i_col ]; \
			i_val = ( i_val + i_offset ) >> i_shift; \
			if (i_val < i_min_val) \
			{ \
				i_val = i_min_val; \
			} \
			if (i_val > i_max_val) \
			{ \
				i_val = i_max_val; \
			} \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
}


#define DEFINE_IP_FILTER_COPY_S_S_C_FUNC(ver,chroma,frac,extent) \
	void x265_ip_filter_##ver##_##chroma##_s_s_##frac##_##extent(short_pixel *ref, \
														int32_t i_ref_stride, \
														short_pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	int32_t i_row, i_col; \
\
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			dst[i_col] = ref[i_col]; \
		} \
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
}

#define DEFINE_IP_FILTER_COPY_HOR_LUMA_P_P_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_P_P_C_FUNC(hor,luma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_VER_LUMA_P_P_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_P_P_C_FUNC(ver,luma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_HOR_LUMA_P_S_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_P_S_C_FUNC(hor,luma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_VER_LUMA_P_S_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_P_S_C_FUNC(ver,luma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_VER_LUMA_S_P_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_S_P_C_FUNC(ver,luma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_VER_LUMA_S_S_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_S_S_C_FUNC(ver,luma,frac,extent) \


#define DEFINE_IP_FILTER_COPY_HOR_CHROMA_P_P_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_P_P_C_FUNC(hor,chroma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_VER_CHROMA_P_P_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_P_P_C_FUNC(ver,chroma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_HOR_CHROMA_P_S_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_P_S_C_FUNC(hor,chroma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_VER_CHROMA_P_S_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_P_S_C_FUNC(ver,chroma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_VER_CHROMA_S_P_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_S_P_C_FUNC(ver,chroma,frac,extent) \

#define DEFINE_IP_FILTER_COPY_VER_CHROMA_S_S_C_FUNC(frac,extent) \
	DEFINE_IP_FILTER_COPY_S_S_C_FUNC(ver,chroma,frac,extent) \


#define DEFINE_IP_FILTER_HOR_LUMA_P_P_C_FUNC(frac,extent) \
	void x265_ip_filter_hor_luma_p_p_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[8]; \
	int32_t i_offset; \
	int16_t i_max_val; \
\
	i_stride = 1 ; \
	i_shift = X265_IF_FILTER_PREC; \
	i_offset = 1 << (i_shift - 1); \
	i_max_val = (1 << i_bit_depth) - 1; \
\
	INIT_IP_FILTER_LUMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_LUMA ; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			i_val = ( i_val < 0 ) ? 0 : i_val; \
			i_val = ( i_val > i_max_val ) ? i_max_val : i_val; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_HOR_LUMA_P_S_C_FUNC(frac,extent) \
	void x265_ip_filter_hor_luma_p_s_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														short_pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[8]; \
	int32_t i_offset; \
\
	i_stride = 1 ; \
	i_shift = X265_IF_FILTER_PREC + i_bit_depth - X265_IF_INTERNAL_PREC ; \
	i_offset = -X265_IF_INTERNAL_OFFS << i_shift; \
\
	INIT_IP_FILTER_LUMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_LUMA ; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_VER_LUMA_P_P_C_FUNC(frac,extent) \
	void x265_ip_filter_ver_luma_p_p_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[8]; \
	int32_t i_offset; \
	int16_t i_max_val; \
\
	i_stride = i_ref_stride ; \
	i_shift = X265_IF_FILTER_PREC; \
	i_offset = 1 << (i_shift - 1); \
	i_max_val = (1 << i_bit_depth) - 1; \
\
	INIT_IP_FILTER_LUMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_LUMA ; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			i_val = ( i_val < 0 ) ? 0 : i_val; \
			i_val = ( i_val > i_max_val ) ? i_max_val : i_val; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_VER_LUMA_P_S_C_FUNC(frac,extent) \
	void x265_ip_filter_ver_luma_p_s_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														short_pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[8]; \
	int32_t i_offset; \
\
	i_stride = i_ref_stride ; \
	i_shift = X265_IF_FILTER_PREC + i_bit_depth - X265_IF_INTERNAL_PREC ; \
	i_offset = -X265_IF_INTERNAL_OFFS << i_shift; \
\
	INIT_IP_FILTER_LUMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_LUMA ; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_VER_LUMA_S_P_C_FUNC(frac,extent) \
	void x265_ip_filter_ver_luma_s_p_##frac##_##extent(short_pixel *ref, \
														int32_t i_ref_stride, \
														pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[8]; \
	int32_t i_offset; \
	int16_t i_max_val; \
\
	i_stride = i_ref_stride ; \
	i_shift = X265_IF_INTERNAL_PREC + X265_IF_FILTER_PREC - i_bit_depth; \
	i_offset = 1 << (i_shift - 1); \
	i_offset += X265_IF_INTERNAL_OFFS << X265_IF_FILTER_PREC; \
	i_max_val = (1 << i_bit_depth) - 1; \
\
	INIT_IP_FILTER_LUMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_LUMA ; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			i_val = ( i_val < 0 ) ? 0 : i_val; \
			i_val = ( i_val > i_max_val ) ? i_max_val : i_val; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_VER_LUMA_S_S_C_FUNC(frac,extent) \
	void x265_ip_filter_ver_luma_s_s_##frac##_##extent(short_pixel *ref, \
														int32_t i_ref_stride, \
														short_pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[8]; \
\
	i_stride = i_ref_stride ; \
	i_shift = X265_IF_FILTER_PREC ; \
\
	INIT_IP_FILTER_LUMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_LUMA ; \
			i_val = i_sum >> i_shift; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_HOR_CHROMA_P_P_C_FUNC(frac,extent) \
	void x265_ip_filter_hor_chroma_p_p_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[4]; \
	int32_t i_offset; \
	int16_t i_max_val; \
\
	i_stride = 1 ; \
	i_shift = X265_IF_FILTER_PREC ; \
	i_offset = 1 << (i_shift - 1); \
	i_max_val = (1 << i_bit_depth) - 1; \
\
	INIT_IP_FILTER_CHROMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_CHROMA; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			i_val = ( i_val < 0 ) ? 0 : i_val; \
			i_val = ( i_val > i_max_val ) ? i_max_val : i_val; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_HOR_CHROMA_P_S_C_FUNC(frac,extent) \
	void x265_ip_filter_hor_chroma_p_s_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														short_pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[4]; \
	int32_t i_offset; \
\
	i_stride = 1 ; \
	i_shift = X265_IF_FILTER_PREC + i_bit_depth - X265_IF_INTERNAL_PREC ; \
	i_offset = -X265_IF_INTERNAL_OFFS << i_shift; \
\
	INIT_IP_FILTER_CHROMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_CHROMA; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_VER_CHROMA_P_P_C_FUNC(frac,extent) \
	void x265_ip_filter_ver_chroma_p_p_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[4]; \
	int32_t i_offset; \
	int16_t i_max_val; \
\
	i_stride = i_ref_stride ; \
	i_shift = X265_IF_FILTER_PREC ; \
	i_offset = 1 << (i_shift - 1); \
	i_max_val = (1 << i_bit_depth) - 1; \
\
	INIT_IP_FILTER_CHROMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_CHROMA; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			i_val = ( i_val < 0 ) ? 0 : i_val; \
			i_val = ( i_val > i_max_val ) ? i_max_val : i_val; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_VER_CHROMA_P_S_C_FUNC(frac,extent) \
	void x265_ip_filter_ver_chroma_p_s_##frac##_##extent(pixel *ref, \
														int32_t i_ref_stride, \
														short_pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[4]; \
	int32_t i_offset; \
\
	i_stride = i_ref_stride ; \
	i_shift = X265_IF_FILTER_PREC + i_bit_depth - X265_IF_INTERNAL_PREC ; \
	i_offset = -X265_IF_INTERNAL_OFFS << i_shift; \
\
	INIT_IP_FILTER_CHROMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_CHROMA; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_VER_CHROMA_S_P_C_FUNC(frac,extent) \
	void x265_ip_filter_ver_chroma_s_p_##frac##_##extent(short_pixel *ref, \
														int32_t i_ref_stride, \
														pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[4]; \
	int32_t i_offset; \
	int16_t i_max_val; \
\
	i_stride = i_ref_stride ; \
	i_shift = X265_IF_INTERNAL_PREC + X265_IF_FILTER_PREC - i_bit_depth; \
	i_offset = 1 << (i_shift - 1); \
	i_offset += X265_IF_INTERNAL_OFFS << X265_IF_FILTER_PREC; \
	i_max_val = (1 << i_bit_depth) - 1; \
\
	INIT_IP_FILTER_CHROMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_CHROMA; \
			i_val = ( i_sum + i_offset ) >> i_shift; \
			i_val = ( i_val < 0 ) ? 0 : i_val; \
			i_val = ( i_val > i_max_val ) ? i_max_val : i_val; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}

#define DEFINE_IP_FILTER_VER_CHROMA_S_S_C_FUNC(frac,extent) \
	void x265_ip_filter_ver_chroma_s_s_##frac##_##extent(short_pixel *ref, \
														int32_t i_ref_stride, \
														short_pixel *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth ) \
{ \
	DECLARE_IP_FILTER_COMMON_DATA ; \
	int16_t c[4]; \
\
	i_stride = i_ref_stride ; \
	i_shift = X265_IF_FILTER_PREC; \
\
	INIT_IP_FILTER_CHROMA_COMMON_DATA(frac) ; \
	for (i_row = 0; i_row < i_height; i_row++) \
	{ \
		for (i_col = 0; i_col < i_width; i_col++) \
		{ \
			IP_FILTER_CHROMA; \
			i_val = i_sum >> i_shift; \
			dst[i_col] = i_val; \
		} \
\
		ref += i_ref_stride; \
		dst += i_dst_stride; \
	} \
\
}


#define DEFINE_ALL_IP_FILTER_LUMA_C_FUNC(DIR,SRC_TYPE_NAME,DST_TYPE_NAME,extent) \
		DEFINE_IP_FILTER_COPY_##DIR##_LUMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(0,extent) \
		DEFINE_IP_FILTER_##DIR##_LUMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(1,extent) \
		DEFINE_IP_FILTER_##DIR##_LUMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(2,extent) \
		DEFINE_IP_FILTER_##DIR##_LUMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(3,extent) \

#define DEFINE_ALL_IP_FILTER_LUMA_CMP_FUNC(dir,src_type,src_type_name,dst_type,dst_type_name,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,luma,src_type,src_type_name,dst_type,dst_type_name,0,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,luma,src_type,src_type_name,dst_type,dst_type_name,1,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,luma,src_type,src_type_name,dst_type,dst_type_name,2,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,luma,src_type,src_type_name,dst_type,dst_type_name,3,extent,extent1,extent2) \

#define DEFINE_ALL_IP_FILTER_CHROMA_C_FUNC(DIR,SRC_TYPE_NAME,DST_TYPE_NAME,extent) \
		DEFINE_IP_FILTER_COPY_##DIR##_CHROMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(0,extent) \
		DEFINE_IP_FILTER_##DIR##_CHROMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(1,extent) \
		DEFINE_IP_FILTER_##DIR##_CHROMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(2,extent) \
		DEFINE_IP_FILTER_##DIR##_CHROMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(3,extent) \
		DEFINE_IP_FILTER_##DIR##_CHROMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(4,extent) \
		DEFINE_IP_FILTER_##DIR##_CHROMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(5,extent) \
		DEFINE_IP_FILTER_##DIR##_CHROMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(6,extent) \
		DEFINE_IP_FILTER_##DIR##_CHROMA_##SRC_TYPE_NAME##_##DST_TYPE_NAME##_C_FUNC(7,extent) \

#define DEFINE_ALL_IP_FILTER_CHROMA_CMP_FUNC(dir,src_type,src_type_name,dst_type,dst_type_name,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,0,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,1,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,2,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,3,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,4,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,5,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,6,extent,extent1,extent2) \
		DEFINE_IP_FILTER_CMP_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,7,extent,extent1,extent2) \

#define X265_IP_FILTER_INITIALIZE_HELP(ip,extent) \
	X265_IP_FILTER_LUMA_INITIALIZE(ip,hor,p,p,extent) ; \
	X265_IP_FILTER_LUMA_INITIALIZE(ip,hor,p,s,extent) ; \
	X265_IP_FILTER_LUMA_INITIALIZE(ip,ver,p,p,extent) ; \
	X265_IP_FILTER_LUMA_INITIALIZE(ip,ver,p,s,extent) ; \
	X265_IP_FILTER_LUMA_INITIALIZE(ip,ver,s,p,extent) ; \
	X265_IP_FILTER_LUMA_INITIALIZE(ip,ver,s,s,extent) ; \
	X265_IP_FILTER_CHROMA_INITIALIZE(ip,hor,p,p,extent) ; \
	X265_IP_FILTER_CHROMA_INITIALIZE(ip,hor,p,s,extent) ; \
	X265_IP_FILTER_CHROMA_INITIALIZE(ip,ver,p,p,extent) ; \
	X265_IP_FILTER_CHROMA_INITIALIZE(ip,ver,p,s,extent) ; \
	X265_IP_FILTER_CHROMA_INITIALIZE(ip,ver,s,p,extent) ; \
	X265_IP_FILTER_CHROMA_INITIALIZE(ip,ver,s,s,extent) ; \

void x265_ip_initialize ( x265_ip_t *ip, unsigned int cpu )
{
	X265_IP_FILTER_INITIALIZE_HELP(ip,c)
	if ( cpu & X265_CPU_SSSE3 )
	{
		X265_IP_FILTER_INITIALIZE_HELP(ip,ssse3)
	}
	//	X265_IP_FILTER_INITIALIZE_HELP(ip,cmp)
}

DEFINE_ALL_IP_FILTER_LUMA_C_FUNC(HOR,P,P,c)
DEFINE_ALL_IP_FILTER_LUMA_C_FUNC(HOR,P,S,c)
DEFINE_ALL_IP_FILTER_LUMA_C_FUNC(VER,P,P,c)
DEFINE_ALL_IP_FILTER_LUMA_C_FUNC(VER,P,S,c)
DEFINE_ALL_IP_FILTER_LUMA_C_FUNC(VER,S,P,c)
DEFINE_ALL_IP_FILTER_LUMA_C_FUNC(VER,S,S,c)
DEFINE_ALL_IP_FILTER_LUMA_CMP_FUNC(hor, pixel,p,pixel,p,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_LUMA_CMP_FUNC(hor, pixel,p,short_pixel,s,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_LUMA_CMP_FUNC(ver, pixel,p,pixel,p,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_LUMA_CMP_FUNC(ver, pixel,p,short_pixel,s,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_LUMA_CMP_FUNC(ver,short_pixel,s,pixel,p,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_LUMA_CMP_FUNC(ver,short_pixel,s,short_pixel,s,cmp,c,ssse3)

DEFINE_ALL_IP_FILTER_CHROMA_C_FUNC(HOR,P,P,c)
DEFINE_ALL_IP_FILTER_CHROMA_C_FUNC(HOR,P,S,c)
DEFINE_ALL_IP_FILTER_CHROMA_C_FUNC(VER,P,P,c)
DEFINE_ALL_IP_FILTER_CHROMA_C_FUNC(VER,P,S,c)
DEFINE_ALL_IP_FILTER_CHROMA_C_FUNC(VER,S,P,c)
DEFINE_ALL_IP_FILTER_CHROMA_C_FUNC(VER,S,S,c)
DEFINE_ALL_IP_FILTER_CHROMA_CMP_FUNC(hor, pixel,p,pixel,p,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_CHROMA_CMP_FUNC(hor, pixel,p,short_pixel,s,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_CHROMA_CMP_FUNC(ver, pixel,p,pixel,p,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_CHROMA_CMP_FUNC(ver, pixel,p,short_pixel,s,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_CHROMA_CMP_FUNC(ver,short_pixel,s,pixel,p,cmp,c,ssse3)
DEFINE_ALL_IP_FILTER_CHROMA_CMP_FUNC(ver,short_pixel,s,short_pixel,s,cmp,c,ssse3)

