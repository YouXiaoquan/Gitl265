

#include "common/common.h"
#include "enc_top.h"

void enc_top_set_default_scaling_list ( x265_t *h )
{
	uint32_t i_size_id = 0 ;
	uint32_t i_list_id = 0 ;

	for(i_size_id = 0; i_size_id < SCALING_LIST_SIZE_NUM; i_size_id++)
	{
		for( i_list_id = 0; i_list_id < x265_scaling_list_get_scaling_list_num (i_size_id) ; ++ i_list_id )
		{
			process_default_marix ( &h->scaling_list, i_size_id, i_list_id ) ;
		}
	}
}

int create_wpp_coders ( x265_t *h, int32_t i_num_substreams )
{
	int loop = 0 ;
	int32_t i_depth = 0 ;
	int32_t i_ci_idx = 0 ;

	if ( NULL != h->pc_sbac_coders )
	{
		return 0 ; // already generated.
	}

	h->enc_top.i_num_substreams = i_num_substreams ;

    CHECKED_MALLOCZERO ( h->pc_sbac_coders,
    					i_num_substreams * sizeof(x265_enc_sbac_t*) );
	for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		h->pc_sbac_coders[loop] = (x265_enc_sbac_t*)x265_enc_sbac_new () ;
		if ( NULL == h->pc_sbac_coders[loop] )
		{
			goto fail ;
		}
	}

    CHECKED_MALLOCZERO ( h->pc_bin_coder_cabacs,
    					i_num_substreams * sizeof(x265_enc_bin_cabac_t*) );
	for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		h->pc_bin_coder_cabacs[loop] = (x265_enc_bin_cabac_t*)x265_enc_bin_cabac_new () ;
		if ( NULL == h->pc_bin_coder_cabacs[loop] )
		{
			goto fail ;
		}
	}

    CHECKED_MALLOCZERO ( h->pc_rd_go_on_sbac_coders,
    					i_num_substreams * sizeof(x265_enc_sbac_t*) );
	for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		h->pc_rd_go_on_sbac_coders[loop] = (x265_enc_sbac_t*)x265_enc_sbac_new () ;
		if ( NULL == h->pc_rd_go_on_sbac_coders[loop] )
		{
			goto fail ;
		}
	}

    CHECKED_MALLOCZERO ( h->pc_rd_go_on_bin_coders_cabac,
    					i_num_substreams * sizeof(x265_enc_bin_cabac_t*) );
	for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		h->pc_rd_go_on_bin_coders_cabac[loop] = (x265_enc_bin_cabac_t*)x265_enc_bin_cabac_new () ;
		if ( NULL == h->pc_rd_go_on_bin_coders_cabac[loop] )
		{
			goto fail ;
		}
	}

    CHECKED_MALLOCZERO ( h->pc_bit_counters,
    					i_num_substreams * sizeof(x265_bit_counter_t*) );
	for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		h->pc_bit_counters[loop] = (x265_bit_counter_t*)x265_bit_counter_new () ;
		if ( NULL == h->pc_bit_counters[loop] )
		{
			goto fail ;
		}
	}

    CHECKED_MALLOCZERO ( h->rd_costs,
    					i_num_substreams * sizeof(x265_rd_cost_t*) );
	for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		h->rd_costs[loop] = x265_rd_cost_new () ;
		if ( NULL == h->rd_costs[loop] )
		{
			goto fail ;
		}
	}
	for ( loop = 0 ; loop < i_num_substreams; ++loop )
	{
		enc_sbac_init ( h->pc_rd_go_on_sbac_coders[loop],
						(x265_enc_bin_if_t*)h->pc_rd_go_on_bin_coders_cabac[loop]) ;

		enc_sbac_init ( h->pc_sbac_coders[loop],
						(x265_enc_bin_if_t*)h->pc_bin_coder_cabacs[loop]) ;
	}

	if( h->param.b_use_sbac_rd )
	{
	    CHECKED_MALLOCZERO ( h->ppppc_rd_sbac_coders,
	    					i_num_substreams * sizeof(x265_enc_entropy_if_t***) );
	    CHECKED_MALLOCZERO ( h->ppppc_bin_coders_cabac,
	    					i_num_substreams * sizeof(x265_enc_bin_if_t***) );

		for ( loop = 0 ; loop < i_num_substreams ; ++loop )
		{
		    CHECKED_MALLOCZERO ( h->ppppc_rd_sbac_coders[loop],
		    					(h->cu.pic.i_total_depth + 1) * sizeof(x265_enc_entropy_if_t**) );
		    CHECKED_MALLOCZERO ( h->ppppc_bin_coders_cabac[loop],
		    					(h->cu.pic.i_total_depth + 1) * sizeof(x265_enc_bin_if_t**) );

			for ( i_depth = 0; i_depth < h->cu.pic.i_total_depth + 1 ; ++ i_depth )
			{
			    CHECKED_MALLOCZERO ( h->ppppc_rd_sbac_coders[loop][i_depth],
			    					CI_NUM * sizeof(x265_enc_entropy_if_t*) );
			    CHECKED_MALLOCZERO ( h->ppppc_bin_coders_cabac[loop][i_depth],
			    					CI_NUM * sizeof(x265_enc_bin_if_t*) );

				for ( i_ci_idx = 0; i_ci_idx < CI_NUM; ++ i_ci_idx )
				{
					h->ppppc_rd_sbac_coders[loop][i_depth][i_ci_idx] = (x265_enc_sbac_t*)x265_enc_sbac_new () ;
					h->ppppc_bin_coders_cabac[loop][i_depth][i_ci_idx] = (x265_enc_bin_cabac_t*)x265_enc_bin_cabac_new () ;
					enc_sbac_init( h->ppppc_rd_sbac_coders[loop][i_depth][i_ci_idx],
							(x265_enc_bin_if_t*)h->ppppc_bin_coders_cabac[loop][i_depth][i_ci_idx] ) ;
				}
			}
		}
	}

	return 0 ;
fail:
	return -1 ;
}


int x265_enc_top_init ( x265_t *h, x265_param_t *param )
{
	h->enc_top.i_gop_size = 1 ;
	h->enc_top.i_poc_last = -1 ;

	x265_enc_bin_if_build_cabac_transition_table () ;
	if ( init_scaling_list ( &h->scaling_list ) )
	{
		goto fail ;
	}

	if ( x265_enc_search_init ( h, &h->enc_search ) )
	{
		goto fail ;
	}
	if ( x265_enc_cavlc_init ( (x265_enc_entropy_if_t*)&h->enc_cavlc ) )
	{
		goto fail ;
	}
	h->pc_enc_cavlc = &h->enc_cavlc ;

	if ( x265_tr_quant_init ( h, &h->tr_quant ) )
	{
		goto fail ;
	}
	if ( x265_loop_filter_init ( &h->loop_filter ) )
	{
		goto fail ;
	}
	if ( x265_enc_sample_adaptive_offset_init (&h->enc_sao ) )
	{
		goto fail ;
	}
	if ( x265_enc_entropy_init (&h->enc_entropy ) )
	{
		goto fail ;
	}
	if ( x265_enc_sbac_init ( (x265_enc_entropy_if_t*)&h->enc_sbac ) )
	{
		goto fail ;
	}
	if ( x265_enc_bin_cabac_init ( (x265_enc_bin_if_t*)(&h->enc_bin_cabac) ) )
	{
		goto fail ;
	}
	if ( x265_enc_slice_init ( h, &h->enc_slice ) )
	{
		goto fail ;
	}
	if ( x265_enc_cu_init ( &h->enc_cu ) )
	{
		goto fail ;
	}
	if ( x265_bit_counter_init ( (x265_bit_if_t*)&h->bit_counter ) )
	{
		goto fail ;
	}
	if ( x265_rd_cost_init ( &h->rd_cost ) )
	{
		goto fail ;
	}
	if ( x265_enc_sbac_init ( (x265_enc_entropy_if_t*)&h->rd_go_on_sbac_coder ) )
	{
		goto fail ;
	}
#if X265_FAST_BIT_EST
	if ( x265_enc_bin_cabac_counter_init ( (x265_enc_bin_if_t*)(&h->rd_go_on_bin_coder_cabac) ) )
	{
		goto fail ;
	}
#else
	if ( x265_enc_bin_cabac_init ( &h->rd_go_on_bin_coder_cabac ) )
	{
		goto fail ;
	}
#endif
	if ( x265_enc_preanalyzer_init ( &h->enc_preanalyzer ) )
	{
		goto fail ;
	}
	if ( x265_enc_rate_ctrl_init ( &h->enc_rate_ctrl ) )
	{
		goto fail ;
	}

    if ( x265_enc_gop_init ( h, &h->enc_gop ) )
    {
    	goto fail ;
    }

	if ( x265_enc_top_create ( h ) )
	{
		goto fail ;
	}

	if ( x265_enc_top_initialize(h) )
	{
		goto fail ;
	}

	enc_sbac_init (&h->rd_go_on_sbac_coder,
					(x265_enc_bin_if_t*)&h->rd_go_on_bin_coder_cabac) ;


	return 0 ;

fail:
	x265_enc_top_deinit ( h ) ;
	return -1 ;

}

void x265_enc_top_deinit ( x265_t *h )
{
	x265_enc_top_deinitialize ( h ) ;
	x265_enc_top_destroy ( h ) ;

	x265_enc_gop_deinit ( h, &h->enc_gop ) ;


#if X265_FAST_BIT_EST
	x265_enc_bin_cabac_counter_deinit ( (x265_enc_bin_if_t*)(&h->rd_go_on_bin_coder_cabac) ) ;
#else
	x265_enc_bin_cabac_deinit ( &h->rd_go_on_bin_coder_cabac ) ;
#endif

	x265_enc_rate_ctrl_deinit ( &h->enc_rate_ctrl ) ;
	x265_enc_preanalyzer_deinit ( &h->enc_preanalyzer ) ;
	x265_enc_sbac_deinit ( (x265_enc_entropy_if_t*)&h->rd_go_on_sbac_coder ) ;
	x265_rd_cost_deinit ( &h->rd_cost ) ;
	x265_bit_counter_deinit ( (x265_bit_if_t*)&h->bit_counter ) ;
	x265_enc_cu_deinit ( &h->enc_cu ) ;
	x265_enc_slice_deinit ( &h->enc_slice ) ;
	x265_enc_bin_cabac_deinit ( (x265_enc_bin_if_t*)(&h->enc_bin_cabac) ) ;
	x265_enc_sbac_deinit ( (x265_enc_entropy_if_t*)&h->enc_sbac ) ;
	x265_enc_entropy_deinit ( &h->enc_entropy ) ;
	x265_enc_sample_adaptive_offset_deinit ( &h->enc_sao ) ;
	x265_loop_filter_deinit ( &h->loop_filter ) ;
	x265_tr_quant_deinit ( &h->tr_quant ) ;
	x265_enc_cavlc_deinit ( (x265_enc_entropy_if_t*)&h->enc_cavlc ) ;
	x265_enc_search_deinit ( h, &h->enc_search ) ;


	deinit_scaling_list ( &h->scaling_list ) ;
}


int x265_enc_top_create ( x265_t *h )
{
	int32_t i_depth = 0 ;
	int32_t i_ci_idx = 0 ;

	if ( x265_enc_gop_create ( h, &h->enc_gop ) )
	{
		goto fail ;
	}
	if ( x265_enc_cu_create ( h, &h->enc_cu ) )
	{
		goto fail ;
	}

	if ( h->param.sps.b_use_sao )
	{
		h->enc_sao.sample_adaptive_offset.b_sao_lcu_boundary = h->param.sao.i_sao_lcu_boundary ;
		h->enc_sao.sample_adaptive_offset.b_sao_lcu_based_optimization = h->param.sao.i_sao_lcu_based_optimization ;
		h->enc_sao.sample_adaptive_offset.i_max_num_offsets_per_pic = h->param.sao.i_max_num_offsets_per_pic ;
		if ( x265_sample_adaptive_offset_create ( h,
												(x265_sample_adaptive_offset_t*)&h->enc_sao ) )
		{
			goto fail ;
		}
		if ( x265_enc_sample_adaptive_offset_create_enc_buffer ( h, &h->enc_sao ) )
		{
			goto fail ;
		}
	}

#if X265_ADAPTIVE_QP_SELECTION
	if ( h->param.b_use_adapt_qp_select )
	{
		x265_tr_quant_init_slice_qp_delta ( &h->tr_quant ) ;
	}
#endif
	if ( x265_loop_filter_create ( &h->loop_filter, h->cu.pic.i_total_depth ) )
	{
		goto fail ;
	}

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	if ( x265_enc_rate_ctrl_initialize ( h, &h->enc_rate_ctrl ) )
	{
		goto fail ;
	}
#else
	if ( x265_enc_rate_ctrl_create ( h, &h->enc_rate_ctrl ) )
	{
		goto fail ;
	}
#endif

	// if SBAC-based RD optimization is used
	if( h->param.b_use_sbac_rd )
	{
	    CHECKED_MALLOCZERO ( h->pppc_rd_sbac_coder,
	    					(h->cu.pic.i_total_depth + 1) * sizeof(x265_enc_entropy_if_t**) );
#if X265_FAST_BIT_EST
	    CHECKED_MALLOCZERO ( h->pppc_bin_coder_cabac,
	    					(h->cu.pic.i_total_depth + 1) * sizeof(x265_enc_bin_cabac_counter_t**) );
#else
	    CHECKED_MALLOCZERO ( h->pppc_bin_coder_cabac,
	    					(h->cu.pic.i_total_depth + 1) * sizeof(x265_enc_bin_cabac_t**) );
#endif
		for ( i_depth = 0 ; i_depth < h->cu.pic.i_total_depth + 1 ; ++ i_depth )
		{
		    CHECKED_MALLOCZERO ( h->pppc_rd_sbac_coder[i_depth],
		    					CI_NUM * sizeof(x265_enc_entropy_if_t*) );
#if X265_FAST_BIT_EST
		    CHECKED_MALLOCZERO ( h->pppc_bin_coder_cabac[i_depth],
		    					CI_NUM * sizeof(x265_enc_bin_cabac_counter_t*) );
#else
		    CHECKED_MALLOCZERO ( h->pppc_bin_coder_cabac[i_depth],
		    					CI_NUM * sizeof(x265_enc_bin_cabac_if_t*) );
#endif
			for ( i_ci_idx = 0; i_ci_idx < CI_NUM ; ++ i_ci_idx )
			{
				h->pppc_rd_sbac_coder[i_depth][i_ci_idx] = (x265_enc_sbac_t*)x265_enc_sbac_new () ;

#if X265_FAST_BIT_EST
				h->pppc_bin_coder_cabac[i_depth][i_ci_idx] = (x265_enc_bin_cabac_counter_t*)x265_enc_bin_cabac_counter_new () ;
#else
				h->pppc_bin_coder_cabac[i_depth][i_ci_idx] = (x265_enc_bin_cabac_t*)x265_enc_bin_cabac_new () ;
#endif
				enc_sbac_init ( h->pppc_rd_sbac_coder[i_depth][i_ci_idx],
								(x265_enc_bin_if_t*)h->pppc_bin_coder_cabac[i_depth][i_ci_idx] ) ;
			}
		}
	}
	return 0 ;

fail:
	x265_enc_top_destroy ( h ) ;
	return -1 ;
}


void x265_enc_top_destroy ( x265_t *h )
{
	int32_t loop = 0 ;
	int32_t i_depth = 0 ;
	int32_t i_ci_idx = 0 ;

	if( h->param.b_use_sbac_rd )
	{
	    if ( h->ppppc_bin_coders_cabac )
	    {
			for ( loop = 0 ; loop < h->enc_top.i_num_substreams ; ++loop )
			{
				if ( h->ppppc_bin_coders_cabac[loop] )
				{
					for ( i_depth = 0; i_depth < h->cu.pic.i_total_depth + 1 ; ++ i_depth )
					{
						if ( h->ppppc_bin_coders_cabac[loop][i_depth] )
						{
							for ( i_ci_idx = 0; i_ci_idx < CI_NUM; ++ i_ci_idx )
							{
								x265_enc_bin_cabac_delete ( (x265_enc_bin_if_t*)h->ppppc_bin_coders_cabac[loop][i_depth][i_ci_idx] ) ;
							}
							x265_free ( h->ppppc_bin_coders_cabac[loop][i_depth] ) ;
						}
					}
					x265_free ( h->ppppc_bin_coders_cabac[loop] ) ;
				}
			}
			x265_free ( h->ppppc_bin_coders_cabac ) ;
	    }
	    if ( h->ppppc_rd_sbac_coders )
	    {
			for ( loop = 0 ; loop < h->enc_top.i_num_substreams ; ++loop )
			{
				if ( h->ppppc_rd_sbac_coders[loop] )
				{
					for ( i_depth = 0; i_depth < h->cu.pic.i_total_depth + 1 ; ++ i_depth )
					{
						if ( h->ppppc_rd_sbac_coders[loop][i_depth] )
						{
							for ( i_ci_idx = 0; i_ci_idx < CI_NUM; ++ i_ci_idx )
							{
								x265_enc_sbac_delete ( (x265_enc_entropy_if_t*)h->ppppc_rd_sbac_coders[loop][i_depth][i_ci_idx] ) ;
							}
							x265_free ( h->ppppc_rd_sbac_coders[loop][i_depth] ) ;
						}
					}
					x265_free ( h->ppppc_rd_sbac_coders[loop] ) ;
				}
			}
			x265_free ( h->ppppc_rd_sbac_coders ) ;
	    }
	}
	for ( loop = 0 ; loop < h->enc_top.i_num_substreams ; ++ loop )
	{
		x265_rd_cost_delete ( h->rd_costs[loop] ) ;
	}
	x265_free ( h->rd_costs ) ;
	for ( loop = 0 ; loop < h->enc_top.i_num_substreams ; ++ loop )
	{
		x265_bit_counter_delete ( (x265_bit_if_t*)h->pc_bit_counters[loop] ) ;
	}
	x265_free ( h->pc_bit_counters ) ;
	for ( loop = 0 ; loop < h->enc_top.i_num_substreams ; ++ loop )
	{
		x265_enc_bin_cabac_delete ( (x265_enc_bin_if_t*)h->pc_rd_go_on_bin_coders_cabac[loop] ) ;
	}
	x265_free ( h->pc_rd_go_on_bin_coders_cabac ) ;
	for ( loop = 0 ; loop < h->enc_top.i_num_substreams ; ++ loop )
	{
		x265_enc_sbac_delete ( (x265_enc_entropy_if_t*)h->pc_rd_go_on_sbac_coders[loop] ) ;
	}
	x265_free ( h->pc_rd_go_on_sbac_coders ) ;
	for ( loop = 0 ; loop < h->enc_top.i_num_substreams ; ++ loop )
	{
		x265_enc_bin_cabac_delete ( (x265_enc_bin_if_t*)h->pc_bin_coder_cabacs[loop] ) ;
	}
	x265_free ( h->pc_bin_coder_cabacs ) ;
	for ( loop = 0 ; loop < h->enc_top.i_num_substreams ; ++ loop )
	{
		x265_enc_sbac_delete ( (x265_enc_entropy_if_t*)h->pc_sbac_coders[loop] ) ;
	}
	x265_free ( h->pc_sbac_coders ) ;

	if ( h->pppc_bin_coder_cabac )
	{
		for ( i_depth = 0; i_depth < h->cu.pic.i_total_depth + 1 ; ++ i_depth )
		{
			if ( h->pppc_bin_coder_cabac[i_depth] )
			{
				for ( i_ci_idx = 0; i_ci_idx < CI_NUM; ++ i_ci_idx )
				{
#if X265_FAST_BIT_EST
					x265_enc_bin_cabac_counter_delete ( (x265_enc_bin_if_t*)h->pppc_bin_coder_cabac[i_depth][i_ci_idx] ) ;
#else
					x265_enc_bin_cabac_delete ( h->pppc_bin_coder_cabac[i_depth][i_ci_idx] ) ;
#endif
				}
				x265_free ( h->pppc_bin_coder_cabac[i_depth] ) ;
			}
		}
		x265_free ( h->pppc_bin_coder_cabac ) ;
	}
	if ( h->pppc_rd_sbac_coder )
	{
		for ( i_depth = 0; i_depth < h->cu.pic.i_total_depth + 1 ; ++ i_depth )
		{
			if ( h->pppc_rd_sbac_coder[i_depth] )
			{
				for ( i_ci_idx = 0; i_ci_idx < CI_NUM; ++ i_ci_idx )
				{
					x265_enc_sbac_delete ( (x265_enc_entropy_if_t*)h->pppc_rd_sbac_coder[i_depth][i_ci_idx] ) ;
				}
				x265_free ( h->pppc_rd_sbac_coder[i_depth] ) ;
			}
		}
		x265_free ( h->pppc_rd_sbac_coder ) ;
	}


#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	x265_enc_rate_ctrl_deinitialize ( h, &h->enc_rate_ctrl ) ;
#else
	x265_enc_rate_ctrl_destroy ( h, &h->enc_rate_ctrl ) ;
#endif

	x265_loop_filter_destroy ( &h->loop_filter ) ;
	if ( h->param.sps.b_use_sao )
	{
		x265_enc_sample_adaptive_offset_destroy_enc_buffer ( h, &h->enc_sao ) ;
		x265_sample_adaptive_offset_destroy ( h,
											(x265_sample_adaptive_offset_t*)&h->enc_sao ) ;
	}
	x265_enc_cu_destroy ( h, &h->enc_cu ) ;
	x265_enc_gop_destroy ( h, &h->enc_gop ) ;
}

int x265_enc_top_initialize ( x265_t *h )
{
	if ( x265_enc_gop_initialize ( h, &h->enc_gop ) )
	{
		goto fail ;
	}

	if ( x265_enc_cu_initialize ( h, &h->enc_cu ) )
	{
		goto fail ;
	}

	if ( x265_tr_quant_initialize ( h, &h->tr_quant ) )
	{
		goto fail ;
	}

	if ( x265_enc_search_initialize ( h,
									&h->enc_search,
									&h->tr_quant,
									h->param.i_search_range,
									h->param.i_bipred_search_range,
									h->param.i_fast_search,
									0,
									&h->enc_entropy,
									&h->rd_cost,
									(x265_enc_sbac_t***)h->pppc_rd_sbac_coder,
									(x265_enc_sbac_t*)&h->rd_go_on_sbac_coder ) )
	{
		goto fail ;
	}

	h->enc_top.i_max_ref_pic_num = 0 ;

	return 0 ;

fail:
	x265_enc_top_deinitialize ( h ) ;
	return -1 ;
}

void x265_enc_top_deinitialize ( x265_t *h )
{

	x265_tr_quant_deinitialize ( h, &h->tr_quant ) ;
	x265_enc_cu_deinitialize ( h, &h->enc_cu ) ;
	x265_enc_gop_deinitialize ( h, &h->enc_gop ) ;


}

void x265_enc_top_print_summary(x265_t *h)
{
	x265_enc_gop_print_out_summary( h,
									&h->enc_gop,
									h->enc_top.i_num_all_pic_coded ) ;

}




