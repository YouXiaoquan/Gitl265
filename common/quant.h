


#ifndef X265_QUANT_H
#define X265_QUANT_H


#if X265_ADAPTIVE_QP_SELECTION

typedef void (*x265_quant_func)(int32_t *p_coef,
								x265_coeff_t* p_q_coef,
								int32_t *p_quant_coeff,
								int32_t *delta_u,
								int32_t i_add,
								int32_t i_q_bits,
								uint32_t *p_ac_sum,
								int32_t b_use_adapt_qp_select,
								int32_t *p_arl_coef,
								int32_t i_q_bits_c
								) ;

#define	DECLARE_QUANT_FUNC(name,extent) \
	void x265_tr_quant_x_quant_##name##_##extent(int32_t *p_coef, \
												x265_coeff_t* p_q_coef, \
												int32_t *p_quant_coeff, \
												int32_t *delta_u, \
												int32_t i_add, \
												int32_t i_q_bits, \
												uint32_t *p_ac_sum, \
												int32_t b_use_adapt_qp_select, \
												int32_t *p_arl_coef, \
												int32_t i_q_bits_c ) ;


#else

typedef void (*x265_quant_func)(int32_t *p_coef,
								x265_coeff_t* p_q_coef,
								int32_t *p_quant_coeff,
								int32_t *delta_u,
								int32_t i_add,
								int32_t i_q_bits,
								uint32_t *p_ac_sum,
								int32_t b_use_adapt_qp_select
								) ;


#define	DECLARE_QUANT_FUNC(name,extent) \
	void x265_tr_quant_x_quant_##name##_##extent(int32_t *p_coef, \
												x265_coeff_t* p_q_coef, \
												int32_t *p_quant_coeff, \
												int32_t *delta_u, \
												int32_t i_add, \
												int32_t i_q_bits, \
												uint32_t *p_ac_sum, \
												int32_t b_use_adapt_qp_select ) ;

#endif

typedef void (*x265_dequant_func)(x265_coeff_t* p_q_coef,
								int32_t* p_coef,
								int32_t *p_dequant_coef,
								int32_t i_bit_depth,
								int32_t i_per,
								int32_t i_scale,
								int32_t b_scaling_list_enabled_flag) ;

#define	DECLARE_DEQUANT_FUNC(name,extent) \
	void x265_tr_quant_x_dequant_##name##_##extent(x265_coeff_t* p_q_coef, \
													int32_t* p_coef, \
													int32_t *p_dequant_coef, \
													int32_t i_bit_depth, \
													int32_t i_per, \
													int32_t i_scale, \
													int32_t b_scaling_list_enabled_flag) ;

#define DECLARE_ALL_QUANT_FUNC(extent) \
		DECLARE_QUANT_FUNC(  4x4,extent) \
		DECLARE_QUANT_FUNC(  8x8,extent) \
		DECLARE_QUANT_FUNC(16x16,extent) \
		DECLARE_QUANT_FUNC(32x32,extent) \

#define DECLARE_ALL_DEQUANT_FUNC(extent) \
		DECLARE_DEQUANT_FUNC(  4x4,extent) \
		DECLARE_DEQUANT_FUNC(  8x8,extent) \
		DECLARE_DEQUANT_FUNC(16x16,extent) \
		DECLARE_DEQUANT_FUNC(32x32,extent) \



DECLARE_ALL_QUANT_FUNC(c)
DECLARE_ALL_QUANT_FUNC(ssse3)
DECLARE_ALL_QUANT_FUNC(cmp)

DECLARE_ALL_DEQUANT_FUNC(c)
DECLARE_ALL_DEQUANT_FUNC(ssse3)
DECLARE_ALL_DEQUANT_FUNC(cmp)


typedef struct
{
	x265_quant_func quant[4] ;
	x265_dequant_func dequant[5] ;

} x265_quant_t ;


void x265_quant_initialize ( x265_quant_t *quant, unsigned int cpu ) ;


#endif

