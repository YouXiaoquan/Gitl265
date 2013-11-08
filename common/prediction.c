

#include "common.h"

pixel *x265_prediction_get_predic_buf ( x265_prediction_t *prediction )
{
	return prediction->image_ext;
}

int32_t x265_prediction_get_predic_buf_width ( x265_prediction_t *prediction )
{
	return prediction->i_image_ext_stride;
}

int32_t x265_prediction_get_predic_buf_height ( x265_prediction_t *prediction )
{
	return prediction->i_image_ext_height;
}



x265_prediction_t *x265_prediction_new ( x265_t *h )
{
	x265_prediction_t *prediction = NULL ;

	CHECKED_MALLOCZERO( prediction, sizeof(x265_prediction_t) );

	return prediction ;
fail:
	x265_prediction_delete ( (void*) prediction ) ;
	return NULL ;
}

void x265_prediction_delete ( x265_prediction_t *prediction )
{
	x265_prediction_deinit ( prediction ) ;
	x265_free ( prediction ) ;
}

int x265_prediction_init ( x265_t *h, x265_prediction_t *prediction )
{
	int i = 0, j = 0 ;
	for (i = 0; i < 4; i++)
	{
		if ( x265_simage_init ( &prediction->filtered_block_tmp[i] ) )
		{
			goto fail ;
		}
		for (j = 0; j < 4; j++)
		{
			if ( x265_image_init ( &prediction->filtered_block[i][j] ) )
			{
				goto fail ;
			}
		}
	}

	if ( x265_image_init ( &prediction->image_pred[0] ) )
	{
		goto fail ;
	}
	if ( x265_image_init ( &prediction->image_pred[1] ) )
	{
		goto fail ;
	}
	if ( x265_simage_init ( &prediction->simage_pred[0] ) )
	{
		goto fail ;
	}
	if ( x265_simage_init ( &prediction->simage_pred[1] ) )
	{
		goto fail ;
	}
	if ( x265_simage_init ( &prediction->image_temp_cand ) )
	{
		goto fail ;
	}
	if ( x265_image_init ( &prediction->image_pred_temp ) )
	{
		goto fail ;
	}

	if ( x265_interpolation_filter_init(h, &prediction->interpolation_filter) )
	{
		goto fail ;
	}

    return 0 ;

fail:
	x265_prediction_deinit ( prediction ) ;
	return -1 ;
}

void x265_prediction_deinit ( x265_prediction_t *prediction )
{
	int i = 0, j = 0 ;

	x265_prediction_deinit_temp_buff ( prediction ) ;

	for (i = 0; i < 4; i++)
	{
		x265_simage_deinit ( &prediction->filtered_block_tmp[i] ) ;
		for (j = 0; j < 4; j++)
		{
			x265_image_deinit ( &prediction->filtered_block[i][j] ) ;
		}
	}

	x265_image_deinit ( &prediction->image_pred[0] ) ;
	x265_image_deinit ( &prediction->image_pred[1] ) ;
	x265_simage_deinit ( &prediction->image_temp_cand ) ;
	x265_simage_deinit ( &prediction->simage_pred[0] ) ;
	x265_simage_deinit ( &prediction->simage_pred[1] ) ;
	x265_image_deinit ( &prediction->image_pred_temp ) ;
}


int x265_prediction_init_temp_buff( x265_t *h, x265_prediction_t *prediction )
{
	int32_t i_ext_i_width = 0 ;
	int32_t i_ext_i_height = 0 ;
	int32_t i = 0, j = 0 ;
	int32_t i_aligned_pixel = 0 ;

	i_aligned_pixel = 16 / sizeof(pixel) ;
	x265_prediction_deinit_temp_buff ( prediction ) ;
	x265_intra_pred_initialize (&prediction->intra_pred, h->param.cpu) ;

	if( prediction->image_ext == NULL )
	{
		i_ext_i_width = h->cu.pic.i_max_cu_width + 16;
		i_ext_i_height = h->cu.pic.i_max_cu_height + 1;
		for (i = 0; i < 4; i++)
		{
			if ( x265_simage_create ( &prediction->filtered_block_tmp[i],
									i_ext_i_width, i_ext_i_height + 7) )
			{
				goto fail ;
			}
			for (j = 0; j < 4; j++)
			{
				if ( x265_image_create ( &prediction->filtered_block[i][j],
										i_ext_i_width, i_ext_i_height ) )
				{
					goto fail ;
				}
			}
		}
		prediction->i_image_ext_height = 4;
		prediction->i_image_ext_stride = (((h->cu.pic.i_max_cu_width * 2 + 1 ) + i_aligned_pixel - 1)
											+ (i_aligned_pixel - 1)) & (~(i_aligned_pixel - 1));
		CHECKED_MALLOCZERO ( prediction->image_ext_buffer,
							prediction->i_image_ext_stride * prediction->i_image_ext_height
							* sizeof (pixel) );
		prediction->image_ext = prediction->image_ext_buffer + i_aligned_pixel ;

		if ( x265_image_create ( &prediction->image_pred[0],
								h->cu.pic.i_max_cu_width, h->cu.pic.i_max_cu_height ) )
		{
			goto fail ;
		}
		if ( x265_image_create ( &prediction->image_pred[1],
								h->cu.pic.i_max_cu_width, h->cu.pic.i_max_cu_height ) )
		{
			goto fail ;
		}
		if ( x265_simage_create ( &prediction->simage_pred[0],
								h->cu.pic.i_max_cu_width, h->cu.pic.i_max_cu_height ) )
		{
			goto fail ;
		}
		if ( x265_simage_create ( &prediction->simage_pred[1],
								h->cu.pic.i_max_cu_width, h->cu.pic.i_max_cu_height ) )
		{
			goto fail ;
		}
		if ( x265_simage_create ( &prediction->image_temp_cand,
								h->cu.pic.i_max_cu_width, h->cu.pic.i_max_cu_height ) )
		{
			goto fail ;
		}
		if ( x265_image_create ( &prediction->image_pred_temp,
								h->cu.pic.i_max_cu_width, h->cu.pic.i_max_cu_height ) )
		{
			goto fail ;
		}
	}

	if (prediction->i_luma_rec_stride != (h->cu.pic.i_max_cu_width>>1) + 1)
	{
		prediction->i_luma_rec_stride =  (h->cu.pic.i_max_cu_width>>1) + 1;
		if (!prediction->luma_rec_buffer)
		{
			CHECKED_MALLOCZERO ( prediction->luma_rec_buffer,
								prediction->i_luma_rec_stride * prediction->i_luma_rec_stride
								* sizeof (pixel) );
		}
	}

	return 0 ;

fail:
	x265_prediction_deinit_temp_buff ( prediction ) ;
	return -1 ;
}

void x265_prediction_deinit_temp_buff( x265_prediction_t *prediction )
{
	int i = 0, j = 0 ;

	for (i = 0; i < 4; i++)
	{
		x265_simage_destroy ( &prediction->filtered_block_tmp[i] ) ;
		for (j = 0; j < 4; j++)
		{
			x265_image_destroy ( &prediction->filtered_block[i][j] ) ;
		}
	}

	x265_image_destroy ( &prediction->image_pred[0] ) ;
	x265_image_destroy ( &prediction->image_pred[1] ) ;
	x265_simage_destroy ( &prediction->image_temp_cand ) ;
	x265_simage_destroy ( &prediction->simage_pred[0] ) ;
	x265_simage_destroy ( &prediction->simage_pred[1] ) ;
	x265_image_destroy ( &prediction->image_pred_temp ) ;
	x265_free ( prediction->luma_rec_buffer ) ;
	x265_free ( prediction->image_ext_buffer ) ;

	prediction->luma_rec_buffer = NULL ;
	prediction->image_ext_buffer = NULL ;
	prediction->image_ext = NULL ;
}



// function for calculating dc value of the reference samples used in _intra prediction
pixel x265_prediction_pred_intra_get_pred_val_dc( pixel *left,
												pixel *top,
												uint32_t i_width,
												uint32_t i_height,
												int32_t b_above,
												int32_t b_left )
{
	int32_t ind, i_sum = 0;
	pixel p_dc_val;

	if (b_above)
	{
		for (ind = 0;ind < i_width;ind++)
		{
			i_sum += top[ind];
		}
	}
	if (b_left)
	{
		for (ind = 0;ind < i_height;ind++)
		{
			i_sum += left[ind];
		}
	}

	if (b_above && b_left)
	{
		p_dc_val = (i_sum + i_width) / (i_width + i_height);
	}
	else if (b_above)
	{
		p_dc_val = (i_sum + i_width/2) / i_width;
	}
	else if (b_left)
	{
		p_dc_val = (i_sum + i_height/2) / i_height;
	}
	else
	{
		p_dc_val = left[0]; // default dc value already calculated and placed in the prediction array if no neighbors are available
	}

	return p_dc_val;
}


void x265_intra_pred_angular (pixel *_src, pixel *_top,
								pixel *_left,
								int32_t stride, int i_chroma_idx,
								int i_mode, int size)
{
    int x, y;
    pixel *src = (pixel*)_src;
    pixel *top  = (pixel*)_top;
    pixel *left = (pixel*)_left;

    static const int intra_pred_angle[] = {
        32, 26, 21, 17, 13, 9, 5, 2, 0, -2, -5, -9, -13, -17, -21, -26, -32,
        -26, -21, -17, -13, -9, -5, -2, 0, 2, 5, 9, 13, 17, 21, 26, 32
    };
    static const int inv_angle[] = {
        -4096, -1638, -910, -630, -482, -390, -315, -256, -315, -390, -482,
        -630, -910, -1638, -4096
    };

    int angle = intra_pred_angle[i_mode-2];
    pixel ref_array[3*X265_MAX_CU_SIZE+1];
    const pixel *ref;
    int last = (size * angle) >> 5;

    if (i_mode >= 18) {
        ref = top - 1;
        if (angle < 0 && last < -1) {
            for (x = 0; x <= size; x++)
                (ref_array + size)[x] = top[x - 1];
            for (x = last; x <= -1; x++)
                (ref_array + size)[x] = left[-1 + ((x * inv_angle[i_mode-11] + 128) >> 8)];
            ref = ref_array + size;
        }

        for (y = 0; y < size; y++) {
            int idx = ((y + 1) * angle) >> 5;
            int fact = ((y + 1) * angle) & 31;
            if (fact) {
                for (x = 0; x < size; x++) {
                	src[y*stride+x] = ((32 - fact) * ref[x + idx + 1] + fact * ref[x + idx + 2] + 16) >> 5;
                }
            } else {
                for (x = 0; x < size; x++) {
                	src[y*stride+x] = ref[x + idx + 1];
                }
            }
        }
        if (i_mode == 26 && i_chroma_idx == 0 && size < 32) {
            for (y = 0; y < size; y++)
            	src[y*stride+0] = x265_clip3_pixel(top[0] + ((left[y] - left[-1]) >> 1), 0, 255);
        }
    } else {
        ref = left - 1;
        if (angle < 0 && last < -1) {
            for (x = 0; x <= size; x++)
                (ref_array + size)[x] = left[x - 1];
            for (x = last; x <= -1; x++)
                (ref_array + size)[x] = top[-1 + ((x * inv_angle[i_mode-11] + 128) >> 8)];
            ref = ref_array + size;
        }

        for (x = 0; x < size; x++) {
            int idx = ((x + 1) * angle) >> 5;
            int fact = ((x + 1) * angle) & 31;
            if (fact) {
                for (y = 0; y < size; y++) {
                	src[y*stride+x] = ((32 - fact) * ref[y + idx + 1] + fact * ref[y + idx + 2] + 16) >> 5;
                }
            } else {
                for (y = 0; y < size; y++) {
                	src[y*stride+x] = ref[y + idx + 1];
                }
            }
        }
        if (i_mode == 10 && i_chroma_idx == 0 && size < 32) {
            for (x = 0; x < size; x++)
            	src[x] = x265_clip3_pixel(left[0] + ((top[x] - top[-1]) >> 1), 0, 255);
        }
    }
}

// function for deriving the angular _intra predictions

/** function for deriving the simplified angular intra predictions.
 * \param p_src pointer to reconstructed sample array
 * \param i_src_stride the stride of the reconstructed sample array
 * \param p_dst reference to pointer for the prediction sample array
 * \param i_dst_stride the stride of the prediction sample array
 * \param i_width the i_width of the block
 * \param i_height the i_height of the block
 * \param i_dir_mode the intra prediction mode index
 * \param blk_above_available boolean indication if the block above is available
 * \param blk_left_available boolean indication if the block to the left is available
 *
 * this function derives the prediction samples for the angular mode based on the prediction direction indicated by
 * the prediction mode index. the prediction direction is given by the displacement of the bottom row of the block and
 * the reference row above the block in the case of vertical prediction or displacement of the rightmost column
 * of the block and reference column left from the block in the case of the horizontal prediction. the displacement
 * is signalled at 1/32 pixel accuracy. when projection of the predicted pixel falls inbetween reference samples,
 * the predicted value for the pixel is linearly interpolated from the reference samples. all reference samples are taken
 * from the extended main reference.
 */
void x265_prediction_x_pred_intra_ang(int32_t bit_depth,
									pixel *left,
									pixel *top,
									pixel** pp_dst,
									int32_t i_dst_stride,
									uint32_t i_width,
									uint32_t i_height,
									uint32_t i_dir_mode,
									int32_t b_blk_above_available,
									int32_t b_blk_left_available,
									int32_t b_filter )
{
	int32_t k,l;
	int32_t i_blk_size = 0;
	pixel* p_dst = NULL;
	// map the mode index to main prediction direction and angle

	int32_t b_mode_dc = 0;
	int32_t b_mode_hor = 0;
	int32_t b_mode_ver = 0;
	int32_t i_intra_pred_angle = 0;
	int32_t i_abs_ang = 0;
	int32_t i_sign_ang = 0;
	int32_t i_inv_angle = 0;
	pixel i_dc_val = 0;
	int32_t i_inv_angle_sum = 0;       // rounding for (shift by 8)
	int32_t i_delta_pos = 0;
	int32_t i_delta_int = 0;
	int32_t i_delta_fract = 0;
	int32_t i_ref_main_index = 0;
	pixel i_tmp = 0;
	// set bitshifts and scale the angle parameter to block size
	int32_t ang_table[9]    = {0,    2,    5,   9,  13,  17,  21,  26,  32};
	int32_t inv_ang_table[9] = {0, 4096, 1638, 910, 630, 482, 390, 315, 256}; // (256 * 32) / angle
	pixel *ref_main;
	pixel *ref_side;
	pixel ref_above[2*X265_MAX_CU_SIZE+1];
	pixel ref_left[2*X265_MAX_CU_SIZE+1];




	i_blk_size = i_width;
	p_dst = *pp_dst;
	// map the mode index to main prediction direction and angle
	assert( i_dir_mode > 0 ); //no planar
	b_mode_dc = i_dir_mode < 2;
	b_mode_hor = !b_mode_dc && (i_dir_mode < 18);
	b_mode_ver = !b_mode_dc && !b_mode_hor;
	i_intra_pred_angle = b_mode_ver ? (int32_t)i_dir_mode - X265_VER_IDX : b_mode_hor ? -((int32_t)i_dir_mode - X265_HOR_IDX) : 0;
	i_abs_ang = abs(i_intra_pred_angle);
	i_sign_ang = i_intra_pred_angle < 0 ? -1 : 1;

	// set bitshifts and scale the angle parameter to block size
	i_inv_angle = inv_ang_table[i_abs_ang];
	i_abs_ang = ang_table[i_abs_ang];
	i_intra_pred_angle = i_sign_ang * i_abs_ang;

	// do the dc prediction
	if (b_mode_dc)
	{
		i_dc_val = x265_prediction_pred_intra_get_pred_val_dc(left,
															top,
															i_width,
															i_height,
															b_blk_above_available,
															b_blk_left_available);

		for (k=0;k<i_blk_size;k++)
		{
			for (l=0;l<i_blk_size;l++)
			{
				p_dst[k*i_dst_stride+l] = i_dc_val;
			}
		}
	}
	// do angular predictions
	else
	{
		// initialise the main and left reference array.
		if (i_intra_pred_angle < 0)
		{
			memcpy (ref_above + i_blk_size - 1, top-1, sizeof(pixel) * (i_blk_size + 1));
			memcpy (ref_left + i_blk_size - 1, left-1, sizeof(pixel) * (i_blk_size + 1));
			ref_main = (b_mode_ver ? ref_above : ref_left) + (i_blk_size-1);
			ref_side = (b_mode_ver ? ref_left : ref_above) + (i_blk_size-1);

			// extend the main reference to the left.
			i_inv_angle_sum    = 128;       // rounding for (shift by 8)
			for (k=-1; k>i_blk_size*i_intra_pred_angle>>5; k--)
			{
				i_inv_angle_sum += i_inv_angle;
				ref_main[k] = ref_side[i_inv_angle_sum>>8];
			}
		}
		else
		{
			memcpy (ref_above, top-1, sizeof(pixel) * (2 * i_blk_size + 1));
			memcpy (ref_left, left-1, sizeof(pixel) * (2 * i_blk_size + 1));
			ref_main = b_mode_ver ? ref_above : ref_left;
			ref_side = b_mode_ver ? ref_left  : ref_above;
		}

		if (i_intra_pred_angle == 0)
		{
			for (k=0;k<i_blk_size;k++)
			{
				for (l=0;l<i_blk_size;l++)
				{
					p_dst[k*i_dst_stride+l] = ref_main[l+1];
				}
			}

			if ( b_filter )
			{
				for (k=0;k<i_blk_size;k++)
				{
					p_dst[k*i_dst_stride] = x265_clip3_int32(p_dst[k*i_dst_stride] + (( ref_side[k+1] - ref_side[0] ) >> 1),
															0,
															(1<<bit_depth)-1);
				}
			}
		}
		else
		{
			for (k=0;k<i_blk_size;k++)
			{
				i_delta_pos += i_intra_pred_angle;
				i_delta_int   = i_delta_pos >> 5;
				i_delta_fract = i_delta_pos & (32 - 1);

				if (i_delta_fract)
				{
					// do linear filtering
					for (l=0;l<i_blk_size;l++)
					{
						i_ref_main_index        = l+i_delta_int+1;
						p_dst[k*i_dst_stride+l] = (pixel) ( ((32-i_delta_fract)*ref_main[i_ref_main_index]+i_delta_fract*ref_main[i_ref_main_index+1]+16) >> 5 );
					}
				}
				else
				{
					// just copy the integer samples
					for (l=0;l<i_blk_size;l++)
					{
						p_dst[k*i_dst_stride+l] = ref_main[l+i_delta_int+1];
					}
				}
			}
		}

		// flip the block if this is the horizontal mode
		if (b_mode_hor)
		{
			for (k=0;k<i_blk_size-1;k++)
			{
				for (l=k+1;l<i_blk_size;l++)
				{
					i_tmp = p_dst[k*i_dst_stride+l];
					p_dst[k*i_dst_stride+l] = p_dst[l*i_dst_stride+k];
					p_dst[l*i_dst_stride+k] = i_tmp;
				}
			}
		}
	}
}

void x265_prediction_pred_intra_luma_ang( x265_t* h,
										x265_prediction_t* prediction,
										x265_pattern_t* pattern,
										uint32_t i_dir_mode,
										pixel* p_pred,
										uint32_t i_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t b_above,
										int32_t b_left )
{
	int32_t i_index = 0 ;
	pixel *p_dst = NULL;
	pixel *left = NULL;
	pixel *top = NULL;
	pixel *p_src;
	x265_intra_pred_t *ip = NULL ;

	p_dst = p_pred;
	assert( h->global.convert_to_bit[ i_width ] >= 0 ); //   4x  4
	assert( h->global.convert_to_bit[ i_width ] <= 5 ); // 128x128
	assert( i_width == i_height  );
	p_src = x265_pattern_get_predictor_ptr(pattern,
											i_dir_mode,
											h->global.convert_to_bit[ i_width ] + 2,
											prediction->image_ext,
											prediction->i_image_ext_stride);
	left = p_src ;
	top = left + prediction->i_image_ext_stride ;
	i_index = h->global.convert_to_bit[i_width] ;
	ip = &prediction->intra_pred ;
	// get starting pixel in block
	// create the prediction
	ip->intra_pred_func[i_index][i_dir_mode](left,
											top,
											p_dst,
											i_stride,
											0,
											h->param.sps.i_bit_depth_y ) ;
}

// angular chroma
void x265_prediction_pred_intra_chroma_ang( x265_t* h,
											x265_prediction_t* prediction,
											pixel* src,
											uint32_t i_dir_mode,
											pixel* pred,
											uint32_t i_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_above,
											int32_t b_left )
{
	int32_t i_index = 0 ;
	pixel *p_dst = NULL;
	pixel *p_src = NULL;
	pixel *left = NULL;
	pixel *top = NULL;
	x265_intra_pred_t *ip = NULL ;

	// get starting pixel in block
	p_dst = pred;
	p_src = src;
	left = p_src ;
	top = left + prediction->i_image_ext_stride ;
	i_index = h->global.convert_to_bit[i_width] ;
	ip = &prediction->intra_pred ;
	// get starting pixel in block
	ip->intra_pred_func[i_index][i_dir_mode](left,
											top,
											p_dst,
											i_stride,
											1,
											h->param.sps.i_bit_depth_c ) ;
}

/** function for deriving planar intra prediction.
 * \param p_src pointer to reconstructed sample array
 * \param i_src_stride the stride of the reconstructed sample array
 * \param p_dst reference to pointer for the prediction sample array
 * \param i_dst_stride the stride of the prediction sample array
 * \param i_width the i_width of the block
 * \param i_height the i_height of the block
 *
 * this function derives the prediction samples for planar mode (intra coding).
 */
void x265_prediction_x_pred_intra_planar(x265_t* h,
										pixel* p_src,
										pixel* p_dst,
										int32_t i_dst_stride,
										uint32_t i_width,
										uint32_t i_height )
{
	int32_t k, l, i_bottom_left, i_top_right;
	int32_t i_hor_pred;
	pixel left_column[X265_MAX_CU_SIZE], top_row[X265_MAX_CU_SIZE], bottom_row[X265_MAX_CU_SIZE], right_column[X265_MAX_CU_SIZE];
	uint32_t i_blk_size = 0;
	uint32_t i_offset_2d = 0;
	uint32_t i_shift_1d = 0;
	uint32_t i_shift_2d = 0;

	i_blk_size = i_width;
	i_offset_2d = i_width;
	i_shift_1d = h->global.convert_to_bit[ i_width ] + 2;
	i_shift_2d = i_shift_1d + 1;

	assert(i_width == i_height);
	// get left and above reference column and row
	memcpy (left_column, p_src+1, sizeof(pixel) * (i_blk_size+1) ) ;
	memcpy (top_row, p_src + i_blk_size * 2 + 1 + 1, sizeof(pixel) * (i_blk_size+1) ) ;

	// prepare intermediate variables used in interpolation
	i_bottom_left = left_column[i_blk_size];
	i_top_right   = top_row[i_blk_size];
	for (k=0;k<i_blk_size;k++)
	{
		bottom_row[k]   = i_bottom_left - top_row[k];
		right_column[k] = i_top_right   - left_column[k];
		top_row[k]      <<= i_shift_1d;
		left_column[k]  <<= i_shift_1d;
	}

	// generate prediction signal
	for (k=0;k<i_blk_size;k++)
	{
		i_hor_pred = left_column[k] + i_offset_2d;
		for (l=0;l<i_blk_size;l++)
		{
			i_hor_pred += right_column[k];
			top_row[l] += bottom_row[l];
			p_dst[k*i_dst_stride+l] = ( (i_hor_pred + top_row[l]) >> i_shift_2d );
		}
	}
}

/** function for filtering intra dc predictor.
 * \param p_src pointer to reconstructed sample array
 * \param i_src_stride the stride of the reconstructed sample array
 * \param p_dst reference to pointer for the prediction sample array
 * \param i_dst_stride the stride of the prediction sample array
 * \param i_width the i_width of the block
 * \param i_height the i_height of the block
 *
 * this function performs filtering left and top edges of the prediction samples for dc mode (intra coding).
 */
void x265_prediction_x_dc_pred_filtering(pixel *left,
										pixel *top,
										pixel **pp_dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height )
{
	pixel *p_dst = NULL;
	int32_t x, y, i_dst_stride2;
	p_dst = *pp_dst;
	// boundary pixels processing
	p_dst[0] = (pixel)((left[0] + top[0] + 2 * p_dst[0] + 2) >> 2);

	for ( x = 1; x < i_width; x++ )
	{
		p_dst[x] = (pixel)((top[x] +  3 * p_dst[x] + 2) >> 2);
	}

	for ( y = 1, i_dst_stride2 = i_dst_stride; y < i_height ; ++ y, i_dst_stride2+=i_dst_stride )
	{
		p_dst[i_dst_stride2] = (pixel)((left[y] + 3 * p_dst[i_dst_stride2] + 2) >> 2);
	}

	return;
}


/** function for checking identical motion.
 * \param x265_data_cu_t* cu
 * \param uint32_t part_addr
 */
int32_t x265_prediction_x_check_identical_motion(x265_t *h,
												x265_data_cu_t* cu,
												uint32_t i_part_addr )
{
	int32_t i_ref_poc_l0 = 0;
	int32_t i_ref_poc_l1 = 0;

	if(B_SLICE == h->slice->i_slice_type
		&& !h->pps[0].b_use_weighted_bi_pred )
	{
		if( x265_cu_mv_field_get_ref_idx(
			x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
										i_part_addr) >= 0
			&& x265_cu_mv_field_get_ref_idx(
				x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
												i_part_addr ) >= 0)
		{
			i_ref_poc_l0 = h->fref[REF_PIC_LIST_0]
			               [x265_cu_mv_field_get_ref_idx(
			            	x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
														i_part_addr)
			                ]->i_poc ;
			i_ref_poc_l1 = h->fref[REF_PIC_LIST_1]
			               [x265_cu_mv_field_get_ref_idx(
			            	x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
														i_part_addr)
			                ]->i_poc ;

			if(i_ref_poc_l0 == i_ref_poc_l1
				&&
				x265_mv_is_equal_mv(x265_cu_mv_field_get_mv(
									x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
														i_part_addr ),
									x265_cu_mv_field_get_mv(
									x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
														i_part_addr ) )
				)
			{
				return 1;
			}
		}
	}
	return 0;
}


void x265_prediction_motion_compensation(x265_t *h,
										x265_prediction_t* prediction,
										x265_data_cu_t *cu,
										x265_image_t* p_image_pred,
										enum ref_pic_list_e i_ref_pic_list,
										int32_t i_part_idx )
{
	int32_t         i_width;
	int32_t         i_height;
	uint32_t        i_part_addr;
	x265_simage_t *simage_pred = NULL ;

	if ( i_part_idx >= 0 )
	{
		x265_data_cu_get_part_index_and_size(cu,
											i_part_idx,
											&i_part_addr,
											&i_width,
											&i_height );
		if ( i_ref_pic_list != REF_PIC_LIST_X )
		{
			if( h->pps[0].b_use_weight_pred )
			{
				simage_pred = &prediction->simage_pred[0] ;
				x265_prediction_x_pred_inter_uni_s(h,
													prediction,
													cu,
													i_part_addr,
													i_width,
													i_height,
													i_ref_pic_list,
													&simage_pred,
													1 );
			}
			else
			{
				x265_prediction_x_pred_inter_uni_p(h,
													prediction,
													cu,
													i_part_addr,
													i_width,
													i_height,
													i_ref_pic_list,
													&p_image_pred,
													0 );
			}
			if ( h->pps[0].b_use_weight_pred )
			{
				x265_weight_prediction_x_weighted_prediction_uni(h,
																(x265_weight_prediction_t*)prediction,
																cu,
																&prediction->simage_pred[0],
																i_part_addr,
																i_width,
																i_height,
																i_ref_pic_list,
																&p_image_pred,
																-1 );
			}
		}
		else
		{
			if ( x265_prediction_x_check_identical_motion(h, cu, i_part_addr ) )
			{
				x265_prediction_x_pred_inter_uni_p(h,
													prediction,
													cu,
													i_part_addr,
													i_width,
													i_height,
													REF_PIC_LIST_0,
													&p_image_pred,
													0 );
			}
			else
			{
				x265_prediction_x_pred_inter_bi(h,
												prediction,
												cu,
												i_part_addr,
												i_width,
												i_height,
												&p_image_pred );
			}
		}
		return;
	}

	for ( i_part_idx = 0; i_part_idx < x265_data_cu_get_num_part_inter(cu); i_part_idx++ )
	{
		x265_data_cu_get_part_index_and_size(cu,
											i_part_idx,
											&i_part_addr,
											&i_width,
											&i_height );
		if ( i_ref_pic_list != REF_PIC_LIST_X )
		{
			if ( h->pps[0].b_use_weight_pred )
			{
				simage_pred = &prediction->simage_pred[0] ;
				x265_prediction_x_pred_inter_uni_s(h,
												prediction,
												cu,
												i_part_addr,
												i_width,
												i_height,
												i_ref_pic_list,
												&simage_pred,
												1 );
			}
			else
			{
				x265_prediction_x_pred_inter_uni_p(h,
												prediction,
												cu,
												i_part_addr,
												i_width,
												i_height,
												i_ref_pic_list,
												&p_image_pred,
												0 );
			}
			if ( h->pps[0].b_use_weight_pred )
			{
				x265_weight_prediction_x_weighted_prediction_uni(h,
																(x265_weight_prediction_t*)prediction,
																cu,
																&prediction->simage_pred[0],
																i_part_addr,
																i_width,
																i_height,
																i_ref_pic_list,
																&p_image_pred,
																-1 );
			}
		}
		else
		{
			if ( x265_prediction_x_check_identical_motion(h, cu, i_part_addr ) )
			{
				x265_prediction_x_pred_inter_uni_p(h,
													prediction,
													cu,
													i_part_addr,
													i_width,
													i_height,
													REF_PIC_LIST_0,
													&p_image_pred,
													0 );
			}
			else
			{
				x265_prediction_x_pred_inter_bi(h,
												prediction,
												cu,
												i_part_addr,
												i_width,
												i_height,
												&p_image_pred );
			}
		}
	}
	return;
}

void x265_prediction_x_pred_inter_uni_p ( x265_t *h,
										x265_prediction_t* prediction,
										x265_data_cu_t *cu,
										uint32_t i_part_addr,
										int32_t i_width,
										int32_t i_height,
										enum ref_pic_list_e i_ref_pic_list,
										x265_image_t **pp_image_pred,
										int32_t b_bi )
{
	int32_t i_ref_idx = 0;
	x265_mv_t mv ;

	i_ref_idx = x265_cu_mv_field_get_ref_idx(
        	x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list),
										i_part_addr) ;
	assert (i_ref_idx >= 0);
	memcpy(&mv,
			x265_cu_mv_field_get_mv(
        	x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list),
										i_part_addr),
			sizeof(x265_mv_t) );


	x265_data_cu_clip_mv(h, cu, &mv);
	x265_prediction_x_pred_inter_luma_blk_p(h,
											prediction,
											cu,
											h->fref[i_ref_pic_list][i_ref_idx],
											i_part_addr,
											&mv,
											i_width,
											i_height,
											pp_image_pred,
											b_bi );
	x265_prediction_x_pred_inter_chroma_blk_p(h,
											prediction,
											cu,
											h->fref[i_ref_pic_list][i_ref_idx],
											i_part_addr,
											&mv,
											i_width,
											i_height,
											pp_image_pred,
											b_bi );
}

void x265_prediction_x_pred_inter_uni_s ( x265_t *h,
										x265_prediction_t* prediction,
										x265_data_cu_t *cu,
										uint32_t i_part_addr,
										int32_t i_width,
										int32_t i_height,
										enum ref_pic_list_e i_ref_pic_list,
										x265_simage_t **pp_simage_pred,
										int32_t b_bi )
{
	int32_t i_ref_idx = 0;
	x265_mv_t mv ;

	i_ref_idx = x265_cu_mv_field_get_ref_idx(
        	x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list),
										i_part_addr) ;
	assert (i_ref_idx >= 0);
	memcpy(&mv,
			x265_cu_mv_field_get_mv(
        	x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list),
										i_part_addr),
			sizeof(x265_mv_t) );


	x265_data_cu_clip_mv(h, cu, &mv);
	x265_prediction_x_pred_inter_luma_blk_s(h,
											prediction,
											cu,
											h->fref[i_ref_pic_list][i_ref_idx],
											i_part_addr,
											&mv,
											i_width,
											i_height,
											pp_simage_pred,
											b_bi );
	x265_prediction_x_pred_inter_chroma_blk_s(h,
											prediction,
											cu,
											h->fref[i_ref_pic_list][i_ref_idx],
											i_part_addr,
											&mv,
											i_width,
											i_height,
											pp_simage_pred,
											b_bi );
}

void x265_prediction_x_pred_inter_bi ( x265_t *h,
										x265_prediction_t* prediction,
										x265_data_cu_t* cu,
										uint32_t i_part_addr,
										int32_t i_width,
										int32_t i_height,
										x265_image_t **pp_image_pred )
{
	x265_simage_t *p_mb_simage;
	int32_t      i_ref_idx[2] = {-1, -1};

	for ( int32_t i_ref_list = 0; i_ref_list < 2; i_ref_list++ )
	{
		enum ref_pic_list_e i_ref_pic_list = (i_ref_list ? REF_PIC_LIST_1 : REF_PIC_LIST_0);
		i_ref_idx[i_ref_list] = x265_cu_mv_field_get_ref_idx(
								x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list),
															i_part_addr) ;
		if ( i_ref_idx[i_ref_list] < 0 )
		{
			continue;
		}

		assert( i_ref_idx[i_ref_list] < h->i_ref[i_ref_pic_list] );

		p_mb_simage = &prediction->simage_pred[i_ref_list];
		if( x265_cu_mv_field_get_ref_idx(
			x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
											i_part_addr) >= 0
			&& x265_cu_mv_field_get_ref_idx(
			x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
											i_part_addr) >= 0 )
		{
			x265_prediction_x_pred_inter_uni_s(h,
												prediction,
												cu,
												i_part_addr,
												i_width,
												i_height,
												i_ref_pic_list,
												&p_mb_simage,
												1 );
		}
		else
		{
			if ( (h->pps[0].b_use_weight_pred
					&& h->slice->i_slice_type == P_SLICE ) ||
			   ( h->pps[0].b_use_weighted_bi_pred
					&& h->slice->i_slice_type == B_SLICE ) )
			{
				x265_prediction_x_pred_inter_uni_s(h,
													prediction,
													cu,
													i_part_addr,
													i_width,
													i_height,
													i_ref_pic_list,
													&p_mb_simage,
													1 );
			}
			else
			{
				x265_prediction_x_pred_inter_uni_p(h,
													prediction,
													cu,
													i_part_addr,
													i_width,
													i_height,
													i_ref_pic_list,
													pp_image_pred,
													0 );
			}
		}
	}

	if ( h->pps[0].b_use_weighted_bi_pred
			&& h->slice->i_slice_type == B_SLICE  )
	{
		x265_weight_prediction_x_weighted_prediction_bi(h,
														(x265_weight_prediction_t*)prediction,
														cu,
														&prediction->simage_pred[0],
														&prediction->simage_pred[1],
														i_ref_idx[0],
														i_ref_idx[1],
														i_part_addr,
														i_width,
														i_height,
														*pp_image_pred );
	}
	else if ( h->pps[0].b_use_weight_pred
			&& h->slice->i_slice_type == P_SLICE )
	{
		x265_weight_prediction_x_weighted_prediction_uni(h,
														(x265_weight_prediction_t*)prediction,
														cu,
														&prediction->simage_pred[0],
														i_part_addr,
														i_width,
														i_height,
														REF_PIC_LIST_0,
														pp_image_pred,
														-1 );
	}
	else if ((i_ref_idx[0] >= 0) && (i_ref_idx[1] >= 0))
	{
		x265_prediction_x_weighted_average(h,
											&prediction->simage_pred[0],
											&prediction->simage_pred[1],
											i_ref_idx[0],
											i_ref_idx[1],
											i_part_addr,
											i_width,
											i_height,
											pp_image_pred );
	}
}

/**
 * \brief generate motion-compensated luma block
 *
 * \param cu       pointer to current cu
 * \param ref_pic   pointer to reference picture
 * \param part_addr address of block within cu
 * \param mv       motion vector
 * \param i_width    i_width of block
 * \param i_height   i_height of block
 * \param dst_pic   pointer to destination picture
 * \param bi       flag indicating whether bipred is used
 */
void x265_prediction_x_pred_inter_luma_blk_p( x265_t *h,
											x265_prediction_t* prediction,
											x265_data_cu_t *cu,
											x265_frame_t *ref_pic,
											uint32_t i_part_addr,
											x265_mv_t *mv,
											int32_t i_width,
											int32_t i_height,
											x265_image_t **pp_dst_image,
											int32_t b_bi )
{
	int32_t i_ref_stride = 0;
	int32_t i_ref_offset = 0;
	pixel *ref = 0;
	int32_t i_dst_stride = 0;
	pixel *dst = 0;
	int32_t i_x_frac = 0;
	int32_t i_y_frac = 0;
	int32_t i_tmp_stride = 0;
	int16_t *tmp = 0;
	int32_t i_filter_size = 0;
	int32_t i_half_filter_size = 0;



	i_ref_stride = ref_pic->i_stride[0];
	i_ref_offset = ( mv->i_hor >> 2 ) + ( mv->i_ver >> 2 ) * i_ref_stride;
	ref = x265_frame_get_luma_addr_p4(h,
									ref_pic,
									x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
									x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_addr )
			+ i_ref_offset;

	i_dst_stride = x265_image_get_stride(*pp_dst_image);
	dst = x265_image_get_luma_addr_p3(h,
									*pp_dst_image,
									i_part_addr );

	i_x_frac = mv->i_hor & 0x3;
	i_y_frac = mv->i_ver & 0x3;


	if ( i_y_frac == 0 )
	{
		x265_interpolation_filter_filter_hor_luma_p_p(h,
												&prediction->interpolation_filter,
												ref,
												i_ref_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												i_x_frac,
												!b_bi );
	}
	else if ( i_x_frac == 0 )
	{
		x265_interpolation_filter_filter_ver_luma_p_p(h,
												&prediction->interpolation_filter,
												ref,
												i_ref_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												i_y_frac,
												1,
												!b_bi );
	}
	else
	{
		i_tmp_stride = x265_simage_get_stride(&prediction->filtered_block_tmp[0]);
		tmp = x265_simage_get_luma_addr_p2(h,
										&prediction->filtered_block_tmp[0]);

		i_filter_size = X265_NTAPS_LUMA;
		i_half_filter_size = ( i_filter_size >> 1 );

		x265_interpolation_filter_filter_hor_luma_p_s(h,
												&prediction->interpolation_filter,
												ref - (i_half_filter_size - 1) * i_ref_stride,
												i_ref_stride,
												tmp,
												i_tmp_stride,
												i_width,
												i_height + i_filter_size - 1,
												i_x_frac,
												0 );
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&prediction->interpolation_filter,
												tmp + (i_half_filter_size - 1) * i_tmp_stride,
												i_tmp_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												i_y_frac,
												0,
												!b_bi);
	}
}

/**
 * \brief generate motion-compensated luma block
 *
 * \param cu       pointer to current cu
 * \param ref_pic   pointer to reference picture
 * \param part_addr address of block within cu
 * \param mv       motion vector
 * \param i_width    i_width of block
 * \param i_height   i_height of block
 * \param dst_pic   pointer to destination picture
 * \param bi       flag indicating whether bipred is used
 */
void x265_prediction_x_pred_inter_luma_blk_s( x265_t *h,
											x265_prediction_t* prediction,
											x265_data_cu_t *cu,
											x265_frame_t *ref_pic,
											uint32_t i_part_addr,
											x265_mv_t *mv,
											int32_t i_width,
											int32_t i_height,
											x265_simage_t **pp_dst_simage,
											int32_t b_bi )
{
	int32_t i_ref_stride = 0;
	int32_t i_ref_offset = 0;
	pixel *ref = 0;
	int32_t i_dst_stride = 0;
	spixel *dst = 0;
	int32_t i_x_frac = 0;
	int32_t i_y_frac = 0;
	int32_t i_tmp_stride = 0;
	int16_t *tmp = 0;
	int32_t i_filter_size = 0;
	int32_t i_half_filter_size = 0;



	i_ref_stride = ref_pic->i_stride[0];
	i_ref_offset = ( mv->i_hor >> 2 ) + ( mv->i_ver >> 2 ) * i_ref_stride;
	ref = x265_frame_get_luma_addr_p4(h,
									ref_pic,
									x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
									x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_addr )
			+ i_ref_offset;

	i_dst_stride = x265_simage_get_stride(*pp_dst_simage);
	dst = x265_simage_get_luma_addr_p3(h,
									*pp_dst_simage,
									i_part_addr );

	i_x_frac = mv->i_hor & 0x3;
	i_y_frac = mv->i_ver & 0x3;


	if ( i_y_frac == 0 )
	{
		x265_interpolation_filter_filter_hor_luma_p_s(h,
												&prediction->interpolation_filter,
												ref,
												i_ref_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												i_x_frac,
												!b_bi );
	}
	else if ( i_x_frac == 0 )
	{
		x265_interpolation_filter_filter_ver_luma_p_s(h,
												&prediction->interpolation_filter,
												ref,
												i_ref_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												i_y_frac,
												1,
												!b_bi );
	}
	else
	{
		i_tmp_stride = x265_simage_get_stride(&prediction->filtered_block_tmp[0]);
		tmp = x265_simage_get_luma_addr_p2(h,
										&prediction->filtered_block_tmp[0]);

		i_filter_size = X265_NTAPS_LUMA;
		i_half_filter_size = ( i_filter_size >> 1 );

		x265_interpolation_filter_filter_hor_luma_p_s(h,
												&prediction->interpolation_filter,
												ref - (i_half_filter_size - 1) * i_ref_stride,
												i_ref_stride,
												tmp,
												i_tmp_stride,
												i_width,
												i_height + i_filter_size - 1,
												i_x_frac,
												0 );
		x265_interpolation_filter_filter_ver_luma_s_s(h,
												&prediction->interpolation_filter,
												tmp + (i_half_filter_size - 1) * i_tmp_stride,
												i_tmp_stride,
												dst,
												i_dst_stride,
												i_width,
												i_height,
												i_y_frac,
												0,
												!b_bi);
	}
}

/**
 * \brief generate motion-compensated chroma block
 *
 * \param cu       pointer to current cu
 * \param ref_pic   pointer to reference picture
 * \param part_addr address of block within cu
 * \param mv       motion vector
 * \param i_width    i_width of block
 * \param i_height   i_height of block
 * \param dst_pic   pointer to destination picture
 * \param bi       flag indicating whether bipred is used
 */
void x265_prediction_x_pred_inter_chroma_blk_p(x265_t *h,
											x265_prediction_t* prediction,
											x265_data_cu_t *cu,
											x265_frame_t *ref_pic,
											uint32_t i_part_addr,
											x265_mv_t *mv,
											int32_t i_width,
											int32_t i_height,
											x265_image_t **pp_dst_image,
											int32_t b_bi )
{
	int32_t i_ref_stride = 0;
	int32_t i_dst_stride = 0;
	int32_t i_ref_offset = 0;
	pixel* ref_cb = 0;
	pixel* ref_cr = 0;
	pixel* dst_cb = 0;
	pixel* dst_cr = 0;
	int32_t i_x_frac  = 0;
	int32_t i_y_frac  = 0;
	uint32_t i_cx_width = 0;
	uint32_t i_cx_height = 0;
	int32_t i_ext_stride = 0;
	int16_t *ext_y = NULL;
	int32_t i_filter_size = 0;
	int32_t i_half_filter_size = 0;




	i_ref_stride = ref_pic->i_stride[1];
	i_dst_stride = x265_image_get_c_stride(*pp_dst_image);
	i_ref_offset  = (mv->i_hor >> 3) + (mv->i_ver >> 3) * i_ref_stride;

	ref_cb = x265_frame_get_cb_addr_p4(h,
										ref_pic,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_addr )
				+ i_ref_offset;
	ref_cr = x265_frame_get_cr_addr_p4(h,
										ref_pic,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_addr )
				+ i_ref_offset;

	dst_cb = x265_image_get_cb_addr_p3(h,
										*pp_dst_image,
										i_part_addr );
	dst_cr = x265_image_get_cr_addr_p3(h,
										*pp_dst_image,
										i_part_addr );

	i_x_frac = mv->i_hor & 0x7;
	i_y_frac = mv->i_ver & 0x7;
	i_cx_width = i_width  >> 1;
	i_cx_height = i_height >> 1;

	i_ext_stride = x265_simage_get_stride(&prediction->filtered_block_tmp[0]);
	ext_y = x265_simage_get_luma_addr_p2(h, &prediction->filtered_block_tmp[0]);
	i_filter_size = X265_NTAPS_CHROMA;
	i_half_filter_size = (i_filter_size>>1);

	if ( i_y_frac == 0 )
	{
		x265_interpolation_filter_filter_hor_chroma_p_p(h,
													&prediction->interpolation_filter,
													ref_cb,
													i_ref_stride,
													dst_cb,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_x_frac,
													!b_bi);
		x265_interpolation_filter_filter_hor_chroma_p_p(h,
													&prediction->interpolation_filter,
													ref_cr,
													i_ref_stride,
													dst_cr,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_x_frac,
													!b_bi);
	}
	else if ( i_x_frac == 0 )
	{
		x265_interpolation_filter_filter_ver_chroma_p_p(h,
													&prediction->interpolation_filter,
													ref_cb,
													i_ref_stride,
													dst_cb,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_y_frac,
													1,
													!b_bi);
		x265_interpolation_filter_filter_ver_chroma_p_p(h,
													&prediction->interpolation_filter,
													ref_cr,
													i_ref_stride,
													dst_cr,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_y_frac,
													1,
													!b_bi);
	}
	else
	{
		x265_interpolation_filter_filter_hor_chroma_p_s(h,
													&prediction->interpolation_filter,
													ref_cb - (i_half_filter_size - 1) * i_ref_stride,
													i_ref_stride,
													ext_y,
													i_ext_stride,
													i_cx_width,
													i_cx_height + i_filter_size - 1,
													i_x_frac,
													0 ) ;
		x265_interpolation_filter_filter_ver_chroma_s_p(h,
													&prediction->interpolation_filter,
													ext_y  + (i_half_filter_size - 1) * i_ext_stride,
													i_ext_stride,
													dst_cb,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_y_frac,
													0,
													!b_bi);

		x265_interpolation_filter_filter_hor_chroma_p_s(h,
													&prediction->interpolation_filter,
													ref_cr - (i_half_filter_size - 1) * i_ref_stride,
													i_ref_stride,
													ext_y,
													i_ext_stride,
													i_cx_width,
													i_cx_height + i_filter_size - 1,
													i_x_frac,
													0);
		x265_interpolation_filter_filter_ver_chroma_s_p(h,
													&prediction->interpolation_filter,
													ext_y + (i_half_filter_size - 1) * i_ext_stride,
													i_ext_stride,
													dst_cr,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_y_frac,
													0,
													!b_bi);
	}
}

/**
 * \brief generate motion-compensated chroma block
 *
 * \param cu       pointer to current cu
 * \param ref_pic   pointer to reference picture
 * \param part_addr address of block within cu
 * \param mv       motion vector
 * \param i_width    i_width of block
 * \param i_height   i_height of block
 * \param dst_pic   pointer to destination picture
 * \param bi       flag indicating whether bipred is used
 */
void x265_prediction_x_pred_inter_chroma_blk_s(x265_t *h,
											x265_prediction_t* prediction,
											x265_data_cu_t *cu,
											x265_frame_t *ref_pic,
											uint32_t i_part_addr,
											x265_mv_t *mv,
											int32_t i_width,
											int32_t i_height,
											x265_simage_t **pp_dst_simage,
											int32_t b_bi )
{
	int32_t i_ref_stride = 0;
	int32_t i_dst_stride = 0;
	int32_t i_ref_offset = 0;
	pixel* ref_cb = 0;
	pixel* ref_cr = 0;
	spixel* dst_cb = 0;
	spixel* dst_cr = 0;
	int32_t i_x_frac  = 0;
	int32_t i_y_frac  = 0;
	uint32_t i_cx_width = 0;
	uint32_t i_cx_height = 0;
	int32_t i_ext_stride = 0;
	int16_t *ext_y = NULL;
	int32_t i_filter_size = 0;
	int32_t i_half_filter_size = 0;

	i_ref_stride = ref_pic->i_stride[1];
	i_dst_stride = x265_simage_get_c_stride(*pp_dst_simage);
	i_ref_offset  = (mv->i_hor >> 3) + (mv->i_ver >> 3) * i_ref_stride;

	ref_cb = x265_frame_get_cb_addr_p4(h,
										ref_pic,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_addr )
				+ i_ref_offset;
	ref_cr = x265_frame_get_cr_addr_p4(h,
										ref_pic,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_addr )
				+ i_ref_offset;

	dst_cb = x265_simage_get_cb_addr_p3(h,
										*pp_dst_simage,
										i_part_addr );
	dst_cr = x265_simage_get_cr_addr_p3(h,
										*pp_dst_simage,
										i_part_addr );

	i_x_frac = mv->i_hor & 0x7;
	i_y_frac = mv->i_ver & 0x7;
	i_cx_width = i_width  >> 1;
	i_cx_height = i_height >> 1;

	i_ext_stride = x265_simage_get_stride(&prediction->filtered_block_tmp[0]);
	ext_y = x265_simage_get_luma_addr_p2(h, &prediction->filtered_block_tmp[0]);
	i_filter_size = X265_NTAPS_CHROMA;
	i_half_filter_size = (i_filter_size>>1);

	if ( i_y_frac == 0 )
	{
		x265_interpolation_filter_filter_hor_chroma_p_s(h,
													&prediction->interpolation_filter,
													ref_cb,
													i_ref_stride,
													dst_cb,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_x_frac,
													!b_bi);
		x265_interpolation_filter_filter_hor_chroma_p_s(h,
													&prediction->interpolation_filter,
													ref_cr,
													i_ref_stride,
													dst_cr,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_x_frac,
													!b_bi);
	}
	else if ( i_x_frac == 0 )
	{
		x265_interpolation_filter_filter_ver_chroma_p_s(h,
													&prediction->interpolation_filter,
													ref_cb,
													i_ref_stride,
													dst_cb,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_y_frac,
													1,
													!b_bi);
		x265_interpolation_filter_filter_ver_chroma_p_s(h,
													&prediction->interpolation_filter,
													ref_cr,
													i_ref_stride,
													dst_cr,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_y_frac,
													1,
													!b_bi);
	}
	else
	{
		x265_interpolation_filter_filter_hor_chroma_p_s(h,
													&prediction->interpolation_filter,
													ref_cb - (i_half_filter_size - 1) * i_ref_stride,
													i_ref_stride,
													ext_y,
													i_ext_stride,
													i_cx_width,
													i_cx_height + i_filter_size - 1,
													i_x_frac,
													0 ) ;
		x265_interpolation_filter_filter_ver_chroma_s_s(h,
													&prediction->interpolation_filter,
													ext_y  + (i_half_filter_size - 1) * i_ext_stride,
													i_ext_stride,
													dst_cb,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_y_frac,
													0,
													!b_bi);

		x265_interpolation_filter_filter_hor_chroma_p_s(h,
													&prediction->interpolation_filter,
													ref_cr - (i_half_filter_size - 1) * i_ref_stride,
													i_ref_stride,
													ext_y,
													i_ext_stride,
													i_cx_width,
													i_cx_height + i_filter_size - 1,
													i_x_frac,
													0);
		x265_interpolation_filter_filter_ver_chroma_s_s(h,
													&prediction->interpolation_filter,
													ext_y + (i_half_filter_size - 1) * i_ext_stride,
													i_ext_stride,
													dst_cr,
													i_dst_stride,
													i_cx_width,
													i_cx_height,
													i_y_frac,
													0,
													!b_bi);
	}
}

void x265_prediction_x_weighted_average( x265_t *h,
										x265_simage_t* p_simage_src0,
										x265_simage_t* p_simage_src1,
										int32_t i_ref_idx0,
										int32_t i_ref_idx1,
										uint32_t i_part_idx,
										int32_t i_width,
										int32_t i_height,
										x265_image_t** pp_image_dst )
{
	if( i_ref_idx0 >= 0 && i_ref_idx1 >= 0 )
	{
		x265_image_add_avg(h,
							*pp_image_dst,
							p_simage_src0,
							p_simage_src1,
							i_part_idx,
							i_width,
							i_height );
	}
	else if ( i_ref_idx0 >= 0 && i_ref_idx1 <  0 )
	{
		x265_simage_copy_part_to_part_simage_p(h,
											p_simage_src0,
											*pp_image_dst,
											i_part_idx,
											i_width,
											i_height );
	}
	else if ( i_ref_idx0 <  0 && i_ref_idx1 >= 0 )
	{
		x265_simage_copy_part_to_part_simage_p(h,
											p_simage_src1,
											*pp_image_dst,
											i_part_idx,
											i_width,
											i_height );
	}
}

// amvp
/*
void x265_prediction_get_mv_pred_amvp( x265_data_cu_t* cu, uint32_t i_part_idx, uint32_t i_part_addr, enum ref_pic_list_e i_ref_pic_list, x265_mv_t& rc_mv_pred )
{
  amvpinfo* pc_a_mvpinfo = cu->get_cu_mv_field(i_ref_pic_list)->get_a_mvpinfo();
  if( pc_a_mvpinfo->i_n <= 1 )
  {
    rc_mv_pred = pc_a_mvpinfo->ac_mv_cand[0];

    cu->set_m_vpidx_sub_parts( 0, i_ref_pic_list, i_part_addr, i_part_idx, cu->get_depth(i_part_addr));
    cu->set_m_vpnum_sub_parts( pc_a_mvpinfo->i_n, i_ref_pic_list, i_part_addr, i_part_idx, cu->get_depth(i_part_addr));
    return;
  }

  assert(cu->get_m_vpidx(i_ref_pic_list,i_part_addr) >= 0);
  rc_mv_pred = pc_a_mvpinfo->ac_mv_cand[cu->get_m_vpidx(i_ref_pic_list,i_part_addr)];
  return;
}
*/
