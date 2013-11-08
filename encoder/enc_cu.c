

#include "common/common.h"


void x265_enc_cu_delete ( x265_enc_cu_t *enc_cu )
{
	x265_free ( enc_cu ) ;
}

x265_enc_cu_t *x265_enc_cu_new ()
{
	x265_enc_cu_t *enc_cu = NULL ;

	CHECKED_MALLOCZERO( enc_cu, sizeof(x265_enc_cu_t) );

	return enc_cu ;
fail:
	x265_enc_cu_delete ( (void*) enc_cu ) ;
	return NULL ;
}

int x265_enc_cu_init ( x265_enc_cu_t *enc_cu )
{

    return 0 ;
}

void x265_enc_cu_deinit ( x265_enc_cu_t *enc_cu )
{

}


int x265_enc_cu_create ( x265_t *h, x265_enc_cu_t *enc_cu )
{
	uint8_t loop = 0 ;
	uint32_t i_num_partitions = 0 ;
	uint32_t i_width = 0 ;
	uint32_t i_height = 0 ;

	enc_cu->i_total_depth = h->cu.pic.i_total_depth + 1 ;

	CHECKED_MALLOCZERO( enc_cu->best_cu,
						(enc_cu->i_total_depth-1)*sizeof(x265_data_cu_t*) );
	CHECKED_MALLOCZERO( enc_cu->temp_cu,
						(enc_cu->i_total_depth-1)*sizeof(x265_data_cu_t*) );

	CHECKED_MALLOCZERO( enc_cu->pred_image_best,
						(enc_cu->i_total_depth-1)*sizeof(x265_image_t*) );
	CHECKED_MALLOCZERO( enc_cu->resi_image_best,
						(enc_cu->i_total_depth-1)*sizeof(x265_image_t*) );
	CHECKED_MALLOCZERO( enc_cu->reco_image_best,
						(enc_cu->i_total_depth-1)*sizeof(x265_image_t*) );
	CHECKED_MALLOCZERO( enc_cu->pred_image_temp,
						(enc_cu->i_total_depth-1)*sizeof(x265_image_t*) );
	CHECKED_MALLOCZERO( enc_cu->resi_image_temp,
						(enc_cu->i_total_depth-1)*sizeof(x265_image_t*) );
	CHECKED_MALLOCZERO( enc_cu->reco_image_temp,
						(enc_cu->i_total_depth-1)*sizeof(x265_image_t*) );
	CHECKED_MALLOCZERO( enc_cu->orig_image,
						(enc_cu->i_total_depth-1)*sizeof(x265_image_t*) );

	for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
	{
		i_num_partitions = 1 << ((enc_cu->i_total_depth - loop - 1) << 1 ) ;
		i_width  = h->cu.pic.i_max_cu_width >> loop ;
		i_height = h->cu.pic.i_max_cu_height >> loop ;

		enc_cu->best_cu[loop] = x265_data_cu_new () ;
		if ( NULL == enc_cu->best_cu[loop] )
		{
			goto fail ;
		}
		if ( x265_data_cu_create ( enc_cu->best_cu[loop],
		                           i_num_partitions,
		                           i_width, i_height,
		                           h->cu.pic.i_max_cu_width >> (enc_cu->i_total_depth - 1)
#if X265_ADAPTIVE_QP_SELECTION
		                           , 0
#endif
								   ) )
		{
			goto fail ;
		}
		enc_cu->temp_cu[loop] = x265_data_cu_new () ;
		if ( NULL == enc_cu->temp_cu[loop] )
		{
			goto fail ;
		}
		if ( x265_data_cu_create ( enc_cu->temp_cu[loop],
		                           i_num_partitions,
		                           i_width, i_height,
		                           h->cu.pic.i_max_cu_width >> (enc_cu->i_total_depth - 1)
#if X265_ADAPTIVE_QP_SELECTION
		                           , 0
#endif
								   ) )
		{
			goto fail ;
		}

		enc_cu->pred_image_best[loop] = x265_image_new () ;
		if ( NULL == enc_cu->pred_image_best[loop] )
		{
			goto fail ;
		}
		if ( x265_image_create ( enc_cu->pred_image_best[loop], i_width, i_height ) )
		{
			goto fail ;
		}

		enc_cu->resi_image_best[loop] = x265_simage_new () ;
		if ( NULL == enc_cu->resi_image_best[loop] )
		{
			goto fail ;
		}
		if ( x265_simage_create ( enc_cu->resi_image_best[loop], i_width, i_height ) )
		{
			goto fail ;
		}

		enc_cu->reco_image_best[loop] = x265_image_new () ;
		if ( NULL == enc_cu->reco_image_best[loop] )
		{
			goto fail ;
		}
		if ( x265_image_create ( enc_cu->reco_image_best[loop], i_width, i_height ) )
		{
			goto fail ;
		}

		enc_cu->pred_image_temp[loop] = x265_image_new () ;
		if ( NULL == enc_cu->pred_image_temp[loop] )
		{
			goto fail ;
		}
		if ( x265_image_create ( enc_cu->pred_image_temp[loop], i_width, i_height ) )
		{
			goto fail ;
		}

		enc_cu->resi_image_temp[loop] = x265_simage_new () ;
		if ( NULL == enc_cu->resi_image_temp[loop] )
		{
			goto fail ;
		}
		if ( x265_simage_create ( enc_cu->resi_image_temp[loop], i_width, i_height ) )
		{
			goto fail ;
		}

		enc_cu->reco_image_temp[loop] = x265_image_new () ;
		if ( NULL == enc_cu->reco_image_temp[loop] )
		{
			goto fail ;
		}
		if ( x265_image_create ( enc_cu->reco_image_temp[loop], i_width, i_height ) )
		{
			goto fail ;
		}

		enc_cu->orig_image[loop] = x265_image_new () ;
		if ( NULL == enc_cu->orig_image[loop] )
		{
			goto fail ;
		}
		if ( x265_image_create ( enc_cu->orig_image[loop], i_width, i_height ) )
		{
			goto fail ;
		}
	}

	enc_cu->b_encode_dqp = 0 ;

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	enc_cu->i_lcu_prediction_sad = 0;
	enc_cu->i_add_sad_depth = 0 ;
	enc_cu->i_temporal_sad = 0 ;
#endif


	return 0 ;

fail:
	x265_enc_cu_destroy ( h, enc_cu ) ;
	return -1 ;
}

void x265_enc_cu_destroy ( x265_t *h, x265_enc_cu_t *enc_cu )
{
	uint8_t loop = 0 ;

	if ( enc_cu->best_cu )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->best_cu[loop] )
			{
				x265_data_cu_destroy ( enc_cu->best_cu[loop] ) ;
				x265_data_cu_delete ( enc_cu->best_cu[loop] ) ;
				enc_cu->best_cu[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->best_cu ) ;
		enc_cu->best_cu = NULL ;
	}

	if ( enc_cu->temp_cu )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->temp_cu[loop] )
			{
				x265_data_cu_destroy ( enc_cu->temp_cu[loop] ) ;
				x265_data_cu_delete ( enc_cu->temp_cu[loop] ) ;
				enc_cu->temp_cu[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->temp_cu ) ;
		enc_cu->temp_cu = NULL ;
	}

	if ( enc_cu->pred_image_best )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->pred_image_best[loop] )
			{
				x265_image_destroy ( enc_cu->pred_image_best[loop] ) ;
				x265_image_delete ( enc_cu->pred_image_best[loop] ) ;
				enc_cu->pred_image_best[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->pred_image_best ) ;
		enc_cu->pred_image_best = NULL ;
	}


	if ( enc_cu->resi_image_best )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->resi_image_best[loop] )
			{
				x265_simage_destroy ( enc_cu->resi_image_best[loop] ) ;
				x265_simage_delete ( enc_cu->resi_image_best[loop] ) ;
				enc_cu->resi_image_best[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->resi_image_best ) ;
		enc_cu->resi_image_best = NULL ;
	}

	if ( enc_cu->reco_image_best )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->reco_image_best[loop] )
			{
				x265_image_destroy ( enc_cu->reco_image_best[loop] ) ;
				x265_image_delete ( enc_cu->reco_image_best[loop] ) ;
				enc_cu->reco_image_best[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->reco_image_best ) ;
		enc_cu->reco_image_best = NULL ;
	}

	if ( enc_cu->pred_image_temp )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->pred_image_temp[loop] )
			{
				x265_image_destroy ( enc_cu->pred_image_temp[loop] ) ;
				x265_image_delete ( enc_cu->pred_image_temp[loop] ) ;
				enc_cu->pred_image_temp[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->pred_image_temp ) ;
		enc_cu->pred_image_temp = NULL ;
	}

	if ( enc_cu->resi_image_temp )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->resi_image_temp[loop] )
			{
				x265_simage_destroy ( enc_cu->resi_image_temp[loop] ) ;
				x265_simage_delete ( enc_cu->resi_image_temp[loop] ) ;
				enc_cu->resi_image_temp[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->resi_image_temp ) ;
		enc_cu->resi_image_temp = NULL ;
	}

	if ( enc_cu->reco_image_temp )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->reco_image_temp[loop] )
			{
				x265_image_destroy ( enc_cu->reco_image_temp[loop] ) ;
				x265_image_delete ( enc_cu->reco_image_temp[loop] ) ;
				enc_cu->reco_image_temp[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->reco_image_temp ) ;
		enc_cu->reco_image_temp = NULL ;
	}

	if ( enc_cu->orig_image )
	{
		for( loop = 0 ; loop < enc_cu->i_total_depth - 1 ; ++ loop )
		{
			if ( enc_cu->orig_image[loop] )
			{
				x265_image_destroy ( enc_cu->orig_image[loop] ) ;
				x265_image_delete ( enc_cu->orig_image[loop] ) ;
				enc_cu->orig_image[loop] = NULL ;
			}
		}
		x265_free ( enc_cu->orig_image ) ;
		enc_cu->orig_image = NULL ;
	}


}

int x265_enc_cu_initialize ( x265_t *h, x265_enc_cu_t *enc_cu )
{

    return 0 ;
}

void x265_enc_cu_deinitialize ( x265_t *h, x265_enc_cu_t *enc_cu )
{

}


void x265_enc_cu_compress_cu ( x265_t *h, x265_enc_cu_t *enc_cu, uint32_t i_cu_addr )
{
	x265_data_cu_t** ppc_best_cu = NULL ;
	x265_data_cu_t** ppc_temp_cu = NULL ;

	// initialize CU data
	x265_data_cu_init_cu ( h, enc_cu->best_cu[0], h->data_cu->base_data_cu.i_cu_addr ) ;
	x265_data_cu_init_cu ( h, enc_cu->temp_cu[0], h->data_cu->base_data_cu.i_cu_addr ) ;

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	enc_cu->i_add_sad_depth = 0;
	enc_cu->i_lcu_prediction_sad = 0 ;
	enc_cu->i_temporal_sad = 0 ;
#endif

	ppc_best_cu = &enc_cu->best_cu[0] ;
	ppc_temp_cu = &enc_cu->temp_cu[0] ;
	// analysis of CU
	x265_enc_cu_x_compress_cu ( h,
								enc_cu,
								ppc_best_cu,
								ppc_temp_cu,
								0,
								SIZE_NONE ) ;
#if X265_ADAPTIVE_QP_SELECTION
	if( h->param.b_use_adapt_qp_select )
	{
		if ( I_SLICE != h->slice->i_slice_type ) //IIII
		{
			x265_enc_cu_x_lcu_collect_arl_stats ( h, enc_cu, h->data_cu ) ;
		}
	}
#endif
}

void x265_enc_cu_encode_cu ( x265_t *h, x265_enc_cu_t *enc_cu, uint32_t i_cu_addr )
{
	if ( h->pps[0].b_use_dqp )
	{
		enc_cu->b_encode_dqp = 1 ;
	}

	// Encode CU data
	x265_enc_cu_x_encode_cu ( h, enc_cu, h->data_cu, 0, 0 ) ;
}

#if X265_ADAPTIVE_QP_SELECTION
/** Collect ARL statistics from one block
  */
int x265_enc_cu_x_tu_collect_arl_stats( x265_enc_cu_t *enc_cu,
										x265_coeff_t* rpcCoeff,
										int32_t* arl_coeff,
										int32_t i_num_coeff_in_cu,
										double* f_c_sum,
										uint32_t* i_num_samples )
{
	int32_t n = 0 ;
    int32_t i_u = 0 ;
    int32_t i_absc = 0 ;

	for( n = 0; n < i_num_coeff_in_cu; n++ )
	{
	    i_u = abs( rpcCoeff[ n ] );
	    i_absc = arl_coeff[ n ];

	    if( i_u != 0 )
	    {
	    	if( i_u < X265_LEVEL_RANGE )
	    	{
	    		f_c_sum[ i_u ] += ( double )i_absc;
	    		i_num_samples[ i_u ]++;
	    	}
	    	else
	    	{
	    		f_c_sum[X265_LEVEL_RANGE] += (double)i_absc - (double)(i_u << X265_ARL_C_PRECISION) ;
	    		i_num_samples[ X265_LEVEL_RANGE ]++;
	    	}
	    }
	}

	return 0;
}

/** Collect ARL statistics from one LCU
 * \param pcCU
 */
void x265_enc_cu_x_lcu_collect_arl_stats ( x265_t *h, x265_enc_cu_t *enc_cu, x265_data_cu_t *cu )
{
	int32_t loop = 0 ;
	double f_c_sum[ X265_LEVEL_RANGE + 1 ];     //: the sum of DCT coefficients corresponding to datatype and quantization output
	uint32_t i_num_samples[ X265_LEVEL_RANGE + 1 ]; //: the number of coefficients corresponding to datatype and quantization output
	x265_coeff_t* coeff_y = NULL ;
	int32_t* arl_coeff_y = NULL ;
	uint32_t i_min_cu_width = 0 ;
	uint32_t i_min_num_coeff_in_cu = 0 ;
	uint32_t i_tr_idx = 0 ;
	int32_t i_u = 0 ;

	coeff_y = cu->tr_coeff_y;
	arl_coeff_y = cu->arl_coeff_y;

	i_min_cu_width = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth ;
	i_min_num_coeff_in_cu = 1 << i_min_cu_width;

	memset( f_c_sum, 0, sizeof( double )*(X265_LEVEL_RANGE+1) );
	memset( i_num_samples, 0, sizeof( uint32_t )*(X265_LEVEL_RANGE+1) );

	// Collect stats to f_c_sum[][] and i_num_samples[][]
	for( loop = 0; loop < cu->base_data_cu.i_num_partition ; ++ loop )
	{
		i_tr_idx = cu->base_data_cu.tr_idx[loop] ;

		if ( MODE_INTER == cu->base_data_cu.pred_mode[loop] )
		{
			if( x265_base_data_cu_get_cbf_p4 ( (x265_base_data_cu_t*)cu,
												loop, TEXT_LUMA, i_tr_idx ) )
			{
				x265_enc_cu_x_tu_collect_arl_stats ( enc_cu,
													coeff_y,
													arl_coeff_y,
													i_min_num_coeff_in_cu,
													f_c_sum,
													i_num_samples ) ;
			}//Note that only int32_terY is processed. QP rounding is based on int32_terY data only.
		}

		coeff_y  += i_min_num_coeff_in_cu;
		arl_coeff_y  += i_min_num_coeff_in_cu;
	}

	for( i_u = 1 ; i_u < X265_LEVEL_RANGE ; ++ i_u )
	{
		h->tr_quant.slice_sum_c[i_u] += f_c_sum[i_u] ;
		h->tr_quant.slice_n_samples[i_u] += i_num_samples[i_u] ;
	}
	h->tr_quant.slice_sum_c[X265_LEVEL_RANGE] += f_c_sum[X265_LEVEL_RANGE] ;
	h->tr_quant.slice_n_samples[X265_LEVEL_RANGE] += i_num_samples[X265_LEVEL_RANGE] ;

}

#endif


#if X265_AMP_ENC_SPEEDUP
void x265_enc_cu_x_compress_cu ( x265_t *h,
								x265_enc_cu_t *enc_cu,
								x265_data_cu_t **ppc_best_cu,
								x265_data_cu_t **ppc_temp_cu,
								uint32_t i_depth,
								enum part_size_e i_parent_part_size )
#else
void x265_enc_cu_x_compress_cu ( x265_t *h,
								x265_enc_cu_t *enc_cu,
								x265_data_cu_t **ppc_best_cu,
								x265_data_cu_t **ppc_temp_cu,
								uint32_t i_depth )
#endif
{
	// variables for fast encoder decision
	int32_t b_early_skip = 0 ;
	int32_t b_try_split = 0 ;
	// variable for early cu determination
	int32_t b_subranch = 0 ;
	// variable for cbf fast mode pu decision
	int32_t b_do_not_block_pu = 0 ;
	int32_t b_early_detection_skip_mode = 0;
	int32_t b_try_split_dqp  = 0 ;
	int32_t boundary = 0;
	uint32_t i_l_pixel_x = 0 ;
	uint32_t i_r_pixel_x = 0 ;
	uint32_t i_t_pixel_y = 0 ;
	uint32_t i_b_pixel_y = 0 ;
	int32_t i_base_qp = 0 ;
	int32_t i_min_qp = 0 ;
	int32_t i_max_qp = 0 ;
	int32_t b_is_add_lowest_qp = 0;
	int32_t i_lowest_qp = 0 ;
	int32_t i_dqp = 0 ;
	int32_t i_qp = 0 ;
	// if slice start or slice end is within this cu...
	x265_slice_t * slice = 0 ;
	int32_t b_slice_start = 0 ;
	int32_t b_slice_end = 0 ;
	int32_t b_inside_picture = 0 ;
	int32_t i_idx = 0 ;
#if X265_AMP_ENC_SPEEDUP
	int32_t b_test_amp_hor = 0, b_test_amp_ver = 0;
#if X265_AMP_MRG
	int32_t b_test_merge_amp_hor = 0, b_test_merge_amp_ver = 0;
#endif
#endif

	uint32_t i_raw_bits = 0 ;
	uint32_t i_best_bits = 0 ;
	int32_t i_start_qp = 0 ;
	uint32_t i_cur_slice_start_part_idx = 0 ;
	uint8_t i_next_depth = 0 ;
	uint32_t i_part_unit_idx = 0 ;
	int32_t b_in_slice = 0 ;
	int32_t b_has_residual = 0;
	uint32_t i_blk_idx = 0 ;
	uint32_t i_target_part_idx = 0 ;
	int32_t b_is_end_of_slice = 0 ;
	int32_t b_is_end_of_slice_segment = 0 ;
	int32_t b_found_non_zero_cbf = 0;


	x265_data_cu_t **ppc_subest_part_cu = NULL ;
	x265_data_cu_t **ppc_sub_temp_part_cu = NULL ;


	double f_rd_skip = 0.0 ;
	static int32_t num [ X265_MAX_CU_DEPTH ];
	static double cost[ X265_MAX_CU_DEPTH ];
	x265_image_copy_from_frame ( h,
								enc_cu->orig_image[i_depth],
								h->fenc,
								(*ppc_best_cu)->base_data_cu.i_cu_addr,
								(*ppc_best_cu)->base_data_cu.i_abs_idx_in_lcu ) ;

	b_try_split = 1;
	f_rd_skip = X265_MAX_DOUBLE ;

	// variable for early cu determination
	b_subranch = 1;
	b_do_not_block_pu = 1;
	b_try_split_dqp  = 1;

	if ( (*ppc_best_cu)->base_data_cu.i_cu_addr == 0 )
	{
		memset( cost, 0, sizeof( cost ) );
		memset( num,  0, sizeof( num  ) );
	}

	i_l_pixel_x = x265_data_cu_get_cu_pel_x(*ppc_best_cu);
	i_r_pixel_x = i_l_pixel_x + x265_data_cu_get_width_p2(*ppc_best_cu, 0)  - 1;
	i_t_pixel_y = x265_data_cu_get_cu_pel_y(*ppc_best_cu);;
	i_b_pixel_y = i_t_pixel_y + x265_data_cu_get_height_p2(*ppc_best_cu, 0) - 1;

	i_base_qp = x265_enc_cu_x_compute_qp ( h, enc_cu, *ppc_best_cu, i_depth );
	i_lowest_qp = - h->sps[0].i_qp_bd_offset_y ;

	if( (h->cu.pic.i_max_cu_width>>i_depth) >= h->pps[0].i_min_cu_dqp_size )
	{
		i_dqp = h->param.rdo.i_max_delta_qp ;
		i_min_qp = x265_clip3_int32 ( i_base_qp - i_dqp, - h->sps[0].i_qp_bd_offset_y, X265_MAX_QP );
		i_max_qp = x265_clip3_int32 ( i_base_qp + i_dqp, - h->sps[0].i_qp_bd_offset_y, X265_MAX_QP );
		if ( h->sps[0].b_use_lossless && (i_lowest_qp < i_min_qp)
				&& h->pps[0].b_use_dqp )
		{
			b_is_add_lowest_qp = 1;
			i_min_qp = i_min_qp - 1;
		}
	}
	else
	{
		i_min_qp = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)(*ppc_temp_cu), 0);
		i_max_qp = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)(*ppc_temp_cu), 0) ;
	}

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	if ( h->param.rc.b_enable_rate_control )
	{
		i_min_qp = h->enc_rate_ctrl.i_rc_qp ;
		i_max_qp = h->enc_rate_ctrl.i_rc_qp ;
	}
#else
	if( h->param.b_enable_rate_ctrl )
	{
		i_qp = enc_cu->rate_ctrl->get_unit_qp();
		i_min_qp = x265_clip3_int32( i_qp, X265_MIN_QP, X265_MAX_QP ) ;
		i_max_qp = x265_clip3_int32( i_qp, X265_MIN_QP, X265_MAX_QP ) ;
	}
#endif

	// if slice start or slice end is within this cu...
	slice = h->slice ;
	b_slice_start = slice->i_slice_segment_cur_start_cu_addr >
					x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)(*ppc_temp_cu) )
					&& slice->i_slice_segment_cur_start_cu_addr <
					x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)(*ppc_temp_cu) )
					+ x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)(*ppc_temp_cu)) ;
	b_slice_end = (slice->i_slice_segment_cur_end_cu_addr >
					x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)(*ppc_temp_cu) )
					&& slice->i_slice_segment_cur_end_cu_addr <
					x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)(*ppc_temp_cu) )
					+ x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)(*ppc_temp_cu)) ) ;
	b_inside_picture = ( i_r_pixel_x < h->sps[0].i_pic_width_in_luma_samples )
						&& ( i_b_pixel_y < h->sps[0].i_pic_height_in_luma_samples );
	// we need to split, so don't try these modes.
	if( !b_slice_end && !b_slice_start && b_inside_picture )
	{
		for ( i_qp = i_min_qp ; i_qp <= i_max_qp ; i_qp++ )
		{
			if (b_is_add_lowest_qp && (i_qp == i_min_qp))
			{
				i_qp = i_lowest_qp;
			}
			// variables for fast encoder decision
			b_early_skip = 0;
			b_try_split = 1;
			f_rd_skip = X265_MAX_DOUBLE ;

			x265_data_cu_init_est_data( h, (*ppc_temp_cu), i_depth, i_qp );

			// do inter modes, skip and 2nx2n
			if( h->slice->i_slice_type != I_SLICE )
			{
				// 2nx2n
				if( h->param.b_use_early_skip_detection )
				{
					x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2Nx2N, 0 );
					x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
				}
				// skip
				x265_enc_cu_x_check_rd_cost_merge_2nx2n( h, enc_cu, ppc_best_cu, ppc_temp_cu, &b_early_detection_skip_mode );//by merge for inter_2nx2n
				x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );

				// fast encoder decision for early skip
				if ( h->param.b_use_fast_enc )
				{
					i_idx = h->global.convert_to_bit[x265_data_cu_get_width_p2(*ppc_best_cu, 0)];
					if ( num [ i_idx ] > 5
							&& f_rd_skip < X265_EARLY_SKIP_THRES *cost[ i_idx ]/num[ i_idx ] )
					{
						b_early_skip = 1;
						b_try_split  = 0;
					}
				}

				if( !h->param.b_use_early_skip_detection )
				{
				// 2nx2n, nx_n
					if ( !b_early_skip )
					{
						x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2Nx2N, 0 );
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
						if( h->param.b_use_cbf_fast_mode )
						{
							b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
						}
					}
				}
			}

			if( (h->cu.pic.i_max_cu_width>>i_depth) >= h->pps[0].i_min_cu_dqp_size )
			{
				if(i_qp == i_base_qp)
				{
					b_try_split_dqp = b_try_split;
				}
			}
			else
			{
				b_try_split_dqp = b_try_split;
			}
			if (b_is_add_lowest_qp && (i_qp == i_lowest_qp))
			{
				i_qp = i_min_qp;
			}
		}

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
		if ( i_depth <= enc_cu->i_add_sad_depth )
		{
			enc_cu->i_lcu_prediction_sad += enc_cu->i_temporal_sad;
			enc_cu->i_add_sad_depth = i_depth;
		}
#endif

		if(!b_early_detection_skip_mode)
		{
			for ( i_qp=i_min_qp; i_qp<=i_max_qp; i_qp++)
			{
				if (b_is_add_lowest_qp && (i_qp == i_min_qp))
				{
					i_qp = i_lowest_qp;
				}
				x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );

				// do inter modes, nx_n, 2nx_n, and nx2n
				if( h->slice->i_slice_type != I_SLICE )
				{
					// 2nx2n, nx_n
					if ( !b_early_skip )
					{
						if( ! ( (x265_data_cu_get_width_p2(*ppc_best_cu, 0) == 8)
							&& (x265_data_cu_get_height_p2(*ppc_best_cu, 0) == 8)
							))
						{
							if( i_depth == h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth && b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_NxN, 0 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
							}
						}
					}

					// 2nx_n, nx2n
					if(b_do_not_block_pu)
					{
						x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_Nx2N, 0 );
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
						if( h->param.b_use_cbf_fast_mode
								&& x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) == SIZE_Nx2N )
						{
							b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
						}
					}
					if(b_do_not_block_pu)
					{
						x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2NxN, 0 );
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
						if( h->param.b_use_cbf_fast_mode
								&& x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) == SIZE_2NxN )
						{
							b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
						}
					}

#if 1
					//! try amp (SIZE_2NxnU, SIZE_2NxnD, SIZE_nLx2N, SIZE_nRx2N)
					if( h->sps[0].amp_acc[i_depth] )
					{
#if X265_AMP_ENC_SPEEDUP
#if X265_AMP_MRG
						x265_enc_cu_derive_test_mode_amp ( enc_cu,
															(*ppc_best_cu),
															i_parent_part_size,
															&b_test_amp_hor,
															&b_test_amp_ver,
															&b_test_merge_amp_hor,
															&b_test_merge_amp_ver ) ;
#else
						x265_enc_cu_derive_test_mode_amp ( enc_cu,
															(*ppc_best_cu),
															i_parent_part_size,
															&b_test_amp_hor,
															&b_test_amp_ver ) ;
#endif

						//! do horizontal amp
						if ( b_test_amp_hor )
						{
							if(b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2NxnU, 0 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
								if( h->param.b_use_cbf_fast_mode
										&& x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) == SIZE_2NxnU )
								{
									b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
								}
							}
							if(b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2NxnD, 0 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
								if( h->param.b_use_cbf_fast_mode
										&& x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) == SIZE_2NxnD )
								{
									b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
								}
							}
						}
#if X265_AMP_MRG
						else if ( b_test_merge_amp_hor )
						{
							if(b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2NxnU, 1 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
								if( h->param.b_use_cbf_fast_mode
										&& x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) == SIZE_2NxnU )
								{
									b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
								}
							}
							if(b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2NxnD, 1 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
								if( h->param.b_use_cbf_fast_mode
										&& x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) == SIZE_2NxnD )
								{
									b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
								}
							}
						}
#endif

						//! do horizontal amp
						if ( b_test_amp_ver )
						{
							if(b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_nLx2N, 0 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
								if( h->param.b_use_cbf_fast_mode
										&& x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) == SIZE_nLx2N )
								{
									b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
								}
							}
							if(b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_nRx2N, 0 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
							}
						}
#if X265_AMP_MRG
						else if ( b_test_merge_amp_ver )
						{
							if(b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_nLx2N, 1 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
								if( h->param.b_use_cbf_fast_mode
										&& x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) == SIZE_nLx2N )
								{
									b_do_not_block_pu = x265_base_data_cu_get_qt_root_cbf ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != 0;
								}
							}
							if(b_do_not_block_pu)
							{
								x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_nRx2N, 1 );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
							}
						}
#endif

#else
						x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2NxnU, 1 );
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );

						x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2NxnD, 1 );
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );

						x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_nLx2N, 1 );
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );

						x265_enc_cu_x_check_rd_cost_inter( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_nRx2N, 1 );
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
#endif
					}
#endif
				}

				// do normal intra modes
				if ( !b_early_skip )
				{
					// speedup for inter frames
					if( h->slice->i_slice_type == I_SLICE ||
						x265_base_data_cu_get_cbf_p3((x265_base_data_cu_t*)(*ppc_best_cu), 0, TEXT_LUMA ) != 0 ||
						x265_base_data_cu_get_cbf_p3((x265_base_data_cu_t*)(*ppc_best_cu), 0, TEXT_CHROMA_U ) != 0 ||
						x265_base_data_cu_get_cbf_p3((x265_base_data_cu_t*)(*ppc_best_cu), 0, TEXT_CHROMA_V ) != 0
							) // avoid very complex intra if it is unlikely
					{
						x265_enc_cu_x_check_rd_cost_intra( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_2Nx2N );
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
						if( i_depth == h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth )
						{
							if( x265_data_cu_get_width_p2(*ppc_temp_cu, 0)
									> ( 1 << h->sps[0].i_quadtree_tu_log2_min_size ) )
							{
								x265_enc_cu_x_check_rd_cost_intra( h, enc_cu, ppc_best_cu, ppc_temp_cu, SIZE_NxN );
								x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
							}
						}
					}
				}

				// test pcm
				if( h->sps[0].b_use_pcm
					&& x265_data_cu_get_width_p2(*ppc_temp_cu, 0) <= (1 << h->sps[0].i_pcm_log2_max_size)
					&& x265_data_cu_get_width_p2(*ppc_temp_cu, 0) >= (1 << h->sps[0].i_pcm_log2_min_size) )
				{
					i_raw_bits = (2 * h->cu.pic.i_bit_depth_y + h->cu.pic.i_bit_depth_c)
									* x265_data_cu_get_width_p2(*ppc_best_cu, 0)
									* x265_data_cu_get_height_p2(*ppc_best_cu, 0) / 2;
					i_best_bits = (*ppc_best_cu)->i_total_bits;
					if((i_best_bits > i_raw_bits) ||
							((*ppc_best_cu)->f_total_cost >
						x265_rd_cost_calc_rd_cost( &h->rd_cost, i_raw_bits, 0, 0, DF_DEFAULT )))
					{
						x265_enc_cu_x_check_intra_pcm ( h, enc_cu, ppc_best_cu, ppc_temp_cu);
						x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );
					}
				}
				if ( b_is_add_lowest_qp && (i_qp == i_lowest_qp))
				{
					i_qp = i_min_qp;
				}
			}
		}

		x265_enc_entropy_reset_bits (&h->enc_entropy) ;
		x265_enc_entropy_encode_split_flag (h,
											&h->enc_entropy,
											*ppc_best_cu,
											0,
											i_depth,
											1 );
		(*ppc_best_cu)->i_total_bits += x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy); // split bits
		if( h->param.b_use_sbac_rd )
		{
			(*ppc_best_cu)->i_total_bins += x265_enc_bin_cabac_get_bins_coded(
					(x265_enc_bin_cabac_t *)x265_enc_sbac_get_enc_bin_if(
					(x265_enc_sbac_t*)h->enc_entropy.enc_entropy_if)) ;
		}
		(*ppc_best_cu)->f_total_cost = x265_rd_cost_calc_rd_cost( &h->rd_cost, (*ppc_best_cu)->i_total_bits, (*ppc_best_cu)->i_total_distortion, 0, DF_DEFAULT ) ;

		// accumulate statistics for early skip
		if ( h->param.b_use_sbac_rd )
		{
			if ( x265_base_data_cu_is_skipped((x265_base_data_cu_t*)(*ppc_best_cu), 0) )
			{
				i_idx = h->global.convert_to_bit[ x265_data_cu_get_width_p2(*ppc_best_cu, 0)];
				cost[ i_idx ] += (*ppc_best_cu)->f_total_cost;
				num [ i_idx ] ++;
			}
		}

		// early cu determination
		if( h->param.b_use_early_cu
				&& x265_base_data_cu_is_skipped((x265_base_data_cu_t*)(*ppc_best_cu), 0) )
		{
			b_subranch = 0;
		}
		else
		{
			b_subranch = 1;
		}
	}
	else if(!(b_slice_end && b_inside_picture))
	{
		boundary = 1;
#if X265_RATE_CONTROL_LAMBDA_DOMAIN
		enc_cu->i_add_sad_depth++;
#endif
	}

	// copy orginal yuv samples to pcm buffer
	if( x265_base_data_cu_is_lossless_coded( h, (x265_base_data_cu_t*)(*ppc_best_cu), 0)
			&& (x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)(*ppc_best_cu), 0) == 0))
	{
		x265_enc_cu_x_fill_pcm_buffer( h, enc_cu, *ppc_best_cu, enc_cu->orig_image[i_depth]);
	}
	if( (h->cu.pic.i_max_cu_width>>i_depth) == h->pps[0].i_min_cu_dqp_size )
	{
		i_dqp = h->param.rdo.i_max_delta_qp ;
		i_min_qp = x265_clip3_int32 ( i_base_qp - i_dqp, - h->sps[0].i_qp_bd_offset_y, X265_MAX_QP );
		i_max_qp = x265_clip3_int32 ( i_base_qp + i_dqp, - h->sps[0].i_qp_bd_offset_y, X265_MAX_QP );
		if ( (h->sps[0].b_use_lossless)
				&& (i_lowest_qp < i_min_qp) && h->pps[0].b_use_dqp )
		{
			b_is_add_lowest_qp = 1;
			i_min_qp = i_min_qp - 1;
		}
	}
	else if( (h->cu.pic.i_max_cu_width>>i_depth) > h->pps[0].i_min_cu_dqp_size )
	{
		i_min_qp = i_base_qp;
		i_max_qp = i_base_qp;
	}
	else
	{
		if ( x265_data_cu_get_slice_segment_start_cu_p2(h->data_cu,
				x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)(*ppc_temp_cu)))
				== slice->i_slice_segment_cur_start_cu_addr )
		{
			i_start_qp = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)(*ppc_temp_cu), 0) ;
		}
		else
		{
			i_cur_slice_start_part_idx = slice->i_slice_segment_cur_start_cu_addr
											% h->cu.pic.i_num_partitions
											- x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)(*ppc_temp_cu)) ;
			i_start_qp = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)(*ppc_temp_cu), i_cur_slice_start_part_idx ) ;
		}
		i_min_qp = i_start_qp;
		i_max_qp = i_start_qp;
	}

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	if ( h->param.rc.b_enable_rate_control )
	{
		i_min_qp = h->enc_rate_ctrl.i_rc_qp ;
		i_max_qp = h->enc_rate_ctrl.i_rc_qp ;
	}
#else
	if( h->param.b_enable_rate_ctrl )
	{
		i_qp = enc_cu->rate_ctrl->get_unit_qp();
		i_min_qp = x265_clip3_int32( i_qp, X265_MIN_QP, X265_MAX_QP ) ;
		i_max_qp = x265_clip3_int32( i_qp, X265_MIN_QP, X265_MAX_QP ) ;
	}
#endif

	for ( i_qp=i_min_qp; i_qp<=i_max_qp; i_qp++)
	{
		if (b_is_add_lowest_qp && (i_qp == i_min_qp))
		{
			i_qp = i_lowest_qp;
		}
		x265_data_cu_init_est_data( h, *ppc_temp_cu, i_depth, i_qp );

		// further split
		if( b_subranch && b_try_split_dqp && i_depth < h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth )
		{
			i_next_depth = i_depth+1;
			ppc_subest_part_cu = &enc_cu->best_cu[i_next_depth];
			ppc_sub_temp_part_cu = &enc_cu->temp_cu[i_next_depth];

			for ( i_part_unit_idx = 0; i_part_unit_idx < 4; i_part_unit_idx++ )
			{
				x265_data_cu_init_sub_cu ( h, *ppc_subest_part_cu, *ppc_temp_cu, i_part_unit_idx, i_next_depth, i_qp );           // clear sub partition datas or init.
				x265_data_cu_init_sub_cu ( h, *ppc_sub_temp_part_cu, *ppc_temp_cu, i_part_unit_idx, i_next_depth, i_qp );           // clear sub partition datas or init.

				b_in_slice = x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)(*ppc_subest_part_cu))
						+ x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)(*ppc_subest_part_cu))
						> slice->i_slice_segment_cur_start_cu_addr
						&& x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)(*ppc_subest_part_cu))
						< slice->i_slice_segment_cur_end_cu_addr ;
				if(b_in_slice
					&& ( x265_data_cu_get_cu_pel_x(*ppc_subest_part_cu) < h->sps[0].i_pic_width_in_luma_samples )
					&& ( x265_data_cu_get_cu_pel_y(*ppc_subest_part_cu) < h->sps[0].i_pic_height_in_luma_samples ) )
				{
					if( h->param.b_use_sbac_rd )
					{
						if ( 0 == i_part_unit_idx) //initialize rd with previous depth buffer
						{
							x265_enc_sbac_load ( h->pppc_rd_sbac_coder[i_next_depth][CI_CURR_BEST],
												h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST] ); //this load is used to simplify the code
						}
						else
						{
							x265_enc_sbac_load ( h->pppc_rd_sbac_coder[i_next_depth][CI_CURR_BEST],
												h->pppc_rd_sbac_coder[i_next_depth][CI_NEXT_BEST] ); //this load is used to simplify the code
						}
					}

#if X265_AMP_ENC_SPEEDUP
					if ( x265_base_data_cu_is_intra((x265_base_data_cu_t*)(*ppc_best_cu), 0) )
					{
						x265_enc_cu_x_compress_cu( h,
													enc_cu,
													ppc_subest_part_cu,
													ppc_sub_temp_part_cu,
													i_next_depth,
													SIZE_NONE );
					}
					else
					{
						x265_enc_cu_x_compress_cu( h,
													enc_cu,
													ppc_subest_part_cu,
													ppc_sub_temp_part_cu,
													i_next_depth,
													x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0) );
					}
#else
					x265_enc_cu_x_compress_cu( h,
												enc_cu,
												subest_part_cu,
												sub_temp_part_cu,
												i_next_depth ) ;
#endif

					x265_data_cu_copy_part_from ( h,
												*ppc_temp_cu,
												*ppc_subest_part_cu,
												i_part_unit_idx,
												i_next_depth );         // keep best part data to current temporary data.
					x265_enc_cu_x_copy_image2tmp( h,
												enc_cu,
												x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)(*ppc_subest_part_cu)) * i_part_unit_idx,
												i_next_depth );
				}
				else if (b_in_slice)
				{
					x265_data_cu_copy_to_pic_p3( h, *ppc_subest_part_cu, i_next_depth );
					// keep best part data to current temporary data.
					x265_data_cu_copy_part_from ( h,
												*ppc_temp_cu,
												*ppc_subest_part_cu,
												i_part_unit_idx,
												i_next_depth );
				}
			}

			if( !boundary )
			{
				x265_enc_entropy_reset_bits (&h->enc_entropy) ;
				x265_enc_entropy_encode_split_flag (h,
													&h->enc_entropy,
													*ppc_temp_cu,
													0,
													i_depth,
													1 );

				(*ppc_temp_cu)->i_total_bits += x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy); // split bits
				if( h->param.b_use_sbac_rd )
				{
					(*ppc_temp_cu)->i_total_bins += x265_enc_bin_cabac_get_bins_coded(
							(x265_enc_bin_cabac_t *)x265_enc_sbac_get_enc_bin_if(
							(x265_enc_sbac_t*)h->enc_entropy.enc_entropy_if)) ;
				}
			}
			(*ppc_temp_cu)->f_total_cost = x265_rd_cost_calc_rd_cost( &h->rd_cost, (*ppc_temp_cu)->i_total_bits, (*ppc_temp_cu)->i_total_distortion, 0, DF_DEFAULT );

			if( (h->cu.pic.i_max_cu_width>>i_depth) == h->pps[0].i_min_cu_dqp_size
					&& h->pps[0].b_use_dqp )
			{
				for( i_blk_idx = 0;
						i_blk_idx < x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)(*ppc_temp_cu));
						i_blk_idx ++)
				{
					if( ( x265_data_cu_get_slice_segment_start_cu_p2( h->data_cu,
							i_blk_idx + x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)(*ppc_temp_cu)))
							== h->slice->i_slice_segment_cur_start_cu_addr )
							&&
						( x265_base_data_cu_get_cbf_p3 ( (x265_base_data_cu_t*)(*ppc_temp_cu), i_blk_idx, TEXT_LUMA )
						|| x265_base_data_cu_get_cbf_p3 ( (x265_base_data_cu_t*)(*ppc_temp_cu), i_blk_idx, TEXT_CHROMA_U )
						|| x265_base_data_cu_get_cbf_p3 ( (x265_base_data_cu_t*)(*ppc_temp_cu), i_blk_idx, TEXT_CHROMA_V )
							)
						)
					{
						b_has_residual = 1;
						break;
					}
				}

				if ( x265_data_cu_get_slice_segment_start_cu_p2( h->data_cu,
						i_blk_idx + x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)(*ppc_temp_cu)))
						!= slice->i_slice_segment_cur_start_cu_addr )
				{
					i_target_part_idx = slice->i_slice_segment_cur_start_cu_addr
										% h->cu.pic.i_num_partitions
										- x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)(*ppc_temp_cu)) ;
				}
				else
				{
					i_target_part_idx = 0;
				}
				if ( b_has_residual )
				{
#if !X265_RDO_WITHOUT_DQP_BITS
					x265_enc_entropy_reset_bits (&h->enc_entropy) ;
					x265_enc_entropy_encode_qp (h,
												&h->enc_entropy,
												*ppc_temp_cu,
												i_target_part_idx,
												0 );
					(*ppc_best_cu)->i_total_bits += x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy); // split bits
					if( h->param.b_use_sbac_rd )
					{
						(*ppc_temp_cu)->i_total_bins += x265_enc_bin_cabac_get_bins_coded(
								(x265_enc_bin_cabac_t *)x265_enc_sbac_get_enc_bin_if(
								(x265_enc_sbac_t*)h->enc_entropy.enc_entropy_if)) ;
					}
					(*ppc_temp_cu)->f_total_cost = x265_rd_cost_calc_rd_cost( &h->rd_cost, (*ppc_temp_cu)->i_total_bits, (*ppc_temp_cu)->i_total_distortion, 0, DF_DEFAULT );
#endif
					x265_base_data_cu_set_qp_sub_cus( h,
													(x265_base_data_cu_t*)(*ppc_temp_cu),
													x265_data_cu_get_ref_qp(h, *ppc_temp_cu, i_target_part_idx ),
													(x265_base_data_cu_t*)(*ppc_temp_cu),
													0,
													i_depth,
													&b_found_non_zero_cbf ) ;
					assert( b_found_non_zero_cbf );
				}
				else
				{
					// set qp to default qp
					x265_base_data_cu_set_qp_sub_parts(h,
														(x265_base_data_cu_t*)(*ppc_temp_cu),
														x265_data_cu_get_ref_qp(h, *ppc_temp_cu, i_target_part_idx ),
														0,
														i_depth );
				}
			}

			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_store ( h->pppc_rd_sbac_coder[i_next_depth][CI_NEXT_BEST],
									h->pppc_rd_sbac_coder[i_depth][CI_TEMP_BEST] ) ;
			}
			b_is_end_of_slice = h->slice->i_slice_mode == FIXED_NUMBER_OF_BYTES
								&& ((*ppc_best_cu)->i_total_bits > h->slice->i_slice_argument << 3) ;
			b_is_end_of_slice_segment = h->slice->i_slice_segment_mode == FIXED_NUMBER_OF_BYTES
										&& ((*ppc_best_cu)->i_total_bits > h->slice->i_slice_segment_argument << 3 ) ;
			if(b_is_end_of_slice||b_is_end_of_slice_segment)
			{
				(*ppc_best_cu)->f_total_cost = (*ppc_temp_cu)->f_total_cost + 1 ;
			}
			x265_enc_cu_x_check_best_mode( h, enc_cu, ppc_best_cu, ppc_temp_cu, i_depth);                                  // rd compare current larger prediction
		}                                                                                  // with sub partitioned prediction.
		if (b_is_add_lowest_qp && (i_qp == i_lowest_qp))
		{
			i_qp = i_min_qp;
		}
	}

	//	print_int_state ( (*ppc_best_cu)->i_total_bits ) ;
	// copy best data to picture for next partition prediction.
	x265_data_cu_copy_to_pic_p3 (h, *ppc_best_cu, i_depth) ;
	x265_enc_cu_x_copy_image2frame( h,
									enc_cu,
									h->fdec,
									(*ppc_best_cu)->base_data_cu.i_cu_addr,
									(*ppc_best_cu)->base_data_cu.i_abs_idx_in_lcu,
									i_depth,
									i_depth,
									*ppc_best_cu,
									i_l_pixel_x,
									i_t_pixel_y ) ;

	if( boundary ||(b_slice_end && b_inside_picture))
	{
		return;
	}

	// assert if best prediction mode is none
	// selected mode's rd-cost must be not x265_max_double.
//	assert( x265_data_cu_get_partition_size_p2(*ppc_best_cu, 0 ) != SIZE_NONE  );
//	assert( x265_base_data_cu_get_prediction_mode_p2 ((x265_base_data_cu_t*)(*ppc_best_cu), 0 ) != MODE_NONE  );
//	assert( (*ppc_best_cu)->f_total_cost != X265_MAX_DOUBLE );



	/**
	x265_image_copy_from_frame ( h,
								enc_cu->reco_image_best[i_depth],
								h->fenc,
								(*ppc_best_cu)->base_data_cu.i_cu_addr,
								(*ppc_best_cu)->base_data_cu.i_abs_idx_in_lcu ) ;
								*/





}

void x265_enc_cu_finish_cu ( x265_t *h,
							x265_enc_cu_t *enc_cu,
							x265_data_cu_t *cu,
							uint32_t i_abs_part_idx,
							uint32_t i_depth )
{
	x265_slice_t *slice = NULL;
	//calculate end address
	uint32_t i_cu_addr = 0;
	uint32_t i_internal_address = 0;
	uint32_t i_external_address = 0;
	uint32_t i_pos_x = 0;
	uint32_t i_pos_y = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_real_end_address = 0;
	// encode slice finish
	int32_t b_terminate_slice = 0;
	uint32_t i_granularity_width = 0;
	int32_t b_granularity_boundary = 0;
	int32_t i_number_of_written_bits = 0;
	// calculate slice end if this cu puts us over slice bit size.
	uint32_t i_granularity_size = 0;
	int32_t i_granularity_end = 0;

	slice = h->slice;
	//calculate end address
	i_cu_addr = x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)cu) + i_abs_part_idx;
	i_internal_address = x265_pic_sym_get_pic_scu_addr(h, slice->i_slice_segment_cur_end_cu_addr - 1) % h->cu.pic.i_num_partitions;
	i_external_address = x265_pic_sym_get_pic_scu_addr(h, slice->i_slice_segment_cur_end_cu_addr - 1) / h->cu.pic.i_num_partitions;
	i_pos_x = ( i_external_address % h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_width + h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_internal_address] ];
	i_pos_y = ( i_external_address / h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_height + h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_internal_address] ];
	i_width = h->sps[0].i_pic_width_in_luma_samples;
	i_height = h->sps[0].i_pic_height_in_luma_samples;
	while(i_pos_x>=i_width||i_pos_y>=i_height)
	{
		i_internal_address--;
		i_pos_x = ( i_external_address % h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_width + h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_internal_address] ];
		i_pos_y = ( i_external_address / h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_height + h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_internal_address] ];
	}

	i_internal_address++;
	if(i_internal_address == h->cu.pic.i_num_partitions)
	{
		i_internal_address = 0;
		i_external_address = h->cu_order_map[h->inverse_cu_order_map[i_external_address]+1];
	}

	i_real_end_address = x265_pic_sym_get_pic_scu_enc_order(h,
															i_external_address * h->cu.pic.i_num_partitions
															+ i_internal_address);

	// encode slice finish
	b_terminate_slice = 0;
	if (i_cu_addr + (h->cu.pic.i_num_partitions >> (i_depth << 1)) == i_real_end_address)
	{
		b_terminate_slice = 1;
	}
	i_granularity_width = h->cu.pic.i_max_cu_width;
	i_pos_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_abs_part_idx] ];
	i_pos_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_abs_part_idx] ];
	b_granularity_boundary=((i_pos_x + x265_data_cu_get_width_p2(cu, i_abs_part_idx)) % i_granularity_width == 0
								||(i_pos_x + x265_data_cu_get_width_p2(cu, i_abs_part_idx) == i_width))
						  && ((i_pos_y + x265_data_cu_get_height_p2(cu, i_abs_part_idx)) % i_granularity_width == 0
								||(i_pos_y + x265_data_cu_get_height_p2(cu, i_abs_part_idx) == i_height)) ;

	if(b_granularity_boundary)
	{
		// the 1-terminating bit is added to all streams, so don't add it here when it's 1.
		if (!b_terminate_slice)
		{
			x265_enc_entropy_encode_terminating_bit(&h->enc_entropy,
													b_terminate_slice ? 1 : 0 );
		}
	}

	if ( cu->b_being_encode )
	{
		return ;
	}

	i_number_of_written_bits = 0;
	if (enc_cu->bit_counter)
	{
		i_number_of_written_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
	}

	// calculate slice end if this cu puts us over slice bit size.
	i_granularity_size = h->cu.pic.i_num_partitions;
	i_granularity_end = ((x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)cu) + i_abs_part_idx) / i_granularity_size) * i_granularity_size;
	if(i_granularity_end <= slice->i_slice_segment_cur_start_cu_addr)
	{
		i_granularity_end += X265_MAX(i_granularity_size,
									(h->cu.pic.i_num_partitions >> (i_depth<<1)));
	}

	// set slice end parameter
	if( (slice->i_slice_mode == FIXED_NUMBER_OF_BYTES)
		&& (!slice->b_finalized)
		&& (slice->i_slice_bits + i_number_of_written_bits) > (slice->i_slice_argument << 3))
	{
		slice->i_slice_segment_cur_end_cu_addr = i_granularity_end ;
		slice->i_slice_cur_end_cu_addr = i_granularity_end ;
		return;
	}
	// set dependent slice end parameter
	if( (slice->i_slice_segment_mode == FIXED_NUMBER_OF_BYTES)
		&& (!slice->b_finalized)
		&& (slice->i_slice_segment_bits + i_number_of_written_bits) > (slice->i_slice_segment_argument << 3))
	{
		slice->i_slice_segment_cur_end_cu_addr = i_granularity_end ;
		return;
	}
	if(b_granularity_boundary)
	{
		slice->i_slice_bits =((uint32_t)(slice->i_slice_bits + i_number_of_written_bits));
		slice->i_slice_segment_bits = slice->i_slice_segment_bits + i_number_of_written_bits ;
		if (enc_cu->bit_counter)
		{
			x265_enc_entropy_reset_bits(&h->enc_entropy);
		}
	}

}

void x265_enc_cu_x_encode_cu ( x265_t *h,
								x265_enc_cu_t *enc_cu,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								uint32_t i_depth )
{
	int32_t b_boundary = 0;
	uint32_t i_l_pel_x = 0;
	uint32_t i_r_pel_x = 0;
	uint32_t i_t_pel_y = 0;
	uint32_t i_b_pel_y = 0;
	x265_slice_t *slice = NULL;
	// if slice start is within this cu...
	int32_t b_slice_start = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_part_unit_idx = 0;
	int32_t b_in_slice = 0;
	int32_t b_code_dqp = 0;





	i_l_pel_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_abs_part_idx] ];
	i_r_pel_x = i_l_pel_x + (h->cu.pic.i_max_cu_width>>i_depth)  - 1;
	i_t_pel_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_abs_part_idx] ];
	i_b_pel_y = i_t_pel_y + (h->cu.pic.i_max_cu_height>>i_depth) - 1;

	slice = h->slice;
	// if slice start is within this cu...
	b_slice_start = slice->i_slice_segment_cur_start_cu_addr
						> h->inverse_cu_order_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)] * h->cu.pic.i_num_partitions + i_abs_part_idx
					&& slice->i_slice_segment_cur_start_cu_addr
						< h->inverse_cu_order_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)] * h->cu.pic.i_num_partitions + i_abs_part_idx
							+ ( h->cu.pic.i_num_partitions >> (i_depth<<1)) ;
	// we need to split, so don't try these modes.
	if( !b_slice_start
		&& ( i_r_pel_x < h->sps[0].i_pic_width_in_luma_samples)
		&& ( i_b_pel_y < h->sps[0].i_pic_height_in_luma_samples) )
	{
		x265_enc_entropy_encode_split_flag(h,
											&h->enc_entropy,
											cu,
											i_abs_part_idx,
											i_depth,
											0 );
	}
	else
	{
		b_boundary = 1;
	}

	if(((i_depth < x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx))
			&& ( i_depth < (h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth)))
			|| b_boundary )
	{
		i_q_num_parts = ( h->cu.pic.i_num_partitions >> (i_depth << 1)) >> 2;
		if((h->cu.pic.i_max_cu_width >> i_depth) == h->pps[0].i_min_cu_dqp_size
				&& h->pps[0].b_use_dqp )
		{
			enc_cu->b_encode_dqp = 1 ;
		}
		for ( i_part_unit_idx = 0;
				i_part_unit_idx < 4;
				i_part_unit_idx++, i_abs_part_idx += i_q_num_parts )
		{
			i_l_pel_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_abs_part_idx] ];
			i_t_pel_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_abs_part_idx] ];
			b_in_slice = x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)cu) + i_abs_part_idx + i_q_num_parts
							> slice->i_slice_segment_cur_start_cu_addr
							&& x265_base_data_cu_get_scu_addr(h, (x265_base_data_cu_t*)cu) + i_abs_part_idx
							< slice->i_slice_segment_cur_end_cu_addr;
			if(b_in_slice
				&& (i_l_pel_x < h->sps[0].i_pic_width_in_luma_samples)
				&& (i_t_pel_y < h->sps[0].i_pic_height_in_luma_samples))
			{
				x265_enc_cu_x_encode_cu(h, enc_cu, cu, i_abs_part_idx, i_depth + 1 );
			}
		}
		return;
	}

	if(((h->cu.pic.i_max_cu_width >> i_depth) >= h->pps[0].i_min_cu_dqp_size)
		&& h->pps[0].b_use_dqp )
	{
		enc_cu->b_encode_dqp = 1 ;
	}

	if (h->pps[0].b_transquant_bypass_enable_flag)
	{
		x265_enc_entropy_encode_cu_transquant_bypass_flag(&h->enc_entropy,
															cu,
															i_abs_part_idx,
															0 );
	}
	if( !(I_SLICE == h->slice->i_slice_type) )
	{
		x265_enc_entropy_encode_skip_flag(h, &h->enc_entropy, cu, i_abs_part_idx, 0 );
	}

	if(x265_base_data_cu_get_skip_flag_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ))
	{
		x265_enc_entropy_encode_merge_index(h, &h->enc_entropy, cu, i_abs_part_idx, 0 );
		x265_enc_cu_finish_cu(h, enc_cu, cu, i_abs_part_idx, i_depth);
		return;
	}
	x265_enc_entropy_encode_pred_mode(h, &h->enc_entropy, cu, i_abs_part_idx, 0 );

	x265_enc_entropy_encode_part_size(h, &h->enc_entropy, cu, i_abs_part_idx, i_depth, 0 );

	if (x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx )
		&& x265_data_cu_get_partition_size_p2(cu, i_abs_part_idx ) == SIZE_2Nx2N )
	{
		x265_enc_entropy_encode_ipcm_info(h, &h->enc_entropy, cu, i_abs_part_idx, 0 );

		if(x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu, i_abs_part_idx))
		{
			// encode slice finish
			x265_enc_cu_finish_cu(h, enc_cu, cu, i_abs_part_idx, i_depth);
			return;
		}
	}

	// prediction info ( int32_tra : direction mode, int32_ter : mv, reference idx )
	x265_enc_entropy_encode_pred_info(h, &h->enc_entropy, cu, i_abs_part_idx, 0 );

	// encode coefficients
	b_code_dqp = enc_cu->b_encode_dqp ;
	x265_enc_entropy_encode_coeff(h,
								&h->enc_entropy,
								cu,
								i_abs_part_idx,
								i_depth,
								x265_data_cu_get_width_p2(cu, i_abs_part_idx),
								x265_data_cu_get_height_p2(cu, i_abs_part_idx),
								&b_code_dqp );
	enc_cu->b_encode_dqp = b_code_dqp ;

	// --- write terminating bit ---
	x265_enc_cu_finish_cu(h, enc_cu, cu, i_abs_part_idx, i_depth);





}



#if X265_AMP_ENC_SPEEDUP
#if X265_AMP_MRG
void x265_enc_cu_derive_test_mode_amp (x265_enc_cu_t *enc_cu,
										x265_data_cu_t *cu,
										enum part_size_e i_parent_part_size,
										int32_t *p_test_amp_hor,
										int32_t *p_test_amp_ver,
										int32_t *p_test_merge_amp_hor,
										int32_t *p_test_merge_amp_ver )
#else
void x265_enc_cu_derive_test_mode_amp (x265_enc_cu_t *enc_cu,
										x265_data_cu_t *cu,
										enum part_size_e i_parent_part_size,
										int32_t *p_test_amp_hor,
										int32_t *p_test_amp_ver )
#endif
{
	if ( x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_2NxN )
	{
		*p_test_amp_hor = 1;
	}
	else if ( x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_Nx2N )
	{
		*p_test_amp_ver = 1;
	}
	else if ( x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_2Nx2N &&
			x265_data_cu_get_merge_flag_p2(cu, 0) == 0
			&& x265_base_data_cu_is_skipped((x265_base_data_cu_t*)cu, 0) == 0 )
	{
		*p_test_amp_hor = 1;
		*p_test_amp_ver = 1;
	}

#if X265_AMP_MRG
	//! Utilizing the partition size of parent PU
	if ( i_parent_part_size >= SIZE_2NxnU && i_parent_part_size <= SIZE_nRx2N )
	{
		*p_test_merge_amp_hor = 1;
		*p_test_merge_amp_ver = 1;
	}

	if ( i_parent_part_size == SIZE_NONE ) //! if parent is intra
	{
		if ( x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_2NxN )
		{
			*p_test_merge_amp_hor = 1;
		}
		else if ( x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_Nx2N )
		{
			*p_test_merge_amp_ver = 1;
		}
	}

	if ( x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_2Nx2N
			&& x265_base_data_cu_is_skipped((x265_base_data_cu_t*)cu, 0) == 0 )
	{
		*p_test_merge_amp_hor = 1;
		*p_test_merge_amp_ver = 1;
	}

	if ( x265_data_cu_get_width_p2(cu, 0) == 64 )
	{
		*p_test_amp_hor = 0;
		*p_test_amp_ver = 0;
	}
#else
	//! Utilizing the partition size of parent PU
	if ( i_parent_part_size >= SIZE_2NxnU && i_parent_part_size <= SIZE_nRx2N )
	{
		*p_test_amp_hor = 1;
		*p_test_amp_ver = 1;
	}

	if ( i_parent_part_size == SIZE_2Nx2N )
	{
		*p_test_amp_hor = 0;
		*p_test_amp_ver = 0;
	}
#endif
}
#endif


void x265_enc_cu_x_check_rd_cost_merge_2nx2n ( x265_t *h,
												x265_enc_cu_t *enc_cu,
												x265_data_cu_t **ppc_best_cu,
												x265_data_cu_t **ppc_temp_cu,
												int32_t *p_early_detection_skip_mode )
{
	uint32_t loop = 0;
	x265_mv_field_t mv_field_neighbours[X265_MRG_MAX_NUM_CANDS << 1]; // double length for mv of both lists
	uint8_t inter_dir_neighbours[X265_MRG_MAX_NUM_CANDS];
	int32_t num_valid_merge_cand = 0;
	uint8_t depth = 0;
	int32_t merge_cand_buffer[X265_MRG_MAX_NUM_CANDS];
	int32_t b_best_is_skip = 0;
	uint32_t iteration;
	uint32_t i_no_residual = 0;
	uint32_t i_merge_cand = 0;
	int32_t org_qp = 0;
	int32_t absoulte_mv = 0;
	uint32_t i_ref_list_idx = 0;
	x265_cu_mv_field_t* cu_mv_field = NULL;
	int32_t i_hor = 0;
	int32_t i_ver = 0;

	for( loop = 0; loop < h->slice->i_max_num_merge_cand ; ++loop )
	{
		inter_dir_neighbours[loop] = 0;
	}
	depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)(*ppc_temp_cu), 0 );
	x265_data_cu_set_part_size_sub_parts(h, *ppc_temp_cu, SIZE_2Nx2N, 0, depth ); // interprets depth relative to lcu level
	x265_base_data_cu_set_cu_transquant_bypass_sub_parts(h,
														(x265_base_data_cu_t*)(*ppc_temp_cu),
														h->param.b_cu_transquant_bypass_flag_value,
														0,
														depth );
	x265_data_cu_get_inter_merge_candidates(h,
											*ppc_temp_cu,
											0,
											0,
											mv_field_neighbours,
											inter_dir_neighbours,
											&num_valid_merge_cand,
											-1 );
	for( loop = 0; loop < h->slice->i_max_num_merge_cand; ++loop )
	{
		merge_cand_buffer[loop] = 0;
	}

	if ( x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)(*ppc_temp_cu), 0))
	{
		iteration = 1;
	}
	else
	{
		iteration = 2;
	}

	for( i_no_residual = 0; i_no_residual < iteration; ++i_no_residual )
	{
		for( i_merge_cand = 0; i_merge_cand < num_valid_merge_cand; ++i_merge_cand )
		{
			{
				if(!(i_no_residual==1 && merge_cand_buffer[i_merge_cand]==1))
				{

					if( !(b_best_is_skip && i_no_residual == 0) )
					{
						// set mc parameters
						x265_base_data_cu_set_pred_mode_sub_parts(h,
																(x265_base_data_cu_t*)(*ppc_temp_cu),
																MODE_INTER,
																0,
																depth ); // interprets depth relative to lcu level
						x265_base_data_cu_set_cu_transquant_bypass_sub_parts(h,
																			(x265_base_data_cu_t*)(*ppc_temp_cu),
																			h->param.b_cu_transquant_bypass_flag_value,
																			0,
																			depth );
						x265_data_cu_set_part_size_sub_parts(h, *ppc_temp_cu, SIZE_2Nx2N, 0, depth ); // interprets depth relative to lcu level
						x265_data_cu_set_merge_flag_sub_parts(h, *ppc_temp_cu, 1, 0, 0, depth ); // interprets depth relative to lcu level
						x265_data_cu_set_merge_index_sub_parts(h, *ppc_temp_cu, i_merge_cand, 0, 0, depth ); // interprets depth relative to lcu level
						x265_data_cu_set_inter_dir_sub_parts(h, *ppc_temp_cu, inter_dir_neighbours[i_merge_cand], 0, 0, depth ); // interprets depth relative to lcu level
						x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)(*ppc_temp_cu), REF_PIC_LIST_0 ),
															&mv_field_neighbours[0 + 2*i_merge_cand],
															SIZE_2Nx2N,
															0,
															0,
															0); // interprets depth relative to rtemp_cu level
						x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)(*ppc_temp_cu), REF_PIC_LIST_1 ),
															&mv_field_neighbours[1 + 2*i_merge_cand],
															SIZE_2Nx2N,
															0,
															0,
															0 ); // interprets depth relative to rtemp_cu level

						// do mc
						x265_prediction_motion_compensation(h,
															(x265_prediction_t*)(&h->enc_search),
															*ppc_temp_cu,
															enc_cu->pred_image_temp[depth],
															REF_PIC_LIST_X,
															-1 );
						// estimate residual and encode everything
						x265_enc_search_encode_res_and_calc_rd_inter_cu(h,
																		&h->enc_search,
																		*ppc_temp_cu,
																		enc_cu->orig_image    [depth],
																		enc_cu->pred_image_temp[depth],
																		&enc_cu->resi_image_temp[depth],
																		&enc_cu->resi_image_best[depth],
																		&enc_cu->reco_image_temp[depth],
																		(i_no_residual? 1:0));


						if(i_no_residual==0)
						{
							if(x265_base_data_cu_get_qt_root_cbf((x265_base_data_cu_t*)(*ppc_temp_cu), 0) == 0)
							{
								merge_cand_buffer[i_merge_cand] = 1;
							}
						}

						x265_base_data_cu_set_skip_flag_sub_parts(h,
																(x265_base_data_cu_t*)(*ppc_temp_cu),
																x265_base_data_cu_get_qt_root_cbf((x265_base_data_cu_t*)(*ppc_temp_cu), 0) == 0,
																0,
																depth );
						org_qp = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)(*ppc_temp_cu), 0);
						x265_enc_cu_x_check_dqp (h, enc_cu, *ppc_temp_cu) ;
						x265_enc_cu_x_check_best_mode( h, enc_cu, ppc_best_cu, ppc_temp_cu, depth);
						x265_data_cu_init_est_data(h, *ppc_temp_cu, depth, org_qp );


						if( h->param.b_use_fast_decision_for_merge
								&& (!b_best_is_skip))
						{
							b_best_is_skip = x265_base_data_cu_get_qt_root_cbf((x265_base_data_cu_t*)(*ppc_best_cu), 0) == 0;
						}

					}
				}
			}
		}

		if(i_no_residual == 0 && h->param.b_use_early_skip_detection)
		{
			if(x265_base_data_cu_get_qt_root_cbf((x265_base_data_cu_t*)(*ppc_best_cu), 0) == 0)
			{
				if( x265_data_cu_get_merge_flag_p2(*ppc_best_cu, 0))
				{
					*p_early_detection_skip_mode = 1;
				}
				else
				{
					absoulte_mv=0;
					for ( i_ref_list_idx = 0; i_ref_list_idx < 2; i_ref_list_idx++ )
					{
						if ( h->i_ref[(enum ref_pic_list_e)i_ref_list_idx] > 0 )
						{
							cu_mv_field = x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)(*ppc_temp_cu), (enum ref_pic_list_e)i_ref_list_idx);
							i_hor = abs(x265_cu_mv_field_get_mvd(cu_mv_field, 0)->i_hor);
							i_ver = abs(x265_cu_mv_field_get_mvd(cu_mv_field, 0)->i_ver);;
							absoulte_mv += (i_hor + i_ver);
						}
					}

					if(absoulte_mv == 0)
					{
						*p_early_detection_skip_mode = 1;
					}
				}
			}
		}
	}
}


#if X265_AMP_MRG
void x265_enc_cu_x_check_rd_cost_inter ( x265_t *h,
										x265_enc_cu_t *enc_cu,
										x265_data_cu_t **ppc_best_cu,
										x265_data_cu_t **ppc_temp_cu,
										enum part_size_e i_part_size,
										int32_t b_use_mrg )
#else
void x265_enc_cu_x_check_rd_cost_inter ( x265_t *h,
										x265_enc_cu_t *enc_cu,
										x265_data_cu_t **ppc_best_cu,
										x265_data_cu_t **ppc_temp_cu,
										enum part_size_e i_part_size )
#endif

{
	uint32_t i_depth = 0 ;
	uint32_t i_sad = 0 ;
	i_depth = x265_base_data_cu_get_depth_p2 ( (x265_base_data_cu_t*)(*ppc_temp_cu), 0 ) ;

	x265_base_data_cu_set_depth_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu), i_depth, 0) ;

	x265_base_data_cu_set_skip_flag_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu), 0, 0, i_depth) ;

	x265_data_cu_set_part_size_sub_parts (h, (*ppc_temp_cu), i_part_size, 0, i_depth) ;
	x265_base_data_cu_set_pred_mode_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu), MODE_INTER, 0, i_depth) ;
	x265_base_data_cu_set_cu_transquant_bypass_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu),
														h->param.b_cu_transquant_bypass_flag_value, 0, i_depth) ;


#if X265_AMP_MRG
	(*ppc_temp_cu)->b_is_merge_amp = 1 ;
	x265_enc_search_pred_inter_search(h,
										&h->enc_search,
										*ppc_temp_cu,
										enc_cu->orig_image[i_depth],
										&enc_cu->pred_image_temp[i_depth],
										&enc_cu->resi_image_temp[i_depth],
										&enc_cu->reco_image_temp[i_depth],
										0,
										b_use_mrg );
#else
	x265_enc_search_pred_inter_search(h,
										&h->enc_search,
										*ppc_temp_cu,
										enc_cu->orig_image[i_depth],
										&enc_cu->pred_image_temp[i_depth],
										&enc_cu->resi_image_temp[i_depth],
										&enc_cu->reco_image_temp[i_depth],
										0 );
#endif

#if X265_AMP_MRG
	if ( !(*ppc_temp_cu)->b_is_merge_amp )
	{
		return;
	}
#endif

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	if ( h->param.rc.b_enable_rate_control
			&& h->param.rc.b_lcu_level_rc
			&& i_part_size == SIZE_2Nx2N
			&& i_depth <= enc_cu->i_add_sad_depth )
	{
		i_sad = x265_rd_cost_get_sad_part(&h->rd_cost,
											h->param.sps.i_bit_depth_y,
											x265_image_get_luma_addr_p2(h, enc_cu->pred_image_temp[i_depth]),
											x265_image_get_stride(enc_cu->pred_image_temp[i_depth]),
											x265_image_get_luma_addr_p2(h, enc_cu->orig_image[i_depth]),
											x265_image_get_stride(enc_cu->orig_image[i_depth]),
											x265_data_cu_get_width_p2(*ppc_temp_cu, 0),
											x265_data_cu_get_height_p2(*ppc_temp_cu, 0));
		enc_cu->i_temporal_sad = ((int32_t)i_sad);
	}
#endif

	x265_enc_search_encode_res_and_calc_rd_inter_cu(h,
													&h->enc_search,
													*ppc_temp_cu,
													enc_cu->orig_image    [i_depth],
													enc_cu->pred_image_temp[i_depth],
													&enc_cu->resi_image_temp[i_depth],
													&enc_cu->resi_image_best[i_depth],
													&enc_cu->reco_image_temp[i_depth],
													0);

	(*ppc_temp_cu)->f_total_cost = x265_rd_cost_calc_rd_cost( &h->rd_cost,
															(*ppc_temp_cu)->i_total_bits,
															(*ppc_temp_cu)->i_total_distortion,
															0,
															DF_DEFAULT ) ;
	x265_enc_cu_x_check_dqp (h, enc_cu, *ppc_temp_cu) ;
	x265_enc_cu_x_check_best_mode( h, enc_cu, ppc_best_cu, ppc_temp_cu, i_depth);
}

void x265_enc_cu_x_check_rd_cost_intra ( x265_t *h,
										x265_enc_cu_t *enc_cu,
										x265_data_cu_t **ppc_best_cu,
										x265_data_cu_t **ppc_temp_cu,
										enum part_size_e i_part_size )
{
	uint32_t i_depth = 0 ;
	int32_t b_separate_luma_chroma = 0 ;
	uint32_t i_pre_calc_dist_c = 0 ;
	int32_t b_code_dqp = 0 ;

	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;

	i_start_time = x265_get_timer_state () ;

	i_depth = x265_base_data_cu_get_depth_p2 ( (x265_base_data_cu_t*)(*ppc_temp_cu), 0 ) ;
	x265_base_data_cu_set_skip_flag_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu), 0, 0, i_depth) ;
	x265_data_cu_set_part_size_sub_parts (h, (*ppc_temp_cu), i_part_size, 0, i_depth) ;
	x265_base_data_cu_set_pred_mode_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu), MODE_INTRA, 0, i_depth) ;
	x265_base_data_cu_set_cu_transquant_bypass_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu),
														h->param.b_cu_transquant_bypass_flag_value, 0, i_depth) ;

	b_separate_luma_chroma = 1 ;

	if ( !b_separate_luma_chroma )
	{
		x265_enc_search_preest_chroma_pred_mode (h,
												&h->enc_search,
												*ppc_temp_cu,
												enc_cu->orig_image[i_depth],
												enc_cu->pred_image_temp[i_depth] ) ;
	}

	x265_enc_search_est_intra_pred_qt (h,
										&h->enc_search,
										*ppc_temp_cu,
										enc_cu->orig_image[i_depth],
										enc_cu->pred_image_temp[i_depth],
										enc_cu->resi_image_temp[i_depth],
										enc_cu->reco_image_temp[i_depth],
										&i_pre_calc_dist_c,
										b_separate_luma_chroma ) ;
	/*
	x265_image_copy_to_frame_luma (h,
									enc_cu->reco_image_temp[i_depth],
									h->fdec,
									x265_base_data_cu_get_addr((x265_base_data_cu_t*)(*ppc_temp_cu)),
									x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)(*ppc_temp_cu)),
									0, 0) ;
	x265_image_copy_to_frame_luma (h,
									enc_cu->reco_image_temp[i_depth],
									h->fintra,
									x265_base_data_cu_get_addr((x265_base_data_cu_t*)(*ppc_temp_cu)),
									x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)(*ppc_temp_cu)),
									0, 0) ;
									*/
	x265_enc_search_est_intra_pred_chroma_qt (h,
										&h->enc_search,
										*ppc_temp_cu,
										enc_cu->orig_image[i_depth],
										enc_cu->pred_image_temp[i_depth],
										enc_cu->resi_image_temp[i_depth],
										enc_cu->reco_image_temp[i_depth],
										&i_pre_calc_dist_c ) ;

	x265_enc_entropy_reset_bits (&h->enc_entropy) ;
	if (h->pps[0].b_transquant_bypass_enable_flag)
	{
		x265_enc_entropy_encode_cu_transquant_bypass_flag (&h->enc_entropy,
															*ppc_temp_cu,
															0, 1) ;
	}
	x265_enc_entropy_encode_skip_flag (h, &h->enc_entropy, *ppc_temp_cu, 0, 1) ;
	x265_enc_entropy_encode_pred_mode (h, &h->enc_entropy, *ppc_temp_cu, 0, 1) ;
	x265_enc_entropy_encode_part_size (h, &h->enc_entropy, *ppc_temp_cu, 0, i_depth, 1) ;
	x265_enc_entropy_encode_pred_info (h, &h->enc_entropy, *ppc_temp_cu, 0, 1) ;
	x265_enc_entropy_encode_ipcm_info (h, &h->enc_entropy, *ppc_temp_cu, 0, 1) ;


	b_code_dqp = enc_cu->b_encode_dqp ;
	x265_enc_entropy_encode_coeff(h,
								&h->enc_entropy,
								*ppc_temp_cu,
								0,
								i_depth,
								x265_data_cu_get_width_p2(*ppc_temp_cu, 0),
								x265_data_cu_get_height_p2(*ppc_temp_cu, 0),
								&b_code_dqp ) ;
	enc_cu->b_encode_dqp = b_code_dqp ;

	if (h->param.b_use_sbac_rd)
	{
		x265_enc_sbac_store ( &h->rd_go_on_sbac_coder,
							h->pppc_rd_sbac_coder[i_depth][CI_TEMP_BEST] ) ;

	}

	(*ppc_temp_cu)->i_total_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) ;
	if( h->param.b_use_sbac_rd )
	{
		(*ppc_temp_cu)->i_total_bins += x265_enc_bin_cabac_get_bins_coded(
				(x265_enc_bin_cabac_t *)x265_enc_sbac_get_enc_bin_if(
				(x265_enc_sbac_t*)h->enc_entropy.enc_entropy_if)) ;
	}
	(*ppc_temp_cu)->f_total_cost = x265_rd_cost_calc_rd_cost( &h->rd_cost,
															(*ppc_temp_cu)->i_total_bits,
															(*ppc_temp_cu)->i_total_distortion,
															0,
															DF_DEFAULT ) ;
	x265_enc_cu_x_check_dqp (h, enc_cu, *ppc_temp_cu) ;
	x265_enc_cu_x_check_best_mode( h, enc_cu, ppc_best_cu, ppc_temp_cu, i_depth);                                  // rd compare current larger prediction


	i_end_time = x265_get_timer_state () ;
	add_total_time ( i_end_time - i_start_time ) ;

}

void x265_enc_cu_x_check_intra_pcm ( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_data_cu_t **ppc_best_cu,
									x265_data_cu_t **ppc_temp_cu )
{
	uint32_t i_depth = 0 ;

	i_depth = x265_base_data_cu_get_depth_p2 ( (x265_base_data_cu_t*)(*ppc_temp_cu), 0 ) ;

	x265_base_data_cu_set_skip_flag_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu), 0, 0, i_depth) ;
	x265_base_data_cu_set_ipcm_flag((x265_base_data_cu_t*)(*ppc_temp_cu), 0, 1) ;
	x265_base_data_cu_set_ipcm_flag_sub_parts (h,
												(x265_base_data_cu_t*)(*ppc_temp_cu),
												1,
												0,
												x265_base_data_cu_get_depth_p2 ( (x265_base_data_cu_t*)(*ppc_temp_cu), 0 )) ;
	x265_data_cu_set_part_size_sub_parts (h, (*ppc_temp_cu), SIZE_2Nx2N, 0, i_depth) ;
	x265_base_data_cu_set_pred_mode_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu), MODE_INTRA, 0, i_depth) ;
	x265_base_data_cu_set_tr_idx_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu), 0, 0, i_depth) ;
	x265_base_data_cu_set_cu_transquant_bypass_sub_parts (h, (x265_base_data_cu_t*)(*ppc_temp_cu),
														h->param.b_cu_transquant_bypass_flag_value, 0, i_depth) ;


	x265_enc_search_ipcm_search(h,
								&h->enc_search,
								*ppc_temp_cu,
								enc_cu->orig_image[i_depth],
								&enc_cu->pred_image_temp[i_depth],
								&enc_cu->resi_image_temp[i_depth],
								&enc_cu->reco_image_temp[i_depth] );

	if (h->param.b_use_sbac_rd)
	{
		x265_enc_sbac_load ( &h->rd_go_on_sbac_coder,
							h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST] ) ;
	}


	x265_enc_entropy_reset_bits (&h->enc_entropy) ;
	if (h->pps[0].b_transquant_bypass_enable_flag)
	{
		x265_enc_entropy_encode_cu_transquant_bypass_flag (&h->enc_entropy,
															*ppc_temp_cu,
															0,
															1) ;
	}
	x265_enc_entropy_encode_skip_flag (h, &h->enc_entropy, *ppc_temp_cu, 0, 1) ;
	x265_enc_entropy_encode_pred_mode (h, &h->enc_entropy, *ppc_temp_cu, 0, 1) ;
	x265_enc_entropy_encode_part_size (h, &h->enc_entropy, *ppc_temp_cu, 0, i_depth, 1) ;
	x265_enc_entropy_encode_ipcm_info (h, &h->enc_entropy, *ppc_temp_cu, 0, 1) ;

	if (h->param.b_use_sbac_rd)
	{
		x265_enc_sbac_store ( &h->rd_go_on_sbac_coder,
							h->pppc_rd_sbac_coder[i_depth][CI_TEMP_BEST] ) ;

	}


	(*ppc_temp_cu)->i_total_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) ;
	if( h->param.b_use_sbac_rd )
	{
		(*ppc_temp_cu)->i_total_bins += x265_enc_bin_cabac_get_bins_coded(
				(x265_enc_bin_cabac_t *)x265_enc_sbac_get_enc_bin_if(
				(x265_enc_sbac_t*)h->enc_entropy.enc_entropy_if)) ;
	}
	(*ppc_temp_cu)->f_total_cost = x265_rd_cost_calc_rd_cost( &h->rd_cost,
															(*ppc_temp_cu)->i_total_bits,
															(*ppc_temp_cu)->i_total_distortion,
															0,
															DF_DEFAULT ) ;
	x265_enc_cu_x_check_dqp (h, enc_cu, *ppc_temp_cu) ;
	x265_enc_cu_x_check_best_mode( h, enc_cu, ppc_best_cu, ppc_temp_cu, i_depth);

}

void x265_enc_cu_x_check_best_mode ( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_data_cu_t **ppc_best_cu,
									x265_data_cu_t **ppc_temp_cu,
									uint32_t i_depth )
{
	x265_data_cu_t *cu = NULL ;
	x265_image_t *image = NULL ;

	//	print_double_state((*ppc_temp_cu)->f_total_cost) ;
	if ( (*ppc_temp_cu)->f_total_cost < (*ppc_best_cu)->f_total_cost )
	{
		cu = *ppc_best_cu ;
		*ppc_best_cu = *ppc_temp_cu ;
		*ppc_temp_cu = cu ;

		image = enc_cu->pred_image_best[i_depth] ;
		enc_cu->pred_image_best[i_depth] = enc_cu->pred_image_temp[i_depth] ;
		enc_cu->pred_image_temp[i_depth] = image ;

		image = enc_cu->reco_image_best[i_depth] ;
		enc_cu->reco_image_best[i_depth] = enc_cu->reco_image_temp[i_depth] ;
		enc_cu->reco_image_temp[i_depth] = image ;

		image = NULL ;
		cu = NULL ;

		if ( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_store ( h->pppc_rd_sbac_coder[i_depth][CI_TEMP_BEST],
								h->pppc_rd_sbac_coder[i_depth][CI_NEXT_BEST] ) ;
		}
	}
}

void x265_enc_cu_x_check_dqp( x265_t *h, x265_enc_cu_t *enc_cu,  x265_data_cu_t* cu )
{
	uint32_t i_depth = 0 ;

	i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 ) ;

	if( h->pps[0].b_use_dqp
			&& (h->cu.pic.i_max_cu_width >> i_depth) >=
			h->pps[0].i_min_cu_dqp_size )
	{
		if ( x265_base_data_cu_get_cbf_p4( (x265_base_data_cu_t*)cu, 0, TEXT_LUMA, 0 )
				|| x265_base_data_cu_get_cbf_p4( (x265_base_data_cu_t*)cu, 0, TEXT_CHROMA_U, 0 )
				|| x265_base_data_cu_get_cbf_p4( (x265_base_data_cu_t*)cu, 0, TEXT_CHROMA_V, 0 ) )
		{
#if !X265_RDO_WITHOUT_DQP_BITS
			x265_enc_entropy_reset_bits (&h->enc_entropy) ;
			x265_enc_entropy_encode_qp (h, &h->enc_entropy, cu, 0, 0 ) ;
			cu->f_total_cost += x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) ;
			if( h->param.b_use_sbac_rd )
			{
				cu->i_total_bins += x265_enc_bin_cabac_get_bins_coded(
						(x265_enc_bin_cabac_t *)x265_enc_sbac_get_enc_bin_if(
						(x265_enc_sbac_t*)h->enc_entropy.enc_entropy_if)) ;
			}
			cu->f_total_cost = x265_rd_cost_calc_rd_cost( &h->rd_cost, cu->i_total_bits, cu->i_total_distortion, 0, DF_DEFAULT ) ;
#endif
		}
		else
		{
			x265_base_data_cu_set_qp_sub_parts(h,
											(x265_base_data_cu_t*)cu,
											x265_data_cu_get_ref_qp(h, cu, 0 ),
											0,
											i_depth );
		}
	}
}


int32_t x265_enc_cu_x_compute_qp ( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_data_cu_t *cu,
									uint32_t i_depth )
{
	int32_t i_base_qp = 0 ;
	int32_t i_qp_offset = 0 ;

	i_base_qp = h->slice->i_slice_qp ;

	return x265_clip3_int32(i_base_qp + i_qp_offset, h->sps[0].i_qp_bd_offset_y, X265_MAX_QP ) ;
}

void x265_enc_cu_x_copy_amvp_info (x265_enc_cu_t *enc_cu,
								x265_amvp_info_t* p_src,
								x265_amvp_info_t* p_dst)
{
	int32_t loop = 0 ;
	p_dst->i_n = p_src->i_n;
	for (loop = 0; loop < p_src->i_n; ++ loop)
	{
		memcpy ( &p_dst->mv_cand[loop], &p_src->mv_cand[loop], sizeof(x265_mv_t)) ;
	}
}

void x265_enc_cu_x_copy_image2frame( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_frame_t *frame,
									uint32_t i_cu_addr,
									uint32_t i_abs_part_idx,
									uint32_t i_depth,
									uint32_t i_src_depth,
									x265_data_cu_t* cu,
									uint32_t i_l_pixel_x,
									uint32_t i_t_pixel_y )
{
	uint32_t i_r_pixel_x = 0 ;
	uint32_t i_b_pixel_y = 0 ;
	x265_slice_t* slice = NULL ;
	int32_t b_slice_start = 0 ;
	int32_t b_slice_end = 0 ;
	uint32_t i_abs_part_idx_in_raster = 0 ;
	uint32_t i_src_blk_width = 0 ;
	uint32_t i_blk_width = 0 ;
	uint32_t i_part_idx_x = 0 ;
	uint32_t i_part_idx_y = 0 ;
	uint32_t i_part_idx = 0 ;
	uint32_t i_q_num_parts = 0 ;
	uint32_t i_part_unit_idx = 0 ;
	uint32_t i_sub_cu_l_pixel_x = 0 ;
	uint32_t i_sub_cu_t_pixel_y = 0 ;
	int32_t b_in_slice = 0  ;


	i_r_pixel_x = i_l_pixel_x + (h->cu.pic.i_max_cu_width >> i_depth)  - 1;
	i_b_pixel_y = i_t_pixel_y + (h->cu.pic.i_max_cu_height >> i_depth) - 1;
	slice = h->slice;
	b_slice_start = slice->i_slice_segment_cur_start_cu_addr >
					h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
					* h->cu.pic.i_num_partitions + i_abs_part_idx &&
					slice->i_slice_segment_cur_start_cu_addr <
					h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
					* h->cu.pic.i_num_partitions + i_abs_part_idx
					+ ( h->cu.pic.i_num_partitions >> (i_depth << 1));
	b_slice_end = slice->i_slice_segment_cur_end_cu_addr >
					h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
					* h->cu.pic.i_num_partitions+i_abs_part_idx &&
					slice->i_slice_segment_cur_end_cu_addr <
					h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
					* h->cu.pic.i_num_partitions + i_abs_part_idx
					+ (h->cu.pic.i_num_partitions >> (i_depth << 1)) ;
	if (!b_slice_end && !b_slice_start
			&& (i_r_pixel_x < h->sps[0].i_pic_width_in_luma_samples)
			&& (i_b_pixel_y < h->sps[0].i_pic_height_in_luma_samples))
	{
		i_abs_part_idx_in_raster = h->scan.zscan_to_raster[i_abs_part_idx];
		i_src_blk_width = h->cu.pic.i_num_part_in_width >> i_src_depth;
		i_blk_width    = h->cu.pic.i_num_part_in_width >> i_depth;
		i_part_idx_x = ( ( i_abs_part_idx_in_raster % h->cu.pic.i_num_part_in_width )
						% i_src_blk_width) / i_blk_width;
		i_part_idx_y = ( ( i_abs_part_idx_in_raster / h->cu.pic.i_num_part_in_width )
						% i_src_blk_width) / i_blk_width;
		i_part_idx = i_part_idx_y * ( i_src_blk_width / i_blk_width ) + i_part_idx_x;

		x265_image_copy_to_frame ( h, enc_cu->reco_image_best[i_src_depth], frame,
									i_cu_addr, i_abs_part_idx,
									i_depth - i_src_depth, i_part_idx ) ;
		x265_image_copy_to_image ( h, enc_cu->reco_image_best[i_src_depth], h->image_intra,
									i_abs_part_idx, i_depth - i_src_depth,
									i_part_idx);
	}
	else
	{
		i_q_num_parts = (h->cu.pic.i_num_partitions >> (i_depth << 1))>>2;

		for ( i_part_unit_idx = 0; i_part_unit_idx < 4; i_part_unit_idx++, i_abs_part_idx+=i_q_num_parts )
		{
			i_sub_cu_l_pixel_x   = i_l_pixel_x + ( h->cu.pic.i_max_cu_width >> (i_depth + 1)) * (i_part_unit_idx & 1);
			i_sub_cu_t_pixel_y   = i_t_pixel_y + ( h->cu.pic.i_max_cu_height >> (i_depth + 1)) * (i_part_unit_idx >> 1);

			b_in_slice = h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
			                     * h->cu.pic.i_num_partitions + i_abs_part_idx
			                     + i_q_num_parts > slice->i_slice_segment_cur_start_cu_addr &&
								 h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
								 * h->cu.pic.i_num_partitions + i_abs_part_idx
								 < slice->i_slice_segment_cur_end_cu_addr ;
			if (b_in_slice
					&& (i_sub_cu_l_pixel_x < h->sps[0].i_pic_width_in_luma_samples)
					&& ( i_sub_cu_t_pixel_y < h->sps[0].i_pic_height_in_luma_samples ) )
			{
				// copy image data to picture yuv
				x265_enc_cu_x_copy_image2frame( h, enc_cu, frame, i_cu_addr,
												i_abs_part_idx, i_depth + 1,
												i_src_depth, cu,
												i_sub_cu_l_pixel_x, i_sub_cu_t_pixel_y ) ;
			}
		}
	}
}

void x265_enc_cu_x_copy_image2tmp( x265_t *h,
								x265_enc_cu_t *enc_cu,
								uint32_t i_part_unit_idx,
								uint32_t i_next_depth )
{
	uint32_t i_curr_depth = 0 ;
	i_curr_depth = i_next_depth - 1;

	x265_image_copy_to_part_image( h,
									enc_cu->reco_image_best[i_next_depth],
									enc_cu->reco_image_temp[i_curr_depth],
									i_part_unit_idx ) ;
}

/** function for filling the pcm buffer of a cu using its original sample array
 * \param cu pointer to current cu
 * \param pc_org_yuv pointer to original sample array
 * \returns void
 */
void x265_enc_cu_x_fill_pcm_buffer ( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_data_cu_t *cu,
									x265_image_t* p_org_image )
{
	int32_t x = 0, y = 0 ;
	uint32_t i_width = 0 ;
	uint32_t i_height = 0 ;
	uint32_t i_width_c = 0 ;
	uint32_t i_height_c = 0 ;
	uint32_t i_src_stride = 0 ;
	uint32_t i_src_stride_c = 0 ;

	pixel *src_y = NULL ;
	pixel *dst_y = NULL ;
	pixel *src_cb = NULL ;
	pixel *dst_cb = NULL ;
	pixel *src_cr = NULL ;
	pixel *dst_cr = NULL ;

	i_width = x265_data_cu_get_width_p2 ( cu, 0 ) ;
	i_height = x265_data_cu_get_height_p2 ( cu, 0 ) ;

	src_y = x265_image_get_luma_addr_p4( h, p_org_image, 0, i_width ) ;
	dst_y = cu->ipcm_sample_y ;
	i_src_stride = p_org_image->i_stride[0];

	for( y = 0; y < i_height; y++ )
	{
		for( x = 0; x < i_width; x++ )
		{
			dst_y[x] = src_y[x];
		}
		dst_y += i_width;
		src_y += i_src_stride;
	}

	src_cb = x265_image_get_cb_addr_p2( h, p_org_image ) ;
	src_cr = x265_image_get_cr_addr_p2( h, p_org_image ) ;
	dst_cb = cu->ipcm_sample_cb ;
	dst_cr = cu->ipcm_sample_cr ;

	i_src_stride_c = p_org_image->i_stride[1];
	i_height_c = i_height >> 1;
	i_width_c = i_width  >> 1;

	for( y = 0; y < i_height_c; y++ )
	{
		for( x = 0; x < i_width_c; x++ )
		{
			dst_cb[x] = src_cb[x];
			dst_cr[x] = src_cr[x];
		}
		dst_cb += i_width_c;
		dst_cr += i_width_c;
		src_cb += i_src_stride_c;
		src_cr += i_src_stride_c;
	}
}



