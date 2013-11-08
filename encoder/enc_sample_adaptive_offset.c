

#include "common/common.h"



void x265_enc_sample_adaptive_offset_set_max_num_offsets_per_pic(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
																int32_t i_val)
{
	enc_sample_adaptive_offset->sample_adaptive_offset.i_max_num_offsets_per_pic = i_val ;
}


int32_t x265_enc_sample_adaptive_offset_get_max_num_offsets_per_pic(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset)
{
	return enc_sample_adaptive_offset->sample_adaptive_offset.i_max_num_offsets_per_pic;
}


x265_enc_sample_adaptive_offset_t *x265_enc_sample_adaptive_offset_new ()
{
	x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset = NULL ;

	CHECKED_MALLOCZERO( enc_sample_adaptive_offset, sizeof(x265_enc_sample_adaptive_offset_t) );
	if ( x265_enc_sample_adaptive_offset_init ( enc_sample_adaptive_offset ) )
	{
		goto fail ;
	}
	return enc_sample_adaptive_offset ;
fail:
	x265_enc_sample_adaptive_offset_delete ( (void*) enc_sample_adaptive_offset ) ;
	return NULL ;
}


void x265_enc_sample_adaptive_offset_delete ( x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset )
{
	x265_free ( enc_sample_adaptive_offset ) ;
}


int x265_enc_sample_adaptive_offset_init ( x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset )
{
	if ( x265_sample_adaptive_offset_init ( (x265_sample_adaptive_offset_t *)enc_sample_adaptive_offset ) )
	{
		goto fail ;
	}
	enc_sample_adaptive_offset->f_Lambda_luma = 0.0 ;
	enc_sample_adaptive_offset->f_Lambda_chroma = 0.0 ;
	enc_sample_adaptive_offset->pppc_rd_sbac_coder = NULL ;
#if X265_FAST_BIT_EST
	enc_sample_adaptive_offset->pppc_bin_coder_cabac = NULL ;
#else
	enc_sample_adaptive_offset->pppc_bin_coder_cabac = NULL ;
#endif

	enc_sample_adaptive_offset->count = NULL ;
	enc_sample_adaptive_offset->offset = NULL ;
	enc_sample_adaptive_offset->offset_org = NULL ;
	enc_sample_adaptive_offset->count_pre_dblk = NULL ;
	enc_sample_adaptive_offset->offset_org_pre_dblk = NULL ;
	enc_sample_adaptive_offset->rate = NULL ;
	enc_sample_adaptive_offset->dist = NULL ;
	enc_sample_adaptive_offset->cost = NULL ;
	enc_sample_adaptive_offset->cost_part_best = NULL ;
	enc_sample_adaptive_offset->dist_org = NULL ;
	enc_sample_adaptive_offset->type_part_best = NULL ;
	enc_sample_adaptive_offset->i_offset_th_y = 0 ;
	enc_sample_adaptive_offset->i_offset_th_c = 0 ;
	enc_sample_adaptive_offset->b_use_sbac_rd = 0 ;
#if X265_SAO_ENCODING_CHOICE
#if X265_SAO_ENCODING_CHOICE_CHROMA
	enc_sample_adaptive_offset->depth_sao_rate[0][0] = 0.0;
	enc_sample_adaptive_offset->depth_sao_rate[0][1] = 0.0;
	enc_sample_adaptive_offset->depth_sao_rate[0][2] = 0.0;
	enc_sample_adaptive_offset->depth_sao_rate[0][3] = 0.0;
	enc_sample_adaptive_offset->depth_sao_rate[1][0] = 0.0;
	enc_sample_adaptive_offset->depth_sao_rate[1][1] = 0.0;
	enc_sample_adaptive_offset->depth_sao_rate[1][2] = 0.0;
	enc_sample_adaptive_offset->depth_sao_rate[1][3] = 0.0;
#else
	enc_sample_adaptive_offset->f_epth_0_sao_rate = 0.0 ;
#endif
#endif

    return 0 ;

fail:
	x265_enc_sample_adaptive_offset_deinit ( enc_sample_adaptive_offset ) ;
	return -1 ;
}

void x265_enc_sample_adaptive_offset_deinit ( x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset )
{

}

double x265_enc_sample_adaptive_offset_x_round_ibdi2(int32_t bit_depth, double x)
{
	return ((x)>0) ? (int32_t)(((int32_t)(x)+(1<<(bit_depth-8-1)))/(1<<(bit_depth-8))) : ((int32_t)(((int32_t)(x)-(1<<(bit_depth-8-1)))/(1<<(bit_depth-8))));
}


double x265_enc_sample_adaptive_offset_x_round_ibdi(int32_t bit_depth, double x)
{
	return (bit_depth > 8 ? x265_enc_sample_adaptive_offset_x_round_ibdi2(bit_depth, (x))
							: ((x)>=0 ? ((int32_t)((x)+0.5))
										: ((int32_t)((x)-0.5)))) ;
}


int x265_enc_sample_adaptive_offset_create_enc_buffer ( x265_t *h, x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset )
{
	int32_t i = 0, j = 0 ;
	int32_t i_max_depth = 0;
	int32_t i_depth = 0;
	int32_t i_ci_idx = 0;

	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->dist_org,
						sizeof(int64_t) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->cost_part_best,
						sizeof(double) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->type_part_best,
						sizeof(int32_t) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);

	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->rate,
						sizeof(int64_t*) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->dist,
						sizeof(int64_t*) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->cost,
						sizeof(double*) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);

	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->count,
						sizeof(int64_t**) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset,
						sizeof(int64_t**) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset_org,
						sizeof(int64_t**) * enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts);


	for ( i = 0 ;i < enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts ; ++ i )
	{
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->rate[i],
							sizeof(int64_t) * MAX_NUM_SAO_TYPE);
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->dist[i],
							sizeof(int64_t) * MAX_NUM_SAO_TYPE);
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->cost[i],
							sizeof(double) * MAX_NUM_SAO_TYPE);

		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->count[i],
							sizeof(int64_t*) * MAX_NUM_SAO_TYPE);
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset[i],
							sizeof(int64_t*) * MAX_NUM_SAO_TYPE);
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset_org[i],
							sizeof(int64_t*) * MAX_NUM_SAO_TYPE);

	    for ( j = 0; j < MAX_NUM_SAO_TYPE ; ++j )
	    {
			CHECKED_MALLOCZERO( enc_sample_adaptive_offset->count[i][j],
								sizeof(int64_t) * X265_MAX_NUM_SAO_CLASS);
			CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset[i][j],
								sizeof(int64_t) * X265_MAX_NUM_SAO_CLASS);
			CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset_org[i][j],
								sizeof(int64_t) * X265_MAX_NUM_SAO_CLASS);
	    }
	}

	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->count_pre_dblk,
						sizeof(int64_t**) * 3);
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset_org_pre_dblk,
						sizeof(int64_t**) * 3);
	for (i = 0 ; i < 3 ; ++i )
	{
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->count_pre_dblk[i],
							sizeof(int64_t*) * MAX_NUM_SAO_TYPE);
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset_org_pre_dblk[i],
							sizeof(int64_t*) * MAX_NUM_SAO_TYPE);
		for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
		{
			CHECKED_MALLOCZERO( enc_sample_adaptive_offset->count_pre_dblk[i][j],
								sizeof(int64_t) * X265_MAX_NUM_SAO_CLASS);
			CHECKED_MALLOCZERO( enc_sample_adaptive_offset->offset_org_pre_dblk[i][j],
								sizeof(int64_t) * X265_MAX_NUM_SAO_CLASS);
		}
	}

	i_max_depth = 4;
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->pppc_rd_sbac_coder,
						sizeof(x265_enc_sbac_t**) * (i_max_depth+1));
#if X265_FAST_BIT_EST
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->pppc_bin_coder_cabac,
						sizeof(x265_enc_bin_cabac_counter_t**) * (i_max_depth+1));
#else
	CHECKED_MALLOCZERO( enc_sample_adaptive_offset->pppc_bin_coder_cabac,
						sizeof(x265_enc_bin_cabac_t**) * (i_max_depth+1));
#endif

	for ( i_depth = 0; i_depth < i_max_depth + 1; i_depth++ )
	{
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth],
							sizeof(x265_enc_sbac_t*) * CI_NUM);
#if X265_FAST_BIT_EST
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth],
							sizeof(x265_enc_bin_cabac_counter_t*) * CI_NUM);
#else
		CHECKED_MALLOCZERO( enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth],
							sizeof(x265_enc_bin_cabac_t*) * CI_NUM);
#endif
		for ( i_ci_idx = 0; i_ci_idx < CI_NUM; i_ci_idx ++ )
		{
			enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth][i_ci_idx] = (x265_enc_sbac_t*)x265_enc_sbac_new ();
			if ( NULL == enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth][i_ci_idx] )
			{
				goto fail ;
			}
#if X265_FAST_BIT_EST
			enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx] = (x265_enc_bin_cabac_counter_t*)x265_enc_bin_cabac_counter_new ();
			if ( NULL == enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx] )
			{
				goto fail ;
			}
#else
			enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx] = (x265_enc_bin_cabac_t*)x265_enc_bin_cabac_new ();
			if ( NULL == enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx] )
			{
				goto fail ;
			}
#endif
			enc_sbac_init( enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth][i_ci_idx],
					(x265_enc_bin_if_t*)enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx] ) ;
	    }
	}

	return 0 ;

fail:
	x265_enc_sample_adaptive_offset_destroy_enc_buffer(h, enc_sample_adaptive_offset) ;
	return -1 ;

}

void x265_enc_sample_adaptive_offset_destroy_enc_buffer ( x265_t *h, x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset )
{
	int32_t loop = 0;
	int32_t i = 0, j = 0 ;
	int32_t i_max_depth = 0;
	int32_t i_depth = 0;
	int32_t i_ci_idx = 0;

	if (enc_sample_adaptive_offset->pppc_rd_sbac_coder)
	{
		for ( i_depth = 0; i_depth < i_max_depth + 1; i_depth++ )
		{
			if (enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth])
			{
				for ( i_ci_idx = 0; i_ci_idx < CI_NUM; i_ci_idx ++ )
				{
					x265_enc_sbac_delete((x265_enc_entropy_if_t*)enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth][i_ci_idx]);
					enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth][i_ci_idx] = NULL ;
				}
				x265_free (enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth]) ;
				enc_sample_adaptive_offset->pppc_rd_sbac_coder[i_depth] = NULL ;
			}
		}
		x265_free (enc_sample_adaptive_offset->pppc_rd_sbac_coder) ;
		enc_sample_adaptive_offset->pppc_rd_sbac_coder = NULL ;
	}

	if (enc_sample_adaptive_offset->pppc_bin_coder_cabac)
	{
		for ( i_depth = 0; i_depth < i_max_depth + 1; i_depth++ )
		{
			if (enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth])
			{
				for ( i_ci_idx = 0; i_ci_idx < CI_NUM; i_ci_idx ++ )
				{
#if X265_FAST_BIT_EST
					x265_enc_bin_cabac_counter_delete((x265_enc_bin_if_t*)enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx]);
					enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx] = NULL ;
#else
					x265_enc_bin_cabac_delete((x265_enc_bin_if_t*)enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx]);
					enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth][i_ci_idx] = NULL ;
#endif
				}
				x265_free (enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth]) ;
				enc_sample_adaptive_offset->pppc_bin_coder_cabac[i_depth] = NULL ;
			}
		}
		x265_free (enc_sample_adaptive_offset->pppc_bin_coder_cabac) ;
		enc_sample_adaptive_offset->pppc_bin_coder_cabac = NULL ;
	}

	if (enc_sample_adaptive_offset->count_pre_dblk)
	{
		for (i = 0 ; i < 3 ; ++i )
		{
			if (enc_sample_adaptive_offset->count_pre_dblk[i])
			{
				for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
				{
					x265_free (enc_sample_adaptive_offset->count_pre_dblk[i][j]) ;
					enc_sample_adaptive_offset->count_pre_dblk[i][j] = NULL ;
				}
				x265_free (enc_sample_adaptive_offset->count_pre_dblk[i]) ;
				enc_sample_adaptive_offset->count_pre_dblk[i] = NULL ;
			}
		}
		x265_free (enc_sample_adaptive_offset->count_pre_dblk) ;
		enc_sample_adaptive_offset->count_pre_dblk = NULL ;
	}

	if (enc_sample_adaptive_offset->offset_org_pre_dblk)
	{
		for (i = 0 ; i < 3 ; ++i )
		{
			if (enc_sample_adaptive_offset->offset_org_pre_dblk[i])
			{
				for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
				{
					x265_free (enc_sample_adaptive_offset->offset_org_pre_dblk[i][j]) ;
					enc_sample_adaptive_offset->offset_org_pre_dblk[i][j] = NULL ;
				}
				x265_free (enc_sample_adaptive_offset->offset_org_pre_dblk[i]) ;
				enc_sample_adaptive_offset->offset_org_pre_dblk[i] = NULL ;
			}
		}
		x265_free (enc_sample_adaptive_offset->offset_org_pre_dblk) ;
		enc_sample_adaptive_offset->offset_org_pre_dblk = NULL ;
	}

	if (enc_sample_adaptive_offset->offset_org)
	{
		for (i = 0 ; i < enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts ; ++i )
		{
			if (enc_sample_adaptive_offset->offset_org[i])
			{
				for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
				{
					x265_free (enc_sample_adaptive_offset->offset_org[i][j]) ;
					enc_sample_adaptive_offset->offset_org[i][j] = NULL ;
				}
				x265_free (enc_sample_adaptive_offset->offset_org[i]) ;
				enc_sample_adaptive_offset->offset_org[i] = NULL ;
			}
		}
		x265_free (enc_sample_adaptive_offset->offset_org) ;
		enc_sample_adaptive_offset->offset_org = NULL ;
	}

	if (enc_sample_adaptive_offset->offset)
	{
		for (i = 0 ; i < enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts ; ++i )
		{
			if (enc_sample_adaptive_offset->offset[i])
			{
				for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
				{
					x265_free (enc_sample_adaptive_offset->offset[i][j]) ;
					enc_sample_adaptive_offset->offset[i][j] = NULL ;
				}
				x265_free (enc_sample_adaptive_offset->offset[i]) ;
				enc_sample_adaptive_offset->offset[i] = NULL ;
			}
		}
		x265_free (enc_sample_adaptive_offset->offset) ;
		enc_sample_adaptive_offset->offset = NULL ;
	}


	if (enc_sample_adaptive_offset->count)
	{
		for (i = 0 ; i < enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts ; ++i )
		{
			if (enc_sample_adaptive_offset->count[i])
			{
				for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
				{
					x265_free (enc_sample_adaptive_offset->count[i][j]) ;
					enc_sample_adaptive_offset->count[i][j] = NULL ;
				}
				x265_free (enc_sample_adaptive_offset->count[i]) ;
				enc_sample_adaptive_offset->count[i] = NULL ;
			}
		}
		x265_free (enc_sample_adaptive_offset->count) ;
		enc_sample_adaptive_offset->count = NULL ;
	}

	if (enc_sample_adaptive_offset->cost)
	{
		for (loop = 0 ; loop < enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts ; ++ loop)
		{
			x265_free (enc_sample_adaptive_offset->cost[loop]) ;
			enc_sample_adaptive_offset->cost[loop] = NULL ;
		}
		enc_sample_adaptive_offset->cost = NULL ;
	}

	if (enc_sample_adaptive_offset->dist)
	{
		for (loop = 0 ; loop < enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts ; ++ loop)
		{
			x265_free (enc_sample_adaptive_offset->dist[loop]) ;
			enc_sample_adaptive_offset->dist[loop] = NULL ;
		}
		enc_sample_adaptive_offset->dist = NULL ;
	}

	if (enc_sample_adaptive_offset->rate)
	{
		for (loop = 0 ; loop < enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts ; ++ loop)
		{
			x265_free (enc_sample_adaptive_offset->rate[loop]) ;
			enc_sample_adaptive_offset->rate[loop] = NULL ;
		}
		enc_sample_adaptive_offset->rate = NULL ;
	}

	x265_free (enc_sample_adaptive_offset->type_part_best) ;
	enc_sample_adaptive_offset->type_part_best = NULL ;
	x265_free (enc_sample_adaptive_offset->cost_part_best) ;
	enc_sample_adaptive_offset->cost_part_best = NULL ;
	x265_free (enc_sample_adaptive_offset->dist_org) ;
	enc_sample_adaptive_offset->dist_org = NULL ;
}


void x265_enc_sample_adaptive_offset_start_sao_enc ( x265_t *h, x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset )
{
	int32_t i_depth = 0 ;
	x265_sao_param_t *sao_param = NULL ;

	enc_sample_adaptive_offset->b_use_sbac_rd = 1 ;
	x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
										(x265_enc_entropy_if_t*)(&h->rd_go_on_sbac_coder),
										h->slice );
	x265_enc_entropy_reset_entropy ( h,
									&h->enc_entropy,
									h->slice->i_slice_type,
									h->slice->i_slice_qp );
	if ( enc_sample_adaptive_offset->b_use_sbac_rd )
	{
		x265_enc_sbac_store (&h->rd_go_on_sbac_coder,
							enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_NEXT_BEST]) ;
		x265_enc_sbac_clear_bits((x265_enc_entropy_if_t*)enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_NEXT_BEST]) ;
		x265_enc_sbac_load (enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_CURR_BEST],
							enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_NEXT_BEST]) ;
	}

	sao_param = h->sao_param ;

	enc_sample_adaptive_offset->sample_adaptive_offset.i_sao_bit_increase_y = X265_MAX ( h->param.sps.i_bit_depth_y - 10, 0 ) ;
	enc_sample_adaptive_offset->sample_adaptive_offset.i_sao_bit_increase_c = X265_MAX ( h->param.sps.i_bit_depth_c - 10, 0 ) ;
	enc_sample_adaptive_offset->i_offset_th_y = 1 << X265_MIN ( h->param.sps.i_bit_depth_y - 5, 5 ) ;
	enc_sample_adaptive_offset->i_offset_th_c = 1 << X265_MIN ( h->param.sps.i_bit_depth_c - 5, 5 ) ;

	x265_sample_adaptive_offset_reset_sao_param((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
												sao_param) ;
	x265_enc_sample_adaptive_offset_reset_stats(enc_sample_adaptive_offset) ;

	sao_param->b_sao_flag[0] = 1 ;
	sao_param->b_sao_flag[1] = 1 ;
	sao_param->b_one_unit_flag[0] = 0 ;
	sao_param->b_one_unit_flag[1] = 0 ;
	sao_param->b_one_unit_flag[2] = 0 ;

#if X265_SAO_ENCODING_CHOICE
	i_depth = h->slice->i_depth ;
#if X265_SAO_ENCODING_CHOICE_CHROMA
	enc_sample_adaptive_offset->num_no_sao[0] = 0;// Luma
	enc_sample_adaptive_offset->num_no_sao[1] = 0;// Chroma
	if( i_depth > 0 && enc_sample_adaptive_offset->depth_sao_rate[0][i_depth-1] > X265_SAO_ENCODING_RATE )
	{
		sao_param->b_sao_flag[0] = 0;
	}
	if( i_depth > 0 && enc_sample_adaptive_offset->depth_sao_rate[1][i_depth-1] > X265_SAO_ENCODING_RATE_CHROMA )
	{
		sao_param->b_sao_flag[1] = 0;
	}
#else
	if( i_depth > 0 && enc_sample_adaptive_offset->f_depth_0_sao_rate > X265_SAO_ENCODING_RATE )
	{
		sao_param->b_sao_flag[0] = 0;
		sao_param->b_sao_flag[1] = 0;
	}
#endif
#endif

}


void x265_enc_sample_adaptive_offset_end_sao_enc ( x265_t *h, x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset )
{

}

int32_t x265_enc_sample_adaptive_offset_x_sign(int32_t x)
{
	return ((x >> 31) | ((int32_t)( (((uint32_t) -x)) >> 31)));
}



void x265_enc_sample_adaptive_offset_dump_rec_data (x265_t *h,
													x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
													int32_t i_addr,
													int32_t i_part_idx,
													int32_t i_y_cb_cr )
{
	static int32_t b_first = 1 ;
	int32_t i = 0, j = 0, k = 0 ;
	int32_t x,y;
	x265_data_cu_t *p_tmp_cu = NULL;
	pixel* p_rec;
	int32_t i_stride;
	int32_t i_lcu_height = 0;
	int32_t i_lcu_width  = 0;
	uint32_t i_l_pel_x = 0;
	uint32_t i_t_pel_y = 0;
	uint32_t i_r_pel_x;
	uint32_t i_b_pel_y;
	int32_t i_pic_width_tmp;
	int32_t i_pic_height_tmp;
	int32_t i_end_x;
	int32_t i_end_y;
	int32_t i_is_chroma = 0;
	int32_t num_skip_line = 0;
	int32_t num_skip_line_right = 0;
	FILE *file = NULL ;

	p_tmp_cu = h->data_cu;
	i_lcu_height = h->sps[0].i_max_cu_height;
	i_lcu_width  = h->sps[0].i_max_cu_width;
	i_l_pel_x = x265_data_cu_get_cu_pel_x(p_tmp_cu);
	i_t_pel_y = x265_data_cu_get_cu_pel_y(p_tmp_cu);
	i_is_chroma = (i_y_cb_cr!=0)? 1:0;
	num_skip_line = i_is_chroma? 2:4;

	if ( b_first )
	{
		file = fopen ( "recon2", "wb" ) ;
		b_first = 0 ;
	}
	else
	{
		file = fopen ( "recon2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}
	if (enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization == 0)
	{
		num_skip_line = 0;
	}

	num_skip_line_right = i_is_chroma? 3:5;
	if (enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization == 0)
	{
		num_skip_line_right = 0;
	}

	i_pic_width_tmp  = enc_sample_adaptive_offset->sample_adaptive_offset.i_pic_width  >> i_is_chroma;
	i_pic_height_tmp = enc_sample_adaptive_offset->sample_adaptive_offset.i_pic_height >> i_is_chroma;
	i_lcu_width     = i_lcu_width    >> i_is_chroma;
	i_lcu_height    = i_lcu_height   >> i_is_chroma;
	i_l_pel_x       = i_l_pel_x      >> i_is_chroma;
	i_t_pel_y       = i_t_pel_y      >> i_is_chroma;
	i_r_pel_x       = i_l_pel_x + i_lcu_width  ;
	i_b_pel_y       = i_t_pel_y + i_lcu_height ;
	i_r_pel_x       = i_r_pel_x > i_pic_width_tmp  ? i_pic_width_tmp  : i_r_pel_x;
	i_b_pel_y       = i_b_pel_y > i_pic_height_tmp ? i_pic_height_tmp : i_b_pel_y;
	i_lcu_width     = i_r_pel_x - i_l_pel_x;
	i_lcu_height    = i_b_pel_y - i_t_pel_y;

	i_stride    =  (i_y_cb_cr == 0) ? h->fdec->i_stride[0]
	                                : h->fdec->i_stride[1];

	//if(i_sao_type == bo_0 || i_sao_type == bo_1)
	{
		if( enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization
			&& enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_boundary )
	    {
			num_skip_line = i_is_chroma? 1:3;
			num_skip_line_right = i_is_chroma? 2:4;
	    }
		num_skip_line = 0;
		num_skip_line_right = 0;
	    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fdec,
	    													i_y_cb_cr,
	    													i_addr);

	    i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width : i_lcu_width-num_skip_line_right;
	    i_end_y   = (i_b_pel_y == i_pic_height_tmp) ? i_lcu_height : i_lcu_height-num_skip_line;
	    for (y=0; y<i_end_y; y++)
	    {
	    	for (x=0; x<i_end_x; x++)
	    	{
//	    		fprintf ( file, "%d ", p_rec[x] ) ;
	    	}
	    	p_rec += i_stride;
//	        fprintf ( file, "\n" ) ;
	    }

	}

	for ( i=0;i<enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts;i++)
	{
		for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
	    {
			for ( k=0;k<X265_MAX_NUM_SAO_CLASS;k++)
		    {
				fprintf ( file, "%d ", (uint32_t)enc_sample_adaptive_offset->count[i][j][k] ) ;
		    }
			fprintf ( file, "\n" ) ;
	    }
		fprintf ( file, "\n" ) ;
		fprintf ( file, "\n" ) ;
	}
	for ( i=0;i<enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts;i++)
	{
		for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
	    {
			for ( k=0;k<X265_MAX_NUM_SAO_CLASS;k++)
		    {
				fprintf ( file, "%d ", (uint32_t)enc_sample_adaptive_offset->offset_org[i][j][k] ) ;
		    }
			fprintf ( file, "\n" ) ;
	    }
		fprintf ( file, "\n" ) ;
		fprintf ( file, "\n" ) ;
	}
	for ( i=0;i<enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts;i++)
	{
		for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
	    {
			for ( k=0;k<X265_MAX_NUM_SAO_CLASS;k++)
		    {
				fprintf ( file, "%d ", (uint32_t)enc_sample_adaptive_offset->offset[i][j][k] ) ;
		    }
			fprintf ( file, "\n" ) ;
	    }
		fprintf ( file, "\n" ) ;
		fprintf ( file, "\n" ) ;
	}

	fclose ( file ) ;
}

/** calculate sao statistics for current lcu without non-crossing slice
 * \param  i_addr,  i_part_idx,  i_y_cb_cr
 */
void x265_enc_sample_adaptive_offset_calc_sao_stats_cu_org(x265_t *h,
															x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
															uint32_t i_cu_addr,
															int32_t i_part_idx,
															int32_t i_y_cb_cr)
{
	int32_t x,y;
	int32_t i_stride;
	int32_t i_lcu_height = 0;
	int32_t i_lcu_width  = 0;
	uint32_t i_l_pel_x = 0;
	uint32_t i_t_pel_y = 0;
	uint32_t i_r_pel_x;
	uint32_t i_b_pel_y;
	int32_t i_class_idx;
	int32_t i_pic_width_tmp;
	int32_t i_pic_height_tmp;
	int32_t i_start_x;
	int32_t i_start_y;
	int32_t i_end_x;
	int32_t i_end_y;
	int32_t i_is_chroma = 0;
	int32_t num_skip_line = 0;
	int32_t num_skip_line_right = 0;
	int32_t i_sign_left;
	int32_t i_sign_right;
	int32_t i_sign_down;
	int32_t i_sign_down1;
	int32_t i_sign_down2;
	uint32_t i_edge_type;
	int64_t* i_stats;
	int64_t* i_count;
	pixel* p_org;
	pixel* p_rec;
	pixel* p_table_bo = NULL;
	x265_data_cu_t *p_tmp_cu = NULL;


	p_tmp_cu = h->data_cu;
	i_lcu_height = h->sps[0].i_max_cu_height;
	i_lcu_width  = h->sps[0].i_max_cu_width;
	i_l_pel_x = x265_data_cu_get_cu_pel_x(p_tmp_cu);
	i_t_pel_y = x265_data_cu_get_cu_pel_y(p_tmp_cu);
	p_table_bo = (i_y_cb_cr==0) ? enc_sample_adaptive_offset->sample_adaptive_offset.luma_table_bo
								: enc_sample_adaptive_offset->sample_adaptive_offset.chroma_table_bo;
	i_is_chroma = (i_y_cb_cr!=0)? 1:0;
	num_skip_line = i_is_chroma? 2:4;



	if (enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization == 0)
	{
		num_skip_line = 0;
	}

	num_skip_line_right = i_is_chroma? 3:5;
	if (enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization == 0)
	{
		num_skip_line_right = 0;
	}

	i_pic_width_tmp  = enc_sample_adaptive_offset->sample_adaptive_offset.i_pic_width  >> i_is_chroma;
	i_pic_height_tmp = enc_sample_adaptive_offset->sample_adaptive_offset.i_pic_height >> i_is_chroma;
	i_lcu_width     = i_lcu_width    >> i_is_chroma;
	i_lcu_height    = i_lcu_height   >> i_is_chroma;
	i_l_pel_x       = i_l_pel_x      >> i_is_chroma;
	i_t_pel_y       = i_t_pel_y      >> i_is_chroma;
	i_r_pel_x       = i_l_pel_x + i_lcu_width  ;
	i_b_pel_y       = i_t_pel_y + i_lcu_height ;
	i_r_pel_x       = i_r_pel_x > i_pic_width_tmp  ? i_pic_width_tmp  : i_r_pel_x;
	i_b_pel_y       = i_b_pel_y > i_pic_height_tmp ? i_pic_height_tmp : i_b_pel_y;
	i_lcu_width     = i_r_pel_x - i_l_pel_x;
	i_lcu_height    = i_b_pel_y - i_t_pel_y;

	i_stride    =  (i_y_cb_cr == 0) ? h->fdec->i_stride[0]
	                                : h->fdec->i_stride[1] ;


//if(i_sao_type == bo_0 || i_sao_type == bo_1)
	{
		if( enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization
				&& enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_boundary )
		{
			num_skip_line = i_is_chroma? 1:3;
			num_skip_line_right = i_is_chroma? 2:4;
		}
		i_stats = enc_sample_adaptive_offset->offset_org[i_part_idx][SAO_BO];
		i_count = enc_sample_adaptive_offset->count    [i_part_idx][SAO_BO];

	    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fenc,
	    													i_y_cb_cr,
	    													i_cu_addr);
	    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fdec,
	    													i_y_cb_cr,
	    													i_cu_addr);

		i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width : i_lcu_width-num_skip_line_right;
		i_end_y   = (i_b_pel_y == i_pic_height_tmp) ? i_lcu_height : i_lcu_height-num_skip_line;
		for (y=0; y<i_end_y; y++)
		{
			for (x=0; x<i_end_x; x++)
			{
				i_class_idx = p_table_bo[p_rec[x]];
				if (i_class_idx)
				{
					i_stats[i_class_idx] += (p_org[x] - p_rec[x]);
					i_count[i_class_idx] ++;
				}
			}
			p_org += i_stride;
			p_rec += i_stride;
		}
	}


	//if (i_sao_type == eo_0  || i_sao_type == eo_1 || i_sao_type == eo_2 || i_sao_type == eo_3)
	{
		//if (i_sao_type == eo_0)
		{
			if( enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization
					&& enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_boundary )
			{
				num_skip_line = i_is_chroma? 1:3;
				num_skip_line_right = i_is_chroma? 3:5;
			}
			i_stats = enc_sample_adaptive_offset->offset_org[i_part_idx][SAO_EO_0];
			i_count = enc_sample_adaptive_offset->count    [i_part_idx][SAO_EO_0];

		    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
		    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
		    													h->fenc,
		    													i_y_cb_cr,
		    													i_cu_addr);
		    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
		    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
		    													h->fdec,
		    													i_y_cb_cr,
		    													i_cu_addr);

			i_start_x = (i_l_pel_x == 0) ? 1 : 0;
			i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width-1 : i_lcu_width-num_skip_line_right;
			for (y=0; y<i_lcu_height-num_skip_line; y++)
			{
				i_sign_left = x265_enc_sample_adaptive_offset_x_sign(p_rec[i_start_x] - p_rec[i_start_x-1]);
				for (x=i_start_x; x< i_end_x; x++)
				{
					i_sign_right =  x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+1]);
					i_edge_type =  i_sign_right + i_sign_left + 2;
					i_sign_left  = -i_sign_right;

					i_stats[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] += (p_org[x] - p_rec[x]);
					i_count[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] ++;
				}
				p_org += i_stride;
				p_rec += i_stride;
			}
		}

		//if (i_sao_type == eo_1)
		{
			if( enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization
					&& enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_boundary )
			{
				num_skip_line = i_is_chroma? 2:4;
				num_skip_line_right = i_is_chroma? 2:4;
			}
			i_stats = enc_sample_adaptive_offset->offset_org[i_part_idx][SAO_EO_1];
			i_count = enc_sample_adaptive_offset->count    [i_part_idx][SAO_EO_1];

		    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
		    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
		    													h->fenc,
		    													i_y_cb_cr,
		    													i_cu_addr);
		    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
		    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
		    													h->fdec,
		    													i_y_cb_cr,
		    													i_cu_addr);

			i_start_y = (i_t_pel_y == 0) ? 1 : 0;
			i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width : i_lcu_width-num_skip_line_right;
			i_end_y   = (i_b_pel_y == i_pic_height_tmp) ? i_lcu_height-1 : i_lcu_height-num_skip_line;
			if (i_t_pel_y == 0)
			{
				p_org += i_stride;
				p_rec += i_stride;
			}

			for (x=0; x< i_lcu_width; x++)
			{
				enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] = x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x-i_stride]);
			}
			for (y=i_start_y; y<i_end_y; y++)
			{
				for (x=0; x<i_end_x; x++)
				{
					i_sign_down     =  x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+i_stride]);
					i_edge_type    =  i_sign_down + enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] + 2;
					enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] = -i_sign_down;

					i_stats[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] += (p_org[x] - p_rec[x]);
					i_count[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] ++;
				}
				p_org += i_stride;
				p_rec += i_stride;
			}
		}
		//if (i_sao_type == eo_2)
		{
			if( enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization
					&& enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_boundary )
			{
				num_skip_line = i_is_chroma? 2:4;
				num_skip_line_right = i_is_chroma? 3:5;
			}
			i_stats = enc_sample_adaptive_offset->offset_org[i_part_idx][SAO_EO_2];
			i_count = enc_sample_adaptive_offset->count    [i_part_idx][SAO_EO_2];

		    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
		    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
		    													h->fenc,
		    													i_y_cb_cr,
		    													i_cu_addr);
		    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
		    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
		    													h->fdec,
		    													i_y_cb_cr,
		    													i_cu_addr);

			i_start_x = (i_l_pel_x == 0) ? 1 : 0;
			i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width-1 : i_lcu_width-num_skip_line_right;

			i_start_y = (i_t_pel_y == 0) ? 1 : 0;
			i_end_y   = (i_b_pel_y == i_pic_height_tmp) ? i_lcu_height-1 : i_lcu_height-num_skip_line;
			if (i_t_pel_y == 0)
			{
				p_org += i_stride;
				p_rec += i_stride;
			}

			for (x=i_start_x; x<i_end_x; x++)
			{
				enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] = x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x-i_stride-1]);
			}
			for (y=i_start_y; y<i_end_y; y++)
			{
				i_sign_down2 = x265_enc_sample_adaptive_offset_x_sign(p_rec[i_stride+i_start_x] - p_rec[i_start_x-1]);
				for (x=i_start_x; x<i_end_x; x++)
				{
					i_sign_down1      =  x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+i_stride+1]) ;
					i_edge_type      =  i_sign_down1 + enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] + 2;
					enc_sample_adaptive_offset->sample_adaptive_offset.up_buff[x+1] = -i_sign_down1;
					i_stats[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] += (p_org[x] - p_rec[x]);
					i_count[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] ++;
				}
				enc_sample_adaptive_offset->sample_adaptive_offset.up_buff[i_start_x] = i_sign_down2;
				enc_sample_adaptive_offset->sample_adaptive_offset.swap     = enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1;
				enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1 = enc_sample_adaptive_offset->sample_adaptive_offset.up_buff;
				enc_sample_adaptive_offset->sample_adaptive_offset.up_buff = enc_sample_adaptive_offset->sample_adaptive_offset.swap;

				p_rec += i_stride;
				p_org += i_stride;
			}
		}
		//if (i_sao_type == eo_3  )
		{
			if( enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization
					&& enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_boundary )
			{
				num_skip_line = i_is_chroma? 2:4;
				num_skip_line_right = i_is_chroma? 3:5;
			}
			i_stats = enc_sample_adaptive_offset->offset_org[i_part_idx][SAO_EO_3];
			i_count = enc_sample_adaptive_offset->count    [i_part_idx][SAO_EO_3];

		    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
		    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
		    													h->fenc,
		    													i_y_cb_cr,
		    													i_cu_addr);
		    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
		    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
		    													h->fdec,
		    													i_y_cb_cr,
		    													i_cu_addr);

			i_start_x = (i_l_pel_x == 0) ? 1 : 0;
			i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width-1 : i_lcu_width-num_skip_line_right;

			i_start_y = (i_t_pel_y == 0) ? 1 : 0;
			i_end_y   = (i_b_pel_y == i_pic_height_tmp) ? i_lcu_height-1 : i_lcu_height-num_skip_line;
			if (i_start_y == 1)
			{
				p_org += i_stride;
				p_rec += i_stride;
			}

			for (x=i_start_x-1; x<i_end_x; x++)
			{
				enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] = x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x-i_stride+1]);
			}

			for (y=i_start_y; y<i_end_y; y++)
			{
				for (x=i_start_x; x<i_end_x; x++)
				{
					i_sign_down1      =  x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+i_stride-1]) ;
					i_edge_type      =  i_sign_down1 + enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] + 2;
					enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x-1] = -i_sign_down1;
					i_stats[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] += (p_org[x] - p_rec[x]);
					i_count[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] ++;
				}
				enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[i_end_x-1] = x265_enc_sample_adaptive_offset_x_sign(p_rec[i_end_x-1 + i_stride] - p_rec[i_end_x]);

				p_rec += i_stride;
				p_org += i_stride;
			}
		}
	}
}

void x265_enc_sample_adaptive_offset_calc_sao_stats_cu_before_dblk(x265_t *h,
																	x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
																	uint32_t i_cu_addr )
{
	int32_t y_cb_cr;
	int32_t x,y;
	int32_t stride;
	int32_t lcu_height = 0;
	int32_t lcu_width = 0;
	uint32_t r_pel_x;
	uint32_t b_pel_y;
	int32_t class_idx;
	int32_t pic_width_tmp = 0;
	int32_t pic_height_tmp = 0;
	int32_t start_x;
	int32_t start_y;
	int32_t end_x;
	int32_t end_y;
	int32_t first_x, first_y;
	int32_t j, k;
	int32_t is_chroma;
	int32_t num_skip_line, num_skip_line_right;
	uint32_t l_pel_x, t_pel_y;
	int32_t sign_left;
	int32_t sign_right;
	int32_t sign_down;
	int32_t sign_down1;
	int32_t sign_down2;
	uint32_t i_edge_type;
	int64_t *stats;
	int64_t *count;
	pixel* p_org;
	pixel* p_rec;
	pixel* p_table_bo;
	x265_data_cu_t *p_tmp_cu;

	lcu_height = h->sps[0].i_max_cu_height;
	lcu_width  = h->sps[0].i_max_cu_width;
	p_tmp_cu = h->data_cu;
	l_pel_x = x265_data_cu_get_cu_pel_x(p_tmp_cu);
	t_pel_y = x265_data_cu_get_cu_pel_y(p_tmp_cu);

	for( y_cb_cr = 0; y_cb_cr < 3; y_cb_cr++ )
	{
		is_chroma = (y_cb_cr!=0)? 1:0;

		for ( j=0;j<MAX_NUM_SAO_TYPE;j++)
		{
			for ( k=0;k< X265_MAX_NUM_SAO_CLASS;k++)
			{
				enc_sample_adaptive_offset->count_pre_dblk    [y_cb_cr][j][k] = 0;
	            enc_sample_adaptive_offset->offset_org_pre_dblk[y_cb_cr][j][k] = 0;
			}
		}
		if( y_cb_cr == 0 )
		{
			pic_width_tmp  = enc_sample_adaptive_offset->sample_adaptive_offset.i_pic_width;
			pic_height_tmp = enc_sample_adaptive_offset->sample_adaptive_offset.i_pic_height;
		}
		else if( y_cb_cr == 1 )
		{
			pic_width_tmp  = enc_sample_adaptive_offset->sample_adaptive_offset.i_pic_width  >> is_chroma;
			pic_height_tmp = enc_sample_adaptive_offset->sample_adaptive_offset.i_pic_height >> is_chroma;
			lcu_width     = lcu_width    >> is_chroma;
			lcu_height    = lcu_height   >> is_chroma;
			l_pel_x       = l_pel_x      >> is_chroma;
			t_pel_y       = t_pel_y      >> is_chroma;
		}
		r_pel_x       = l_pel_x + lcu_width  ;
		b_pel_y       = t_pel_y + lcu_height ;
		r_pel_x       = r_pel_x > pic_width_tmp  ? pic_width_tmp  : r_pel_x;
		b_pel_y       = b_pel_y > pic_height_tmp ? pic_height_tmp : b_pel_y;
		lcu_width     = r_pel_x - l_pel_x;
		lcu_height    = b_pel_y - t_pel_y;

		stride    =  (y_cb_cr == 0) ? h->fdec->i_stride[0]
		                            : h->fdec->i_stride[1] ;
		p_table_bo = (y_cb_cr==0) ? enc_sample_adaptive_offset->sample_adaptive_offset.luma_table_bo
									: enc_sample_adaptive_offset->sample_adaptive_offset.chroma_table_bo;

		//if(i_sao_type == bo)

		num_skip_line = is_chroma? 1:3;
		num_skip_line_right = is_chroma? 2:4;

		stats = enc_sample_adaptive_offset->offset_org_pre_dblk[y_cb_cr][SAO_BO];
		count = enc_sample_adaptive_offset->count_pre_dblk[y_cb_cr][SAO_BO];

	    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fenc,
	    													y_cb_cr,
	    													i_cu_addr);
	    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fdec,
	    													y_cb_cr,
	    													i_cu_addr);

		start_x   = (r_pel_x == pic_width_tmp) ? lcu_width : lcu_width-num_skip_line_right;
		start_y   = (b_pel_y == pic_height_tmp) ? lcu_height : lcu_height-num_skip_line;

		for (y=0; y<lcu_height; y++)
		{
			for (x=0; x<lcu_width; x++)
			{
				if( x < start_x && y < start_y )
				{
					continue;
				}

	            class_idx = p_table_bo[p_rec[x]];
	            if (class_idx)
	            {
	            	stats[class_idx] += (p_org[x] - p_rec[x]);
	            	count[class_idx] ++;
	            }
			}
			p_org += stride;
			p_rec += stride;
		}

		//if (i_sao_type == eo_0)

		num_skip_line = is_chroma? 1:3;
		num_skip_line_right = is_chroma? 3:5;

		stats = enc_sample_adaptive_offset->offset_org_pre_dblk[y_cb_cr][SAO_EO_0];
		count = enc_sample_adaptive_offset->count_pre_dblk[y_cb_cr][SAO_EO_0];

	    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fenc,
	    													y_cb_cr,
	    													i_cu_addr);
	    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fdec,
	    													y_cb_cr,
	    													i_cu_addr);

		start_x   = (r_pel_x == pic_width_tmp) ? lcu_width-1 : lcu_width-num_skip_line_right;
		start_y   = (b_pel_y == pic_height_tmp) ? lcu_height : lcu_height-num_skip_line;
		first_x   = (l_pel_x == 0) ? 1 : 0;
		end_x   = (r_pel_x == pic_width_tmp) ? lcu_width-1 : lcu_width-1;

		for (y=0; y<lcu_height; y++)
		{
			sign_left = x265_enc_sample_adaptive_offset_x_sign(p_rec[first_x] - p_rec[first_x-1]);
			for (x=first_x; x< end_x; x++)
			{
				sign_right =  x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+1]);
	            i_edge_type =  sign_right + sign_left + 2;
	            sign_left  = -sign_right;

	            if( x < start_x && y < start_y )
	            {
	            	continue;
	            }

	            stats[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] += (p_org[x] - p_rec[x]);
	            count[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] ++;
			}
			p_org += stride;
			p_rec += stride;
		}

		//if (i_sao_type == eo_1)

		num_skip_line = is_chroma? 2:4;
		num_skip_line_right = is_chroma? 2:4;

		stats = enc_sample_adaptive_offset->offset_org_pre_dblk[y_cb_cr][SAO_EO_1];
		count = enc_sample_adaptive_offset->count_pre_dblk[y_cb_cr][SAO_EO_1];

	    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fenc,
	    													y_cb_cr,
	    													i_cu_addr);
	    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fdec,
	    													y_cb_cr,
	    													i_cu_addr);

		start_x   = (r_pel_x == pic_width_tmp) ? lcu_width : lcu_width-num_skip_line_right;
		start_y   = (b_pel_y == pic_height_tmp) ? lcu_height-1 : lcu_height-num_skip_line;
		first_y = (t_pel_y == 0) ? 1 : 0;
		end_y   = (b_pel_y == pic_height_tmp) ? lcu_height-1 : lcu_height-1;
		if (first_y == 1)
		{
			p_org += stride;
			p_rec += stride;
		}

		for (x=0; x< lcu_width; x++)
		{
			enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] = x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x-stride]);
		}
		for (y=first_y; y<end_y; y++)
		{
			for (x=0; x<lcu_width; x++)
			{
				sign_down     =  x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+stride]);
	            i_edge_type    =  sign_down + enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] + 2;
	            enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] = -sign_down;

	            if( x < start_x && y < start_y )
	            {
	            	continue;
	            }

	            stats[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] += (p_org[x] - p_rec[x]);
	            count[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] ++;
			}
			p_org += stride;
			p_rec += stride;
		}

		//if (i_sao_type == eo_2)

		num_skip_line = is_chroma? 2:4;
		num_skip_line_right = is_chroma? 3:5;

		stats = enc_sample_adaptive_offset->offset_org_pre_dblk[y_cb_cr][SAO_EO_2];
		count = enc_sample_adaptive_offset->count_pre_dblk[y_cb_cr][SAO_EO_2];

	    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fenc,
	    													y_cb_cr,
	    													i_cu_addr);
	    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fdec,
	    													y_cb_cr,
	    													i_cu_addr);

		start_x   = (r_pel_x == pic_width_tmp) ? lcu_width-1 : lcu_width-num_skip_line_right;
		start_y   = (b_pel_y == pic_height_tmp) ? lcu_height-1 : lcu_height-num_skip_line;
		first_x   = (l_pel_x == 0) ? 1 : 0;
		first_y = (t_pel_y == 0) ? 1 : 0;
		end_x   = (r_pel_x == pic_width_tmp) ? lcu_width-1 : lcu_width-1;
		end_y   = (b_pel_y == pic_height_tmp) ? lcu_height-1 : lcu_height-1;
		if (first_y == 1)
		{
			p_org += stride;
			p_rec += stride;
		}

		for (x=first_x; x<end_x; x++)
		{
			enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] = x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x-stride-1]);
		}
		for (y=first_y; y<end_y; y++)
		{
			sign_down2 = x265_enc_sample_adaptive_offset_x_sign(p_rec[stride+start_x] - p_rec[start_x-1]);
			for (x=first_x; x<end_x; x++)
			{
				sign_down1      =  x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+stride+1]) ;
	            i_edge_type      =  sign_down1 + enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] + 2;
	            enc_sample_adaptive_offset->sample_adaptive_offset.up_buff[x+1] = -sign_down1;

	            if( x < start_x && y < start_y )
	            {
	            	continue;
	            }

	            stats[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] += (p_org[x] - p_rec[x]);
	            count[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] ++;
			}
			enc_sample_adaptive_offset->sample_adaptive_offset.up_buff[first_x] = sign_down2;
			enc_sample_adaptive_offset->sample_adaptive_offset.swap     = enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1;
			enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1 = enc_sample_adaptive_offset->sample_adaptive_offset.up_buff;
			enc_sample_adaptive_offset->sample_adaptive_offset.up_buff = enc_sample_adaptive_offset->sample_adaptive_offset.swap;

			p_rec += stride;
			p_org += stride;
		}

		//if (i_sao_type == eo_3)

		num_skip_line = is_chroma? 2:4;
		num_skip_line_right = is_chroma? 3:5;

		stats = enc_sample_adaptive_offset->offset_org_pre_dblk[y_cb_cr][SAO_EO_3];
		count = enc_sample_adaptive_offset->count_pre_dblk[y_cb_cr][SAO_EO_3];

	    p_org = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fenc,
	    													y_cb_cr,
	    													i_cu_addr);
	    p_rec = x265_sample_adaptive_offset_get_frame_addr(h,
	    													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
	    													h->fdec,
	    													y_cb_cr,
	    													i_cu_addr);

		start_x   = (r_pel_x == pic_width_tmp) ? lcu_width-1 : lcu_width-num_skip_line_right;
		start_y   = (b_pel_y == pic_height_tmp) ? lcu_height-1 : lcu_height-num_skip_line;
		first_x   = (l_pel_x == 0) ? 1 : 0;
		first_y = (t_pel_y == 0) ? 1 : 0;
		end_x   = (r_pel_x == pic_width_tmp) ? lcu_width-1 : lcu_width-1;
		end_y   = (b_pel_y == pic_height_tmp) ? lcu_height-1 : lcu_height-1;
		if (first_y == 1)
		{
			p_org += stride;
			p_rec += stride;
		}

		for (x=first_x-1; x<end_x; x++)
		{
			enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] = x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x-stride+1]);
		}

		for (y=first_y; y<end_y; y++)
		{
			for (x=first_x; x<end_x; x++)
			{
	            sign_down1      =  x265_enc_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+stride-1]) ;
	            i_edge_type      =  sign_down1 + enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x] + 2;
	            enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[x-1] = -sign_down1;

	            if( x < start_x && y < start_y )
	            {
	            	continue;
	            }

	            stats[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] += (p_org[x] - p_rec[x]);
	            count[p_sample_adaptive_offset_class_static->eo_table[i_edge_type]] ++;
			}
			enc_sample_adaptive_offset->sample_adaptive_offset.up_buff_1[end_x-1] = x265_enc_sample_adaptive_offset_x_sign(p_rec[end_x-1 + stride] - p_rec[end_x]);

			p_rec += stride;
			p_org += stride;
		}
	}

//	dump_rec_data( i_cu_addr, 0, 0 ) ;
}

void x265_enc_sample_adaptive_offset_reset_stats(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset)
{
	int32_t i = 0, j = 0, k = 0 ;

	for ( i = 0 ; i < enc_sample_adaptive_offset->sample_adaptive_offset.i_num_total_parts; ++i)
	{
		enc_sample_adaptive_offset->cost_part_best[i] = X265_MAX_DOUBLE;
		enc_sample_adaptive_offset->type_part_best[i] = -1;
		enc_sample_adaptive_offset->dist_org[i] = 0;
		for ( j = 0 ; j < MAX_NUM_SAO_TYPE ; ++ j)
		{
			enc_sample_adaptive_offset->dist[i][j] = 0;
			enc_sample_adaptive_offset->rate[i][j] = 0;
			enc_sample_adaptive_offset->cost[i][j] = 0.0;
			for ( k = 0 ; k < X265_MAX_NUM_SAO_CLASS ; ++k )
			{
				enc_sample_adaptive_offset->count [i][j][k] = 0;
				enc_sample_adaptive_offset->offset[i][j][k] = 0;
				enc_sample_adaptive_offset->offset_org[i][j][k] = 0;
			}
		}
	}
}


void x265_enc_sample_adaptive_offset_sao_process_cu ( x265_t *h,
													x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
													x265_sao_param_t *sao_param,
													uint32_t i_cu_addr,
													double f_lambda,
													double f_lambda_chroma,
													int32_t i_depth )
{
	uint32_t i_new_cu_addr = 0 ;

	x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
										(x265_enc_entropy_if_t*)(&h->rd_go_on_sbac_coder),
										h->slice );

	if ( enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization)
	{
		x265_enc_sample_adaptive_offset_rdo_sao_unit_cu(h,
														enc_sample_adaptive_offset,
														sao_param,
														i_cu_addr,
														f_lambda,
														f_lambda_chroma, i_depth);
	}
	if ( ( i_cu_addr % h->cu.pic.i_width_in_cu > 0 )
			&& ( i_cu_addr / h->cu.pic.i_width_in_cu > 0 ) )
	{
		i_new_cu_addr = i_cu_addr - h->cu.pic.i_width_in_cu - 1 ;
		if (sao_param->b_sao_flag[0])
		{
			x265_sample_adaptive_offset_process_cu(h,
													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
													sao_param->sao_lcu_param[0],
													i_new_cu_addr,
													sao_param->b_one_unit_flag[0],
													0);
		}
		if (sao_param->b_sao_flag[1])
		{
			x265_sample_adaptive_offset_process_cu(h,
													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
													sao_param->sao_lcu_param[1],
													i_new_cu_addr,
													sao_param->b_one_unit_flag[1],
													1);
			x265_sample_adaptive_offset_process_cu(h,
													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
													sao_param->sao_lcu_param[2],
													i_new_cu_addr,
													sao_param->b_one_unit_flag[2],
													2);
		}
		if ( ( i_cu_addr % h->cu.pic.i_width_in_cu ==
				h->cu.pic.i_width_in_cu - 1 ) )
		{
			i_new_cu_addr = i_cu_addr - h->cu.pic.i_width_in_cu ;
			if (sao_param->b_sao_flag[0])
			{
				x265_sample_adaptive_offset_process_cu(h,
														(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
														sao_param->sao_lcu_param[0],
														i_new_cu_addr,
														sao_param->b_one_unit_flag[0],
														0);
			}
			if (sao_param->b_sao_flag[1])
			{
				x265_sample_adaptive_offset_process_cu(h,
														(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
														sao_param->sao_lcu_param[1],
														i_new_cu_addr,
														sao_param->b_one_unit_flag[1],
														1);
				x265_sample_adaptive_offset_process_cu(h,
														(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
														sao_param->sao_lcu_param[2],
														i_new_cu_addr,
														sao_param->b_one_unit_flag[2],
														2);
			}
		}
	}
	if ( i_cu_addr == h->cu.pic.i_num_cus_in_frame - 1 )
	{
		x265_enc_sample_adaptive_offset_sao_process_last_row_cu(h,
																enc_sample_adaptive_offset,
																sao_param,
																f_lambda,
																f_lambda_chroma,
																i_depth ) ;
	}
}

void x265_enc_sample_adaptive_offset_sao_process_last_row_cu(x265_t *h,
															x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
															x265_sao_param_t *sao_param,
															double f_lambda,
															double f_lambda_chroma,
															int32_t depth )
{
	uint32_t i_cu_addr = 0 ;
	uint32_t i_frame_width_in_cu = 0 ;
	uint32_t i_frame_width_in_cu_mul_two = 0 ;
	uint32_t i_frame_height_in_cu = 0 ;

	i_frame_width_in_cu = h->cu.pic.i_width_in_cu ;
	i_frame_width_in_cu_mul_two = h->cu.pic.i_width_in_cu_mul_two ;
	i_frame_height_in_cu = h->cu.pic.i_height_in_cu ;

	for ( i_cu_addr = h->cu.pic.i_num_cus_in_frame - i_frame_width_in_cu ;
			i_cu_addr < h->cu.pic.i_num_cus_in_frame ;
			++ i_cu_addr )
	{
		if (sao_param->b_sao_flag[0])
		{
			x265_sample_adaptive_offset_process_cu(h,
													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
													sao_param->sao_lcu_param[0],
													i_cu_addr,
													sao_param->b_one_unit_flag[0], 0);
		}
		if (sao_param->b_sao_flag[1])
		{
			x265_sample_adaptive_offset_process_cu(h,
													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
													sao_param->sao_lcu_param[1],
													i_cu_addr,
													sao_param->b_one_unit_flag[1],
													1);
			x265_sample_adaptive_offset_process_cu(h,
													(x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
													sao_param->sao_lcu_param[2],
													i_cu_addr,
													sao_param->b_one_unit_flag[2],
													2);
		}
	}


#if X265_SAO_ENCODING_CHOICE
#if X265_SAO_ENCODING_CHOICE_CHROMA
	if( !sao_param->b_sao_flag[0])
	{
		enc_sample_adaptive_offset->depth_sao_rate[0][depth] = 1.0;
	}
	else
	{
		enc_sample_adaptive_offset->depth_sao_rate[0][depth]
		   = enc_sample_adaptive_offset->num_no_sao[0]
		        / ((double) i_frame_height_in_cu*i_frame_width_in_cu);
	}
	if( !sao_param->b_sao_flag[1])
	{
		enc_sample_adaptive_offset->depth_sao_rate[1][depth] = 1.0;
	}
	else
	{
		enc_sample_adaptive_offset->depth_sao_rate[1][depth]
		   = enc_sample_adaptive_offset->num_no_sao[1]
		        /((double) i_frame_height_in_cu*i_frame_width_in_cu_mul_two);
	}
#else
	if( depth == 0)
	{
		// update SAO Rate
		enc_sample_adaptive_offset->f_depth_0_sao_rate
			= enc_sample_adaptive_offset->i_num_no_sao
				/((double) i_frame_height_in_cu*i_frame_width_in_cu*3);
	}
#endif
#endif

}


/** check merge sao unit
 * \param sao_unit_curr current sao unit
 * \param sao_unit_check sao unit tobe check
 * \param dir direction
 */
void x265_enc_sample_adaptive_offset_check_merge(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_unit_curr,
												x265_sao_lcu_param_t *sao_unit_check,
												int32_t dir)
{
	int32_t loop ;
	int32_t count_diff = 0;

	if (sao_unit_curr->i_part_idx != sao_unit_check->i_part_idx)
	{
		if (sao_unit_curr->i_type_idx !=-1)
		{
			if (sao_unit_curr->i_type_idx == sao_unit_check->i_type_idx)
			{
				for ( loop = 0 ; loop < sao_unit_curr->i_length ; ++loop )
				{
					count_diff += (sao_unit_curr->offset[loop] != sao_unit_check->offset[loop]);
				}
				count_diff += (sao_unit_curr->i_sub_type_idx != sao_unit_check->i_sub_type_idx);
				if (count_diff ==0)
				{
					sao_unit_curr->i_part_idx = sao_unit_check->i_part_idx;
					if (dir == 1)
					{
						sao_unit_curr->b_merge_up_flag = 1;
						sao_unit_curr->b_merge_left_flag = 0;
					}
					else
					{
						sao_unit_curr->b_merge_up_flag = 0;
						sao_unit_curr->b_merge_left_flag = 1;
					}
				}
			}
		}
		else
		{
			if (sao_unit_curr->i_type_idx == sao_unit_check->i_type_idx)
			{
				sao_unit_curr->i_part_idx = sao_unit_check->i_part_idx;
				if (dir == 1)
				{
					sao_unit_curr->b_merge_up_flag = 1;
					sao_unit_curr->b_merge_left_flag = 0;
				}
				else
				{
					sao_unit_curr->b_merge_up_flag = 0;
					sao_unit_curr->b_merge_left_flag = 1;
				}
			}
		}
	}
}

void x265_enc_sample_adaptive_offset_rdo_sao_unit_cu(x265_t *h,
													x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
													x265_sao_param_t *sao_param,
													uint32_t i_cu_addr,
													double lambda,
													double lambda_chroma,
													int32_t depth)
{
	int32_t j, k;
	int32_t idx_y;
	int32_t idx_x;
	int32_t frame_width_in_cu = 0;
	uint32_t i_cu_addr_up = 0;
	uint32_t i_cu_addr_left = 0;
	int32_t comp_idx = 0;
	int32_t allow_merge_left = 0;
	int32_t allow_merge_up = 0;
	uint32_t rate;
	int32_t merge_up = 0 ;
	double best_cost, merge_cost;
	double comp_distortion[3];
	x265_sao_lcu_param_t merge_sao_param[3][2];

	frame_width_in_cu = sao_param->i_num_cu_in_width;
	i_cu_addr_up = -1;
	i_cu_addr_left = -1;

	idx_y = i_cu_addr / frame_width_in_cu ;
	idx_x = i_cu_addr % frame_width_in_cu ;

	i_cu_addr_up = i_cu_addr < frame_width_in_cu ? -1 : idx_x + frame_width_in_cu * (idx_y - 1);
	i_cu_addr_left = idx_x == 0 ? -1 : idx_x - 1 + frame_width_in_cu * idx_y ;
	allow_merge_left = 1;
	allow_merge_up   = 1;

	if (idx_x!=0)
	{
		// check tile id and slice id
		if (h->tile_idx_map[i_cu_addr-1] != h->tile_idx_map[i_cu_addr])
		{
			allow_merge_left = 0;
		}
	}
	else
	{
		allow_merge_left = 0;
	}
	if (idx_y!=0)
	{
		if (h->tile_idx_map[i_cu_addr-enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width]
		                    != h->tile_idx_map[i_cu_addr])
		{
			allow_merge_up = 0;
		}
	}
	else
	{
		allow_merge_up = 0;
	}

	comp_distortion[0] = 0;
	comp_distortion[1] = 0;
	comp_distortion[2] = 0;
	x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
						enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_CURR_BEST]);
	if (allow_merge_left)
	{
		h->enc_entropy.enc_entropy_if->code_sao_merge(h->enc_entropy.enc_entropy_if, 0);
	}
	if (allow_merge_up)
	{
		h->enc_entropy.enc_entropy_if->code_sao_merge(h->enc_entropy.enc_entropy_if, 0);
	}
	x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
						enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
	// reset stats y, cb, cr
	for ( comp_idx=0;comp_idx<3;comp_idx++)
	{
		for ( j = 0 ; j < MAX_NUM_SAO_TYPE ; j++)
		{
			for ( k = 0 ; k < X265_MAX_NUM_SAO_CLASS ; k++ )
			{
				enc_sample_adaptive_offset->offset   [comp_idx][j][k] = 0;
				if( enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_based_optimization
						&& enc_sample_adaptive_offset->sample_adaptive_offset.b_sao_lcu_boundary )
				{
					enc_sample_adaptive_offset->count    [comp_idx][j][k] = enc_sample_adaptive_offset->count_pre_dblk    [comp_idx][j][k];
					enc_sample_adaptive_offset->offset_org[comp_idx][j][k] = enc_sample_adaptive_offset->offset_org_pre_dblk[comp_idx][j][k];
				}
				else
				{
					enc_sample_adaptive_offset->count    [comp_idx][j][k] = 0;
					enc_sample_adaptive_offset->offset_org[comp_idx][j][k] = 0;
				}
			}
		}
		sao_param->sao_lcu_param[comp_idx][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two].i_type_idx       =  -1;
		sao_param->sao_lcu_param[comp_idx][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two].b_merge_up_flag   = 0;
		sao_param->sao_lcu_param[comp_idx][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two].b_merge_left_flag = 0;
		sao_param->sao_lcu_param[comp_idx][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two].i_sub_type_idx    = 0;
#if X265_SAO_ENCODING_CHOICE
		if( (comp_idx ==0 && sao_param->b_sao_flag[0])|| (comp_idx >0 && sao_param->b_sao_flag[1]) )
#endif
		{
			x265_enc_sample_adaptive_offset_calc_sao_stats_cu_org(h,
																enc_sample_adaptive_offset,
																i_cu_addr,
																comp_idx,
																comp_idx);

		}
	}
	x265_enc_sample_adaptive_offset_sao_component_param_dist(h,
															enc_sample_adaptive_offset,
															allow_merge_left,
															allow_merge_up,
															sao_param,
															i_cu_addr,
															i_cu_addr_up,
															i_cu_addr_left,
															0,
															lambda,
															&merge_sao_param[0][0],
															&comp_distortion[0]);
	x265_enc_sample_adaptive_offset_sao_2_chroma_param_dist(h,
															enc_sample_adaptive_offset,
															allow_merge_left,
															allow_merge_up,
															sao_param,
															i_cu_addr,
															i_cu_addr_up,
															i_cu_addr_left,
															lambda_chroma,
															&merge_sao_param[1][0],
															&merge_sao_param[2][0],
															&comp_distortion[0]);
	if( sao_param->b_sao_flag[0] || sao_param->b_sao_flag[1] )
	{
		// cost of new sao_params
		x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
							enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_CURR_BEST]);
		x265_enc_sbac_reset_bits((x265_enc_entropy_if_t*)(&h->rd_go_on_sbac_coder));
		if (allow_merge_left)
		{
			h->enc_entropy.enc_entropy_if->code_sao_merge(h->enc_entropy.enc_entropy_if, 0);
		}
		if (allow_merge_up)
		{
			h->enc_entropy.enc_entropy_if->code_sao_merge(h->enc_entropy.enc_entropy_if, 0);
		}
		for ( comp_idx=0;comp_idx<3;comp_idx++)
		{
			if( (comp_idx ==0 && sao_param->b_sao_flag[0]) || (comp_idx >0 && sao_param->b_sao_flag[1]))
			{
				x265_enc_entropy_encode_sao_offset(h,
													&h->enc_entropy,
													&sao_param->sao_lcu_param[comp_idx][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two],
													comp_idx);
			}
		}

		rate = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
		best_cost = comp_distortion[0] + (double)rate;
		x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
							enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);

		// cost of merge
		for( merge_up=0; merge_up<2; ++merge_up)
		{
			if ( (allow_merge_left && (merge_up==0)) || (allow_merge_up && (merge_up==1)) )
			{
				x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
									enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_CURR_BEST]);
				x265_enc_sbac_reset_bits((x265_enc_entropy_if_t*)(&h->rd_go_on_sbac_coder));
				if (allow_merge_left)
				{
					h->enc_entropy.enc_entropy_if->code_sao_merge(h->enc_entropy.enc_entropy_if, 1 - merge_up);
				}
				if ( allow_merge_up && (merge_up==1) )
				{
					h->enc_entropy.enc_entropy_if->code_sao_merge(h->enc_entropy.enc_entropy_if, 1);
				}

				rate = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
				merge_cost = comp_distortion[merge_up+1] + (double)rate;
				if (merge_cost < best_cost)
				{
					best_cost = merge_cost;
					x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
										enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
					for ( comp_idx=0;comp_idx<3;comp_idx++)
					{
						merge_sao_param[comp_idx][merge_up].b_merge_left_flag = 1-merge_up;
						merge_sao_param[comp_idx][merge_up].b_merge_up_flag = merge_up;
						if( (comp_idx==0 && sao_param->b_sao_flag[0]) || (comp_idx>0 && sao_param->b_sao_flag[1]))
						{
							x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
																		&sao_param->sao_lcu_param[comp_idx][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two],
																		&merge_sao_param[comp_idx][merge_up] );
						}
					}
				}
			}
		}

#if X265_SAO_ENCODING_CHOICE
#if X265_SAO_ENCODING_CHOICE_CHROMA
		if( sao_param->sao_lcu_param[0][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two].i_type_idx == -1)
		{
			enc_sample_adaptive_offset->num_no_sao[0]++;
		}
		if( sao_param->sao_lcu_param[1][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two].i_type_idx == -1)
		{
			enc_sample_adaptive_offset->num_no_sao[1]+=2;
		}
#else
		for ( comp_idx=0;comp_idx<3;comp_idx++)
        {
			if( depth == 0 && sao_param->sao_lcu_param[comp_idx][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two].i_type_idx == -1)
			{
				enc_sample_adaptive_offset->i_num_no_sao++;
			}
        }
#endif
#endif
		x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
							enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
		x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
							enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_CURR_BEST]);
	}
}

/** rate distortion optimization of sao unit
 * \param sao_param sao parameters
 * \param i_cu_addr address
 * \param i_cu_addr_up above address
 * \param i_cu_addr_left left address
 * \param y_cb_cr color component index
 * \param lambda
 */
int64_t x265_enc_sample_adaptive_offset_est_sao_type_dist(x265_t *h,
														x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
														int32_t comp_idx,
														int32_t type_idx,
														int32_t shift,
														double lambda,
														int32_t *current_distortion_table_bo,
														double *current_rd_cost_table_bo)
{
	int64_t est_dist = 0;
	int32_t class_idx;
	int32_t bit_depth = 0;
	int32_t sao_bit_increase = 0;
	int32_t sao_offset_th = 0;

	bit_depth = (comp_idx==0) ? h->param.sps.i_bit_depth_y
								: h->param.sps.i_bit_depth_c;
	sao_bit_increase = (comp_idx==0) ? enc_sample_adaptive_offset->sample_adaptive_offset.i_sao_bit_increase_y
										: enc_sample_adaptive_offset->sample_adaptive_offset.i_sao_bit_increase_c;
	sao_offset_th = (comp_idx==0) ? enc_sample_adaptive_offset->i_offset_th_y : enc_sample_adaptive_offset->i_offset_th_c;

	for( class_idx = 1;
			class_idx < ((type_idx < SAO_BO) ? p_sample_adaptive_offset_class_static->num_class[type_idx] + 1
												: SAO_MAX_BO_CLASSES+1) ;
			class_idx++)
	{
		if( type_idx == SAO_BO)
		{
			current_distortion_table_bo[class_idx-1] = 0;
			current_rd_cost_table_bo[class_idx-1] = lambda;
		}
		if(enc_sample_adaptive_offset->count [comp_idx][type_idx][class_idx])
		{
			enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx]
			   = (int64_t) x265_enc_sample_adaptive_offset_x_round_ibdi(bit_depth, (double)(enc_sample_adaptive_offset->offset_org[comp_idx][type_idx][class_idx]<<(bit_depth-8)) / (double)(enc_sample_adaptive_offset->count [comp_idx][type_idx][class_idx]<<sao_bit_increase));
			enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx] = x265_clip3_int64(enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx],
																									- sao_offset_th + 1,
																									sao_offset_th - 1 );
			if (type_idx < 4)
			{
				if ( enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx]<0 && class_idx<3 )
				{
					enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx] = 0;
				}
				if ( enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx]>0 && class_idx>=3)
				{
					enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx] = 0;
				}
			}
			enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx] =
					x265_enc_sample_adaptive_offset_est_iter_offset(enc_sample_adaptive_offset,
																	type_idx,
																	class_idx,
																	lambda,
																	enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx],
																	enc_sample_adaptive_offset->count [comp_idx][type_idx][class_idx],
																	enc_sample_adaptive_offset->offset_org[comp_idx][type_idx][class_idx],
																	shift,
																	sao_bit_increase,
																	current_distortion_table_bo,
																	current_rd_cost_table_bo,
																	sao_offset_th );
		}
		else
		{
			enc_sample_adaptive_offset->offset_org[comp_idx][type_idx][class_idx] = 0;
			enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx] = 0;
		}
		if( type_idx != SAO_BO )
		{
			est_dist +=
					x265_enc_sample_adaptive_offset_est_sao_dist(enc_sample_adaptive_offset,
																	enc_sample_adaptive_offset->count [comp_idx][type_idx][class_idx],
																	enc_sample_adaptive_offset->offset[comp_idx][type_idx][class_idx] << sao_bit_increase,
																	enc_sample_adaptive_offset->offset_org[comp_idx][type_idx][class_idx],
																	shift);
		}
	}
	return est_dist;
}

int64_t x265_enc_sample_adaptive_offset_est_sao_dist(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
													int64_t count,
													int64_t offset,
													int64_t offset_org,
													int64_t shift)
{
	return ((count * offset * offset - offset_org * offset * 2) >> shift);
}

int64_t x265_enc_sample_adaptive_offset_est_iter_offset(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
														int32_t type_idx,
														int32_t class_idx,
														double lambda,
														int64_t offset_input,
														int64_t count,
														int64_t offset_org,
														int32_t shift,
														int32_t bit_increase,
														int32_t *current_distortion_table_bo,
														double *current_rd_cost_table_bo,
														int32_t offset_th )
{
	//clean up, best_q_offset.
	int64_t iter_offset, temp_offset;
	int64_t temp_dist, temp_rate;
	double temp_cost, temp_min_cost;
	int64_t offset_output = 0;

	iter_offset = offset_input;
	// assuming sending quantized value 0 results in zero offset and sending the value zero needs 1 bit. entropy coder can be used to measure the exact rate here.
	temp_min_cost = lambda;
	while (iter_offset != 0)
	{
		// calculate the bits required for signalling the offset
		temp_rate = (type_idx == SAO_BO) ? (abs((int32_t)iter_offset)+2) : (abs((int32_t)iter_offset)+1);
		if (abs((int32_t)iter_offset)==offset_th-1)
		{
			temp_rate --;
		}
		// do the dequntization before distorion calculation
		temp_offset  = iter_offset << bit_increase;
		temp_dist    = x265_enc_sample_adaptive_offset_est_sao_dist(enc_sample_adaptive_offset,
																	count,
																	temp_offset,
																	offset_org,
																	shift);
		temp_cost    = ((double)temp_dist + lambda * (double) temp_rate);
		if(temp_cost < temp_min_cost)
		{
			temp_min_cost = temp_cost;
			offset_output = iter_offset;
			if(type_idx == SAO_BO)
			{
				current_distortion_table_bo[class_idx-1] = (int32_t) temp_dist;
				current_rd_cost_table_bo[class_idx-1] = temp_cost;
			}
		}
		iter_offset = (iter_offset > 0) ? (iter_offset-1):(iter_offset+1);
	}
	return offset_output;
}


void x265_enc_sample_adaptive_offset_sao_component_param_dist(x265_t *h,
																x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
																int32_t allow_merge_left,
																int32_t allow_merge_up,
																x265_sao_param_t *sao_param,
																uint32_t i_cu_addr,
																uint32_t i_cu_addr_up,
																uint32_t i_cu_addr_left,
																int32_t y_cb_cr,
																double lambda,
																x265_sao_lcu_param_t *comp_sao_param,
																double *comp_distortion)
{
	int32_t i = 0 ;
	uint32_t j = 0;
	int32_t type_idx;
	int64_t est_dist;
	int32_t class_idx;
	int32_t shift = 0;
	int64_t best_dist;
	int32_t best_class_table_bo    = 0;
	int32_t idx_neighbor = 0;
	int32_t merge_band_position = 0;
	int32_t i_merge_offset;
	int32_t current_distortion_table_bo[X265_MAX_NUM_SAO_CLASS];
	double current_rd_cost_table_bo[X265_MAX_NUM_SAO_CLASS];
	double f_cost_part_best = 0.0 ;
	double best_rd_cost_table_bo = 0.0;
	double est_rate = 0;
	double current_rd_cost = 0.0;
	x265_sao_lcu_param_t sao_lcu_param_rdo;
	x265_sao_lcu_param_t *sao_lcu_param_neighbor = NULL;
	x265_sao_lcu_param_t *sao_lcu_param = &(sao_param->sao_lcu_param[y_cb_cr][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two]);


	shift = 2 * X265_DISTORTION_PRECISION_ADJUSTMENT(((y_cb_cr==0) ? h->param.sps.i_bit_depth_y
																	: h->param.sps.i_bit_depth_c) - 8);
	f_cost_part_best = X265_MAX_DOUBLE;
	best_rd_cost_table_bo = X265_MAX_DOUBLE;

	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
												&sao_lcu_param_rdo);

	x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
						enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
	x265_enc_sbac_reset_bits((x265_enc_entropy_if_t*)(&h->rd_go_on_sbac_coder)) ;
	x265_enc_entropy_encode_sao_offset(h, &h->enc_entropy, &sao_lcu_param_rdo, y_cb_cr);
	f_cost_part_best = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) * lambda ;

	x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
												sao_lcu_param,
												&sao_lcu_param_rdo );
	best_dist = 0;


	for ( type_idx = 0; type_idx < MAX_NUM_SAO_TYPE; type_idx++)
	{
		est_dist = x265_enc_sample_adaptive_offset_est_sao_type_dist(h,
																	enc_sample_adaptive_offset,
																	y_cb_cr,
																	type_idx,
																	shift,
																	lambda,
																	current_distortion_table_bo,
																	current_rd_cost_table_bo);

		if( type_idx == SAO_BO )
		{
			// estimate best position
			current_rd_cost = 0.0;

			for( i = 0 ; i < SAO_MAX_BO_CLASSES - SAO_BO_LEN + 1 ; i++)
			{
				current_rd_cost = 0.0;
				for( j = i; j < i + SAO_BO_LEN ; j++)
				{
					current_rd_cost += current_rd_cost_table_bo[j];
				}

				if( current_rd_cost < best_rd_cost_table_bo)
				{
					best_rd_cost_table_bo = current_rd_cost;
					best_class_table_bo  = i;
				}
			}

			// re code all offsets
			// code center
			est_dist = 0;
			for(class_idx = best_class_table_bo;
					class_idx < best_class_table_bo + SAO_BO_LEN;
					class_idx++)
			{
				est_dist += current_distortion_table_bo[class_idx];
			}
		}
		x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, &sao_lcu_param_rdo);
		sao_lcu_param_rdo.i_length = p_sample_adaptive_offset_class_static->num_class[type_idx];
		sao_lcu_param_rdo.i_type_idx = type_idx;
		sao_lcu_param_rdo.b_merge_left_flag = 0;
		sao_lcu_param_rdo.b_merge_up_flag   = 0;
		sao_lcu_param_rdo.i_sub_type_idx = (type_idx == SAO_BO) ? best_class_table_bo : 0;
		for (class_idx = 0; class_idx < sao_lcu_param_rdo.i_length; class_idx++)
		{
			sao_lcu_param_rdo.offset[class_idx] = (int32_t)enc_sample_adaptive_offset->offset[y_cb_cr][type_idx][class_idx+sao_lcu_param_rdo.i_sub_type_idx+1];
		}

		x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
							enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
		x265_enc_sbac_reset_bits((x265_enc_entropy_if_t*)(&h->rd_go_on_sbac_coder)) ;
		x265_enc_entropy_encode_sao_offset(h, &h->enc_entropy, &sao_lcu_param_rdo, y_cb_cr);
		est_rate = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);

		enc_sample_adaptive_offset->cost[y_cb_cr][type_idx] = (double)((double)est_dist + lambda * (double) est_rate);

		if(enc_sample_adaptive_offset->cost[y_cb_cr][type_idx] < f_cost_part_best)
		{
			f_cost_part_best = enc_sample_adaptive_offset->cost[y_cb_cr][type_idx];
			x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
														sao_lcu_param,
														&sao_lcu_param_rdo );
			best_dist = est_dist;
		}
	}
	comp_distortion[0] += ((double)best_dist/lambda);
	x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
						enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
	x265_enc_entropy_encode_sao_offset(h, &h->enc_entropy, sao_lcu_param, y_cb_cr);
	x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
						enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);


	// merge left or merge up

	for ( idx_neighbor=0;idx_neighbor<2;idx_neighbor++)
	{
		sao_lcu_param_neighbor = NULL;
		if (allow_merge_left && i_cu_addr_left>=0 && idx_neighbor ==0)
		{
			sao_lcu_param_neighbor = &(sao_param->sao_lcu_param[y_cb_cr][i_cu_addr_left%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two]);
		}
		else if (allow_merge_up && i_cu_addr_up>=0 && idx_neighbor ==1)
		{
			sao_lcu_param_neighbor = &(sao_param->sao_lcu_param[y_cb_cr][i_cu_addr_up%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two]);
		}
		if (sao_lcu_param_neighbor!=NULL)
		{
			est_dist = 0;
			type_idx = sao_lcu_param_neighbor->i_type_idx;
			if (type_idx>=0)
			{
				merge_band_position = (type_idx == SAO_BO) ? sao_lcu_param_neighbor->i_sub_type_idx:0;
				for(class_idx = 0;
						class_idx < p_sample_adaptive_offset_class_static->num_class[type_idx];
						class_idx++)
				{
					i_merge_offset = sao_lcu_param_neighbor->offset[class_idx];
					est_dist += x265_enc_sample_adaptive_offset_est_sao_dist(enc_sample_adaptive_offset,
																			enc_sample_adaptive_offset->count [y_cb_cr][type_idx][class_idx+merge_band_position+1],
																			i_merge_offset,
																			enc_sample_adaptive_offset->offset_org[y_cb_cr][type_idx][class_idx+merge_band_position+1],
																			shift);
				}
			}
			else
			{
				est_dist = 0;
			}

			x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
														&comp_sao_param[idx_neighbor],
														sao_lcu_param_neighbor );
			comp_sao_param[idx_neighbor].b_merge_up_flag   = idx_neighbor;
			comp_sao_param[idx_neighbor].b_merge_left_flag = !idx_neighbor;

			comp_distortion[idx_neighbor+1] += ((double)est_dist/lambda);
		}
	}

}

void x265_enc_sample_adaptive_offset_sao_2_chroma_param_dist(x265_t *h,
															x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
															int32_t allow_merge_left,
															int32_t allow_merge_up,
															x265_sao_param_t *sao_param,
															uint32_t i_cu_addr,
															uint32_t i_cu_addr_up,
															uint32_t i_cu_addr_left,
															double lambda,
															x265_sao_lcu_param_t *cr_sao_param,
															x265_sao_lcu_param_t *cb_sao_param,
															double *distortion)
{
	int32_t i = 0;
	uint32_t j = 0;
	int32_t type_idx;
	int64_t est_dist[2];
	int32_t class_idx;
	int32_t shift = 0;
	int64_t best_dist = 0;
	int32_t current_distortion_table_bo[X265_MAX_NUM_SAO_CLASS];
	int32_t comp_idx = 0;
	int32_t idx_neighbor = 0;
	int32_t merge_band_position = 0;
	int32_t i_merge_offset = 0;

	double cost_part_best = 0.0;
	double best_rd_cost_table_bo;
	double current_rd_cost_table_bo[X265_MAX_NUM_SAO_CLASS];
	double est_rate = 0.0;
	double current_rd_cost = 0.0;
	int32_t best_class_table_bo[2] = {0, 0};
	x265_sao_lcu_param_t *sao_merge_param[2][2];
	x265_sao_lcu_param_t sao_lcu_param_rdo[2];
	x265_sao_lcu_param_t *sao_lcu_param_neighbor[2] = {NULL, NULL};
	x265_sao_lcu_param_t *sao_lcu_param[2] =
			{&(sao_param->sao_lcu_param[1][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two]),
			&(sao_param->sao_lcu_param[2][i_cu_addr%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two])};

	shift = 2 * X265_DISTORTION_PRECISION_ADJUSTMENT(h->param.sps.i_bit_depth_c-8);

	sao_merge_param[0][0] = &cr_sao_param[0];
	sao_merge_param[0][1] = &cr_sao_param[1];
	sao_merge_param[1][0] = &cb_sao_param[0];
	sao_merge_param[1][1] = &cb_sao_param[1];

	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, sao_lcu_param[0]);
	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, sao_lcu_param[1]);
	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, sao_merge_param[0][0]);
	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, sao_merge_param[0][1]);
	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, sao_merge_param[1][0]);
	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, sao_merge_param[1][1]);


	cost_part_best = X265_MAX_DOUBLE;



	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, &sao_lcu_param_rdo[0]);
	x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, &sao_lcu_param_rdo[1]);


	x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
						enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
	x265_enc_sbac_reset_bits((x265_enc_entropy_if_t*)(&h->rd_go_on_sbac_coder)) ;
	x265_enc_entropy_encode_sao_offset(h, &h->enc_entropy, &sao_lcu_param_rdo[0], 1);
	x265_enc_entropy_encode_sao_offset(h, &h->enc_entropy, &sao_lcu_param_rdo[1], 2);
	cost_part_best = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy)*lambda ;
	x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
												sao_lcu_param[0],
												&sao_lcu_param_rdo[0] );
	x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
												sao_lcu_param[1],
												&sao_lcu_param_rdo[1] );

	for ( type_idx = 0 ; type_idx < MAX_NUM_SAO_TYPE; type_idx++)
	{
		if( type_idx == SAO_BO )
		{
			// estimate best position
			for( comp_idx = 0; comp_idx < 2; comp_idx++)
			{
				current_rd_cost = 0.0;
				best_rd_cost_table_bo = X265_MAX_DOUBLE;
				est_dist[comp_idx] =
				x265_enc_sample_adaptive_offset_est_sao_type_dist(h,
																	enc_sample_adaptive_offset,
																	comp_idx+1,
																	type_idx,
																	shift,
																	lambda,
																	current_distortion_table_bo,
																	current_rd_cost_table_bo);
				for( i=0; i< SAO_MAX_BO_CLASSES - SAO_BO_LEN + 1; i++)
				{
					current_rd_cost = 0.0;
					for( j = i; j < i + SAO_BO_LEN; j++)
					{
						current_rd_cost += current_rd_cost_table_bo[j];
					}

					if( current_rd_cost < best_rd_cost_table_bo)
					{
						best_rd_cost_table_bo = current_rd_cost;
						best_class_table_bo[comp_idx]  = i;
					}
				}

				// re code all offsets
				// code center
				est_dist[comp_idx] = 0;
				for(class_idx = best_class_table_bo[comp_idx];
						class_idx < best_class_table_bo[comp_idx] + SAO_BO_LEN;
						class_idx++)
				{
					est_dist[comp_idx] += current_distortion_table_bo[class_idx];
				}
			}
		}
		else
		{
			est_dist[0] = x265_enc_sample_adaptive_offset_est_sao_type_dist(h,
																			enc_sample_adaptive_offset,
																			1,
																			type_idx,
																			shift,
																			lambda,
																			current_distortion_table_bo,
																			current_rd_cost_table_bo);
			est_dist[1] = x265_enc_sample_adaptive_offset_est_sao_type_dist(h,
																			enc_sample_adaptive_offset,
																			2,
																			type_idx,
																			shift,
																			lambda,
																			current_distortion_table_bo,
																			current_rd_cost_table_bo);
		}

		x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
							enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
		x265_enc_sbac_reset_bits((x265_enc_entropy_if_t*)(&h->rd_go_on_sbac_coder)) ;

		for( comp_idx = 0; comp_idx < 2; comp_idx++)
		{
			x265_sample_adaptive_offset_reset_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset, &sao_lcu_param_rdo[comp_idx]);
			sao_lcu_param_rdo[comp_idx].i_length = p_sample_adaptive_offset_class_static->num_class[type_idx];
			sao_lcu_param_rdo[comp_idx].i_type_idx = type_idx;
			sao_lcu_param_rdo[comp_idx].b_merge_left_flag = 0;
			sao_lcu_param_rdo[comp_idx].b_merge_up_flag   = 0;
			sao_lcu_param_rdo[comp_idx].i_sub_type_idx = (type_idx == SAO_BO) ? best_class_table_bo[comp_idx] : 0;
			for (class_idx = 0; class_idx < sao_lcu_param_rdo[comp_idx].i_length; class_idx++)
			{
				sao_lcu_param_rdo[comp_idx].offset[class_idx] = (int32_t)enc_sample_adaptive_offset->offset[comp_idx+1][type_idx][class_idx+sao_lcu_param_rdo[comp_idx].i_sub_type_idx+1];
			}
			x265_enc_entropy_encode_sao_offset(h, &h->enc_entropy, &sao_lcu_param_rdo[comp_idx], comp_idx+1);
		}
		est_rate = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);
		enc_sample_adaptive_offset->cost[1][type_idx] = (double)((double)(est_dist[0] + est_dist[1])  + lambda * (double) est_rate);

		if(enc_sample_adaptive_offset->cost[1][type_idx] < cost_part_best)
		{
			cost_part_best = enc_sample_adaptive_offset->cost[1][type_idx];
			x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
														sao_lcu_param[0],
														&sao_lcu_param_rdo[0] );
			x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
														sao_lcu_param[1],
														&sao_lcu_param_rdo[1] );
			best_dist = (est_dist[0]+est_dist[1]);
		}
	}

	distortion[0] += ((double)best_dist/lambda);
	x265_enc_sbac_load(&h->rd_go_on_sbac_coder,
						enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);
	x265_enc_entropy_encode_sao_offset(h, &h->enc_entropy, sao_lcu_param[0], 1);
	x265_enc_entropy_encode_sao_offset(h, &h->enc_entropy, sao_lcu_param[1], 2);
	x265_enc_sbac_store(&h->rd_go_on_sbac_coder,
						enc_sample_adaptive_offset->pppc_rd_sbac_coder[0][CI_TEMP_BEST]);

	// merge left or merge up

	for ( idx_neighbor=0;idx_neighbor<2;idx_neighbor++)
	{
		for( comp_idx = 0; comp_idx < 2; comp_idx++)
		{
			sao_lcu_param_neighbor[comp_idx] = NULL;
			if (allow_merge_left && i_cu_addr_left>=0 && idx_neighbor ==0)
			{
				sao_lcu_param_neighbor[comp_idx] = &(sao_param->sao_lcu_param[comp_idx+1][i_cu_addr_left%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two]);
			}
			else if (allow_merge_up && i_cu_addr_up>=0 && idx_neighbor ==1)
			{
				sao_lcu_param_neighbor[comp_idx] = &(sao_param->sao_lcu_param[comp_idx+1][i_cu_addr_up%enc_sample_adaptive_offset->sample_adaptive_offset.i_num_cu_in_width_mul_two]);
			}
			if (sao_lcu_param_neighbor[comp_idx]!=NULL)
			{
				est_dist[comp_idx] = 0;
				type_idx = sao_lcu_param_neighbor[comp_idx]->i_type_idx;
				if (type_idx>=0)
				{
					merge_band_position = (type_idx == SAO_BO)?sao_lcu_param_neighbor[comp_idx]->i_sub_type_idx:0;
					for(class_idx = 0; class_idx < p_sample_adaptive_offset_class_static->num_class[type_idx]; class_idx++)
					{
						i_merge_offset = sao_lcu_param_neighbor[comp_idx]->offset[class_idx];
						est_dist[comp_idx] +=
						x265_enc_sample_adaptive_offset_est_sao_dist(enc_sample_adaptive_offset,
																	enc_sample_adaptive_offset->count [comp_idx+1][type_idx][class_idx+merge_band_position+1],
																	i_merge_offset,
																	enc_sample_adaptive_offset->offset_org[comp_idx+1][type_idx][class_idx+merge_band_position+1],
																	shift);
					}
				}
				else
				{
					est_dist[comp_idx] = 0;
				}

				x265_sample_adaptive_offset_copy_sao_unit((x265_sample_adaptive_offset_t*)enc_sample_adaptive_offset,
															sao_merge_param[comp_idx][idx_neighbor],
															sao_lcu_param_neighbor[comp_idx] );
				sao_merge_param[comp_idx][idx_neighbor]->b_merge_up_flag   = idx_neighbor;
				sao_merge_param[comp_idx][idx_neighbor]->b_merge_left_flag = !idx_neighbor;
				distortion[idx_neighbor+1] += ((double)est_dist[comp_idx]/lambda);
			}
		}
	}
}

//! \}

