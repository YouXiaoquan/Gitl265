
#include "common/common.h"
#include "set.h"
#include "enc_global.h"
#include "enc_top.h"
#include "enc_gop.h"



void x265_reference_update ( x265_t *h, x265_frame_t *frame ) ;
void x265_reference_remove ( x265_t *h ) ;

int x265_find_matching_ltrp ( x265_t *h,
							x265_slice_t *slice,
							uint32_t *ltrps_index,
							int32_t ltrp_poc,
							int32_t b_used_flag )
{
	// Bool state = true, state2 = false;
	int32_t i_lsb = 0 ;
	int32_t loop = 0 ;
	i_lsb = ltrp_poc % (1 << h->sps[0].i_bits_for_poc) ;
	for ( loop = 0; loop < h->sps[0].i_num_long_term_ref_pic_sps ; ++ loop )
	{
		if ( (i_lsb == h->sps[0].lt_ref_pic_poc_lsb_sps[loop])
				&& (b_used_flag == h->sps[0].used_by_curr_pic_lt_sps_flag[loop] ) )
		{
			*ltrps_index = loop ;
			return 1;
		}
	}
	return 0 ;

}

void x265_x_pred_weight_table_write ( x265_t *h, bs_t *s, x265_slice_t *slice )
{
	int32_t loop = 0 ;
	int32_t b_chroma = 0 ; // color always present in HEVC ?
	int32_t i_nb_ref = 0 ;
	int32_t b_denom_coded = 0 ;
	int32_t i_mode = 0 ;
	int32_t i_num_ref = 0 ;
	int32_t i_ref_idx = 0 ;
	int32_t i_total_signalled_weight_flags = 0 ;
	int32_t i_delta_denom = 0 ;
	int32_t i_delta_weight = 0 ;
	int32_t i_pred = 0 ;
	int32_t i_delta_chroma = 0 ;
	enum ref_pic_list_e i_ref_pic_list = 0 ;
	x265_wp_scaling_param_t *wp_scaling_param = NULL ;

	b_chroma = 1 ; // color always present in HEVC ?
	i_nb_ref = (slice->i_slice_type == B_SLICE ) ? (2) : (1);

	if ( (slice->i_slice_type == P_SLICE && h->pps[0].b_use_weight_pred)
			|| (slice->i_slice_type == B_SLICE && h->pps[0].b_use_weighted_bi_pred ) )
	{
		i_mode = 1; // explicit
	}
	if(i_mode == 1)
	{
		for ( i_num_ref = 0 ; i_num_ref < i_nb_ref ; ++ i_num_ref )
		{
			i_ref_pic_list = ( i_num_ref ? REF_PIC_LIST_1 : REF_PIC_LIST_0 );

			for ( i_ref_idx = 0 ; i_ref_idx < h->i_ref[i_ref_pic_list] ; i_ref_idx++ )
			{
				wp_scaling_param = slice->weight_pred_table[i_ref_pic_list][i_ref_idx] ;
				if ( !b_denom_coded )
				{
					bs_write_ue ( s, wp_scaling_param[0].i_log2_weight_denom );     // ue(v): luma_log2_weight_denom

					if( b_chroma )
					{
						i_delta_denom = (wp_scaling_param[1].i_log2_weight_denom - wp_scaling_param->i_log2_weight_denom ) ;
						bs_write_se ( s, i_delta_denom );       // se(v): delta_chroma_log2_weight_denom
					}
					b_denom_coded = 1 ;
				}
				bs_write1 ( s, wp_scaling_param[0].b_present_flag ) ;               // u(1): luma_weight_lX_flag
				i_total_signalled_weight_flags += wp_scaling_param[0].b_present_flag ;
			}
			if (b_chroma)
			{
				for ( i_ref_idx = 0 ; i_ref_idx <h->i_ref[i_ref_pic_list] ; i_ref_idx++ )
				{
					wp_scaling_param = slice->weight_pred_table[i_ref_pic_list][i_ref_idx] ;
					bs_write1 ( s, wp_scaling_param[1].b_present_flag ) ;           // u(1): chroma_weight_lX_flag
					i_total_signalled_weight_flags += 2 * wp_scaling_param[1].b_present_flag ;
				}
			}

			for ( i_ref_idx = 0 ; i_ref_idx < h->i_ref[i_ref_pic_list] ; i_ref_idx++ )
			{
				wp_scaling_param = slice->weight_pred_table[i_ref_pic_list][i_ref_idx] ;
				if ( wp_scaling_param[0].b_present_flag )
				{
					i_delta_weight = (wp_scaling_param[0].i_weight - (1  << wp_scaling_param[0].i_log2_weight_denom ) ) ;
					bs_write_se ( s, i_delta_weight );                  // se(v): delta_luma_weight_lX
					bs_write_se ( s, wp_scaling_param[0].offset );                       // se(v): luma_offset_lX
				}

				if ( b_chroma )
				{
					if ( wp_scaling_param[1].b_present_flag )
					{
						for ( loop = 1 ; loop < 3 ; ++ loop )
						{
							i_delta_weight = (wp_scaling_param[loop].i_weight - (1<<wp_scaling_param[1].i_log2_weight_denom));
							bs_write_se ( s, i_delta_weight );            // se(v): delta_chroma_weight_lX

							i_pred = ( 128 - ( ( 128*wp_scaling_param[loop].i_weight)>>(wp_scaling_param[loop].i_log2_weight_denom) ) );
							i_delta_chroma = (wp_scaling_param[loop].i_offset - i_pred);
							bs_write_se ( s, i_delta_chroma );            // se(v): delta_chroma_offset_lX
						}
					}
				}
			}
		}
		assert(i_total_signalled_weight_flags<=24);
	}
}

void x265_slice_header_write( x265_t *h, bs_t *s, x265_slice_t *slice )
{
	int32_t loop = 0 ;
	int32_t i_max_slice_segment_address = 0 ;
	int32_t i_bits_slice_segment_address = 0;
	int32_t i_ctu_address = 0 ;
	int32_t i_slice_segment_address = 0 ;
    int32_t i_pic_order_cnt_lsb = 0 ;
	int32_t i_num_bits = 0 ;
	int32_t i_num_ltrp_in_sh = 0 ;
	int32_t ltrp_in_sps[X265_MAX_NUM_REF_PICS];
	int32_t i_num_ltrp_in_sps = 0;
	uint32_t i_ltrp_index;
	int32_t i_counter = 0;
	int32_t i_bits_for_ltrp_in_sps = 0;
	int32_t i_prev_delta_msb = 0, i_prev_lsb = 0;
	int32_t i_offset = 0 ;
	int32_t b_delta_flag = 0 ;
	int32_t i_difference_in_delta_msb = 0 ;
	int32_t b_override_flag = 0 ;
	int32_t i_num_rps_curr_temp_list0 = 0 ;
	int32_t i_num_rps_curr_temp_list1 = 0 ;
	int32_t i_length = 1 ;
	int32_t i_enc_cabac_table_idx = 0 ;
	int32_t b_enc_cabac_init_flag = 0 ;
	int32_t i_code = 0 ;
	int32_t b_is_sao_enabled = 0 ;
	int32_t b_is_dbf_enabled = 0 ;
	enum slice_type_e i_slice_type = 0 ;
	x265_ref_pic_list_modification_t *ref_pic_list_modification = NULL ;

	x265_sao_param_t *sao_param = NULL ;
    x265_reference_picture_set_t *rps = NULL ;

	if ( h->sps[0].b_use_sao )
	{
		sao_param = h->sao_param ;
		slice->b_sao_enabled_flag = sao_param->b_sao_flag[0] ;
		slice->b_sao_enabled_flag_chroma = sao_param->b_sao_flag[1] ;
	}

	i_max_slice_segment_address = h->cu.pic.i_num_cus_in_frame ;
	i_bits_slice_segment_address = 0 ;
	while(i_max_slice_segment_address>(1<<i_bits_slice_segment_address))
	{
		i_bits_slice_segment_address++;
	}

	if (slice->b_next_slice)
	{
		// Calculate slice address
	    i_ctu_address = (slice->i_slice_cur_start_cu_addr / h->cu.pic.i_num_partitions );
	}
	else
	{
		// Calculate slice address
	    i_ctu_address = (slice->i_slice_segment_cur_start_cu_addr / h->cu.pic.i_num_partitions );
	}
	//write slice address
	i_slice_segment_address = h->cu_order_map[i_ctu_address] ;

	bs_write1 ( s, i_slice_segment_address == 0 ) ;
	if ( x265_slice_set_get_rap_pic_flag ( slice ) )
	{
		bs_write1 ( s, 0 ) ;
	}
	bs_write_ue ( s, h->pps[0].i_pps_id ) ;
	slice->b_dependent_slice_segment_flag = !slice->b_next_slice ;
	if ( h->pps[0].b_dependent_slice_segments_enabled_flag && ( i_slice_segment_address !=0 ) )
	{
		bs_write1 ( s, slice->b_dependent_slice_segment_flag ? 1 : 0 ) ;
	}
	if(i_slice_segment_address>0)
	{
		bs_write (s, i_bits_slice_segment_address, i_slice_segment_address );
	}

	if ( !slice->b_dependent_slice_segment_flag )
	{
	    for ( loop = 0; loop < h->pps[0].i_num_extra_slice_header_bits ; ++ loop )
	    {
			bs_write1 ( s, 0 ) ;
	    }

		bs_write_ue ( s, slice->i_slice_type ) ;

	    if ( h->pps[0].b_output_flag_present_flag )
	    {
			bs_write1 ( s, slice->b_pic_output_flag ? 1 : 0 ) ;
	    }

	    // in the first version chroma_format_idc is equal to one, thus colour_plane_id will not be present
	    assert ( h->sps[0].i_chroma_format_idc == 1 ) ;
	    // if( separate_colour_plane_flag  ==  1 )
	    //   colour_plane_id                                      u(2)

	    if( ! x265_slice_get_idr_pic_flag ( slice ) )
	    {
	    	i_pic_order_cnt_lsb = (h->fdec->i_poc - slice->i_last_idr + (1 << h->sps[0].i_bits_for_poc)) % (1 << h->sps[0].i_bits_for_poc);
	    	bs_write ( s, h->sps[0].i_bits_for_poc, i_pic_order_cnt_lsb ) ;
	    	rps = slice->rps ;
	    	if(slice->i_rps_idx < 0)
	    	{
		    	bs_write1 ( s, 0 ) ;
	    		x265_short_term_ref_pic_set_write ( s, rps, 1, h->sps[0].rps_list.i_number_of_reference_picture_sets ) ;
	    	}
	    	else
	    	{
		    	bs_write1 ( s, 1 ) ;
	    		i_num_bits = 0;
	    		while ((1 << i_num_bits) < h->sps[0].rps_list.i_number_of_reference_picture_sets )
	    		{
	    			i_num_bits ++ ;
	    		}
	    		if (i_num_bits > 0)
	    		{
	    	    	bs_write ( s, i_num_bits, slice->i_rps_idx ) ;
	    		}
	    	}
	    	if ( h->sps[0].b_long_term_refs_present )
	    	{
	    		i_num_ltrp_in_sh = rps->i_number_of_longterm_pictures ;
	    		i_num_ltrp_in_sps = 0 ;
	    		i_counter = 0 ;
	    		for( loop = rps->i_number_of_pictures - 1 ;
	    				loop > rps->i_number_of_pictures - rps->i_number_of_longterm_pictures - 1 ;
	    				-- loop )
	    		{
	    			if ( x265_find_matching_ltrp( h,
	    										slice,
	    										&i_ltrp_index,
	    										rps->poc[loop],
	    										rps->used[loop] ))
	    			{
	    				ltrp_in_sps[i_num_ltrp_in_sps] = i_ltrp_index;
	    				i_num_ltrp_in_sps++;
	    			}
	    			else
	    			{
	    				i_counter++;
	    			}
	    		}
	    		i_num_ltrp_in_sh -= i_num_ltrp_in_sps;

	    		while (h->sps[0].i_num_long_term_ref_pic_sps > (1 << i_bits_for_ltrp_in_sps))
	    		{
	    			i_bits_for_ltrp_in_sps++;
	    		}
	    		if (h->sps[0].i_num_long_term_ref_pic_sps > 0)
	    		{
	    			bs_write_ue ( s, i_num_ltrp_in_sps ) ;
	    		}
    			bs_write_ue ( s, i_num_ltrp_in_sh ) ;
	    		// Note that the LSBs of the LT ref. pic. POCs must be sorted before.
	    		// Not sorted here because LT ref indices will be used in setRefPicList()
	    		i_prev_delta_msb = 0 ;
	    		i_prev_lsb = 0;
	    		i_offset = rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures ;
	    		for( loop = rps->i_number_of_pictures - 1 ; loop > i_offset - 1 ; -- loop )
	    		{
	    			if (i_counter < i_num_ltrp_in_sps)
	    			{
	    				if (i_bits_for_ltrp_in_sps > 0)
	    				{
	    					bs_write( s, i_bits_for_ltrp_in_sps, ltrp_in_sps[i_counter] ) ;
	    				}
	    			}
	    			else
	    			{
	    				bs_write ( s, h->sps[0].i_bits_for_poc, rps->poc_lsb_lt[loop] ) ;
	    				bs_write1 ( s, rps->used[loop] ) ;
	    			}
    				bs_write1 ( s, rps->delta_poc_msb_present_flag[loop] ) ;

	    			if ( rps->delta_poc_msb_present_flag[loop] )
	    			{
	    				b_delta_flag = 0 ;
	    				//  First LTRP from SPS                 ||  First LTRP from SH                              || curr LSB            != prev LSB
	    				if( (loop == rps->i_number_of_pictures - 1)
	    						|| (loop == rps->i_number_of_pictures - 1 - i_num_ltrp_in_sps)
	    						|| (rps->poc_lsb_lt[loop] != i_prev_lsb) )
	    				{
	    					b_delta_flag = 1 ;
	    				}
	    				if(b_delta_flag)
	    				{
	        				bs_write_ue ( s, rps->delta_poc_msb_cycle_lt[loop] ) ;
	    				}
	    				else
	    				{
	    					i_difference_in_delta_msb = rps->delta_poc_msb_cycle_lt[loop] - i_prev_delta_msb;
	    					assert(i_difference_in_delta_msb >= 0);
	        				bs_write_ue ( s, i_difference_in_delta_msb ) ;
	    				}
	    				i_prev_lsb = rps->poc_lsb_lt[loop];
	    				i_prev_delta_msb = rps->delta_poc_msb_cycle_lt[loop] ;
	    			}
	    		}
	    	}
	    	if (h->sps[0].b_tmvp_flags_present)
	    	{
				bs_write1 ( s, slice->b_enable_tmvp_flag ? 1 : 0 ) ;
	    	}
	    }
	    if(h->sps[0].b_use_sao)
	    {
			bs_write1 ( s, slice->b_sao_enabled_flag ? 1 : 0 ) ;
			bs_write1 ( s, h->sao_param->b_sao_flag[1] ? 1 : 0 ) ;
	    }

	    //check if numrefidxes match the defaults. If not, override

	    if ( ! ( I_SLICE == slice->i_slice_type ) )
	    {
	    	b_override_flag = (h->i_ref[REF_PIC_LIST_0] != h->pps[0].i_num_ref_idx_l0_default_active ) ;
	    	bs_write1 ( s, b_override_flag ? 1 : 0 );
	    	if (b_override_flag)
	    	{
	    		bs_write_ue ( s,  h->i_ref[REF_PIC_LIST_0] - 1 );
	    		if ( B_SLICE == slice->i_slice_type )
	    		{
		    		bs_write_ue ( s,  h->i_ref[REF_PIC_LIST_1] - 1 );
	    		}
	    		else
	    		{
	    			h->i_ref[REF_PIC_LIST_1] = 0 ;
	    		}
	    	}
	    }
	    else
	    {
			h->i_ref[REF_PIC_LIST_0] = 0 ;
			h->i_ref[REF_PIC_LIST_1] = 0 ;
	    }

	    if ( h->pps[0].b_lists_modification_present_flag
	    		&& ( x265_slice_get_num_rps_curr_temp_list ( slice ) > 1) )
	    {
	    	ref_pic_list_modification = & h->ref_pic_list_modification ;
	    	if ( ! ( I_SLICE == slice->i_slice_type ) )
	    	{
	    		bs_write1 ( s, h->ref_pic_list_modification.ref_pic_set_idx_l0 ? 1 : 0 ) ;
	    		if ( h->ref_pic_list_modification.ref_pic_set_idx_l0 )
	    		{
	    			i_num_rps_curr_temp_list0 = x265_slice_get_num_rps_curr_temp_list ( slice ) ;
	    			if (i_num_rps_curr_temp_list0 > 1)
	    			{
	    				i_length = 1;
	    				i_num_rps_curr_temp_list0 --;
	    				while ( i_num_rps_curr_temp_list0 >>= 1)
	    				{
	    					i_length ++;
	    				}
	    				for( loop = 0; loop < h->i_ref[REF_PIC_LIST_0] ; ++ loop )
	    				{
	    					bs_write ( s, i_length, ref_pic_list_modification->ref_pic_set_idx_l0[loop] ) ;
	    				}
	    			}
	    		}
	    	}
	    	if( B_SLICE == slice->i_slice_type )
	    	{
	    		bs_write1 ( s, h->ref_pic_list_modification.ref_pic_set_idx_l1 ? 1 : 0 ) ;
	    		if ( h->ref_pic_list_modification.ref_pic_set_idx_l1 )
	    		{
	    			i_num_rps_curr_temp_list1 = x265_slice_get_num_rps_curr_temp_list ( slice ) ;
	    			if (i_num_rps_curr_temp_list1 > 1)
	    			{
	    				i_length = 1;
	    				i_num_rps_curr_temp_list1 --;
	    				while ( i_num_rps_curr_temp_list1 >>= 1)
	    				{
	    					i_length ++;
	    				}
	    				for( loop = 0; loop < h->i_ref[REF_PIC_LIST_1] ; ++ loop )
	    				{
	    					bs_write ( s, i_length, ref_pic_list_modification->ref_pic_set_idx_l1[loop] ) ;
	    				}
	    			}
	    		}
	    	}
	    }

	    if ( B_SLICE == slice->i_slice_type )
	    {
	    	bs_write1 ( s, slice->b_l_mvd_l1_zero );
	    }

	    if ( ! (I_SLICE == slice->i_slice_type) )
	    {
	    	if ( ! (I_SLICE == slice->i_slice_type)
	    			&& h->pps[0].b_cabac_init_present_flag )
	    	{
	    		i_slice_type   = slice->i_slice_type;
	    		i_enc_cabac_table_idx = h->pps[0].i_enc_cabac_table_idx ;
	    		b_enc_cabac_init_flag = (i_slice_type != i_enc_cabac_table_idx && i_enc_cabac_table_idx!=I_SLICE) ? 1 : 0 ;
	    		slice->b_cabac_init_flag = b_enc_cabac_init_flag ;
	    		bs_write1 ( s, b_enc_cabac_init_flag ? 1 : 0 ) ;
	    	}
	    }

	    if ( slice->b_enable_tmvp_flag )
	    {
	    	if ( slice->i_slice_type == B_SLICE )
	    	{
	    		bs_write1 ( s, slice->i_col_from_l0_flag );
	    	}

	    	if ( slice->i_slice_type != I_SLICE &&
	    		((slice->i_col_from_l0_flag == 1
	    		&& h->i_ref[REF_PIC_LIST_0] > 1)
	    		|| (slice->i_col_from_l0_flag == 0
	    		&& h->i_ref[REF_PIC_LIST_1] > 1)))
	    	{
	    		bs_write_ue ( s,  slice->i_col_ref_idx );
	    	}
	    }
	    if ( (h->pps[0].b_use_weight_pred && slice->i_slice_type==P_SLICE)
	    		|| (h->pps[0].b_use_weighted_bi_pred && slice->i_slice_type==B_SLICE) )
	    {
	    	x265_x_pred_weight_table_write ( h, s, slice ) ;
	    }
	    assert(slice->i_max_num_merge_cand<=X265_MRG_MAX_NUM_CANDS);
	    if ( ! (I_SLICE == slice->i_slice_type ) )
	    {
	    	bs_write_ue ( s, X265_MRG_MAX_NUM_CANDS - slice->i_max_num_merge_cand ) ;
	    }
	    i_code = slice->i_slice_qp - ( h->pps[0].i_pic_init_qp_minus26 + 26 );
	    bs_write_se ( s, i_code );
	    if (h->pps[0].b_slice_chroma_qp_flag)
	    {
	    	i_code = slice->i_slice_qp_delta_cb;
		    bs_write_se ( s, i_code );
	    	i_code = slice->i_slice_qp_delta_cr ;
		    bs_write_se ( s, i_code );
	    }
	    if (h->pps[0].b_deblocking_filter_control_present_flag)
	    {
	    	if (h->pps[0].b_deblocking_filter_override_enabled_flag )
	    	{
	    		bs_write1 ( s,slice->b_deblocking_filter_override_flag );
	    	}
	    	if ( slice->b_deblocking_filter_override_flag )
	    	{
	    		bs_write1 ( s, slice->b_deblocking_filter_disable );
	    		if ( ! slice->b_deblocking_filter_disable )
	    		{
	    		    bs_write_se ( s, slice->i_deblocking_filter_beta_offset_div2 );
	    		    bs_write_se ( s, slice->i_deblocking_filter_tc_offset_div2 );
	    		}
	    	}
	    }

	    b_is_sao_enabled = (!h->sps[0].b_use_sao) ? 0
	    					: (slice->b_sao_enabled_flag || slice->b_sao_enabled_flag_chroma );
	    b_is_dbf_enabled = ( !slice->b_deblocking_filter_disable );

	    if(h->pps[0].b_loop_filter_across_slices_enabled_flag && ( b_is_sao_enabled || b_is_dbf_enabled ))
	    {
	    	bs_write1 ( s,slice->b_lf_cross_slice_boundary_flag ? 1 : 0 );
	    }
	}
	if ( h->pps[0].b_slice_header_extension_present_flag )
	{
		bs_write_ue ( s, 0 );
	}




}

x265_t *x265_encoder_open( x265_param_t *param )
{
    x265_t *h;
	int loop = 0 ;
    int init_nal_count = 0 ;
    int i_reference_and_current = 0 ;
    int i_buffered = 0 ;
    int i_unused = 0 ;
    char buf[1000], *p;

    CHECKED_MALLOCZERO( h, sizeof(x265_t) );


    if ( x265_sample_adaptive_offset_class_static_init () )
    {
    	goto fail ;
    }
    if ( x265_pattern_class_static_init () )
    {
    	goto fail ;
    }
#if X265_ADAPTIVE_QP_SELECTION
    if ( x265_data_cu_class_static_init () )
    {
    	goto fail ;
    }
#endif


    /* Create a copy of param */
    memcpy( &h->param, param, sizeof(x265_param_t) );

    init_ref_pic_list_modification ( &h->ref_pic_list_modification ) ;
    if ( x265_pic_sym_init ( h, param ) )
    {
    	goto fail ;
    }
    if ( x265_global_init (h) )
    {
    	goto fail ;
    }
    if ( x265_enc_top_init ( h, param ) )
    {
    	goto fail ;
    }
    x265_vps_init( h->vps, &h->param );
    if ( x265_sps_init( h->sps, h, &h->param ) )
    {
    	goto fail ;
    }
    memcpy ( &h->vps[0].ptl, &h->sps[0].ptl, sizeof (x265_ptl_t) ) ;
#if X265_L0043_TIMING_INFO
    h->vps[0].timing_info.b_timing_info_present_flag = 0 ;
#endif
    if ( x265_pps_init( h->pps, &h->sps[0], h, &h->param ) )
    {
    	goto fail ;
    }

    h->param.i_slice_count = 1 ;
    h->out.i_bitstream = BITSTREAM_MAX_SIZE ;
    init_nal_count = h->param.i_slice_count ;
    CHECKED_MALLOC( h->out.p_bitstream, h->out.i_bitstream );
    /* Start each thread with room for init_nal_count NAL units; it'll realloc later if needed. */
    CHECKED_MALLOC( h->out.nal, init_nal_count*sizeof(x265_nal_t) );
    h->out.i_nals_allocated = init_nal_count ;

    h->nal_buffer_size = h->out.i_bitstream * 3/2 + 4;
    CHECKED_MALLOC( h->nal_buffer, h->nal_buffer_size );


    i_reference_and_current = h->param.gop.i_gop_size
    							+ h->param.gop.max_dec_pic_buffering[X265_MAX_TLAYER]
    							+ 2 ;
    i_buffered = h->param.gop.i_gop_size * 2 + 2 ;
    i_unused = i_reference_and_current + i_buffered ;
    h->frames.i_reference_and_current = i_reference_and_current ;
    CHECKED_MALLOCZERO( h->frames.unused[0],
    					(i_unused + 1) * sizeof(x265_frame_t *) );
    CHECKED_MALLOCZERO( h->frames.unused[1],
    					(i_unused + 1) * sizeof(x265_frame_t *) );
    CHECKED_MALLOCZERO( h->frames.reference,
    					(i_reference_and_current + 1) * sizeof(x265_frame_t *) );
    CHECKED_MALLOCZERO( h->frames.current,
    					(i_reference_and_current + 1) * sizeof(x265_frame_t *) );
    CHECKED_MALLOCZERO( h->frames.buffered,
    					(i_buffered + 1) * sizeof(x265_frame_t *) );
    CHECKED_MALLOCZERO( h->fref[0],
    					(i_reference_and_current + 1) * sizeof(x265_frame_t *) );
    CHECKED_MALLOCZERO( h->fref[1],
    					(i_reference_and_current + 1) * sizeof(x265_frame_t *) );

    if ( x265_scan_init ( &h->scan, h ) )
    {
    	goto fail ;
    }
    x265_mc_init( h->param.cpu, &h->mc ) ;
    x265_bitstream_init( h->param.cpu, &h->bsf ) ;




    p = buf + sprintf( buf, "using cpu capabilities:" );
    for( loop = 0; x265_cpu_names[loop].flags; loop++ )
    {
        if( !strcmp(x265_cpu_names[loop].name, "SSE2")
            && (h->param.cpu & (X265_CPU_SSE2_IS_FAST|X265_CPU_SSE2_IS_SLOW)) )
        {
            continue;
        }
        if( !strcmp(x265_cpu_names[loop].name, "SSE3")
            && ((h->param.cpu & X265_CPU_SSSE3) || !(h->param.cpu & X265_CPU_CACHELINE_64)) )
        {
            continue;
        }
        if( !strcmp(x265_cpu_names[loop].name, "SSE4.1")
            && (h->param.cpu & X265_CPU_SSE42) )
        {
            continue;
        }
        if( (h->param.cpu & x265_cpu_names[loop].flags) == x265_cpu_names[loop].flags
            && (!loop || x265_cpu_names[loop].flags != x265_cpu_names[loop-1].flags) )
        {
            p += sprintf( p, " %s", x265_cpu_names[loop].name );
        }
    }
    if( !h->param.cpu )
        p += sprintf( p, " none!" );
    x265_log( h, X265_LOG_INFO, "%s\n", buf );



    return h;
fail:
	x265_encoder_close ( h );
    return NULL;
}


/* internal usage */
void x265_nal_start( x265_t *h, int i_type)
{
    x265_nal_t *nal = &h->out.nal[h->out.i_nal];

    nal->i_type = i_type;
    nal->i_temporal_id = 0 ;
    nal->i_reserved_zero6_bits = 0 ;
    nal->b_long_start_code = 1;

    nal->i_payload = 0;
    nal->p_payload = &h->out.p_bitstream[bs_pos( &h->out.bs ) / 8];
}

/* if number of allocated nals is not enough, re-allocate a larger one. */
int x265_nal_check_buffer( x265_t *h )
{
    if( h->out.i_nal >= h->out.i_nals_allocated )
    {
        x265_nal_t *new_out = x265_malloc( sizeof(x265_nal_t) * (h->out.i_nals_allocated*2) );
        if( !new_out )
        {
            return -1;
        }
        memcpy( new_out, h->out.nal, sizeof(x265_nal_t) * (h->out.i_nals_allocated) );
        x265_free( h->out.nal );
        h->out.nal = new_out;
        h->out.i_nals_allocated *= 2;
    }
    return 0;
}

int x265_nal_end( x265_t *h )
{
    x265_nal_t *nal = &h->out.nal[h->out.i_nal];
    uint8_t *end = &h->out.p_bitstream[bs_pos( &h->out.bs ) / 8];
    nal->i_payload = end - nal->p_payload;
    /* nal_escape_mmx reads past the end of the input.
     * While undefined padding wouldn't actually affect the output, it makes valgrind unhappy. */
    memset( end, 0xff, 32 );
    if( h->param.nalu_process )
    {
        h->param.nalu_process( h, nal );
    }
    h->out.i_nal++;

    return x265_nal_check_buffer( h );
}

int x265_encoder_encapsulate_nals( x265_t *h, int start )
{
    int nal_size = 0, previous_nal_size = 0;

    if( h->param.nalu_process )
    {
        for( int i = start; i < h->out.i_nal; ++ i )
        {
            nal_size += h->out.nal[i].i_payload;
        }
        return nal_size;
    }

    for( int i = 0; i < start; ++ i )
    {
        previous_nal_size += h->out.nal[i].i_payload;
    }

    for( int i = start; i < h->out.i_nal; ++ i )
    {
        nal_size += h->out.nal[i].i_payload;
    }

    /* Worst-case NAL unit escaping: reallocate the buffer if it's too small. */
    int necessary_size = nal_size * 3/2 + h->out.i_nal * 4;
    if( h->nal_buffer_size < necessary_size )
    {
        h->nal_buffer_size = necessary_size * 2;
        uint8_t *buf = x265_malloc( h->nal_buffer_size );
        if( !buf )
        {
            return -1;
        }
        if( previous_nal_size )
        {
            memcpy( buf, h->nal_buffer, previous_nal_size );
        }
        x265_free( h->nal_buffer );
        h->nal_buffer = buf;
    }

    uint8_t *nal_buffer = h->nal_buffer + previous_nal_size;

    for( int i = start; i < h->out.i_nal; ++ i )
    {
        h->out.nal[i].b_long_start_code = !i || h->out.nal[i].i_type == NAL_SPS
        									|| h->out.nal[i].i_type == NAL_PPS ;
        x265_nal_encode( h, nal_buffer, &h->out.nal[i] );
        nal_buffer += h->out.nal[i].i_payload;
    }

    return nal_buffer - (h->nal_buffer + previous_nal_size);
}

/****************************************************************************
 * x265_encoder_headers:
 ****************************************************************************/
int x265_encoder_headers( x265_t *h, x265_nal_t **pp_nal, int *pi_nal )
{
    int frame_size = 0;
    /* init bitstream context */
    h->out.i_nal = 0;
    bs_init( &h->out.bs, h->out.p_bitstream, h->out.i_bitstream );

    /* Write VPS, SPS and PPS. */

    // generate video parameters
    x265_nal_start( h, NAL_VPS );
    x265_vps_write( &h->out.bs, h->vps );
    if( x265_nal_end( h ) )
    {
        return -1;
    }

    //	generate sequence parameters
    x265_nal_start( h, NAL_SPS );
    x265_sps_write( h, &h->out.bs, h->sps );
    if( x265_nal_end( h ) )
    {
        return -1;
    }

    //	generate picture parameters
    x265_nal_start( h, NAL_PPS );
    x265_pps_write( h, &h->out.bs, h->pps );
    if( x265_nal_end( h ) )
    {
        return -1;
    }

    frame_size = x265_encoder_encapsulate_nals( h, 0 );
    if( frame_size < 0 )
    {
        return -1;
    }

    /* now set output*/
    *pi_nal = h->out.i_nal;
    *pp_nal = &h->out.nal[0];
    h->out.i_nal = 0;

    return frame_size;
}



/****************************************************************************
 * x265_encoder_close:
 ****************************************************************************/
void x265_encoder_close  ( x265_t *h )
{
	if ( h )
	{
		x265_scan_deinit ( &h->scan ) ;

		x265_free( h->fref[1] );
		x265_free( h->fref[0] );
		x265_frame_delete_list( h, h->frames.buffered );
		x265_frame_delete_list( h, h->frames.current );
		x265_frame_delete_list( h, h->frames.reference );
		x265_frame_delete_list( h, h->frames.unused[1] );
		x265_frame_delete_list( h, h->frames.unused[0] );


		x265_free( h->nal_buffer );

		x265_free ( h->out.nal ) ;
		x265_free ( h->out.p_bitstream ) ;

		x265_pps_deinit( h->pps );
		x265_sps_deinit( h->sps );
		x265_vps_deinit( h->vps );

		x265_enc_top_deinit ( h ) ;
		x265_global_deinit ( h ) ;
		x265_pic_sym_deinit ( h ) ;

#if X265_ADAPTIVE_QP_SELECTION
		x265_data_cu_class_static_deinit () ;
#endif
		x265_pattern_class_static_deinit () ;
		x265_sample_adaptive_offset_class_static_deinit () ;

		x265_free ( h ) ;
	}
}

int x265_encoder_encode( x265_t *h,
						 x265_nal_t **pp_nal, int *pi_nal,
						 x265_picture_t *pic_in,
						 x265_picture_t *pic_out )
{
	int32_t i_gop_id = 0 ;
	int32_t loop = 0 ;
	int32_t i_num_frame_rcvd = 0 ;
	int32_t i_poc = 0 ;
	int32_t i_gop_encoded = 0 ;
	int32_t i_buffer_frame_list_size = 0 ;
	int32_t frame_size = 0 ;
	x265_frame_t *frame = NULL ;
	x265_frame_t *temp_frame = NULL ;

	if ( pic_in )
	{
		frame = x265_frame_pop_unused( h, 0 );
		if( !frame )
		{
			return -1;
		}
		if( x265_frame_copy_picture( h, frame, pic_in ) < 0 )
		{
			return -1;
		}
		x265_frame_buffering ( h, frame ) ;
		if ( h->frames.i_frame_buf_num < h->enc_top.i_gop_size + h->param.gop.i_gop_size + 2 )
		{
			return 0;
		}
	}

    h->out.i_nal = 0;
    bs_init( &h->out.bs, h->out.p_bitstream, h->out.i_bitstream );
    if ( 0 == h->enc_top.i_gop_id )
    {
    	i_buffer_frame_list_size = x265_frame_list_size (h->frames.buffered) ;
    	if ( h->enc_top.i_gop_size > i_buffer_frame_list_size )
    	{
    		i_num_frame_rcvd = i_buffer_frame_list_size ;
    	}
    	else
    	{
    		i_num_frame_rcvd = h->enc_top.i_gop_size ;
    	}
    	for ( loop = 0 ; loop < i_num_frame_rcvd ; ++ loop )
    	{
    		h->enc_top.i_poc_last ++ ;
    		h->enc_top.i_num_frame_rcvd ++ ;
    		frame = x265_frame_buffering_pop ( h ) ;
    		frame->i_poc = h->enc_top.i_poc_last ;
    		h->frames.current[loop] = frame ;
    	}
    	h->enc_top.i_frames_to_be_encoded = h->enc_top.i_poc_last + 1 ;

    	//	x265_frame_t *x265_frame_get_from_poc ( x265_frame_t **list, int32_t i_poc )
    	i_gop_encoded = 0 ;
    	for ( i_gop_id = 0 ; i_gop_id < h->enc_top.i_gop_size ; ++ i_gop_id )
    	{
    	    i_poc = h->enc_top.i_poc_last - h->enc_top.i_num_frame_rcvd
    	    		+ h->param.gop.gop_list[i_gop_id].i_poc ;
    	    if( 0 == h->enc_top.i_poc_last )
    	    {
    	    	i_poc = 0 ;
    	    }
    	    if ( i_poc < h->enc_top.i_frames_to_be_encoded )
    	    {
    	    	frame = x265_frame_get_from_poc ( h->frames.current, i_poc ) ;
        	    i_poc = h->enc_top.i_poc_last - h->enc_top.i_num_frame_rcvd
        	    		+ i_gop_encoded + 1 ;
    	    	temp_frame = x265_frame_get_from_poc ( h->frames.current, i_poc ) ;
    	    	frame->i_dts = temp_frame->i_pts ;
        	    i_gop_encoded ++ ;
    	    }
    	}
    }

    i_poc = h->enc_top.i_poc_last - h->enc_top.i_num_frame_rcvd
    		+ h->param.gop.gop_list[h->enc_top.i_gop_id].i_poc ;
    if( 0 == h->enc_top.i_poc_last )
    {
    	i_poc = 0 ;
    }
    while ( i_poc >= h->enc_top.i_frames_to_be_encoded )
    {
    	h->enc_top.i_gop_id ++ ;
        i_poc = h->enc_top.i_poc_last - h->enc_top.i_num_frame_rcvd
        		+ h->param.gop.gop_list[h->enc_top.i_gop_id].i_poc ;
        if( 0 == h->enc_top.i_poc_last )
        {
        	i_poc = 0 ;
        }
    }

    h->fenc = x265_frame_pop_from_poc ( h->frames.current, i_poc ) ;
    h->fdec = x265_frame_pop_unused( h, 1 );
    h->fdec->i_dts = h->fenc->i_dts ;
    h->fdec->i_pts = h->fenc->i_pts ;
    h->fdec->i_poc = h->fenc->i_poc ;
    h->fdec->fdec_data->b_is_border_extended = 0 ;

    frame_size = x265_enc_gop_encoder_slice( h, &h->enc_gop, pp_nal, pi_nal,
    										pic_in, pic_out ) ;
    if ( frame_size < 0 )
    {
    	goto fail ;
    }

    x265_reference_update ( h, h->fdec ) ;

    h->enc_top.i_gop_id ++ ;
    h->enc_top.i_gop_encoded ++ ;
    h->enc_top.i_num_all_pic_coded ++ ;
	if ( h->enc_top.i_gop_id == h->enc_top.i_gop_size )
	{
	    h->enc_top.i_gop_id = 0 ;
	    h->enc_top.i_gop_encoded = 0 ;
	    h->enc_top.i_num_frame_rcvd = 0 ;
		h->enc_top.i_gop_size = h->param.gop.i_gop_size ;
	    x265_reference_remove ( h ) ;
	}
	x265_frame_push_unused( h, h->fenc ) ;

    h->frames.i_frame_buf_num -- ;

	return frame_size ;

fail:
	return -1 ;
}

int32_t x265_encoder_frame_end( x265_t *h,
							x265_nal_t **pp_nal,
							int *pi_nal,
							x265_picture_t *pic_out )
{
	int32_t frame_size = 0 ;

    bs_flush ( &h->out.bs ) ;
    if( x265_nal_end( h ) )
    {
        goto fail ;
    }
    frame_size = x265_encoder_encapsulate_nals( h, 0 );

    *pi_nal = h->out.i_nal;
    *pp_nal = &h->out.nal[0];
    h->out.i_nal = 0;

	pic_out->img.i_width = h->param.i_width ;
	pic_out->img.i_height = h->param.i_height ;
    pic_out->i_pts = h->fdec->i_pts;
    pic_out->i_dts = h->fdec->i_dts;
    for( int i = 0; i < h->fdec->i_plane; i++ )
    {
        pic_out->img.i_stride[i] = h->fdec->i_stride[i];
        pic_out->img.plane[i] = h->fdec->plane[i];
    }
    if ( frame_size < 0 )
    {
    	goto fail ;
    }

    return frame_size;

fail:
	return -1 ;
}

void x265_encoder_print_summary(x265_t *h)
{
	x265_enc_top_print_summary(h) ;
}

int x265_encoder_delayed_frames( x265_t *h )
{
	if ( h->frames.i_frame_buf_num > 0 )
	{
	    return 1 ;
	}
	return 0 ;
}

void x265_reference_update ( x265_t *h, x265_frame_t *frame )
{
	int i_index = 0 ;

	x265_frame_expand_border( h, frame ) ;
	while ( h->frames.reference[i_index] )
	{
		++ i_index ;
	}
	h->frames.reference[i_index] = frame ;
}

void x265_reference_remove ( x265_t *h )
{
	int i_index = 0 ;
	x265_frame_t *frame = NULL ;

	while ( h->frames.reference[i_index] )
	{
		if ( h->frames.reference[i_index]->fdec_data->b_reference )
		{
			++ i_index ;
		}
		else
		{
			frame = x265_frame_pop_from_position ( h->frames.reference, i_index ) ;
			x265_frame_push_unused( h, frame ) ;
		}
	}

}

