

#include "common.h"


#if X265_ADAPTIVE_QP_SELECTION

#define DEFINE_ONE_QUANT_C_FUNC(i_blk_size) \
void x265_tr_quant_x_quant_##i_blk_size##x##i_blk_size##_c(int32_t *p_coef, \
															x265_coeff_t* p_q_coef, \
															int32_t *p_quant_coeff, \
															int32_t *delta_u, \
															int32_t i_add, \
															int32_t i_q_bits, \
															uint32_t *p_ac_sum, \
															int32_t b_use_adapt_qp_select, \
															int32_t *p_arl_coef, \
															int32_t i_q_bits_c ) \
{ \
	int32_t loop = 0 ; \
	int32_t i_level = 0; \
	int32_t i_sign = 0; \
	int32_t i_tmp_level = 0 ; \
	int32_t i_add_c = 0 ; \
	int32_t i_q_bits8 = 0 ; \
\
	i_add_c = 1 << (i_q_bits_c - 1) ; \
	i_q_bits8 = i_q_bits - 8; \
	for( loop = 0; loop < i_blk_size*i_blk_size; loop++ ) \
	{ \
		i_level = p_coef[loop]; \
		i_sign = (i_level < 0 ? -1: 1); \
\
		i_tmp_level = (int32_t)abs(i_level) * p_quant_coeff[loop]; \
		if( b_use_adapt_qp_select ) \
		{ \
			p_arl_coef[loop] = (int32_t)((i_tmp_level + i_add_c ) >> i_q_bits_c); \
		} \
		i_level = (int32_t)((i_tmp_level + i_add ) >> i_q_bits); \
		delta_u[loop] = (int32_t)((i_tmp_level - (i_level<<i_q_bits) )>> i_q_bits8); \
		*p_ac_sum += i_level; \
		i_level *= i_sign; \
		p_q_coef[loop] = x265_clip3_int32( i_level, -32768, 32767 ); \
	} \
} \

#define	DEFINE_ONE_QUANT_CMP_FUNC(name, extent,extent1,extent2,blk_size) \
	void x265_tr_quant_x_quant_##name##_##extent(int32_t *p_coef, \
												x265_coeff_t* p_q_coef1, \
												int32_t *p_quant_coeff, \
												int32_t *delta_u1, \
												int32_t i_add, \
												int32_t i_q_bits, \
												uint32_t *p_ac_sum, \
												int32_t b_use_adapt_qp_select, \
												int32_t *p_arl_coef1, \
												int32_t i_q_bits_c ) \
{ \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
	int32_t p_arl_coef2[blk_size*blk_size] ; \
	int32_t delta_u2[blk_size*blk_size] ; \
	x265_coeff_t p_q_coef2[blk_size*blk_size] ; \
	uint32_t i_sum = 0 ; \
\
	memset(p_arl_coef2, 0, sizeof(int32_t) * blk_size * blk_size) ; \
	memset(delta_u2, 0, sizeof(int32_t) * blk_size * blk_size) ; \
	memset(p_q_coef2, 0, sizeof(x265_coeff_t) * blk_size * blk_size) ; \
	\
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_quant_##name##_##extent2(p_coef, \
									p_q_coef1, \
									p_quant_coeff, \
									delta_u1, \
									i_add, \
									i_q_bits, \
									p_ac_sum, \
									b_use_adapt_qp_select, \
									p_arl_coef1, \
									i_q_bits_c) ; \
\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_quant_##name##_##extent1(p_coef, \
												p_q_coef2, \
												p_quant_coeff, \
												delta_u2, \
												i_add, \
												i_q_bits, \
												&i_sum, \
												b_use_adapt_qp_select, \
												p_arl_coef2, \
												i_q_bits_c) ; \
\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
\
	if(memory_compare_int32(p_q_coef1, blk_size, p_q_coef2, blk_size, blk_size, blk_size)) \
	{ \
		fprintf (stderr, \
				"x265_tr_quant_x_quant_%dx%d Error:\tp_q_coef\n,", \
				blk_size, blk_size) ; \
		exit(0) ; \
	} \
\
	if(memory_compare_int32(delta_u1, blk_size, delta_u2, blk_size, blk_size, blk_size)) \
	{ \
		fprintf (stderr, \
				"x265_tr_quant_x_quant_%dx%d Error:\tdelta_u\n,", \
				blk_size, blk_size) ; \
		exit(0) ; \
	} \
\
	if((*p_ac_sum) != i_sum) \
	{ \
		fprintf (stderr, \
				"x265_tr_quant_x_quant_%dx%d Error:\t%d != %d\n\n,", \
				blk_size, blk_size, (*p_ac_sum), i_sum) ; \
		exit(0) ; \
	} \
	if(memory_compare_int32(p_arl_coef1, blk_size, p_arl_coef2, blk_size, blk_size, blk_size)) \
	{ \
		fprintf (stderr, \
				"x265_tr_quant_x_quant_%dx%d Error:\tp_arl_coef\n,", \
				blk_size, blk_size) ; \
		exit(0) ; \
	} \
	\
	\
}

#else


#define DEFINE_ONE_QUANT_C_FUNC(i_blk_size) \
void x265_tr_quant_x_quant_##i_blk_size##x##i_blk_size##_c(int32_t *p_coef, \
															x265_coeff_t* p_q_coef, \
															int32_t *p_quant_coeff, \
															int32_t *delta_u, \
															int32_t i_add, \
															int32_t i_q_bits, \
															uint32_t *p_ac_sum, \
															int32_t b_use_adapt_qp_select ) \
{ \
	int32_t loop = 0 ; \
	int32_t i_level = 0; \
	int32_t i_sign = 0; \
	int32_t i_q_bits8 = 0 ; \
\
	i_q_bits8 = i_q_bits - 8; \
	for( loop = 0; loop < i_blk_size*i_blk_size; loop++ ) \
	{ \
		i_level = p_coef[loop]; \
		i_sign = (i_level < 0 ? -1: 1); \
\
		i_level = ((int32_t)abs(i_level) * p_quant_coeff[loop] + i_add ) >> i_q_bits; \
		delta_u[loop] = (int32_t)(((int32_t)abs(p_coef[loop]) * p_quant_coeff[loop] - (i_level << i_q_bits)) >> i_q_bits8 ); \
		*p_ac_sum += i_level; \
		i_level *= i_sign; \
		p_q_coef[loop] = x265_clip3_int32( i_level, -32768, 32767 ); \
	} \
} \


#define	DEFINE_ONE_QUANT_CMP_FUNC(name, extent,extent1,extent2,blk_size) \
	void x265_tr_quant_x_quant_##name##_##extent(int32_t *p_coef, \
												x265_coeff_t* p_q_coef1, \
												int32_t *p_quant_coeff, \
												int32_t *delta_u1, \
												int32_t i_add, \
												int32_t i_q_bits, \
												uint32_t *p_ac_sum, \
												int32_t b_use_adapt_qp_select ) \
{ \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
	int32_t delta_u2[blk_size*blk_size] ; \
	x265_coeff_t p_q_coef2[blk_size*blk_size] ; \
	uint32_t i_sum = 0 ; \
\
	memset(delta_u2, 0, sizeof(int32_t) * blk_size * blk_size) ; \
	memset(p_q_coef2, 0, sizeof(x265_coeff_t) * blk_size * blk_size) ; \
	\
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_quant_##name##_##extent2(p_coef, \
									p_q_coef1, \
									p_quant_coeff, \
									delta_u1, \
									i_add, \
									i_q_bits, \
									p_ac_sum, \
									b_use_adapt_qp_select) ; \
\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_quant_##name##_##extent1(p_coef, \
												p_q_coef2, \
												p_quant_coeff, \
												delta_u2, \
												i_add, \
												i_q_bits, \
												&i_sum, \
												b_use_adapt_qp_select) ; \
\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
\
	if(memory_compare_int32(p_q_coef1, blk_size, p_q_coef2, blk_size, blk_size, blk_size)) \
	{ \
		fprintf (stderr, \
				"x265_tr_quant_x_quant_%dx%d Error:\tp_q_coef\n,", \
				blk_size, blk_size) ; \
		exit(0) ; \
	} \
\
	if(memory_compare_int32(delta_u1, blk_size, delta_u2, blk_size, blk_size, blk_size)) \
	{ \
		fprintf (stderr, \
				"x265_tr_quant_x_quant_%dx%d Error:\tdelta_u\n,", \
				blk_size, blk_size) ; \
		exit(0) ; \
	} \
\
	if((*p_ac_sum) != i_sum) \
	{ \
		fprintf (stderr, \
				"x265_tr_quant_x_quant_%dx%d Error:\t%d != %d\n\n,", \
				blk_size, blk_size, (*p_ac_sum), i_sum) ; \
		exit(0) ; \
	} \
	\
	\
}

#endif

#define DEFINE_ONE_DEQUANT_C_FUNC(i_blk_size, i_log2_tr_size) \
void x265_tr_quant_x_dequant_##i_blk_size##x##i_blk_size##_c(x265_coeff_t* p_q_coef, \
															int32_t* p_coef, \
															int32_t *p_dequant_coef, \
															int32_t i_bit_depth, \
															int32_t i_per, \
															int32_t i_scale, \
															int32_t b_scaling_list_enabled_flag) \
{ \
	int32_t loop = 0 ; \
	int32_t i_shift, i_add, i_coeff_q; \
	int32_t i_transform_shift = 0; \
	x265_coeff_t clip_q_coef; \
\
	i_transform_shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log2_tr_size; \
	i_shift = X265_QUANT_IQUANT_SHIFT - X265_QUANT_SHIFT - i_transform_shift; \
	if(b_scaling_list_enabled_flag) \
	{ \
		i_shift += 4; \
		if(i_shift > i_per) \
		{ \
			i_add = 1 << (i_shift - i_per - 1); \
\
			for( loop = 0; loop < i_blk_size * i_blk_size; loop++ ) \
			{ \
				clip_q_coef = x265_clip3_int32( p_q_coef[loop], -32768, 32767 ); \
				i_coeff_q = ((clip_q_coef * p_dequant_coef[loop]) + i_add ) >> (i_shift -  i_per); \
				p_coef[loop] = x265_clip3_int32(i_coeff_q, -32768, 32767); \
			} \
		} \
		else \
		{ \
			for( loop = 0; loop < i_blk_size*i_blk_size; loop++ ) \
			{ \
				clip_q_coef = x265_clip3_int32( p_q_coef[loop], -32768, 32767 ); \
				i_coeff_q   = x265_clip3_int32( clip_q_coef * p_dequant_coef[loop], -32768, 32767 ); \
				p_coef[loop] = x265_clip3_int32( i_coeff_q << (i_per - i_shift), -32768, 32767 ); \
			} \
		} \
	} \
	else \
	{ \
		i_add = 1 << (i_shift-1); \
		for( loop = 0; loop < i_blk_size*i_blk_size; loop++ ) \
		{ \
			clip_q_coef = x265_clip3_int32( p_q_coef[loop], -32768, 32767 ); \
			i_coeff_q = ( clip_q_coef * i_scale + i_add ) >> i_shift; \
			p_coef[loop] = x265_clip3_int32(i_coeff_q, -32768, 32767); \
		} \
	} \
} \


#define	DEFINE_ONE_DEQUANT_CMP_FUNC(name, extent,extent1,extent2,blk_size) \
	void x265_tr_quant_x_dequant_##name##_##extent(x265_coeff_t* p_q_coef, \
													int32_t* p_coef1, \
													int32_t *p_dequant_coef, \
													int32_t i_bit_depth, \
													int32_t i_per, \
													int32_t i_scale, \
													int32_t b_scaling_list_enabled_flag) \
{ \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
	int32_t p_coef2[blk_size*blk_size] ; \
\
	memset(p_coef2, 0, sizeof(int32_t) * blk_size * blk_size) ; \
	\
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_dequant_##name##_##extent2(p_q_coef, \
												p_coef1, \
												p_dequant_coef, \
												i_bit_depth, \
												i_per, \
												i_scale, \
												b_scaling_list_enabled_flag ) ; \
\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_dequant_##name##_##extent1(p_q_coef, \
												p_coef2, \
												p_dequant_coef, \
												i_bit_depth, \
												i_per, \
												i_scale, \
												b_scaling_list_enabled_flag ) ; \
\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
\
	if(memory_compare_int32(p_coef1, blk_size, p_coef2, blk_size, blk_size, blk_size)) \
	{ \
		fprintf (stderr, \
				"x265_tr_quant_x_dequant_%dx%d Error:\tp_q_coef\n", \
				blk_size, blk_size) ; \
		exit(0) ; \
	} \
\
	\
	\
}

#define DEFINE_ALL_QUANT_C_FUNC
	DEFINE_ONE_QUANT_C_FUNC( 4)
	DEFINE_ONE_QUANT_C_FUNC( 8)
	DEFINE_ONE_QUANT_C_FUNC(16)
	DEFINE_ONE_QUANT_C_FUNC(32)

#define DEFINE_ALL_DEQUANT_C_FUNC
	DEFINE_ONE_DEQUANT_C_FUNC( 4, 2)
	DEFINE_ONE_DEQUANT_C_FUNC( 8, 3)
	DEFINE_ONE_DEQUANT_C_FUNC(16, 4)
	DEFINE_ONE_DEQUANT_C_FUNC(32, 5)

#define	DEFINE_ALL_QUANT_CMP_FUNC(extent,extent1,extent2) \
		DEFINE_ONE_QUANT_CMP_FUNC(  4x4, extent,extent1,extent2, 4) \
		DEFINE_ONE_QUANT_CMP_FUNC(  8x8, extent,extent1,extent2, 8) \
		DEFINE_ONE_QUANT_CMP_FUNC(16x16, extent,extent1,extent2,16) \
		DEFINE_ONE_QUANT_CMP_FUNC(32x32, extent,extent1,extent2,32)

#define	DEFINE_ALL_DEQUANT_CMP_FUNC(extent,extent1,extent2) \
		DEFINE_ONE_DEQUANT_CMP_FUNC(  4x4, extent,extent1,extent2, 4) \
		DEFINE_ONE_DEQUANT_CMP_FUNC(  8x8, extent,extent1,extent2, 8) \
		DEFINE_ONE_DEQUANT_CMP_FUNC(16x16, extent,extent1,extent2,16) \
		DEFINE_ONE_DEQUANT_CMP_FUNC(32x32, extent,extent1,extent2,32)


DEFINE_ALL_QUANT_C_FUNC
DEFINE_ALL_QUANT_CMP_FUNC(cmp,c,ssse3)

DEFINE_ALL_DEQUANT_C_FUNC
DEFINE_ALL_DEQUANT_CMP_FUNC(cmp,c,ssse3)

#define X265_QUANT_INITIALIZE(quant_func,extent) \
	quant_func[0] = x265_tr_quant_x_quant_4x4_##extent ; \
	quant_func[1] = x265_tr_quant_x_quant_8x8_##extent ; \
	quant_func[2] = x265_tr_quant_x_quant_16x16_##extent ; \
	quant_func[3] = x265_tr_quant_x_quant_32x32_##extent ;

#define X265_DEQUANT_INITIALIZE(dequant_func,extent) \
	dequant_func[0] = x265_tr_quant_x_dequant_4x4_##extent ; \
	dequant_func[1] = x265_tr_quant_x_dequant_8x8_##extent ; \
	dequant_func[2] = x265_tr_quant_x_dequant_16x16_##extent ; \
	dequant_func[3] = x265_tr_quant_x_dequant_32x32_##extent ; \
	dequant_func[4] = x265_tr_quant_x_dequant_4x4_##extent ;



void x265_quant_initialize ( x265_quant_t *quant, unsigned int cpu )
{
	X265_QUANT_INITIALIZE(quant->quant, c) ;
	X265_DEQUANT_INITIALIZE(quant->dequant, c) ;
	if ( cpu & X265_CPU_SSSE3 )
	{
		X265_QUANT_INITIALIZE(quant->quant, ssse3) ;
		X265_DEQUANT_INITIALIZE(quant->dequant, ssse3) ;
	}
	//	X265_QUANT_INITIALIZE(quant->quant, cmp) ;
	//	X265_DEQUANT_INITIALIZE(quant->dequant, cmp) ;
}


