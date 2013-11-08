


#include "common.h"
#include "tr_quant_help.h"
#include "x86/rdoq_x86.h"

int32_t max_go_rice_range[5] =
{
	3, 6, 12, 24, 48
};
void x265_tr_quant_x_rate_dist_opt_quant_ssse3 (x265_t *h,
												x265_tr_quant_t *tr_quant,
												x265_data_cu_t* cu,
												int32_t* p_src_coeff,
												x265_coeff_t *p_dst_coeff,
#if X265_ADAPTIVE_QP_SELECTION
												int32_t **pp_arl_dst_coeff,
#endif
												uint32_t i_width,
												uint32_t i_height,
												uint32_t *p_abs_sum,
												enum text_type_e i_text_type,
												uint32_t i_abs_part_idx )
{
	ALIGNED_ARRAY_16( int32_t, level_double_array, [1024] ) ;
	ALIGNED_ARRAY_16( int32_t, arl_dst_coeff_array, [1024] ) ;
	ALIGNED_ARRAY_16( int32_t, max_level_array, [1024] ) ;
	int32_t i_level_double = 0 ;
	int32_t i_max_abs_level = 0 ;
	int32_t loop = 0;
	int32_t i_rate_now = 0;
	int32_t i_rate_inc_up = 0;
	int32_t i_rate_inc_down = 0;
	int32_t i_sig_rate_delta = 0 ;

	int32_t i_q_bits = 0;
	int32_t i_q_bits_sub8 = 0 ;
	uint32_t i_log2_tr_size = 0;
	uint32_t i_bit_depth = 0;
	int32_t i_transform_shift = 0;
	uint32_t i_go_rice_param = 0;
	uint32_t i_log2_blk_size = 0;
	uint32_t i_max_num_coeff = 0;
	int32_t i_scaling_list_type = 0 ;
	int32_t *p_q_coef_org = NULL;
	int32_t *p_q_coef = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t i_q_bits_c = 0;
#endif
	uint32_t i_scan_idx = 0;
	int32_t delta_u      [ 32 * 32 ];
	uint32_t *scan_cg = NULL;
	uint32_t i_cg_size = 0;
	uint32_t sig_coeff_group_flag[ X265_MLS_GRP_NUM ];
	uint32_t i_num_blk_side = 0;
	int32_t i_cg_last_scan_pos = 0;
	uint32_t i_ctx_set = 0;
	int32_t i_c1 = 0;
	int32_t i_c2 = 0;
	int32_t i_last_scan_pos = 0;
	uint32_t i_c1_idx = 0;
	uint32_t i_c2_idx = 0;
	int32_t i_base_level = 0;
	uint32_t *scan = NULL;
	uint32_t i_cg_num = 0;
	int32_t i_scan_pos = 0;
	x265_coeff_group_rd_stats_t rd_stats;
	int32_t i_cg_scan_pos = 0 ;
	uint32_t i_cg_blk_pos = 0;
	uint32_t i_cg_pos_y = 0;
	uint32_t i_cg_pos_x = 0;
	int32_t i_pattern_sig_ctx = 0;
	int32_t i_scan_posin_cg = 0;
	uint32_t i_blk_pos = 0;
	uint32_t i_level = 0;
	uint32_t i_one_ctx = 0;
	uint32_t i_abs_ctx = 0;
	uint32_t i_pos_y = 0;
	uint32_t i_pos_x = 0;
	uint16_t i_ctx_sig = 0;
	int32_t i_ctx_cbf = 0;
	int32_t i_best_last_idx_p1 = 0;
	int32_t b_found_last = 0;
	int32_t i_rd_factor = 0;
	int32_t i_last_cg = 0;
	int32_t i_abs_sum = 0 ;
	int32_t i_sub_set = 0;
	int32_t i_sub_pos = 0;
	int32_t i_first_nz_pos_in_cg = 0;
	int32_t i_last_nz_pos_in_cg = 0 ;
	uint32_t i_sign_bit = 0;
	int64_t i_min_cost_inc = 0, i_cur_cost = 0;
	int32_t i_min_pos = 0, i_final_change = 0, i_cur_change=0;
	int32_t i_cost_up = 0;
	int32_t i_cost_down = 0;
	uint32_t i_this_sign_bit = 0;

	double f_temp = 0.0;
	double f_block_uncoded_cost = 0.0;
	double f_base_cost = 0.0;
	double f_cost_zero_cg = 0.0;
	double f_best_cost = 0.0;
	double f_total_cost = 0.0;
	double *p_err_scale_org = NULL;
	double *p_err_scale = NULL;
	double cost_coeff [ 32 * 32 ];
	double cost_sig   [ 32 * 32 ];
	double cost_coeff0[ 32 * 32 ];
	double cost_coeff_group_sig[ X265_MLS_GRP_NUM ];

	int one_ctx_array [ 32 * 32 ] ;
	int abs_ctx_array [ 32 * 32 ] ;
	int go_rice_param_array [ 32 * 32 ] ;
	int c1_idx_array [ 32 * 32 ] ;
	int c2_idx_array [ 32 * 32 ] ;
	int ctx_sig_array [ 32 * 32 ] ;

	declare_x_get_coded_level1 ;
	declare_x_get_ic_rate_cost1 ;
	declare_x_get_ic_rate1 ;
	declare_x_get_rate_last1 ;


	i_q_bits = tr_quant->qp_param.i_bits;
	i_log2_tr_size = h->global.convert_to_bit[ i_width ] + 2;
	i_bit_depth = i_text_type == TEXT_LUMA ? h->param.sps.i_bit_depth_y : h->param.sps.i_bit_depth_c;
	i_transform_shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log2_tr_size;  // represents scaling through forward transform
	i_log2_blk_size = h->global.convert_to_bit[ i_width ] + 2;
	i_max_num_coeff = i_width * i_height;
	i_scaling_list_type = (x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx) ? 0 : 3) + et_table[(int32_t)i_text_type];
	assert(i_scaling_list_type < 6);

	i_q_bits = X265_QUANT_SHIFT + tr_quant->qp_param.i_per + i_transform_shift;                   // right shift of non-rdoq quantizer;  level = (coeff*i_q + offset)>>q_bits
	i_q_bits_sub8 = i_q_bits - 8 ;

	p_err_scale_org = tr_quant->err_scale[i_log2_tr_size - 2][i_scaling_list_type][tr_quant->qp_param.i_rem] ;
	p_q_coef_org = tr_quant->quant_coef[i_log2_tr_size-2][i_scaling_list_type][tr_quant->qp_param.i_rem] ;
	p_q_coef = p_q_coef_org;
	p_err_scale = p_err_scale_org;
#if X265_ADAPTIVE_QP_SELECTION
	i_q_bits_c = i_q_bits - X265_ARL_C_PRECISION ;
#endif
	i_scan_idx = x265_data_cu_get_coef_scan_idx(h,
												cu,
												i_abs_part_idx,
												i_width,
												i_text_type == TEXT_LUMA,
												x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx));

	scan_cg = h->scan.sig_last_scan[ i_scan_idx ][ i_log2_blk_size > 3 ? i_log2_blk_size-2-1 : 0  ];
	if( i_log2_blk_size == 3 )
	{
		scan_cg = h->scan.sig_last_scan_8x8[ i_scan_idx ];
	}
	else if( i_log2_blk_size == 5 )
	{
		scan_cg = h->scan.sig_last_scan_cg_32x32;
	}

	i_cg_size = (1 << X265_MLS_CG_SIZE);         // 16
	i_num_blk_side = i_width >> 2 ;
	i_cg_last_scan_pos = -1;
	i_c1 = 1;
	i_last_scan_pos = -1;
	scan = h->scan.sig_last_scan[ i_scan_idx ][ i_log2_blk_size - 1 ];
	i_cg_num = (1 << (i_log2_blk_size << 1)) >> X265_MLS_CG_SIZE ;

	memset( cost_coeff_group_sig,   0, sizeof(double) * X265_MLS_GRP_NUM );
	memset( sig_coeff_group_flag,   0, sizeof(uint32_t) * X265_MLS_GRP_NUM );

#if X265_ADAPTIVE_QP_SELECTION
	memset(*pp_arl_dst_coeff, 0, sizeof(int32_t) *  i_max_num_coeff);
#endif
	memset( cost_coeff, 0, sizeof(double) *  i_max_num_coeff );
	memset( cost_sig,   0, sizeof(double) *  i_max_num_coeff );
	memset( delta_u,       0, sizeof(int32_t) *  i_max_num_coeff );
	memset( p_dst_coeff, 0, sizeof(x265_coeff_t) *  i_max_num_coeff ) ;


	if ( 4 == i_width )
	{
		x265_rdoq_0_8_ssse3 (p_src_coeff,
							p_q_coef,
							level_double_array,
							arl_dst_coeff_array,
							max_level_array,
							i_q_bits,
							i_q_bits_c,
							p_err_scale,
							cost_coeff0,
							&f_block_uncoded_cost ) ;
	}
	if ( 8 == i_width )
	{
		x265_rdoq_1_8_ssse3 (p_src_coeff,
							p_q_coef,
							level_double_array,
							arl_dst_coeff_array,
							max_level_array,
							i_q_bits,
							i_q_bits_c,
							p_err_scale,
							cost_coeff0,
							&f_block_uncoded_cost ) ;

	}
	if ( 16 == i_width )
	{
		x265_rdoq_2_8_ssse3 (p_src_coeff,
							p_q_coef,
							level_double_array,
							arl_dst_coeff_array,
							max_level_array,
							i_q_bits,
							i_q_bits_c,
							p_err_scale,
							cost_coeff0,
							&f_block_uncoded_cost ) ;
	}
	if ( 32 == i_width )
	{
		x265_rdoq_3_8_ssse3 (p_src_coeff,
							p_q_coef,
							level_double_array,
							arl_dst_coeff_array,
							max_level_array,
							i_q_bits,
							i_q_bits_c,
							p_err_scale,
							cost_coeff0,
							&f_block_uncoded_cost ) ;
	}


	for (i_cg_scan_pos = i_cg_num-1; i_cg_scan_pos >= 0; i_cg_scan_pos--)
	{
		i_cg_blk_pos = scan_cg[ i_cg_scan_pos ];
	    i_cg_pos_y = i_cg_blk_pos / i_num_blk_side;
	    i_cg_pos_x = i_cg_blk_pos - (i_cg_pos_y * i_num_blk_side);
	    memset( &rd_stats, 0, sizeof (x265_coeff_group_rd_stats_t));

	    i_pattern_sig_ctx = x265_tr_quant_calc_pattern_sig_ctx(tr_quant,
	    														sig_coeff_group_flag,
	    														i_cg_pos_x,
	    														i_cg_pos_y,
	    														i_width,
	    														i_height);
		for ( i_scan_posin_cg = i_cg_size - 1;
	    		i_scan_posin_cg >= 0; i_scan_posin_cg-- )
	    {
	    	i_scan_pos = i_cg_scan_pos * i_cg_size + i_scan_posin_cg;
	    	//===== quantization =====
	    	i_blk_pos = scan[i_scan_pos];
	    	i_level_double = level_double_array[i_blk_pos] ;
	    	i_max_abs_level = max_level_array[i_blk_pos] ;
	    	// set coeff
	    	f_temp = p_err_scale[i_blk_pos];

#if X265_ADAPTIVE_QP_SELECTION
	    	if( tr_quant->b_use_adapt_qp_select )
	    	{
	    	//	(*pp_arl_dst_coeff)[i_blk_pos]   = (int32_t)((i_level_double + i_add_c) >> i_q_bits_c);
	    	}
#endif
	    	if ( i_max_abs_level > 0 && i_last_scan_pos < 0 )
	    	{
	    		i_last_scan_pos = i_scan_pos;
	    		i_ctx_set = (i_scan_pos < X265_SCAN_SET_SIZE || i_text_type != TEXT_LUMA) ? 0 : 2;
	    		i_cg_last_scan_pos = i_cg_scan_pos;
	    	}

	    	if ( i_last_scan_pos >= 0 )
	    	{
				//===== coefficient level estimation =====
	    		i_one_ctx = (i_ctx_set << 2) + i_c1;
	    		i_abs_ctx = i_ctx_set + i_c2;

	    		one_ctx_array[i_blk_pos] = i_one_ctx ;
	    		abs_ctx_array[i_blk_pos] = i_abs_ctx ;
	    		go_rice_param_array[i_blk_pos] = i_go_rice_param ;
	    		c1_idx_array[i_blk_pos] = i_c1_idx ;
	    		c2_idx_array[i_blk_pos] = i_c2_idx ;

	    		i_base_level = (i_c1_idx < X265_C1FLAG_NUMBER)
								? (2 + (i_c2_idx < X265_C2FLAG_NUMBER)) : 1;

	    		if( i_scan_pos == i_last_scan_pos )
	    		{
	    			x265_tr_quant_x_get_coded_level1 () ;
	    		}
	    		else
	    		{
		    		ctx_sig_array[i_blk_pos] = tr_quant->ctx_sig_list[i_text_type][i_pattern_sig_ctx][i_scan_idx][i_log2_blk_size][i_blk_pos];
		    		x265_tr_quant_x_get_coded_level2() ;
	    		}

	    		x265_tr_quant_add_rd_stats_f_sig_cost() ;

	    		//	rd_stats.f_sig_cost += cost_sig[ i_scan_pos ];
		    	if (i_level)
		    	{
		    		sig_coeff_group_flag[ i_cg_blk_pos ] = 1;

		    		x265_tr_quant_add_rd_stats_f_coded_leveland_dist();
		    		rd_stats.f_uncoded_dist += cost_coeff0[ i_blk_pos ];
		    		if ( i_scan_posin_cg != 0 )
		    		{
		    			rd_stats.i_nnz_before_pos0++;
		    		}
		    		p_dst_coeff[ i_blk_pos ] = i_level;
		    		delta_u[ i_blk_pos ] = (i_level_double - ((int32_t)i_level << i_q_bits)) >> i_q_bits_sub8;
		    	}
		    	else
		    	{
		    		delta_u[ i_blk_pos ] = i_level_double >> i_q_bits_sub8 ;
		    	}

	    		x265_tr_quant_store_coeff_sig() ;

	            //	print_double_state (cost_coeff[ i_scan_pos ]) ;


	    		if( i_level >= i_base_level )
	    		{
	    			if (i_level > max_go_rice_range[i_go_rice_param])
	    			{
	    				i_go_rice_param = X265_MIN(i_go_rice_param+ 1, 4);
	    			}
	    		}
	    		if ( i_level >= 1)
	    		{
	    			i_c1_idx ++;
	    		}

	    		//===== update bin model =====
	    		if( i_level > 1 )
	    		{
	    			i_c1 = 0;
	    			i_c2 += (i_c2 < 2);
	    			i_c2_idx ++;
	    		}
	    		else if( (i_c1 < 3) && (i_c1 > 0) && i_level)
	    		{
	    			i_c1++;
	    		}

	    		//===== context set update =====
	    		if( ( i_scan_pos % X265_SCAN_SET_SIZE == 0 ) && ( i_scan_pos > 0 ) )
	    		{
	    			i_c2 = 0;
	    			i_go_rice_param = 0;

	    			i_c1_idx = 0;
	    			i_c2_idx = 0;
	    			i_ctx_set = (i_scan_pos == X265_SCAN_SET_SIZE
	    						|| i_text_type != TEXT_LUMA) ? 0 : 2;
	    			if( i_c1 == 0 )
	    			{
	    				i_ctx_set++;
	    			}
	    			i_c1 = 1;
	    		}
	    	}
	    	else
	    	{
	    		f_base_cost += cost_coeff0[ i_blk_pos ];
	    	}

	    	if (i_scan_posin_cg == 0 )
	    	{
	    		rd_stats.f_sig_cost_0 = cost_sig[ i_scan_pos ];
	    	}

	    } //end for (i_scan_posin_cg)


		if (i_cg_last_scan_pos >= 0)
	    {
	    	if( i_cg_scan_pos )
	    	{
	    		if (sig_coeff_group_flag[ i_cg_blk_pos ] == 0)
	    		{
	    			i_ctx_sig = x265_tr_quant_get_sig_coeff_group_ctx_inc(tr_quant,
	    																sig_coeff_group_flag,
	    																i_cg_pos_x,
	    																i_cg_pos_y,
	    																i_width,
	    																i_height);
	    			/*
	    			__asm__ __volatile__
	    	        (
			    	    "FLDL (%3)\n"
			    	    "FLDL (%2)\n"
		    	        "FLDL (%1)\n"
	    	        	"FILDL (%0)\n"
	    	        	"FMULP %%ST(0), %%ST(1)\n"
		    	        "FSUBP %%ST(0), %%ST(1)\n"
		    	        "FADDP %%ST(0), %%ST(1)\n"
	    	        	"FSTPL (%3)\n"
	    	        	:
	    	        	:
	    	        	"a"(&tr_quant->est_bits_sbac->significant_coeff_group_bits[ i_ctx_sig ][0]),
	    	            "b"(&tr_quant->f_lambda),
	    	            "c"(&rd_stats.f_sig_cost),
	    	            "d"(&f_base_cost)
	    	            :
	    	        ) ;
	    	        */

	    			//	tr_quant->est_bits_sbac->significant_coeff_group_bits[ i_ctx_num_sig ][ i_significance_coeff_group ]
	    			f_base_cost += x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																	0,
	    																	i_ctx_sig)
									- rd_stats.f_sig_cost;
	    			cost_coeff_group_sig[ i_cg_scan_pos ] = x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																							0,
	    																							i_ctx_sig);
	    		}
	    		else
	    		{
	    			if (i_cg_scan_pos < i_cg_last_scan_pos) //skip the last coefficient group, which will be handled together with last position below.
	    			{
	    				if ( rd_stats.i_nnz_before_pos0 == 0 )
	    				{
	    					f_base_cost -= rd_stats.f_sig_cost_0;
	    					rd_stats.f_sig_cost -= rd_stats.f_sig_cost_0;
	    				}
	    				// rd-cost if sig_coeff_group_flag = 0, initialization
	    				f_cost_zero_cg = f_base_cost;

	    				// add sig_coeff_group_flag cost to total cost
	    				i_ctx_sig = x265_tr_quant_get_sig_coeff_group_ctx_inc(tr_quant,
	    																		sig_coeff_group_flag,
	    																		i_cg_pos_x,
	    																		i_cg_pos_y,
	    																		i_width,
	    																		i_height);
	    				if (i_cg_scan_pos < i_cg_last_scan_pos)
	    				{
	    					f_base_cost  += x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																			1,
	    																			i_ctx_sig);
	    					f_cost_zero_cg += x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																				0,
	    																				i_ctx_sig);
	    					cost_coeff_group_sig[ i_cg_scan_pos ] = x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																									1,
	    																									i_ctx_sig);
	    				}

	    				x265_tr_quant_add_rd_stats_cost() ;

	    				// try to convert the current coeff group from non-zero to all-zero
//	    				f_cost_zero_cg += rd_stats.f_uncoded_dist;  // distortion for resetting non-zero levels to zero levels
//	    				f_cost_zero_cg -= rd_stats.f_coded_leveland_dist;   // distortion and level cost for keeping all non-zero levels
//	    				f_cost_zero_cg -= rd_stats.f_sig_cost;     // sig cost for all coeffs, including zero levels and non-zerl levels

	    				// if we can save cost, change this block to all-zero block
	    				if ( f_cost_zero_cg < f_base_cost )
	    				{
	    					sig_coeff_group_flag[ i_cg_blk_pos ] = 0;
	    					f_base_cost = f_cost_zero_cg;
	    					if (i_cg_scan_pos < i_cg_last_scan_pos)
	    					{
	    						cost_coeff_group_sig[ i_cg_scan_pos ] = x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																										0,
	    																										i_ctx_sig);
	    					}
	    					// reset coeffs to 0 in this block
	    					for ( i_scan_posin_cg = i_cg_size-1; i_scan_posin_cg >= 0; i_scan_posin_cg--)
	    					{
	    						i_scan_pos = i_cg_scan_pos*i_cg_size + i_scan_posin_cg;
	    						i_blk_pos = scan[ i_scan_pos ];

	    						if (p_dst_coeff[ i_blk_pos ])
	    						{
	    							p_dst_coeff [ i_blk_pos ] = 0;
	    							cost_coeff[ i_scan_pos ] = cost_coeff0[ i_blk_pos ];
	    							cost_sig  [ i_scan_pos ] = 0;
	    						}
	    					}
	    				} // end if ( f_cost_all_zeros < f_base_cost )
	    			}
	    		} // end if if (sig_coeff_group_flag[ i_cg_blk_pos ] == 0)
	    	}
	    	else
	    	{
	    		sig_coeff_group_flag[ i_cg_blk_pos ] = 1;
	    	}
	    }
	} //end for (i_cg_scan_pos)


	//===== estimate last position =====
	if ( i_last_scan_pos < 0 )
	{
		return;
	}


	if( !x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx )
		&& i_text_type == TEXT_LUMA
		&& x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) == 0 )
	{
		f_best_cost  = f_block_uncoded_cost + x265_tr_quant_x_get_i_cost(tr_quant,
																		tr_quant->est_bits_sbac->block_root_cbp_bits[ i_ctx_cbf ][ 0 ] );
		f_base_cost += x265_tr_quant_x_get_i_cost(tr_quant,
												tr_quant->est_bits_sbac->block_root_cbp_bits[ i_ctx_cbf ][ 1 ] );
	}
	else
	{
		i_ctx_cbf = x265_data_cu_get_ctx_qt_cbf(cu,
												i_text_type,
												x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) );
		i_ctx_cbf = ( i_text_type ? TEXT_CHROMA : i_text_type ) * X265_NUM_QT_CBF_CTX + i_ctx_cbf;
		f_best_cost  = f_block_uncoded_cost + x265_tr_quant_x_get_i_cost(tr_quant,
																		tr_quant->est_bits_sbac->block_cbp_bits[ i_ctx_cbf ][ 0 ] );
		f_base_cost += x265_tr_quant_x_get_i_cost(tr_quant,
												tr_quant->est_bits_sbac->block_cbp_bits[ i_ctx_cbf ][ 1 ] );
	}

	b_found_last = 0;
	for (i_cg_scan_pos = i_cg_last_scan_pos; i_cg_scan_pos >= 0; i_cg_scan_pos--)
	{
	    i_cg_blk_pos = scan_cg[ i_cg_scan_pos ];

	    f_base_cost -= cost_coeff_group_sig [ i_cg_scan_pos ];
	    if (sig_coeff_group_flag[ i_cg_blk_pos ])
	    {
	    	for (i_scan_posin_cg = i_cg_size-1; i_scan_posin_cg >= 0; i_scan_posin_cg--)
	    	{
	    		i_scan_pos = i_cg_scan_pos * i_cg_size + i_scan_posin_cg;
	    		if (i_scan_pos > i_last_scan_pos)
	    		{
	    			continue;
	    		}
	    		i_blk_pos = scan[i_scan_pos];

	    		if( p_dst_coeff[ i_blk_pos ] )
	    		{
	    			i_pos_y = i_blk_pos >> i_log2_blk_size;
	    			i_pos_x = i_blk_pos - ( i_pos_y << i_log2_blk_size );

	    			if (i_scan_idx == SCAN_VER)
	    			{
	    				x265_tr_quant_x_get_rate_last1(i_pos_y,i_pos_x) ;
	    			}
	    			else
	    			{
	    				x265_tr_quant_x_get_rate_last1(i_pos_x,i_pos_y);
	    			}

	    			if( f_total_cost < f_best_cost )
	    			{
	    				i_best_last_idx_p1 = i_scan_pos + 1;
	    				f_best_cost = f_total_cost;
	    			}
	    			if( p_dst_coeff[ i_blk_pos ] > 1 )
	    			{
	    				b_found_last = 1;
	    				break;
	    			}
	    			f_base_cost -= cost_coeff[ i_scan_pos ];
	    			f_base_cost += cost_coeff0[ i_blk_pos ];
	    		}
	    		else
	    		{
	    			f_base_cost -= cost_sig[ i_scan_pos ];
	    		}
	    	} //end for
	    	if (b_found_last)
	    	{
	    		break;
	    	}
	    } // end if (sig_coeff_group_flag[ i_cg_blk_pos ])
	} // end for

	for ( i_scan_pos = 0; i_scan_pos < i_best_last_idx_p1; i_scan_pos++ )
	{
	    i_blk_pos = scan[ i_scan_pos ];
	    i_level  = p_dst_coeff[ i_blk_pos ];
	    *p_abs_sum += i_level;
	    p_dst_coeff[ i_blk_pos ] = ( p_src_coeff[ i_blk_pos ] < 0 ) ? -i_level : i_level;
	}

	//===== clean uncoded coefficients =====
	for ( i_scan_pos = i_best_last_idx_p1; i_scan_pos <= i_last_scan_pos; i_scan_pos++ )
	{
		p_dst_coeff[ scan[ i_scan_pos ] ] = 0;
	}

	if( h->pps[0].i_sign_hide_flag && *p_abs_sum>=2)
	{
		i_rd_factor = (int32_t) (
	                     inv_quant_scales[tr_quant->qp_param.i_rem]
	                     * inv_quant_scales[tr_quant->qp_param.i_rem]
	                     * (1 << (2 * tr_quant->qp_param.i_per))
	                     / tr_quant->f_lambda / 16 / (1 << X265_DISTORTION_PRECISION_ADJUSTMENT(2 * (i_bit_depth - 8)))
	                     + 0.5);
		i_last_cg = -1;

	    for( i_sub_set = (i_width*i_height-1) >> X265_LOG2_SCAN_SET_SIZE;
	    		i_sub_set >= 0; i_sub_set-- )
	    {
	    	i_sub_pos = i_sub_set << X265_LOG2_SCAN_SET_SIZE;
	    	i_first_nz_pos_in_cg = X265_SCAN_SET_SIZE ;
	    	i_last_nz_pos_in_cg = -1 ;
	    	i_abs_sum = 0 ;

	    	for( loop = X265_SCAN_SET_SIZE - 1; loop >= 0; --loop )
	    	{
	    		if( p_dst_coeff[ scan[ loop + i_sub_pos ]] )
	    		{
	    			i_last_nz_pos_in_cg = loop;
	    			break;
	    		}
	    	}

	    	for(loop = 0; loop < X265_SCAN_SET_SIZE ; loop++ )
	    	{
	    		if( p_dst_coeff[ scan[ loop + i_sub_pos ]] )
	    		{
	    			i_first_nz_pos_in_cg = loop;
	    			break;
	    		}
	    	}

	    	for(loop = i_first_nz_pos_in_cg; loop <= i_last_nz_pos_in_cg; loop++ )
	    	{
	    		i_abs_sum += p_dst_coeff[ scan[ loop + i_sub_pos ]];
	    	}

	    	if(i_last_nz_pos_in_cg>=0 && i_last_cg==-1)
	    	{
	    		i_last_cg = 1;
	    	}

	    	if( i_last_nz_pos_in_cg - i_first_nz_pos_in_cg >= X265_SBH_THRESHOLD )
	    	{
	    		i_sign_bit = (p_dst_coeff[scan[i_sub_pos+i_first_nz_pos_in_cg]]>0?0:1);
	    		if( i_sign_bit!=(i_abs_sum&0x1) )  // hide but need tune
	    		{
	    			// calculate the cost
	    			i_min_cost_inc = X265_MAX_INT64 ;
	    			i_cur_cost = X265_MAX_INT64;
	    			i_min_pos = -1 ;
	    			i_final_change = 0 ;
	    			i_cur_change = 0 ;

	    			for( loop = (i_last_cg==1 ? i_last_nz_pos_in_cg : X265_SCAN_SET_SIZE - 1) ;
	    					loop >= 0; --loop )
	    			{
	    				i_blk_pos = scan[ loop + i_sub_pos ];
						i_sig_rate_delta = ((loop + i_sub_pos == i_last_scan_pos)
											? 0
											:   tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][ 1 ]
											  - tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][ 0 ]) ;
	    				if(p_dst_coeff[ i_blk_pos ] != 0 )
	    				{
	    					i_abs_level3 = (p_dst_coeff[i_blk_pos] >= 0 ? p_dst_coeff[i_blk_pos] : -p_dst_coeff[i_blk_pos]) ;
	    					x265_tr_quant_x_get_ic_rate1(i_rate_now) ;
	    					i_abs_level3 += 1 ;
	    					x265_tr_quant_x_get_ic_rate1(i_rate_inc_up) ;
	    	    			i_rate_inc_up -= i_rate_now ;
	    					i_abs_level3 -= 2 ;
	    					x265_tr_quant_x_get_ic_rate1(i_rate_inc_down) ;
	    	    			i_rate_inc_down -= i_rate_now ;

	    	    			i_cost_up = i_rd_factor * ( - delta_u[i_blk_pos] ) + i_rate_inc_up ;
	    					i_cost_down = i_rd_factor * (   delta_u[i_blk_pos] ) + i_rate_inc_down
	    					              -   ( MY_ABS(p_dst_coeff[i_blk_pos])==1?((1<<15)+i_sig_rate_delta):0 );

	    					if(i_last_cg==1 && i_last_nz_pos_in_cg==loop && MY_ABS(p_dst_coeff[i_blk_pos])==1)
	    					{
	    						i_cost_down -= (4<<15) ;
	    					}

	    					if(i_cost_up < i_cost_down)
	    					{
	    						i_cur_cost = i_cost_up;
	    						i_cur_change =  1 ;
	    					}
	    					else
	    					{
	    						i_cur_change = -1 ;
	    						if(loop==i_first_nz_pos_in_cg && MY_ABS(p_dst_coeff[i_blk_pos])==1)
	    						{
	    							i_cur_cost = X265_MAX_INT64 ;
	    						}
	    						else
	    						{
	    							i_cur_cost = i_cost_down ;
	    						}
	    					}
	    				}
	    				else
	    				{
	    	    			i_rate_inc_up = tr_quant->est_bits_sbac->greater_one_bits[ one_ctx_array[i_blk_pos] ][ 0 ];
	    					i_cur_cost = i_rd_factor * ( - (MY_ABS(delta_u[i_blk_pos])) ) + (1<<15) + i_rate_inc_up + i_sig_rate_delta ;
	    					i_cur_change = 1 ;

	    					if(loop<i_first_nz_pos_in_cg)
	    					{
	    						i_this_sign_bit = (p_src_coeff[i_blk_pos] >= 0 ? 0 : 1 ) ;
	    						if(i_this_sign_bit != i_sign_bit )
	    						{
	    							i_cur_cost = X265_MAX_INT64;
	    						}
	    					}
	    				}

	    				if( i_cur_cost<i_min_cost_inc)
	    				{
	    					i_min_cost_inc = i_cur_cost ;
	    					i_final_change = i_cur_change ;
	    					i_min_pos = i_blk_pos ;
	    				}
	    			}

	    			if(p_q_coef[i_min_pos] == 32767 || p_q_coef[i_min_pos] == -32768)
	    			{
	    				i_final_change = -1;
	    			}

	    			if(p_src_coeff[i_min_pos]>=0)
	    			{
	    				p_dst_coeff[i_min_pos] += i_final_change ;
	    			}
	    			else
	    			{
	    				p_dst_coeff[i_min_pos] -= i_final_change ;
	    			}
	    		}
	    	}

	    	if(i_last_cg==1)
	    	{
	    		i_last_cg=0 ;
	    	}
	    }
	}
}


void x265_tr_quant_ic_rate_cost_init ( x265_tr_quant_t *tr_quant )
{
	int32_t i_go_rice_param = 0 ;
	int32_t i_symbol = 0 ;

	for ( i_go_rice_param = 0 ; i_go_rice_param < 5 ; ++ i_go_rice_param)
	{
		for ( i_symbol = 0 ; i_symbol < X265_IC_RATE_COST_TABLE_LENGTH ; ++ i_symbol )
		{
			tr_quant->ic_rate_cost[i_go_rice_param][i_symbol]
			        = x265_tr_quant_get_init_ic_rate_cost_value(tr_quant,
																i_go_rice_param,
																i_symbol ) ;
		}
	}
}

int32_t x265_tr_quant_get_init_ic_rate_cost_value(x265_tr_quant_t *tr_quant,
												int32_t i_go_rice_param,
												int32_t i_symbol )
{
	int32_t i_length = 0 ;
	int32_t i_init_ic_rate_cost_value = 0 ;

	if (i_symbol < (X265_COEF_REMAIN_BIN_REDUCTION << i_go_rice_param))
	{
		i_init_ic_rate_cost_value += ((i_symbol>>i_go_rice_param)+1+i_go_rice_param)<< 15;
	}
	else
	{
		i_length = i_go_rice_param;
		i_symbol  = i_symbol - ( X265_COEF_REMAIN_BIN_REDUCTION << i_go_rice_param);
		while (i_symbol >= (1<<i_length))
		{
			i_symbol -=  (1<<(i_length++));
		}
		i_init_ic_rate_cost_value += (X265_COEF_REMAIN_BIN_REDUCTION
										+ i_length + 1 - i_go_rice_param
										+ i_length ) << 15;
	}

	return i_init_ic_rate_cost_value ;
}


