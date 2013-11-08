


#ifndef X265_TRANSFORM_H
#define X265_TRANSFORM_H


typedef void (*x265_tr_func)(int16_t *residual,
							int32_t i_residual_stride,
							int32_t *coeff,
							int32_t i_bit_depth) ;

typedef void (*x265_i_tr_func)(int16_t *residual,
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth) ;


typedef struct
{
	x265_tr_func tr[5] ;
	x265_i_tr_func itr[5] ;

} x265_transform_t ;


#define	DEFINE_ONE_TR_CMP_FUNC(name, extent,extent1,extent2,blk_size) \
		void x265_tr_quant_x_tr_##name##_##extent(int16_t *residual, \
												int32_t i_residual_stride, \
												int32_t *coeff, \
 												int32_t i_bit_depth) \
{ \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
	int32_t temp_coeff[blk_size*blk_size] ; \
	\
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_tr_##name##_##extent2(residual, i_residual_stride, coeff, i_bit_depth) ; \
	\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_tr_##name##_##extent1(residual, i_residual_stride, temp_coeff, i_bit_depth) ; \
\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
	if ( memory_compare_int32(coeff, blk_size, temp_coeff, blk_size, blk_size, blk_size ) ) \
	{ \
		fprintf ( stderr, "Partial Butter Fly %d Error\n", blk_size ) ; \
		exit (0) ; \
	} \
	\
	\
}

#define	DEFINE_ONE_ITR_CMP_FUNC(name, extent,extent1,extent2,blk_size) \
		void x265_tr_quant_x_itr_##name##_##extent(int16_t *residual, \
												int32_t i_residual_stride, \
												int32_t *coeff, \
 												int32_t i_bit_depth) \
{ \
	int64_t i_start_time = 0 ; \
	int64_t i_end_time = 0 ; \
	int16_t temp_residual[blk_size*blk_size] ; \
	\
	memset(temp_residual,0,sizeof(int16_t)*blk_size) ; \
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_itr_##name##_##extent2(residual, i_residual_stride, coeff, i_bit_depth) ; \
	\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_2 ( i_end_time - i_start_time ) ; \
	i_start_time = x265_get_timer_state () ; \
	x265_tr_quant_x_itr_##name##_##extent1(temp_residual, blk_size, coeff, i_bit_depth) ; \
\
	i_end_time = x265_get_timer_state () ; \
	add_timer_status_1 ( i_end_time - i_start_time ) ; \
	if ( memory_compare_int16(residual, i_residual_stride, temp_residual, blk_size, blk_size, blk_size ) ) \
	{ \
		fprintf ( stderr, "Partial Inverse Butter Fly %d Error\n", blk_size ) ; \
		exit (0) ; \
	} \
	\
	\
}

#define X265_TR_INITIALIZE(tr_func,extent) \
	tr_func[1] = x265_tr_quant_x_tr_8x8_##extent ; \
	tr_func[2] = x265_tr_quant_x_tr_16x16_##extent ; \
	tr_func[3] = x265_tr_quant_x_tr_32x32_##extent ; \

/*
#define X265_ITR_INITIALIZE(itr_func,extent) \
	itr_func[1] = x265_tr_quant_x_itr_8x8_##extent ; \
	itr_func[2] = x265_tr_quant_x_itr_16x16_##extent ; \
	itr_func[3] = x265_tr_quant_x_itr_32x32_##extent ; \
*/

#define	DECLARE_TR_FUNC(name,extent) \
		void x265_tr_quant_x_tr_##name##_##extent(int16_t *residual, \
												int32_t i_residual_stride, \
												int32_t *coeff, \
												int32_t i_bit_depth) ; \

#define	DECLARE_ITR_FUNC(name,extent) \
		void x265_tr_quant_x_itr_##name##_##extent(int16_t *residual, \
												int32_t i_residual_stride, \
												int32_t *coeff, \
												int32_t i_bit_depth) ; \

#define	DEFINE_ALL_TR_CMP_FUNC(extent,extent1,extent2) \
		DEFINE_ONE_TR_CMP_FUNC(  8x8, extent,extent1,extent2, 8) \
		DEFINE_ONE_TR_CMP_FUNC(16x16, extent,extent1,extent2,16) \
		DEFINE_ONE_TR_CMP_FUNC(32x32, extent,extent1,extent2,32) \

#define	DEFINE_ALL_ITR_CMP_FUNC(extent,extent1,extent2) \
		DEFINE_ONE_ITR_CMP_FUNC(  8x8, extent,extent1,extent2, 8) \
		DEFINE_ONE_ITR_CMP_FUNC(16x16, extent,extent1,extent2,16) \
		DEFINE_ONE_ITR_CMP_FUNC(32x32, extent,extent1,extent2,32) \

#define DECLARE_ALL_TR_FUNC(extent) \
		DECLARE_TR_FUNC(  4x4,extent) \
		DECLARE_TR_FUNC(  8x8,extent) \
		DECLARE_TR_FUNC(16x16,extent) \
		DECLARE_TR_FUNC(32x32,extent) \

#define DECLARE_ALL_ITR_FUNC(extent) \
		DECLARE_ITR_FUNC(  4x4,extent) \
		DECLARE_ITR_FUNC(  8x8,extent) \
		DECLARE_ITR_FUNC(16x16,extent) \
		DECLARE_ITR_FUNC(32x32,extent) \

DECLARE_ALL_TR_FUNC(c)
DECLARE_ALL_TR_FUNC(ssse3)
DECLARE_ALL_ITR_FUNC(c)
DECLARE_ALL_ITR_FUNC(ssse3)





void x265_transform_initialize ( x265_transform_t *transform, unsigned int cpu ) ;


#endif



