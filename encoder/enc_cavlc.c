



#include "common/common.h"




void x265_enc_cavlc_determine_cabac_init_idx ( x265_t *h,
											x265_enc_entropy_if_t *enc_entropy_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp )
{

}

void x265_enc_cavlc_set_bitstream ( x265_enc_entropy_if_t *enc_entropy_if, x265_bit_if_t* bit_if )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	enc_cavlc->bit_if = bit_if ;
}

void x265_enc_cavlc_set_slice ( x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t* slice )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	enc_cavlc->slice = slice ;
}

void x265_enc_cavlc_reset_bits ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	enc_cavlc->bit_if->reset_bits (enc_cavlc->bit_if) ;
}

void x265_enc_cavlc_clear_bits ( x265_enc_entropy_if_t *enc_entropy_if )
{

}

void x265_enc_cavlc_reset_coeff_cost ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	enc_cavlc->i_coeff_cost = 0 ;
}

uint32_t x265_enc_cavlc_get_number_of_written_bits ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	return enc_cavlc->bit_if->get_number_of_written_bits (enc_cavlc->bit_if) ;
}

uint32_t x265_enc_cavlc_get_coeff_cost ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	return enc_cavlc->i_coeff_cost ;
}

void x265_enc_cavlc_code_sao_sign( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code )
{
	printf ( "Not supported\n" ) ;
	assert (0);
}

void x265_enc_cavlc_update_context_tables_p4 ( x265_enc_entropy_if_t *enc_entropy_if,
												enum slice_type_e i_slice_type,
												int32_t i_qp,
												int32_t b_execute_finish )
{

}

void x265_enc_cavlc_update_context_tables_p3 ( x265_enc_entropy_if_t *enc_entropy_if,
												enum slice_type_e i_slice_type,
												int32_t i_qp )
{

}

void x265_enc_cavlc_code_sao_max_uvlc( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code,
									uint32_t max_symbol )
{
	printf ( "Not supported\n" ) ;
	assert (0);
}

void x265_enc_cavlc_code_sao_merge ( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code )
{
	printf ( "Not supported\n" ) ;
	assert (0);
}

void x265_enc_cavlc_code_sao_type_idx ( x265_enc_entropy_if_t* enc_entropy_if,
										uint32_t code)
{
	printf ( "Not supported\n" ) ;
	assert (0);
}

void x265_enc_cavlc_code_sao_uflc ( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t length,
									uint32_t code )
{
	printf ( "Not supported\n" ) ;
	assert (0);
}

x265_enc_entropy_if_t *x265_enc_cavlc_new ()
{
	x265_enc_entropy_if_t *enc_entropy_if = NULL ;

	CHECKED_MALLOCZERO( enc_entropy_if, sizeof(x265_enc_cavlc_t) );
	if ( x265_enc_cavlc_init(enc_entropy_if) )
	{
		goto fail ;
	}

	return enc_entropy_if ;
fail:
	x265_enc_cavlc_delete ( enc_entropy_if ) ;
	return NULL ;
}

void x265_enc_cavlc_delete ( x265_enc_entropy_if_t *enc_entropy_if )
{
	if ( enc_entropy_if )
	{
		x265_enc_cavlc_deinit ( enc_entropy_if ) ;
		x265_free ( enc_entropy_if ) ;
	}
}

int x265_enc_cavlc_init ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_entropy_if_init ( enc_entropy_if ) ;

	enc_entropy_if->enc_entropy_if_delete = x265_enc_cavlc_delete ;
	enc_entropy_if->reset_entropy = x265_enc_cavlc_reset_entropy ;
	enc_entropy_if->print_cabac_state = x265_enc_cavlc_print_cabac_state ;
	enc_entropy_if->determine_cabac_init_idx = x265_enc_cavlc_determine_cabac_init_idx ;
	enc_entropy_if->set_bitstream = x265_enc_cavlc_set_bitstream ;
	enc_entropy_if->set_slice = x265_enc_cavlc_set_slice ;
	enc_entropy_if->reset_bits = x265_enc_cavlc_reset_bits ;
	enc_entropy_if->clear_bits = x265_enc_cavlc_clear_bits ;
	enc_entropy_if->reset_coeff_cost = x265_enc_cavlc_reset_coeff_cost ;
	enc_entropy_if->get_number_of_written_bits = x265_enc_cavlc_get_number_of_written_bits ;
	enc_entropy_if->get_coeff_cost = x265_enc_cavlc_get_coeff_cost ;
	enc_entropy_if->code_vps = x265_enc_cavlc_code_vps ;
	enc_entropy_if->code_sps = x265_enc_cavlc_code_sps ;
	enc_entropy_if->code_pps = x265_enc_cavlc_code_pps ;
	enc_entropy_if->code_slice_header = x265_enc_cavlc_code_slice_header ;
	enc_entropy_if->code_tiles_wpp_entry_point = x265_enc_cavlc_code_tiles_wpp_entry_point ;
	enc_entropy_if->code_terminating_bit = x265_enc_cavlc_code_terminating_bit ;
	enc_entropy_if->code_slice_finish = x265_enc_cavlc_code_slice_finish ;
	enc_entropy_if->code_mvp_idx = x265_enc_cavlc_code_mvp_idx ;
	enc_entropy_if->code_scaling_list = x265_enc_cavlc_code_scaling_list ;
	enc_entropy_if->code_cu_transquant_bypass_flag = x265_enc_cavlc_code_cu_transquant_bypass_flag ;
	enc_entropy_if->code_skip_flag = x265_enc_cavlc_code_skip_flag ;
	enc_entropy_if->code_merge_flag = x265_enc_cavlc_code_merge_flag ;
	enc_entropy_if->code_merge_index = x265_enc_cavlc_code_merge_index ;
	enc_entropy_if->code_split_flag = x265_enc_cavlc_code_split_flag ;
	enc_entropy_if->code_part_size = x265_enc_cavlc_code_part_size ;
	enc_entropy_if->code_pred_mode = x265_enc_cavlc_code_pred_mode ;
	enc_entropy_if->code_ipcm_info = x265_enc_cavlc_code_ipcm_info ;
	enc_entropy_if->code_transform_sub_div_flag = x265_enc_cavlc_code_transform_sub_div_flag ;
	enc_entropy_if->code_qt_cbf = x265_enc_cavlc_code_qt_cbf ;
	enc_entropy_if->code_qt_root_cbf = x265_enc_cavlc_code_qt_root_cbf ;
	enc_entropy_if->code_qt_cbf_zero = x265_enc_cavlc_code_qt_cbf_zero ;
	enc_entropy_if->code_qt_root_cbf_zero = x265_enc_cavlc_code_qt_root_cbf_zero ;
	enc_entropy_if->code_intra_dir_luma_ang = x265_enc_cavlc_code_intra_dir_luma_ang ;
	enc_entropy_if->code_intra_dir_chroma = x265_enc_cavlc_code_intra_dir_chroma ;
	enc_entropy_if->code_inter_dir = x265_enc_cavlc_code_inter_dir ;
	enc_entropy_if->code_ref_frm_idx = x265_enc_cavlc_code_ref_frm_idx ;
	enc_entropy_if->code_mvd = x265_enc_cavlc_code_mvd ;
	enc_entropy_if->code_delta_qp = x265_enc_cavlc_code_delta_qp ;
	enc_entropy_if->code_coeff_nxn = x265_enc_cavlc_code_coeff_nxn ;
	enc_entropy_if->code_transform_skip_flags = x265_enc_cavlc_code_transform_skip_flags ;
	enc_entropy_if->code_sao_sign = x265_enc_cavlc_code_sao_sign ;
	enc_entropy_if->code_sao_max_uvlc = x265_enc_cavlc_code_sao_max_uvlc ;
	enc_entropy_if->code_sao_merge = x265_enc_cavlc_code_sao_merge ;
	enc_entropy_if->code_sao_type_idx = x265_enc_cavlc_code_sao_type_idx ;
	enc_entropy_if->code_sao_uflc = x265_enc_cavlc_code_sao_uflc ;
	enc_entropy_if->est_bit = x265_enc_cavlc_est_bit ;
	enc_entropy_if->update_context_tables_p4 = x265_enc_cavlc_update_context_tables_p4 ;
	enc_entropy_if->update_context_tables_p3 = x265_enc_cavlc_update_context_tables_p3 ;
	enc_entropy_if->code_df_flag = x265_enc_cavlc_code_df_flag ;
	enc_entropy_if->code_df_svlc = x265_enc_cavlc_code_df_svlc ;

	return 0 ;
}

void x265_enc_cavlc_deinit ( x265_enc_entropy_if_t *enc_entropy_if )
{

}


void x265_enc_cavlc_reset_entropy ( x265_t *h,
									x265_enc_entropy_if_t *enc_entropy_if,
									enum slice_type_e i_slice_type,
									int32_t i_slice_qp )
{

}

void x265_enc_cavlc_print_cabac_state ( x265_enc_entropy_if_t *enc_entropy_if )
{

}

void x265_enc_cavlc_code_df_flag ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_code )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	x265_enc_cavlc_write1 ( enc_cavlc, i_code ) ;
}

void x265_enc_cavlc_code_df_svlc ( x265_enc_entropy_if_t *enc_entropy_if, int32_t i_code )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	x265_enc_cavlc_write_se ( enc_cavlc, i_code ) ;
}



void x265_enc_cavlc_code_short_term_ref_pic_set ( x265_enc_entropy_if_t *enc_entropy_if,
												x265_reference_picture_set_t* rps,
												int32_t b_called_from_slice_header,
												int32_t i_idx)
{
	int32_t loop = 0 ;
	int32_t i_delta_rps = 0 ;
	int32_t i_ref_idc = 0 ;
	int32_t i_prev = 0 ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	if (i_idx > 0)
	{
		// inter_RPS_prediction_flag
		x265_enc_cavlc_write1 ( enc_cavlc, rps->b_inter_rps_prediction ) ;
	}
	if (rps->b_inter_rps_prediction)
	{
		i_delta_rps = rps->i_delta_rps;
		if (b_called_from_slice_header)
		{
			// delta index of the Reference Picture Set used for prediction minus 1
			x265_enc_cavlc_write_ue ( enc_cavlc, rps->i_delta_ridx_minus1 ) ;
		}

		//delta_rps_sign
		x265_enc_cavlc_write ( enc_cavlc, 1, (i_delta_rps >=0 ? 0: 1) ) ;
		// absolute delta RPS minus 1
		x265_enc_cavlc_write_ue ( enc_cavlc, abs(i_delta_rps) - 1 ) ;

		for( loop = 0 ; loop < rps->i_num_ref_idc ; ++ loop )
		{
			i_ref_idc = rps->ref_idc[loop] ;
			//first bit is "1" if Idc is 1
			x265_enc_cavlc_write ( enc_cavlc, 1, (i_ref_idc==1? 1: 0) ) ;
			if ( i_ref_idc != 1 )
			{
				//second bit is "1" if Idc is 2, "0" otherwise.
				x265_enc_cavlc_write ( enc_cavlc, 1, i_ref_idc>>1  ) ;
			}
		}
	}
	else
	{
		x265_enc_cavlc_write_ue ( enc_cavlc, rps->i_number_of_negative_pictures ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, rps->i_number_of_positive_pictures ) ;
		i_prev = 0;
		for( loop = 0 ; loop < rps->i_number_of_negative_pictures ; ++ loop )
		{
			x265_enc_cavlc_write_ue ( enc_cavlc, i_prev - rps->delta_poc[loop] - 1 ) ;
			i_prev = rps->delta_poc[loop] ;
			x265_enc_cavlc_write1 ( enc_cavlc, rps->used[loop] ) ;
		}
		i_prev = 0;
		for( loop = rps->i_number_of_negative_pictures ;
				loop < rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures ;
				++ loop )
		{
			x265_enc_cavlc_write_ue ( enc_cavlc, rps->delta_poc[loop] - i_prev - 1 ) ;
			i_prev = rps->delta_poc[loop] ;
			x265_enc_cavlc_write1 ( enc_cavlc, rps->used[loop] ) ;
		}
	}
}

void x265_enc_cavlc_code_pps ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_pps_t *pps )
{
	int32_t loop = 0 ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	x265_enc_cavlc_write_ue ( enc_cavlc, pps->i_pps_id ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, pps->i_sps_id ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_dependent_slice_segments_enabled_flag ? 1 : 0 ) ;
#if X265_L0255_MOVE_PPS_FLAGS
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_output_flag_present_flag ? 1 : 0 ) ;
	x265_enc_cavlc_write ( enc_cavlc, 3, pps->i_num_extra_slice_header_bits ) ;
#endif
	x265_enc_cavlc_write1 ( enc_cavlc, pps->i_sign_hide_flag ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_cabac_init_present_flag ? 1 : 0 ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, pps->i_num_ref_idx_l0_default_active - 1 ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, pps->i_num_ref_idx_l1_default_active - 1 ) ;

	x265_enc_cavlc_write_se ( enc_cavlc, pps->i_pic_init_qp_minus26 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_constrained_intra_pred ? 1 : 0 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_use_transform_skip ? 1 : 0 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_use_dqp ? 1 : 0 ) ;
	if ( pps->b_use_dqp )
	{
		x265_enc_cavlc_write_ue ( enc_cavlc, pps->i_max_cu_dqp_depth ) ;
	}
	x265_enc_cavlc_write_se ( enc_cavlc, pps->i_chroma_cb_qp_offset ) ;
	x265_enc_cavlc_write_se ( enc_cavlc, pps->i_chroma_cr_qp_offset ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_slice_chroma_qp_flag ) ;

	// Use of Weighting Prediction (P_SLICE)
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_use_weight_pred ? 1 : 0 ) ;
	// Use of Weighting Bi-Prediction (B_SLICE)
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_use_weighted_bi_pred ? 1 : 0 ) ;
#if !X265_L0255_MOVE_PPS_FLAGS
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_output_flag_present_flag ? 1 : 0 ) ;
#endif
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_transquant_bypass_enable_flag ? 1 : 0 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_tiles_enabled_flag ? 1 : 0 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_entropy_coding_sync_enabled_flag ? 1 : 0 ) ;

	if( pps->b_tiles_enabled_flag )
	{
		x265_enc_cavlc_write_ue ( enc_cavlc, pps->i_num_columns_minus1 ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, pps->i_num_rows_minus1 ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, pps->i_uniform_spacing_flag ) ;
		if( 0 == pps->i_uniform_spacing_flag )
		{
			for( loop = 0 ; loop < pps->i_num_columns_minus1 ; ++ loop )
			{
				x265_enc_cavlc_write_ue ( enc_cavlc, pps->column_width[loop] - 1 ) ;
			}
			for( loop = 0 ; loop < pps->i_num_rows_minus1 ; ++ loop )
			{
				x265_enc_cavlc_write_ue ( enc_cavlc, pps->row_height[loop] - 1 ) ;
			}
		}
		if ( 0 != pps->i_num_columns_minus1 || 0 != pps->i_num_rows_minus1 )
		{
			x265_enc_cavlc_write1 ( enc_cavlc, pps->b_loop_filter_across_tiles_enabled_flag ? 1 : 0 ) ;
		}
	}
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_loop_filter_across_slices_enabled_flag ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_deblocking_filter_control_present_flag ) ;
	if ( pps->b_deblocking_filter_control_present_flag )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, pps->b_deblocking_filter_override_enabled_flag ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, pps->b_pic_disable_deblocking_filter_flag ) ;
		if ( ! pps->b_pic_disable_deblocking_filter_flag )
		{
			x265_enc_cavlc_write_se ( enc_cavlc, pps->i_deblocking_filter_beta_offset_div2 ) ;
			x265_enc_cavlc_write_se ( enc_cavlc, pps->i_deblocking_filter_tc_offset_div2 ) ;
		}
	}

	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_scaling_list_present_flag ) ;
	if( pps->b_scaling_list_present_flag )
	{
		x265_enc_cavlc_code_scaling_list ( h, enc_entropy_if, &h->scaling_list ) ;
	}
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_lists_modification_present_flag ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, pps->i_log2_parallel_merge_level_minus2 ) ;
#if !X265_L0255_MOVE_PPS_FLAGS
	x265_enc_cavlc_write ( enc_cavlc, 3, pps->i_num_extra_slice_header_bits ) ;
#endif
	x265_enc_cavlc_write1 ( enc_cavlc, pps->b_slice_header_extension_present_flag ? 1 : 0 ) ;

	x265_enc_cavlc_write1 ( enc_cavlc, 0 ) ;
}

void x265_enc_cavlc_code_vui ( x265_enc_entropy_if_t *enc_entropy_if, x265_vui_t *vui, x265_sps_t *sps )
{
	x265_window_t *default_display_window = NULL ;
	x265_timing_info_t *timing_info ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_aspect_ratio_info_present_flag ? 0 : 1 ) ;
	if ( vui->b_aspect_ratio_info_present_flag )
	{
		x265_enc_cavlc_write ( enc_cavlc, 8, vui->i_aspect_ratio_idc ) ;
		if ( 255 == vui->i_aspect_ratio_idc )
		{
			x265_enc_cavlc_write ( enc_cavlc, 16, vui->i_sar_width ) ;
			x265_enc_cavlc_write ( enc_cavlc, 16, vui->i_sar_height ) ;
		}
	}

	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_overscan_info_present_flag ? 0 : 1 ) ;
	if ( vui->b_overscan_info_present_flag )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, vui->b_overscan_appropriate_flag ) ;
	}
	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_video_signal_type_present_flag ? 0 : 1 ) ;
	if ( vui->b_video_signal_type_present_flag )
	{
		x265_enc_cavlc_write ( enc_cavlc, 3, vui->i_video_format ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, vui->b_video_full_range_flag ? 0 : 1 ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, vui->b_colour_description_present_flag ? 0 : 1 ) ;
		if ( vui->b_colour_description_present_flag )
		{
			x265_enc_cavlc_write ( enc_cavlc, 8, vui->i_colour_primaries ) ;
			x265_enc_cavlc_write ( enc_cavlc, 8, vui->i_transfer_characteristics ) ;
			x265_enc_cavlc_write ( enc_cavlc, 8, vui->i_matrix_coefficients ) ;
		}
	}

	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_chroma_loc_info_present_flag ? 0 : 1 ) ;
	if ( vui->b_chroma_loc_info_present_flag )
	{
		x265_enc_cavlc_write_ue ( enc_cavlc, vui->i_chroma_sample_loc_type_top_field ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, vui->i_chroma_sample_loc_type_bottom_field ) ;
	}

	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_neutral_chroma_indication_flag ? 0 : 1 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_field_seq_flag ? 0 : 1 ) ;
	// not currently supported
	assert ( 0 == vui->b_field_seq_flag ) ;

	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_frame_field_info_present_flag ? 0 : 1 ) ;

	default_display_window = &vui->default_display_window ;
	x265_enc_cavlc_write1 ( enc_cavlc, default_display_window->b_enabled_flag ) ;
	if ( default_display_window->b_enabled_flag )
	{
		x265_enc_cavlc_write_ue_big ( enc_cavlc, default_display_window->i_win_left_offset ) ;
		x265_enc_cavlc_write_ue_big ( enc_cavlc, default_display_window->i_win_right_offset ) ;
		x265_enc_cavlc_write_ue_big ( enc_cavlc, default_display_window->i_win_top_offset ) ;
		x265_enc_cavlc_write_ue_big ( enc_cavlc, default_display_window->i_win_bottom_offset ) ;
	}

#if X265_L0043_TIMING_INFO
	timing_info = &vui->timing_info ;
	x265_enc_cavlc_write1 ( enc_cavlc, timing_info->b_timing_info_present_flag ) ;
	if ( timing_info->b_timing_info_present_flag )
	{
		x265_enc_cavlc_write ( enc_cavlc, 32, timing_info->i_num_units_in_tick ) ;
		x265_enc_cavlc_write ( enc_cavlc, 32, timing_info->i_time_scale ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, timing_info->b_poc_proportional_to_timing_flag ) ;
		if ( timing_info->b_poc_proportional_to_timing_flag )
		{
			x265_enc_cavlc_write_ue_big ( enc_cavlc, timing_info->i_num_ticks_poc_diff_one_minus1 ) ;
		}
#endif
		x265_enc_cavlc_write1 ( enc_cavlc, vui->b_hrd_parameters_present_flag ) ;
		if ( vui->b_hrd_parameters_present_flag )
		{
			x265_enc_cavlc_code_hrd_parameters ( enc_entropy_if, &vui->hrd_parameters, 1, sps->i_max_t_layers - 1 ) ;
		}
#if X265_L0043_TIMING_INFO
	}
#endif
#if !X265_L0043_TIMING_INFO
	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_poc_proportional_to_timing_flag ) ;
	if ( vui->b_poc_proportional_to_timing_flag && vui->hrd_parameters.b_timing_info_present_flag )
	{
		x265_enc_cavlc_write_ue_big ( enc_cavlc, vui->i_num_ticks_poc_diff_one_minus1 ) ;
	}
#endif

	x265_enc_cavlc_write1 ( enc_cavlc, vui->b_bitstream_restriction_flag ) ;
	if ( vui->b_bitstream_restriction_flag )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, vui->b_tiles_fixed_structure_flag ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, vui->b_motion_vectors_over_pic_boundaries_flag ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, vui->b_restricted_ref_pic_lists_flag ) ;
#if X265_L0043_MSS_IDC
		x265_enc_cavlc_write_ue ( enc_cavlc, vui->i_min_spatial_segmentation_idc ) ;
#else
		x265_enc_cavlc_write ( enc_cavlc, 8, vui->i_min_spatial_segmentation_idc ) ;
#endif
		x265_enc_cavlc_write_ue_big ( enc_cavlc, vui->i_max_bytes_per_pic_denom ) ;
		x265_enc_cavlc_write_ue_big ( enc_cavlc, vui->i_max_bits_per_min_cu_denom ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, vui->i_log2_max_mv_length_horizontal ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, vui->i_log2_max_mv_length_vertical ) ;
	}
}


void x265_enc_cavlc_code_hrd_parameters ( x265_enc_entropy_if_t *enc_entropy_if,
										x265_hrd_t *hrd,
										int32_t b_common_inf_present_flag,
										uint32_t i_max_num_sub_layers_minus1 )
{
	int32_t i = 0, j = 0, i_nal_or_vcl ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	if( b_common_inf_present_flag )
	{
#if !X265_L0043_TIMING_INFO
		x265_enc_cavlc_write1 ( enc_cavlc, hrd->b_timing_info_present_flag ? 1 : 0 ) ;
		if( hrd->b_timing_info_present_flag )
		{
			x265_enc_cavlc_write ( enc_cavlc, 32, hrd->i_num_units_in_tick ) ;
			x265_enc_cavlc_write ( enc_cavlc, 32, hrd->i_time_scale ) ;
		}
#endif
		x265_enc_cavlc_write1 ( enc_cavlc, hrd->b_nal_hrd_parameters_present_flag ? 1 : 0 ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, hrd->b_vcl_hrd_parameters_present_flag ? 1 : 0  ) ;
		if( hrd->b_nal_hrd_parameters_present_flag || hrd->b_vcl_hrd_parameters_present_flag )
		{
			x265_enc_cavlc_write1 ( enc_cavlc, hrd->b_sub_pic_cpb_params_present_flag ? 1 : 0 ) ;
			if( hrd->b_sub_pic_cpb_params_present_flag )
			{
				x265_enc_cavlc_write ( enc_cavlc, 8, hrd->i_tick_divisor_minus2 ) ;
				x265_enc_cavlc_write ( enc_cavlc, 5, hrd->i_du_cpb_removal_delay_length_minus1 ) ;
				x265_enc_cavlc_write1 ( enc_cavlc, hrd->b_sub_pic_cpb_params_in_pic_timing_sei_flag ? 1 : 0 ) ;
#if X265_L0044_DU_DPB_OUTPUT_DELAY_HRD
				x265_enc_cavlc_write ( enc_cavlc, 5, hrd->i_dpb_output_delay_length_minus1 ) ;
#endif
			}
			x265_enc_cavlc_write ( enc_cavlc, 4, hrd->i_bit_rate_scale ) ;
			x265_enc_cavlc_write ( enc_cavlc, 4, hrd->i_cpb_size_scale ) ;
			if( hrd->b_sub_pic_cpb_params_present_flag )
			{
				x265_enc_cavlc_write ( enc_cavlc, 4, hrd->i_du_cpb_size_scale ) ;
			}
			x265_enc_cavlc_write ( enc_cavlc, 5, hrd->i_initial_cpb_removal_delay_length_minus1 ) ;
			x265_enc_cavlc_write ( enc_cavlc, 5, hrd->i_cpb_removal_delay_length_minus1 ) ;
			x265_enc_cavlc_write ( enc_cavlc, 5, hrd->i_dpb_output_delay_length_minus1 ) ;
		}
	}
	for( i = 0; i <= i_max_num_sub_layers_minus1; i ++ )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, hrd->hrd[i].b_fixed_pic_rate_flag ? 1 : 0 ) ;
		if( ! hrd->hrd[i].b_fixed_pic_rate_flag )
		{
			x265_enc_cavlc_write1 ( enc_cavlc, hrd->hrd[i].b_fixed_pic_rate_within_cvs_flag ? 1 : 0 ) ;
		}
		else
		{
			hrd->hrd[i].b_fixed_pic_rate_within_cvs_flag = 1 ;
		}
		if( hrd->hrd[i].b_fixed_pic_rate_within_cvs_flag )
		{
			x265_enc_cavlc_write_ue_big ( enc_cavlc, hrd->hrd[i].i_pic_duration_in_tc_minus1 ) ;
		}
#if X265_L0372
		else
		{
			x265_enc_cavlc_write1 ( enc_cavlc, hrd->hrd[i].b_low_delay_hrd_flag ) ;
		}
		if ( ! hrd->hrd[i].b_low_delay_hrd_flag )
		{
			x265_enc_cavlc_write_ue ( enc_cavlc, hrd->hrd[i].i_cpb_cnt_minus1 ) ;
		}
#else
		x265_enc_cavlc_write1 ( enc_cavlc, hrd->hrd[i].b_low_delay_hrd_flag ? 1 : 0 ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, hrd->hrd[i].i_cpb_cnt_minus1 ) ;
#endif

		for( i_nal_or_vcl = 0; i_nal_or_vcl < 2; i_nal_or_vcl ++ )
		{
			if( ( ( i_nal_or_vcl == 0 ) && ( hrd->b_nal_hrd_parameters_present_flag ) ) ||
					( ( i_nal_or_vcl == 1 ) && ( hrd->b_vcl_hrd_parameters_present_flag ) ) )
			{
				for( j = 0; j <= ( hrd->hrd[i].i_cpb_cnt_minus1 ) ; j ++ )
				{
					x265_enc_cavlc_write_ue_big ( enc_cavlc, hrd->hrd[i].bit_rate_value_minus1[j][i_nal_or_vcl] ) ;
					x265_enc_cavlc_write_ue_big ( enc_cavlc, hrd->hrd[i].cpb_size_value[j][i_nal_or_vcl] ) ;
					if( hrd->b_sub_pic_cpb_params_present_flag )
					{
#if X265_L0363_DU_BIT_RATE
						x265_enc_cavlc_write_ue_big ( enc_cavlc, hrd->hrd[i].du_bit_rate_value[j][i_nal_or_vcl] ) ;

#endif
						x265_enc_cavlc_write_ue_big ( enc_cavlc, hrd->hrd[i].du_cpb_size_value[j][i_nal_or_vcl] ) ;
					}
					x265_enc_cavlc_write1 ( enc_cavlc, hrd->hrd[i].cbr_flag[j][i_nal_or_vcl] ) ;
				}
			}
		}
	}
}

void x265_enc_cavlc_code_sps( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_sps_t *sps )
{
	int32_t loop = 0 ;
	int32_t b_sub_layer_ordering_info_present_flag = 0 ;
	x265_window_t *conformance_window = NULL ;
	uint32_t i_min_cu_size = 0 ;
	uint32_t log2_min_cu_size = 0;
	x265_rps_list_t *rps_list = NULL ;
	x265_reference_picture_set_t *rps = NULL ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	x265_enc_cavlc_write ( enc_cavlc, 4, sps->i_vps_id ) ;
	x265_enc_cavlc_write ( enc_cavlc, 3, sps->i_max_t_layers - 1 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_temporal_id_nesting_flag ? 1 : 0 ) ;
	x265_enc_cavlc_code_ptl ( enc_entropy_if, &sps->ptl, 1, sps->i_max_t_layers - 1 ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_sps_id ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_chroma_format_idc ) ;
	assert ( sps->i_chroma_format_idc == 1) ;
	// in the first version chroma_format_idc can only be equal to 1 (4:2:0)
	if( sps->i_chroma_format_idc == 3 )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, 0 ) ;
	}

	x265_enc_cavlc_write_ue_big ( enc_cavlc, sps->i_pic_width_in_luma_samples ) ;
	x265_enc_cavlc_write_ue_big ( enc_cavlc, sps->i_pic_height_in_luma_samples ) ;
	conformance_window = &sps->conformance_window ;

	x265_enc_cavlc_write1 ( enc_cavlc, conformance_window->b_enabled_flag ? 1 : 0 ) ;
	if ( conformance_window->b_enabled_flag )
	{
		x265_enc_cavlc_write_ue_big ( enc_cavlc, conformance_window->i_win_left_offset / 2 ) ;
		x265_enc_cavlc_write_ue_big ( enc_cavlc, conformance_window->i_win_right_offset / 2 ) ;
		x265_enc_cavlc_write_ue_big ( enc_cavlc, conformance_window->i_win_top_offset / 2 ) ;
		x265_enc_cavlc_write_ue_big ( enc_cavlc, conformance_window->i_win_bottom_offset / 2 ) ;
	}

	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_bit_depth_y - 8 ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_bit_depth_c - 8 ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_bits_for_poc - 4 ) ;

	b_sub_layer_ordering_info_present_flag = 1 ;
	x265_enc_cavlc_write1 ( enc_cavlc, b_sub_layer_ordering_info_present_flag ) ;
	for ( loop = 0 ; loop <= sps->i_max_t_layers - 1 ; ++ loop )
	{
		x265_enc_cavlc_write_ue ( enc_cavlc, sps->max_dec_pic_buffering[loop] ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, sps->num_reorder_pics[loop] ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, sps->max_latency_increase[loop] ) ;
		if ( !b_sub_layer_ordering_info_present_flag )
		{
			break ;
		}
	}

	assert ( sps->i_max_cu_width == sps->i_max_cu_height ) ;


	i_min_cu_size = sps->i_max_cu_width >> ( sps->i_max_cu_depth - sps->i_add_cu_depth ) ;
	log2_min_cu_size = 0;
	while(i_min_cu_size > 1)
	{
		i_min_cu_size >>= 1;
	    log2_min_cu_size++;
	}

	x265_enc_cavlc_write_ue ( enc_cavlc, log2_min_cu_size - 3 ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_max_cu_depth - sps->i_add_cu_depth ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_quadtree_tu_log2_min_size - 2 ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_quadtree_tu_log2_max_size - sps->i_quadtree_tu_log2_min_size ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_quadtree_tu_max_depth_inter - 1 ) ;
	x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_quadtree_tu_max_depth_intra - 1 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_scaling_list_enabled_flag ? 1 : 0 ) ;
	if ( sps->b_scaling_list_enabled_flag )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, sps->b_scaling_list_present_flag ? 1 : 0 ) ;
		if ( sps->b_scaling_list_present_flag )
		{
			x265_enc_cavlc_code_scaling_list ( h, enc_entropy_if, &h->scaling_list ) ;
		}
	}
	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_use_amp ? 1 : 0 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_use_sao ? 1 : 0 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_use_pcm ? 1 : 0 ) ;
	if ( sps->b_use_pcm )
	{
		x265_enc_cavlc_write ( enc_cavlc, 4, sps->i_pcm_bit_depth_luma - 1 ) ;
		x265_enc_cavlc_write ( enc_cavlc, 4, sps->i_pcm_bit_depth_chroma - 1 ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_pcm_log2_min_size - 3 ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_pcm_log2_max_size - sps->i_pcm_log2_min_size ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, sps->b_pcm_filter_disable_flag ? 1 : 0 ) ;
	}

	assert( sps->i_max_t_layers > 0 ) ;

	rps_list = &sps->rps_list ;

	x265_enc_cavlc_write_ue ( enc_cavlc,rps_list->i_number_of_reference_picture_sets ) ;
	for ( loop = 0 ; loop < rps_list->i_number_of_reference_picture_sets ; ++ loop )
	{
	    rps = &rps_list->reference_picture_sets[loop] ;
	    x265_enc_cavlc_code_short_term_ref_pic_set ( enc_entropy_if, rps, 0, loop ) ;
	}
	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_long_term_refs_present ? 1 : 0 ) ;
	if (sps->b_long_term_refs_present)
	{
		x265_enc_cavlc_write_ue ( enc_cavlc, sps->i_num_long_term_ref_pic_sps ) ;
		for ( loop = 0; loop < sps->i_num_long_term_ref_pic_sps ; ++ loop )
		{
			x265_enc_cavlc_write ( enc_cavlc, sps->i_bits_for_poc, sps->lt_ref_pic_poc_lsb_sps[loop] ) ;
			x265_enc_cavlc_write1 ( enc_cavlc, sps->used_by_curr_pic_lt_sps_flag[loop] ) ;
		}
	}

	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_tmvp_flags_present ? 1 : 0 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_use_strong_intra_smoothing ? 1 : 0 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, sps->b_vui_parameters_present_flag ? 1 : 0 ) ;
	if (sps->b_vui_parameters_present_flag)
	{
		x265_enc_cavlc_code_vui ( enc_entropy_if, &sps->vui_parameters, sps ) ;
	}

	x265_enc_cavlc_write1 ( enc_cavlc, 0 ) ;
}


void x265_enc_cavlc_code_vps( x265_enc_entropy_if_t *enc_entropy_if, x265_vps_t *vps )
{
	int loop = 0 ;
	uint32_t i_ops_idx = 0 ;
	int32_t b_sub_layer_ordering_info_present_flag = 1 ;
	x265_timing_info_t *timing_info = NULL ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	x265_enc_cavlc_write ( enc_cavlc, 4, vps->i_vps_id ) ;
	x265_enc_cavlc_write ( enc_cavlc, 2, 3 ) ;
	x265_enc_cavlc_write ( enc_cavlc, 6, 0 ) ;
	x265_enc_cavlc_write ( enc_cavlc, 3, vps->i_max_t_layers - 1 ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, vps->b_temporal_id_nesting_flag ) ;
	x265_enc_cavlc_write ( enc_cavlc, 16, 0xFFFF ) ;

	x265_enc_cavlc_code_ptl ( enc_entropy_if, &vps->ptl, 1, vps->i_max_t_layers - 1 ) ;

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	x265_bit_rate_pic_rate_info_write ( s, &vps->b_bit_rate_pic_rate_info,
										0, vps->i_max_t_layers - 1 ) ;
#endif
	x265_enc_cavlc_write1 ( enc_cavlc, b_sub_layer_ordering_info_present_flag ) ;
	for ( loop = 0 ; loop <= vps->i_max_t_layers - 1 ; ++ loop )
	{
		x265_enc_cavlc_write_ue ( enc_cavlc, vps->max_dec_pic_buffering[loop] ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, vps->num_reorder_pics[loop] ) ;
		x265_enc_cavlc_write_ue ( enc_cavlc, vps->max_latency_increase[loop] ) ;
		if ( ! b_sub_layer_ordering_info_present_flag )
		{
			break ;
		}
	}

	x265_enc_cavlc_write ( enc_cavlc, 6, vps->i_max_nuh_reserved_zero_layer_id ) ;
	vps->i_num_op_sets = 1 ;
	x265_enc_cavlc_write_ue ( enc_cavlc, vps->i_num_op_sets - 1 ) ;

	for ( i_ops_idx = 0 ; loop <= (vps->i_num_op_sets - 1) ; ++ i_ops_idx )
	{
		for ( loop = 0 ; loop < vps->i_max_nuh_reserved_zero_layer_id ; ++ loop )
		{
			vps->layer_id_included_flag[i_ops_idx][loop] = 1 ;
			x265_enc_cavlc_write1 ( enc_cavlc, vps->layer_id_included_flag[i_ops_idx][loop] ? 1 : 0 ) ;
		}
	}


#if X265_L0043_TIMING_INFO
	timing_info = &vps->timing_info ;
	x265_enc_cavlc_write1 ( enc_cavlc, timing_info->b_timing_info_present_flag ) ;
	if ( timing_info->b_timing_info_present_flag )
	{
		x265_enc_cavlc_write ( enc_cavlc, 32, timing_info->i_num_units_in_tick ) ;
		x265_enc_cavlc_write ( enc_cavlc, 32, timing_info->i_time_scale ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, timing_info->b_poc_proportional_to_timing_flag ) ;
		if ( timing_info->b_poc_proportional_to_timing_flag )
		{
			x265_enc_cavlc_write_ue_big ( enc_cavlc, timing_info->i_num_ticks_poc_diff_one_minus1 ) ;
		}
#endif
		vps->i_num_hrd_parameters = 0 ;
		x265_enc_cavlc_write_ue ( enc_cavlc, vps->i_num_hrd_parameters ) ;
		if ( vps->i_num_hrd_parameters )
		{
			create_hrd_param_buffer( vps ) ;
		}

		for ( loop = 0 ; loop < vps->i_num_hrd_parameters ; ++ loop )
		{
			vps->hrd_op_set_idx[loop] = 0 ;
			x265_enc_cavlc_write_ue ( enc_cavlc, vps->hrd_op_set_idx[loop] ) ;
			if ( loop > 0 )
			{
				x265_enc_cavlc_write1 ( enc_cavlc, vps->cprms_present_flag[loop] ? 1 : 0 ) ;
			}
			x265_enc_cavlc_code_hrd_parameters ( enc_entropy_if,
												&vps->hrd_parameters[loop],
												vps->cprms_present_flag[loop],
												vps->i_max_t_layers - 1 ) ;
		}
#if X265_L0043_TIMING_INFO
	}
#endif
	x265_enc_cavlc_write1 ( enc_cavlc, 0 ) ;

}

void x265_enc_cavlc_code_slice_header ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t *slice )
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

    x265_reference_picture_set_t *rps = NULL ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;

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

	x265_enc_cavlc_write1 ( enc_cavlc, i_slice_segment_address == 0 ) ;
	if ( x265_slice_set_get_rap_pic_flag ( slice ) )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, 0 ) ;
	}
	x265_enc_cavlc_write_ue ( enc_cavlc, h->pps[0].i_pps_id ) ;
	slice->b_dependent_slice_segment_flag = !slice->b_next_slice ;
	if ( h->pps[0].b_dependent_slice_segments_enabled_flag && ( i_slice_segment_address !=0 ) )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, slice->b_dependent_slice_segment_flag ? 1 : 0 ) ;
	}
	if(i_slice_segment_address>0)
	{
		x265_enc_cavlc_write ( enc_cavlc, i_bits_slice_segment_address, i_slice_segment_address );
	}

	if ( !slice->b_dependent_slice_segment_flag )
	{
	    for ( loop = 0; loop < h->pps[0].i_num_extra_slice_header_bits ; ++ loop )
	    {
			x265_enc_cavlc_write1 ( enc_cavlc, 0 ) ;
	    }

		x265_enc_cavlc_write_ue ( enc_cavlc, slice->i_slice_type ) ;

	    if ( h->pps[0].b_output_flag_present_flag )
	    {
			x265_enc_cavlc_write1 ( enc_cavlc, slice->b_pic_output_flag ? 1 : 0 ) ;
	    }

	    // in the first version chroma_format_idc is equal to one, thus colour_plane_id will not be present
	    assert ( h->sps[0].i_chroma_format_idc == 1 ) ;
	    // if( separate_colour_plane_flag  ==  1 )
	    //   colour_plane_id                                      u(2)

	    if( ! x265_slice_get_idr_pic_flag ( slice ) )
	    {
	    	i_pic_order_cnt_lsb = (h->fdec->i_poc - slice->i_last_idr + (1 << h->sps[0].i_bits_for_poc)) % (1 << h->sps[0].i_bits_for_poc);
	    	x265_enc_cavlc_write ( enc_cavlc, h->sps[0].i_bits_for_poc, i_pic_order_cnt_lsb ) ;
	    	rps = slice->rps ;
	    	if(slice->i_rps_idx < 0)
	    	{
		    	x265_enc_cavlc_write1 ( enc_cavlc, 0 ) ;
	    		x265_enc_cavlc_code_short_term_ref_pic_set ( enc_entropy_if, rps, 1, h->sps[0].rps_list.i_number_of_reference_picture_sets ) ;
	    	}
	    	else
	    	{
		    	x265_enc_cavlc_write1 ( enc_cavlc, 1 ) ;
	    		i_num_bits = 0;
	    		while ((1 << i_num_bits) < h->sps[0].rps_list.i_number_of_reference_picture_sets )
	    		{
	    			i_num_bits ++ ;
	    		}
	    		if (i_num_bits > 0)
	    		{
	    	    	x265_enc_cavlc_write ( enc_cavlc, i_num_bits, slice->i_rps_idx ) ;
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
	    			if ( x265_enc_cavlc_find_matching_ltrp( h,
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
	    			x265_enc_cavlc_write_ue ( enc_cavlc, i_num_ltrp_in_sps ) ;
	    		}
    			x265_enc_cavlc_write_ue ( enc_cavlc, i_num_ltrp_in_sh ) ;
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
	    					x265_enc_cavlc_write ( enc_cavlc, i_bits_for_ltrp_in_sps, ltrp_in_sps[i_counter] ) ;
	    				}
	    			}
	    			else
	    			{
	    				x265_enc_cavlc_write ( enc_cavlc, h->sps[0].i_bits_for_poc, rps->poc_lsb_lt[loop] ) ;
	    				x265_enc_cavlc_write1 ( enc_cavlc, rps->used[loop] ) ;
	    			}
    				x265_enc_cavlc_write1 ( enc_cavlc, rps->delta_poc_msb_present_flag[loop] ) ;

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
	        				x265_enc_cavlc_write_ue ( enc_cavlc, rps->delta_poc_msb_cycle_lt[loop] ) ;
	    				}
	    				else
	    				{
	    					i_difference_in_delta_msb = rps->delta_poc_msb_cycle_lt[loop] - i_prev_delta_msb;
	    					assert(i_difference_in_delta_msb >= 0);
	        				x265_enc_cavlc_write_ue ( enc_cavlc, i_difference_in_delta_msb ) ;
	    				}
	    				i_prev_lsb = rps->poc_lsb_lt[loop];
	    				i_prev_delta_msb = rps->delta_poc_msb_cycle_lt[loop] ;
	    			}
	    		}
	    	}
	    	if (h->sps[0].b_tmvp_flags_present)
	    	{
				x265_enc_cavlc_write1 ( enc_cavlc, slice->b_enable_tmvp_flag ? 1 : 0 ) ;
	    	}
	    }
	    if(h->sps[0].b_use_sao)
	    {
			x265_enc_cavlc_write1 ( enc_cavlc, slice->b_sao_enabled_flag ? 1 : 0 ) ;
			x265_enc_cavlc_write1 ( enc_cavlc, h->sao_param->b_sao_flag[1] ? 1 : 0 ) ;
	    }

	    //check if numrefidxes match the defaults. If not, override

	    if ( ! ( I_SLICE == slice->i_slice_type ) )
	    {
	    	b_override_flag = (h->i_ref[REF_PIC_LIST_0] != h->pps[0].i_num_ref_idx_l0_default_active ) ;
	    	x265_enc_cavlc_write1 ( enc_cavlc, b_override_flag ? 1 : 0 );
	    	if (b_override_flag)
	    	{
	    		x265_enc_cavlc_write_ue ( enc_cavlc,  h->i_ref[REF_PIC_LIST_0] - 1 );
	    		if ( B_SLICE == slice->i_slice_type )
	    		{
		    		x265_enc_cavlc_write_ue ( enc_cavlc,  h->i_ref[REF_PIC_LIST_1] - 1 );
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
	    		x265_enc_cavlc_write1 ( enc_cavlc, h->ref_pic_list_modification.ref_pic_set_idx_l0 ? 1 : 0 ) ;
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
	    					x265_enc_cavlc_write ( enc_cavlc, i_length, ref_pic_list_modification->ref_pic_set_idx_l0[loop] ) ;
	    				}
	    			}
	    		}
	    	}
	    	if( B_SLICE == slice->i_slice_type )
	    	{
	    		x265_enc_cavlc_write1 ( enc_cavlc, h->ref_pic_list_modification.ref_pic_set_idx_l1 ? 1 : 0 ) ;
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
	    					x265_enc_cavlc_write ( enc_cavlc, i_length, ref_pic_list_modification->ref_pic_set_idx_l1[loop] ) ;
	    				}
	    			}
	    		}
	    	}
	    }

	    if ( B_SLICE == slice->i_slice_type )
	    {
	    	x265_enc_cavlc_write1 ( enc_cavlc, slice->b_l_mvd_l1_zero );
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
	    		x265_enc_cavlc_write1 ( enc_cavlc, b_enc_cabac_init_flag ? 1 : 0 ) ;
	    	}
	    }

	    if ( slice->b_enable_tmvp_flag )
	    {
	    	if ( slice->i_slice_type == B_SLICE )
	    	{
	    		x265_enc_cavlc_write1 ( enc_cavlc, slice->i_col_from_l0_flag );
	    	}

	    	if ( slice->i_slice_type != I_SLICE &&
	    		((slice->i_col_from_l0_flag == 1
	    		&& h->i_ref[REF_PIC_LIST_0] > 1)
	    		|| (slice->i_col_from_l0_flag == 0
	    		&& h->i_ref[REF_PIC_LIST_1] > 1)))
	    	{
	    		x265_enc_cavlc_write_ue ( enc_cavlc,  slice->i_col_ref_idx );
	    	}
	    }
	    if ( (h->pps[0].b_use_weight_pred && slice->i_slice_type==P_SLICE)
	    		|| (h->pps[0].b_use_weighted_bi_pred && slice->i_slice_type==B_SLICE) )
	    {
	    	x265_enc_cavlc_x_code_pred_weight_table ( h, enc_entropy_if, slice ) ;
	    }
	    assert(slice->i_max_num_merge_cand<=X265_MRG_MAX_NUM_CANDS);
	    if ( ! (I_SLICE == slice->i_slice_type ) )
	    {
	    	x265_enc_cavlc_write_ue ( enc_cavlc, X265_MRG_MAX_NUM_CANDS - slice->i_max_num_merge_cand ) ;
	    }
	    i_code = slice->i_slice_qp - ( h->pps[0].i_pic_init_qp_minus26 + 26 );
	    x265_enc_cavlc_write_se ( enc_cavlc, i_code );
	    if (h->pps[0].b_slice_chroma_qp_flag)
	    {
	    	i_code = slice->i_slice_qp_delta_cb;
		    x265_enc_cavlc_write_se ( enc_cavlc, i_code );
	    	i_code = slice->i_slice_qp_delta_cr ;
		    x265_enc_cavlc_write_se ( enc_cavlc, i_code );
	    }
	    if (h->pps[0].b_deblocking_filter_control_present_flag)
	    {
	    	if (h->pps[0].b_deblocking_filter_override_enabled_flag )
	    	{
	    		x265_enc_cavlc_write1 ( enc_cavlc,slice->b_deblocking_filter_override_flag );
	    	}
	    	if ( slice->b_deblocking_filter_override_flag )
	    	{
	    		x265_enc_cavlc_write1 ( enc_cavlc, slice->b_deblocking_filter_disable );
	    		if ( ! slice->b_deblocking_filter_disable )
	    		{
	    		    x265_enc_cavlc_write_se ( enc_cavlc, slice->i_deblocking_filter_beta_offset_div2 );
	    		    x265_enc_cavlc_write_se ( enc_cavlc, slice->i_deblocking_filter_tc_offset_div2 );
	    		}
	    	}
	    }

	    b_is_sao_enabled = (!h->sps[0].b_use_sao) ? 0
	    					: (slice->b_sao_enabled_flag || slice->b_sao_enabled_flag_chroma );
	    b_is_dbf_enabled = ( !slice->b_deblocking_filter_disable );

	    if(h->pps[0].b_loop_filter_across_slices_enabled_flag && ( b_is_sao_enabled || b_is_dbf_enabled ))
	    {
	    	x265_enc_cavlc_write1 ( enc_cavlc,slice->b_lf_cross_slice_boundary_flag ? 1 : 0 );
	    }
	}
	if ( h->pps[0].b_slice_header_extension_present_flag )
	{
		x265_enc_cavlc_write_ue ( enc_cavlc, 0 );
	}
}

void x265_enc_cavlc_code_ptl ( x265_enc_entropy_if_t *enc_entropy_if,
								x265_ptl_t *ptl,
								int32_t b_profile_present_flag,
								int32_t i_max_num_sub_layers_minus1 )
{
	int loop = 0 ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	if ( b_profile_present_flag )
	{
		x265_enc_cavlc_code_profile_tier_level ( enc_entropy_if, &ptl->p_general_ptl ) ;
	}
	x265_enc_cavlc_write ( enc_cavlc, 8, ptl->p_general_ptl.i_level_idc ) ;

#if X265_L0363_BYTE_ALIGN
	for ( loop = 0 ; loop < i_max_num_sub_layers_minus1 ; ++ loop )
	{
		if ( b_profile_present_flag )
		{
			x265_enc_cavlc_write1 ( enc_cavlc, ptl->sub_layer_profile_present_flag[loop] ) ;
		}
		x265_enc_cavlc_write1 ( enc_cavlc, ptl->sub_layer_level_present_flag[loop] ) ;
	}

	if ( i_max_num_sub_layers_minus1 > 0 )
	{
		for ( loop = i_max_num_sub_layers_minus1 ; loop < 8 ; ++ loop )
		{
			x265_enc_cavlc_write ( enc_cavlc, 2, 0 ) ;
		}
	}
#endif

	for ( loop = 0 ; loop < i_max_num_sub_layers_minus1 ; ++ loop )
	{
#if !X265_L0363_BYTE_ALIGN
		if ( b_profile_present_flag )
		{
			x265_enc_cavlc_write1 ( enc_cavlc, ptl->sub_layer_profile_present_flag[loop] ) ;
		}
		x265_enc_cavlc_write1 ( enc_cavlc, ptl->sub_layer_level_present_flag[loop] ) ;
#endif
		if ( b_profile_present_flag && ptl->sub_layer_profile_present_flag[loop] )
		{
			x265_enc_cavlc_code_profile_tier_level ( enc_entropy_if, &ptl->sub_layer_ptl[loop] ) ;
		}
		if ( ptl->sub_layer_profile_present_flag[loop] )
		{
			x265_enc_cavlc_write ( enc_cavlc, 8, ptl->sub_layer_ptl[loop].i_level_idc ) ;
		}
	}
}


void x265_enc_cavlc_code_profile_tier_level ( x265_enc_entropy_if_t *enc_entropy_if,
											x265_profile_tier_level_t *profile_tier_level )
{
	int loop = 0 ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	x265_enc_cavlc_write ( enc_cavlc, 2, profile_tier_level->i_profile_space ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, profile_tier_level->b_tier_flag ) ;
	x265_enc_cavlc_write ( enc_cavlc, 5, profile_tier_level->i_profile_idc ) ;

	for ( loop = 0 ; loop < 32 ; ++ loop )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, profile_tier_level->profile_compatibility_flag[loop] ) ;
	}

#if X265_L0046_CONSTRAINT_FLAGS
	x265_enc_cavlc_write1 ( enc_cavlc, profile_tier_level->b_progressive_source_flag ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, profile_tier_level->b_interlaced_source_flag ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, profile_tier_level->b_non_packed_constraint_flag ) ;
	x265_enc_cavlc_write1 ( enc_cavlc, profile_tier_level->b_frame_only_constraint_flag ) ;

	x265_enc_cavlc_write ( enc_cavlc, 16, 0 ) ;
	x265_enc_cavlc_write ( enc_cavlc, 16, 0 ) ;
	x265_enc_cavlc_write ( enc_cavlc, 12, 0 ) ;

#elif X265_L0363_MORE_BITS
	x265_enc_cavlc_write ( enc_cavlc, 16, 0 ) ;
	x265_enc_cavlc_write ( enc_cavlc, 16, 0 ) ;
	x265_enc_cavlc_write ( enc_cavlc, 16, 0 ) ;
#else
	x265_enc_cavlc_write ( enc_cavlc, 16, 0 ) ;
#endif
}

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
void x265_enc_cavlc_bit_rate_pic_rate_info_write ( x265_enc_entropy_if_t *enc_entropy_if,
													x265_bit_rate_pic_rate_info_t *bit_rate_pic_rate_info,
													int i_temp_level_low, int i_temp_level_high )
{
	int loop = 0 ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	for( loop = i_temp_level_low ; loop <= i_temp_level_high ; ++ loop )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, bit_rate_pic_rate_info->bit_rate_info_present_flag[loop] ) ;
		x265_enc_cavlc_write1 ( enc_cavlc, bit_rate_pic_rate_info->pic_rate_info_present_flag[loop] ) ;;
		if( bit_rate_pic_rate_info->bit_rate_info_present_flag[loop] )
		{
			x265_enc_cavlc_write ( enc_cavlc, 16, bit_rate_pic_rate_info->avg_bit_rate[loop] ) ;
			x265_enc_cavlc_write ( enc_cavlc, 16, bit_rate_pic_rate_info->max_bit_rate[loop] ) ;
		}
		if( bit_rate_pic_rate_info->pic_rate_info_present_flag )
		{
			x265_enc_cavlc_write ( enc_cavlc, 2, bit_rate_pic_rate_info->constant_pic_rate_idc[loop] ) ;
			x265_enc_cavlc_write ( enc_cavlc, 16, bit_rate_pic_rate_info->avg_pic_rate[loop] ) ;
		}
	}
}
#endif


int x265_enc_cavlc_code_tiles_wpp_entry_point( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if,
												bs_t *s, x265_slice_t *slice )
{
	int32_t i_idx = 0 ;
	uint32_t i_num_entry_point_offsets = 0 ;
	uint32_t i_offset_len_minus1 = 0 ;
	uint32_t i_max_offset = 0;
	int32_t  i_num_zero_substreams_at_start_of_slice  = 0;
	uint32_t *entry_point_offset = NULL;
	uint32_t *substream_sizes = NULL ;
    int32_t i_max_num_parts = 0 ;
    int32_t i_num_zero_substreams_at_end_of_slice = 0 ;

	if ( (!h->pps[0].b_tiles_enabled_flag) && (!h->pps[0].b_entropy_coding_sync_enabled_flag))
	{
		return 0 ;
	}

	if ( h->pps[0].b_tiles_enabled_flag )
	{
	    i_num_entry_point_offsets = x265_slice_get_tile_location_count(slice);
		CHECKED_MALLOCZERO(entry_point_offset,
				sizeof(uint32_t) * i_num_entry_point_offsets );
	    for ( i_idx = 0; i_idx < x265_slice_get_tile_location_count(slice); ++ i_idx )
	    {
	    	if ( i_idx == 0 )
	    	{
	    		entry_point_offset [ i_idx ] = x265_slice_get_tile_location(slice, 0) ;
	    	}
	    	else
	    	{
	    		entry_point_offset [ i_idx ] = x265_slice_get_tile_location(slice, i_idx)
												- x265_slice_get_tile_location(slice, i_idx - 1);
	    	}

	    	if ( entry_point_offset[ i_idx ] > i_max_offset )
	    	{
	    		i_max_offset = entry_point_offset[ i_idx ];
	    	}
	    }
	}
	else if ( h->pps[0].b_entropy_coding_sync_enabled_flag )
	{
		substream_sizes = slice->substream_sizes;
	    i_max_num_parts = h->cu.pic.i_num_partitions ;
	    i_num_zero_substreams_at_start_of_slice = slice->i_slice_segment_cur_start_cu_addr
	    											/ i_max_num_parts
	    											/ h->cu.pic.i_width_in_cu ;
	    i_num_zero_substreams_at_end_of_slice = h->cu.pic.i_height_in_cu - 1
	    										- ((slice->i_slice_segment_cur_end_cu_addr - 1)
	    											/ i_max_num_parts / h->cu.pic.i_width_in_cu ) ;
	    i_num_entry_point_offsets = h->pps[0].i_num_substreams - i_num_zero_substreams_at_start_of_slice - i_num_zero_substreams_at_end_of_slice - 1;
	    slice->i_num_entry_point_offsets = i_num_entry_point_offsets ;
		CHECKED_MALLOCZERO(entry_point_offset,
							i_num_entry_point_offsets*sizeof(uint32_t));
	    for ( i_idx=0; i_idx<i_num_entry_point_offsets; i_idx++)
	    {
	    	entry_point_offset[ i_idx ] = ( substream_sizes[ i_idx+i_num_zero_substreams_at_start_of_slice ] >> 3 ) ;
	    	if ( entry_point_offset[ i_idx ] > i_max_offset )
	    	{
	    		i_max_offset = entry_point_offset[ i_idx ];
	    	}
	    }
	}
	// determine number of bits "i_offset_len_minus1+1" required for entry point information
	i_offset_len_minus1 = 0;
	while (i_max_offset >= (1u << (i_offset_len_minus1 + 1)))
	{
		i_offset_len_minus1++;
		assert(i_offset_len_minus1 + 1 < 32);
	}

	bs_write_ue ( s, i_num_entry_point_offsets );
	if (i_num_entry_point_offsets>0)
	{
		bs_write_ue ( s, i_offset_len_minus1 ) ;
	}

	for ( i_idx = 0 ; i_idx < i_num_entry_point_offsets ; ++ i_idx )
	{
#if X265_L0116_ENTRY_POINT
		bs_write ( s, i_offset_len_minus1 + 1, entry_point_offset[i_idx] -1 );
#else
		bs_write ( s, i_offset_len_minus1 + 1, entry_point_offset[i_idx] );
#endif
	}

	x265_free ( entry_point_offset ) ;
	return 0 ;


fail:
	x265_free ( entry_point_offset ) ;
	return -1 ;
}


void x265_enc_cavlc_code_terminating_bit (x265_enc_entropy_if_t *enc_entropy_if,
										uint32_t i_ls_last )
{
}

void x265_enc_cavlc_code_slice_finish (x265_enc_entropy_if_t *enc_entropy_if )
{
}

void x265_enc_cavlc_code_mvp_idx (x265_enc_entropy_if_t *enc_entropy_if,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								enum ref_pic_list_e i_ref_list )
{
	assert(0);
}

void x265_enc_cavlc_code_part_size(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									uint32_t i_depth )
{
	assert(0);
}

void x265_enc_cavlc_code_pred_mode(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_merge_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_merge_index(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_inter_mode_flag(x265_enc_entropy_if_t *enc_entropy_if,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth,
										uint32_t i_enc_mode )
{
	assert(0);
}

void x265_enc_cavlc_code_cu_transquant_bypass_flag(x265_enc_entropy_if_t *enc_entropy_if,
												x265_data_cu_t *cu,
												uint32_t i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_skip_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu, uint32_t
									i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_split_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									uint32_t i_depth )
{
	assert(0);
}

void x265_enc_cavlc_code_transform_sub_div_flag(x265_t* h,
												x265_enc_entropy_if_t *enc_entropy_if,
												uint32_t i_symbol,
												uint32_t i_ctx )
{
	assert(0);
}

void x265_enc_cavlc_code_qt_cbf(x265_t* h,
								x265_enc_entropy_if_t *enc_entropy_if,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								enum text_type_e i_text_type,
								uint32_t i_tr_depth )
{
	assert(0);
}

void x265_enc_cavlc_code_qt_root_cbf(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_qt_cbf_zero(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									enum text_type_e i_text_type,
									uint32_t i_tr_depth )
{
	assert(0);
}

void x265_enc_cavlc_code_qt_root_cbf_zero(x265_enc_entropy_if_t *enc_entropy_if,
										x265_data_cu_t *cu )
{
	assert(0);
}

void x265_enc_cavlc_code_transform_skip_flags(x265_t* h,
											x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx,
											uint32_t width,
											uint32_t height,
											enum text_type_e i_text_type )
{
	assert(0);
}

/** code i_pcm information.
 * \param pc_c_u pointer to cu
 * \param i_abs_part_idx cu index
 * \returns void
 */
void x265_enc_cavlc_code_ipcm_info(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_intra_dir_luma_ang(x265_t* h,
											x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx,
											int32_t b_is_multiple)
{
	assert(0);
}

void x265_enc_cavlc_code_intra_dir_chroma(x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_inter_dir(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx )
{
	assert(0);
}

void x265_enc_cavlc_code_ref_frm_idx(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									enum ref_pic_list_e i_ref_pic_list )
{
	assert(0);
}

void x265_enc_cavlc_code_mvd(x265_t* h,
							x265_enc_entropy_if_t *enc_entropy_if,
							x265_data_cu_t *cu,
							uint32_t i_abs_part_idx,
							enum ref_pic_list_e i_ref_pic_list )
{
	assert(0);
}


void x265_enc_cavlc_code_delta_qp ( x265_t *h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx )
{
	int32_t i_qp_bd_offset_y = 0 ;
	int32_t i_dqp = 0 ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	i_dqp = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, i_abs_part_idx)
			- x265_data_cu_get_ref_qp ( h, cu, i_abs_part_idx ) ;

	i_qp_bd_offset_y = h->sps[0].i_qp_bd_offset_y ;
	i_dqp = (i_dqp + 78 + i_qp_bd_offset_y + (i_qp_bd_offset_y/2))
				% (52 + i_qp_bd_offset_y) - 26 - (i_qp_bd_offset_y/2);
	x265_enc_cavlc_write_se ( enc_cavlc, i_dqp ) ;
}



void x265_enc_cavlc_code_coeff_nxn(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									x265_coeff_t *coeff,
									uint32_t i_abs_part_idx,
									uint32_t i_width,
									uint32_t i_height, uint32_t i_depth,
									enum text_type_e i_text_type )
{
	assert(0);
}

void x265_enc_cavlc_est_bit(x265_t *h,
							x265_enc_entropy_if_t *enc_entropy_if,
							x265_est_bits_sbac_struct_t* est_bits_sbac_struct,
							int32_t i_width,
							int32_t i_height,
							enum text_type_e i_text_type )
{
	//	printf("error : no vlc mode support in this version\n");
	return;
}

// ====================================================================================================================
// protected member functions
// ====================================================================================================================


/** write pcm alignment bits.
 * \returns void
 */
void  x265_enc_cavlc_x_write_pcm_align_zero(x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	enc_cavlc->bit_if->write_align_zero (enc_cavlc->bit_if) ;
}

void x265_enc_cavlc_x_write_unary_max_symbol(x265_enc_entropy_if_t *enc_entropy_if,
											uint32_t i_symbol,
											uint32_t i_max_symbol )
{
	int32_t b_code_last = 0 ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	if (i_max_symbol == 0)
	{
		return;
	}
	x265_enc_cavlc_write1 ( enc_cavlc, i_symbol ? 1 : 0 );
	if ( i_symbol == 0 )
	{
		return;
	}

	b_code_last = ( i_max_symbol > i_symbol );

	while( --i_symbol )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, 1 );
	}
	if( b_code_last )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, 0 );
	}
	return;
}

void x265_enc_cavlc_x_write_ex_golomb_level(x265_enc_entropy_if_t *enc_entropy_if,
											uint32_t i_symbol )
{
	uint32_t i_count = 0;
    int32_t b_no_ex_go = 0 ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	if( i_symbol )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, 1 );
		b_no_ex_go = (i_symbol < 13);

		while( --i_symbol && ++i_count < 13 )
		{
			x265_enc_cavlc_write1 ( enc_cavlc, 1 );
		}
		if( b_no_ex_go )
		{
			x265_enc_cavlc_write1 ( enc_cavlc, 0 );
		}
		else
		{
			x265_enc_cavlc_x_write_unary_max_symbol( enc_entropy_if, i_symbol, 0 );
		}
	}
	else
	{
		x265_enc_cavlc_write1 ( enc_cavlc, 0 );
	}
	return;
}


void x265_enc_cavlc_x_write_ep_ex_golomb(x265_enc_entropy_if_t *enc_entropy_if,
										uint32_t i_symbol,
										uint32_t i_count )
{
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	while( i_symbol >= (uint32_t)(1<<i_count) )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, 1 );
		i_symbol -= 1<<i_count;
		i_count  ++;
	}
	x265_enc_cavlc_write1 ( enc_cavlc, 0 );
	while( i_count-- )
	{
		x265_enc_cavlc_write1 ( enc_cavlc, (i_symbol>>i_count) & 1 );
	}
	return;
}

void x265_enc_cavlc_x_code_pred_weight_table ( x265_t *h,
											x265_enc_entropy_if_t *enc_entropy_if,
											x265_slice_t *slice )
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
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
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
					x265_enc_cavlc_write_ue ( enc_cavlc, wp_scaling_param[0].i_log2_weight_denom );     // ue(v): luma_log2_weight_denom

					if( b_chroma )
					{
						i_delta_denom = (wp_scaling_param[1].i_log2_weight_denom - wp_scaling_param->i_log2_weight_denom ) ;
						x265_enc_cavlc_write_se ( enc_cavlc, i_delta_denom );       // se(v): delta_chroma_log2_weight_denom
					}
					b_denom_coded = 1 ;
				}
				x265_enc_cavlc_write1 ( enc_cavlc, wp_scaling_param[0].b_present_flag ) ;               // u(1): luma_weight_lX_flag
				i_total_signalled_weight_flags += wp_scaling_param[0].b_present_flag ;
			}
			if (b_chroma)
			{
				for ( i_ref_idx = 0 ; i_ref_idx <h->i_ref[i_ref_pic_list] ; i_ref_idx++ )
				{
					wp_scaling_param = slice->weight_pred_table[i_ref_pic_list][i_ref_idx] ;
					x265_enc_cavlc_write1 ( enc_cavlc, wp_scaling_param[1].b_present_flag ) ;           // u(1): chroma_weight_lX_flag
					i_total_signalled_weight_flags += 2 * wp_scaling_param[1].b_present_flag ;
				}
			}

			for ( i_ref_idx = 0 ; i_ref_idx < h->i_ref[i_ref_pic_list] ; i_ref_idx++ )
			{
				wp_scaling_param = slice->weight_pred_table[i_ref_pic_list][i_ref_idx] ;
				if ( wp_scaling_param[0].b_present_flag )
				{
					i_delta_weight = (wp_scaling_param[0].i_weight - (1  << wp_scaling_param[0].i_log2_weight_denom ) ) ;
					x265_enc_cavlc_write_se ( enc_cavlc, i_delta_weight );                  // se(v): delta_luma_weight_lX
					x265_enc_cavlc_write_se ( enc_cavlc, wp_scaling_param[0].offset );                       // se(v): luma_offset_lX
				}

				if ( b_chroma )
				{
					if ( wp_scaling_param[1].b_present_flag )
					{
						for ( loop = 1 ; loop < 3 ; ++ loop )
						{
							i_delta_weight = (wp_scaling_param[loop].i_weight - (1<<wp_scaling_param[1].i_log2_weight_denom));
							x265_enc_cavlc_write_se ( enc_cavlc, i_delta_weight );            // se(v): delta_chroma_weight_lX

							i_pred = ( 128 - ( ( 128*wp_scaling_param[loop].i_weight)>>(wp_scaling_param[loop].i_log2_weight_denom) ) );
							i_delta_chroma = (wp_scaling_param[loop].i_offset - i_pred);
							x265_enc_cavlc_write_se ( enc_cavlc, i_delta_chroma );            // se(v): delta_chroma_offset_lX
						}
					}
				}
			}
		}
		assert(i_total_signalled_weight_flags<=24);
	}
}

void x265_enc_cavlc_code_scaling_list ( x265_t *h,
										x265_enc_entropy_if_t *enc_entropy_if,
										x265_scaling_list_t *scaling_list )
{
	uint32_t i_size_id = 0 ;
	uint32_t i_list_id = 0 ;
	int32_t b_scaling_list_pred_mode_flag ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	for ( i_size_id = 0; i_size_id < SCALING_LIST_SIZE_NUM ; ++ i_size_id )
	{
		for ( i_list_id = 0; i_list_id < x265_scaling_list_get_scaling_list_num(i_size_id) ; ++ i_list_id )
		{
			b_scaling_list_pred_mode_flag = x265_enc_cavlc_scaling_list_check_pred_mode ( scaling_list,
																						i_size_id,
																						i_list_id ) ;
			x265_enc_cavlc_write1 ( enc_cavlc, b_scaling_list_pred_mode_flag ) ;
			// Copy Mode
	        if ( ! b_scaling_list_pred_mode_flag )
	        {
	        	x265_enc_cavlc_write_se ( enc_cavlc, i_list_id - scaling_list->ref_matrix_id[i_size_id][i_list_id] ) ;
	        }
	        else// DPCM Mode
	        {
	        	x265_enc_cavlc_x_code_scaling_list_write ( h, enc_entropy_if, scaling_list, i_size_id, i_list_id ) ;
	        }
		}
    }
}

void x265_enc_cavlc_x_code_scaling_list_write ( x265_t *h,
												x265_enc_entropy_if_t *enc_entropy_if,
												x265_scaling_list_t *scaling_list,
												uint32_t i_size_id,
												uint32_t i_list_id)
{
	int i_coef_num = 0 ;
	int32_t loop = 0 ;
	uint32_t * scan = 0 ;
	int32_t i_next_coef = SCALING_LIST_START_VALUE;
	int32_t i_data = 0 ;
	int32_t *src = NULL ;
	x265_enc_cavlc_t *enc_cavlc = NULL ;

	enc_cavlc = (x265_enc_cavlc_t*) enc_entropy_if ;
	i_coef_num = x265_scaling_list_get_coef_num ( i_size_id ) ;
	src = scaling_list->scaling_list_coef[i_size_id][i_list_id] ;
	scan = (i_size_id == 0) ? h->scan.sig_last_scan [ SCAN_DIAG ] [ 1 ]
	                        : h->scan.sig_last_scan_cg_32x32 ;
    if( i_size_id > SCALING_LIST_8x8 )
    {
    	x265_enc_cavlc_write_se ( enc_cavlc, scaling_list->scaling_list_dc[i_size_id][i_list_id] - 8 ) ;
    	i_next_coef = scaling_list->scaling_list_dc[i_size_id][i_list_id];
    }

    for ( loop = 0 ; loop < i_coef_num ; ++ loop )
    {
    	i_data = src[scan[loop]] - i_next_coef;
    	i_next_coef = src[scan[loop]];
    	if (i_data > 127)
    	{
    		i_data = i_data - 256;
    	}
    	if (i_data < -128)
    	{
    		i_data = i_data + 256;
    	}
    	x265_enc_cavlc_write_se ( enc_cavlc, i_data ) ;
    }
}



int x265_enc_cavlc_find_matching_ltrp ( x265_t *h,
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

int x265_enc_cavlc_scaling_list_check_pred_mode ( x265_scaling_list_t *scaling_list,
												uint32_t i_size_id, uint32_t i_list_id )
{
	int32_t i_pred_list_id = 0 ;
	for( i_pred_list_id = (int32_t)i_list_id ; i_pred_list_id >= 0 ; -- i_pred_list_id )
	{
		// check value of matrix
		if( !memcmp( scaling_list->scaling_list_coef[i_size_id][i_list_id],
					((i_list_id == i_pred_list_id) ?
					get_scaling_list_default_address (i_size_id, i_pred_list_id) :
					scaling_list->scaling_list_coef[i_size_id][i_pred_list_id]),
					sizeof(int32_t) * X265_MIN(X265_MAX_MATRIX_COEF_NUM, x265_scaling_list_get_scaling_list_size(i_size_id))
					)
		&& ((i_size_id < SCALING_LIST_16x16)
		// check DC value
			|| (scaling_list->scaling_list_dc[i_size_id][i_list_id]
			== scaling_list->scaling_list_dc[i_size_id][i_pred_list_id])))
		{
			scaling_list->ref_matrix_id[i_size_id][i_list_id] = i_pred_list_id ;
			return 0 ;
		}
	}
	return 1;
}


void x265_enc_cavlc_write ( x265_enc_cavlc_t *enc_cavlc, int i_count, uint32_t i_bits )
{
	enc_cavlc->bit_if->write ( enc_cavlc->bit_if, i_bits, i_count ) ;
}

void x265_enc_cavlc_write1 ( x265_enc_cavlc_t *enc_cavlc, uint32_t i_bits )
{
	enc_cavlc->bit_if->write ( enc_cavlc->bit_if, i_bits, 1 ) ;
}

void x265_enc_cavlc_write_ue ( x265_enc_cavlc_t *enc_cavlc, uint32_t val )
{
	enc_cavlc->bit_if->write ( enc_cavlc->bit_if, val+1, x264_ue_size_tab[val+1] ) ;
}

void x265_enc_cavlc_write_ue_big( x265_enc_cavlc_t *enc_cavlc, unsigned int val )
{
    int size = 0;
    int tmp = ++val;
    if( tmp >= 0x10000 )
    {
        size = 32;
        tmp >>= 16;
    }
    if( tmp >= 0x100 )
    {
        size += 16;
        tmp >>= 8;
    }
    size += x264_ue_size_tab[tmp];
    enc_cavlc->bit_if->write ( enc_cavlc->bit_if, 0, size>>1 );
    enc_cavlc->bit_if->write ( enc_cavlc->bit_if, val, (size>>1)+1 );
}

void x265_enc_cavlc_write_se ( x265_enc_cavlc_t *enc_cavlc, uint32_t val )
{
    int size = 0;
    /* Faster than (val <= 0 ? -val*2+1 : val*2) */
    /* 4 instructions on x86, 3 on ARM */
    int tmp = 1 - val*2;
    if( tmp < 0 ) tmp = val*2;
    val = tmp;

    if( tmp >= 0x100 )
    {
        size = 16;
        tmp >>= 8;
    }
    size += x264_ue_size_tab[tmp];

	enc_cavlc->bit_if->write ( enc_cavlc->bit_if, val, size ) ;
}

