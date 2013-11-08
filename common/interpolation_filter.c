

#include "common.h"


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


void x265_interpolation_filter_delete ( x265_interpolation_filter_t *interpolation_filter )
{
	x265_free ( interpolation_filter ) ;
}

x265_interpolation_filter_t *x265_interpolation_filter_new (x265_t *h)
{
	x265_interpolation_filter_t *interpolation_filter = NULL ;

	CHECKED_MALLOCZERO( interpolation_filter, sizeof(x265_interpolation_filter_t) );

	return interpolation_filter ;
fail:
	x265_interpolation_filter_delete ( (void*) interpolation_filter ) ;
	return NULL ;
}

int x265_interpolation_filter_init ( x265_t *h, x265_interpolation_filter_t *interpolation_filter )
{
	x265_ip_initialize ( &interpolation_filter->ip, h->param.cpu ) ;
    return 0 ;
}

void x265_interpolation_filter_deinit ( x265_interpolation_filter_t *interpolation_filter )
{

}

// ====================================================================================================================
// private member functions
// ====================================================================================================================

/**
 * \brief apply unit fir filter to a block of samples
 *
 * \param bit_depth   bit_depth of samples
 * \param src        pointer to source samples
 * \param src_stride  stride of source samples
 * \param dst        pointer to destination samples
 * \param dst_stride  stride of destination samples
 * \param width      width of block
 * \param height     height of block
 * \param is_first    flag indicating whether it is the first filtering operation
 * \param is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_copy_p_p(x265_interpolation_filter_t *interpolation_filter,
											int32_t i_bit_depth,
											pixel *src,
											int32_t i_src_stride,
											pixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last)
{
	int32_t i_row, i_col;
    int16_t i_val = 0;
	int32_t i_shift = 0;
	int16_t i_offset = 0;
	int16_t i_max_val = 0;
	int16_t i_min_val = 0;

	if ( b_is_first == b_is_last )
	{
		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				dst[i_col] = src[i_col];
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
	else if ( b_is_first )
	{
		i_shift = X265_IF_INTERNAL_PREC - i_bit_depth;

		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				i_val = src[i_col] << i_shift;
				dst[i_col] = i_val - (int16_t)X265_IF_INTERNAL_OFFS;
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
	else
	{
		i_shift = X265_IF_INTERNAL_PREC - i_bit_depth;
		i_offset = X265_IF_INTERNAL_OFFS;
		i_offset += i_shift ? (1 << (i_shift - 1)):0;
		i_max_val = (1 << i_bit_depth) - 1;
		i_min_val = 0;
		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				i_val = src[ i_col ];
				i_val = ( i_val + i_offset ) >> i_shift;
				if (i_val < i_min_val)
				{
					i_val = i_min_val;
				}
				if (i_val > i_max_val)
				{
					i_val = i_max_val;
				}
				dst[i_col] = i_val;
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
}

/**
 * \brief apply unit fir filter to a block of samples
 *
 * \param bit_depth   bit_depth of samples
 * \param src        pointer to source samples
 * \param src_stride  stride of source samples
 * \param dst        pointer to destination samples
 * \param dst_stride  stride of destination samples
 * \param width      width of block
 * \param height     height of block
 * \param is_first    flag indicating whether it is the first filtering operation
 * \param is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_copy_p_s(x265_interpolation_filter_t *interpolation_filter,
											int32_t i_bit_depth,
											pixel *src,
											int32_t i_src_stride,
											spixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last)
{
	int32_t i_row, i_col;
    int16_t i_val = 0;
	int32_t i_shift = 0;
	int16_t i_offset = 0;
	int16_t i_max_val = 0;
	int16_t i_min_val = 0;

	if ( b_is_first == b_is_last )
	{
		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				dst[i_col] = src[i_col];
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
	else if ( b_is_first )
	{
		i_shift = X265_IF_INTERNAL_PREC - i_bit_depth;

		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				i_val = src[i_col] << i_shift;
				dst[i_col] = i_val - (int16_t)X265_IF_INTERNAL_OFFS;
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
	else
	{
		i_shift = X265_IF_INTERNAL_PREC - i_bit_depth;
		i_offset = X265_IF_INTERNAL_OFFS;
		i_offset += i_shift ? (1 << (i_shift - 1)):0;
		i_max_val = (1 << i_bit_depth) - 1;
		i_min_val = 0;
		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				i_val = src[ i_col ];
				i_val = ( i_val + i_offset ) >> i_shift;
				if (i_val < i_min_val)
				{
					i_val = i_min_val;
				}
				if (i_val > i_max_val)
				{
					i_val = i_max_val;
				}
				dst[i_col] = i_val;
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
}

/**
 * \brief apply unit fir filter to a block of samples
 *
 * \param bit_depth   bit_depth of samples
 * \param src        pointer to source samples
 * \param src_stride  stride of source samples
 * \param dst        pointer to destination samples
 * \param dst_stride  stride of destination samples
 * \param width      width of block
 * \param height     height of block
 * \param is_first    flag indicating whether it is the first filtering operation
 * \param is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_copy_s_p(x265_interpolation_filter_t *interpolation_filter,
											int32_t i_bit_depth,
											spixel *src,
											int32_t i_src_stride,
											pixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last)
{
	int32_t i_row, i_col;
    int16_t i_val = 0;
	int32_t i_shift = 0;
	int16_t i_offset = 0;
	int16_t i_max_val = 0;
	int16_t i_min_val = 0;

	if ( b_is_first == b_is_last )
	{
		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				dst[i_col] = src[i_col];
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
	else if ( b_is_first )
	{
		i_shift = X265_IF_INTERNAL_PREC - i_bit_depth;

		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				i_val = src[i_col] << i_shift;
				dst[i_col] = i_val - (int16_t)X265_IF_INTERNAL_OFFS;
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
	else
	{
		i_shift = X265_IF_INTERNAL_PREC - i_bit_depth;
		i_offset = X265_IF_INTERNAL_OFFS;
		i_offset += i_shift ? (1 << (i_shift - 1)):0;
		i_max_val = (1 << i_bit_depth) - 1;
		i_min_val = 0;
		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				i_val = src[ i_col ];
				i_val = ( i_val + i_offset ) >> i_shift;
				if (i_val < i_min_val)
				{
					i_val = i_min_val;
				}
				if (i_val > i_max_val)
				{
					i_val = i_max_val;
				}
				dst[i_col] = i_val;
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
}

/**
 * \brief apply unit fir filter to a block of samples
 *
 * \param bit_depth   bit_depth of samples
 * \param src        pointer to source samples
 * \param src_stride  stride of source samples
 * \param dst        pointer to destination samples
 * \param dst_stride  stride of destination samples
 * \param width      width of block
 * \param height     height of block
 * \param is_first    flag indicating whether it is the first filtering operation
 * \param is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_copy_s_s(x265_interpolation_filter_t *interpolation_filter,
											int32_t i_bit_depth,
											spixel *src,
											int32_t i_src_stride,
											spixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last)
{
	int32_t i_row, i_col;
    int16_t i_val = 0;
	int32_t i_shift = 0;
	int16_t i_offset = 0;
	int16_t i_max_val = 0;
	int16_t i_min_val = 0;

	if ( b_is_first == b_is_last )
	{
		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				dst[i_col] = src[i_col];
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
	else if ( b_is_first )
	{
		i_shift = X265_IF_INTERNAL_PREC - i_bit_depth;

		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				i_val = src[i_col] << i_shift;
				dst[i_col] = i_val - (int16_t)X265_IF_INTERNAL_OFFS;
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
	else
	{
		i_shift = X265_IF_INTERNAL_PREC - i_bit_depth;
		i_offset = X265_IF_INTERNAL_OFFS;
		i_offset += i_shift ? (1 << (i_shift - 1)):0;
		i_max_val = (1 << i_bit_depth) - 1;
		i_min_val = 0;
		for (i_row = 0; i_row < i_height; i_row++)
		{
			for (i_col = 0; i_col < i_width; i_col++)
			{
				i_val = src[ i_col ];
				i_val = ( i_val + i_offset ) >> i_shift;
				if (i_val < i_min_val)
				{
					i_val = i_min_val;
				}
				if (i_val > i_max_val)
				{
					i_val = i_max_val;
				}
				dst[i_col] = i_val;
			}

			src += i_src_stride;
			dst += i_dst_stride;
		}
	}
}
/**
 * \brief apply fir filter to a block of samples
 *
 * \tparam n          number of taps
 * \tparam is_vertical flag indicating filtering along vertical direction
 * \tparam is_first    flag indicating whether it is the first filtering operation
 * \tparam is_last     flag indicating whether it is the last filtering operation
 * \param  bit_depth   bit depth of samples
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  coeff      pointer to filter taps
 */

void x265_interpolation_filter_filter_p_p(x265_interpolation_filter_t *interpolation_filter,
									int32_t n,
									int32_t b_is_vertical,
									int32_t b_is_first,
									int32_t b_is_last,
									int32_t i_bit_depth,
									pixel *src,
									int32_t i_src_stride,
									pixel *dst,
									int32_t i_dst_stride,
									int32_t i_width,
									int32_t i_height,
									int16_t *coeff)
{
	int32_t i_row, i_col;
	int16_t c[8];
	int32_t i_stride = 0;
	int32_t i_offset;
	int16_t i_max_val;
	int32_t i_head_room = 0;
	int32_t i_shift = 0;
	int32_t i_sum = 0;
	int16_t i_val = 0;

	c[2] = 0 ;
	c[3] = 0 ;
	c[4] = 0 ;
	c[5] = 0 ;
	c[6] = 0 ;
	c[7] = 0 ;

	c[0] = coeff[0];
	c[1] = coeff[1];
	if ( n >= 4 )
	{
		c[2] = coeff[2];
		c[3] = coeff[3];
	}
	if ( n >= 6 )
	{
		c[4] = coeff[4];
		c[5] = coeff[5];
	}
	if ( n == 8 )
	{
		c[6] = coeff[6];
		c[7] = coeff[7];
	}

	i_stride = ( b_is_vertical ) ? i_src_stride : 1;
	src -= ( n/2 - 1 ) * i_stride;

	i_head_room = X265_IF_INTERNAL_PREC - i_bit_depth;
	i_shift = X265_IF_FILTER_PREC;
	if ( b_is_last )
	{
		i_shift += (b_is_first) ? 0 : i_head_room;
		i_offset = 1 << (i_shift - 1);
		i_offset += (b_is_first) ? 0 : X265_IF_INTERNAL_OFFS << X265_IF_FILTER_PREC;
		i_max_val = (1 << i_bit_depth) - 1;
	}
	else
	{
		i_shift -= (b_is_first) ? i_head_room : 0;
		i_offset = (b_is_first) ? -X265_IF_INTERNAL_OFFS << i_shift : 0;
		i_max_val = 0;
	}

	for (i_row = 0; i_row < i_height; i_row++)
	{
		for (i_col = 0; i_col < i_width; i_col++)
		{
			i_sum = src[ i_col + 0 * i_stride] * c[0];
			i_sum += src[ i_col + 1 * i_stride] * c[1];
			if ( n >= 4 )
			{
				i_sum += src[ i_col + 2 * i_stride] * c[2];
				i_sum += src[ i_col + 3 * i_stride] * c[3];
			}
			if ( n >= 6 )
			{
				i_sum += src[ i_col + 4 * i_stride] * c[4];
				i_sum += src[ i_col + 5 * i_stride] * c[5];
			}
			if ( n == 8 )
			{
				i_sum += src[ i_col + 6 * i_stride] * c[6];
				i_sum += src[ i_col + 7 * i_stride] * c[7];
			}

			i_val = ( i_sum + i_offset ) >> i_shift;
			if ( b_is_last )
			{
				i_val = ( i_val < 0 ) ? 0 : i_val;
				i_val = ( i_val > i_max_val ) ? i_max_val : i_val;
			}
			dst[i_col] = i_val;
		}

		src += i_src_stride;
		dst += i_dst_stride;
	}
}

/**
 * \brief apply fir filter to a block of samples
 *
 * \tparam n          number of taps
 * \tparam is_vertical flag indicating filtering along vertical direction
 * \tparam is_first    flag indicating whether it is the first filtering operation
 * \tparam is_last     flag indicating whether it is the last filtering operation
 * \param  bit_depth   bit depth of samples
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  coeff      pointer to filter taps
 */

void x265_interpolation_filter_filter_p_s(x265_interpolation_filter_t *interpolation_filter,
									int32_t n,
									int32_t b_is_vertical,
									int32_t b_is_first,
									int32_t b_is_last,
									int32_t i_bit_depth,
									pixel *src,
									int32_t i_src_stride,
									spixel *dst,
									int32_t i_dst_stride,
									int32_t i_width,
									int32_t i_height,
									int16_t *coeff)
{
	int32_t i_row, i_col;
	int16_t c[8];
	int32_t i_stride = 0;
	int32_t i_offset;
	int16_t i_max_val;
	int32_t i_head_room = 0;
	int32_t i_shift = 0;
	int32_t i_sum = 0;
	int16_t i_val = 0;

	c[2] = 0 ;
	c[3] = 0 ;
	c[4] = 0 ;
	c[5] = 0 ;
	c[6] = 0 ;
	c[7] = 0 ;

	c[0] = coeff[0];
	c[1] = coeff[1];
	if ( n >= 4 )
	{
		c[2] = coeff[2];
		c[3] = coeff[3];
	}
	if ( n >= 6 )
	{
		c[4] = coeff[4];
		c[5] = coeff[5];
	}
	if ( n == 8 )
	{
		c[6] = coeff[6];
		c[7] = coeff[7];
	}

	i_stride = ( b_is_vertical ) ? i_src_stride : 1;
	src -= ( n/2 - 1 ) * i_stride;

	i_head_room = X265_IF_INTERNAL_PREC - i_bit_depth;
	i_shift = X265_IF_FILTER_PREC;
	if ( b_is_last )
	{
		i_shift += (b_is_first) ? 0 : i_head_room;
		i_offset = 1 << (i_shift - 1);
		i_offset += (b_is_first) ? 0 : X265_IF_INTERNAL_OFFS << X265_IF_FILTER_PREC;
		i_max_val = (1 << i_bit_depth) - 1;
	}
	else
	{
		i_shift -= (b_is_first) ? i_head_room : 0;
		i_offset = (b_is_first) ? -X265_IF_INTERNAL_OFFS << i_shift : 0;
		i_max_val = 0;
	}

	for (i_row = 0; i_row < i_height; i_row++)
	{
		for (i_col = 0; i_col < i_width; i_col++)
		{
			i_sum = src[ i_col + 0 * i_stride] * c[0];
			i_sum += src[ i_col + 1 * i_stride] * c[1];
			if ( n >= 4 )
			{
				i_sum += src[ i_col + 2 * i_stride] * c[2];
				i_sum += src[ i_col + 3 * i_stride] * c[3];
			}
			if ( n >= 6 )
			{
				i_sum += src[ i_col + 4 * i_stride] * c[4];
				i_sum += src[ i_col + 5 * i_stride] * c[5];
			}
			if ( n == 8 )
			{
				i_sum += src[ i_col + 6 * i_stride] * c[6];
				i_sum += src[ i_col + 7 * i_stride] * c[7];
			}

			i_val = ( i_sum + i_offset ) >> i_shift;
			if ( b_is_last )
			{
				i_val = ( i_val < 0 ) ? 0 : i_val;
				i_val = ( i_val > i_max_val ) ? i_max_val : i_val;
			}
			dst[i_col] = i_val;
		}

		src += i_src_stride;
		dst += i_dst_stride;
	}
}

/**
 * \brief apply fir filter to a block of samples
 *
 * \tparam n          number of taps
 * \tparam is_vertical flag indicating filtering along vertical direction
 * \tparam is_first    flag indicating whether it is the first filtering operation
 * \tparam is_last     flag indicating whether it is the last filtering operation
 * \param  bit_depth   bit depth of samples
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  coeff      pointer to filter taps
 */

void x265_interpolation_filter_filter_s_p(x265_interpolation_filter_t *interpolation_filter,
									int32_t n,
									int32_t b_is_vertical,
									int32_t b_is_first,
									int32_t b_is_last,
									int32_t i_bit_depth,
									spixel *src,
									int32_t i_src_stride,
									pixel *dst,
									int32_t i_dst_stride,
									int32_t i_width,
									int32_t i_height,
									int16_t *coeff)
{
	int32_t i_row, i_col;
	int16_t c[8];
	int32_t i_stride = 0;
	int32_t i_offset;
	int16_t i_max_val;
	int32_t i_head_room = 0;
	int32_t i_shift = 0;
	int32_t i_sum = 0;
	int16_t i_val = 0;

	c[2] = 0 ;
	c[3] = 0 ;
	c[4] = 0 ;
	c[5] = 0 ;
	c[6] = 0 ;
	c[7] = 0 ;

	c[0] = coeff[0];
	c[1] = coeff[1];
	if ( n >= 4 )
	{
		c[2] = coeff[2];
		c[3] = coeff[3];
	}
	if ( n >= 6 )
	{
		c[4] = coeff[4];
		c[5] = coeff[5];
	}
	if ( n == 8 )
	{
		c[6] = coeff[6];
		c[7] = coeff[7];
	}

	i_stride = ( b_is_vertical ) ? i_src_stride : 1;
	src -= ( n/2 - 1 ) * i_stride;

	i_head_room = X265_IF_INTERNAL_PREC - i_bit_depth;
	i_shift = X265_IF_FILTER_PREC;
	if ( b_is_last )
	{
		i_shift += (b_is_first) ? 0 : i_head_room;
		i_offset = 1 << (i_shift - 1);
		i_offset += (b_is_first) ? 0 : X265_IF_INTERNAL_OFFS << X265_IF_FILTER_PREC;
		i_max_val = (1 << i_bit_depth) - 1;
	}
	else
	{
		i_shift -= (b_is_first) ? i_head_room : 0;
		i_offset = (b_is_first) ? -X265_IF_INTERNAL_OFFS << i_shift : 0;
		i_max_val = 0;
	}

	for (i_row = 0; i_row < i_height; i_row++)
	{
		for (i_col = 0; i_col < i_width; i_col++)
		{
			i_sum = src[ i_col + 0 * i_stride] * c[0];
			i_sum += src[ i_col + 1 * i_stride] * c[1];
			if ( n >= 4 )
			{
				i_sum += src[ i_col + 2 * i_stride] * c[2];
				i_sum += src[ i_col + 3 * i_stride] * c[3];
			}
			if ( n >= 6 )
			{
				i_sum += src[ i_col + 4 * i_stride] * c[4];
				i_sum += src[ i_col + 5 * i_stride] * c[5];
			}
			if ( n == 8 )
			{
				i_sum += src[ i_col + 6 * i_stride] * c[6];
				i_sum += src[ i_col + 7 * i_stride] * c[7];
			}

			i_val = ( i_sum + i_offset ) >> i_shift;
			if ( b_is_last )
			{
				i_val = ( i_val < 0 ) ? 0 : i_val;
				i_val = ( i_val > i_max_val ) ? i_max_val : i_val;
			}
			dst[i_col] = i_val;
		}

		src += i_src_stride;
		dst += i_dst_stride;
	}
}

/**
 * \brief apply fir filter to a block of samples
 *
 * \tparam n          number of taps
 * \tparam is_vertical flag indicating filtering along vertical direction
 * \tparam is_first    flag indicating whether it is the first filtering operation
 * \tparam is_last     flag indicating whether it is the last filtering operation
 * \param  bit_depth   bit depth of samples
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  coeff      pointer to filter taps
 */

void x265_interpolation_filter_filter_s_s(x265_interpolation_filter_t *interpolation_filter,
									int32_t n,
									int32_t b_is_vertical,
									int32_t b_is_first,
									int32_t b_is_last,
									int32_t i_bit_depth,
									spixel *src,
									int32_t i_src_stride,
									spixel *dst,
									int32_t i_dst_stride,
									int32_t i_width,
									int32_t i_height,
									int16_t *coeff)
{
	int32_t i_row, i_col;
	int16_t c[8];
	int32_t i_stride = 0;
	int32_t i_offset;
	int16_t i_max_val;
	int32_t i_head_room = 0;
	int32_t i_shift = 0;
	int32_t i_sum = 0;
	int16_t i_val = 0;

	c[2] = 0 ;
	c[3] = 0 ;
	c[4] = 0 ;
	c[5] = 0 ;
	c[6] = 0 ;
	c[7] = 0 ;

	c[0] = coeff[0];
	c[1] = coeff[1];
	if ( n >= 4 )
	{
		c[2] = coeff[2];
		c[3] = coeff[3];
	}
	if ( n >= 6 )
	{
		c[4] = coeff[4];
		c[5] = coeff[5];
	}
	if ( n == 8 )
	{
		c[6] = coeff[6];
		c[7] = coeff[7];
	}

	i_stride = ( b_is_vertical ) ? i_src_stride : 1;
	src -= ( n/2 - 1 ) * i_stride;

	i_head_room = X265_IF_INTERNAL_PREC - i_bit_depth;
	i_shift = X265_IF_FILTER_PREC;
	if ( b_is_last )
	{
		i_shift += (b_is_first) ? 0 : i_head_room;
		i_offset = 1 << (i_shift - 1);
		i_offset += (b_is_first) ? 0 : X265_IF_INTERNAL_OFFS << X265_IF_FILTER_PREC;
		i_max_val = (1 << i_bit_depth) - 1;
	}
	else
	{
		i_shift -= (b_is_first) ? i_head_room : 0;
		i_offset = (b_is_first) ? -X265_IF_INTERNAL_OFFS << i_shift : 0;
		i_max_val = 0;
	}

	for (i_row = 0; i_row < i_height; i_row++)
	{
		for (i_col = 0; i_col < i_width; i_col++)
		{
			i_sum = src[ i_col + 0 * i_stride] * c[0];
			i_sum += src[ i_col + 1 * i_stride] * c[1];
			if ( n >= 4 )
			{
				i_sum += src[ i_col + 2 * i_stride] * c[2];
				i_sum += src[ i_col + 3 * i_stride] * c[3];
			}
			if ( n >= 6 )
			{
				i_sum += src[ i_col + 4 * i_stride] * c[4];
				i_sum += src[ i_col + 5 * i_stride] * c[5];
			}
			if ( n == 8 )
			{
				i_sum += src[ i_col + 6 * i_stride] * c[6];
				i_sum += src[ i_col + 7 * i_stride] * c[7];
			}

			i_val = ( i_sum + i_offset ) >> i_shift;
			if ( b_is_last )
			{
				i_val = ( i_val < 0 ) ? 0 : i_val;
				i_val = ( i_val > i_max_val ) ? i_max_val : i_val;
			}
			dst[i_col] = i_val;
		}

		src += i_src_stride;
		dst += i_dst_stride;
	}
}

/**
 * \brief filter a block of samples (horizontal)
 *
 * \tparam n          number of taps
 * \param  bit_depth   bit depth of samples
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  is_last     flag indicating whether it is the last filtering operation
 * \param  coeff      pointer to filter taps
 */
void x265_interpolation_filter_filter_hor_p_p(x265_interpolation_filter_t *interpolation_filter,
										int32_t n,
										int32_t i_bit_depth,
										pixel *src,
										int32_t i_src_stride,
										pixel *dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t b_is_last,
										int16_t *coeff)
{
	if ( b_is_last )
	{
		x265_interpolation_filter_filter_p_p(interpolation_filter,
										n,
										0,
										1,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else
	{
		x265_interpolation_filter_filter_p_p(interpolation_filter,
										n,
										0,
										1,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
}
/**
 * \brief filter a block of samples (horizontal)
 *
 * \tparam n          number of taps
 * \param  bit_depth   bit depth of samples
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  is_last     flag indicating whether it is the last filtering operation
 * \param  coeff      pointer to filter taps
 */
void x265_interpolation_filter_filter_hor_p_s(x265_interpolation_filter_t *interpolation_filter,
										int32_t n,
										int32_t i_bit_depth,
										pixel *src,
										int32_t i_src_stride,
										spixel *dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t b_is_last,
										int16_t *coeff)
{
	if ( b_is_last )
	{
		x265_interpolation_filter_filter_p_s(interpolation_filter,
										n,
										0,
										1,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else
	{
		x265_interpolation_filter_filter_p_s(interpolation_filter,
										n,
										0,
										1,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
}

/**
 * \brief filter a block of samples (vertical)
 *
 * \tparam n          number of taps
 * \param  bit_dpeth   sample bit depth
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 * \param  coeff      pointer to filter taps
 */
void x265_interpolation_filter_filter_ver_p_p(x265_interpolation_filter_t *interpolation_filter,
											int32_t n,
											int32_t i_bit_depth,
											pixel *src,
											int32_t i_src_stride,
											pixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last,
											int16_t *coeff)
{
	if ( b_is_first && b_is_last )
	{
		x265_interpolation_filter_filter_p_p(interpolation_filter,
										n,
										1,
										1,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else if ( b_is_first && !b_is_last )
	{
		x265_interpolation_filter_filter_p_p(interpolation_filter,
										n,
										1,
										1,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height, coeff);
	}
	else if ( !b_is_first && b_is_last )
	{
		x265_interpolation_filter_filter_p_p(interpolation_filter,
										n,
										1,
										0,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else
	{
		x265_interpolation_filter_filter_p_p(interpolation_filter,
										n,
										1,
										0,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
}
/**
 * \brief filter a block of samples (vertical)
 *
 * \tparam n          number of taps
 * \param  bit_dpeth   sample bit depth
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 * \param  coeff      pointer to filter taps
 */
void x265_interpolation_filter_filter_ver_p_s(x265_interpolation_filter_t *interpolation_filter,
											int32_t n,
											int32_t i_bit_depth,
											pixel *src,
											int32_t i_src_stride,
											spixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last,
											int16_t *coeff)
{
	if ( b_is_first && b_is_last )
	{
		x265_interpolation_filter_filter_p_s(interpolation_filter,
										n,
										1,
										1,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else if ( b_is_first && !b_is_last )
	{
		x265_interpolation_filter_filter_p_s(interpolation_filter,
										n,
										1,
										1,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height, coeff);
	}
	else if ( !b_is_first && b_is_last )
	{
		x265_interpolation_filter_filter_p_s(interpolation_filter,
										n,
										1,
										0,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else
	{
		x265_interpolation_filter_filter_p_s(interpolation_filter,
										n,
										1,
										0,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
}

/**
 * \brief filter a block of samples (vertical)
 *
 * \tparam n          number of taps
 * \param  bit_dpeth   sample bit depth
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 * \param  coeff      pointer to filter taps
 */
void x265_interpolation_filter_filter_ver_s_p(x265_interpolation_filter_t *interpolation_filter,
											int32_t n,
											int32_t i_bit_depth,
											spixel *src,
											int32_t i_src_stride,
											pixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last,
											int16_t *coeff)
{
	if ( b_is_first && b_is_last )
	{
		x265_interpolation_filter_filter_s_p(interpolation_filter,
										n,
										1,
										1,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else if ( b_is_first && !b_is_last )
	{
		x265_interpolation_filter_filter_s_p(interpolation_filter,
										n,
										1,
										1,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height, coeff);
	}
	else if ( !b_is_first && b_is_last )
	{
		x265_interpolation_filter_filter_s_p(interpolation_filter,
										n,
										1,
										0,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else
	{
		x265_interpolation_filter_filter_s_p(interpolation_filter,
										n,
										1,
										0,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
}

/**
 * \brief filter a block of samples (vertical)
 *
 * \tparam n          number of taps
 * \param  bit_dpeth   sample bit depth
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 * \param  coeff      pointer to filter taps
 */
void x265_interpolation_filter_filter_ver_s_s(x265_interpolation_filter_t *interpolation_filter,
											int32_t n,
											int32_t i_bit_depth,
											spixel *src,
											int32_t i_src_stride,
											spixel *dst,
											int32_t i_dst_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_is_first,
											int32_t b_is_last,
											int16_t *coeff)
{
	if ( b_is_first && b_is_last )
	{
		x265_interpolation_filter_filter_s_s(interpolation_filter,
										n,
										1,
										1,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else if ( b_is_first && !b_is_last )
	{
		x265_interpolation_filter_filter_s_s(interpolation_filter,
										n,
										1,
										1,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height, coeff);
	}
	else if ( !b_is_first && b_is_last )
	{
		x265_interpolation_filter_filter_s_s(interpolation_filter,
										n,
										1,
										0,
										1,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
	else
	{
		x265_interpolation_filter_filter_s_s(interpolation_filter,
										n,
										1,
										0,
										0,
										i_bit_depth,
										src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										coeff);
	}
}

// ====================================================================================================================
// public member functions
// ====================================================================================================================

/**
 * \brief filter a block of luma samples (horizontal)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_hor_luma_p_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;

	assert(i_frac >= 0 && i_frac < 4);
	ip = &interpolation_filter->ip ;

	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_p_p(interpolation_filter,
											h->param.sps.i_bit_depth_y,
											src,
											i_src_stride,
											dst,
											i_dst_stride,
											i_width,
											i_height,
											1,
											b_is_last );
	}
	else
	{
		ip->ip_filter_hor_luma_p_p[i_frac](src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										h->param.sps.i_bit_depth_y,
										NULL) ;
	}
}


// ====================================================================================================================
// public member functions
// ====================================================================================================================

/**
 * \brief filter a block of luma samples (horizontal)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_hor_luma_p_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;

	assert(i_frac >= 0 && i_frac < 4);
	ip = &interpolation_filter->ip ;

	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_p_s(interpolation_filter,
											h->param.sps.i_bit_depth_y,
											src,
											i_src_stride,
											dst,
											i_dst_stride,
											i_width,
											i_height,
											1,
											b_is_last );
	}
	else
	{
		ip->ip_filter_hor_luma_p_s[i_frac](src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										h->param.sps.i_bit_depth_y,
										NULL) ;
	}
}

/**
 * \brief filter a block of luma samples (vertical)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_ver_luma_p_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;

	assert(i_frac >= 0 && i_frac < 4);
	ip = &interpolation_filter->ip ;

	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_p_p(interpolation_filter,
												h->param.sps.i_bit_depth_y,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												b_is_first,
												b_is_last );
	}
	else
	{
		ip->ip_filter_ver_luma_p_p[i_frac](src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										h->param.sps.i_bit_depth_y,
										NULL) ;

	}
}
void x265_interpolation_filter_filter_ver_luma_p_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;

	assert(i_frac >= 0 && i_frac < 4);
	ip = &interpolation_filter->ip ;

	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_p_s(interpolation_filter,
												h->param.sps.i_bit_depth_y,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												b_is_first,
												b_is_last );
	}
	else
	{
		ip->ip_filter_ver_luma_p_s[i_frac](src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										h->param.sps.i_bit_depth_y,
										NULL) ;
	}
}
/**
 * \brief filter a block of luma samples (vertical)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_ver_luma_s_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												spixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;

	assert(i_frac >= 0 && i_frac < 4);
	ip = &interpolation_filter->ip ;
	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_s_p(interpolation_filter,
												h->param.sps.i_bit_depth_y,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												b_is_first,
												b_is_last );
	}
	else
	{
		ip->ip_filter_ver_luma_s_p[i_frac](src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										h->param.sps.i_bit_depth_y,
										NULL) ;
	}
}
/**
 * \brief filter a block of luma samples (vertical)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_ver_luma_s_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												spixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;

	assert(i_frac >= 0 && i_frac < 4);
	ip = &interpolation_filter->ip ;
	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_s_s(interpolation_filter,
												h->param.sps.i_bit_depth_y,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												b_is_first,
												b_is_last );
	}
	else
	{
		ip->ip_filter_ver_luma_s_s[i_frac](src,
											i_src_stride,
											dst,
											i_dst_stride,
											i_width,
											i_height,
											h->param.sps.i_bit_depth_y,
											NULL) ;
	}
}

/**
 * \brief filter a block of chroma samples (horizontal)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_hor_chroma_p_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;
	assert(i_frac >= 0 && i_frac < 8);

	ip = &interpolation_filter->ip ;
	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_p_p(interpolation_filter,
												h->param.sps.i_bit_depth_c,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												1,
												b_is_last );
	}
	else
	{
		ip->ip_filter_hor_chroma_p_p[i_frac](src,
											i_src_stride,
											dst,
											i_dst_stride,
											i_width,
											i_height,
											h->param.sps.i_bit_depth_c,
											NULL) ;
	}
}
/**
 * \brief filter a block of chroma samples (horizontal)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_hor_chroma_p_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;
	assert(i_frac >= 0 && i_frac < 8);

	ip = &interpolation_filter->ip ;
	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_p_s(interpolation_filter,
												h->param.sps.i_bit_depth_c,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												1,
												b_is_last );
	}
	else
	{
		ip->ip_filter_hor_chroma_p_s[i_frac](src,
											i_src_stride,
											dst,
											i_dst_stride,
											i_width,
											i_height,
											h->param.sps.i_bit_depth_c,
											NULL) ;
	}
}

/**
 * \brief filter a block of chroma samples (vertical)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_ver_chroma_p_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;
	assert(i_frac >= 0 && i_frac < 8);

	ip = &interpolation_filter->ip ;
	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_p_p(interpolation_filter,
												h->param.sps.i_bit_depth_c,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												b_is_first,
												b_is_last );
	}
	else
	{
		ip->ip_filter_ver_chroma_p_p[i_frac](src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										h->param.sps.i_bit_depth_c,
										NULL) ;
	}
}

/**
 * \brief filter a block of chroma samples (vertical)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_ver_chroma_p_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												pixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;
	assert(i_frac >= 0 && i_frac < 8);

	ip = &interpolation_filter->ip ;
	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_p_s(interpolation_filter,
												h->param.sps.i_bit_depth_c,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												b_is_first,
												b_is_last );
	}
	else
	{
		ip->ip_filter_ver_chroma_p_s[i_frac](src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										h->param.sps.i_bit_depth_c,
										NULL) ;
	}
}

/**
 * \brief filter a block of chroma samples (vertical)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_ver_chroma_s_p(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												spixel *src,
												int32_t i_src_stride,
												pixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;

	assert(i_frac >= 0 && i_frac < 8);
	ip = &interpolation_filter->ip ;
	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_s_p(interpolation_filter,
												h->param.sps.i_bit_depth_c,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												b_is_first,
												b_is_last );
	}
	else
	{
		ip->ip_filter_ver_chroma_s_p[i_frac](src,
										i_src_stride,
										dst,
										i_dst_stride,
										i_width,
										i_height,
										h->param.sps.i_bit_depth_c,
										NULL) ;
	}
}

/**
 * \brief filter a block of chroma samples (vertical)
 *
 * \param  src        pointer to source samples
 * \param  src_stride  stride of source samples
 * \param  dst        pointer to destination samples
 * \param  dst_stride  stride of destination samples
 * \param  width      width of block
 * \param  height     height of block
 * \param  frac       fractional sample offset
 * \param  is_first    flag indicating whether it is the first filtering operation
 * \param  is_last     flag indicating whether it is the last filtering operation
 */
void x265_interpolation_filter_filter_ver_chroma_s_s(x265_t *h,
												x265_interpolation_filter_t *interpolation_filter,
												spixel *src,
												int32_t i_src_stride,
												spixel *dst,
												int32_t i_dst_stride,
												int32_t i_width,
												int32_t i_height,
												int32_t i_frac,
												int32_t b_is_first,
												int32_t b_is_last )
{
	x265_ip_t *ip = NULL ;

	assert(i_frac >= 0 && i_frac < 8);
	ip = &interpolation_filter->ip ;
	if ( i_frac == 0 )
	{
		x265_interpolation_filter_filter_copy_s_s(interpolation_filter,
												h->param.sps.i_bit_depth_c,
												src,
												i_src_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												b_is_first,
												b_is_last );
	}
	else
	{
		ip->ip_filter_ver_chroma_s_s[i_frac](src,
											i_src_stride,
											dst,
											i_dst_stride,
											i_width,
											i_height,
											h->param.sps.i_bit_depth_c,
											NULL) ;
	}
}

//! \}







