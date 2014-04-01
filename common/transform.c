


#include "common.h"
#include "transform.h"


void x265_tr_quant_x_tr_dst_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 4
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = i_bit_depth - 7 ;
	i_shift_2nd = 8 ;
	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( block + BLK_SIZE * loop, residual + i_residual_stride * loop, sizeof(int16_t)*BLK_SIZE );
	}

	fast_forward_dst( block, tmp, i_shift_1st );
	fast_forward_dst( tmp, temp_coeff, i_shift_2nd );

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		coeff[ loop ] = temp_coeff[loop];
	}

#undef BLK_SIZE
}


void x265_tr_quant_x_tr_4x4_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 4
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = i_bit_depth - 7 ;
	i_shift_2nd = 8 ;
	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( block + BLK_SIZE * loop, residual + i_residual_stride * loop, sizeof(int16_t)*BLK_SIZE );
	}

	partial_butterfly4( block, tmp, i_shift_1st, BLK_SIZE );
	partial_butterfly4( tmp, temp_coeff, i_shift_2nd, BLK_SIZE );

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		coeff[ loop ] = temp_coeff[loop];
	}

#undef BLK_SIZE
}

void x265_tr_quant_x_tr_8x8_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 8
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = i_bit_depth - 6 ;
	i_shift_2nd = 9 ;
	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( block + BLK_SIZE * loop, residual + i_residual_stride * loop, sizeof(int16_t)*BLK_SIZE );
	}

	partial_butterfly8( block, tmp, i_shift_1st, BLK_SIZE );
	partial_butterfly8( tmp, temp_coeff, i_shift_2nd, BLK_SIZE );

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		coeff[ loop ] = temp_coeff[loop];
	}

#undef BLK_SIZE
}

void x265_tr_quant_x_tr_16x16_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 16
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = i_bit_depth - 5 ;
	i_shift_2nd = 10 ;
	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( block + BLK_SIZE * loop, residual + i_residual_stride * loop, sizeof(int16_t)*BLK_SIZE );
	}

	partial_butterfly16( block, tmp, i_shift_1st, BLK_SIZE );
	partial_butterfly16( tmp, temp_coeff, i_shift_2nd, BLK_SIZE );

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		coeff[ loop ] = temp_coeff[loop];
	}

#undef BLK_SIZE
}

void x265_tr_quant_x_tr_32x32_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 32
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = i_bit_depth - 4 ;
	i_shift_2nd = 11 ;
	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( block + BLK_SIZE * loop, residual + i_residual_stride * loop, sizeof(int16_t)*BLK_SIZE );
	}

	partial_butterfly32( block, tmp, i_shift_1st, BLK_SIZE );
	partial_butterfly32( tmp, temp_coeff, i_shift_2nd, BLK_SIZE );

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		coeff[ loop ] = temp_coeff[loop];
	}

#undef BLK_SIZE
}

void x265_tr_quant_x_itr_8x8_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 8
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = 7 ;
	i_shift_2nd = 20 - i_bit_depth ;

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		temp_coeff[ loop ] = coeff[loop];
	}

	partial_butterfly_inverse8( temp_coeff, tmp, i_shift_1st, BLK_SIZE );
	partial_butterfly_inverse8( tmp, block, i_shift_2nd, BLK_SIZE );

	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( residual + i_residual_stride * loop, block + BLK_SIZE * loop, sizeof(int16_t)*BLK_SIZE );
	}
#undef BLK_SIZE
}


void x265_tr_quant_x_itr_dst_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 4
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = 7 ;
	i_shift_2nd = 20 - i_bit_depth ;

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		temp_coeff[ loop ] = coeff[loop];
	}

	fast_inverse_dst( temp_coeff, tmp, i_shift_1st );
	fast_inverse_dst( tmp, block, i_shift_2nd );

	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( residual + i_residual_stride * loop, block + BLK_SIZE * loop, sizeof(int16_t)*BLK_SIZE );
	}
#undef BLK_SIZE
}

void x265_tr_quant_x_itr_4x4_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 4
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = 7 ;
	i_shift_2nd = 20 - i_bit_depth ;

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		temp_coeff[ loop ] = coeff[loop];
	}

	partial_butterfly_inverse4( temp_coeff, tmp, i_shift_1st, BLK_SIZE );
	partial_butterfly_inverse4( tmp, block, i_shift_2nd, BLK_SIZE );

	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( residual + i_residual_stride * loop, block + BLK_SIZE * loop, sizeof(int16_t)*BLK_SIZE );
	}
#undef BLK_SIZE
}

void x265_tr_quant_x_itr_16x16_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 16
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = 7 ;
	i_shift_2nd = 20 - i_bit_depth ;

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		temp_coeff[ loop ] = coeff[loop];
	}

	partial_butterfly_inverse16( temp_coeff, tmp, i_shift_1st, BLK_SIZE );
	partial_butterfly_inverse16( tmp, block, i_shift_2nd, BLK_SIZE );

	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( residual + i_residual_stride * loop, block + BLK_SIZE * loop, sizeof(int16_t)*BLK_SIZE );
	}


#undef BLK_SIZE

}

void x265_tr_quant_x_itr_32x32_c(int16_t *residual, \
								int32_t i_residual_stride,
								int32_t *coeff,
								int32_t i_bit_depth)
{
#define BLK_SIZE 32
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t loop = 0 ;
	int16_t block[ BLK_SIZE * BLK_SIZE ];
	int16_t temp_coeff[ BLK_SIZE * BLK_SIZE ];
	int16_t tmp[ BLK_SIZE * BLK_SIZE ];

	i_shift_1st = 7 ;
	i_shift_2nd = 20 - i_bit_depth ;

	for ( loop = 0; loop < (BLK_SIZE * BLK_SIZE); ++loop )
	{
		temp_coeff[ loop ] = coeff[loop];
	}

	partial_butterfly_inverse32( temp_coeff, tmp, i_shift_1st, BLK_SIZE );
	partial_butterfly_inverse32( tmp, block, i_shift_2nd, BLK_SIZE );

	for (loop = 0; loop < BLK_SIZE; ++ loop)
	{
		memcpy( residual + i_residual_stride * loop, block + BLK_SIZE * loop, sizeof(int16_t)*BLK_SIZE );
	}


#undef BLK_SIZE

}


#define DEFINE_ONE_TRS_C_FUNC(i_blk_size, i_log2_blk_size) \
void x265_tr_quant_x_trs_##i_blk_size##x##i_blk_size##_c(int16_t *residual, \
								int32_t i_residual_stride, \
								int32_t *coeff, \
								int32_t i_bit_depth) \
{ \
	int32_t  shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log2_blk_size; \
	uint32_t transform_skip_shift; \
	int32_t  j,k; \
	if(shift >= 0) \
	{ \
		transform_skip_shift = shift; \
		for (j = 0; j < i_blk_size; j++) \
		{ \
			for(k = 0; k < i_blk_size; k ++) \
			{ \
				coeff[j*i_blk_size + k] = residual[j * i_residual_stride + k] << transform_skip_shift; \
			} \
		} \
	} \
	else \
	{ \
		int32_t offset; \
		transform_skip_shift = -shift; \
		offset = (1 << (transform_skip_shift - 1)); \
		for (j = 0; j < i_blk_size; j++) \
		{ \
			for(k = 0; k < i_blk_size; k ++) \
			{ \
				coeff[j*i_blk_size + k] = (residual[j * i_residual_stride + k] + offset) >> transform_skip_shift; \
			} \
		} \
	} \
}


#define DEFINE_ONE_ITRS_C_FUNC(i_blk_size, i_log2_blk_size) \
void x265_tr_quant_x_itrs_##i_blk_size##x##i_blk_size##_c(int16_t *residual, \
														int32_t i_residual_stride, \
														int32_t *coeff, \
														int32_t i_bit_depth) \
{ \
	int32_t  shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log2_blk_size; \
	uint32_t transform_skip_shift; \
	int32_t  j,k; \
	if(shift > 0) \
	{ \
		int32_t offset; \
		transform_skip_shift = shift; \
		offset = (1 << (transform_skip_shift -1)); \
		for ( j = 0; j < i_blk_size; j++ ) \
		{ \
			for(k = 0; k < i_blk_size; k ++) \
			{ \
				residual[j * i_residual_stride + k] = (coeff[j*i_blk_size+k] + offset) >> transform_skip_shift; \
			} \
		} \
	} \
	else \
	{ \
		transform_skip_shift = - shift; \
		for ( j = 0; j < i_blk_size; j++ ) \
		{ \
			for(k = 0; k < i_blk_size; k ++) \
			{ \
				residual[j * i_residual_stride + k] = coeff[j*i_blk_size+k] << transform_skip_shift; \
			} \
		} \
	} \
}

#define DEFINE_ALL_TRS_C_FUNC
	DEFINE_ONE_TRS_C_FUNC( 4, 2)
	DEFINE_ONE_TRS_C_FUNC( 8, 3)
	DEFINE_ONE_TRS_C_FUNC(16, 4)
	DEFINE_ONE_TRS_C_FUNC(32, 5)


#define DEFINE_ALL_ITRS_C_FUNC
	DEFINE_ONE_ITRS_C_FUNC( 4, 2)
	DEFINE_ONE_ITRS_C_FUNC( 8, 3)
	DEFINE_ONE_ITRS_C_FUNC(16, 4)
	DEFINE_ONE_ITRS_C_FUNC(32, 5)


DEFINE_ALL_TR_CMP_FUNC(cmp,c,ssse3)
DEFINE_ALL_TRS_C_FUNC
DEFINE_ALL_TRS_CMP_FUNC(cmp,c,ssse3)
DEFINE_ALL_ITR_CMP_FUNC(cmp,c,ssse3)
DEFINE_ALL_ITRS_C_FUNC
DEFINE_ALL_ITRS_CMP_FUNC(cmp,c,ssse3)

void x265_transform_initialize ( x265_transform_t *transform, unsigned int cpu )
{
	X265_TR_INITIALIZE(transform->tr, c) ;
	X265_ITR_INITIALIZE(transform->itr, c) ;
	if ( cpu & X265_CPU_SSSE3 )
	{
		X265_TR_INITIALIZE(transform->tr, ssse3) ;
		X265_ITR_INITIALIZE(transform->itr, ssse3) ;
	}
	//	X265_TR_INITIALIZE(transform->tr, cmp) ;
	//	X265_ITR_INITIALIZE(transform->itr, cmp) ;
}


