


#include "common.h"
#include "transform.h"


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


DEFINE_ALL_TR_CMP_FUNC(cmp,c,ssse3)
DEFINE_ALL_ITR_CMP_FUNC(cmp,c,ssse3)

#define X265_ITR_INITIALIZE(itr_func,extent) \
	itr_func[1] = x265_tr_quant_x_itr_8x8_##extent ; \
	itr_func[2] = x265_tr_quant_x_itr_16x16_##extent ; \
	itr_func[3] = x265_tr_quant_x_itr_32x32_##extent ; \

void x265_transform_initialize ( x265_transform_t *transform, unsigned int cpu )
{
	X265_TR_INITIALIZE(transform->tr, c) ;
	X265_ITR_INITIALIZE(transform->itr, c) ;
	if ( cpu & X265_CPU_SSSE3 )
	{
		X265_TR_INITIALIZE(transform->tr, ssse3) ;
		X265_ITR_INITIALIZE(transform->itr, ssse3) ;
	}
}


