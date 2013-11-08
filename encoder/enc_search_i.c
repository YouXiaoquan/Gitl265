

#include "common/common.h"





static const uint32_t d_filter[9] =
{
  0, 1, 0,
  2, 3, 2,
  0, 1, 0
};


#if X265_FAST_UDI_USE_MPM
const uint8_t intra_mode_num_fast[7] =
{
	3,  //   2x2
	8,  //   4x4
	8,  //   8x8
	3,  //  16x16
	3,  //  32x32
	3,  //  64x64
	3   // 128x128
};
#else // FAST_UDI_USE_MPM
const uint8_t intra_mode_num_fast[7] =
{
	3,  //   2x2
	9,  //   4x4
	9,  //   8x8
	4,  //  16x16   33
	4,  //  32x32   33
	5,  //  64x64   33
	4   // 128x128  33
};
#endif // FAST_UDI_USE_MPM


x265_enc_search_t *x265_enc_search_new (x265_t *h)
{
	x265_enc_search_t *enc_search = NULL ;

	CHECKED_MALLOCZERO( enc_search, sizeof(x265_enc_search_t) );
	if ( x265_enc_search_init ( h, enc_search ) )
	{
		goto fail ;
	}

	return enc_search ;
fail:
	x265_enc_search_delete ( h, (void*) enc_search ) ;
	return NULL ;
}

void x265_enc_search_delete ( x265_t *h, x265_enc_search_t *enc_search )
{
	x265_enc_search_deinit ( h, enc_search ) ;
	x265_free ( enc_search ) ;
}

int x265_enc_search_init ( x265_t *h, x265_enc_search_t *enc_search )
{
	if ( x265_prediction_init(h, (x265_prediction_t*) enc_search) )
	{
		goto fail ;
	}

	if ( x265_image_init (&enc_search->tmp_image_pred ) )
	{
		goto fail ;
	}
	if ( x265_image_init (&enc_search->qt_temp_transform_skip_image ) )
	{
		goto fail ;
	}

	enc_search->ppc_qt_temp_coeff_y  = NULL;
	enc_search->ppc_qt_temp_coeff_cb = NULL;
	enc_search->ppc_qt_temp_coeff_cr = NULL;
	enc_search->pc_qt_temp_coeff_y   = NULL;
	enc_search->pc_qt_temp_coeff_cb  = NULL;
	enc_search->pc_qt_temp_coeff_cr  = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	enc_search->ppc_qt_temp_arl_coeff_y  = NULL;
	enc_search->ppc_qt_temp_arl_coeff_cb = NULL;
	enc_search->ppc_qt_temp_arl_coeff_cr = NULL;
	enc_search->pc_qt_temp_arl_coeff_y   = NULL;
	enc_search->pc_qt_temp_arl_coeff_cb  = NULL;
	enc_search->pc_qt_temp_arl_coeff_cr  = NULL;
#endif
	enc_search->qt_temp_tr_idx   = NULL;
	enc_search->qt_temp_cbf[0] = NULL ;
	enc_search->qt_temp_cbf[1] = NULL ;
	enc_search->qt_temp_cbf[2] = NULL ;
	enc_search->ppc_qt_temp_image = NULL;
	enc_search->temp_pel = NULL;
	enc_search->shared_pred_transform_skip[0] = NULL ;
	enc_search->shared_pred_transform_skip[1] = NULL ;
	enc_search->shared_pred_transform_skip[2] = NULL ;
	enc_search->pc_qt_temp_tu_coeff_y   = NULL;
	enc_search->pc_qt_temp_tu_coeff_cb  = NULL;
	enc_search->pc_qt_temp_tu_coeff_cr  = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	enc_search->pc_qt_temp_tu_arl_coeff_y  = NULL;
	enc_search->pc_qt_temp_tu_arl_coeff_cb = NULL;
	enc_search->pc_qt_temp_tu_arl_coeff_cr = NULL;
#endif
	enc_search->qt_temp_transform_skip_flag[0] = NULL;
	enc_search->qt_temp_transform_skip_flag[1] = NULL;
	enc_search->qt_temp_transform_skip_flag[2] = NULL;
	x265_enc_search_set_wp_scaling_dist_param( h, enc_search,
												NULL, -1, REF_PIC_LIST_X );

	enc_search->b_has_init = 1  ;
    return 0 ;

fail:
	return -1 ;
}

void x265_enc_search_deinit ( x265_t *h, x265_enc_search_t *enc_search )
{
	x265_enc_search_deinitialize ( h, enc_search ) ;

	x265_image_deinit (&enc_search->tmp_image_pred ) ;
	x265_image_deinit (&enc_search->qt_temp_transform_skip_image ) ;
	x265_prediction_deinit((x265_prediction_t*) enc_search) ;
}


int x265_enc_search_initialize ( x265_t *h,
								x265_enc_search_t *enc_search,
								x265_tr_quant_t*  pc_tr_quant,
								int32_t i_search_range,
								int32_t i_bipred_search_range,
								int32_t i_fast_search,
								int32_t i_max_delta_qp,
								x265_enc_entropy_t* pc_entropy_coder,
								x265_rd_cost_t* pc_rd_cost,
								x265_enc_sbac_t*** pppc_rd_sbac_coder,
								x265_enc_sbac_t* pc_rd_go_on_sbac_coder
								)
{
	uint32_t loop = 0 ;
	int32_t i_dir = 0 ;
	int32_t i_ref_idx = 0 ;
	uint32_t i_num_partitions = 0 ;
	uint32_t i_num_layers_allocated = 0 ;

	enc_search->i_search_range         = i_search_range;
	enc_search->i_bipred_search_range    = i_bipred_search_range;
	enc_search->i_fast_search          = i_fast_search;
	enc_search->i_max_delta_qp          = i_max_delta_qp;

	for ( i_dir = 0; i_dir < 2; i_dir++)
	{
		for ( i_ref_idx = 0; i_ref_idx < 33; i_ref_idx++)
		{
			enc_search->adapt_sr[i_dir][i_ref_idx] = i_search_range;
		}
	}

	enc_search->d_filter = d_filter + 4;

	// initialize motion cost
#if !X265_FIX203
	rd_cost_init_rate_distortion_model( &h->rd_cost, enc_search->i_search_range << 2 );
#endif

	for( int32_t i_num = 0; i_num < X265_AMVP_MAX_NUM_CANDS+1; i_num++)
	{
		for( int32_t idx = 0; idx < X265_AMVP_MAX_NUM_CANDS; idx++)
		{
			if (idx < i_num)
			{
				enc_search->mvp_idx_cost[idx][i_num] = x265_enc_search_x_get_mvp_idx_bits ( enc_search,
																							idx, i_num ) ;
			}
			else
			{
				enc_search->mvp_idx_cost[idx][i_num] = X265_MAX_INT ;
			}
		}
	}

	if ( x265_prediction_init_temp_buff( h, (x265_prediction_t*)enc_search) )
	{
		goto fail ;
	}

	CHECKED_MALLOCZERO(enc_search->temp_pel , h->cu.pic.i_max_cu_width*h->cu.pic.i_max_cu_height*sizeof(pixel));

	i_num_layers_allocated = h->param.sps.i_quadtree_tu_log2_max_size
							- h->param.sps.i_quadtree_tu_log2_min_size + 1 ;
	CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_coeff_y,
						i_num_layers_allocated * sizeof(x265_coeff_t*));
	CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_coeff_cb,
						i_num_layers_allocated * sizeof(x265_coeff_t*));
	CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_coeff_cr,
						i_num_layers_allocated * sizeof(x265_coeff_t*));
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_coeff_y,
						h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height
						* sizeof(x265_coeff_t ));
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_coeff_cb,
						(h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height >> 2)
						* sizeof(x265_coeff_t ));
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_coeff_cr,
						(h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height >> 2)
						* sizeof(x265_coeff_t ));
#if X265_ADAPTIVE_QP_SELECTION
	CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_arl_coeff_y,
						i_num_layers_allocated * sizeof(int32_t*));
	CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_arl_coeff_cb,
						i_num_layers_allocated * sizeof(int32_t*));
	CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_arl_coeff_cr,
						i_num_layers_allocated * sizeof(int32_t*));
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_arl_coeff_y,
						h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height
						* sizeof(int32_t ));
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_arl_coeff_cb,
						(h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height >> 2)
						* sizeof(int32_t ));
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_arl_coeff_cr,
						(h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height >> 2)
						* sizeof(int32_t ));
#endif

	i_num_partitions = 1<<(h->cu.pic.i_total_depth<<1);
	CHECKED_MALLOCZERO(enc_search->qt_temp_tr_idx,
						i_num_partitions * sizeof(uint8_t)) ;
	CHECKED_MALLOCZERO(enc_search->qt_temp_cbf[0],
						i_num_partitions * sizeof(uint8_t)) ;
	CHECKED_MALLOCZERO(enc_search->qt_temp_cbf[1],
						i_num_partitions * sizeof(uint8_t)) ;
	CHECKED_MALLOCZERO(enc_search->qt_temp_cbf[2],
						i_num_partitions * sizeof(uint8_t)) ;
	CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_image,
						i_num_layers_allocated * sizeof(x265_image_t*)) ;
	CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_simage,
						i_num_layers_allocated * sizeof(x265_simage_t*)) ;
	for( loop = 0; loop < i_num_layers_allocated; ++loop )
	{
		CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_coeff_y[loop],
							h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height
							* sizeof(x265_coeff_t));
		CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_coeff_cb[loop],
							(h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height >> 2)
							* sizeof(x265_coeff_t));
		CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_coeff_cr[loop],
							(h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height >> 2)
							* sizeof(x265_coeff_t));
#if X265_ADAPTIVE_QP_SELECTION
		CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_arl_coeff_y[loop],
							h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height
							* sizeof(int32_t));
		CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_arl_coeff_cb[loop],
							(h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height >> 2)
							* sizeof(int32_t));
		CHECKED_MALLOCZERO(enc_search->ppc_qt_temp_arl_coeff_cr[loop],
							(h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height >> 2)
							* sizeof(int32_t));
#endif
		enc_search->ppc_qt_temp_image[loop] = x265_image_new () ;
		if ( NULL == enc_search->ppc_qt_temp_image[loop] )
		{
			goto fail ;
		}
		if ( x265_image_create ( enc_search->ppc_qt_temp_image[loop],
		                         h->cu.pic.i_max_cu_width,
		                         h->cu.pic.i_max_cu_height ) )
		{
			goto fail ;
		}
		enc_search->ppc_qt_temp_simage[loop] = x265_simage_new () ;
		if ( NULL == enc_search->ppc_qt_temp_simage[loop] )
		{
			goto fail ;
		}
		if ( x265_simage_create ( enc_search->ppc_qt_temp_simage[loop],
		                         h->cu.pic.i_max_cu_width,
		                         h->cu.pic.i_max_cu_height ) )
		{
			goto fail ;
		}
	}
	CHECKED_MALLOCZERO(enc_search->shared_pred_transform_skip[0],
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(pixel)) ;
	CHECKED_MALLOCZERO(enc_search->shared_pred_transform_skip[1],
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(pixel)) ;
	CHECKED_MALLOCZERO(enc_search->shared_pred_transform_skip[2],
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(pixel)) ;
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_tu_coeff_y,
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(x265_coeff_t)) ;
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_tu_coeff_cb,
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(x265_coeff_t)) ;
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_tu_coeff_cr,
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(x265_coeff_t)) ;
#if X265_ADAPTIVE_QP_SELECTION
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_tu_arl_coeff_y,
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(int32_t)) ;
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_tu_arl_coeff_cb,
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(int32_t)) ;
	CHECKED_MALLOCZERO(enc_search->pc_qt_temp_tu_arl_coeff_cr,
						X265_MAX_TS_WIDTH * X265_MAX_TS_HEIGHT * sizeof(int32_t)) ;
#endif

	if ( x265_image_create ( &enc_search->qt_temp_transform_skip_image,
	                         h->cu.pic.i_max_cu_width,
	                         h->cu.pic.i_max_cu_height ) )
	{
		goto fail ;
	}
	CHECKED_MALLOCZERO(enc_search->qt_temp_transform_skip_flag[0],
						i_num_partitions * sizeof(uint8_t)) ;
	CHECKED_MALLOCZERO(enc_search->qt_temp_transform_skip_flag[1],
						i_num_partitions * sizeof(uint8_t)) ;
	CHECKED_MALLOCZERO(enc_search->qt_temp_transform_skip_flag[2],
						i_num_partitions * sizeof(uint8_t)) ;
	if ( x265_image_create ( &enc_search->tmp_image_pred, X265_MAX_CU_SIZE, X265_MAX_CU_SIZE ) )
	{
		goto fail ;
	}

	enc_search->b_has_init = 1 ;

	return 0 ;
fail:
	x265_enc_search_deinitialize ( h, enc_search ) ;
	return -1 ;

}

void x265_enc_search_deinitialize ( x265_t *h, x265_enc_search_t *enc_search )
{
	uint32_t loop = 0 ;
	uint32_t i_num_layers_allocated = 0 ;

	if ( enc_search->b_has_init )
	{
		if ( enc_search->temp_pel )
		{
			x265_free ( enc_search->temp_pel ) ;
			enc_search->temp_pel = NULL;
		}

		i_num_layers_allocated = h->param.sps.i_quadtree_tu_log2_max_size
								- h->param.sps.i_quadtree_tu_log2_min_size + 1 ;

		if ( enc_search->ppc_qt_temp_coeff_y )
		{
			for( loop = 0 ; loop < i_num_layers_allocated ; ++ loop )
			{
				x265_free ( enc_search->ppc_qt_temp_coeff_y[loop] ) ;
				enc_search->ppc_qt_temp_coeff_y[loop] = NULL ;
			}
			x265_free ( enc_search->ppc_qt_temp_coeff_y ) ;
			enc_search->ppc_qt_temp_coeff_y = NULL ;
		}
		if ( enc_search->ppc_qt_temp_coeff_cb )
		{
			for( loop = 0 ; loop < i_num_layers_allocated ; ++ loop )
			{
				x265_free ( enc_search->ppc_qt_temp_coeff_cb[loop] ) ;
				enc_search->ppc_qt_temp_coeff_cb[loop] = NULL ;
			}
			x265_free ( enc_search->ppc_qt_temp_coeff_cb ) ;
			enc_search->ppc_qt_temp_coeff_cb = NULL ;
		}
		if ( enc_search->ppc_qt_temp_coeff_cr )
		{
			for( loop = 0 ; loop < i_num_layers_allocated ; ++ loop )
			{
				x265_free ( enc_search->ppc_qt_temp_coeff_cr[loop] ) ;
				enc_search->ppc_qt_temp_coeff_cr[loop] = NULL ;
			}
			x265_free ( enc_search->ppc_qt_temp_coeff_cr ) ;
			enc_search->ppc_qt_temp_coeff_cr = NULL ;
		}
		if ( enc_search->ppc_qt_temp_arl_coeff_y )
		{
			for( loop = 0 ; loop < i_num_layers_allocated ; ++ loop )
			{
				x265_free ( enc_search->ppc_qt_temp_arl_coeff_y[loop] ) ;
				enc_search->ppc_qt_temp_arl_coeff_y[loop] = NULL ;
			}
			x265_free ( enc_search->ppc_qt_temp_arl_coeff_y ) ;
			enc_search->ppc_qt_temp_arl_coeff_y = NULL ;
		}
		if ( enc_search->ppc_qt_temp_arl_coeff_cb )
		{
			for( loop = 0 ; loop < i_num_layers_allocated ; ++ loop )
			{
				x265_free ( enc_search->ppc_qt_temp_arl_coeff_cb[loop] ) ;
				enc_search->ppc_qt_temp_arl_coeff_cb[loop] = NULL ;
			}
			x265_free ( enc_search->ppc_qt_temp_arl_coeff_cb ) ;
			enc_search->ppc_qt_temp_arl_coeff_cb = NULL ;
		}
		if ( enc_search->ppc_qt_temp_arl_coeff_cr )
		{
			for( loop = 0 ; loop < i_num_layers_allocated ; ++ loop )
			{
				x265_free ( enc_search->ppc_qt_temp_arl_coeff_cr[loop] ) ;
				enc_search->ppc_qt_temp_arl_coeff_cr[loop] = NULL ;
			}
			x265_free ( enc_search->ppc_qt_temp_arl_coeff_cr ) ;
			enc_search->ppc_qt_temp_arl_coeff_cr = NULL ;
		}

		if ( enc_search->ppc_qt_temp_image )
		{
			for( loop = 0 ; loop < i_num_layers_allocated ; ++ loop )
			{
				if ( enc_search->ppc_qt_temp_image[loop] )
				{
					x265_image_destroy ( enc_search->ppc_qt_temp_image[loop] ) ;
					x265_image_delete ( enc_search->ppc_qt_temp_image[loop] ) ;
					enc_search->ppc_qt_temp_image[loop] = NULL ;
				}
			}
			x265_free ( enc_search->ppc_qt_temp_image ) ;
			enc_search->ppc_qt_temp_image = NULL ;
		}
		if ( enc_search->ppc_qt_temp_simage )
		{
			for( loop = 0 ; loop < i_num_layers_allocated ; ++ loop )
			{
				if ( enc_search->ppc_qt_temp_simage[loop] )
				{
					x265_simage_destroy ( enc_search->ppc_qt_temp_simage[loop] ) ;
					x265_simage_delete ( enc_search->ppc_qt_temp_simage[loop] ) ;
					enc_search->ppc_qt_temp_simage[loop] = NULL ;
				}
			}
			x265_free ( enc_search->ppc_qt_temp_simage ) ;
			enc_search->ppc_qt_temp_simage = NULL ;
		}
		x265_free ( enc_search->pc_qt_temp_coeff_y ) ;
		x265_free ( enc_search->pc_qt_temp_coeff_cb ) ;
		x265_free ( enc_search->pc_qt_temp_coeff_cr ) ;
#if X265_ADAPTIVE_QP_SELECTION
		x265_free ( enc_search->pc_qt_temp_arl_coeff_y ) ;
		x265_free ( enc_search->pc_qt_temp_arl_coeff_cb ) ;
		x265_free ( enc_search->pc_qt_temp_arl_coeff_cr ) ;
#endif
		x265_free ( enc_search->qt_temp_tr_idx ) ;
		x265_free ( enc_search->qt_temp_cbf[0] ) ;
		x265_free ( enc_search->qt_temp_cbf[1] ) ;
		x265_free ( enc_search->qt_temp_cbf[2] ) ;
		x265_free ( enc_search->shared_pred_transform_skip[0] ) ;
		x265_free ( enc_search->shared_pred_transform_skip[1] ) ;
		x265_free ( enc_search->shared_pred_transform_skip[2] ) ;
		x265_free ( enc_search->pc_qt_temp_tu_coeff_y ) ;
		x265_free ( enc_search->pc_qt_temp_tu_coeff_cb ) ;
		x265_free ( enc_search->pc_qt_temp_tu_coeff_cr ) ;
#if X265_ADAPTIVE_QP_SELECTION
		x265_free ( enc_search->pc_qt_temp_tu_arl_coeff_y ) ;
		x265_free ( enc_search->pc_qt_temp_tu_arl_coeff_cb ) ;
		x265_free ( enc_search->pc_qt_temp_tu_arl_coeff_cr ) ;
#endif
		x265_free ( enc_search->qt_temp_transform_skip_flag[0] ) ;
		x265_free ( enc_search->qt_temp_transform_skip_flag[1] ) ;
		x265_free ( enc_search->qt_temp_transform_skip_flag[2] ) ;
		x265_image_destroy ( &enc_search->qt_temp_transform_skip_image ) ;
		x265_image_destroy ( &enc_search->tmp_image_pred ) ;

		enc_search->pc_qt_temp_coeff_y   = NULL;
		enc_search->pc_qt_temp_coeff_cb  = NULL;
		enc_search->pc_qt_temp_coeff_cr  = NULL;
#if X265_ADAPTIVE_QP_SELECTION
		enc_search->pc_qt_temp_arl_coeff_y   = NULL;
		enc_search->pc_qt_temp_arl_coeff_cb  = NULL;
		enc_search->pc_qt_temp_arl_coeff_cr  = NULL;
#endif
		enc_search->qt_temp_tr_idx   = NULL;
		enc_search->qt_temp_cbf[0] = NULL ;
		enc_search->qt_temp_cbf[1] = NULL ;
		enc_search->qt_temp_cbf[2] = NULL ;
		enc_search->shared_pred_transform_skip[0] = NULL ;
		enc_search->shared_pred_transform_skip[1] = NULL ;
		enc_search->shared_pred_transform_skip[2] = NULL ;
		enc_search->pc_qt_temp_tu_coeff_y   = NULL;
		enc_search->pc_qt_temp_tu_coeff_cb  = NULL;
		enc_search->pc_qt_temp_tu_coeff_cr  = NULL;
	#if X265_ADAPTIVE_QP_SELECTION
		enc_search->pc_qt_temp_tu_arl_coeff_y  = NULL;
		enc_search->pc_qt_temp_tu_arl_coeff_cb = NULL;
		enc_search->pc_qt_temp_tu_arl_coeff_cr = NULL;
	#endif
		enc_search->qt_temp_transform_skip_flag[0] = NULL;
		enc_search->qt_temp_transform_skip_flag[1] = NULL;
		enc_search->qt_temp_transform_skip_flag[2] = NULL;

		enc_search->b_has_init = 0 ;
	}
}

void x265_enc_search_x_enc_sub_div_cbf_qt(x265_t* h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										uint32_t i_tr_depth,
										uint32_t i_abs_part_idx,
										int32_t b_luma,
										int32_t b_chroma )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_sub_div = 0;
	uint32_t i_log2_trafo_size = 0;
	uint32_t i_q_part_num = 0;
	uint32_t i_part = 0;

	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth;
	i_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );
	i_sub_div = ( i_tr_mode > i_tr_depth ? 1 : 0 );
	i_log2_trafo_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width] + 2 - i_full_depth;

	{
		if( x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, 0) == MODE_INTRA
			&& x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_NxN && i_tr_depth == 0 )
		{
			assert( i_sub_div );
		}
		else if( i_log2_trafo_size > h->sps[0].i_quadtree_tu_log2_max_size )
		{
			assert( i_sub_div );
		}
		else if( i_log2_trafo_size == h->sps[0].i_quadtree_tu_log2_min_size )
		{
			assert( !i_sub_div );
		}
		else if( i_log2_trafo_size == x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx) )
		{
			assert( !i_sub_div );
		}
		else
		{
			assert( i_log2_trafo_size > x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx) );
			if( b_luma )
			{
				x265_enc_entropy_encode_transform_sub_div_flag(h,
																&h->enc_entropy,
																i_sub_div,
																5 - i_log2_trafo_size );
			}
		}
	}

	if ( b_chroma )
	{
		if( i_log2_trafo_size > 2 )
		{
			if( i_tr_depth == 0
				|| x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_tr_depth - 1))
			{
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_CHROMA_U,
												i_tr_depth );
			}
			if( i_tr_depth == 0
				|| x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_tr_depth-1 ) )
			{
				x265_enc_entropy_encode_qt_cbf(h,
												&h->enc_entropy,
												cu,
												i_abs_part_idx,
												TEXT_CHROMA_V,
												i_tr_depth );
			}
		}
	}

	if( i_sub_div )
	{
		i_q_part_num = h->cu.pic.i_num_partitions >> ( ( i_full_depth + 1 ) << 1 );
		for( i_part = 0; i_part < 4; i_part++ )
		{
			x265_enc_search_x_enc_sub_div_cbf_qt(h,
												enc_search,
												cu,
												i_tr_depth + 1,
												i_abs_part_idx + i_part * i_q_part_num,
												b_luma,
												b_chroma );
		}
		return;
	}

	{
		//===== cbfs =====
		if( b_luma )
		{
			x265_enc_entropy_encode_qt_cbf(h,
											&h->enc_entropy,
											cu,
											i_abs_part_idx,
											TEXT_LUMA,
											i_tr_mode );
		}
	}
}

void x265_enc_search_x_enc_coeff_qt(x265_t *h,
									x265_enc_search_t *enc_search,
									x265_data_cu_t *cu,
									uint32_t i_tr_depth,
									uint32_t i_abs_part_idx,
									enum text_type_e i_text_type,
									int32_t b_real_coeff )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_sub_div = 0;
	uint32_t i_log2_trafo_size = 0;
	uint32_t i_q_part_num = 0;
	uint32_t i_part = 0;
	uint32_t i_chroma = 0;
	uint32_t i_qp_div = 0;
	int32_t b_first_q = 0;
	//===== coefficients =====
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_coeff_offset = 0;
	uint32_t i_qt_layer = 0;
	x265_coeff_t *coeff = NULL;

	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth;
	i_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );
	i_sub_div = ( i_tr_mode > i_tr_depth ? 1 : 0 );
	i_log2_trafo_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width] + 2 - i_full_depth;
	i_chroma   = ( i_text_type != TEXT_LUMA ? 1 : 0 );

	if( i_sub_div )
	{
		i_q_part_num = h->cu.pic.i_num_partitions >> ( ( i_full_depth + 1 ) << 1 );
		for( i_part = 0; i_part < 4; i_part++ )
		{
			x265_enc_search_x_enc_coeff_qt(h,
											enc_search,
											cu,
											i_tr_depth + 1,
											i_abs_part_idx + i_part * i_q_part_num,
											i_text_type,
											b_real_coeff );
		}
		return;
	}

	if( i_text_type != TEXT_LUMA && i_log2_trafo_size == 2 )
	{
		assert( i_tr_depth > 0 );
		i_tr_depth--;
		i_qp_div = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 ) + i_tr_depth) << 1);
		b_first_q = ((i_abs_part_idx % i_qp_div) == 0);
		if( !b_first_q )
		{
			return;
		}
	}

	//===== coefficients =====
	i_width = x265_data_cu_get_width_p2(cu, 0) >> ( i_tr_depth + i_chroma );
	i_height = x265_data_cu_get_height_p2(cu, 0) >> ( i_tr_depth + i_chroma );
	i_coeff_offset = (h->cu.pic.i_min_cu_width * h->cu.pic.i_min_cu_height * i_abs_part_idx ) >> ( i_chroma << 1 );
	i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_trafo_size;
	switch( i_text_type )
	{
	case TEXT_LUMA:
		coeff = ( b_real_coeff ? x265_data_cu_get_coeff_y(cu)
								: enc_search->ppc_qt_temp_coeff_y[i_qt_layer] );
		break;
	case TEXT_CHROMA_U:
		coeff = ( b_real_coeff ? x265_data_cu_get_coeff_cb(cu)
							: enc_search->ppc_qt_temp_coeff_cb[i_qt_layer] );
		break;
	case TEXT_CHROMA_V:
		coeff = ( b_real_coeff ? x265_data_cu_get_coeff_cr(cu)
							: enc_search->ppc_qt_temp_coeff_cr[i_qt_layer] );
		break;
	default:  assert(0);
		break;
	}
	coeff += i_coeff_offset;

	x265_enc_entropy_encode_coeff_nxn(h,
									&h->enc_entropy,
									cu,
									coeff,
									i_abs_part_idx,
									i_width,
									i_height,
									i_full_depth,
									i_text_type );
}


void x265_enc_search_x_enc_intra_header(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										uint32_t i_tr_depth,
										uint32_t i_abs_part_idx,
										int32_t b_luma,
										int32_t b_chroma )
{
	uint32_t i_q_num_parts = 0;
	uint32_t i_part = 0;

	if( b_luma )
	{
		// cu header
		if( i_abs_part_idx == 0 )
		{
			if(!(I_SLICE == h->slice->i_slice_type))
			{
				if (h->pps[0].b_transquant_bypass_enable_flag)
				{
					x265_enc_entropy_encode_cu_transquant_bypass_flag(&h->enc_entropy,
																		cu,
																		0,
																		1);
				}
				x265_enc_entropy_encode_skip_flag(h, &h->enc_entropy, cu, 0, 1);
				x265_enc_entropy_encode_pred_mode(h, &h->enc_entropy, cu, 0, 1);
			}

			x265_enc_entropy_encode_part_size(h,
											&h->enc_entropy,
											cu,
											0,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0), 1 );

			if (x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, 0)
					&& x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_2Nx2N )
			{
				x265_enc_entropy_encode_ipcm_info(h, &h->enc_entropy, cu, 0, 1 );

				if ( x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu, 0))
				{
					return;
				}
			}
		}
		// luma prediction mode
		if( x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_2Nx2N )
		{
			if( i_abs_part_idx == 0 )
			{
				x265_enc_entropy_encode_intra_dir_mode_luma(h,
															&h->enc_entropy,
															cu,
															0,
															0);
			}
		}
		else
		{
			i_q_num_parts = x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu) >> 2;
			if( i_tr_depth == 0 )
			{
				assert( i_abs_part_idx == 0 );
				for( i_part = 0; i_part < 4; i_part++ )
				{
					x265_enc_entropy_encode_intra_dir_mode_luma(h,
																&h->enc_entropy,
																cu,
																i_part * i_q_num_parts,
																0);
				}
			}
			else if( ( i_abs_part_idx % i_q_num_parts ) == 0 )
			{
				x265_enc_entropy_encode_intra_dir_mode_luma(h,
															&h->enc_entropy,
															cu,
															i_abs_part_idx,
															0);
			}
		}
	}
	if( b_chroma )
	{
		// chroma prediction mode
		if( i_abs_part_idx == 0 )
		{
			x265_enc_entropy_encode_intra_dir_mode_chroma(&h->enc_entropy, cu, 0, 1 );
		}
	}
}


uint32_t x265_enc_search_x_get_intra_bits_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma,
											int32_t b_chroma,
											int32_t b_real_coeff /* just for test */ )
{
	uint32_t i_bits = 0;

	x265_enc_entropy_reset_bits(&h->enc_entropy);
	x265_enc_search_x_enc_intra_header(h,
									enc_search,
									cu,
									i_tr_depth,
									i_abs_part_idx,
									b_luma,
									b_chroma );
	x265_enc_search_x_enc_sub_div_cbf_qt(h,
										enc_search,
										cu,
										i_tr_depth,
										i_abs_part_idx,
										b_luma,
										b_chroma );

	if( b_luma )
	{
		x265_enc_search_x_enc_coeff_qt(h,
										enc_search,
										cu,
										i_tr_depth,
										i_abs_part_idx,
										TEXT_LUMA,
										b_real_coeff );
	}
	if( b_chroma )
	{
		x265_enc_search_x_enc_coeff_qt(h,
										enc_search,
										cu,
										i_tr_depth,
										i_abs_part_idx,
										TEXT_CHROMA_U,
										b_real_coeff );
		x265_enc_search_x_enc_coeff_qt(h,
										enc_search,
										cu,
										i_tr_depth,
										i_abs_part_idx,
										TEXT_CHROMA_V,
										b_real_coeff );
	}
	i_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
	return i_bits;
}


uint32_t x265_enc_search_x_get_intra_bits_qt_chroma(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													uint32_t i_tr_depth,
													uint32_t i_abs_part_idx,
													uint32_t i_chroma_id,
													int32_t b_real_coeff /* just for test */ )
{
	uint32_t i_bits = 0;

	x265_enc_entropy_reset_bits(&h->enc_entropy);
	if( i_chroma_id == TEXT_CHROMA_U)
	{
		x265_enc_search_x_enc_coeff_qt(h,
										enc_search,
										cu,
										i_tr_depth,
										i_abs_part_idx,
										TEXT_CHROMA_U,
										b_real_coeff );
	}
	else if(i_chroma_id == TEXT_CHROMA_V)
	{
		x265_enc_search_x_enc_coeff_qt(h,
										enc_search,
										cu,
										i_tr_depth,
										i_abs_part_idx,
										TEXT_CHROMA_V,
										b_real_coeff );
	}

	i_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
	return i_bits;
}


void x265_enc_search_x_intra_coding_luma_blk(x265_t* h,
											x265_enc_search_t* enc_search,
											x265_data_cu_t* cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											x265_image_t *p_org_image,
											x265_image_t *p_pred_image,
											x265_simage_t *p_resi_image,
											uint32_t *p_dist,
											int32_t b_default0_save1_load2 )
{
	uint32_t i_luma_pred_mode = 0;
	uint32_t i_full_depth = 0;
	uint32_t i_width = 0;
	uint32_t i_height= 0;
	uint32_t i_stride= 0;
	pixel *org = NULL;
	pixel *pred = NULL;
	spixel *resi = NULL;
	pixel *reco = NULL;

	uint32_t i_log2_tr_size = 0;
	uint32_t i_qt_layer    = 0;
	uint32_t i_num_coeff_per_inc = 0;
	x265_coeff_t *coeff = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff = NULL;
#endif
	pixel *rec_qt = NULL;
	uint32_t i_rec_qt_stride = 0;
	uint32_t i_zorder = 0;
	pixel *rec_ipred = NULL;
	uint32_t i_rec_ipred_stride = 0;
	int32_t b_use_transform_skip = 0;
	int32_t b_above_avail = 0;
	int32_t b_left_avail  = 0;
	pixel *p_pred = NULL;
	pixel *p_pred_buf = NULL;
	int32_t k = 0;
	uint32_t i_y = 0;
	uint32_t i_x = 0;
	pixel *p_org = NULL;
	spixel *p_resi = NULL;
	pixel *p_reco = NULL;
	pixel *p_rec_qt = NULL;
	pixel *p_rec_ipred = NULL;
	uint32_t i_abs_sum = 0;
	int32_t i_scaling_list_type = 0 ;

	i_luma_pred_mode = x265_base_data_cu_get_luma_intra_dir_p2((x265_base_data_cu_t*)cu,  i_abs_part_idx );
	i_full_depth  = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 )  + i_tr_depth;
	i_width = x265_data_cu_get_width_p2(cu,  0 ) >> i_tr_depth;
	i_height= x265_data_cu_get_height_p2(cu,  0 ) >> i_tr_depth;
	i_stride = x265_image_get_stride(p_org_image);
	org = x265_image_get_luma_addr_p3(h, p_org_image, i_abs_part_idx );
	pred = x265_image_get_luma_addr_p3(h, p_pred_image, i_abs_part_idx );
	resi = x265_simage_get_luma_addr_p3(h, p_resi_image, i_abs_part_idx );
	reco = x265_image_get_luma_addr_p3(h, p_pred_image, i_abs_part_idx );

	i_log2_tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> i_full_depth ] + 2;
	i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
	i_num_coeff_per_inc = h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height >> ( h->sps[0].i_max_cu_depth << 1 );
	coeff = enc_search->ppc_qt_temp_coeff_y[ i_qt_layer ] + i_num_coeff_per_inc * i_abs_part_idx;
#if X265_ADAPTIVE_QP_SELECTION
	arl_coeff = enc_search->ppc_qt_temp_arl_coeff_y[ i_qt_layer ] + i_num_coeff_per_inc * i_abs_part_idx;
#endif
	rec_qt = x265_image_get_luma_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
	i_rec_qt_stride = x265_image_get_stride(enc_search->ppc_qt_temp_image[ i_qt_layer ]);

	i_zorder = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu ) + i_abs_part_idx;
	rec_ipred = x265_image_get_luma_addr_p3(h, h->image_intra, i_zorder );;
	i_rec_ipred_stride = x265_image_get_stride(h->image_intra);
	b_use_transform_skip  = x265_data_cu_get_transform_skip_p3(cu, i_abs_part_idx, TEXT_LUMA);


	if(b_default0_save1_load2 != 2)
	{
		x265_pattern_init_pattern_p5(h,
									x265_data_cu_get_pattern(cu),
									cu,
									i_tr_depth,
									i_abs_part_idx );
		x265_pattern_init_adi_pattern(h,
									x265_data_cu_get_pattern(cu),
									cu,
									h->image_intra,
									i_abs_part_idx,
									i_tr_depth,
									enc_search->prediction.image_ext,
									enc_search->prediction.i_image_ext_stride,
									enc_search->prediction.i_image_ext_height,
									&b_above_avail,
									&b_left_avail,
									0 );
		//===== get prediction signal =====
		x265_prediction_pred_intra_luma_ang(h,
											(x265_prediction_t*)enc_search,
											x265_data_cu_get_pattern(cu),
											i_luma_pred_mode,
											pred,
											i_stride,
											i_width,
											i_height,
											b_above_avail,
											b_left_avail );
		// save prediction
		if(b_default0_save1_load2 == 1)
		{
			p_pred = pred;
			p_pred_buf = enc_search->shared_pred_transform_skip[0];
			k = 0;
			for( i_y = 0; i_y < i_height; i_y++ )
			{
				for( i_x = 0; i_x < i_width; i_x++ )
				{
					p_pred_buf[ k ++ ] = p_pred[ i_x ];
				}
				p_pred += i_stride;
			}
		}
	}
	else
	{
		// load prediction
		p_pred = pred;
		p_pred_buf = enc_search->shared_pred_transform_skip[0];
		k = 0;
		for( i_y = 0; i_y < i_height; i_y++ )
		{
			for( i_x = 0; i_x < i_width; i_x++ )
			{
				p_pred[ i_x ] = p_pred_buf[ k ++ ];
			}
			p_pred += i_stride;
		}
	}

	//===== get residual signal =====

	// get residual
	p_org = org;
	p_pred = pred;
	p_resi = resi;
	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			p_resi[ i_x ] = p_org[ i_x ] - p_pred[ i_x ];
		}
		p_org  += i_stride;
		p_resi += i_stride;
		p_pred += i_stride;
	}


	//===== init availability pattern =====
	//===== transform and quantization =====
	//--- init rate estimation arrays for rdoq ---
	if( b_use_transform_skip ?
			h->param.rdo.b_use_rdo_q_ts :
			h->param.rdo.b_use_rdo_q )
	{
		x265_enc_entropy_estimate_bit(h,
									&h->enc_entropy,
									h->tr_quant.est_bits_sbac,
									i_width,
									i_width,
									TEXT_LUMA );
	}

	//--- transform and quantization ---
	x265_base_data_cu_set_tr_idx_sub_parts(h,
											(x265_base_data_cu_t*)cu,
											i_tr_depth,
											i_abs_part_idx,
											i_full_depth );
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
								resi,
								i_stride,
								coeff,
#if X265_ADAPTIVE_QP_SELECTION
								&arl_coeff,
#endif
								i_width,
								i_height,
								&i_abs_sum,
								TEXT_LUMA,
								i_abs_part_idx,
								b_use_transform_skip );

	//--- set coded block flag ---
	x265_data_cu_set_cbf_sub_parts_p6(h,
									cu,
									( i_abs_sum ? 1 : 0 ) << i_tr_depth,
									TEXT_LUMA,
									i_abs_part_idx,
									i_full_depth );
	//--- inverse transform ---
	if( i_abs_sum )
	{
		i_scaling_list_type = 0 + et_table[(int32_t)TEXT_LUMA];
		assert(i_scaling_list_type < 6);
		x265_tr_quant_inv_transform_nxn(h,
										&h->tr_quant,
										x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
										TEXT_LUMA,
										x265_base_data_cu_get_luma_intra_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ),
										resi,
										i_stride,
										coeff,
										i_width,
										i_height,
										i_scaling_list_type,
										b_use_transform_skip );
	}
	else
	{
		p_resi = resi;
		memset( coeff, 0, sizeof( x265_coeff_t ) * i_width * i_height );
		for( i_y = 0; i_y < i_height; i_y++ )
		{
			memset( p_resi, 0, sizeof( spixel ) * i_width );
			p_resi += i_stride;
		}
	}


	//===== reconstruction =====
	p_pred = pred;
	p_resi = resi;
	p_reco = reco;
	p_rec_qt = rec_qt;
	p_rec_ipred  = rec_ipred;
	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			p_reco [ i_x ] = x265_clip_y( h, p_pred[ i_x ] + p_resi[ i_x ] );
			p_rec_qt [ i_x ] = p_reco[ i_x ];
			p_rec_ipred[ i_x ] = p_reco[ i_x ];
		}
		p_pred += i_stride;
		p_resi += i_stride;
		p_reco += i_stride;
		p_rec_qt += i_rec_qt_stride;
		p_rec_ipred += i_rec_ipred_stride;
	}

	//===== update distortion =====
	*p_dist += x265_rd_cost_get_dist_part(h,
										&h->rd_cost,
										h->cu.pic.i_bit_depth_y,
										reco,
										i_stride,
										org,
										i_stride,
										i_width,
										i_height,
										TEXT_LUMA,
										DF_SSE );



	//	print_int_state ( *p_dist ) ;
}

void x265_enc_search_x_intra_coding_chroma_blk( x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												uint32_t i_tr_depth,
												uint32_t i_abs_part_idx,
												x265_image_t *p_org_image,
												x265_image_t *p_pred_image,
												x265_simage_t *p_resi_image,
												uint32_t *p_dist,
												uint32_t i_chroma_id,
												int32_t b_default0_save1_load2 )
{
	uint32_t i_org_tr_depth = 0;
	uint32_t i_full_depth = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_qp_div = 0;
	int32_t b_first_q = 0;
	enum text_type_e  i_text_type = 0;
	uint32_t i_chroma_pred_mode = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_stride = 0;
	pixel *org = NULL;
	pixel *pred = NULL;
	spixel *resi = NULL;
	pixel *reco = NULL;
	uint32_t i_qt_layer = 0;
	uint32_t i_num_coeff_per_inc  = 0;
	x265_coeff_t *coeff = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff = NULL;
#endif
	pixel *rec_qt = NULL;
	uint32_t i_rec_qt_stride = 0;
	uint32_t i_zorder= 0;
	pixel* rec_ipred = 0;
	uint32_t i_rec_ipred_stride = 0;
	int32_t b_use_transform_skip_chroma = 0;
	int32_t b_above_avail = 0;
	int32_t b_left_avail  = 0;
	pixel *p_pat_chroma = NULL;
	pixel *p_pred = NULL;
	pixel *p_pred_buf = NULL;
	int32_t k = 0;
	uint32_t i_y = 0;
	uint32_t i_x = 0;
	pixel *p_org = NULL;
	spixel *p_resi = NULL;
	uint32_t i_abs_sum = 0;
	int32_t i_cur_chroma_qp_offset = 0;
	int32_t i_scaling_list_type = 0;
	pixel *p_reco = NULL;
	pixel *p_rec_qt = NULL;
	pixel *p_rec_ipred = NULL;

	i_org_tr_depth = i_tr_depth;
	i_full_depth  = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_tr_depth;
	i_log2_tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> i_full_depth] + 2;
	if( i_log2_tr_size == 2 )
	{
		assert( i_tr_depth > 0 );
		i_tr_depth--;
		i_qp_div = h->cu.pic.i_num_partitions
					>> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_tr_depth ) << 1);
		b_first_q = ( ( i_abs_part_idx % i_qp_div ) == 0 );
		if( !b_first_q )
		{
			return;
		}
	}

	i_text_type = (i_chroma_id > 0 ? TEXT_CHROMA_V : TEXT_CHROMA_U);
	i_chroma_pred_mode = x265_data_cu_get_chroma_intra_dir_p2(cu,  i_abs_part_idx );
	i_width = x265_data_cu_get_width_p2(cu, 0 ) >> ( i_tr_depth + 1 );
	i_height= x265_data_cu_get_height_p2(cu, 0 ) >> ( i_tr_depth + 1 );
	i_stride = x265_image_get_c_stride (p_org_image);
	org = ( i_chroma_id > 0 ? x265_image_get_cr_addr_p3(h, p_org_image, i_abs_part_idx ) : x265_image_get_cb_addr_p3(h, p_org_image, i_abs_part_idx ) );
	pred = ( i_chroma_id > 0 ? x265_image_get_cr_addr_p3(h, p_pred_image, i_abs_part_idx ) : x265_image_get_cb_addr_p3(h, p_pred_image, i_abs_part_idx ) );
	resi = ( i_chroma_id > 0 ? x265_simage_get_cr_addr_p3(h, p_resi_image, i_abs_part_idx ) : x265_simage_get_cb_addr_p3(h, p_resi_image, i_abs_part_idx ) );
	reco = ( i_chroma_id > 0 ? x265_image_get_cr_addr_p3(h, p_pred_image, i_abs_part_idx ) : x265_image_get_cb_addr_p3(h, p_pred_image, i_abs_part_idx ) );
	i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
	i_num_coeff_per_inc = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height >> ( h->sps[0].i_max_cu_depth << 1 ) ) >> 2;
	coeff = ( i_chroma_id > 0 ? enc_search->ppc_qt_temp_coeff_cr[ i_qt_layer ] : enc_search->ppc_qt_temp_coeff_cb[ i_qt_layer ] ) + i_num_coeff_per_inc * i_abs_part_idx;
#if X265_ADAPTIVE_QP_SELECTION
	arl_coeff = ( i_chroma_id > 0 ? enc_search->ppc_qt_temp_arl_coeff_cr[ i_qt_layer ] : enc_search->ppc_qt_temp_arl_coeff_cb[ i_qt_layer ] ) + i_num_coeff_per_inc * i_abs_part_idx;
#endif
	rec_qt = ( i_chroma_id > 0
				? x265_image_get_cr_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx )
				: x265_image_get_cb_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx ) );
	i_rec_qt_stride = x265_image_get_c_stride(enc_search->ppc_qt_temp_image[ i_qt_layer ]);

	i_zorder = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_abs_part_idx;
	rec_ipred = ( i_chroma_id > 0
				? x265_image_get_cr_addr_p3(h, h->image_intra, i_zorder )
				: x265_image_get_cb_addr_p3(h, h->image_intra, i_zorder ) ) ;
	i_rec_ipred_stride = x265_image_get_c_stride(h->image_intra);
	b_use_transform_skip_chroma = x265_data_cu_get_transform_skip_p3(cu, i_abs_part_idx, i_text_type);
	//===== update chroma mode =====
	if( i_chroma_pred_mode == X265_DM_CHROMA_IDX )
	{
		i_chroma_pred_mode = x265_base_data_cu_get_luma_intra_dir_p2((x265_base_data_cu_t*)cu,  0 );
	}

	if( b_default0_save1_load2 != 2 )
	{
		x265_pattern_init_pattern_p5(h,
									x265_data_cu_get_pattern(cu),
									cu,
									i_tr_depth,
									i_abs_part_idx );
		x265_pattern_init_adi_pattern_chroma(h,
											x265_data_cu_get_pattern(cu),
											cu,
											h->image_intra,
											i_abs_part_idx,
											i_tr_depth,
											enc_search->prediction.image_ext,
											enc_search->prediction.i_image_ext_stride,
											enc_search->prediction.i_image_ext_height,
											&b_above_avail,
											&b_left_avail );
		p_pat_chroma = ( i_chroma_id > 0
						? x265_pattern_get_adi_cr_buf(x265_data_cu_get_pattern(cu),
													i_width,
													i_height,
													enc_search->prediction.image_ext,
													enc_search->prediction.i_image_ext_stride)
						: x265_pattern_get_adi_cb_buf(x265_data_cu_get_pattern(cu),
													i_width,
													i_height,
													enc_search->prediction.image_ext,
													enc_search->prediction.i_image_ext_stride ));

		//===== get prediction signal =====
		{
			x265_prediction_pred_intra_chroma_ang(h,
												(x265_prediction_t*)enc_search,
												p_pat_chroma,
												i_chroma_pred_mode,
												pred,
												i_stride,
												i_width,
												i_height,
												b_above_avail,
												b_left_avail );
		}
			// save prediction
		if( b_default0_save1_load2 == 1 )
		{
			p_pred = pred;
			p_pred_buf = enc_search->shared_pred_transform_skip[1 + i_chroma_id];
			k = 0;
			for( i_y = 0; i_y < i_height; i_y++ )
			{
				for( i_x = 0; i_x < i_width; i_x++ )
				{
					p_pred_buf[ k ++ ] = p_pred[ i_x ];
				}
				p_pred += i_stride;
			}
		}
	}
	else
	{
		// load prediction
		p_pred = pred;
		p_pred_buf = enc_search->shared_pred_transform_skip[1 + i_chroma_id];
		k = 0;
		for( i_y = 0; i_y < i_height; i_y++ )
		{
			for( i_x = 0; i_x < i_width; i_x++ )
			{
				p_pred[ i_x ] = p_pred_buf[ k ++ ];
			}
			p_pred += i_stride;
		}
	}

	//===== get residual signal =====
	{
		// get residual
		p_org = org;
		p_pred = pred;
		p_resi = resi;
		for( i_y = 0; i_y < i_height; i_y++ )
		{
			for( i_x = 0; i_x < i_width; i_x++ )
			{
				p_resi[ i_x ] = p_org[ i_x ] - p_pred[ i_x ];
			}
			p_org  += i_stride;
			p_resi += i_stride;
			p_pred += i_stride;
		}
	}


	//===== init availability pattern =====
	//===== transform and quantization =====

	//--- init rate estimation arrays for rdoq ---
	if( b_use_transform_skip_chroma
			? h->param.rdo.b_use_rdo_q_ts
			: h->param.rdo.b_use_rdo_q )
	{
		x265_enc_entropy_estimate_bit(h,
									&h->enc_entropy,
									h->tr_quant.est_bits_sbac,
									i_width,
									i_width,
									i_text_type );
	}


	if(i_text_type == TEXT_CHROMA_U)
	{
		i_cur_chroma_qp_offset = h->pps[0].i_chroma_cb_qp_offset + h->slice->i_slice_qp_delta_cb;
	}
	else
	{
		i_cur_chroma_qp_offset = h->pps[0].i_chroma_cr_qp_offset + h->slice->i_slice_qp_delta_cr;
	}
	x265_tr_quant_set_qp_for_quant(&h->tr_quant,
									x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ),
									TEXT_CHROMA,
									h->sps[0].i_qp_bd_offset_c,
									i_cur_chroma_qp_offset );

#if X265_RDOQ_CHROMA_LAMBDA
	x265_tr_quant_select_lambda(&h->tr_quant, TEXT_CHROMA);
#endif
	x265_tr_quant_transform_nxn(h,
								&h->tr_quant,
								cu,
								resi,
								i_stride,
								coeff,
#if X265_ADAPTIVE_QP_SELECTION
								&arl_coeff,
#endif
								i_width,
								i_height,
								&i_abs_sum,
								i_text_type,
								i_abs_part_idx,
								b_use_transform_skip_chroma );
	//--- set coded block flag ---
	x265_data_cu_set_cbf_sub_parts_p6(h,
									cu,
									( i_abs_sum ? 1 : 0 ) << i_org_tr_depth,
									i_text_type,
									i_abs_part_idx,
									x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth );
	//--- inverse transform ---
	if( i_abs_sum )
	{
		i_scaling_list_type = 0 + et_table[(int32_t)i_text_type];
		assert(i_scaling_list_type < 6);
		x265_tr_quant_inv_transform_nxn(h,
										&h->tr_quant,
										x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
										TEXT_CHROMA,
										X265_REG_DCT,
										resi,
										i_stride,
										coeff,
										i_width,
										i_height,
										i_scaling_list_type,
										b_use_transform_skip_chroma );
	}
	else
	{
		p_resi = resi;
		memset( coeff, 0, sizeof( x265_coeff_t ) * i_width * i_height );
		for( i_y = 0; i_y < i_height; i_y++ )
		{
			memset( p_resi, 0, sizeof( spixel ) * i_width );
			p_resi += i_stride;
		}
	}


	//===== reconstruction =====
	{
		p_pred = pred;
		p_resi = resi;
		p_reco = reco;
		p_rec_qt = rec_qt;
		p_rec_ipred = rec_ipred;
		for( i_y = 0; i_y < i_height; i_y++ )
		{
			for( i_x = 0; i_x < i_width; i_x++ )
			{
				p_reco [ i_x ] = x265_clip_c(h, p_pred[ i_x ] + p_resi[ i_x ] );
				p_rec_qt [ i_x ] = p_reco[ i_x ];
				p_rec_ipred[ i_x ] = p_reco[ i_x ];
			}
			p_pred += i_stride;
			p_resi += i_stride;
			p_reco += i_stride;
			p_rec_qt += i_rec_qt_stride;
			p_rec_ipred += i_rec_ipred_stride;
		}
	}

	//===== update distortion =====
#if X265_WEIGHTED_CHROMA_DISTORTION
	*p_dist += x265_rd_cost_get_dist_part(h,
										&h->rd_cost,
										h->cu.pic.i_bit_depth_c,
										reco,
										i_stride,
										org,
										i_stride,
										i_width,
										i_height,
										i_text_type,
										DF_SSE );
#else
	*p_dist += x265_rd_cost_get_dist_part(h,
										&h->rd_cost,
										h->cu.pic.i_bit_depth_c,
										reco,
										i_stride,
										org,
										i_stride,
										i_width,
										i_height,
										TEXT_LUMA,
										DF_SSE );
#endif


}



void x265_enc_search_x_recur_intra_coding_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma_only,
											x265_image_t *p_org_image,
											x265_image_t *p_pred_image,
											x265_simage_t *p_resi_image,
											uint32_t *p_dist_y,
											uint32_t *p_dist_c,
#if X265_HHI_RQT_INTRA_SPEEDUP
											int32_t b_check_first,
#endif
											double *p_rd_cost )
{
	uint32_t i_full_depth = 0;
	uint32_t i_log2_tr_size = 0;
	int32_t b_check_full = 0;
	int32_t b_check_split = 0;

#if X265_HHI_RQT_INTRA_SPEEDUP
#if X265_L0232_RD_PENALTY
	int32_t i_max_tu_size = 0;
	int32_t b_is_intra_slice = 0;
	int32_t b_no_split_intra_max_tu_size = 0;
#endif
#else
#if X265_L0232_RD_PENALTY
	int32_t i_max_tu_size = 0;
	int32_t b_is_intra_slice = 0;
#endif
#endif
	double f_single_cost = 0.0 ;
	uint32_t i_single_dist_y = 0;
	uint32_t i_single_dist_c = 0;
	uint32_t i_single_cbf_y = 0;
	uint32_t i_single_cbf_u = 0;
	uint32_t i_single_cbf_v = 0;
	int32_t b_check_transform_skip  = 0;
	uint32_t i_width_transform_skip = 0;
	uint32_t i_height_transform_skip = 0;
	int32_t i_best_mode_id    = 0;
	int32_t best_mode_id_uv[2] = {0, 0};
	uint32_t i_single_dist_y_tmp = 0;
	uint32_t i_single_dist_c_tmp = 0;
	uint32_t i_single_cbf_y_tmp = 0;
	uint32_t i_single_cbf_u_tmp = 0;
	uint32_t i_single_cbf_v_tmp = 0;
	double single_cost_tmp = 0;
	int32_t b_default0_save1_load2 = 0;
	int32_t first_check_id  = 0;
	uint32_t i_qp_div = 0;
	int32_t  b_first_q = 0;
	int32_t i_mode_id = 0;
	uint32_t i_single_bits = 0;
	double f_split_cost = 0.0;
	uint32_t i_split_dist_y = 0;
	uint32_t i_split_dist_c = 0;
	uint32_t i_q_parts_div = 0;
	uint32_t i_abs_part_idx_sub = 0;
	uint32_t i_split_cbf_y = 0;
	uint32_t i_split_cbf_u = 0;
	uint32_t i_split_cbf_v = 0;
	uint32_t i_part = 0;
	uint32_t i_offs = 0;
	uint32_t i_split_bits = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_qt_layer = 0;
	uint32_t i_zorder = 0;
	pixel *p_src = NULL;
	uint32_t i_src_stride = 0;
	pixel *p_des = 0;
	uint32_t i_des_stride = 0;
	uint32_t i_y = 0;
	uint32_t i_x = 0;

	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) +  i_tr_depth;
	i_log2_tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> i_full_depth] + 2;
	b_check_full = (i_log2_tr_size <= h->sps[0].i_quadtree_tu_log2_max_size);
	b_check_split = (i_log2_tr_size > x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx));

#if X265_HHI_RQT_INTRA_SPEEDUP
#if X265_L0232_RD_PENALTY
	i_max_tu_size = h->sps[0].i_quadtree_tu_log2_max_size;
	b_is_intra_slice = (h->slice->i_slice_type == I_SLICE);
	// don't check split if tu size is less or equal to max tu size
	b_no_split_intra_max_tu_size = b_check_full;
	if(h->param.rdo.i_rd_penalty && !b_is_intra_slice)
	{
		// in addition don't check split if tu size is less or equal to 16x16 tu size for non-intra slice
		b_no_split_intra_max_tu_size = ( i_log2_tr_size <= X265_MIN(i_max_tu_size, 4));

		// if maximum rd-penalty don't check tu size 32x32
		if(2 == h->param.rdo.i_rd_penalty)
		{
			b_check_full = ( i_log2_tr_size  <= X265_MIN(i_max_tu_size, 4));
		}
	}
	if( b_check_first && b_no_split_intra_max_tu_size )
#else
	if( b_check_first && b_check_full )
#endif
	{
		b_check_split = 0;
	}
	#else
#if X265_L0232_RD_PENALTY
	i_max_tu_size = h->sps[0].i_quadtree_tu_log2_max_size;
	b_is_intra_slice = (h->slice->i_slice_type == I_SLICE);
	// if maximum rd-penalty don't check tu size 32x32
	if((2 == h->param.rdo.i_rd_penalty) && !b_is_intra_slice)
	{
		b_check_full = ( i_log2_tr_size  <= X265_MIN(i_max_tu_size,4));
	}
#endif
#endif
	f_single_cost = X265_MAX_DOUBLE;
	b_check_transform_skip = h->pps[0].b_use_transform_skip;
	i_width_transform_skip = x265_data_cu_get_width_p2(cu, 0) >> i_tr_depth;
	i_height_transform_skip = x265_data_cu_get_height_p2(cu, 0) >> i_tr_depth;
	b_check_transform_skip &= (i_width_transform_skip == 4 && i_height_transform_skip == 4);
	b_check_transform_skip &= (!x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, 0));
	b_check_transform_skip &= (!((x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0 ) == 0) && h->sps[0].b_use_lossless));
	if ( h->param.b_use_transform_skip_fast )
	{
		b_check_transform_skip  &= (x265_data_cu_get_partition_size_p2(cu, i_abs_part_idx)==SIZE_NxN);
	}

	if( b_check_full )
	{
		if(b_check_transform_skip == 1)
		{
			//----- store original entropy coding status -----
			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_store (&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_ROOT ] );
			}

			i_qp_div = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 ) + (i_tr_depth - 1)) << 1);
			b_first_q = ( ( i_abs_part_idx % i_qp_div ) == 0 );

			for(i_mode_id = first_check_id; i_mode_id < 2; i_mode_id ++)
			{
				i_single_dist_y_tmp = 0;
				i_single_dist_c_tmp = 0;
				x265_data_cu_set_transform_skip_sub_parts_p6(h,
															cu,
															i_mode_id,
															TEXT_LUMA,
															i_abs_part_idx,
															i_full_depth);
				if(i_mode_id == first_check_id)
				{
					b_default0_save1_load2 = 1;
				}
				else
				{
					b_default0_save1_load2 = 2;
				}
				//----- code luma block with given intra prediction mode and store cbf-----
				x265_enc_search_x_intra_coding_luma_blk(h,
														enc_search,
														cu,
														i_tr_depth,
														i_abs_part_idx,
														p_org_image,
														p_pred_image,
														p_resi_image,
														&i_single_dist_y_tmp,
														b_default0_save1_load2);
				i_single_cbf_y_tmp = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_abs_part_idx, TEXT_LUMA, i_tr_depth );
				//----- code chroma blocks with given intra prediction mode and store cbf-----
				if( !b_luma_only )
				{
					if(b_first_q)
					{
						x265_data_cu_set_transform_skip_sub_parts_p6(h,
																	cu,
																	i_mode_id,
																	TEXT_CHROMA_U,
																	i_abs_part_idx,
																	i_full_depth);
						x265_data_cu_set_transform_skip_sub_parts_p6(h,
																	cu,
																	i_mode_id,
																	TEXT_CHROMA_V,
																	i_abs_part_idx,
																	i_full_depth);
					}
					x265_enc_search_x_intra_coding_chroma_blk(h,
															enc_search,
															cu,
															i_tr_depth,
															i_abs_part_idx,
															p_org_image,
															p_pred_image,
															p_resi_image,
															&i_single_dist_c_tmp,
															0,
															b_default0_save1_load2);
					x265_enc_search_x_intra_coding_chroma_blk(h,
															enc_search,
															cu,
															i_tr_depth,
															i_abs_part_idx,
															p_org_image,
															p_pred_image,
															p_resi_image,
															&i_single_dist_c_tmp,
															1,
															b_default0_save1_load2);
					i_single_cbf_u_tmp = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_abs_part_idx, TEXT_CHROMA_U, i_tr_depth );
					i_single_cbf_v_tmp = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_abs_part_idx, TEXT_CHROMA_V, i_tr_depth );
				}


				//----- determine rate and r-d cost -----
				if(i_mode_id == 1 && i_single_cbf_y_tmp == 0)
				{
					//in order not to code ts flag when cbf is zero, the case for ts with cbf being zero is forbidden.
					single_cost_tmp = X265_MAX_DOUBLE;
				}
				else
				{
					i_single_bits = x265_enc_search_x_get_intra_bits_qt(h,
																		enc_search,
																		cu,
																		i_tr_depth,
																		i_abs_part_idx,
																		1,
																		!b_luma_only,
																		0);
					single_cost_tmp = x265_rd_cost_calc_rd_cost(&h->rd_cost,
																i_single_bits,
																i_single_dist_y_tmp + i_single_dist_c_tmp,
																0,
																DF_DEFAULT);
				}

				if(single_cost_tmp < f_single_cost)
				{
					f_single_cost = single_cost_tmp;
					i_single_dist_y = i_single_dist_y_tmp;
					i_single_dist_c = i_single_dist_c_tmp;
					i_single_cbf_y = i_single_cbf_y_tmp;
					i_single_cbf_u = i_single_cbf_u_tmp;
					i_single_cbf_v= i_single_cbf_v_tmp;
					i_best_mode_id = i_mode_id;
					if(i_best_mode_id == first_check_id)
					{
						x265_enc_search_x_store_intra_result_qt(h,
																enc_search,
																cu,
																i_tr_depth,
																i_abs_part_idx,
																b_luma_only );
						if(h->param.b_use_sbac_rd)
						{
							x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
												h->pppc_rd_sbac_coder[ i_full_depth ][ CI_TEMP_BEST ] );
						}
					}
				}
				if(i_mode_id == first_check_id)
				{
					x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
										h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_ROOT ] );
				}
			}

			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														i_best_mode_id,
														TEXT_LUMA,
														i_abs_part_idx,
														i_full_depth );

			if(i_best_mode_id == first_check_id)
			{
				x265_enc_search_x_load_intra_result_qt(h,
														enc_search,
														cu,
														i_tr_depth,
														i_abs_part_idx,
														b_luma_only );
				x265_data_cu_set_cbf_sub_parts_p6(h,
												cu,
												i_single_cbf_y << i_tr_depth,
												TEXT_LUMA,
												i_abs_part_idx,
												i_full_depth );
				if( !b_luma_only )
				{
					if(b_first_q)
					{
						x265_data_cu_set_cbf_sub_parts_p6(h,
														cu,
														i_single_cbf_u << i_tr_depth,
														TEXT_CHROMA_U,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_tr_depth - 1 );
						x265_data_cu_set_cbf_sub_parts_p6(h,
														cu,
														i_single_cbf_v << i_tr_depth,
														TEXT_CHROMA_V,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_tr_depth - 1 );
					}
				}
				if(h->param.b_use_sbac_rd)
				{
					x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
										h->pppc_rd_sbac_coder[ i_full_depth ][ CI_TEMP_BEST ] );
				}
			}

			if( !b_luma_only )
			{
				best_mode_id_uv[0] = best_mode_id_uv[1] = i_best_mode_id;
				if(b_first_q && i_best_mode_id == 1)
				{
					//in order not to code ts flag when cbf is zero, the case for ts with cbf being zero is forbidden.
					if(i_single_cbf_u == 0)
					{
						x265_data_cu_set_transform_skip_sub_parts_p6(h,
																	cu,
																	0,
																	TEXT_CHROMA_U,
																	i_abs_part_idx,
																	i_full_depth);
						best_mode_id_uv[0] = 0;
					}
					if(i_single_cbf_v == 0)
					{
						x265_data_cu_set_transform_skip_sub_parts_p6(h,
																	cu,
																	0,
																	TEXT_CHROMA_V,
																	i_abs_part_idx,
																	i_full_depth);
						best_mode_id_uv[1] = 0;
					}
				}
			}


		}
		else
		{
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														0,
														TEXT_LUMA,
														i_abs_part_idx,
														i_full_depth );
			//----- store original entropy coding status -----
			if( h->param.b_use_sbac_rd && b_check_split )
			{
				x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_ROOT ] );
			}
			//----- code luma block with given intra prediction mode and store cbf-----
			f_single_cost   = 0.0;
			x265_enc_search_x_intra_coding_luma_blk(h,
													enc_search,
													cu,
													i_tr_depth,
													i_abs_part_idx,
													p_org_image,
													p_pred_image,
													p_resi_image,
													&i_single_dist_y,
													0 );
			if( b_check_split )
			{
				i_single_cbf_y = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_abs_part_idx, TEXT_LUMA, i_tr_depth );
			}
			//----- code chroma blocks with given intra prediction mode and store cbf-----
			if( !b_luma_only )
			{
				x265_data_cu_set_transform_skip_sub_parts_p6(h,
															cu,
															0,
															TEXT_CHROMA_U,
															i_abs_part_idx,
															i_full_depth );
				x265_data_cu_set_transform_skip_sub_parts_p6(h,
															cu,
															0,
															TEXT_CHROMA_V,
															i_abs_part_idx,
															i_full_depth );
				x265_enc_search_x_intra_coding_chroma_blk(h,
														enc_search,
														cu,
														i_tr_depth,
														i_abs_part_idx,
														p_org_image,
														p_pred_image,
														p_resi_image,
														&i_single_dist_c,
														0,
														0);
				x265_enc_search_x_intra_coding_chroma_blk(h,
														enc_search,
														cu,
														i_tr_depth,
														i_abs_part_idx,
														p_org_image,
														p_pred_image,
														p_resi_image,
														&i_single_dist_c,
														1,
														0);
				if( b_check_split )
				{
					i_single_cbf_u = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_abs_part_idx, TEXT_CHROMA_U, i_tr_depth );
					i_single_cbf_v = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_abs_part_idx, TEXT_CHROMA_V, i_tr_depth );
				}
			}


			//----- determine rate and r-d cost -----
			i_single_bits = x265_enc_search_x_get_intra_bits_qt(h,
																enc_search,
																cu,
																i_tr_depth,
																i_abs_part_idx,
																1,
																!b_luma_only,
																0);
#if X265_L0232_RD_PENALTY
			if(h->param.rdo.i_rd_penalty
				&& (i_log2_tr_size==5)
				&& !b_is_intra_slice)
			{
				i_single_bits = i_single_bits * 4;
			}
#endif
			f_single_cost = x265_rd_cost_calc_rd_cost(&h->rd_cost,
													i_single_bits,
													i_single_dist_y + i_single_dist_c,
													0,
													DF_DEFAULT );
		}
	}

	if( b_check_split )
	{
		//----- store full entropy coding status, load original entropy coding status -----
		if( h->param.b_use_sbac_rd )
		{
			if( b_check_full )
			{
				x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_TEST ] );
				x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_ROOT ] );
			}
			else
			{
				x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_ROOT ] );
			}
		}

		i_q_parts_div= h->cu.pic.i_num_partitions >> ((i_full_depth + 1 ) << 1);
		i_abs_part_idx_sub = i_abs_part_idx;

		for( i_part = 0; i_part < 4; i_part++, i_abs_part_idx_sub += i_q_parts_div )
		{

#if X265_HHI_RQT_INTRA_SPEEDUP
			x265_enc_search_x_recur_intra_coding_qt(h,
													enc_search,
													cu,
													i_tr_depth + 1,
													i_abs_part_idx_sub,
													b_luma_only,
													p_org_image,
													p_pred_image,
													p_resi_image,
													&i_split_dist_y,
													&i_split_dist_c,
													b_check_first,
													&f_split_cost );
#else
			x265_enc_search_x_recur_intra_coding_qt(h,
													enc_search,
													cu,
													i_tr_depth + 1,
													i_abs_part_idx_sub,
													b_luma_only,
													p_org_image,
													p_pred_image,
													p_resi_image,
													&i_split_dist_y,
													&i_split_dist_c,
													&f_split_cost );
#endif

			i_split_cbf_y |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx_sub, TEXT_LUMA, i_tr_depth + 1 );
			if(!b_luma_only)
			{
				i_split_cbf_u |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx_sub, TEXT_CHROMA_U, i_tr_depth + 1 );
				i_split_cbf_v |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx_sub, TEXT_CHROMA_V, i_tr_depth + 1 );
			}
		}

		for( i_offs = 0; i_offs < 4 * i_q_parts_div; i_offs++ )
		{
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_LUMA )[ i_abs_part_idx + i_offs ]
									   |= (i_split_cbf_y << i_tr_depth );
		}
		if( !b_luma_only )
		{
			for( i_offs = 0; i_offs < 4 * i_q_parts_div; i_offs++ )
			{
				x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U )[ i_abs_part_idx + i_offs ]
										   |= ( i_split_cbf_u << i_tr_depth );
				x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V )[ i_abs_part_idx + i_offs ]
										   |= ( i_split_cbf_v << i_tr_depth );
			}
		}
		//----- restore context states -----
		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_ROOT ] );
		}


		//----- determine rate and r-d cost -----
		i_split_bits = x265_enc_search_x_get_intra_bits_qt(h,
															enc_search,
															cu,
															i_tr_depth,
															i_abs_part_idx,
															1,
															!b_luma_only,
															0);

		f_split_cost  = x265_rd_cost_calc_rd_cost(&h->rd_cost,
												i_split_bits,
												i_split_dist_y + i_split_dist_c,
												0,
												DF_DEFAULT);

		//===== compare and set best =====
		if( f_split_cost < f_single_cost )
		{
			//--- update cost ---
			*p_dist_y += i_split_dist_y;
			*p_dist_c += i_split_dist_c;
			*p_rd_cost  += f_split_cost;
			return;
		}

		//----- set entropy coding status -----
		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_TEST ] );
		}

		//--- set transform index and cbf values ---
		x265_base_data_cu_set_tr_idx_sub_parts(h,
											(x265_base_data_cu_t*)cu,
											i_tr_depth,
											i_abs_part_idx,
											i_full_depth );
		x265_data_cu_set_cbf_sub_parts_p6(h,
										cu,
										i_single_cbf_y << i_tr_depth,
										TEXT_LUMA,
										i_abs_part_idx,
										i_full_depth );
		x265_data_cu_set_transform_skip_sub_parts_p6(h,
													cu,
													i_best_mode_id,
													TEXT_LUMA,
													i_abs_part_idx,
													i_full_depth );
		if( !b_luma_only )
		{
			x265_data_cu_set_cbf_sub_parts_p6(h,
											cu,
											i_single_cbf_u << i_tr_depth,
											TEXT_CHROMA_U,
											i_abs_part_idx,
											i_full_depth );
			x265_data_cu_set_cbf_sub_parts_p6(h,
											cu,
											i_single_cbf_v << i_tr_depth,
											TEXT_CHROMA_V,
											i_abs_part_idx,
											i_full_depth );
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														best_mode_id_uv[0],
														TEXT_CHROMA_U,
														i_abs_part_idx,
														i_full_depth);
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														best_mode_id_uv[1],
														TEXT_CHROMA_V,
														i_abs_part_idx,
														i_full_depth);
		}

		//--- set reconstruction for next intra prediction blocks ---
		i_width = x265_data_cu_get_width_p2(cu, 0 ) >> i_tr_depth;
		i_height = x265_data_cu_get_height_p2(cu, 0 ) >> i_tr_depth;
		i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
		i_zorder = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu ) + i_abs_part_idx;
		p_src = x265_image_get_luma_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
		i_src_stride = x265_image_get_stride (enc_search->ppc_qt_temp_image[ i_qt_layer ]);
		p_des = x265_image_get_luma_addr_p3(h, h->image_intra, i_zorder );
		i_des_stride = x265_image_get_stride(h->image_intra);
		for( i_y = 0; i_y < i_height; i_y++, p_src += i_src_stride, p_des += i_des_stride )
		{
			for( i_x = 0; i_x < i_width; i_x++ )
			{
				p_des[ i_x ] = p_src[ i_x ];
			}
		}
		if( !b_luma_only )
		{
			i_width   >>= 1;
			i_height  >>= 1;
			p_src = x265_image_get_cb_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
			i_src_stride = x265_image_get_c_stride (enc_search->ppc_qt_temp_image[ i_qt_layer ]);
			p_des = x265_image_get_cb_addr_p3(h, h->image_intra, i_zorder );
			i_des_stride = x265_image_get_c_stride(h->image_intra);
			for( i_y = 0; i_y < i_height; i_y++, p_src += i_src_stride, p_des += i_des_stride )
			{
				for( i_x = 0; i_x < i_width; i_x++ )
				{
					p_des[ i_x ] = p_src[ i_x ];
				}
			}
			p_src = x265_image_get_cr_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
			p_des = x265_image_get_cr_addr_p3(h, h->image_intra, i_zorder );
			for( i_y = 0; i_y < i_height; i_y++, p_src += i_src_stride, p_des += i_des_stride )
			{
				for( i_x = 0; i_x < i_width; i_x++ )
				{
					p_des[ i_x ] = p_src[ i_x ];
				}
			}
		}
	}
	*p_dist_y += i_single_dist_y;
	*p_dist_c += i_single_dist_c;
	*p_rd_cost  += f_single_cost;
}


void x265_enc_search_x_set_intra_result_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma_only,
											x265_image_t* p_reco_image )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_qt_layer = 0;
	int32_t b_skip_chroma  = 0;
	int32_t b_chroma_same  = 0;
	uint32_t i_qp_div = 0;
	uint32_t i_num_coeff_y = 0;
	uint32_t i_num_coeff_inc_y = 0;
	x265_coeff_t *coeff_src_y = NULL;
	x265_coeff_t *coeff_dst_y = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_src_y = NULL;
	int32_t *arl_coeff_dst_y = NULL;
#endif
	uint32_t i_num_coeff_c = 0;
	uint32_t i_num_coeff_inc_c = 0;
	x265_coeff_t *coeff_src_u = NULL;
	x265_coeff_t *coeff_src_v = NULL;
	x265_coeff_t *coeff_dst_u = NULL;
	x265_coeff_t *coeff_dst_v = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_src_u = NULL;
	int32_t *arl_coeff_src_v = NULL;
	int32_t *arl_coeff_dst_u = NULL;
	int32_t *arl_coeff_dst_v = NULL;
#endif
	uint32_t i_log2_tr_size_chroma = 0;
	uint32_t i_num_q_part = 0;
	uint32_t i_part = 0;





	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth;
	i_tr_mode= x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu,  i_abs_part_idx );
	if(  i_tr_mode == i_tr_depth )
	{
		i_log2_tr_size = h->global.convert_to_bit[ h->sps[0].i_max_cu_width >> i_full_depth ] + 2;
		i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
		if( !b_luma_only && i_log2_tr_size == 2 )
		{
			assert( i_tr_depth > 0 );
			i_qp_div = h->cu.pic.i_num_partitions >> (( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0) + i_tr_depth - 1 ) << 1);
			b_skip_chroma  = ( ( i_abs_part_idx % i_qp_div ) != 0 );
			b_chroma_same  = 1;
		}

		//===== copy transform coefficients =====
		i_num_coeff_y = ( h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height ) >> ( i_full_depth << 1 );
		i_num_coeff_inc_y = ( h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height ) >> ( h->sps[0].i_max_cu_depth << 1 );
		coeff_src_y = enc_search->ppc_qt_temp_coeff_y [ i_qt_layer ] + ( i_num_coeff_inc_y * i_abs_part_idx );
		coeff_dst_y = x265_data_cu_get_coeff_y(cu) + ( i_num_coeff_inc_y * i_abs_part_idx );
		memcpy ( coeff_dst_y, coeff_src_y, sizeof( x265_coeff_t ) * i_num_coeff_y );
#if X265_ADAPTIVE_QP_SELECTION
		arl_coeff_src_y = enc_search->ppc_qt_temp_arl_coeff_y [ i_qt_layer ] + ( i_num_coeff_inc_y * i_abs_part_idx );
		arl_coeff_dst_y = x265_data_cu_get_arl_coeff_y(cu) + ( i_num_coeff_inc_y * i_abs_part_idx );
		memcpy( arl_coeff_dst_y, arl_coeff_src_y, sizeof( int32_t ) * i_num_coeff_y );
#endif
		if( !b_luma_only && !b_skip_chroma )
		{
			i_num_coeff_c    = ( b_chroma_same ? i_num_coeff_y    : i_num_coeff_y    >> 2 );
			i_num_coeff_inc_c = i_num_coeff_inc_y >> 2;
			coeff_src_u = enc_search->ppc_qt_temp_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			coeff_src_v = enc_search->ppc_qt_temp_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			coeff_dst_u = x265_data_cu_get_coeff_cb(cu) + ( i_num_coeff_inc_c * i_abs_part_idx );
			coeff_dst_v = x265_data_cu_get_coeff_cr(cu) + ( i_num_coeff_inc_c * i_abs_part_idx );
			memcpy( coeff_dst_u, coeff_src_u, sizeof( x265_coeff_t ) * i_num_coeff_c );
			memcpy( coeff_dst_v, coeff_src_v, sizeof( x265_coeff_t ) * i_num_coeff_c );
#if X265_ADAPTIVE_QP_SELECTION
			arl_coeff_src_u = enc_search->ppc_qt_temp_arl_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			arl_coeff_src_v = enc_search->ppc_qt_temp_arl_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			arl_coeff_dst_u = x265_data_cu_get_arl_coeff_cb(cu) + ( i_num_coeff_inc_c * i_abs_part_idx );
			arl_coeff_dst_v = x265_data_cu_get_arl_coeff_cr(cu) + ( i_num_coeff_inc_c * i_abs_part_idx );
			memcpy( arl_coeff_dst_u, arl_coeff_src_u, sizeof( int32_t ) * i_num_coeff_c );
			memcpy( arl_coeff_dst_v, arl_coeff_src_v, sizeof( int32_t ) * i_num_coeff_c );
#endif
		}

		//===== copy reconstruction =====
		x265_image_copy_part_to_part_luma(h,
										enc_search->ppc_qt_temp_image[ i_qt_layer ],
										p_reco_image,
										i_abs_part_idx,
										1 << i_log2_tr_size,
										1 << i_log2_tr_size );
		if( !b_luma_only && !b_skip_chroma )
		{
			i_log2_tr_size_chroma = ( b_chroma_same ? i_log2_tr_size : i_log2_tr_size - 1 );
			x265_image_copy_part_to_part_chroma(h,
												enc_search->ppc_qt_temp_image[ i_qt_layer ],
												p_reco_image,
												i_abs_part_idx,
												1 << i_log2_tr_size_chroma,
												1 << i_log2_tr_size_chroma );
		}
	}
	else
	{
		i_num_q_part = h->cu.pic.i_num_partitions >> ((i_full_depth + 1) << 1);
		for( i_part = 0; i_part < 4; i_part++ )
		{
			x265_enc_search_x_set_intra_result_qt(h,
												enc_search,
												cu,
												i_tr_depth + 1,
												i_abs_part_idx + i_part * i_num_q_part,
												b_luma_only,
												p_reco_image );
		}
	}
}

void x265_enc_search_x_store_intra_result_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma_only )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_qt_layer = 0;
	int32_t b_skip_chroma  = 0;
	int32_t b_chroma_same  = 0;
	uint32_t i_qp_div = 0;
	uint32_t i_num_coeff_y = 0;
	uint32_t i_num_coeff_inc_y = 0;
	x265_coeff_t* coeff_src_y = 0;
	x265_coeff_t* coeff_dst_y = 0;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_src_y = NULL;
	int32_t *arl_coeff_dst_y = NULL;
#endif
	uint32_t i_num_coeff_c = 0;;
	uint32_t i_num_coeff_inc_c = 0;
	x265_coeff_t *coeff_src_u = NULL;
	x265_coeff_t *coeff_src_v = NULL;
	x265_coeff_t *coeff_dst_u = NULL;
	x265_coeff_t *coeff_dst_v = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_src_u = NULL;
	int32_t *arl_coeff_src_v = NULL;
	int32_t *arl_coeff_dst_u = NULL;
	int32_t *arl_coeff_dst_v = NULL;
#endif
	uint32_t i_log2_tr_size_chroma = 0;

	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth;
	i_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu,  i_abs_part_idx );
	assert(  i_tr_mode == i_tr_depth );
	i_log2_tr_size = h->global.convert_to_bit[ h->sps[0].i_max_cu_width >> i_full_depth ] + 2;
	i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
	if( !b_luma_only && i_log2_tr_size == 2 )
	{
		assert( i_tr_depth > 0 );
		i_qp_div = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_tr_depth - 1 ) << 1 );
		b_skip_chroma  = ( ( i_abs_part_idx % i_qp_div ) != 0 );
		b_chroma_same  = 1;
	}

	//===== copy transform coefficients =====
	i_num_coeff_y = ( h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height ) >> ( i_full_depth << 1 );
	i_num_coeff_inc_y = ( h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height ) >> ( h->sps[0].i_max_cu_depth << 1 );
	coeff_src_y = enc_search->ppc_qt_temp_coeff_y [ i_qt_layer ] + ( i_num_coeff_inc_y * i_abs_part_idx );
	coeff_dst_y = enc_search->pc_qt_temp_tu_coeff_y;
	memcpy( coeff_dst_y, coeff_src_y, sizeof( x265_coeff_t ) * i_num_coeff_y );
#if X265_ADAPTIVE_QP_SELECTION
	arl_coeff_src_y = enc_search->ppc_qt_temp_arl_coeff_y [ i_qt_layer ] + ( i_num_coeff_inc_y * i_abs_part_idx );
	arl_coeff_dst_y = enc_search->pc_qt_temp_tu_arl_coeff_y;
	memcpy( arl_coeff_dst_y, arl_coeff_src_y, sizeof( int32_t ) * i_num_coeff_y );
#endif
	if( !b_luma_only && !b_skip_chroma )
	{
		i_num_coeff_c = ( b_chroma_same ? i_num_coeff_y    : i_num_coeff_y    >> 2 );
		i_num_coeff_inc_c = i_num_coeff_inc_y >> 2;
		coeff_src_u = enc_search->ppc_qt_temp_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		coeff_src_v = enc_search->ppc_qt_temp_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		coeff_dst_u = enc_search->pc_qt_temp_tu_coeff_cb;
		coeff_dst_v = enc_search->pc_qt_temp_tu_coeff_cr;
		memcpy( coeff_dst_u, coeff_src_u, sizeof( x265_coeff_t ) * i_num_coeff_c );
		memcpy( coeff_dst_v, coeff_src_v, sizeof( x265_coeff_t ) * i_num_coeff_c );
#if X265_ADAPTIVE_QP_SELECTION
		arl_coeff_src_u = enc_search->ppc_qt_temp_arl_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		arl_coeff_src_v = enc_search->ppc_qt_temp_arl_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		arl_coeff_dst_u = enc_search->pc_qt_temp_tu_arl_coeff_cb;
		arl_coeff_dst_v = enc_search->pc_qt_temp_tu_arl_coeff_cr;
		memcpy( arl_coeff_dst_u, arl_coeff_src_u, sizeof( int32_t ) * i_num_coeff_c );
		memcpy( arl_coeff_dst_v, arl_coeff_src_v, sizeof( int32_t ) * i_num_coeff_c );
#endif
	}

	//===== copy reconstruction =====
	x265_image_copy_part_to_part_luma(h,
									enc_search->ppc_qt_temp_image[ i_qt_layer ],
									&enc_search->qt_temp_transform_skip_image,
									i_abs_part_idx,
									1 << i_log2_tr_size,
									1 << i_log2_tr_size );

	if( !b_luma_only && !b_skip_chroma )
	{
		i_log2_tr_size_chroma = ( b_chroma_same ? i_log2_tr_size : i_log2_tr_size - 1 );
		x265_image_copy_part_to_part_chroma(h,
										enc_search->ppc_qt_temp_image[ i_qt_layer ],
										&enc_search->qt_temp_transform_skip_image,
										i_abs_part_idx,
										1 << i_log2_tr_size_chroma,
										1 << i_log2_tr_size_chroma );
	}

}

void x265_enc_search_x_load_intra_result_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma_only )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_qt_layer = 0;
	int32_t b_skip_chroma = 0;
	int32_t b_chroma_same = 0;
	uint32_t i_qp_div = 0;
	uint32_t i_num_coeff_y = 0;
	uint32_t i_num_coeff_inc_y = 0;
	x265_coeff_t *coeff_dst_y = NULL;
	x265_coeff_t *coeff_src_y = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_dst_y = NULL;
	int32_t *arl_coeff_src_y = NULL;
#endif
	uint32_t i_num_coeff_c = 0;
	uint32_t i_num_coeff_inc_c = 0;
	x265_coeff_t *coeff_dst_u = NULL;
	x265_coeff_t *coeff_dst_v = NULL;
	x265_coeff_t *coeff_src_u = NULL;
	x265_coeff_t *coeff_src_v = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_dst_u = NULL;
	int32_t *arl_coeff_dst_v = NULL;
	int32_t *arl_coeff_src_u = NULL;
	int32_t *arl_coeff_src_v = NULL;
#endif
	uint32_t i_log2_tr_size_chroma = 0;
	uint32_t i_zorder = 0;
	pixel *rec_ipred = NULL;
	uint32_t i_rec_ipred_stride = 0;
	pixel *rec_qt = NULL;
	uint32_t i_rec_qt_stride = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	pixel *p_rec_qt = NULL;
	pixel *p_rec_ipred = NULL;
	uint32_t i_y = 0;
	uint32_t i_x = 0;


	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth;
	i_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu,  i_abs_part_idx );
	assert(  i_tr_mode == i_tr_depth );
	i_log2_tr_size = h->global.convert_to_bit[ h->cu.pic.i_max_cu_width >> i_full_depth ] + 2;
	i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
	if( !b_luma_only && i_log2_tr_size == 2 )
	{
		assert( i_tr_depth > 0 );
		i_qp_div = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_tr_depth - 1 ) << 1 );
		b_skip_chroma  = ( ( i_abs_part_idx % i_qp_div ) != 0 );
		b_chroma_same  = 1;
	}

	//===== copy transform coefficients =====
	i_num_coeff_y = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height) >> (i_full_depth << 1);
	i_num_coeff_inc_y = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height) >> ( h->sps[0].i_max_cu_depth << 1 );
	coeff_dst_y = enc_search->ppc_qt_temp_coeff_y [ i_qt_layer ] + ( i_num_coeff_inc_y * i_abs_part_idx );
	coeff_src_y = enc_search->pc_qt_temp_tu_coeff_y;
	memcpy( coeff_dst_y, coeff_src_y, sizeof( x265_coeff_t ) * i_num_coeff_y );
#if X265_ADAPTIVE_QP_SELECTION
	arl_coeff_dst_y = enc_search->ppc_qt_temp_arl_coeff_y [ i_qt_layer ] + ( i_num_coeff_inc_y * i_abs_part_idx );
	arl_coeff_src_y = enc_search->pc_qt_temp_tu_arl_coeff_y;
	memcpy( arl_coeff_dst_y, arl_coeff_src_y, sizeof( int32_t ) * i_num_coeff_y );
#endif
	if( !b_luma_only && !b_skip_chroma )
	{
		i_num_coeff_c = ( b_chroma_same ? i_num_coeff_y : i_num_coeff_y >> 2 );
		i_num_coeff_inc_c = i_num_coeff_inc_y >> 2;
		coeff_dst_u = enc_search->ppc_qt_temp_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		coeff_dst_v = enc_search->ppc_qt_temp_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		coeff_src_u = enc_search->pc_qt_temp_tu_coeff_cb;
		coeff_src_v = enc_search->pc_qt_temp_tu_coeff_cr;
		memcpy( coeff_dst_u, coeff_src_u, sizeof( x265_coeff_t ) * i_num_coeff_c );
		memcpy( coeff_dst_v, coeff_src_v, sizeof( x265_coeff_t ) * i_num_coeff_c );
#if X265_ADAPTIVE_QP_SELECTION
		arl_coeff_dst_u = enc_search->ppc_qt_temp_arl_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		arl_coeff_dst_v = enc_search->ppc_qt_temp_arl_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		arl_coeff_src_u = enc_search->pc_qt_temp_tu_arl_coeff_cb;
		arl_coeff_src_v = enc_search->pc_qt_temp_tu_arl_coeff_cr;
		memcpy( arl_coeff_dst_u, arl_coeff_src_u, sizeof( int32_t ) * i_num_coeff_c );
		memcpy( arl_coeff_dst_v, arl_coeff_src_v, sizeof( int32_t ) * i_num_coeff_c );
#endif
	}

	//===== copy reconstruction =====
	x265_image_copy_part_to_part_luma(h,
									&enc_search->qt_temp_transform_skip_image,
									enc_search->ppc_qt_temp_image[ i_qt_layer ],
									i_abs_part_idx,
									1 << i_log2_tr_size,
									1 << i_log2_tr_size );

	if( !b_luma_only && !b_skip_chroma )
	{
		i_log2_tr_size_chroma = ( b_chroma_same ? i_log2_tr_size : i_log2_tr_size - 1 );
		x265_image_copy_part_to_part_chroma(h,
										&enc_search->qt_temp_transform_skip_image,
										enc_search->ppc_qt_temp_image[ i_qt_layer ],
										i_abs_part_idx,
										1 << i_log2_tr_size_chroma,
										1 << i_log2_tr_size_chroma );
	}

	i_zorder= x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_abs_part_idx;
	rec_ipred = x265_image_get_luma_addr_p3(h, h->image_intra, i_zorder );
	i_rec_ipred_stride = x265_image_get_stride(h->image_intra);
	rec_qt = x265_image_get_luma_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
	i_rec_qt_stride = x265_image_get_stride(enc_search->ppc_qt_temp_image[ i_qt_layer ]);
	i_width = x265_data_cu_get_width_p2(cu,  0 ) >> i_tr_depth;
	i_height = x265_data_cu_get_height_p2(cu,  0 ) >> i_tr_depth;
	p_rec_qt = rec_qt;
	p_rec_ipred = rec_ipred;
	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			p_rec_ipred[ i_x ] = p_rec_qt   [ i_x ];
		}
		p_rec_qt    += i_rec_qt_stride;
		p_rec_ipred += i_rec_ipred_stride;
	}

	if( !b_luma_only && !b_skip_chroma )
	{
		rec_ipred = x265_image_get_cb_addr_p3(h, h->image_intra, i_zorder );
		rec_qt = x265_image_get_cb_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
		p_rec_qt = rec_qt;
		p_rec_ipred  = rec_ipred;
		for( i_y = 0; i_y < i_height; i_y++ )
		{
			for( i_x = 0; i_x < i_width; i_x++ )
			{
				p_rec_ipred[ i_x ] = p_rec_qt[ i_x ];
			}
			p_rec_qt    += i_rec_qt_stride;
			p_rec_ipred += i_rec_ipred_stride;
		}
		rec_ipred = x265_image_get_cr_addr_p3(h, h->image_intra, i_zorder );
		rec_qt = x265_image_get_cr_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
		p_rec_qt = rec_qt;
		p_rec_ipred  = rec_ipred;
		for( i_y = 0; i_y < i_height; i_y++ )
		{
			for( i_x = 0; i_x < i_width; i_x++ )
			{
				p_rec_ipred[ i_x ] = p_rec_qt[ i_x ];
			}
			p_rec_qt    += i_rec_qt_stride;
			p_rec_ipred += i_rec_ipred_stride;
		}
	}
}

void x265_enc_search_x_store_intra_result_chroma_qt(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													uint32_t i_tr_depth,
													uint32_t i_abs_part_idx,
													uint32_t i_state_u0_v1_both2 )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_qt_layer = 0;;
	int32_t b_chroma_same = 0;
	uint32_t i_qp_div = 0;
	uint32_t i_num_coeff_c = 0;
	uint32_t i_num_coeff_inc_c = 0;
	x265_coeff_t *coeff_src_u = NULL;
	x265_coeff_t *coeff_dst_u = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_src_u = NULL;
	int32_t *arl_coeff_dst_u = NULL;
#endif
	x265_coeff_t *coeff_src_v = NULL;
	x265_coeff_t *coeff_dst_v = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_src_v = NULL;
	int32_t *arl_coeff_dst_v = NULL;
#endif
	uint32_t i_log2_tr_size_chroma = 0;

	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth;
	i_tr_mode    = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu,  i_abs_part_idx );
	if(  i_tr_mode == i_tr_depth )
	{
		i_log2_tr_size = h->global.convert_to_bit[ h->sps[0].i_max_cu_width >> i_full_depth ] + 2;
		i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
		if( i_log2_tr_size == 2 )
		{
			assert( i_tr_depth > 0 );
			i_tr_depth --;
			i_qp_div = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0 ) + i_tr_depth) << 1);
			if( ( i_abs_part_idx % i_qp_div ) != 0 )
			{
				return;
			}
			b_chroma_same = 1;
		}

		//===== copy transform coefficients =====
		i_num_coeff_c = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height) >> (i_full_depth << 1);
		if( !b_chroma_same )
		{
			i_num_coeff_c >>= 2;
		}
		i_num_coeff_inc_c = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height) >> ((h->sps[0].i_max_cu_depth << 1) + 2);
		if(i_state_u0_v1_both2 == 0 || i_state_u0_v1_both2 == 2)
		{
			coeff_src_u = enc_search->ppc_qt_temp_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			coeff_dst_u = enc_search->pc_qt_temp_tu_coeff_cb;
			memcpy( coeff_dst_u, coeff_src_u, sizeof( x265_coeff_t ) * i_num_coeff_c );

#if X265_ADAPTIVE_QP_SELECTION
			arl_coeff_src_u = enc_search->ppc_qt_temp_arl_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			arl_coeff_dst_u = enc_search->pc_qt_temp_tu_arl_coeff_cb;
			memcpy( arl_coeff_dst_u, arl_coeff_src_u, sizeof( int32_t ) * i_num_coeff_c );
#endif
		}
		if(i_state_u0_v1_both2 == 1 || i_state_u0_v1_both2 == 2)
		{
			coeff_src_v = enc_search->ppc_qt_temp_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			coeff_dst_v = enc_search->pc_qt_temp_tu_coeff_cr;
			memcpy( coeff_dst_v, coeff_src_v, sizeof( x265_coeff_t ) * i_num_coeff_c );
			#if X265_ADAPTIVE_QP_SELECTION
			arl_coeff_src_v = enc_search->ppc_qt_temp_arl_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			arl_coeff_dst_v = enc_search->pc_qt_temp_tu_arl_coeff_cr;
			memcpy( arl_coeff_dst_v, arl_coeff_src_v, sizeof( int32_t ) * i_num_coeff_c );
#endif
		}

		//===== copy reconstruction =====
		i_log2_tr_size_chroma = ( b_chroma_same ? i_log2_tr_size : i_log2_tr_size - 1 );
		x265_image_copy_part_to_part_chroma_p7(h,
												enc_search->ppc_qt_temp_image[ i_qt_layer ],
												&enc_search->qt_temp_transform_skip_image,
												i_abs_part_idx,
												1 << i_log2_tr_size_chroma,
												1 << i_log2_tr_size_chroma,
												i_state_u0_v1_both2 );
	}
}


void x265_enc_search_x_load_intra_result_chroma_qt(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													uint32_t i_tr_depth,
													uint32_t i_abs_part_idx,
													uint32_t i_state_u0_v1_both2 )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_qt_layer = 0;
	int32_t b_chroma_same = 0;
	uint32_t i_qp_div = 0;
	//===== copy transform coefficients =====
	uint32_t i_num_coeff_c = 0;
	uint32_t i_num_coeff_inc_c = 0;
	x265_coeff_t *coeff_dst_u = NULL;
	x265_coeff_t *coeff_src_u = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_dst_u = NULL;
	int32_t *arl_coeff_src_u = NULL;
#endif
	x265_coeff_t* coeff_dst_v = NULL;
	x265_coeff_t* coeff_src_v = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t* arl_coeff_dst_v = NULL;
	int32_t* arl_coeff_src_v = NULL;
#endif
	//===== copy reconstruction =====
	uint32_t i_log2_tr_size_chroma = 0;
	uint32_t i_zorder = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_rec_qt_stride = 0;
	uint32_t i_rec_ipred_stride = 0;
	pixel *rec_ipred = NULL;
	pixel *rec_qt = NULL;
	pixel *p_rec_qt = NULL;
	pixel *p_rec_ipred = NULL;
	uint32_t i_y = 0;
	uint32_t i_x = 0;


	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth;
	i_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu,  i_abs_part_idx );
	if(  i_tr_mode == i_tr_depth )
	{
		i_log2_tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> i_full_depth ] + 2;
		i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
		if( i_log2_tr_size == 2 )
		{
			assert( i_tr_depth > 0 );
			i_tr_depth --;
			i_qp_div = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_tr_depth) << 1);
			if( ( i_abs_part_idx % i_qp_div ) != 0 )
			{
				return;
			}
			b_chroma_same = 1;
		}

		//===== copy transform coefficients =====
		i_num_coeff_c = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height) >> (i_full_depth << 1);
		if( !b_chroma_same )
		{
			i_num_coeff_c >>= 2;
		}
		i_num_coeff_inc_c = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height) >> ((h->sps[0].i_max_cu_depth << 1 ) + 2);

		if(i_state_u0_v1_both2 ==0 || i_state_u0_v1_both2 == 2)
		{
			coeff_dst_u = enc_search->ppc_qt_temp_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			coeff_src_u = enc_search->pc_qt_temp_tu_coeff_cb;
			memcpy( coeff_dst_u, coeff_src_u, sizeof( x265_coeff_t ) * i_num_coeff_c );
#if X265_ADAPTIVE_QP_SELECTION
			arl_coeff_dst_u = enc_search->ppc_qt_temp_arl_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			arl_coeff_src_u = enc_search->pc_qt_temp_tu_arl_coeff_cb;
			memcpy( arl_coeff_dst_u, arl_coeff_src_u, sizeof( int32_t ) * i_num_coeff_c );
#endif
		}
		if(i_state_u0_v1_both2 ==1 || i_state_u0_v1_both2 == 2)
		{
			coeff_dst_v = enc_search->ppc_qt_temp_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			coeff_src_v = enc_search->pc_qt_temp_tu_coeff_cr;
			memcpy( coeff_dst_v, coeff_src_v, sizeof( x265_coeff_t ) * i_num_coeff_c );
#if X265_ADAPTIVE_QP_SELECTION
			arl_coeff_dst_v = enc_search->ppc_qt_temp_arl_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
			arl_coeff_src_v = enc_search->pc_qt_temp_tu_arl_coeff_cr;
			memcpy( arl_coeff_dst_v, arl_coeff_src_v, sizeof( int32_t ) * i_num_coeff_c );
#endif
		}

		//===== copy reconstruction =====
		i_log2_tr_size_chroma = ( b_chroma_same ? i_log2_tr_size : i_log2_tr_size - 1 );
		x265_image_copy_part_to_part_chroma_p7(h,
												&enc_search->qt_temp_transform_skip_image,
												enc_search->ppc_qt_temp_image[ i_qt_layer ],
												i_abs_part_idx,
												1 << i_log2_tr_size_chroma,
												1 << i_log2_tr_size_chroma,
												i_state_u0_v1_both2);
		i_zorder= x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu ) + i_abs_part_idx;
		i_width = x265_data_cu_get_width_p2(cu, 0 ) >> (i_tr_depth + 1);
		i_height= x265_data_cu_get_height_p2(cu, 0 ) >> (i_tr_depth + 1);
		i_rec_qt_stride = x265_image_get_c_stride (enc_search->ppc_qt_temp_image[ i_qt_layer ]);
		i_rec_ipred_stride = x265_image_get_c_stride (h->image_intra);

		if(i_state_u0_v1_both2 ==0 || i_state_u0_v1_both2 == 2)
		{
			rec_ipred = x265_image_get_cb_addr_p3(h, h->image_intra, i_zorder );
			rec_qt = x265_image_get_cb_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
			p_rec_qt = rec_qt;
			p_rec_ipred = rec_ipred;
			for( i_y = 0; i_y < i_height; i_y++ )
			{
				for( i_x = 0; i_x < i_width; i_x++ )
				{
					p_rec_ipred[ i_x ] = p_rec_qt[ i_x ];
				}
				p_rec_qt    += i_rec_qt_stride;
				p_rec_ipred += i_rec_ipred_stride;
			}
		}
		if(i_state_u0_v1_both2 == 1 || i_state_u0_v1_both2 == 2)
		{
			rec_ipred = x265_image_get_cr_addr_p3(h, h->image_intra, i_zorder );
			rec_qt = x265_image_get_cr_addr_p3(h, enc_search->ppc_qt_temp_image[ i_qt_layer ], i_abs_part_idx );
			p_rec_qt= rec_qt;
			p_rec_ipred = rec_ipred;
			for( i_y = 0; i_y < i_height; i_y++ )
			{
				for( i_x = 0; i_x < i_width; i_x++ )
				{
					p_rec_ipred[ i_x ] = p_rec_qt[ i_x ];
				}
				p_rec_qt    += i_rec_qt_stride;
				p_rec_ipred += i_rec_ipred_stride;
			}
		}
	}
}

void x265_enc_search_x_recur_intra_chroma_coding_qt(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													uint32_t i_tr_depth,
													uint32_t i_abs_part_idx,
													x265_image_t *p_org_image,
													x265_image_t *p_pred_image,
													x265_simage_t *p_resi_image,
													uint32_t *p_dist )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	int32_t b_check_transform_skip = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_actual_tr_depth = 0;
	uint32_t i_qp_div = 0;
	int32_t b_first_q = 0;
	int32_t i_n_luma_skip = 0;
	uint32_t i_abs_part_idx_sub = 0;
	int32_t i_chroma_id = 0;
	double f_single_cost = 0;
	int32_t i_best_mode_id= 0;
	uint32_t i_single_dist_c = 0;
	uint32_t i_single_cbf_c = 0;
	uint32_t i_single_dist_c_tmp = 0;
	double f_single_cost_tmp = 0.0;
	uint32_t i_single_cbf_ctmp = 0;
	int32_t b_default0_save1_load2 = 0;
	int32_t i_first_check_id  = 0;
	int32_t i_chroma_mode_id = 0;
	uint32_t i_bits_tmp = 0;
	uint32_t i_split_cbf_u = 0;
	uint32_t i_split_cbf_v = 0;
	uint32_t i_q_parts_div = 0;
	uint32_t i_part = 0;
	uint32_t i_offs = 0;

	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) +  i_tr_depth;
	i_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu,  i_abs_part_idx );

	if(  i_tr_mode == i_tr_depth )
	{
		b_check_transform_skip = h->pps[0].b_use_transform_skip;
		i_log2_tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> i_full_depth] + 2;
		i_actual_tr_depth = i_tr_depth;
		if( i_log2_tr_size == 2 )
		{
			assert( i_tr_depth > 0 );
			i_actual_tr_depth--;
			i_qp_div = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_actual_tr_depth) << 1);
			b_first_q = ( ( i_abs_part_idx % i_qp_div ) == 0 );
			if( !b_first_q )
			{
				return;
			}
		}

		b_check_transform_skip &= (i_log2_tr_size <= 3);
		if ( h->param.b_use_transform_skip_fast )
		{
			b_check_transform_skip &= (i_log2_tr_size < 3);
			if (b_check_transform_skip)
			{
				i_n_luma_skip = 0;
				for(i_abs_part_idx_sub = i_abs_part_idx;
						i_abs_part_idx_sub < i_abs_part_idx + 4;
						i_abs_part_idx_sub ++)
				{
					i_n_luma_skip += x265_data_cu_get_transform_skip_p3(cu,
																		i_abs_part_idx_sub,
																		TEXT_LUMA);
				}
				b_check_transform_skip &= (i_n_luma_skip > 0);
			}
		}

		if(b_check_transform_skip)
		{
			//use rdo to decide whether cr/cb takes ts
			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[i_full_depth][CI_QT_TRAFO_ROOT] );
			}

			for(i_chroma_id = 0; i_chroma_id < 2; i_chroma_id ++)
			{
				f_single_cost = X265_MAX_DOUBLE;
				i_best_mode_id= 0;
				i_single_dist_c = 0;
				i_single_cbf_c = 0;
				i_single_dist_c_tmp = 0;
				f_single_cost_tmp = 0;
				i_single_cbf_ctmp = 0;
				b_default0_save1_load2 = 0;
				i_first_check_id  = 0;
				for(i_chroma_mode_id = i_first_check_id; i_chroma_mode_id < 2; i_chroma_mode_id ++)
				{
					x265_data_cu_set_transform_skip_sub_parts_p6(h,
																cu,
																i_chroma_mode_id,
																(enum text_type_e)(i_chroma_id + 2),
																i_abs_part_idx,
																x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) +  i_actual_tr_depth);
					if(i_chroma_mode_id == i_first_check_id)
					{
						b_default0_save1_load2 = 1;
					}
					else
					{
						b_default0_save1_load2 = 2;
					}
					i_single_dist_c_tmp = 0;
					x265_enc_search_x_intra_coding_chroma_blk(h,
															enc_search,
															cu,
															i_tr_depth,
															i_abs_part_idx,
															p_org_image,
															p_pred_image,
															p_resi_image,
															&i_single_dist_c_tmp,
															i_chroma_id,
															b_default0_save1_load2);
					i_single_cbf_ctmp = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, (enum text_type_e)(i_chroma_id + 2), i_tr_depth);

					if(i_chroma_mode_id == 1 && i_single_cbf_ctmp == 0)
					{
						//in order not to code ts flag when cbf is zero, the case for ts with cbf being zero is forbidden.
						f_single_cost_tmp = X265_MAX_DOUBLE;
					}
					else
					{
						i_bits_tmp = x265_enc_search_x_get_intra_bits_qt_chroma(h,
																				enc_search,
																				cu,
																				i_tr_depth,
																				i_abs_part_idx,
																				i_chroma_id + 2,
																				0 );
						f_single_cost_tmp = x265_rd_cost_calc_rd_cost(&h->rd_cost,
																		i_bits_tmp,
																		i_single_dist_c_tmp,
																		0,
																		DF_DEFAULT);
					}

					if(f_single_cost_tmp < f_single_cost)
					{
						f_single_cost = f_single_cost_tmp;
						i_single_dist_c = i_single_dist_c_tmp;
						i_best_mode_id = i_chroma_mode_id;
						i_single_cbf_c = i_single_cbf_ctmp;

						if(i_best_mode_id == i_first_check_id)
						{
							x265_enc_search_x_store_intra_result_chroma_qt(h,
																			enc_search,
																			cu,
																			i_tr_depth,
																			i_abs_part_idx,
																			i_chroma_id);
							if(h->param.b_use_sbac_rd)
							{
								x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
													h->pppc_rd_sbac_coder[ i_full_depth ][ CI_TEMP_BEST ] );
							}
						}
					}
					if(i_chroma_mode_id == i_first_check_id)
					{
						x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
											h->pppc_rd_sbac_coder[ i_full_depth ][ CI_QT_TRAFO_ROOT ] );
					}
				}

				if(i_best_mode_id == i_first_check_id)
				{
					x265_enc_search_x_load_intra_result_chroma_qt(h,
																enc_search,
																cu,
																i_tr_depth,
																i_abs_part_idx,
																i_chroma_id);
					x265_data_cu_set_cbf_sub_parts_p6(h,
													cu,
													i_single_cbf_c << i_tr_depth,
													(enum text_type_e)(i_chroma_id + 2),
													i_abs_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_actual_tr_depth );
					if( h->param.b_use_sbac_rd )
					{
						x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
											h->pppc_rd_sbac_coder[ i_full_depth ][ CI_TEMP_BEST ] );
					}
				}
				x265_data_cu_set_transform_skip_sub_parts_p6(h,
															cu,
															i_best_mode_id,
															(enum text_type_e)(i_chroma_id + 2),
															i_abs_part_idx,
															x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_actual_tr_depth );
				*p_dist += i_single_dist_c;

				if(i_chroma_id == 0)
				{
					if( h->param.b_use_sbac_rd )
					{
						x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
											h->pppc_rd_sbac_coder[i_full_depth][CI_QT_TRAFO_ROOT] );
					}
				}
			}
		}
		else
		{
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														0,
														TEXT_CHROMA_U,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_actual_tr_depth );
			x265_data_cu_set_transform_skip_sub_parts_p6(h,
														cu,
														0,
														TEXT_CHROMA_V,
														i_abs_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_actual_tr_depth );
			x265_enc_search_x_intra_coding_chroma_blk(h,
													enc_search,
													cu,
													i_tr_depth,
													i_abs_part_idx,
													p_org_image,
													p_pred_image,
													p_resi_image,
													p_dist,
													0,
													0);
			x265_enc_search_x_intra_coding_chroma_blk(h,
													enc_search,
													cu,
													i_tr_depth,
													i_abs_part_idx,
													p_org_image,
													p_pred_image,
													p_resi_image,
													p_dist,
													1,
													0);
		}
	}
	else
	{
		i_q_parts_div = (h->cu.pic.i_num_partitions >> ((i_full_depth + 1 ) << 1));
		i_abs_part_idx_sub = i_abs_part_idx;

		for( i_part = 0; i_part < 4; i_part++, i_abs_part_idx_sub += i_q_parts_div )
		{
			x265_enc_search_x_recur_intra_chroma_coding_qt(h,
															enc_search,
															cu,
															i_tr_depth + 1,
															i_abs_part_idx_sub,
															p_org_image,
															p_pred_image,
															p_resi_image,
															p_dist );
			i_split_cbf_u |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_abs_part_idx_sub, TEXT_CHROMA_U, i_tr_depth + 1 );
			i_split_cbf_v |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_abs_part_idx_sub, TEXT_CHROMA_V, i_tr_depth + 1 );
		}

		for( i_offs = 0; i_offs < 4 * i_q_parts_div; i_offs++ )
		{
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U )[ i_abs_part_idx + i_offs ] |= ( i_split_cbf_u << i_tr_depth );
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V )[ i_abs_part_idx + i_offs ] |= ( i_split_cbf_v << i_tr_depth );
		}
	}
}

void x265_enc_search_x_set_intra_result_chroma_qt(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												uint32_t i_tr_depth,
												uint32_t i_abs_part_idx,
												x265_image_t *p_reco_image )
{
	uint32_t i_full_depth = 0;
	uint32_t i_tr_mode = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_qt_layer = 0;
	int32_t b_chroma_same  = 0;
	uint32_t i_qp_div = 0;
	uint32_t i_num_coeff_c = 0;
	uint32_t i_num_coeff_inc_c = 0;
	x265_coeff_t *coeff_src_u = NULL;
	x265_coeff_t *coeff_src_v = NULL;
	x265_coeff_t *coeff_dst_u = NULL;
	x265_coeff_t *coeff_dst_v = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *arl_coeff_src_u = NULL;
	int32_t *arl_coeff_src_v = NULL;
	int32_t *arl_coeff_dst_u = NULL;
	int32_t *arl_coeff_dst_v = NULL;
#endif
	uint32_t i_log2_tr_size_chroma = 0;
	uint32_t i_num_q_part = 0;
	uint32_t i_part = 0;


	i_full_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_tr_depth;
	i_tr_mode= x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu,  i_abs_part_idx );
	if(  i_tr_mode == i_tr_depth )
	{
		i_log2_tr_size = h->global.convert_to_bit[ h->sps[0].i_max_cu_width >> i_full_depth ] + 2;
		i_qt_layer = h->sps[0].i_quadtree_tu_log2_max_size - i_log2_tr_size;
		if( i_log2_tr_size == 2 )
		{
			assert( i_tr_depth > 0 );
			i_qp_div = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu,  0 ) + i_tr_depth - 1 ) << 1);
			if( ( i_abs_part_idx % i_qp_div ) != 0 )
			{
				return;
			}
			b_chroma_same= 1;
		}

		//===== copy transform coefficients =====
		i_num_coeff_c = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height) >> ( i_full_depth << 1 );
		if( !b_chroma_same )
		{
			i_num_coeff_c>>= 2;
		}
		i_num_coeff_inc_c = (h->sps[0].i_max_cu_width * h->sps[0].i_max_cu_height) >> (( h->sps[0].i_max_cu_depth << 1) + 2);
		coeff_src_u = enc_search->ppc_qt_temp_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		coeff_src_v = enc_search->ppc_qt_temp_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		coeff_dst_u = x265_data_cu_get_coeff_cb(cu) + ( i_num_coeff_inc_c * i_abs_part_idx );
		coeff_dst_v = x265_data_cu_get_coeff_cr(cu) + ( i_num_coeff_inc_c * i_abs_part_idx );
		memcpy( coeff_dst_u, coeff_src_u, sizeof( x265_coeff_t ) * i_num_coeff_c );
		memcpy( coeff_dst_v, coeff_src_v, sizeof( x265_coeff_t ) * i_num_coeff_c );
#if X265_ADAPTIVE_QP_SELECTION
		arl_coeff_src_u = enc_search->ppc_qt_temp_arl_coeff_cb[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		arl_coeff_src_v = enc_search->ppc_qt_temp_arl_coeff_cr[ i_qt_layer ] + ( i_num_coeff_inc_c * i_abs_part_idx );
		arl_coeff_dst_u = x265_data_cu_get_arl_coeff_cb(cu) + ( i_num_coeff_inc_c * i_abs_part_idx );
		arl_coeff_dst_v = x265_data_cu_get_arl_coeff_cr(cu) + ( i_num_coeff_inc_c * i_abs_part_idx );
		memcpy( arl_coeff_dst_u, arl_coeff_src_u, sizeof( int32_t ) * i_num_coeff_c );
		memcpy( arl_coeff_dst_v, arl_coeff_src_v, sizeof( int32_t ) * i_num_coeff_c );
#endif

		//===== copy reconstruction =====
		i_log2_tr_size_chroma = ( b_chroma_same ? i_log2_tr_size : i_log2_tr_size - 1 );
		x265_image_copy_part_to_part_chroma(h,
											enc_search->ppc_qt_temp_image[ i_qt_layer ],
											p_reco_image,
											i_abs_part_idx,
											1 << i_log2_tr_size_chroma,
											1 << i_log2_tr_size_chroma );
	}
	else
	{
		i_num_q_part = h->cu.pic.i_num_partitions >> (( i_full_depth + 1 ) << 1);
		for( i_part = 0; i_part < 4; i_part++ )
		{
			x265_enc_search_x_set_intra_result_chroma_qt(h,
														enc_search,
														cu,
														i_tr_depth + 1,
														i_abs_part_idx + i_part * i_num_q_part,
														p_reco_image );
		}
	}
}




void x265_enc_search_preest_chroma_pred_mode ( x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												x265_image_t *p_org_image,
												x265_image_t *p_pred_image )
{
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_stride = 0;
	pixel *p_org_u = 0;
	pixel *p_org_v = 0;
	pixel *p_pred_u = 0;
	pixel *p_pred_v = 0;
	int32_t b_above_avail = 0;
	int32_t b_left_avail  = 0;
	pixel *p_pat_chroma_u = NULL;
	pixel *p_pat_chroma_v = NULL;
	uint32_t i_min_mode = 0;
	uint32_t i_max_mode = 4;
	uint32_t i_best_mode = 0;
	uint32_t i_min_sad = 0;
	uint32_t i_mode  = 0;
	uint32_t i_sad = 0;


	i_width = x265_data_cu_get_width_p2(cu,  0 ) >> 1;
	i_height = x265_data_cu_get_height_p2(cu,  0 ) >> 1;
	i_stride = x265_image_get_c_stride(p_org_image);
	p_org_u = x265_image_get_cb_addr_p3(h, p_org_image, 0 );
	p_org_v = x265_image_get_cr_addr_p3(h, p_org_image, 0 );
	p_pred_u = x265_image_get_cb_addr_p3(h, p_pred_image, 0 );;
	p_pred_v = x265_image_get_cr_addr_p3(h, p_pred_image, 0 );;

	x265_pattern_init_pattern_p5(h, x265_data_cu_get_pattern(cu), cu, 0, 0 );
	x265_pattern_init_adi_pattern_chroma(h,
										x265_data_cu_get_pattern(cu),
										cu,
										h->image_intra,
										0,
										0,
										enc_search->prediction.image_ext,
										enc_search->prediction.i_image_ext_stride,
										enc_search->prediction.i_image_ext_height,
										&b_above_avail,
										&b_left_avail );
	p_pat_chroma_u = x265_pattern_get_adi_cb_buf(x265_data_cu_get_pattern(cu),
												i_width,
												i_height,
												enc_search->prediction.image_ext,
												enc_search->prediction.i_image_ext_stride );
	p_pat_chroma_v = x265_pattern_get_adi_cr_buf(x265_data_cu_get_pattern(cu),
												i_width,
												i_height,
												enc_search->prediction.image_ext,
												enc_search->prediction.i_image_ext_stride );

	i_max_mode = 4;
	i_best_mode = X265_MAX_UINT;
	i_min_sad = X265_MAX_UINT;
	for( i_mode  = i_min_mode; i_mode < i_max_mode; i_mode++ )
	{
		//--- get prediction ---
		x265_prediction_pred_intra_chroma_ang(h,
											(x265_prediction_t*)enc_search,
											p_pat_chroma_u,
											i_mode,
											p_pred_u,
											i_stride,
											i_width,
											i_height,
											b_above_avail,
											b_left_avail );
		x265_prediction_pred_intra_chroma_ang(h,
											(x265_prediction_t*)enc_search,
											p_pat_chroma_v,
											i_mode,
											p_pred_v,
											i_stride,
											i_width,
											i_height,
											b_above_avail,
											b_left_avail );

		//--- get sad ---
		i_sad = x265_rd_cost_calc_had(&h->rd_cost,
										h->cu.pic.i_bit_depth_c,
										p_org_u,
										i_stride,
										p_pred_u,
										i_stride,
										i_width,
										i_height );
		i_sad += x265_rd_cost_calc_had(&h->rd_cost,
										h->cu.pic.i_bit_depth_c,
										p_org_v,
										i_stride,
										p_pred_v,
										i_stride,
										i_width,
										i_height );
		//--- check ---
		if( i_sad < i_min_sad )
		{
			i_min_sad   = i_sad;
			i_best_mode = i_mode;
		}
	}

	//===== set chroma pred mode =====
	x265_data_cu_set_chrom_intra_dir_sub_parts(h,
												cu,
												i_best_mode,
												0,
												x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));

}



void x265_enc_search_est_intra_pred_qt ( x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *p_org_image,
										x265_image_t *p_pred_image,
										x265_simage_t *p_resi_image,
										x265_image_t *p_reco_image,
										uint32_t *p_dist_c,
										int32_t b_luma_only )
{
	int32_t loop = 0;
	int32_t i = 0, j = 0;
	uint32_t i_depth = 0;
	uint32_t i_num_pu = 0;
	uint32_t i_init_tr_depth = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_width_bit= 0;
	uint32_t i_overall_dist_y = 0;
	uint32_t i_overall_dist_c = 0;
	uint32_t i_cand_num;
	uint32_t i_part_offset = 0;
	uint32_t i_pu = 0;
	int32_t b_above_avail = 0;
	int32_t b_left_avail  = 0;
	int32_t i_num_modes_available = 35;
	pixel *p_org = NULL;
	pixel *p_pred = NULL;
	uint32_t i_stride = 0;
	uint32_t rd_mode_list[X265_FAST_UDI_MAX_RDMODE_NUM];
	int32_t i_num_modes_for_full_rd = 0;
	int32_t b_do_fast_search = 0;
	int32_t i_mode_idx = 0;
	int32_t i_mode = 0;
	uint32_t i_sad = 0;
	uint32_t i_mode_bits = 0;
#if X265_FAST_UDI_USE_MPM
	int32_t preds[3] = {-1, -1, -1};
	int32_t i_num_cand = 0;
	int32_t b_most_probable_mode_included = 0;
	int32_t i_most_probable_mode = 0;
#endif


#if X265_HHI_RQT_INTRA_SPEEDUP_MOD
	uint32_t i_second_best_mode = 0;
	double f_second_best_pucost = 0;
#endif
	uint32_t i_best_pu_mode = 0;
	uint32_t i_best_pu_dist_y = 0;
	uint32_t i_best_pu_dist_c = 0;
	uint32_t i_org_mode = 0;
	uint32_t i_q_part_num = 0;

#if X265_HHI_RQT_INTRA_SPEEDUP
#if X265_HHI_RQT_INTRA_SPEEDUP_MOD
#endif
#if X265_HHI_RQT_INTRA_SPEEDUP_MOD
	uint32_t i_org_mode = 0;
#else
#endif
	uint32_t i_pu_dist_y = 0;
	uint32_t i_pu_dist_c = 0;
#endif
	int32_t b_skip_chroma = 0;
	int32_t b_chroma_same = 0;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_comp_width = 0;
	uint32_t i_comp_height = 0;
	uint32_t i_zorder = 0;
	pixel *p_des = NULL;
	uint32_t i_des_stride = 0;
	pixel *p_src = NULL;
	uint32_t i_src_stride = 0;
	uint32_t i_y = 0;
	uint32_t i_x = 0;
	uint32_t i_comb_cbf_y = 0;
	uint32_t i_comb_cbf_u = 0;
	uint32_t i_comb_cbf_v = 0;
	uint32_t i_part_idx  = 0;
	uint32_t i_part = 0;
	uint32_t i_offs = 0;
	double f_cost = 0;
	double f_pu_cost = 0.0;
	double f_best_pu_cost = 0;
	double cand_cost_list[ X265_FAST_UDI_MAX_RDMODE_NUM ];


	i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0);
	i_num_pu = x265_data_cu_get_num_part_inter(cu);
	i_init_tr_depth = x265_data_cu_get_partition_size_p2(cu, 0) == SIZE_2Nx2N ? 0 : 1;
	i_width = x265_data_cu_get_width_p2(cu, 0) >> i_init_tr_depth;
	i_height = x265_data_cu_get_height_p2(cu, 0) >> i_init_tr_depth;
	i_q_num_parts = x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu) >> 2;
	i_width_bit = x265_data_cu_get_intra_size_idx(cu, 0);



	//===== set qp and clear cbf =====
	if ( h->pps[0].b_use_dqp  )
	{
		x265_base_data_cu_set_qp_sub_parts(h,
											(x265_base_data_cu_t*)cu,
											x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, 0),
											0,
											i_depth );
	}
	else
	{
		x265_base_data_cu_set_qp_sub_parts(h,
											(x265_base_data_cu_t*)cu,
											h->slice->i_slice_qp,
											0,
											i_depth );
	}
	for( i_pu = 0; i_pu < i_num_pu; i_pu++, i_part_offset += i_q_num_parts )
	{
		//===== init pattern for luma prediction =====
		x265_pattern_init_pattern_p5(h,
									x265_data_cu_get_pattern(cu),
									cu,
									i_init_tr_depth,
									i_part_offset );
		x265_pattern_init_adi_pattern(h,
									x265_data_cu_get_pattern(cu),
									cu,
									h->image_intra,
									i_part_offset,
									i_init_tr_depth,
									enc_search->prediction.image_ext,
									enc_search->prediction.i_image_ext_stride,
									enc_search->prediction.i_image_ext_height,
									&b_above_avail,
									&b_left_avail,
									0 );

		//===== determine set of modes to be tested (using prediction signal only) =====
		i_num_modes_available= 35; //total number of _intra modes
		p_org = x265_image_get_luma_addr_p4( h, p_org_image, i_pu, i_width );
		p_pred = x265_image_get_luma_addr_p4 (h, p_pred_image, i_pu, i_width );
		i_stride = x265_image_get_stride(p_pred_image);
		i_num_modes_for_full_rd = intra_mode_num_fast[ i_width_bit ];

		b_do_fast_search = (i_num_modes_for_full_rd != i_num_modes_available);
		if (b_do_fast_search)
		{
			assert(i_num_modes_for_full_rd < i_num_modes_available);

			for(loop = 0; loop < i_num_modes_for_full_rd; loop++ )
			{
				cand_cost_list[ loop ] = X265_MAX_DOUBLE;
			}
			i_cand_num = 0;

			for( i_mode_idx = 0; i_mode_idx < i_num_modes_available; i_mode_idx++ )
			{
				i_mode = i_mode_idx;

				x265_prediction_pred_intra_luma_ang(h,
													(x265_prediction_t*)enc_search,
													x265_data_cu_get_pattern(cu),
													i_mode,
													p_pred,
													i_stride,
													i_width,
													i_height,
													b_above_avail,
													b_left_avail );

				// use hadamard transform here
				i_sad = x265_rd_cost_calc_had(&h->rd_cost,
												h->cu.pic.i_bit_depth_y,
												p_org,
												i_stride,
												p_pred,
												i_stride,
												i_width,
												i_height );
				//	print_int_state (i_sad) ;

				i_mode_bits = x265_enc_search_x_mode_bits_intra(h,
																enc_search,
																cu,
																i_mode,
																i_pu,
																i_part_offset,
																i_depth,
																i_init_tr_depth );
				f_cost = (double)i_sad
						+ (double)i_mode_bits * h->rd_cost.f_sqrt_lambda;

				i_cand_num += x265_enc_search_x_update_cand_list(enc_search,
																i_mode,
																f_cost,
																i_num_modes_for_full_rd,
																rd_mode_list,
																cand_cost_list );
			}

#if X265_FAST_UDI_USE_MPM
			i_mode = -1;
			i_num_cand = x265_data_cu_get_intra_dir_luma_predictor(h,
																	cu,
																	i_part_offset,
																	preds,
																	&i_mode );
			if( i_mode >= 0 )
			{
				i_num_cand = i_mode;
			}

			for( j = 0; j < i_num_cand; j++)

			{
				b_most_probable_mode_included = 0;
				i_most_probable_mode = preds[j];

				for( i = 0; i < i_num_modes_for_full_rd; i++)
				{
					b_most_probable_mode_included |= (i_most_probable_mode == rd_mode_list[i]);
				}
				if (!b_most_probable_mode_included)
				{
					rd_mode_list[i_num_modes_for_full_rd++] = i_most_probable_mode;
				}
			}
#endif // fast_udi_use_mpm
		}
		else
		{
			for( loop = 0; loop < i_num_modes_for_full_rd; ++loop)
			{
				rd_mode_list[loop] = loop;
			}
		}

		//===== check modes (using r-d costs) =====
#if X265_HHI_RQT_INTRA_SPEEDUP_MOD
		i_second_best_mode = X265_MAX_UINT;
		f_second_best_pucost = X265_MAX_DOUBLE;
#endif

		f_best_pu_cost = X265_MAX_DOUBLE;
		for( i_mode = 0; i_mode < i_num_modes_for_full_rd; i_mode++ )
		{
			// set luma prediction mode
			i_org_mode = rd_mode_list[i_mode];

			x265_base_data_cu_set_luma_intra_dir_sub_parts(h,
														(x265_base_data_cu_t*)cu,
														i_org_mode,
														i_part_offset,
														i_depth + i_init_tr_depth );

			// set context models
			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST] );
			}

			i_pu_dist_y = 0 ;
			i_pu_dist_c = 0 ;
			f_pu_cost = 0.0 ;

			// determine residual for partition
#if X265_HHI_RQT_INTRA_SPEEDUP
			x265_enc_search_x_recur_intra_coding_qt(h,
													enc_search,
													cu,
													i_init_tr_depth,
													i_part_offset,
													b_luma_only,
													p_org_image,
													p_pred_image,
													p_resi_image,
													&i_pu_dist_y,
													&i_pu_dist_c,
													1,
													&f_pu_cost );
#else
			x265_enc_search_x_recur_intra_coding_qt(h,
													enc_search,
													cu,
													i_init_tr_depth,
													i_part_offset,
													b_luma_only,
													p_org_image,
													p_pred_image,
													p_resi_image,
													&i_pu_dist_y,
													&i_pu_dist_c,
													&f_pu_cost );
#endif

			// check r-d cost
			if( f_pu_cost < f_best_pu_cost )
			{
#if X265_HHI_RQT_INTRA_SPEEDUP_MOD
				i_second_best_mode  = i_best_pu_mode;
				d_second_best_pucost = d_best_pucost;
#endif
				i_best_pu_mode  = i_org_mode;
				i_best_pu_dist_y = i_pu_dist_y;
				i_best_pu_dist_c = i_pu_dist_c;
				f_best_pu_cost = f_pu_cost;

				x265_enc_search_x_set_intra_result_qt(h,
													enc_search,
													cu,
													i_init_tr_depth,
													i_part_offset,
													b_luma_only,
													p_reco_image );

				i_q_part_num = h->cu.pic.i_num_partitions >> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_init_tr_depth ) << 1 );
				memcpy( enc_search->qt_temp_tr_idx,
						x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_cbf[0],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_LUMA) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_cbf[1],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_cbf[2],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_transform_skip_flag[0],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_LUMA) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_transform_skip_flag[1],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_U) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_transform_skip_flag[2],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_V) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
			}
#if X265_HHI_RQT_INTRA_SPEEDUP_MOD
			else if( d_pucost < d_second_best_pucost )
			{
				i_second_best_mode  = i_org_mode;
				d_second_best_pucost = d_pucost;
			}
#endif
		} // mode loop

#if X265_HHI_RQT_INTRA_SPEEDUP
#if X265_HHI_RQT_INTRA_SPEEDUP_MOD
		for( loop =0; loop < 2; ++loop )
#endif
		{
#if X265_HHI_RQT_INTRA_SPEEDUP_MOD
			i_org_mode   = loop ? i_second_best_mode  : i_best_pu_mode;
			if( i_org_mode == X265_MAX_UINT )
			{
				break;
			}
#else
			i_org_mode = i_best_pu_mode;
#endif

			x265_base_data_cu_set_luma_intra_dir_sub_parts(h,
															(x265_base_data_cu_t*)cu,
															i_org_mode,
															i_part_offset,
															i_depth + i_init_tr_depth );

			// set context models
			if( h->param.b_use_sbac_rd )
			{
				x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
									h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST] );
			}

			i_pu_dist_y = 0 ;
			i_pu_dist_c = 0 ;
			f_pu_cost = 0.0 ;

			x265_enc_search_x_recur_intra_coding_qt(h,
													enc_search,
													cu,
													i_init_tr_depth,
													i_part_offset,
													b_luma_only,
													p_org_image,
													p_pred_image,
													p_resi_image,
													&i_pu_dist_y,
													&i_pu_dist_c,
													0,
													&f_pu_cost );

			// check r-d cost
			if( f_pu_cost < f_best_pu_cost )
			{
				i_best_pu_mode = i_org_mode;
				i_best_pu_dist_y = i_pu_dist_y;
				i_best_pu_dist_c = i_pu_dist_c;
				f_best_pu_cost = f_pu_cost;

				x265_enc_search_x_set_intra_result_qt(h,
													enc_search,
													cu,
													i_init_tr_depth,
													i_part_offset,
													b_luma_only,
													p_reco_image );

				i_q_part_num = h->cu.pic.i_num_partitions
								>> ((x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_init_tr_depth ) << 1 );
				memcpy( enc_search->qt_temp_tr_idx,
						x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_cbf[0],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_LUMA) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_cbf[1],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_cbf[2],
						x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_transform_skip_flag[0],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_LUMA) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_transform_skip_flag[1],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_U) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
				memcpy( enc_search->qt_temp_transform_skip_flag[2],
						x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_V) + i_part_offset,
						i_q_part_num * sizeof(uint8_t));
			}
		} // mode loop
#endif

		//--- update overall distortion ---
		i_overall_dist_y += i_best_pu_dist_y;
		i_overall_dist_c += i_best_pu_dist_c;

		//--- update transform index and cbf ---
		i_q_part_num = h->cu.pic.i_num_partitions
						>> ( ( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_init_tr_depth ) << 1 );
		memcpy( x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu) + i_part_offset,
				enc_search->qt_temp_tr_idx,
				i_q_part_num * sizeof(uint8_t));
		memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_LUMA) + i_part_offset,
				enc_search->qt_temp_cbf[0],
				i_q_part_num * sizeof(uint8_t));
		memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U) + i_part_offset,
				enc_search->qt_temp_cbf[1],
				i_q_part_num * sizeof(uint8_t));
		memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V) + i_part_offset,
				enc_search->qt_temp_cbf[2],
				i_q_part_num * sizeof(uint8_t));
		memcpy( x265_data_cu_get_transform_skip_p2(cu, TEXT_LUMA) + i_part_offset,
				enc_search->qt_temp_transform_skip_flag[0],
				i_q_part_num * sizeof(uint8_t));
		memcpy( x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_U) + i_part_offset,
				enc_search->qt_temp_transform_skip_flag[1],
				i_q_part_num * sizeof(uint8_t));
		memcpy( x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_V) + i_part_offset,
				enc_search->qt_temp_transform_skip_flag[2],
				i_q_part_num * sizeof(uint8_t));
		//--- set reconstruction for next intra prediction blocks ---
		if( i_pu != i_num_pu - 1 )
		{
			i_log2_tr_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width >> (x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0) + i_init_tr_depth ) ] + 2;
			if( !b_luma_only && i_log2_tr_size == 2 )
			{
				assert( i_init_tr_depth  > 0 );
				b_skip_chroma  = ( i_pu != 0 );
				b_chroma_same  = 1;
			}

			i_comp_width = x265_data_cu_get_width_p2(cu,  0 ) >> i_init_tr_depth;
			i_comp_height = x265_data_cu_get_height_p2(cu,  0 ) >> i_init_tr_depth;
			i_zorder = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_offset;
			p_des = x265_image_get_luma_addr_p3(h, h->image_intra, i_zorder );
			i_des_stride = x265_image_get_stride(h->image_intra);
			p_src = x265_image_get_luma_addr_p3(h, p_reco_image, i_part_offset);
			i_src_stride = x265_image_get_stride(p_reco_image);
			for(i_y = 0; i_y < i_comp_height; i_y++, p_src += i_src_stride, p_des += i_des_stride )
			{
				for( i_x = 0; i_x < i_comp_width; i_x++ )
				{
					p_des[ i_x ] = p_src[ i_x ];
				}
			}
			if( !b_luma_only && !b_skip_chroma )
			{
				if( !b_chroma_same )
				{
					i_comp_width   >>= 1;
					i_comp_height  >>= 1;
				}
				p_des = x265_image_get_cb_addr_p3(h, h->image_intra, i_zorder );
				i_des_stride = x265_image_get_c_stride(h->image_intra);
				p_src = x265_image_get_cb_addr_p3(h, p_reco_image, i_part_offset);
				i_src_stride = x265_image_get_c_stride(p_reco_image);
				for( i_y = 0; i_y < i_comp_height; i_y++, p_src += i_src_stride, p_des += i_des_stride )
				{
					for( i_x = 0; i_x < i_comp_width; i_x++ )
					{
						p_des[ i_x ] = p_src[ i_x ];
					}
				}
				p_des = x265_image_get_cr_addr_p3(h, h->image_intra, i_zorder );
				p_src = x265_image_get_cr_addr_p3(h, p_reco_image, i_part_offset);
				for( i_y = 0; i_y < i_comp_height; i_y++, p_src += i_src_stride, p_des += i_des_stride )
				{
					for( i_x = 0; i_x < i_comp_width; i_x++ )
					{
						p_des[ i_x ] = p_src[ i_x ];
					}
				}
			}
		}

		//=== update pu data ====
		x265_base_data_cu_set_luma_intra_dir_sub_parts(h,
														(x265_base_data_cu_t*)cu,
														i_best_pu_mode,
														i_part_offset,
														i_depth + i_init_tr_depth );
		x265_data_cu_copy_to_pic_p5(h, cu, i_depth, i_pu, i_init_tr_depth );
	} // pu loop


	if( i_num_pu > 1 )
	{
		// set cbf for all blocks
		for( i_part = 0; i_part < 4; i_part++, i_part_idx += i_q_num_parts )
		{
			i_comb_cbf_y |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_part_idx, TEXT_LUMA,1 );
			i_comb_cbf_u |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_part_idx, TEXT_CHROMA_U, 1 );
			i_comb_cbf_v |= x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,  i_part_idx, TEXT_CHROMA_V, 1 );
		}
		for( i_offs = 0; i_offs < 4 * i_q_num_parts; i_offs++ )
		{
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu,  TEXT_LUMA)[ i_offs ] |= i_comb_cbf_y;
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu,  TEXT_CHROMA_U )[ i_offs ] |= i_comb_cbf_u;
			x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu,  TEXT_CHROMA_V )[ i_offs ] |= i_comb_cbf_v;
		}
	}

	//===== reset context models =====
	if(h->param.b_use_sbac_rd)
	{
		x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
							h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST]);
	}

	//===== set distortion (rate and r-d costs are determined later) =====
	*p_dist_c = i_overall_dist_c;
	cu->i_total_distortion = i_overall_dist_y + i_overall_dist_c;

}



void x265_enc_search_est_intra_pred_chroma_qt( x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												x265_image_t *p_org_image,
												x265_image_t *p_pred_image,
												x265_simage_t *p_resi_image,
												x265_image_t *p_reco_image,
												uint32_t *p_pre_calc_dist_c )
{
	uint32_t i_depth = 0;
	uint32_t i_best_mode = 0;
	uint32_t i_best_dist = 0;
	uint32_t i_min_mode = 0;
	uint32_t i_max_mode = X265_NUM_CHROMA_MODE;
	uint32_t i_mode = 0;
	uint32_t i_dist = 0;
	uint32_t i_bits = 0;
	uint32_t i_qpn = 0;
	uint32_t mode_list[ X265_NUM_CHROMA_MODE ];
	double f_best_cost = 0.0;
	double f_cost = 0.0;


	i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0);
	f_best_cost = X265_MAX_DOUBLE;
	x265_data_cu_get_allowed_chroma_dir(cu,  0, mode_list );
	i_max_mode = X265_NUM_CHROMA_MODE;
	//----- check chroma modes -----
	for( i_mode = i_min_mode; i_mode < i_max_mode; i_mode++ )
	{
		//----- restore context models -----
		if( h->param.b_use_sbac_rd )
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST] );
		}
		i_dist = 0 ;
		x265_data_cu_set_chrom_intra_dir_sub_parts(h,
													cu,
													mode_list[i_mode],
													0,
													i_depth );
		x265_enc_search_x_recur_intra_chroma_coding_qt(h,
													enc_search,
													cu,
													0,
													0,
													p_org_image,
													p_pred_image,
													p_resi_image,
													&i_dist );

		if( h->param.b_use_sbac_rd
			&& h->pps[0].b_use_transform_skip)
		{
			x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
								h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST] );
		}
		i_bits = x265_enc_search_x_get_intra_bits_qt(h,
													enc_search,
													cu,
													0,
													0,
													0,
													1,
													0 );
		f_cost = x265_rd_cost_calc_rd_cost(&h->rd_cost,
											i_bits,
											i_dist,
											0,
											DF_DEFAULT);
		//----- compare -----
		if( f_cost < f_best_cost )
		{
			f_best_cost = f_cost;
			i_best_dist = i_dist;
			i_best_mode = mode_list[i_mode];
			i_qpn = h->cu.pic.i_num_partitions >> ( i_depth << 1 );
			x265_enc_search_x_set_intra_result_chroma_qt(h,
														enc_search,
														cu,
														0,
														0,
														p_reco_image );
			memcpy( enc_search->qt_temp_cbf[1],
					x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U ),
					i_qpn * sizeof(uint8_t));
			memcpy( enc_search->qt_temp_cbf[2],
					x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V ),
					i_qpn * sizeof(uint8_t));
			memcpy( enc_search->qt_temp_transform_skip_flag[1],
					x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_U ),
					i_qpn * sizeof(uint8_t));
			memcpy( enc_search->qt_temp_transform_skip_flag[2],
					x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_V ),
					i_qpn * sizeof(uint8_t));
		}
	}

	//----- set data -----
	i_qpn = h->cu.pic.i_num_partitions >> ( i_depth << 1 );
	memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_U ),
			enc_search->qt_temp_cbf[1],
			i_qpn * sizeof(uint8_t));
	memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu, TEXT_CHROMA_V ),
			enc_search->qt_temp_cbf[2],
			i_qpn * sizeof(uint8_t));
	memcpy( x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_U ),
			enc_search->qt_temp_transform_skip_flag[1],
			i_qpn * sizeof(uint8_t));
	memcpy( x265_data_cu_get_transform_skip_p2(cu, TEXT_CHROMA_V ),
			enc_search->qt_temp_transform_skip_flag[2],
			i_qpn * sizeof(uint8_t));
	x265_data_cu_set_chrom_intra_dir_sub_parts(h, cu, i_best_mode, 0, i_depth );
	cu->i_total_distortion += i_best_dist - *p_pre_calc_dist_c;

	//----- restore context models -----
	if( h->param.b_use_sbac_rd )
	{
		x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
							h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST] );
	}
}


uint32_t x265_enc_search_x_get_mvp_idx_bits ( x265_enc_search_t *enc_search,
											int32_t idx,
											int32_t i_num )
{
	uint32_t i_length = 0 ;
	int32_t i_temp = 0 ;
	int32_t b_code_last = 0 ;

	assert(idx >= 0 && i_num >= 0 && idx < i_num);
	if (i_num == 1)

	{
		return 0;
    }

	i_length = 1;
	i_temp = idx;
	if ( i_temp == 0 )
	{
		return i_length;
	}

	b_code_last = ( i_num-1 > i_temp );

	i_length += (i_temp-1);

	if( b_code_last )
	{
		i_length++;
	}

	return i_length;
}


uint32_t x265_enc_search_x_mode_bits_intra(x265_t *h,
											x265_enc_search_t* enc_search,
											x265_data_cu_t *cu,
											uint32_t i_mode,
											uint32_t i_pu,
											uint32_t i_part_offset,
											uint32_t i_depth,
											uint32_t i_init_tr_depth )
{
	if( h->param.b_use_sbac_rd )
	{
		x265_enc_sbac_load_intra_dir_mode_luma(&h->rd_go_on_sbac_coder,
												h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST] );
	}

	x265_base_data_cu_set_luma_intra_dir_sub_parts(h,
													(x265_base_data_cu_t*)cu,
													i_mode,
													i_part_offset,
													i_depth + i_init_tr_depth );
	x265_enc_entropy_reset_bits(&h->enc_entropy);
	x265_enc_entropy_encode_intra_dir_mode_luma(h,
												&h->enc_entropy,
												cu,
												i_part_offset,
												0 );

	return x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
}

uint32_t x265_enc_search_x_update_cand_list(x265_enc_search_t* enc_search,
											uint32_t i_mode,
											double f_cost,
											uint32_t i_fast_cand_num,
											uint32_t *cand_mode_list,
											double *cand_cost_list )
{
	uint32_t loop = 0;
	uint32_t i_shift = 0;

	while ( i_shift<i_fast_cand_num
			&& f_cost < cand_cost_list[ i_fast_cand_num-1-i_shift ] )
	{
		i_shift++;
	}

	if( i_shift!=0 )
	{
		for( loop = 1; loop < i_shift; loop++)
		{
			cand_mode_list[i_fast_cand_num - loop] = cand_mode_list[i_fast_cand_num - 1 - loop];
			cand_cost_list[i_fast_cand_num - loop ] = cand_cost_list[i_fast_cand_num - 1 - loop];
		}
		cand_mode_list[i_fast_cand_num - i_shift] = i_mode;
		cand_cost_list[i_fast_cand_num - i_shift] = f_cost;
		return 1;
 	}

	return 0;
}


void x265_enc_search_set_wp_scaling_dist_param ( x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t* cu,
												int32_t i_ref_idx,
												enum ref_pic_list_e ref_pic_list_cur )
{
	if ( i_ref_idx<0 )
	{
		enc_search->dist_param.b_apply_weight = 0 ;
		return;
	}

	x265_slice_t *slice = h->slice ;
	x265_pps_t *pps = &h->pps[0] ;
	x265_wp_scaling_param_t *wp0 , *wp1;
	enc_search->dist_param.b_apply_weight =
			( slice->i_slice_type == P_SLICE && pps->b_use_weight_pred )
			|| ( slice->i_slice_type == B_SLICE && pps->b_use_weighted_bi_pred ) ;
	if ( !enc_search->dist_param.b_apply_weight )
	{
		return ;
	}

	int32_t i_ref_idx0 = ( ref_pic_list_cur == REF_PIC_LIST_0 ) ? i_ref_idx : (-1);
	int32_t i_ref_idx1 = ( ref_pic_list_cur == REF_PIC_LIST_1 ) ? i_ref_idx : (-1);

	x265_weight_prediction_get_wp_scaling( h,
											(x265_weight_prediction_t*)enc_search,
											cu, &i_ref_idx0, &i_ref_idx1,
											&wp0 , &wp1 );

	if ( i_ref_idx0 < 0 ) wp0 = NULL;
	if ( i_ref_idx1 < 0 ) wp1 = NULL;

	enc_search->dist_param.wp_scaling_param  = NULL;

	if ( ref_pic_list_cur == REF_PIC_LIST_0 )
	{
		enc_search->dist_param.wp_scaling_param = wp0;
	}
	else
	{
		enc_search->dist_param.wp_scaling_param = wp1;
	}
}

