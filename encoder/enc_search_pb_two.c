
#include "common/common.h"



void x265_enc_search_x_motion_estimation(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *image_org,
										int32_t i_part_idx,
										enum ref_pic_list_e e_ref_pic_list,
										x265_mv_t *p_mv_pred,
										int32_t i_ref_idx_pred,
										x265_mv_t *p_mv,
										uint32_t *p_bits,
										uint32_t *p_cost,
										int32_t b_bi)
{
	uint32_t i_part_addr;
	int32_t i_roi_width;
	int32_t i_roi_height;
	x265_mv_t mv_half, mv_qter;
	x265_mv_t mv_srch_rng_lt;
	x265_mv_t mv_srch_rng_rb;
	x265_image_t *image = NULL;
	int32_t i_srch_rng = 0;
	x265_pattern_t *pattern_key = NULL;
	double f_weight = 0.0;
	x265_image_t * image_other = NULL;
	pixel *p_ref_y = NULL;
	int32_t i_ref_stride = 0;
	x265_mv_t mv_pred ;
	uint32_t i_mv_bits = 0;

	image = image_org;
	enc_search->i_search_range = enc_search->adapt_sr[e_ref_pic_list][i_ref_idx_pred];
	i_srch_rng = ( b_bi ? enc_search->i_bipred_search_range : enc_search->i_search_range );
	pattern_key = x265_data_cu_get_pattern(cu);
	f_weight  = 1.0;
	x265_data_cu_get_part_index_and_size(cu, i_part_idx, &i_part_addr, &i_roi_width, &i_roi_height);

	if ( b_bi )
	{
		image_other = &enc_search->prediction.image_pred[1-(int32_t)e_ref_pic_list];
		image = &enc_search->prediction.image_pred_temp;
		x265_image_copy_part_to_part_image(h,
											image_org,
											image,
											i_part_addr,
											i_roi_width,
											i_roi_height );

		x265_image_remove_high_freq(h,
									image,
									image_other,
									i_part_addr,
									i_roi_width,
									i_roi_height );
		f_weight = 0.5;
	}

	//  search key pattern initialization
	x265_pattern_init_pattern_p9(pattern_key,
								x265_image_get_luma_addr_p3(h, image, i_part_addr),
								x265_image_get_cb_addr_p3(h, image, i_part_addr),
								x265_image_get_cr_addr_p3(h, image, i_part_addr),
								i_roi_width,
								i_roi_height,
								x265_image_get_stride(image),
								0,
								0 );

	p_ref_y = x265_frame_get_luma_addr_p4(h,
										h->fref[e_ref_pic_list][i_ref_idx_pred],
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_addr );
	i_ref_stride = x265_frame_get_stride(h->fref[e_ref_pic_list][i_ref_idx_pred]);

	memcpy(&mv_pred, p_mv_pred, sizeof(x265_mv_t));

	if ( b_bi )
	{
		x265_enc_search_x_set_search_range(h,
											enc_search,
											cu,
											p_mv,
											i_srch_rng,
											&mv_srch_rng_lt,
											&mv_srch_rng_rb );
	}
	else
	{
		x265_enc_search_x_set_search_range(h,
											enc_search,
											cu,
											&mv_pred,
											i_srch_rng,
											&mv_srch_rng_lt,
											&mv_srch_rng_rb );
	}

	x265_rd_cost_get_motion_cost(&h->rd_cost, 1, 0 );

	x265_rd_cost_set_predictor(&h->rd_cost, p_mv_pred);
	h->rd_cost.i_cost_scale = 2;

	x265_enc_search_set_wp_scaling_dist_param(h,
											enc_search,
											cu,
											i_ref_idx_pred,
											e_ref_pic_list );
	//  do integer search
	if ( !enc_search->i_fast_search || b_bi )
	{
		x265_enc_search_x_pattern_search(h,
											enc_search,
											pattern_key,
											p_ref_y,
											i_ref_stride,
											&mv_srch_rng_lt,
											&mv_srch_rng_rb,
											p_mv,
											p_cost );
	}
	else
	{
		memcpy(p_mv, &mv_pred, sizeof(x265_mv_t));
		x265_enc_search_x_pattern_search_fast(h,
												enc_search,
												cu,
												pattern_key,
												p_ref_y,
												i_ref_stride,
												&mv_srch_rng_lt,
												&mv_srch_rng_rb,
												p_mv,
												p_cost );
	}

	x265_rd_cost_get_motion_cost(&h->rd_cost, 1, 0 );
	h->rd_cost.i_cost_scale = 1 ;

	{
		x265_enc_search_x_pattern_search_frac_dif(h,
													enc_search,
													cu,
													pattern_key,
													p_ref_y,
													i_ref_stride,
													p_mv,
													&mv_half,
													&mv_qter,
													p_cost,
													b_bi);
	}

	h->rd_cost.i_cost_scale = 0;
	p_mv->i_hor = (p_mv->i_hor << 2) ;
	p_mv->i_ver = (p_mv->i_ver << 2) ;

	mv_half.i_hor = (mv_half.i_hor << 1) ;
	mv_half.i_ver = (mv_half.i_ver << 1) ;
	x265_mv_add_to(p_mv, &mv_half);

	x265_mv_add_to(p_mv, &mv_qter);

	i_mv_bits = x265_rd_cost_get_bits(&h->rd_cost, p_mv->i_hor, p_mv->i_ver);

	*p_bits += i_mv_bits;
	*p_cost  = (uint32_t)( floor( f_weight * ((double)*p_cost
						- (double)x265_rd_cost_get_cost_p2(&h->rd_cost, i_mv_bits )))
						+ (double)x265_rd_cost_get_cost_p2(&h->rd_cost, *p_bits ) );
}

void x265_enc_search_x_set_search_range(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_mv_t *p_mv_pred,
										int32_t i_srch_rng,
										x265_mv_t *p_mv_srch_rng_lt,
										x265_mv_t* p_mv_srch_rng_rb )
{
	int32_t  i_mv_shift = 2;
	x265_mv_t tmp_mv_pred ;

	memcpy(&tmp_mv_pred, p_mv_pred, sizeof(x265_mv_t)) ;
	x265_data_cu_clip_mv(h, cu, &tmp_mv_pred );

	p_mv_srch_rng_lt->i_hor = tmp_mv_pred.i_hor - (i_srch_rng << i_mv_shift);
	p_mv_srch_rng_lt->i_ver = tmp_mv_pred.i_ver - (i_srch_rng << i_mv_shift);

	p_mv_srch_rng_rb->i_hor = tmp_mv_pred.i_hor + (i_srch_rng << i_mv_shift);
	p_mv_srch_rng_rb->i_ver = tmp_mv_pred.i_ver + (i_srch_rng << i_mv_shift);
	x265_data_cu_clip_mv(h, cu, p_mv_srch_rng_lt );
	x265_data_cu_clip_mv(h, cu, p_mv_srch_rng_rb );

	p_mv_srch_rng_lt->i_hor = (p_mv_srch_rng_lt->i_hor >> i_mv_shift);
	p_mv_srch_rng_lt->i_ver = (p_mv_srch_rng_lt->i_ver >> i_mv_shift);
	p_mv_srch_rng_rb->i_hor = (p_mv_srch_rng_rb->i_hor >> i_mv_shift);
	p_mv_srch_rng_rb->i_ver = (p_mv_srch_rng_rb->i_ver >> i_mv_shift);
}


void x265_enc_search_x_pattern_search(x265_t* h,
									x265_enc_search_t *enc_search,
									x265_pattern_t *pattern_key,
									pixel *p_ref_y,
									int32_t i_ref_stride,
									x265_mv_t *p_mv_srch_rng_lt,
									x265_mv_t *p_mv_srch_rng_rb,
									x265_mv_t *p_mv,
									uint32_t *p_sad )
{
	int32_t x = 0, y = 0;
	int32_t i_srch_rng_hor_left = 0;
	int32_t i_srch_rng_hor_right = 0;
	int32_t i_srch_rng_ver_top = 0;
	int32_t i_srch_rng_ver_bottom = 0;

	uint32_t i_sad;
	uint32_t i_sad_best = 0;
	int32_t i_best_x = 0;
	int32_t i_best_y = 0;

	i_srch_rng_hor_left = p_mv_srch_rng_lt->i_hor;
	i_srch_rng_hor_right = p_mv_srch_rng_rb->i_hor;
	i_srch_rng_ver_top = p_mv_srch_rng_lt->i_ver;
	i_srch_rng_ver_bottom = p_mv_srch_rng_rb->i_ver;

	i_sad_best = X265_MAX_UINT;


	pixel *p_ref_srch;

	//-- jclee for using the sad function pointer
	x265_rd_cost_set_dist_param_p6_2(h,
									&h->rd_cost,
									pattern_key,
									p_ref_y,
									i_ref_stride,
									&enc_search->dist_param );

	// fast encoder decision: use subsampled sad for integer me
	if ( h->param.b_use_fast_enc )
	{
		if ( enc_search->dist_param.i_rows > 8 )
		{
			enc_search->dist_param.i_sub_shift = 1;
		}
	}

	p_ref_y += (i_srch_rng_ver_top * i_ref_stride);
	for ( y = i_srch_rng_ver_top; y <= i_srch_rng_ver_bottom; y++ )
	{
		for ( x = i_srch_rng_hor_left; x <= i_srch_rng_hor_right; x++ )
		{
			//  find min. distortion position
			p_ref_srch = p_ref_y + x;
			enc_search->dist_param.cur = p_ref_srch;

			enc_search->dist_param.i_comp = 0;

			enc_search->dist_param.i_bit_depth = h->cu.pic.i_bit_depth_y;
			i_sad = enc_search->dist_param.dist_func(&h->rd_cost, &enc_search->dist_param );

			// motion cost
			i_sad += x265_rd_cost_get_cost_p3(&h->rd_cost, x, y);

			if ( i_sad < i_sad_best )
			{
				i_sad_best = i_sad;
				i_best_x    = x;
				i_best_y    = y;
			}
		}
		p_ref_y += i_ref_stride;
	}

	p_mv->i_hor = i_best_x ;
	p_mv->i_ver = i_best_y ;
	*p_sad = i_sad_best - x265_rd_cost_get_cost_p3(&h->rd_cost, i_best_x, i_best_y);

	return;
}



void x265_enc_search_x_pattern_search_fast(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											x265_pattern_t *pattern_key,
											pixel *p_ref_y,
											int32_t i_ref_stride,
											x265_mv_t *p_mv_srch_rng_lt,
											x265_mv_t *p_mv_srch_rng_rb,
											x265_mv_t *p_mv,
											uint32_t *p_sad )
{
	x265_data_cu_get_mv_pred_left (cu, &enc_search->mv_predictors[0] );
	x265_data_cu_get_mv_pred_above (cu, &enc_search->mv_predictors[1] );
	x265_data_cu_get_mv_pred_above_right (cu, &enc_search->mv_predictors[2] );

	switch ( enc_search->i_fast_search )
	{
	case 1:
		x265_enc_search_x_tz_search(h,
									enc_search,
									cu,
									pattern_key,
									p_ref_y,
									i_ref_stride,
									p_mv_srch_rng_lt,
									p_mv_srch_rng_rb,
									p_mv,
									p_sad );
		break;
	default:
		break;
	}
}

void x265_enc_search_x_tz_search(x265_t *h,
								x265_enc_search_t *enc_search,
								x265_data_cu_t *cu,
								x265_pattern_t *pattern_key,
								pixel *p_ref_y,
								int32_t i_ref_stride,
								x265_mv_t *p_mv_srch_rng_lt,
								x265_mv_t *p_mv_srch_rng_rb,
								x265_mv_t *p_mv,
								uint32_t *p_sad )
{
	uint32_t i_index = 0 ;
	int32_t i_srch_rng_hor_left = 0;
	int32_t i_srch_rng_hor_right = 0;
	int32_t i_srch_rng_ver_top = 0;
	int32_t i_srch_rng_ver_bottom = 0;
	X265_TZ_SEARCH_CONFIGURATION
	uint32_t i_search_range = 0;
	// start search
	int32_t  i_dist = 0;
	int32_t  i_start_x = 0;
	int32_t  i_start_y = 0;
	x265_mv_t mv ;
	x265_int_tz_search_struct_t int_tz_search_struct;

	i_srch_rng_hor_left = p_mv_srch_rng_lt->i_hor;
	i_srch_rng_hor_right = p_mv_srch_rng_rb->i_hor;
	i_srch_rng_ver_top = p_mv_srch_rng_lt->i_ver;
	i_srch_rng_ver_bottom = p_mv_srch_rng_rb->i_ver;
	i_search_range = enc_search->i_search_range;

	x265_data_cu_clip_mv(h, cu, p_mv);
	p_mv->i_hor = (p_mv->i_hor >> 2) ;
	p_mv->i_ver = (p_mv->i_ver >> 2) ;
	// init tzsearch_struct
	int_tz_search_struct.i_y_stride    = i_ref_stride;
	int_tz_search_struct.ref_y = p_ref_y;
	int_tz_search_struct.i_best_sad   = X265_MAX_UINT;

	// set rmv (median predictor) as start point and as best point
	x265_enc_search_x_tz_search_help(h,
									enc_search,
									pattern_key,
									&int_tz_search_struct,
									p_mv->i_hor,
									p_mv->i_ver,
									0,
									0 );

	// test whether one of pred_a, pred_b, pred_c mv is better start point than median predictor
	if ( b_test_other_predicted_mv )
	{
		for ( i_index = 0; i_index < 3; i_index++ )
		{
			memcpy(&mv, &enc_search->mv_predictors[i_index], sizeof(x265_mv_t));
			x265_data_cu_clip_mv(h, cu, &mv);
			mv.i_hor = (mv.i_hor >> 2);
			mv.i_ver = (mv.i_ver >> 2);
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											&int_tz_search_struct,
											mv.i_hor,
											mv.i_ver,
											0,
											0 );
		}
	}

	// test whether zero mv is better start point than median predictor
	if ( b_test_zero_vector )
	{
		x265_enc_search_x_tz_search_help(h,
										enc_search,
										pattern_key,
										&int_tz_search_struct,
										0,
										0,
										0,
										0 );
	}

	// start search
	i_dist = 0;
	i_start_x = int_tz_search_struct.i_best_x;
	i_start_y = int_tz_search_struct.i_best_y;

	// first search
	for ( i_dist = 1; i_dist <= (int32_t)i_search_range; i_dist*=2 )
	{
		if ( b_first_search_diamond == 1 )
		{
			x265_enc_search_x_tz8_point_diamond_search(h,
														enc_search,
														pattern_key,
														&int_tz_search_struct,
														p_mv_srch_rng_lt,
														p_mv_srch_rng_rb,
														i_start_x,
														i_start_y,
														i_dist );
		}
		else
		{
			x265_enc_search_x_tz8_point_square_search(h,
													enc_search,
													pattern_key,
													&int_tz_search_struct,
													p_mv_srch_rng_lt,
													p_mv_srch_rng_rb,
													i_start_x,
													i_start_y,
													i_dist );
		}

		if ( b_first_search_stop && ( int_tz_search_struct.i_best_round >= i_first_search_rounds ) ) // stop criterion
		{
			break;
		}
	}

	// test whether zero mv is a better start point than median predictor
	if ( b_test_zero_vector_start && ((int_tz_search_struct.i_best_x != 0) || (int_tz_search_struct.i_best_y != 0)) )
	{
		x265_enc_search_x_tz_search_help(h,
										enc_search,
										pattern_key,
										&int_tz_search_struct,
										0,
										0,
										0,
										0 );
		if ( (int_tz_search_struct.i_best_x == 0) && (int_tz_search_struct.i_best_y == 0) )
		{
			// test its neighborhood
			for ( i_dist = 1; i_dist <= (int32_t)i_search_range; i_dist*=2 )
			{
				x265_enc_search_x_tz8_point_diamond_search(h,
															enc_search,
															pattern_key,
															&int_tz_search_struct,
															p_mv_srch_rng_lt,
															p_mv_srch_rng_rb,
															0,
															0,
															i_dist );
				if ( b_test_zero_vector_stop && (int_tz_search_struct.i_best_round > 0) ) // stop criterion
				{
					break;
				}
			}
		}
	}

	// calculate only 2 missing points instead 8 points if int_tz_search_struct.i_best_distance == 1
	if ( int_tz_search_struct.i_best_distance == 1 )
	{
		int_tz_search_struct.i_best_distance = 0;
		x265_enc_search_x_tz2_point_search(h,
											enc_search,
											pattern_key,
											&int_tz_search_struct,
											p_mv_srch_rng_lt,
											p_mv_srch_rng_rb );
	}

	// raster search if distance is too big
	if ( b_enable_raster_search && ( ((int32_t)(int_tz_search_struct.i_best_distance) > i_raster) || b_always_raster_search ) )
	{
		int_tz_search_struct.i_best_distance = i_raster;
		for ( i_start_y = i_srch_rng_ver_top; i_start_y <= i_srch_rng_ver_bottom; i_start_y += i_raster )
		{
			for ( i_start_x = i_srch_rng_hor_left; i_start_x <= i_srch_rng_hor_right; i_start_x += i_raster )
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												&int_tz_search_struct,
												i_start_x,
												i_start_y,
												0,
												i_raster );
			}
		}
	}

	// raster refinement
	if ( b_raster_refinement_enable && int_tz_search_struct.i_best_distance > 0 )
	{
		while ( int_tz_search_struct.i_best_distance > 0 )
		{
			i_start_x = int_tz_search_struct.i_best_x;
			i_start_y = int_tz_search_struct.i_best_y;
			if ( int_tz_search_struct.i_best_distance > 1 )
			{
				i_dist = int_tz_search_struct.i_best_distance >>= 1;
				if ( b_raster_refinement_diamond == 1 )
				{
					x265_enc_search_x_tz8_point_diamond_search(h,
																enc_search,
																pattern_key,
																&int_tz_search_struct,
																p_mv_srch_rng_lt,
																p_mv_srch_rng_rb,
																i_start_x,
																i_start_y,
																i_dist );
				}
				else
				{
					x265_enc_search_x_tz8_point_square_search(h,
															enc_search,
															pattern_key,
															&int_tz_search_struct,
															p_mv_srch_rng_lt,
															p_mv_srch_rng_rb,
															i_start_x,
															i_start_y,
															i_dist );
				}
			}

			// calculate only 2 missing points instead 8 points if int_tz_search_struct.i_best_distance == 1
			if ( int_tz_search_struct.i_best_distance == 1 )
			{
				int_tz_search_struct.i_best_distance = 0;
				if ( int_tz_search_struct.i_point_nr != 0 )
				{
					x265_enc_search_x_tz2_point_search(h,
														enc_search,
														pattern_key,
														&int_tz_search_struct,
														p_mv_srch_rng_lt,
														p_mv_srch_rng_rb );
				}
			}
		}
	}

	// start refinement
	if ( b_star_refinement_enable && int_tz_search_struct.i_best_distance > 0 )
	{
		while ( int_tz_search_struct.i_best_distance > 0 )
		{
			i_start_x = int_tz_search_struct.i_best_x;
			i_start_y = int_tz_search_struct.i_best_y;
			int_tz_search_struct.i_best_distance = 0;
			int_tz_search_struct.i_point_nr = 0;
			for ( i_dist = 1; i_dist < (int32_t)i_search_range + 1; i_dist*=2 )
			{
				if ( b_star_refinement_diamond == 1 )
				{
					x265_enc_search_x_tz8_point_diamond_search(h,
																enc_search,
																pattern_key,
																&int_tz_search_struct,
																p_mv_srch_rng_lt,
																p_mv_srch_rng_rb,
																i_start_x,
																i_start_y,
																i_dist );
				}
				else
				{
					x265_enc_search_x_tz8_point_square_search(h,
															enc_search,
															pattern_key,
															&int_tz_search_struct,
															p_mv_srch_rng_lt,
															p_mv_srch_rng_rb,
															i_start_x,
															i_start_y,
															i_dist );
				}
				if ( b_star_refinement_stop && (int_tz_search_struct.i_best_round >= i_star_refinement_rounds) ) // stop criterion
				{
					break;
				}
			}

			// calculate only 2 missing points instead 8 points if c_strukt.i_best_distance == 1
			if ( int_tz_search_struct.i_best_distance == 1 )
			{
				int_tz_search_struct.i_best_distance = 0;
				if ( int_tz_search_struct.i_point_nr != 0 )
				{
					x265_enc_search_x_tz2_point_search(h,
														enc_search,
														pattern_key,
														&int_tz_search_struct,
														p_mv_srch_rng_lt,
														p_mv_srch_rng_rb );
				}
			}
		}
	}

	// write out best match
	p_mv->i_hor = int_tz_search_struct.i_best_x ;
	p_mv->i_ver = int_tz_search_struct.i_best_y ;
	*p_sad = int_tz_search_struct.i_best_sad
			- x265_rd_cost_get_cost_p3(&h->rd_cost, int_tz_search_struct.i_best_x, int_tz_search_struct.i_best_y );
}


void x265_enc_search_x_pattern_search_frac_dif(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												x265_pattern_t *pattern_key,
												pixel *p_ref_y,
												int32_t i_ref_stride,
												x265_mv_t *p_mv_int,
												x265_mv_t *p_mv_half,
												x265_mv_t *p_mv_qter,
												uint32_t *p_cost,
												int32_t b_bi_pred )
{
	//  reference pattern initialization (integer scale)
	x265_pattern_t pattern_roi;
	int32_t i_offset = 0;
	x265_mv_t base_ref_mv;

	i_offset = p_mv_int->i_hor + p_mv_int->i_ver * i_ref_stride;
	x265_mv_set_zero(&base_ref_mv) ;
	x265_pattern_init_pattern_p9(&pattern_roi,
								p_ref_y +  i_offset,
								NULL,
								NULL,
								x265_pattern_get_roi_width(pattern_key),
								x265_pattern_get_roi_height(pattern_key),
								i_ref_stride,
								0,
								0 );

	//  half-pel refinement
	x265_enc_search_x_ext_dif_up_sampling_h(h, enc_search, &pattern_roi, b_bi_pred );

	memcpy(p_mv_half, p_mv_int, sizeof(x265_mv_t));
	p_mv_half->i_hor = (p_mv_half->i_hor << 1);    // for mv-cost
	p_mv_half->i_ver = (p_mv_half->i_ver << 1);    // for mv-cost
	*p_cost = x265_enc_search_x_pattern_refinement(h,
													enc_search,
													pattern_key,
													&base_ref_mv,
													2,
													p_mv_half   );

	h->rd_cost.i_cost_scale = 0 ;

	x265_enc_search_x_ext_dif_up_sampling_q(h,
											enc_search,
											&pattern_roi,
											p_mv_half,
											b_bi_pred );
	memcpy(&base_ref_mv, p_mv_half, sizeof(x265_mv_t));
	base_ref_mv.i_hor = (base_ref_mv.i_hor << 1);
	base_ref_mv.i_ver = (base_ref_mv.i_ver << 1);

	memcpy(p_mv_qter, p_mv_int, sizeof(x265_mv_t));
	p_mv_qter->i_hor = (p_mv_qter->i_hor << 1);
	p_mv_qter->i_ver = (p_mv_qter->i_ver << 1);

	x265_mv_add_to(p_mv_qter, p_mv_half);
	p_mv_qter->i_hor = (p_mv_qter->i_hor << 1);
	p_mv_qter->i_ver = (p_mv_qter->i_ver << 1);
	*p_cost = x265_enc_search_x_pattern_refinement(h,
													enc_search,
													pattern_key,
													&base_ref_mv,
													1,
													p_mv_qter );
}


/** encode residual and calculate rate-distortion for a cu block
 * \param cu
 * \param image_org
 * \param image_pred
 * \param rimage_resi
 * \param rimage_resi_best
 * \param rimage_rec
 * \param b_skip_res
 * \returns void
 */

void x265_enc_search_encode_res_and_calc_rd_inter_cu(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													x265_image_t *p_image_org,
													x265_image_t *p_image_pred,
													x265_simage_t **pp_image_resi,
													x265_simage_t **pp_image_resi_best,
													x265_image_t **pp_image_reco,
													int32_t b_skip_res )
{
	int32_t b_high_pass = 0;
	uint32_t i_bits = 0 ;
	uint32_t i_bits_best = 0;
	uint32_t i_distortion = 0 ;
	uint32_t i_distortion_best = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	int32_t qp = 0, qp_best = 0, qp_min = 0, qp_max = 0;
	double  d_cost = 0.0, d_cost_best = 0.0;
	uint32_t i_tr_level = 0;
	uint32_t i_max_tr_mode = 0;
	uint32_t i_zero_distortion = 0;
	uint32_t zero_resi_bits = 0;
	double d_zero_cost = 0.0;
	uint32_t i_qpart_num = 0;
#if 0 // check
	uint32_t i_bits_for_coeff = 0;
#endif
	double d_exact_cost = 0.0;
	x265_image_t *p_dummy = NULL;


	if ( x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, 0) )
	{
		return;
	}

	b_high_pass = h->slice->i_depth ? 1 : 0;
	i_width = x265_data_cu_get_width_p2(cu, 0);
	i_height = x265_data_cu_get_height_p2(cu, 0);

	//  no residual coding : skip mode
	if ( b_skip_res )
	{
		x265_base_data_cu_set_skip_flag_sub_parts(h,
												(x265_base_data_cu_t*)cu,
												1,
												0,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));
		x265_simage_clear (*pp_image_resi);
		x265_image_copy_to_part_image(h, p_image_pred, *pp_image_reco, 0 );

#if X265_WEIGHTED_CHROMA_DISTORTION
		i_distortion = x265_rd_cost_get_dist_part(h,
												&h->rd_cost,
												h->cu.pic.i_bit_depth_y,
												x265_image_get_luma_addr_p2(h, *pp_image_reco),
												x265_image_get_stride(*pp_image_reco),
												x265_image_get_luma_addr_p2(h, p_image_org),
												x265_image_get_stride(p_image_org),
												i_width,
												i_height,
												TEXT_LUMA,
												DF_SSE)
							+ x265_rd_cost_get_dist_part(h,
														&h->rd_cost,
														h->cu.pic.i_bit_depth_c,
														x265_image_get_cb_addr_p2(h, *pp_image_reco),
														x265_image_get_c_stride(*pp_image_reco),
														x265_image_get_cb_addr_p2(h, p_image_org),
														x265_image_get_c_stride(p_image_org),
														i_width >> 1,
														i_height >> 1,
														TEXT_CHROMA_U,
														DF_SSE)
							+ x265_rd_cost_get_dist_part(h,
														&h->rd_cost,
														h->cu.pic.i_bit_depth_c,
														x265_image_get_cr_addr_p2(h, *pp_image_reco),
														x265_image_get_c_stride(*pp_image_reco),
														x265_image_get_cr_addr_p2(h, p_image_org),
														x265_image_get_c_stride(p_image_org),
														i_width >> 1,
														i_height >> 1,
														TEXT_CHROMA_V,
														DF_SSE);
#else
		i_distortion = x265_rd_cost_get_dist_part(h,
													&h->rd_cost,
													h->cu.pic.i_bit_depth_y,
													x265_image_get_luma_addr_p2(h, *pp_image_reco),
													x265_image_get_stride(*pp_image_reco),
													x265_image_get_luma_addr_p2(h, p_image_org),
													x265_image_get_stride(p_image_org),
													i_width,
													i_height,
													DF_SSE)
						+ x265_rd_cost_get_dist_part(h,
													&h->rd_cost,
													h->cu.pic.i_bit_depth_c,
													x265_image_get_cb_addr_p2(h, *pp_image_reco),
													x265_image_get_c_stride(*pp_image_reco),
													x265_image_get_cb_addr_p2(h, p_image_org),
													x265_image_get_c_stride(p_image_org),
													i_width >> 1,
													i_height >> 1,
													DF_SSE)
						+ x265_rd_cost_get_dist_part(h,
													&h->rd_cost,
													h->cu.pic.i_bit_depth_c,
													x265_image_get_cr_addr_p2(h, *pp_image_reco),
													x265_image_get_c_stride(*pp_image_reco),
													x265_image_get_cr_addr_p2(h, p_image_org),
													x265_image_get_c_stride(p_image_org),
													i_width >> 1,
													i_height >> 1,
													DF_SSE);
#endif

		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)][CI_CURR_BEST]);
		}

		x265_enc_entropy_reset_bits(&h->enc_entropy);
		if (h->pps[0].b_transquant_bypass_enable_flag)
		{
			x265_enc_entropy_encode_cu_transquant_bypass_flag(&h->enc_entropy, cu, 0, 1);
		}
		x265_enc_entropy_encode_skip_flag(h, &h->enc_entropy, cu, 0, 1);
		x265_enc_entropy_encode_merge_index(h, &h->enc_entropy, cu, 0, 1 );

		i_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
		cu->i_total_bits = i_bits;
		cu->i_total_distortion = i_distortion;
		cu->f_total_cost = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_bits, i_distortion, 0, DF_DEFAULT );

		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)][CI_TEMP_BEST]);
		}

		x265_data_cu_set_cbf_sub_parts_p7_1(h,
											cu,
											0,
											0,
											0,
											0,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));
		x265_base_data_cu_set_tr_idx_sub_parts(h,
												(x265_base_data_cu_t*)cu,
												0,
												0,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) );

		return;
	}

	//  residual coding.
	d_cost_best = X265_MAX_DOUBLE;
	i_tr_level = 0;
	if( x265_data_cu_get_width_p2(cu, 0) > h->sps[0].i_max_tr_size )
	{
		while( x265_data_cu_get_width_p2(cu, 0) > (h->sps[0].i_max_tr_size << i_tr_level) )
		{
			i_tr_level++;
		}
	}
	i_max_tr_mode = h->sps[0].i_max_tr_depth + i_tr_level;

	while((i_width>>i_max_tr_mode) < (h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth))
	{
		i_max_tr_mode--;
	}

	qp_min =  b_high_pass ? x265_clip3_int32(x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0) - enc_search->i_max_delta_qp, -h->sps[0].i_qp_bd_offset_y, X265_MAX_QP)
							: x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0);
	qp_max =  b_high_pass ? x265_clip3_int32(x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0) + enc_search->i_max_delta_qp, -h->sps[0].i_qp_bd_offset_y, X265_MAX_QP)
							: x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0);

	x265_simage_subtract(h, *pp_image_resi, p_image_org, p_image_pred, 0, i_width );

	for ( qp = qp_min; qp <= qp_max; qp++ )
	{
		d_cost = 0.;
		i_bits = 0;
		i_distortion = 0;
		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)][ CI_CURR_BEST ] );
		}

		i_zero_distortion = 0;
		x265_enc_search_x_estimate_residual_qt(h,
												enc_search,
												cu,
												0,
												0,
												0,
												*pp_image_resi,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0),
												&d_cost,
												&i_bits,
												&i_distortion,
												&i_zero_distortion );

		x265_enc_entropy_reset_bits(&h->enc_entropy);
		x265_enc_entropy_encode_qt_root_cbf_zero(&h->enc_entropy, cu );
		zero_resi_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
		d_zero_cost = x265_rd_cost_calc_rd_cost(&h->rd_cost, zero_resi_bits, i_zero_distortion, 0, DF_DEFAULT);
		if(x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, 0))
		{
			d_zero_cost = d_cost + 1;
		}
		if ( d_zero_cost < d_cost )
		{
			d_cost   = d_zero_cost;
			i_bits  = 0;
			i_distortion = i_zero_distortion;

			i_qpart_num = h->cu.pic.i_num_partitions >> (x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) << 1);
			memset( x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu),
					0,
					i_qpart_num * sizeof(uint8_t));
			memset( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_LUMA ),
					0,
					i_qpart_num * sizeof(uint8_t));
			memset( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U ),
					0,
					i_qpart_num * sizeof(uint8_t));
			memset( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V ),
					0,
					i_qpart_num * sizeof(uint8_t));
			memset( x265_data_cu_get_coeff_y(cu),
					0,
					i_width * i_height * sizeof( x265_coeff_t ) );
			memset( x265_data_cu_get_coeff_cb(cu),
					0,
					i_width * i_height * sizeof( x265_coeff_t ) >> 2 );
			memset( x265_data_cu_get_coeff_cr(cu),
					0,
					i_width * i_height * sizeof( x265_coeff_t ) >> 2 );
			x265_data_cu_set_transform_skip_sub_parts_p7(h,
														cu,
														0,
														0,
														0,
														0,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) );
		}
		else
		{
			x265_enc_search_x_set_residual_qt_data(h,
													enc_search,
													cu,
													0,
													0,
													0,
													NULL,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0),
													0 );
		}

		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)][CI_CURR_BEST] );
		}
#if 0 // check
		{
			x265_enc_entropy_reset_bits(&h->enc_entropy);
			x265_enc_entropy_encode_coeff(h,
										&h->enc_entropy,
										cu,
										0,
										x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0),
										x265_data_cu_get_width_p2(cu, 0),
										x265_data_cu_get_height_p2(cu, 0) );
			i_bits_for_coeff = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)][CI_CURR_BEST] );
			}
			if( i_bits_for_coeff != i_bits )
			{
				assert( 0 );
			}
		}
#endif
		i_bits = 0;
		{
			x265_enc_search_x_add_symbol_bits_inter(h,
													enc_search,
													cu,
													0,
													0,
													&i_bits,
													&p_dummy,
													NULL,
													&p_dummy );
		}


		d_exact_cost = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_bits, i_distortion, 0, DF_DEFAULT);
		d_cost = d_exact_cost;

		if ( d_cost < d_cost_best )
		{
			if ( !x265_base_data_cu_get_qt_root_cbf((x265_base_data_cu_t*)cu, 0))
			{
				x265_simage_clear(*pp_image_resi_best);
			}
			else
			{
				x265_enc_search_x_set_residual_qt_data(h,
														enc_search,
														cu,
														0,
														0,
														0,
														*pp_image_resi_best,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0),
														1 );
			}

			if( qp_min != qp_max && qp != qp_max )
			{
				i_qpart_num = h->cu.pic.i_num_partitions >> (x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) << 1);
				memcpy( enc_search->qt_temp_tr_idx,
						x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu),
						i_qpart_num * sizeof(uint8_t) );
				memcpy( enc_search->qt_temp_cbf[0],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_LUMA ),
						i_qpart_num * sizeof(uint8_t) );
				memcpy( enc_search->qt_temp_cbf[1],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U ),
						i_qpart_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_cbf[2],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V ),
						i_qpart_num * sizeof(uint8_t) );
				memcpy( enc_search->pc_qt_temp_coeff_y,
						x265_data_cu_get_coeff_y(cu),
						i_width * i_height * sizeof( x265_coeff_t ) );
				memcpy( enc_search->pc_qt_temp_coeff_cb,
						x265_data_cu_get_coeff_cb(cu),
						i_width * i_height * sizeof( x265_coeff_t ) >> 2 );
				memcpy( enc_search->pc_qt_temp_coeff_cr,
						x265_data_cu_get_coeff_cr(cu),
						i_width * i_height * sizeof( x265_coeff_t ) >> 2 );
#if X265_ADAPTIVE_QP_SELECTION
				memcpy( enc_search->pc_qt_temp_arl_coeff_y,
						x265_data_cu_get_arl_coeff_y(cu),
						i_width * i_height * sizeof( int32_t ) );
				memcpy( enc_search->pc_qt_temp_arl_coeff_cb,
						x265_data_cu_get_arl_coeff_cb(cu),
						i_width * i_height * sizeof( int32_t ) >> 2 );
				memcpy( enc_search->pc_qt_temp_arl_coeff_cr,
						x265_data_cu_get_arl_coeff_cr(cu),
						i_width * i_height * sizeof( int32_t ) >> 2 );
#endif
				memcpy( enc_search->qt_temp_transform_skip_flag[0],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_LUMA),
						i_qpart_num * sizeof( uint8_t ));
				memcpy( enc_search->qt_temp_transform_skip_flag[1],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_U),
						i_qpart_num * sizeof( uint8_t ) );
				memcpy( enc_search->qt_temp_transform_skip_flag[2],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_V),
						i_qpart_num * sizeof( uint8_t ) );
			}
			i_bits_best  = i_bits;
			i_distortion_best = i_distortion;
			d_cost_best   = d_cost;
			qp_best = qp;
			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)][ CI_TEMP_BEST ] );
			}
		}
	}

	assert ( d_cost_best != X265_MAX_DOUBLE );

	if( qp_min != qp_max && qp_best != qp_max )
	{
		if( h->param.b_use_sbac_rd )
		{
			assert( 0 ); // check
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)][ CI_TEMP_BEST ] );
		}
		// copy best cbf and tr_idx to cu
		i_qpart_num = h->cu.pic.i_num_partitions >> (x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) << 1);
		memcpy( x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu),
				enc_search->qt_temp_tr_idx,
				i_qpart_num * sizeof(uint8_t) );
		memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_LUMA ),
				enc_search->qt_temp_cbf[0],
				i_qpart_num * sizeof(uint8_t) );
		memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U ),
				enc_search->qt_temp_cbf[1],
				i_qpart_num * sizeof(uint8_t) );
		memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V ),
				enc_search->qt_temp_cbf[2],
				i_qpart_num * sizeof(uint8_t) );
		memcpy( x265_data_cu_get_coeff_y(cu),
				enc_search->pc_qt_temp_coeff_y,
				i_width * i_height * sizeof( x265_coeff_t ) );
		memcpy( x265_data_cu_get_coeff_cb(cu),
				enc_search->pc_qt_temp_coeff_cb,
				i_width * i_height * sizeof( x265_coeff_t ) >> 2 );
		memcpy( x265_data_cu_get_coeff_cr(cu),
				enc_search->pc_qt_temp_coeff_cr,
				i_width * i_height * sizeof( x265_coeff_t ) >> 2 );
#if X265_ADAPTIVE_QP_SELECTION
		memcpy( x265_data_cu_get_arl_coeff_y(cu),
				enc_search->pc_qt_temp_arl_coeff_y,
				i_width * i_height * sizeof( int32_t ) );
		memcpy( x265_data_cu_get_arl_coeff_cb(cu),
				enc_search->pc_qt_temp_arl_coeff_cb,
				i_width * i_height * sizeof( int32_t ) >> 2 );
		memcpy( x265_data_cu_get_arl_coeff_cr(cu),
				enc_search->pc_qt_temp_arl_coeff_cr,
				i_width * i_height * sizeof( int32_t ) >> 2 );
#endif
		memcpy( x265_data_cu_get_transform_skip_p2(cu, TEXT_LUMA),
				enc_search->qt_temp_transform_skip_flag[0],
				i_qpart_num * sizeof( uint8_t ) );
		memcpy( x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_U),
				enc_search->qt_temp_transform_skip_flag[1],
				i_qpart_num * sizeof( uint8_t ) );
		memcpy( x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_V),
				enc_search->qt_temp_transform_skip_flag[2],
				i_qpart_num * sizeof( uint8_t ) );
	}
	x265_image_add_clip(h, *pp_image_reco, p_image_pred, *pp_image_resi_best, 0, i_width );

	// update with clipped distortion and cost (qp estimation loop uses unclipped values)
#if X265_WEIGHTED_CHROMA_DISTORTION
	i_distortion_best = x265_rd_cost_get_dist_part(h,
											&h->rd_cost,
											h->cu.pic.i_bit_depth_y,
											x265_image_get_luma_addr_p2(h, *pp_image_reco),
											x265_image_get_stride(*pp_image_reco),
											x265_image_get_luma_addr_p2(h, p_image_org),
											x265_image_get_stride(p_image_org),
											i_width,
											i_height,
											TEXT_LUMA,
											DF_SSE)
						+ x265_rd_cost_get_dist_part(h,
													&h->rd_cost,
													h->cu.pic.i_bit_depth_c,
													x265_image_get_cb_addr_p2(h, *pp_image_reco),
													x265_image_get_c_stride(*pp_image_reco),
													x265_image_get_cb_addr_p2(h, p_image_org),
													x265_image_get_c_stride(p_image_org),
													i_width >> 1,
													i_height >> 1,
													TEXT_CHROMA_U,
													DF_SSE)
						+ x265_rd_cost_get_dist_part(h,
													&h->rd_cost,
													h->cu.pic.i_bit_depth_c,
													x265_image_get_cr_addr_p2(h, *pp_image_reco),
													x265_image_get_c_stride(*pp_image_reco),
													x265_image_get_cr_addr_p2(h, p_image_org),
													x265_image_get_c_stride(p_image_org),
													i_width >> 1,
													i_height >> 1,
													TEXT_CHROMA_V,
													DF_SSE);
#else
	i_distortion_best = x265_rd_cost_get_dist_part(h,
												&h->rd_cost,
												h->cu.pic.i_bit_depth_y,
												x265_image_get_luma_addr_p2(h, *pp_image_reco),
												x265_image_get_stride(*pp_image_reco),
												x265_image_get_luma_addr_p2(h, p_image_org),
												x265_image_get_stride(p_image_org),
												i_width,
												i_height,
												DF_SSE)
					+ x265_rd_cost_get_dist_part(h,
												&h->rd_cost,
												h->cu.pic.i_bit_depth_c,
												x265_image_get_cb_addr_p2(h, *pp_image_reco),
												x265_image_get_c_stride(*pp_image_reco),
												x265_image_get_cb_addr_p2(h, p_image_org),
												x265_image_get_c_stride(p_image_org),
												i_width >> 1,
												i_height >> 1,
												DF_SSE)
					+ x265_rd_cost_get_dist_part(h,
												&h->rd_cost,
												h->cu.pic.i_bit_depth_c,
												x265_image_get_cr_addr_p2(h, *pp_image_reco),
												x265_image_get_c_stride(*pp_image_reco),
												x265_image_get_cr_addr_p2(h, p_image_org),
												x265_image_get_c_stride(p_image_org),
												i_width >> 1,
												i_height >> 1,
												DF_SSE);
#endif
	d_cost_best = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_bits_best, i_distortion_best, 0, DF_DEFAULT );

	cu->i_total_bits = i_bits_best;
	cu->i_total_distortion = i_distortion_best;
	cu->f_total_cost = d_cost_best;

	if ( x265_base_data_cu_is_skipped((x265_base_data_cu_t*)cu, 0) )
	{
		x265_data_cu_set_cbf_sub_parts_p7_1(h,
											cu,
											0,
											0,
											0,
											0,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 ) );
	}

	x265_base_data_cu_set_qp_sub_parts(h,
										(x265_base_data_cu_t*)cu,
										qp_best,
										0,
										x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));

}




void x265_enc_search_x_estimate_residual_qt(x265_t* h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_quadrant,
											uint32_t i_abs_part_idx,
											uint32_t abs_t_upart_idx,
											x265_simage_t *resi,
											const uint32_t i_depth,
											double *p_cost,
											uint32_t *p_bits,
											uint32_t *p_dist,
											uint32_t *p_zero_dist )
{
	int32_t loop = 0 ;
	uint32_t i_tr_mode = 0;
	uint32_t i_log2tr_size = 0;
	uint32_t split_flag = 0;
	int32_t b_check_full = 0;
	int32_t b_check_split = 0;
	int32_t b_code_chroma = 0;
	uint32_t i_tr_mode_c = 0;
	uint32_t i_log2tr_size_c = 0;
	uint32_t i_qpdiv = 0;
	uint32_t i_set_cbf = 0;
	// code full block
	double d_single_cost = 0.0;
	uint32_t i_single_bits = 0;
	uint32_t i_single_dist = 0;
	uint32_t i_abs_sum_y = 0, i_abs_sum_u = 0, i_abs_sum_v = 0;
	uint32_t i_best_transform_mode[3] = {0};
	uint32_t i_num_coeff_per_abs_part_idx_increment = 0;
	uint32_t i_qt_temp_access_layer = 0;
	x265_coeff_t *coeff_curr_y = NULL;
	x265_coeff_t *coeff_curr_u = NULL;
	x265_coeff_t *coeff_curr_v = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_curr_y = NULL;
	int32_t *arl_coeff_curr_u = NULL;
	int32_t *arl_coeff_curr_v = NULL;
#endif
	int32_t tr_width = 0, tr_height = 0, tr_width_c = 0, tr_height_c = 0;
	uint32_t abs_t_upart_idx_c = 0;
	double min_cost_y = 0.0;
	double min_cost_u = 0.0;
	double min_cost_v = 0.0;
	int32_t b_check_transform_skip_y = 0;
	int32_t b_check_transform_skip_uv = 0;
	int32_t cur_chroma_qp_offset = 0;
	uint32_t i_single_bits_y = 0;
	uint32_t i_single_bits_u = 0;
	uint32_t i_single_bits_v = 0;
	uint32_t i_num_samples_luma = 0;
	uint32_t i_num_samples_chro = 0;
	uint32_t i_dist_y = 0;
	spixel *resi_curr_y = NULL;
	int32_t scaling_list_type = 0;
	uint32_t i_nonzero_dist_y = 0;
	double single_cost_y = 0.0;
	uint32_t i_null_bits_y = 0;
	double null_cost_y = 0.0;
	spixel *ptr = NULL;
	uint32_t i_stride = 0;
	uint32_t i_y = 0;
	uint32_t i_dist_u = 0;
	uint32_t i_dist_v = 0;
	spixel *resi_curr_u = NULL;
	uint32_t i_nonzero_dist_u = 0;
	double d_single_cost_u = 0.0;
	uint32_t i_null_bits_u = 0;
	double d_null_cost_u = 0.0;
	spixel *resi_curr_v = NULL;
	double d_single_cost_v = 0.0;
	uint32_t i_null_bits_v = 0;
	double d_null_cost_v = 0.0;
	uint32_t i_abs_sum_transform_skip_y;
	double d_single_cost_y;
	uint32_t resi_y_stride = 0;
	x265_coeff_t best_coeff_y[32*32];
#if X265_ADAPTIVE_QP_SELECTION
	x265_coeff_t best_arl_coeff_y[32*32];
#endif
	spixel best_resi_y[32*32];
	uint32_t i_ts_single_bits_y = 0;
	uint32_t i_nonzero_dist_v, i_abs_sum_transform_skipu, i_abs_sum_transform_skip_v;
	uint32_t resi_c_stride = 0;
	x265_coeff_t best_coeff_u[32*32], best_coeff_v[32*32];
#if X265_ADAPTIVE_QP_SELECTION
	x265_coeff_t best_arl_coeff_u[32*32], best_arl_coeff_v[32*32];
#endif
	spixel best_resi_u[32*32], best_resi_v[32*32];
	uint32_t i_subdiv_dist = 0;
	uint32_t i_subdiv_bits = 0;
	double d_subdiv_cost = 0.0;
	uint32_t i_qpart_num_subdiv = 0;
	uint32_t ns_addr = 0;
	uint32_t i_y_cbf = 0;
	uint32_t i_u_cbf = 0;
	uint32_t i_v_cbf = 0;


	i_tr_mode = i_depth - x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 );
	assert( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 )
			== x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) );
	i_log2tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> i_depth]+2;
	split_flag = ((h->sps[0].i_quadtree_tu_max_depth_inter == 1)
					&& x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) == MODE_INTER
					&& ( x265_data_cu_get_partition_size_p2(cu, i_abs_part_idx) != SIZE_2Nx2N ));
	if(split_flag
			&& i_depth == x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx)
			&& ( i_log2tr_size >  x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx)))
	{
		b_check_full = 0;
	}
	else
	{
		b_check_full =  ( i_log2tr_size <= h->sps[0].i_quadtree_tu_log2_max_size );
	}

	b_check_split  = ( i_log2tr_size >  x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx) );
	assert( b_check_full || b_check_split );
	b_code_chroma   = 1;
	i_tr_mode_c= i_tr_mode;
	i_log2tr_size_c = i_log2tr_size-1;
	if( i_log2tr_size == 2 )
	{
		i_log2tr_size_c++;
		i_tr_mode_c --;
		i_qpdiv = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 ) + i_tr_mode_c ) << 1 );
		b_code_chroma = ( ( i_abs_part_idx % i_qpdiv ) == 0 );
	}

	i_set_cbf = 1 << i_tr_mode;
	// code full block
	d_single_cost = X265_MAX_DOUBLE;

	if( h->param.b_use_sbac_rd )
	{
		x265_enc_sbac_store(&h->rd_go_on_sbac_coder, h->pppc_rd_sbac_coder[ i_depth ][ CI_QT_TRAFO_ROOT ] );
	}

	if( b_check_full )
	{
		i_num_coeff_per_abs_part_idx_increment = h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height >> (h->sps[0].i_max_cu_depth << 1 );
		i_qt_temp_access_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2tr_size;
		coeff_curr_y = enc_search->ppc_qt_temp_coeff_y [i_qt_temp_access_layer] +  i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx;
		coeff_curr_u = enc_search->ppc_qt_temp_coeff_cb[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
		coeff_curr_v = enc_search->ppc_qt_temp_coeff_cr[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
#if X265_ADAPTIVE_QP_SELECTION
		arl_coeff_curr_y = enc_search->ppc_qt_temp_arl_coeff_y [i_qt_temp_access_layer] +  i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx;
		arl_coeff_curr_u = enc_search->ppc_qt_temp_arl_coeff_cb[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
		arl_coeff_curr_v = enc_search->ppc_qt_temp_arl_coeff_cr[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
#endif
		abs_t_upart_idx_c = i_abs_part_idx;

		tr_width  = tr_height  = 1 << i_log2tr_size;
		tr_width_c = tr_height_c = 1 <<i_log2tr_size_c;
		x265_base_data_cu_set_tr_idx_sub_parts(h,
												(x265_base_data_cu_t*)cu,
												i_depth - x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 ),
												i_abs_part_idx,
												i_depth );
		min_cost_y = X265_MAX_DOUBLE;
		min_cost_u = X265_MAX_DOUBLE;
		min_cost_v = X265_MAX_DOUBLE;
		b_check_transform_skip_y = h->pps[0].b_use_transform_skip && tr_width == 4 && tr_height == 4;
		b_check_transform_skip_uv = h->pps[0].b_use_transform_skip && tr_width_c == 4 && tr_height_c == 4;

		b_check_transform_skip_y &= (!x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, 0));
		b_check_transform_skip_uv &= (!x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, 0));

		x265_data_cu_set_transform_skip_sub_parts_p6(h,
													cu,
													0,
													TEXT_LUMA,
													i_abs_part_idx,
													i_depth );
		if( b_code_chroma )
		{
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														0,
														TEXT_CHROMA_U,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c );
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														0,
														TEXT_CHROMA_V,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c );
		}

		if (h->param.rdo.b_use_rdo_q)
		{
			x265_enc_entropy_estimate_bit(h,
										&h->enc_entropy,
										h->tr_quant.est_bits_sbac,
										tr_width,
										tr_height,
										TEXT_LUMA );
		}

		x265_tr_quant_set_qp_for_quant(&h->tr_quant,
										x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
										TEXT_LUMA,
										h->sps[0].i_qp_bd_offset_y,
										0 );

#if X265_RDOQ_CHROMA_LAMBDA
		x265_tr_quant_select_lambda(&h->tr_quant, TEXT_LUMA);
#endif
		x265_tr_quant_transform_nxn(h,
									&h->tr_quant,
									cu,
									x265_simage_get_luma_addr_p3(h, resi, abs_t_upart_idx ),
									x265_simage_get_stride(resi),
									coeff_curr_y,
#if X265_ADAPTIVE_QP_SELECTION
									&arl_coeff_curr_y,
#endif
									tr_width,
									tr_height,
									&i_abs_sum_y,
									TEXT_LUMA,
									i_abs_part_idx,
									0 );

		x265_data_cu_set_cbf_sub_parts_p6(h,
											cu,
											i_abs_sum_y ? i_set_cbf : 0,
											TEXT_LUMA,
											i_abs_part_idx,
											i_depth );

		if( b_code_chroma )
		{
			if (h->param.rdo.b_use_rdo_q)
			{
				x265_enc_entropy_estimate_bit(h,
											&h->enc_entropy,
											h->tr_quant.est_bits_sbac,
											tr_width_c,
											tr_height_c,
											TEXT_CHROMA );
			}

			cur_chroma_qp_offset = h->pps[0].i_chroma_cb_qp_offset + h->slice->i_slice_qp_delta_cb ;
			x265_tr_quant_set_qp_for_quant(&h->tr_quant,
											x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
											TEXT_CHROMA,
											h->sps[0].i_qp_bd_offset_c,
											cur_chroma_qp_offset );

#if X265_RDOQ_CHROMA_LAMBDA
			x265_tr_quant_select_lambda(&h->tr_quant, TEXT_CHROMA);
#endif

			x265_tr_quant_transform_nxn(h,
										&h->tr_quant,
										cu,
										x265_simage_get_cb_addr_p3(h, resi, abs_t_upart_idx ),
										x265_simage_get_c_stride(resi),
										coeff_curr_u,
#if X265_ADAPTIVE_QP_SELECTION
										&arl_coeff_curr_u,
#endif
										tr_width_c,
										tr_height_c,
										&i_abs_sum_u,
										TEXT_CHROMA_U,
										i_abs_part_idx,
										0 );

			cur_chroma_qp_offset = h->pps[0].i_chroma_cr_qp_offset + h->slice->i_slice_qp_delta_cr ;
			x265_tr_quant_set_qp_for_quant(&h->tr_quant,
											x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
											TEXT_CHROMA,
											h->sps[0].i_qp_bd_offset_c,
											cur_chroma_qp_offset );
			x265_tr_quant_transform_nxn(h,
										&h->tr_quant,
										cu,
										x265_simage_get_cr_addr_p3(h, resi, abs_t_upart_idx ),
										x265_simage_get_c_stride(resi),
										coeff_curr_v,
#if X265_ADAPTIVE_QP_SELECTION
										&arl_coeff_curr_u,
#endif
										tr_width_c,
										tr_height_c,
										&i_abs_sum_v,
										TEXT_CHROMA_V,
										i_abs_part_idx,
										0 );

			x265_data_cu_set_cbf_sub_parts_p6(h,
												cu,
												i_abs_sum_u ? i_set_cbf : 0,
												TEXT_CHROMA_U,
												i_abs_part_idx,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c );
			x265_data_cu_set_cbf_sub_parts_p6(h,
												cu,
												i_abs_sum_v ? i_set_cbf : 0,
												TEXT_CHROMA_V,
												i_abs_part_idx,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c );
		}

		x265_enc_entropy_reset_bits(&h->enc_entropy);

		{
			x265_enc_entropy_encode_qt_cbf(h,
											&h->enc_entropy,
											cu,
											i_abs_part_idx,
											TEXT_LUMA,
											i_tr_mode );
		}

		x265_enc_entropy_encode_coeff_nxn(h,
										&h->enc_entropy,
										cu,
										coeff_curr_y,
										i_abs_part_idx,
										tr_width,
										tr_height,
										i_depth,
										TEXT_LUMA );
		i_single_bits_y = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);

		i_single_bits_u = 0;
		i_single_bits_v = 0;
		if( b_code_chroma )
		{
			{
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_CHROMA_U,
												i_tr_mode );
			}
			x265_enc_entropy_encode_coeff_nxn(h,
												&h->enc_entropy,
												cu,
												coeff_curr_u,
												i_abs_part_idx,
												tr_width_c,
												tr_height_c,
												i_depth,
												TEXT_CHROMA_U );
			i_single_bits_u = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) - i_single_bits_y;

			{
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_CHROMA_V,
												i_tr_mode );
			}
			x265_enc_entropy_encode_coeff_nxn(h,
												&h->enc_entropy,
												cu,
												coeff_curr_v,
												i_abs_part_idx,
												tr_width_c,
												tr_height_c,
												i_depth,
												TEXT_CHROMA_V );
			i_single_bits_v = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) - ( i_single_bits_y + i_single_bits_u );
		}

		i_num_samples_luma = 1 << (i_log2tr_size<<1);
		i_num_samples_chro = 1 << (i_log2tr_size_c<<1);

		memset( enc_search->temp_pel, 0, sizeof( pixel ) * i_num_samples_luma ); // not necessary needed for inside of recursion (only at the beginning)

		i_dist_y = x265_rd_cost_get_sse_p_s(h,
											&h->rd_cost,
											h->cu.pic.i_bit_depth_y,
											enc_search->temp_pel,
											tr_width,
											x265_simage_get_luma_addr_p3(h, resi, abs_t_upart_idx),
											x265_simage_get_stride(resi),
											tr_width,
											tr_height,
											TEXT_LUMA,
											DF_SSE ); // initialized with zero residual destortion

		if ( p_zero_dist )
		{
			*p_zero_dist += i_dist_y;
		}
		if( i_abs_sum_y )
		{
			resi_curr_y = x265_simage_get_luma_addr_p3(h, enc_search->ppc_qt_temp_simage[ i_qt_temp_access_layer ], abs_t_upart_idx );

			x265_tr_quant_set_qp_for_quant(&h->tr_quant,
											x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
											TEXT_LUMA,
											h->sps[0].i_qp_bd_offset_y,
											0 );

			scaling_list_type = 3 + et_table[(int32_t)TEXT_LUMA];
			assert(scaling_list_type < 6);
			x265_tr_quant_inv_transform_nxn(h,
											&h->tr_quant,
											x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
											TEXT_LUMA,
											X265_REG_DCT,
											resi_curr_y,
											x265_simage_get_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
											coeff_curr_y,
											tr_width,
											tr_height,
											scaling_list_type,
											0 );//this is for inter mode only

			i_nonzero_dist_y = x265_rd_cost_get_sse_s_s(h,
														&h->rd_cost,
														h->cu.pic.i_bit_depth_y,
														x265_simage_get_luma_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx ),
														x265_simage_get_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
														x265_simage_get_luma_addr_p3(h, resi, abs_t_upart_idx ),
														x265_simage_get_stride(resi),
														tr_width,
														tr_height,
														TEXT_LUMA,
														DF_SSE );
			if (x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, 0))
			{
				i_dist_y = i_nonzero_dist_y;
			}
			else
			{
				single_cost_y = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_single_bits_y, i_nonzero_dist_y, 0, DF_DEFAULT );
				x265_enc_entropy_reset_bits(&h->enc_entropy);
				x265_enc_entropy_encode_qt_cbf_zero(&h->enc_entropy, cu, TEXT_LUMA,i_tr_mode );
				i_null_bits_y   = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
				null_cost_y   = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_null_bits_y, i_dist_y, 0, DF_DEFAULT);
				if( null_cost_y < single_cost_y )
				{
					i_abs_sum_y = 0;
					memset( coeff_curr_y, 0, sizeof( x265_coeff_t ) * i_num_samples_luma );
					if( b_check_transform_skip_y )
					{
						min_cost_y = null_cost_y;
					}
				}
				else
				{
					i_dist_y = i_nonzero_dist_y;
					if( b_check_transform_skip_y )
					{
						min_cost_y = single_cost_y;
					}
				}
			}
		}
		else if( b_check_transform_skip_y )
		{
			x265_enc_entropy_reset_bits(&h->enc_entropy);
			x265_enc_entropy_encode_qt_cbf_zero(&h->enc_entropy, cu, TEXT_LUMA, i_tr_mode );
			i_null_bits_y = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
			min_cost_y = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_null_bits_y, i_dist_y, 0, DF_DEFAULT );
		}

		if( !i_abs_sum_y )
		{
			ptr = x265_simage_get_luma_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx );
			i_stride = x265_simage_get_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]);
			for( i_y = 0; i_y < tr_height; ++i_y )
			{
				memset( ptr, 0, sizeof( spixel ) * tr_width );
				ptr += i_stride;
			}
		}

		i_dist_u = 0;
		i_dist_v = 0;
		if( b_code_chroma )
		{
			i_dist_u = x265_rd_cost_get_sse_p_s(h,
												&h->rd_cost,
												h->cu.pic.i_bit_depth_c,
												enc_search->temp_pel,
												tr_width_c,
												x265_simage_get_cb_addr_p3(h, resi, abs_t_upart_idx_c ),
												x265_simage_get_c_stride(resi),
												tr_width_c,
												tr_height_c
#if X265_WEIGHTED_CHROMA_DISTORTION
												,TEXT_CHROMA_U
#endif
												,DF_SSE); // initialized with zero residual destortion
			if ( p_zero_dist )
			{
				*p_zero_dist += i_dist_u;
			}
			if( i_abs_sum_u )
			{
				resi_curr_u = x265_simage_get_cb_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c );
				cur_chroma_qp_offset = h->pps[0].i_chroma_cb_qp_offset + h->slice->i_slice_qp_delta_cb;
				x265_tr_quant_set_qp_for_quant(&h->tr_quant,
												x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
												TEXT_CHROMA,
												h->sps[0].i_qp_bd_offset_c,
												cur_chroma_qp_offset );

				scaling_list_type = 3 + et_table[(int32_t)TEXT_CHROMA_U];
				assert(scaling_list_type < 6);
				x265_tr_quant_inv_transform_nxn(h,
												&h->tr_quant,
												x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
												TEXT_CHROMA,
												X265_REG_DCT,
												resi_curr_u,
												x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
												coeff_curr_u,
												tr_width_c,
												tr_height_c,
												scaling_list_type,
												0);

				i_nonzero_dist_u = x265_rd_cost_get_sse_s_s(h,
															&h->rd_cost,
															h->cu.pic.i_bit_depth_c,
															x265_simage_get_cb_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c),
															x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
															x265_simage_get_cb_addr_p3(h, resi, abs_t_upart_idx_c),
															x265_simage_get_c_stride(resi),
															tr_width_c,
															tr_height_c
#if X265_WEIGHTED_CHROMA_DISTORTION
															,TEXT_CHROMA_U
#endif
															,DF_SSE);

				if(x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, 0))
				{
					i_dist_u = i_nonzero_dist_u;
				}
				else
				{
					d_single_cost_u = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_single_bits_u, i_nonzero_dist_u, 0, DF_DEFAULT );
					x265_enc_entropy_reset_bits(&h->enc_entropy);
					x265_enc_entropy_encode_qt_cbf_zero(&h->enc_entropy, cu, TEXT_CHROMA_U,i_tr_mode );
					i_null_bits_u = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
					d_null_cost_u = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_null_bits_u, i_dist_u, 0, DF_DEFAULT );
					if( d_null_cost_u < d_single_cost_u )
					{
						i_abs_sum_u = 0;
						memset( coeff_curr_u, 0, sizeof( x265_coeff_t ) * i_num_samples_chro );
						if( b_check_transform_skip_uv )
						{
							min_cost_u = d_null_cost_u;
						}
					}
					else
					{
						i_dist_u = i_nonzero_dist_u;
						if( b_check_transform_skip_uv )
						{
							min_cost_u = d_single_cost_u;
						}
					}
				}
			}
			else if( b_check_transform_skip_uv )
			{
				x265_enc_entropy_reset_bits(&h->enc_entropy);
				x265_enc_entropy_encode_qt_cbf_zero(&h->enc_entropy, cu, TEXT_CHROMA_U, i_tr_mode_c );
				i_null_bits_u = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
				min_cost_u = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_null_bits_u, i_dist_u, 0, DF_DEFAULT );
			}
			if( !i_abs_sum_u )
			{
				ptr = x265_simage_get_cb_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c );
				i_stride = x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]);
				for( i_y = 0; i_y < tr_height_c; ++i_y )
				{
					memset( ptr, 0, sizeof(spixel) * tr_width_c );
					ptr += i_stride;
				}
			}

			i_dist_v = x265_rd_cost_get_sse_p_s(h,
												&h->rd_cost,
												h->cu.pic.i_bit_depth_c,
												enc_search->temp_pel,
												tr_width_c,
												x265_simage_get_cr_addr_p3(h, resi, abs_t_upart_idx_c),
												x265_simage_get_c_stride(resi),
												tr_width_c,
												tr_height_c
#if X265_WEIGHTED_CHROMA_DISTORTION
												,TEXT_CHROMA_V
#endif
												,DF_SSE); // initialized with zero residual destortion
			if ( p_zero_dist )
			{
				*p_zero_dist += i_dist_v;
			}
			if( i_abs_sum_v )
			{
				resi_curr_v = x265_simage_get_cr_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c );
				cur_chroma_qp_offset = h->pps[0].i_chroma_cr_qp_offset + h->slice->i_slice_qp_delta_cr;
				x265_tr_quant_set_qp_for_quant(&h->tr_quant,
												x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
												TEXT_CHROMA,
												h->sps[0].i_qp_bd_offset_c,
												cur_chroma_qp_offset );

				scaling_list_type = 3 + et_table[(int32_t)TEXT_CHROMA_V];
				assert(scaling_list_type < 6);
				x265_tr_quant_inv_transform_nxn(h,
												&h->tr_quant,
												x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
												TEXT_CHROMA,
												X265_REG_DCT,
												resi_curr_v,
												x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
												coeff_curr_v,
												tr_width_c,
												tr_height_c,
												scaling_list_type,
												0 );

				i_nonzero_dist_v = x265_rd_cost_get_sse_s_s(h,
															&h->rd_cost,
															h->cu.pic.i_bit_depth_c,
															x265_simage_get_cr_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c),
															x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
															x265_simage_get_cr_addr_p3(h, resi, abs_t_upart_idx_c),
															x265_simage_get_c_stride(resi),
															tr_width_c,
															tr_height_c
#if X265_WEIGHTED_CHROMA_DISTORTION
															,TEXT_CHROMA_V
#endif
															,DF_SSE);
				if (x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, 0))
				{
					i_dist_v = i_nonzero_dist_v;
				}
				else
				{
					d_single_cost_v = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_single_bits_v, i_nonzero_dist_v, 0, DF_DEFAULT );
					x265_enc_entropy_reset_bits(&h->enc_entropy);
					x265_enc_entropy_encode_qt_cbf_zero(&h->enc_entropy, cu, TEXT_CHROMA_V,i_tr_mode );
					i_null_bits_v = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
					d_null_cost_v = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_null_bits_v, i_dist_v, 0, DF_DEFAULT );
					if( d_null_cost_v < d_single_cost_v )
					{
						i_abs_sum_v = 0;
						memset( coeff_curr_v, 0, sizeof( x265_coeff_t ) * i_num_samples_chro );
						if( b_check_transform_skip_uv )
						{
							min_cost_v = d_null_cost_v;
						}
					}
					else
					{
						i_dist_v = i_nonzero_dist_v;
						if( b_check_transform_skip_uv )
						{
							min_cost_v = d_single_cost_v;
						}
					}
				}
			}
			else if( b_check_transform_skip_uv )
			{
				x265_enc_entropy_reset_bits(&h->enc_entropy);
				x265_enc_entropy_encode_qt_cbf_zero(&h->enc_entropy, cu, TEXT_CHROMA_V, i_tr_mode_c );
				i_null_bits_v = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
				min_cost_v = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_null_bits_v, i_dist_v, 0, DF_DEFAULT );
			}
			if( !i_abs_sum_v )
			{
				ptr = x265_simage_get_cr_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c );
				i_stride = x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]);
				for( i_y = 0; i_y < tr_height_c; ++i_y )
				{
					memset( ptr, 0, sizeof(spixel) * tr_width_c );
					ptr += i_stride;
				}
			}
		}
		x265_data_cu_set_cbf_sub_parts_p6(h,
										cu,
										i_abs_sum_y ? i_set_cbf : 0,
										TEXT_LUMA,
										i_abs_part_idx,
										i_depth );
		if( b_code_chroma )
		{
			x265_data_cu_set_cbf_sub_parts_p6(h,
											cu,
											i_abs_sum_u ? i_set_cbf : 0,
											TEXT_CHROMA_U,
											i_abs_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c );
			x265_data_cu_set_cbf_sub_parts_p6(h,
											cu,
											i_abs_sum_v ? i_set_cbf : 0,
											TEXT_CHROMA_V,
											i_abs_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c );
		}

		if( b_check_transform_skip_y )
		{
			resi_curr_y = x265_simage_get_luma_addr_p3(h, enc_search->ppc_qt_temp_simage[ i_qt_temp_access_layer ], abs_t_upart_idx );
			resi_y_stride = x265_simage_get_stride(enc_search->ppc_qt_temp_simage[ i_qt_temp_access_layer ]);

			memcpy( best_coeff_y, coeff_curr_y, sizeof(x265_coeff_t) * i_num_samples_luma );

#if X265_ADAPTIVE_QP_SELECTION
			memcpy( best_arl_coeff_y, arl_coeff_curr_y, sizeof(x265_coeff_t) * i_num_samples_luma );
#endif

			for ( loop = 0; loop < tr_height; ++loop )
			{
				memcpy( &best_resi_y[loop*tr_width],
						resi_curr_y+loop*resi_y_stride,
						sizeof(spixel) * tr_width );
			}

			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_load(&h->rd_go_on_sbac_coder, h->pppc_rd_sbac_coder[ i_depth ][ CI_QT_TRAFO_ROOT ] );
			}

			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														1,
														TEXT_LUMA,
														i_abs_part_idx,
														i_depth );

			if (h->param.rdo.b_use_rdo_q_ts)
			{
				x265_enc_entropy_estimate_bit(h,
											&h->enc_entropy,
											h->tr_quant.est_bits_sbac,
											tr_width,
											tr_height,
											TEXT_LUMA );
			}

			x265_tr_quant_set_qp_for_quant(&h->tr_quant,
											x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
											TEXT_LUMA,
											h->sps[0].i_qp_bd_offset_y,
											0 );

#if X265_RDOQ_CHROMA_LAMBDA
			x265_tr_quant_select_lambda(&h->tr_quant, TEXT_LUMA);
#endif
			x265_tr_quant_transform_nxn(h,
										&h->tr_quant,
										cu,
										x265_simage_get_luma_addr_p3(h, resi, abs_t_upart_idx ),
										x265_simage_get_stride(resi),
										coeff_curr_y,
#if X265_ADAPTIVE_QP_SELECTION
										&arl_coeff_curr_y,
#endif
										tr_width,
										tr_height,
										&i_abs_sum_transform_skip_y,
										TEXT_LUMA,
										i_abs_part_idx,
										1 );
			x265_data_cu_set_cbf_sub_parts_p6(h,
											cu,
											i_abs_sum_transform_skip_y ? i_set_cbf : 0,
											TEXT_LUMA,
											i_abs_part_idx,
											i_depth );

			if( i_abs_sum_transform_skip_y != 0 )
			{
				x265_enc_entropy_reset_bits(&h->enc_entropy);
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_LUMA,
												i_tr_mode );
				x265_enc_entropy_encode_coeff_nxn(h,
												&h->enc_entropy,
												cu,
												coeff_curr_y,
												i_abs_part_idx,
												tr_width,
												tr_height,
												i_depth,
												TEXT_LUMA );
				i_ts_single_bits_y = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);

				x265_tr_quant_set_qp_for_quant(&h->tr_quant,
												x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
												TEXT_LUMA,
												h->sps[0].i_qp_bd_offset_y,
												0 );

				scaling_list_type = 3 + et_table[(int32_t)TEXT_LUMA];
				assert(scaling_list_type < 6);

				x265_tr_quant_inv_transform_nxn(h,
												&h->tr_quant,
												x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
												TEXT_LUMA,
												X265_REG_DCT,
												resi_curr_y,
												x265_simage_get_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
												coeff_curr_y,
												tr_width,
												tr_height,
												scaling_list_type,
												1 );

				i_nonzero_dist_y = x265_rd_cost_get_sse_s_s(h,
															&h->rd_cost,
															h->cu.pic.i_bit_depth_y,
															x265_simage_get_luma_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx ),
															x265_simage_get_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
															x265_simage_get_luma_addr_p3(h, resi, abs_t_upart_idx ),
															x265_simage_get_stride(resi),
															tr_width,
															tr_height,
															TEXT_LUMA,
															DF_SSE );

				d_single_cost_y = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_ts_single_bits_y, i_nonzero_dist_y, 0, DF_DEFAULT );
			}

			if( !i_abs_sum_transform_skip_y || min_cost_y < d_single_cost_y )
			{
				x265_data_cu_set_transform_skip_sub_parts_p6(h,
															cu,
															0,
															TEXT_LUMA,
															i_abs_part_idx,
															i_depth );
				memcpy( coeff_curr_y, best_coeff_y, sizeof(x265_coeff_t) * i_num_samples_luma );
#if X265_ADAPTIVE_QP_SELECTION
				memcpy( arl_coeff_curr_y, best_arl_coeff_y, sizeof(x265_coeff_t) * i_num_samples_luma );
#endif
				for( int32_t i = 0; i < tr_height; ++i )
				{
					memcpy( resi_curr_y+i*resi_y_stride, &best_resi_y[i*tr_width], sizeof(spixel) * tr_width );
				}
			}
			else
			{
				i_dist_y = i_nonzero_dist_y;
				i_abs_sum_y = i_abs_sum_transform_skip_y;
				i_best_transform_mode[0] = 1;
			}

			x265_data_cu_set_cbf_sub_parts_p6(h,
												cu,
												i_abs_sum_y ? i_set_cbf : 0,
												TEXT_LUMA,
												i_abs_part_idx,
												i_depth );
		}

		if( b_code_chroma && b_check_transform_skip_uv  )
		{
			resi_curr_u = x265_simage_get_cb_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c );
			resi_curr_v = x265_simage_get_cr_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c );
			resi_c_stride = x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]);

			memcpy( best_coeff_u, coeff_curr_u, sizeof(x265_coeff_t) * i_num_samples_chro );
			memcpy( best_coeff_v, coeff_curr_v, sizeof(x265_coeff_t) * i_num_samples_chro );
#if X265_ADAPTIVE_QP_SELECTION
			memcpy( best_arl_coeff_u, arl_coeff_curr_u, sizeof(x265_coeff_t) * i_num_samples_chro );
			memcpy( best_arl_coeff_v, arl_coeff_curr_v, sizeof(x265_coeff_t) * i_num_samples_chro );
#endif

			for (loop = 0; loop < tr_height_c; ++loop )
			{
				memcpy( &best_resi_u[loop*tr_width_c],
						resi_curr_u+loop*resi_c_stride,
						sizeof(spixel) * tr_width_c );
				memcpy( &best_resi_v[loop*tr_width_c],
						resi_curr_v+loop*resi_c_stride,
						sizeof(spixel) * tr_width_c );
			}

			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_load(&h->rd_go_on_sbac_coder, h->pppc_rd_sbac_coder[ i_depth ][ CI_QT_TRAFO_ROOT ] );
			}

			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														1,
														TEXT_CHROMA_U,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c );
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														1,
														TEXT_CHROMA_V,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c );

			if (h->param.rdo.b_use_rdo_q_ts)
			{
				x265_enc_entropy_estimate_bit(h,
											&h->enc_entropy,
											h->tr_quant.est_bits_sbac,
											tr_width_c,
											tr_height_c,
											TEXT_CHROMA );
			}

			cur_chroma_qp_offset = h->pps[0].i_chroma_cb_qp_offset + h->slice->i_slice_qp_delta_cb;
			x265_tr_quant_set_qp_for_quant(&h->tr_quant,
											x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
											TEXT_CHROMA,
											h->sps[0].i_qp_bd_offset_c,
											cur_chroma_qp_offset );

#if X265_RDOQ_CHROMA_LAMBDA
			x265_tr_quant_select_lambda(&h->tr_quant, TEXT_CHROMA);
#endif

			x265_tr_quant_transform_nxn(h,
										&h->tr_quant,
										cu,
										x265_simage_get_cb_addr_p3(h, resi, abs_t_upart_idx ),
										x265_simage_get_c_stride(resi),
										coeff_curr_u,
#if X265_ADAPTIVE_QP_SELECTION
										&arl_coeff_curr_u,
#endif
										tr_width_c,
										tr_height_c,
										&i_abs_sum_transform_skipu,
										TEXT_CHROMA_U,
										i_abs_part_idx,
										1 );
			cur_chroma_qp_offset = h->pps[0].i_chroma_cr_qp_offset + h->slice->i_slice_qp_delta_cr;
			x265_tr_quant_set_qp_for_quant(&h->tr_quant,
											x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
											TEXT_CHROMA,
											h->sps[0].i_qp_bd_offset_c,
											cur_chroma_qp_offset );
			x265_tr_quant_transform_nxn(h,
										&h->tr_quant,
										cu,
										x265_simage_get_cr_addr_p3(h, resi, abs_t_upart_idx ),
										x265_simage_get_c_stride(resi),
										coeff_curr_v,
#if X265_ADAPTIVE_QP_SELECTION
										&arl_coeff_curr_v,
#endif
										tr_width_c,
										tr_height_c,
										&i_abs_sum_transform_skip_v,
										TEXT_CHROMA_V,
										i_abs_part_idx,
										1 );

			x265_data_cu_set_cbf_sub_parts_p6(h,
												cu,
												i_abs_sum_transform_skipu ? i_set_cbf : 0,
												TEXT_CHROMA_U,
												i_abs_part_idx,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );
			x265_data_cu_set_cbf_sub_parts_p6(h,
												cu,
												i_abs_sum_transform_skip_v ? i_set_cbf : 0,
												TEXT_CHROMA_V,
												i_abs_part_idx,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );

			x265_enc_entropy_reset_bits(&h->enc_entropy);
			i_single_bits_u = 0;
			i_single_bits_v = 0;

			if( i_abs_sum_transform_skipu )
			{
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_CHROMA_U,
												i_tr_mode );
				x265_enc_entropy_encode_coeff_nxn(h,
												&h->enc_entropy,
												cu,
												coeff_curr_u,
												i_abs_part_idx,
												tr_width_c,
												tr_height_c,
												i_depth,
												TEXT_CHROMA_U );
				i_single_bits_u = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);

				cur_chroma_qp_offset = h->pps[0].i_chroma_cb_qp_offset + h->slice->i_slice_qp_delta_cb;
				x265_tr_quant_set_qp_for_quant(&h->tr_quant,
												x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
												TEXT_CHROMA,
												h->sps[0].i_qp_bd_offset_c,
												cur_chroma_qp_offset );

				scaling_list_type = 3 + et_table[(int32_t)TEXT_CHROMA_U];
				assert(scaling_list_type < 6);

				x265_tr_quant_inv_transform_nxn(h,
												&h->tr_quant,
												x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
												TEXT_CHROMA,
												X265_REG_DCT,
												resi_curr_u,
												x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
												coeff_curr_u,
												tr_width_c,
												tr_height_c,
												scaling_list_type,
												1);

				i_nonzero_dist_u = x265_rd_cost_get_sse_s_s(h,
															&h->rd_cost,
															h->cu.pic.i_bit_depth_c,
															x265_simage_get_cb_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c),
															x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
															x265_simage_get_cb_addr_p3(h, resi, abs_t_upart_idx_c),
															x265_simage_get_c_stride(resi),
															tr_width_c,
															tr_height_c
#if X265_WEIGHTED_CHROMA_DISTORTION
															,TEXT_CHROMA_U
#endif
															,DF_SSE);

				d_single_cost_u = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_single_bits_u, i_nonzero_dist_u, 0, DF_DEFAULT );
			}

			if( !i_abs_sum_transform_skipu || min_cost_u < d_single_cost_u )
			{
				x265_data_cu_set_transform_skip_sub_parts_p6(h,
															cu,
															0,
															TEXT_CHROMA_U,
															i_abs_part_idx,
															x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );

				memcpy( coeff_curr_u, best_coeff_u, sizeof (x265_coeff_t) * i_num_samples_chro );
#if X265_ADAPTIVE_QP_SELECTION
				memcpy( arl_coeff_curr_u, best_arl_coeff_u, sizeof (x265_coeff_t) * i_num_samples_chro );
#endif
				for( int32_t i = 0; i < tr_height_c; ++i )
				{
					memcpy( resi_curr_u+i*resi_c_stride, &best_resi_u[i*tr_width_c], sizeof(spixel) * tr_width_c );
				}
			}
			else
			{
				i_dist_u = i_nonzero_dist_u;
				i_abs_sum_u = i_abs_sum_transform_skipu;
				i_best_transform_mode[1] = 1;
			}

			if( i_abs_sum_transform_skip_v )
			{
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_CHROMA_V,
												i_tr_mode );
				x265_enc_entropy_encode_coeff_nxn(h,
												&h->enc_entropy,
												cu,
												coeff_curr_v,
												i_abs_part_idx,
												tr_width_c,
												tr_height_c,
												i_depth,
												TEXT_CHROMA_V );
				i_single_bits_v = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) - i_single_bits_u;

				cur_chroma_qp_offset = h->pps[0].i_chroma_cr_qp_offset + h->slice->i_slice_qp_delta_cr;
				x265_tr_quant_set_qp_for_quant(&h->tr_quant,
												x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
												TEXT_CHROMA,
												h->sps[0].i_qp_bd_offset_c,
												cur_chroma_qp_offset );

				scaling_list_type = 3 + et_table[(int32_t)TEXT_CHROMA_V];
				assert(scaling_list_type < 6);

				x265_tr_quant_inv_transform_nxn(h,
												&h->tr_quant,
												x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
												TEXT_CHROMA,
												X265_REG_DCT,
												resi_curr_v,
												x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
												coeff_curr_v,
												tr_width_c,
												tr_height_c,
												scaling_list_type,
												1 );

				i_nonzero_dist_v = x265_rd_cost_get_sse_s_s(h,
															&h->rd_cost,
															h->cu.pic.i_bit_depth_c,
															x265_simage_get_cr_addr_p3(h, enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer], abs_t_upart_idx_c),
															x265_simage_get_c_stride(enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer]),
															x265_simage_get_cr_addr_p3(h, resi, abs_t_upart_idx_c),
															x265_simage_get_c_stride(resi),
															tr_width_c,
															tr_height_c
#if X265_WEIGHTED_CHROMA_DISTORTION
															,TEXT_CHROMA_V
#endif
															,DF_SSE);
				d_single_cost_v = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_single_bits_v, i_nonzero_dist_v, 0, DF_DEFAULT );
			}

			if( !i_abs_sum_transform_skip_v || min_cost_v < d_single_cost_v )
			{
				x265_data_cu_set_transform_skip_sub_parts_p6(h,
															cu,
															0,
															TEXT_CHROMA_V,
															i_abs_part_idx,
															x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );

				memcpy( coeff_curr_v, best_coeff_v, sizeof(x265_coeff_t) * i_num_samples_chro );
#if X265_ADAPTIVE_QP_SELECTION
				memcpy( arl_coeff_curr_v, best_arl_coeff_v, sizeof(x265_coeff_t) * i_num_samples_chro );
#endif
				for( int32_t i = 0; i < tr_height_c; ++i )
				{
					memcpy( resi_curr_v+i*resi_c_stride, &best_resi_v[i*tr_width_c], sizeof(spixel) * tr_width_c );
				}
			}
			else
			{
				i_dist_v = i_nonzero_dist_v;
				i_abs_sum_v = i_abs_sum_transform_skip_v;
				i_best_transform_mode[2] = 1;
			}

			x265_data_cu_set_cbf_sub_parts_p6(h,
												cu,
												i_abs_sum_u ? i_set_cbf : 0,
												TEXT_CHROMA_U,
												i_abs_part_idx,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );
			x265_data_cu_set_cbf_sub_parts_p6(h,
												cu,
												i_abs_sum_v ? i_set_cbf : 0,
												TEXT_CHROMA_V,
												i_abs_part_idx,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );
		}

		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder, h->pppc_rd_sbac_coder[ i_depth ][ CI_QT_TRAFO_ROOT ] );
		}

		x265_enc_entropy_reset_bits(&h->enc_entropy);

		{
			if( i_log2tr_size > x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx) )
			{
				x265_enc_entropy_encode_transform_sub_div_flag(h,
																&h->enc_entropy,
																0,
																5 - i_log2tr_size );
			}
		}

		{
			if( b_code_chroma )
			{
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_CHROMA_U,
												i_tr_mode );
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_CHROMA_V,
												i_tr_mode );
			}

			x265_enc_entropy_encode_qt_cbf(h,
											&h->enc_entropy,
											cu,
											i_abs_part_idx,
											TEXT_LUMA,
											i_tr_mode );
		}

		x265_enc_entropy_encode_coeff_nxn(h,
											&h->enc_entropy,
											cu,
											coeff_curr_y,
											i_abs_part_idx,
											tr_width,
											tr_height,
											i_depth,
											TEXT_LUMA );

		if( b_code_chroma )
		{
			x265_enc_entropy_encode_coeff_nxn(h,
												&h->enc_entropy,
												cu,
												coeff_curr_u,
												i_abs_part_idx,
												tr_width_c,
												tr_height_c,
												i_depth,
												TEXT_CHROMA_U );
			x265_enc_entropy_encode_coeff_nxn(h,
												&h->enc_entropy,
												cu,
												coeff_curr_v,
												i_abs_part_idx,
												tr_width_c,
												tr_height_c,
												i_depth,
												TEXT_CHROMA_V );
		}

		i_single_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);

		i_single_dist = i_dist_y + i_dist_u + i_dist_v;
		d_single_cost = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_single_bits, i_single_dist, 0, DF_DEFAULT );
	}

	// code sub-blocks
	if( b_check_split )
	{
		if( h->param.b_use_sbac_rd && b_check_full )
		{
			x265_enc_sbac_store(&h->rd_go_on_sbac_coder, h->pppc_rd_sbac_coder[ i_depth ][ CI_QT_TRAFO_TEST ] );
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder, h->pppc_rd_sbac_coder[ i_depth ][ CI_QT_TRAFO_ROOT ] );
		}
		i_subdiv_dist = 0;
		i_subdiv_bits = 0;
		d_subdiv_cost = 0.0;

		i_qpart_num_subdiv = h->cu.pic.i_num_partitions >> ((i_depth + 1 ) << 1);
		for( loop = 0; loop < 4; ++ loop )
		{
			ns_addr = i_abs_part_idx + loop * i_qpart_num_subdiv;
			x265_enc_search_x_estimate_residual_qt(h,
													enc_search,
													cu,
													loop,
													i_abs_part_idx + loop * i_qpart_num_subdiv,
													ns_addr,
													resi,
													i_depth + 1,
													&d_subdiv_cost,
													&i_subdiv_bits,
													&i_subdiv_dist,
													b_check_full ? NULL : p_zero_dist );
		}

		i_y_cbf = 0;
		i_u_cbf = 0;
		i_v_cbf = 0;
		for( loop = 0; loop < 4; ++loop )
		{
			i_y_cbf |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx + loop * i_qpart_num_subdiv, TEXT_LUMA,i_tr_mode + 1 );
			i_u_cbf |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx + loop * i_qpart_num_subdiv, TEXT_CHROMA_U, i_tr_mode + 1 );
			i_v_cbf |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx + loop * i_qpart_num_subdiv, TEXT_CHROMA_V, i_tr_mode + 1 );
		}
		for( loop = 0; loop < 4 * i_qpart_num_subdiv; ++loop )
		{
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_LUMA)[i_abs_part_idx + loop] |= i_y_cbf << i_tr_mode;
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U )[i_abs_part_idx + loop] |= i_u_cbf << i_tr_mode;
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V )[i_abs_part_idx + loop] |= i_v_cbf << i_tr_mode;
		}

		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder, h->pppc_rd_sbac_coder[ i_depth ][ CI_QT_TRAFO_ROOT ] );
		}
		x265_enc_entropy_reset_bits(&h->enc_entropy);

		{
			x265_enc_search_x_encode_residual_qt(h,
												enc_search,
												cu,
												i_abs_part_idx,
												i_depth,
												1,
												TEXT_LUMA );
			x265_enc_search_x_encode_residual_qt(h,
												enc_search,
												cu,
												i_abs_part_idx,
												i_depth,
												0,
												TEXT_LUMA );
			x265_enc_search_x_encode_residual_qt(h,
												enc_search,
												cu,
												i_abs_part_idx,
												i_depth,
												0,
												TEXT_CHROMA_U );
			x265_enc_search_x_encode_residual_qt(h,
												enc_search,
												cu,
												i_abs_part_idx,
												i_depth,
												0,
												TEXT_CHROMA_V );
		}

		i_subdiv_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
		d_subdiv_cost  = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_subdiv_bits, i_subdiv_dist, 0, DF_DEFAULT );

		if( i_y_cbf || i_u_cbf || i_v_cbf || !b_check_full )
		{
			if( d_subdiv_cost < d_single_cost )
			{
				*p_cost += d_subdiv_cost;
				*p_bits += i_subdiv_bits;
				*p_dist += i_subdiv_dist;
				return;
			}
		}
		x265_data_cu_set_transform_skip_sub_parts_p6(h,
													cu,
													i_best_transform_mode[0],
													TEXT_LUMA,
													i_abs_part_idx,
													i_depth );
		if(b_code_chroma)
		{
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														i_best_transform_mode[1],
														TEXT_CHROMA_U,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														i_best_transform_mode[2],
														TEXT_CHROMA_V,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );
		}
		assert( b_check_full );
		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder, h->pppc_rd_sbac_coder[ i_depth ][ CI_QT_TRAFO_TEST ] );
		}
	}
	*p_cost += d_single_cost;
	*p_bits += i_single_bits;
	*p_dist += i_single_dist;

	x265_base_data_cu_set_tr_idx_sub_parts(h,
											(x265_base_data_cu_t*)cu,
											i_tr_mode,
											i_abs_part_idx,
											i_depth );

	x265_data_cu_set_cbf_sub_parts_p6(h,
										cu,
										i_abs_sum_y ? i_set_cbf : 0,
										TEXT_LUMA,
										i_abs_part_idx,
										i_depth );
	if( b_code_chroma )
	{
		x265_data_cu_set_cbf_sub_parts_p6(h,
											cu,
											i_abs_sum_u ? i_set_cbf : 0,
											TEXT_CHROMA_U,
											i_abs_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );
		x265_data_cu_set_cbf_sub_parts_p6(h,
											cu,
											i_abs_sum_v ? i_set_cbf : 0,
											TEXT_CHROMA_V,
											i_abs_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)+i_tr_mode_c );
	}
}

void x265_enc_search_x_encode_residual_qt(x265_t* h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										const uint32_t i_depth,
										int32_t b_subdiv_and_cbf,
										enum text_type_e e_type )
{
	uint32_t loop = 0;
	uint32_t i_curr_tr_mode = 0;
	uint32_t i_tr_mode = 0;
	int32_t b_subdiv = 0;
	uint32_t i_log2tr_size = 0;
	int32_t b_first_cbf_of_cu = 0;
	uint32_t i_num_coeff_per_abs_part_idx_increment = 0;
	uint32_t i_qt_temp_access_layer = 0;
	x265_coeff_t *coeff_curr_y = NULL;
	x265_coeff_t *coeff_curr_u = NULL;
	x265_coeff_t *coeff_curr_v = NULL;
	int32_t b_code_chroma = 0;
	uint32_t i_tr_mode_c = 0;
	uint32_t i_log2tr_size_c = 0;
	uint32_t  i_qpdiv = 0;
	int32_t tr_width = 0;
	int32_t tr_height = 0;
	uint32_t i_qpart_num_subdiv = 0;




	assert(x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0)
			== x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) );
	i_curr_tr_mode = i_depth - x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 );
	i_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );
	b_subdiv = i_curr_tr_mode != i_tr_mode;
	i_log2tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> i_depth]+2;

	{
		if( b_subdiv_and_cbf
			&& i_log2tr_size <= h->sps[0].i_quadtree_tu_log2_max_size
			&& i_log2tr_size > x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx))
		{
			x265_enc_entropy_encode_transform_sub_div_flag(h,
															&h->enc_entropy,
															b_subdiv,
															5 - i_log2tr_size );
		}
	}

	{
		assert( x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) != MODE_INTRA );
		if( b_subdiv_and_cbf )
		{
			b_first_cbf_of_cu = i_curr_tr_mode == 0;
			if( b_first_cbf_of_cu || i_log2tr_size > 2 )
			{
				if( b_first_cbf_of_cu
						|| x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_curr_tr_mode - 1 ) )
				{
					x265_enc_entropy_encode_qt_cbf(h,
													&h->enc_entropy,
													cu,
													i_abs_part_idx,
													TEXT_CHROMA_U,
													i_curr_tr_mode );
				}
				if( b_first_cbf_of_cu
						|| x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_curr_tr_mode - 1 ) )
				{
					x265_enc_entropy_encode_qt_cbf(h,
													&h->enc_entropy,
													cu,
													i_abs_part_idx,
													TEXT_CHROMA_V,
													i_curr_tr_mode );
				}
			}
			else if( i_log2tr_size == 2 )
			{
				assert( x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_curr_tr_mode )
						== x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_curr_tr_mode - 1 ) );
				assert( x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_curr_tr_mode )
						== x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_curr_tr_mode - 1 ) );
			}
		}
	}

	if( !b_subdiv )
	{
		i_num_coeff_per_abs_part_idx_increment = h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height >> ( h->sps[0].i_max_cu_depth << 1 );
		//assert( 16 == i_num_coeff_per_abs_part_idx_increment ); // check
		i_qt_temp_access_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2tr_size;
		coeff_curr_y = enc_search->ppc_qt_temp_coeff_y [i_qt_temp_access_layer] +  i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx;
		coeff_curr_u = enc_search->ppc_qt_temp_coeff_cb[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
		coeff_curr_v = enc_search->ppc_qt_temp_coeff_cr[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);

		b_code_chroma   = 1;
		i_tr_mode_c= i_tr_mode;
		i_log2tr_size_c = i_log2tr_size-1;
		if( i_log2tr_size == 2 )
		{
			i_log2tr_size_c++;
			i_tr_mode_c    --;
			i_qpdiv = h->cu.pic.i_num_partitions >> ( ( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 ) + i_tr_mode_c ) << 1 );
			b_code_chroma   = ( ( i_abs_part_idx % i_qpdiv ) == 0 );
		}

		if( b_subdiv_and_cbf )
		{
			{
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_LUMA,
												i_tr_mode );
			}
		}
		else
		{
			if( e_type == TEXT_LUMA
				&& x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_LUMA,i_tr_mode ) )
			{
				tr_width  = 1 << i_log2tr_size;
				tr_height = 1 << i_log2tr_size;
				x265_enc_entropy_encode_coeff_nxn(h,
												&h->enc_entropy,
												cu,
												coeff_curr_y,
												i_abs_part_idx,
												tr_width,
												tr_height,
												i_depth,
												TEXT_LUMA );
			}
			if( b_code_chroma )
			{
				tr_width  = 1 << i_log2tr_size_c;
				tr_height = 1 << i_log2tr_size_c;
				if( e_type == TEXT_CHROMA_U
						&& x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_tr_mode ) )
				{
					x265_enc_entropy_encode_coeff_nxn(h,
													&h->enc_entropy,
													cu,
													coeff_curr_u,
													i_abs_part_idx,
													tr_width,
													tr_height,
													i_depth,
													TEXT_CHROMA_U );
				}
				if( e_type == TEXT_CHROMA_V
						&& x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_tr_mode ) )
				{
					x265_enc_entropy_encode_coeff_nxn(h,
													&h->enc_entropy,
													cu,
													coeff_curr_v,
													i_abs_part_idx,
													tr_width,
													tr_height,
													i_depth,
													TEXT_CHROMA_V );
				}
			}
		}
	}
	else
	{
		if( b_subdiv_and_cbf
				|| x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, e_type, i_curr_tr_mode ) )
		{
			i_qpart_num_subdiv = h->cu.pic.i_num_partitions >> ((i_depth + 1 ) << 1);
			for( loop = 0; loop < 4; ++ loop )
			{
				x265_enc_search_x_encode_residual_qt(h,
													enc_search,
													cu,
													i_abs_part_idx + loop * i_qpart_num_subdiv,
													i_depth + 1,
													b_subdiv_and_cbf,
													e_type );
			}
		}
	}

}


void x265_enc_search_x_set_residual_qt_data(x265_t* h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_quadrant,
											uint32_t i_abs_part_idx,
											uint32_t abs_t_upart_idx,
											x265_simage_t *resi,
											uint32_t i_depth,
											int32_t b_spatial )
{
	uint32_t loop = 0;
	uint32_t i_curr_tr_mode = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_log2tr_size = 0;
	uint32_t i_qt_temp_access_layer = 0;
	int32_t b_code_chroma = 0;
	uint32_t i_tr_mode_c = 0;
	uint32_t i_log2tr_size_c = 0;
	uint32_t  i_qpdiv = 0;
	int32_t tr_width = 0;
	int32_t tr_height = 0;
	uint32_t i_num_coeff_per_abs_part_idx_increment = 0;
	uint32_t i_num_coeff_y = 0;
	x265_coeff_t *coeff_src_y = NULL;
	x265_coeff_t *coeff_dst_y = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_src_y = NULL;
	int32_t *arl_coeff_dst_y = NULL;
#endif
	uint32_t i_num_coeff_c = 0;
	x265_coeff_t *coeff_srcu = NULL;
	x265_coeff_t *coeff_src_v = NULL;
	x265_coeff_t *coeff_dst_u = NULL;
	x265_coeff_t *coeff_dst_v = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_srcu = NULL;
	int32_t *arl_coeff_src_v = NULL;
	int32_t *arl_coeff_dst_u = NULL;
	int32_t *arl_coeff_dst_v = NULL;
#endif
	uint32_t i_qpart_num_subdiv = 0;
	uint32_t ns_addr = 0;

	assert( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 )
			== x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) );
	i_curr_tr_mode = i_depth - x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 );
	i_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );

	if( i_curr_tr_mode == i_tr_mode )
	{
		i_log2tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> i_depth]+2;
		i_qt_temp_access_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2tr_size;

		b_code_chroma   = 1;
		i_tr_mode_c= i_tr_mode;
		i_log2tr_size_c = i_log2tr_size-1;
		if( i_log2tr_size == 2 )
		{
			i_log2tr_size_c++;
			i_tr_mode_c    --;
			i_qpdiv = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_mode_c) << 1);
			b_code_chroma   = ( ( i_abs_part_idx % i_qpdiv ) == 0 );
		}

		if( b_spatial )
		{
			tr_width  = 1 << i_log2tr_size;
			tr_height = 1 << i_log2tr_size;
			x265_simage_copy_part_to_part_luma_s (h,
												enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer],
												resi,
												abs_t_upart_idx,
												tr_width,
												tr_height );

			if( b_code_chroma )
			{
				{
					x265_simage_copy_part_to_part_chroma_s(h,
														enc_search->ppc_qt_temp_simage[i_qt_temp_access_layer],
														resi,
														i_abs_part_idx,
														1 << i_log2tr_size_c,
														1 << i_log2tr_size_c );
				}
			}
		}
		else
		{
			i_num_coeff_per_abs_part_idx_increment = h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height >> (h->sps[0].i_max_cu_depth << 1);
			i_num_coeff_y = ( 1 << ( i_log2tr_size << 1 ) );
			coeff_src_y = enc_search->ppc_qt_temp_coeff_y [i_qt_temp_access_layer] +  i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx;
			coeff_dst_y = x265_data_cu_get_coeff_y(cu) + i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx;
			memcpy( coeff_dst_y, coeff_src_y, sizeof( x265_coeff_t ) * i_num_coeff_y );
#if X265_ADAPTIVE_QP_SELECTION
			arl_coeff_src_y = enc_search->ppc_qt_temp_arl_coeff_y [i_qt_temp_access_layer] +  i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx;
			arl_coeff_dst_y = x265_data_cu_get_arl_coeff_y(cu) + i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx;
			memcpy( arl_coeff_dst_y, arl_coeff_src_y, sizeof( int32_t ) * i_num_coeff_y );
#endif
			if( b_code_chroma )
			{
				i_num_coeff_c = ( 1 << ( i_log2tr_size_c << 1 ) );
				coeff_srcu = enc_search->ppc_qt_temp_coeff_cb[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
				coeff_src_v = enc_search->ppc_qt_temp_coeff_cr[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
				coeff_dst_u = x265_data_cu_get_coeff_cb(cu) + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
				coeff_dst_v = x265_data_cu_get_coeff_cr(cu) + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
				memcpy( coeff_dst_u, coeff_srcu, sizeof( x265_coeff_t ) * i_num_coeff_c );
				memcpy( coeff_dst_v, coeff_src_v, sizeof( x265_coeff_t ) * i_num_coeff_c );
#if X265_ADAPTIVE_QP_SELECTION
				arl_coeff_srcu = enc_search->ppc_qt_temp_arl_coeff_cb[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
				arl_coeff_src_v = enc_search->ppc_qt_temp_arl_coeff_cr[i_qt_temp_access_layer] + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
				arl_coeff_dst_u = x265_data_cu_get_arl_coeff_cb(cu) + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
				arl_coeff_dst_v = x265_data_cu_get_arl_coeff_cr(cu) + (i_num_coeff_per_abs_part_idx_increment * i_abs_part_idx>>2);
				memcpy( arl_coeff_dst_u, arl_coeff_srcu, sizeof( int32_t ) * i_num_coeff_c );
				memcpy( arl_coeff_dst_v, arl_coeff_src_v, sizeof( int32_t ) * i_num_coeff_c );
#endif
			}
		}
	}
	else
	{
		i_qpart_num_subdiv = h->cu.pic.i_num_partitions >> ((i_depth + 1 ) << 1);
		for( loop = 0; loop < 4; ++ loop )
		{
			ns_addr = i_abs_part_idx + loop * i_qpart_num_subdiv;
			x265_enc_search_x_set_residual_qt_data(h,
													enc_search,
													cu,
													loop,
													i_abs_part_idx + loop * i_qpart_num_subdiv,
													ns_addr,
													resi,
													i_depth + 1,
													b_spatial );
		}
	}

}


/** add inter-prediction syntax elements for a cu block
 * \param cu
 * \param i_qp
 * \param i_tr_mode
 * \param ri_bits
 * \param rimage_rec
 * \param image_pred
 * \param rimage_resi
 * \returns void
 */


void x265_enc_search_x_add_symbol_bits_inter(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_qp,
											uint32_t i_tr_mode,
											uint32_t *p_bits,
											x265_image_t **pp_image_rec,
											x265_image_t *p_image_pred,
											x265_image_t **pp_image_resi )
{
	int32_t b_dummy = 0;

	if(x265_data_cu_get_merge_flag_p2(cu, 0)
			&& x265_data_cu_get_partition_size_p2(cu, 0 ) == SIZE_2Nx2N
			&& (!x265_base_data_cu_get_qt_root_cbf((x265_base_data_cu_t*)cu, 0)))
	{
		x265_base_data_cu_set_skip_flag_sub_parts(h,
												(x265_base_data_cu_t*)cu,
												1,
												0,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));

		x265_enc_entropy_reset_bits(&h->enc_entropy);
		if(h->pps[0].b_transquant_bypass_enable_flag)
		{
			x265_enc_entropy_encode_cu_transquant_bypass_flag(&h->enc_entropy, cu, 0, 1);
		}
		x265_enc_entropy_encode_skip_flag(h, &h->enc_entropy, cu, 0, 1);
		x265_enc_entropy_encode_merge_index(h, &h->enc_entropy, cu, 0, 1);
		*p_bits += x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
	}
	else
	{
		x265_enc_entropy_reset_bits(&h->enc_entropy);
		if(h->pps[0].b_transquant_bypass_enable_flag)
		{
			x265_enc_entropy_encode_cu_transquant_bypass_flag(&h->enc_entropy, cu, 0, 1);
		}
		x265_enc_entropy_encode_skip_flag(h, &h->enc_entropy, cu, 0, 1 );
		x265_enc_entropy_encode_pred_mode(h, &h->enc_entropy, cu, 0, 1 );
		x265_enc_entropy_encode_part_size(h,
										&h->enc_entropy,
										cu,
										0,
										x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0), 1 );
		x265_enc_entropy_encode_pred_info(h, &h->enc_entropy, cu, 0, 1 );
		x265_enc_entropy_encode_coeff(h,
										&h->enc_entropy,
										cu,
										0,
										x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0),
										x265_data_cu_get_width_p2(cu, 0),
										x265_data_cu_get_height_p2(cu, 0),
										&b_dummy );

		*p_bits += x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
	}
}

/**
 * \brief generate half-sample interpolated block
 *
 * \param pattern reference picture roi
 * \param bi_pred    flag indicating whether block is for biprediction
 */

void x265_enc_search_x_ext_dif_up_sampling_h(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_pattern_t* pattern,
											int32_t b_bi_pred )
{
	int32_t width = 0;
	int32_t height= 0;
	int32_t src_stride = 0;
	int32_t int_stride = 0;
	int32_t dst_stride = 0;
	spixel *int_ptr = NULL;
	pixel *dst_ptr = NULL;
	int32_t filter_size = 0;
	int32_t half_filter_size = 0;
	pixel *src_ptr = NULL;

	width = x265_pattern_get_roi_width(pattern);
	height = x265_pattern_get_roi_height(pattern);
	src_stride  = x265_pattern_get_pattern_l_stride(pattern);

	int_stride = x265_simage_get_stride(&enc_search->prediction.filtered_block_tmp[0]);
	dst_stride = x265_image_get_stride(&enc_search->prediction.filtered_block[0][0]);
	filter_size = X265_NTAPS_LUMA;
	half_filter_size = (filter_size>>1);
	src_ptr = x265_pattern_get_roi_y(pattern) - half_filter_size*src_stride - 1;


	x265_interpolation_filter_filter_hor_luma_p_s(h,
											&enc_search->prediction.interpolation_filter,
											src_ptr,
											src_stride,
											x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[0]),
											int_stride,
											width + 1,
											height + filter_size,
											0,
											0);
	x265_interpolation_filter_filter_hor_luma_p_s(h,
											&enc_search->prediction.interpolation_filter,
											src_ptr,
											src_stride,
											x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[2]),
											int_stride,
											width + 1,
											height + filter_size,
											2,
											0);

	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[0]) + half_filter_size * int_stride + 1;
	dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[0][0]);
	x265_interpolation_filter_filter_ver_luma_s_p(h,
											&enc_search->prediction.interpolation_filter,
											int_ptr,
											int_stride,
											dst_ptr,
											dst_stride,
											width + 0,
											height + 0,
											0,
											0,
											1);

	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[0]) + (half_filter_size-1) * int_stride + 1;
	dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[2][0]);
	x265_interpolation_filter_filter_ver_luma_s_p(h,
											&enc_search->prediction.interpolation_filter,
											int_ptr,
											int_stride,
											dst_ptr,
											dst_stride,
											width + 0,
											height + 1,
											2,
											0,
											1);

	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[2]) + half_filter_size * int_stride;
	dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[0][2]);
	x265_interpolation_filter_filter_ver_luma_s_p(h,
											&enc_search->prediction.interpolation_filter,
											int_ptr,
											int_stride,
											dst_ptr,
											dst_stride,
											width + 1,
											height + 0,
											0,
											0,
											1);

	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[2]) + (half_filter_size-1) * int_stride;
	dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[2][2]);
	x265_interpolation_filter_filter_ver_luma_s_p(h,
											&enc_search->prediction.interpolation_filter,
											int_ptr,
											int_stride,
											dst_ptr,
											dst_stride,
											width + 1,
											height + 1,
											2,
											0,
											1);
}




/**
 * \brief generate quarter-sample interpolated blocks
 *
 * \param pattern    reference picture roi
 * \param half_pixel_ref half-pel mv
 * \param bi_predflag indicating whether block is for biprediction
 */
void x265_enc_search_x_ext_dif_up_sampling_q(x265_t *h,
											x265_enc_search_t* enc_search,
											x265_pattern_t* pattern,
											x265_mv_t *half_pixel_ref,
											int32_t b_bi_pred )
{
	int32_t width = 0;
	int32_t height = 0;
	int32_t src_stride = 0;

	pixel *src_ptr;
	int32_t int_stride = 0;
	int32_t dst_stride = 0;
	spixel *int_ptr = NULL;
	pixel *dst_ptr = NULL;
	int32_t filter_size = 0;
	int32_t half_filter_size = 0;
	int32_t ext_height = 0;


	width = x265_pattern_get_roi_width(pattern);
	height = x265_pattern_get_roi_height(pattern);
	src_stride  = x265_pattern_get_pattern_l_stride(pattern);
	int_stride = x265_simage_get_stride(&enc_search->prediction.filtered_block_tmp[0]);
	dst_stride = x265_image_get_stride(&enc_search->prediction.filtered_block[0][0]);
	filter_size = X265_NTAPS_LUMA;
	half_filter_size = (filter_size >> 1);
	ext_height = (half_pixel_ref->i_ver == 0)
					? height + filter_size : height + filter_size-1;

	// horizontal filter 1/4
	src_ptr = x265_pattern_get_roi_y(pattern) - half_filter_size * src_stride - 1;
	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[1]);
	if (half_pixel_ref->i_ver > 0)
	{
		src_ptr += src_stride;
	}
	if (half_pixel_ref->i_hor >= 0)
	{
		src_ptr += 1;
	}
	x265_interpolation_filter_filter_hor_luma_p_s(h,
											&enc_search->prediction.interpolation_filter,
											src_ptr,
											src_stride,
											int_ptr,
											int_stride,
											width,
											ext_height,
											1,
											0);

	// horizontal filter 3/4
	src_ptr = x265_pattern_get_roi_y(pattern) - half_filter_size*src_stride - 1;
	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[3]);
	if (half_pixel_ref->i_ver > 0)
	{
		src_ptr += src_stride;
	}
	if (half_pixel_ref->i_hor > 0)
	{
		src_ptr += 1;
	}
	x265_interpolation_filter_filter_hor_luma_p_s(h,
											&enc_search->prediction.interpolation_filter,
											src_ptr,
											src_stride,
											int_ptr,
											int_stride,
											width,
											ext_height,
											3,
											0);

	// generate @ 1,1
	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[1]) + (half_filter_size-1) * int_stride;
	dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[1][1]);
	if (half_pixel_ref->i_ver == 0)
	{
		int_ptr += int_stride;
	}
	x265_interpolation_filter_filter_ver_luma_s_p(h,
											&enc_search->prediction.interpolation_filter,
											int_ptr,
											int_stride,
											dst_ptr,
											dst_stride,
											width,
											height,
											1,
											0,
											1);

	// generate @ 3,1
	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[1]) + (half_filter_size-1) * int_stride;
	dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[3][1]);
	x265_interpolation_filter_filter_ver_luma_s_p(h,
											&enc_search->prediction.interpolation_filter,
											int_ptr,
											int_stride,
											dst_ptr,
											dst_stride,
											width,
											height,
											3,
											0,
											1);

	if (half_pixel_ref->i_ver != 0)
	{
		// generate @ 2,1
		int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[1]) + (half_filter_size-1) * int_stride;
		dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[2][1]);
		if (half_pixel_ref->i_ver == 0)
		{
			int_ptr += int_stride;
		}
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&enc_search->prediction.interpolation_filter,
												int_ptr,
												int_stride,
												dst_ptr,
												dst_stride,
												width,
												height,
												2,
												0,
												1);

		// generate @ 2,3
		int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[3]) + (half_filter_size-1) * int_stride;
		dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[2][3]);
		if (half_pixel_ref->i_ver == 0)
		{
			int_ptr += int_stride;
		}
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&enc_search->prediction.interpolation_filter,
												int_ptr,
												int_stride,
												dst_ptr,
												dst_stride,
												width,
												height,
												2,
												0,
												1);
	}
	else
	{
		// generate @ 0,1
		int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[1]) + half_filter_size * int_stride;
		dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[0][1]);
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&enc_search->prediction.interpolation_filter,
												int_ptr,
												int_stride,
												dst_ptr,
												dst_stride,
												width,
												height,
												0,
												0,
												1);

		// generate @ 0,3
		int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[3]) + half_filter_size * int_stride;
		dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[0][3]);
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&enc_search->prediction.interpolation_filter,
												int_ptr,
												int_stride,
												dst_ptr,
												dst_stride,
												width,
												height,
												0,
												0,
												1);
	}

	if (half_pixel_ref->i_hor != 0)
	{
		// generate @ 1,2
		int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[2]) + (half_filter_size-1) * int_stride;
		dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[1][2]);
		if (half_pixel_ref->i_hor > 0)
		{
			int_ptr += 1;
		}
		if (half_pixel_ref->i_ver >= 0)
		{
			int_ptr += int_stride;
		}
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&enc_search->prediction.interpolation_filter,
												int_ptr,
												int_stride,
												dst_ptr,
												dst_stride,
												width,
												height,
												1,
												0,
												1);

		// generate @ 3,2
		int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[2]) + (half_filter_size-1) * int_stride;
		dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[3][2]);
		if (half_pixel_ref->i_hor > 0)
		{
			int_ptr += 1;
		}
		if (half_pixel_ref->i_ver > 0)
		{
			int_ptr += int_stride;
		}
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&enc_search->prediction.interpolation_filter,
												int_ptr,
												int_stride,
												dst_ptr,
												dst_stride,
												width,
												height,
												3,
												0,
												1);
	}
	else
	{
		// generate @ 1,0
		int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[0]) + (half_filter_size-1) * int_stride + 1;
		dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[1][0]);
		if (half_pixel_ref->i_ver >= 0)
		{
			int_ptr += int_stride;
		}
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&enc_search->prediction.interpolation_filter,
												int_ptr,
												int_stride,
												dst_ptr,
												dst_stride,
												width,
												height,
												1,
												0,
												1);

		// generate @ 3,0
		int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[0]) + (half_filter_size-1) * int_stride + 1;
		dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[3][0]);
		if (half_pixel_ref->i_ver > 0)
		{
			int_ptr += int_stride;
		}
		x265_interpolation_filter_filter_ver_luma_s_p(h,
												&enc_search->prediction.interpolation_filter,
												int_ptr,
												int_stride,
												dst_ptr,
												dst_stride,
												width,
												height,
												3,
												0,
												1);
	}

	// generate @ 1,3
	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[3]) + (half_filter_size-1) * int_stride;
	dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[1][3]);
	if (half_pixel_ref->i_ver == 0)
	{
		int_ptr += int_stride;
	}
	x265_interpolation_filter_filter_ver_luma_s_p(h,
											&enc_search->prediction.interpolation_filter,
											int_ptr,
											int_stride,
											dst_ptr,
											dst_stride,
											width,
											height,
											1,
											0,
											1);

	// generate @ 3,3
	int_ptr = x265_simage_get_luma_addr_p2(h, &enc_search->prediction.filtered_block_tmp[3]) + (half_filter_size-1) * int_stride;
	dst_ptr = x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[3][3]);
	x265_interpolation_filter_filter_ver_luma_s_p(h,
											&enc_search->prediction.interpolation_filter,
											int_ptr,
											int_stride,
											dst_ptr,
											dst_stride,
											width,
											height,
											3,
											0,
											1);
}

