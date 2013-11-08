

#include "common/common.h"
#include "set.h"



void x265_vps_init( x265_vps_t *vps, x265_param_t *param )
{
	int loop = 0 ;
#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	x265_bit_rate_pic_rate_info_t *bit_rate_pic_rate_info = NULL ;
#endif

	init_vps ( vps ) ;
	vps->i_max_t_layers = param->sps.i_max_temp_layer ;
	if ( 1 == vps->i_max_t_layers )
	{
		vps->b_temporal_id_nesting_flag = 1 ;
	}
	vps->i_max_layers = 1 ;

	for ( loop = 0 ; loop < X265_MAX_TLAYER ; ++ loop )
	{
		vps->num_reorder_pics[loop] = param->gop.num_reorder_pics[loop] ;
		vps->max_dec_pic_buffering[loop] = param->gop.max_dec_pic_buffering[loop] ;
	}

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	bit_rate_pic_rate_info = &vps->b_bit_rate_pic_rate_info ;
	// The number of bit rate/pic rate have to equal to number of sub-layers.
	if ( param->vps.i_bit_rate_pic_rate_max_t_layers )
	{
		assert( param->vps.i_bit_rate_pic_rate_max_t_layers
				== vps->i_max_t_layers ) ;
	}
	for( loop = 0; loop < param->vps.i_bit_rate_pic_rate_max_t_layers ; ++ loop )
	{
		bit_rate_pic_rate_info->bit_rate_info_present_flag[loop] = param->vps.bit_rate_info_present_flag[loop] ;
		if ( bit_rate_pic_rate_info->bit_rate_info_present_flag[loop] )
		{
			bit_rate_pic_rate_info->avg_bit_rate[loop] = param->vps.avg_bit_rate[loop] ;
			bit_rate_pic_rate_info->max_bit_rate[loop] = param->vps.max_bit_rate[loop] ;
		}
	}

	for( loop = 0; loop < param->vps.i_bit_rate_pic_rate_max_t_layers ; ++ loop )
	{
		bit_rate_pic_rate_info->pic_rate_info_present_flag[loop] = param->vps.pic_rate_info_present_flag[loop] ;
		if ( bit_rate_pic_rate_info->pic_rate_info_present_flag[loop] )
		{
			bit_rate_pic_rate_info->avg_pic_rate[loop] = param->vps.avg_pic_rate[loop] ;
			bit_rate_pic_rate_info->constant_pic_rate_idc[loop] = param->vps.constant_pic_rate_idc[loop] ;
		}
	}
#endif

}

void x265_vps_deinit( x265_vps_t *vps )
{
	deinit_vps ( vps ) ;
}

void x265_profile_tier_level_write ( bs_t *s, x265_profile_tier_level_t *profile_tier_level )
{
	int loop = 0 ;

	bs_write ( s, 2, profile_tier_level->i_profile_space ) ;
	bs_write1 ( s, profile_tier_level->b_tier_flag ) ;
	bs_write ( s, 5, profile_tier_level->i_profile_idc ) ;

	for ( loop = 0 ; loop < 32 ; ++ loop )
	{
		bs_write1 ( s, profile_tier_level->profile_compatibility_flag[loop] ) ;
	}

#if X265_L0046_CONSTRAINT_FLAGS
	bs_write1 ( s, profile_tier_level->b_progressive_source_flag ) ;
	bs_write1 ( s, profile_tier_level->b_interlaced_source_flag ) ;
	bs_write1 ( s, profile_tier_level->b_non_packed_constraint_flag ) ;
	bs_write1 ( s, profile_tier_level->b_frame_only_constraint_flag ) ;

	bs_write ( s, 16, 0 ) ;
	bs_write ( s, 16, 0 ) ;
	bs_write ( s, 12, 0 ) ;

#elif X265_L0363_MORE_BITS
	bs_write ( s, 16, 0 ) ;
	bs_write ( s, 16, 0 ) ;
	bs_write ( s, 16, 0 ) ;
#else
	bs_write ( s, 16, 0 ) ;
#endif
}

void x265_ptl_write ( bs_t *s, x265_ptl_t *ptl,
					int32_t b_profile_present_flag,
					int32_t i_max_num_sub_layers_minus1 )
{
	int loop = 0 ;

	if ( b_profile_present_flag )
	{
		x265_profile_tier_level_write ( s, &ptl->p_general_ptl ) ;
	}
	bs_write ( s, 8, ptl->p_general_ptl.i_level_idc ) ;

#if X265_L0363_BYTE_ALIGN
	for ( loop = 0 ; loop < i_max_num_sub_layers_minus1 ; ++ loop )
	{
		if ( b_profile_present_flag )
		{
			bs_write1 ( s, ptl->sub_layer_profile_present_flag[loop] ) ;
		}
		bs_write1 ( s, ptl->sub_layer_level_present_flag[loop] ) ;
	}

	if ( i_max_num_sub_layers_minus1 > 0 )
	{
		for ( loop = i_max_num_sub_layers_minus1 ; loop < 8 ; ++ loop )
		{
			bs_write ( s, 2, 0 ) ;
		}
	}
#endif

	for ( loop = 0 ; loop < i_max_num_sub_layers_minus1 ; ++ loop )
	{
#if !X265_L0363_BYTE_ALIGN
		if ( b_profile_present_flag )
		{
			bs_write1 ( s, ptl->sub_layer_profile_present_flag[loop] ) ;
		}
		bs_write1 ( s, ptl->sub_layer_level_present_flag[loop] ) ;
#endif
		if ( b_profile_present_flag && ptl->sub_layer_profile_present_flag[loop] )
		{
			x265_profile_tier_level_write ( s, &ptl->sub_layer_ptl[loop] ) ;
		}
		if ( ptl->sub_layer_profile_present_flag[loop] )
		{
			bs_write ( s, 8, ptl->sub_layer_ptl[loop].i_level_idc ) ;
		}
	}
}

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
void x265_bit_rate_pic_rate_info_write ( bs_t *s,
										x265_bit_rate_pic_rate_info_t *bit_rate_pic_rate_info,
										int i_temp_level_low, int i_temp_level_high )
{
	int loop = 0 ;

	for( loop = i_temp_level_low ; loop <= i_temp_level_high ; ++ loop )
	{
		bs_write1 ( s, bit_rate_pic_rate_info->bit_rate_info_present_flag[loop] ) ;
		bs_write1 ( s, bit_rate_pic_rate_info->pic_rate_info_present_flag[loop] ) ;;
		if( bit_rate_pic_rate_info->bit_rate_info_present_flag[loop] )
		{
			bs_write ( s, 16, bit_rate_pic_rate_info->avg_bit_rate[loop] ) ;
			bs_write ( s, 16, bit_rate_pic_rate_info->max_bit_rate[loop] ) ;
		}
		if( bit_rate_pic_rate_info->pic_rate_info_present_flag )
		{
			bs_write ( s, 2, bit_rate_pic_rate_info->constant_pic_rate_idc[loop] ) ;
			bs_write ( s, 16, bit_rate_pic_rate_info->avg_pic_rate[loop] ) ;
		}
	}
}
#endif

void x265_hrd_parameters_write ( bs_t *s, x265_hrd_t *hrd,
								int32_t b_common_inf_present_flag,
								uint32_t i_max_num_sub_layers_minus1 )
{
	int32_t i = 0, j = 0, i_nal_or_vcl ;

	if( b_common_inf_present_flag )
	{
#if !X265_L0043_TIMING_INFO
		bs_write1 ( s, hrd->b_timing_info_present_flag ? 1 : 0 ) ;
		if( hrd->b_timing_info_present_flag )
		{
			bs_write ( s, 32, hrd->i_num_units_in_tick ) ;
			bs_write ( s, 32, hrd->i_time_scale ) ;
		}
#endif
		bs_write1 ( s, hrd->b_nal_hrd_parameters_present_flag ? 1 : 0 ) ;
		bs_write1 ( s, hrd->b_vcl_hrd_parameters_present_flag ? 1 : 0  ) ;
		if( hrd->b_nal_hrd_parameters_present_flag || hrd->b_vcl_hrd_parameters_present_flag )
		{
			bs_write1 ( s, hrd->b_sub_pic_cpb_params_present_flag ? 1 : 0 ) ;
			if( hrd->b_sub_pic_cpb_params_present_flag )
			{
				bs_write ( s, 8, hrd->i_tick_divisor_minus2 ) ;
				bs_write ( s, 5, hrd->i_du_cpb_removal_delay_length_minus1 ) ;
				bs_write1 ( s, hrd->b_sub_pic_cpb_params_in_pic_timing_sei_flag ? 1 : 0 ) ;
#if X265_L0044_DU_DPB_OUTPUT_DELAY_HRD
				bs_write ( s, 5, hrd->i_dpb_output_delay_length_minus1 ) ;
#endif
			}
			bs_write ( s, 4, hrd->i_bit_rate_scale ) ;
			bs_write ( s, 4, hrd->i_cpb_size_scale ) ;
			if( hrd->b_sub_pic_cpb_params_present_flag )
			{
				bs_write ( s, 4, hrd->i_du_cpb_size_scale ) ;
			}
			bs_write ( s, 5, hrd->i_initial_cpb_removal_delay_length_minus1 ) ;
			bs_write ( s, 5, hrd->i_cpb_removal_delay_length_minus1 ) ;
			bs_write ( s, 5, hrd->i_dpb_output_delay_length_minus1 ) ;
		}
	}
	for( i = 0; i <= i_max_num_sub_layers_minus1; i ++ )
	{
		bs_write1 ( s, hrd->hrd[i].b_fixed_pic_rate_flag ? 1 : 0 ) ;
		if( ! hrd->hrd[i].b_fixed_pic_rate_flag )
		{
			bs_write1 ( s, hrd->hrd[i].b_fixed_pic_rate_within_cvs_flag ? 1 : 0 ) ;
		}
		else
		{
			hrd->hrd[i].b_fixed_pic_rate_within_cvs_flag = 1 ;
		}
		if( hrd->hrd[i].b_fixed_pic_rate_within_cvs_flag )
		{
			bs_write_ue_big ( s, hrd->hrd[i].i_pic_duration_in_tc_minus1 ) ;
		}
#if X265_L0372
		else
		{
			bs_write1 ( s, hrd->hrd[i].b_low_delay_hrd_flag ) ;
		}
		if ( ! hrd->hrd[i].b_low_delay_hrd_flag )
		{
			bs_write_ue ( s, hrd->hrd[i].i_cpb_cnt_minus1 ) ;
		}
#else
		bs_write1 ( s, hrd->hrd[i].b_low_delay_hrd_flag ? 1 : 0 ) ;
		bs_write_ue ( s, hrd->hrd[i].i_cpb_cnt_minus1 ) ;
#endif

		for( i_nal_or_vcl = 0; i_nal_or_vcl < 2; i_nal_or_vcl ++ )
		{
			if( ( ( i_nal_or_vcl == 0 ) && ( hrd->b_nal_hrd_parameters_present_flag ) ) ||
					( ( i_nal_or_vcl == 1 ) && ( hrd->b_vcl_hrd_parameters_present_flag ) ) )
			{
				for( j = 0; j <= ( hrd->hrd[i].i_cpb_cnt_minus1 ) ; j ++ )
				{
					bs_write_ue_big ( s, hrd->hrd[i].bit_rate_value_minus1[j][i_nal_or_vcl] ) ;
					bs_write_ue_big ( s, hrd->hrd[i].cpb_size_value[j][i_nal_or_vcl] ) ;
					if( hrd->b_sub_pic_cpb_params_present_flag )
					{
#if X265_L0363_DU_BIT_RATE
						bs_write_ue_big ( s, hrd->hrd[i].du_bit_rate_value[j][i_nal_or_vcl] ) ;

#endif
						bs_write_ue_big ( s, hrd->hrd[i].du_cpb_size_value[j][i_nal_or_vcl] ) ;
					}
					bs_write1 ( s, hrd->hrd[i].cbr_flag[j][i_nal_or_vcl] ) ;
				}
			}
		}
	}
}

void x265_vps_write( bs_t *s, x265_vps_t *vps )
{
	int loop = 0 ;
	uint32_t i_ops_idx = 0 ;
	int32_t b_sub_layer_ordering_info_present_flag = 1 ;
	x265_timing_info_t *timing_info = NULL ;

	bs_write ( s, 4, vps->i_vps_id ) ;
	bs_write ( s, 2, 3 ) ;
	bs_write ( s, 6, 0 ) ;
	bs_write ( s, 3, vps->i_max_t_layers - 1 ) ;
	bs_write1 ( s, vps->b_temporal_id_nesting_flag ) ;
	bs_write ( s, 16, 0xFFFF ) ;

	x265_ptl_write ( s, &vps->ptl, 1, vps->i_max_t_layers - 1 ) ;

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	x265_bit_rate_pic_rate_info_write ( s, &vps->b_bit_rate_pic_rate_info,
										0, vps->i_max_t_layers - 1 ) ;
#endif
	bs_write1 ( s, b_sub_layer_ordering_info_present_flag ) ;
	for ( loop = 0 ; loop <= vps->i_max_t_layers - 1 ; ++ loop )
	{
		bs_write_ue ( s, vps->max_dec_pic_buffering[loop] ) ;
		bs_write_ue ( s, vps->num_reorder_pics[loop] ) ;
		bs_write_ue ( s, vps->max_latency_increase[loop] ) ;
		if ( ! b_sub_layer_ordering_info_present_flag )
		{
			break ;
		}
	}

	bs_write ( s, 6, vps->i_max_nuh_reserved_zero_layer_id ) ;
	vps->i_num_op_sets = 1 ;
	bs_write_ue ( s, vps->i_num_op_sets - 1 ) ;

	for ( i_ops_idx = 0 ; loop <= (vps->i_num_op_sets - 1) ; ++ i_ops_idx )
	{
		for ( loop = 0 ; loop < vps->i_max_nuh_reserved_zero_layer_id ; ++ loop )
		{
			vps->layer_id_included_flag[i_ops_idx][loop] = 1 ;
			bs_write1 ( s, vps->layer_id_included_flag[i_ops_idx][loop] ? 1 : 0 ) ;
		}
	}


#if X265_L0043_TIMING_INFO
	timing_info = &vps->timing_info ;
	bs_write1 ( s, timing_info->b_timing_info_present_flag ) ;
	if ( timing_info->b_timing_info_present_flag )
	{
		bs_write ( s, 32, timing_info->i_num_units_in_tick ) ;
		bs_write ( s, 32, timing_info->i_time_scale ) ;
		bs_write1 ( s, timing_info->b_poc_proportional_to_timing_flag ) ;
		if ( timing_info->b_poc_proportional_to_timing_flag )
		{
			bs_write_ue_big ( s, timing_info->i_num_ticks_poc_diff_one_minus1 ) ;
		}
#endif
		vps->i_num_hrd_parameters = 0 ;
		bs_write_ue ( s, vps->i_num_hrd_parameters ) ;
		if ( vps->i_num_hrd_parameters )
		{
			create_hrd_param_buffer( vps ) ;
		}

		for ( loop = 0 ; loop < vps->i_num_hrd_parameters ; ++ loop )
		{
			vps->hrd_op_set_idx[loop] = 0 ;
			bs_write_ue ( s, vps->hrd_op_set_idx[loop] ) ;
			if ( loop > 0 )
			{
				bs_write1 ( s, vps->cprms_present_flag[loop] ? 1 : 0 ) ;
			}
			x265_hrd_parameters_write ( s, &vps->hrd_parameters[loop],
										vps->cprms_present_flag[loop],
										vps->i_max_t_layers - 1 ) ;
		}
#if X265_L0043_TIMING_INFO
	}
#endif
	bs_write1 ( s, 0 ) ;

	bs_write1 ( s, 1 ) ;
	bs_flush ( s ) ;
}

void x265_vui_init( x265_vui_t *vui, x265_param_t *param )
{
	vui->b_aspect_ratio_info_present_flag = ( -1 != param->sps.vui.i_aspect_ratio_idc ) ? 1 : 0 ;
	vui->i_aspect_ratio_idc = param->sps.vui.i_aspect_ratio_idc ;
	vui->i_sar_width = param->sps.vui.i_sar_width ;
	vui->i_sar_height = param->sps.vui.i_sar_height ;
	vui->b_overscan_info_present_flag = param->sps.vui.b_overscan_info_present_flag ;
	vui->b_overscan_appropriate_flag = param->sps.vui.b_overscan_appropriate_flag ;
	vui->b_video_signal_type_present_flag = param->sps.vui.b_video_signal_type_present_flag ;
	vui->i_video_format = param->sps.vui.i_video_format ;
	vui->b_video_full_range_flag = param->sps.vui.b_video_full_range_flag ;
	vui->b_colour_description_present_flag = param->sps.vui.b_colour_description_present_flag ;
	vui->i_colour_primaries = param->sps.vui.i_colour_primaries ;
	vui->i_transfer_characteristics = param->sps.vui.i_transfer_characteristics ;
	vui->i_matrix_coefficients = param->sps.vui.i_matrix_coefficients ;
	vui->b_chroma_loc_info_present_flag = param->sps.vui.b_chroma_loc_info_present_flag ;
	vui->i_chroma_sample_loc_type_top_field = param->sps.vui.i_chroma_sample_loc_type_top_field ;
	vui->i_chroma_sample_loc_type_bottom_field = param->sps.vui.i_chroma_sample_loc_type_bottom_field ;
	vui->b_neutral_chroma_indication_flag = param->sps.vui.b_neutral_chroma_indication_flag ;
	set_window ( &vui->default_display_window,
				param->sps.vui.i_def_disp_win_left_offset,
				param->sps.vui.i_def_disp_win_right_offset,
				param->sps.vui.i_def_disp_win_top_offset,
				param->sps.vui.i_def_disp_win_bottom_offset ) ;
	vui->b_frame_field_info_present_flag = param->sps.vui.b_frame_field_info_present_flag ;
	vui->b_field_seq_flag = 0 ;
	vui->b_hrd_parameters_present_flag = 0 ;

#if X265_L0043_TIMING_INFO
	vui->timing_info.b_poc_proportional_to_timing_flag = param->sps.vui.b_poc_proportional_to_timing_flag ;
	vui->timing_info.i_num_ticks_poc_diff_one_minus1 = param->sps.vui.i_num_ticks_poc_diff_one_minus1 ;
#else
	vui->b_poc_proportional_to_timing_flag = param->sps.vui.b_poc_proportional_to_timing_flag ;
	vui->i_num_ticks_poc_diff_one_minus1 = param->sps.vui.i_num_ticks_poc_diff_one_minus1 ;
#endif

	vui->b_bitstream_restriction_flag = param->sps.vui.b_bitstream_restriction_flag ;
	vui->b_tiles_fixed_structure_flag = param->sps.vui.b_tiles_fixed_structure_flag ;
	vui->b_motion_vectors_over_pic_boundaries_flag = param->sps.vui.b_motion_vectors_over_pic_boundaries_flag ;
	vui->i_min_spatial_segmentation_idc = param->sps.vui.i_min_spatial_segmentation_idc ;
	vui->i_max_bytes_per_pic_denom = param->sps.vui.i_max_bytes_per_pic_denom ;
	vui->i_max_bits_per_min_cu_denom = param->sps.vui.i_max_bits_per_min_cu_denom ;
	vui->i_log2_max_mv_length_horizontal = param->sps.vui.i_log2_max_mv_length_horizontal ;
	vui->i_log2_max_mv_length_vertical = param->sps.vui.i_log2_max_mv_length_vertical ;
}

void x265_vui_write ( bs_t *s, x265_vui_t *vui, x265_sps_t *sps )
{
	x265_window_t *default_display_window = NULL ;
	x265_timing_info_t *timing_info ;

	bs_write1 ( s, vui->b_aspect_ratio_info_present_flag ? 0 : 1 ) ;
	if ( vui->b_aspect_ratio_info_present_flag )
	{
		bs_write ( s, 8, vui->i_aspect_ratio_idc ) ;
		if ( 255 == vui->i_aspect_ratio_idc )
		{
			bs_write ( s, 16, vui->i_sar_width ) ;
			bs_write ( s, 16, vui->i_sar_height ) ;
		}
	}

	bs_write1 ( s, vui->b_overscan_info_present_flag ? 0 : 1 ) ;
	if ( vui->b_overscan_info_present_flag )
	{
		bs_write1 ( s, vui->b_overscan_appropriate_flag ) ;
	}
	bs_write1 ( s, vui->b_video_signal_type_present_flag ? 0 : 1 ) ;
	if ( vui->b_video_signal_type_present_flag )
	{
		bs_write ( s, 3, vui->i_video_format ) ;
		bs_write1 ( s, vui->b_video_full_range_flag ? 0 : 1 ) ;
		bs_write1 ( s, vui->b_colour_description_present_flag ? 0 : 1 ) ;
		if ( vui->b_colour_description_present_flag )
		{
			bs_write ( s, 8, vui->i_colour_primaries ) ;
			bs_write ( s, 8, vui->i_transfer_characteristics ) ;
			bs_write ( s, 8, vui->i_matrix_coefficients ) ;
		}
	}

	bs_write1 ( s, vui->b_chroma_loc_info_present_flag ? 0 : 1 ) ;
	if ( vui->b_chroma_loc_info_present_flag )
	{
		bs_write_ue ( s, vui->i_chroma_sample_loc_type_top_field ) ;
		bs_write_ue ( s, vui->i_chroma_sample_loc_type_bottom_field ) ;
	}

	bs_write1 ( s, vui->b_neutral_chroma_indication_flag ? 0 : 1 ) ;
	bs_write1 ( s, vui->b_field_seq_flag ? 0 : 1 ) ;
	// not currently supported
	assert ( 0 == vui->b_field_seq_flag ) ;

	bs_write1 ( s, vui->b_frame_field_info_present_flag ? 0 : 1 ) ;

	default_display_window = &vui->default_display_window ;
	bs_write1 ( s, default_display_window->b_enabled_flag ) ;
	if ( default_display_window->b_enabled_flag )
	{
		bs_write_ue_big ( s, default_display_window->i_win_left_offset ) ;
		bs_write_ue_big ( s, default_display_window->i_win_right_offset ) ;
		bs_write_ue_big ( s, default_display_window->i_win_top_offset ) ;
		bs_write_ue_big ( s, default_display_window->i_win_bottom_offset ) ;
	}

#if X265_L0043_TIMING_INFO
	timing_info = &vui->timing_info ;
	bs_write1 ( s, timing_info->b_timing_info_present_flag ) ;
	if ( timing_info->b_timing_info_present_flag )
	{
		bs_write ( s, 32, timing_info->i_num_units_in_tick ) ;
		bs_write ( s, 32, timing_info->i_time_scale ) ;
		bs_write1 ( s, timing_info->b_poc_proportional_to_timing_flag ) ;
		if ( timing_info->b_poc_proportional_to_timing_flag )
		{
			bs_write_ue_big ( s, timing_info->i_num_ticks_poc_diff_one_minus1 ) ;
		}
#endif
		bs_write1 ( s, vui->b_hrd_parameters_present_flag ) ;
		if ( vui->b_hrd_parameters_present_flag )
		{
			x265_hrd_parameters_write ( s, &vui->hrd_parameters, 1, sps->i_max_t_layers - 1 ) ;
		}
#if X265_L0043_TIMING_INFO
	}
#endif
#if !X265_L0043_TIMING_INFO
	bs_write1 ( s, vui->b_poc_proportional_to_timing_flag ) ;
	if ( vui->b_poc_proportional_to_timing_flag && vui->hrd_parameters.b_timing_info_present_flag )
	{
		bs_write_ue_big ( s, vui->i_num_ticks_poc_diff_one_minus1 ) ;
	}
#endif

	bs_write1 ( s, vui->b_bitstream_restriction_flag ) ;
	if ( vui->b_bitstream_restriction_flag )
	{
		bs_write1 ( s, vui->b_tiles_fixed_structure_flag ) ;
		bs_write1 ( s, vui->b_motion_vectors_over_pic_boundaries_flag ) ;
		bs_write1 ( s, vui->b_restricted_ref_pic_lists_flag ) ;
#if X265_L0043_MSS_IDC
		bs_write_ue ( s, vui->i_min_spatial_segmentation_idc ) ;
#else
		bs_write ( s, 8, vui->i_min_spatial_segmentation_idc ) ;
#endif
		bs_write_ue_big ( s, vui->i_max_bytes_per_pic_denom ) ;
		bs_write_ue_big ( s, vui->i_max_bits_per_min_cu_denom ) ;
		bs_write_ue ( s, vui->i_log2_max_mv_length_horizontal ) ;
		bs_write_ue ( s, vui->i_log2_max_mv_length_vertical ) ;
	}
}

void x265_rps_list_init( x265_rps_list_t *rps_list, x265_param_t *param )
{
	int32_t i = 0, j = 0, k = 0 ;
	x265_reference_picture_set_t *rps = NULL ;
	x265_reference_picture_set_t *rps_ref = NULL ;
	// temporary variable
	x265_reference_picture_set_t rps_temp ;
	gop_entry_t *gop_entry = NULL ;
	int i_num_neg = 0, i_num_pos = 0 ;
    int32_t i_delta_rps = 0 ;
    int32_t i_delta_poc = 0 ;
    int32_t i_num_ref_delta_poc = 0 ;
    int32_t i_count = 0 ;
    int32_t i_ref_delta_poc = 0 ;


	for ( i = 0; i < param->gop.i_gop_size + param->gop.i_extraRPSs ; ++ i )
	{
		gop_entry = &param->gop.gop_list[i] ;
	    rps = &rps_list->reference_picture_sets[i] ;
	    rps->i_number_of_pictures = gop_entry->i_num_ref_pics ;
	    rps->i_num_ref_idc = gop_entry->i_num_ref_idc ;
	    i_num_neg = 0 ;
	    i_num_pos = 0 ;
	    for( j = 0; j < gop_entry->i_num_ref_pics ; ++ j )
	    {
	    	rps->delta_poc[j] = gop_entry->reference_pics[j] ;
	    	rps->used[j] = gop_entry->used_by_curr_pic[j] ;
	    	if ( gop_entry->reference_pics[j] > 0 )
	    	{
	    		++ i_num_pos ;
	    	}
	    	else
	    	{
	    		++ i_num_neg ;
	    	}
	    }
	    rps->i_number_of_negative_pictures = i_num_neg ;
	    rps->i_number_of_positive_pictures = i_num_pos ;

#if X265_AUTO_INTER_RPS
	    rps->b_inter_rps_prediction = (gop_entry->i_inter_rps_prediction > 0) ? 1 : 0 ;
	    rps->i_delta_ridx_minus1 = 0 ;
	    // get the reference RPS
	    rps_ref = &rps_list->reference_picture_sets[i-1] ;

	    // Automatic generation of the inter RPS idc based on the RIdx provided.
	    if (gop_entry->i_inter_rps_prediction == 2)
	    {
	    	// the ref POC - current POC
	    	i_delta_rps = param->gop.gop_list[i-1].i_poc - gop_entry->i_poc ;
	    	i_num_ref_delta_poc = rps_ref->i_number_of_pictures ;

	           // set delta RPS
	    	rps->i_delta_rps = i_delta_rps ;
	    	// set the numRefIdc to the number of pictures in the reference RPS + 1.
	    	rps->i_num_ref_idc = i_num_ref_delta_poc + 1 ;
	    	i_count = 0 ;
	    	// cycle through pics in reference RPS.
	    	for ( j = 0 ; j <= i_num_ref_delta_poc ; ++ j )
	    	{
	    		// if it is the last decoded picture, set i_ref_delta_poc = 0
	    		i_ref_delta_poc = (j<i_num_ref_delta_poc) ? rps_ref->delta_poc[j] : 0 ;
	    		rps->ref_idc[j] = 0 ;
	    		// cycle through pics in current RPS.
	    		for ( k = 0; k < rps->i_number_of_pictures ; ++ k )
	    		{
	    			// if the current RPS has a same picture as the reference RPS.
	    			if (rps->delta_poc[k] == ( i_ref_delta_poc + i_delta_rps))
	    			{
	    				rps->ref_idc[j] = rps->used[k] ? 1 : 2 ;
	    				i_count ++ ;
	    				break;
	    			}
	    		}
	    	}
	    	if ( i_count != rps->i_number_of_pictures )
	    	{
	    		fprintf ( stderr, "Warning: Unable fully predict all delta POCs using the reference RPS index given in the config file.  Setting int32_ter RPS to false for this RPS.\n" ) ;
	    		rps->b_inter_rps_prediction = 0 ;
	    	}
	    }
	    // inter RPS idc based on the RefIdc values provided in config file.
	    else if ( gop_entry->i_inter_rps_prediction == 1 )
	    {
	    	rps->i_delta_rps = gop_entry->i_delta_rps ;
	    	rps->i_num_ref_idc = gop_entry->i_num_ref_idc ;
	    	for ( j = 0; j < gop_entry->i_num_ref_idc ; ++ j )
	    	{
	    		rps->ref_idc[j] = gop_entry->i_ref_idc[j] ;
	    	}
#if X265_WRITE_BACK
	    	// the folowing code overwrite the i_delta_poc and Used by current values read from the config file with the ones
	    	// computed from the RefIdc.  A warning is printed if they are not identical.
	    	i_num_neg = 0 ;
	    	i_num_pos = 0 ;
	    	init_reference_picture_set ( &rps_temp ) ;

	    	for ( j = 0; j < gop_entry->i_num_ref_idc; j++ )
	    	{
	    		if (gop_entry->i_ref_idc[j])
	    		{
	    			i_delta_poc = gop_entry->i_delta_rps + ((j < rps_ref->i_number_of_pictures)? rps_ref->delta_poc[j] : 0) ;
	    			rps_temp.delta_poc[(i_num_neg+i_num_pos)] = i_delta_poc ;
	    			rps_temp.used[(i_num_neg+i_num_pos)] = gop_entry->i_ref_idc[j] == 1 ? 1 : 0 ;
	    			if (i_delta_poc<0)
	    			{
	    				i_num_neg++;
	    			}
	    			else
	    			{
	    				i_num_pos++;
	    			}
	    		}
	    	}
	    	if (i_num_neg != rps->i_number_of_negative_pictures)
	    	{
	    		fprintf ( stderr, "Warning: number of negative pictures in RPS is different between intra and inter RPS specified in the config file.\n" ) ;
	    		rps->i_number_of_negative_pictures = i_num_neg ;
	    		rps->i_number_of_positive_pictures = i_num_neg+i_num_pos ;
	    	}
	    	if (i_num_pos != rps->i_number_of_positive_pictures)
	    	{
	    		fprintf ( stderr, "Warning: number of positive pictures in RPS is different between intra and inter RPS specified in the config file.\n" ) ;
	    		rps->i_number_of_positive_pictures = i_num_pos ;
	    		rps->i_number_of_positive_pictures = i_num_neg + i_num_pos ;
	    	}
	    	rps_temp.i_number_of_pictures = i_num_neg + i_num_pos ;
	    	rps_temp.i_number_of_negative_pictures = i_num_neg ;
	    	// sort the created delta POC before comparing
	    	sort_delta_poc ( &rps_temp ) ;
	    	// check if Delta POC and Used are the same
	    	// print warning if they are not.
	    	for ( j = 0; j < gop_entry->i_num_ref_idc; j++ )
	    	{
	    		if (rps_temp.delta_poc[j] != rps->delta_poc[j])
				{
					fprintf ( stderr, "Warning: delta POC is different between intra RPS and inter RPS specified in the config file.\n" ) ;
					rps->delta_poc[j] = rps_temp.delta_poc[j] ;
				}
	    		if (rps_temp.used[j] != rps->used[j])
	    		{
	    			fprintf ( stderr, "Warning: Used by Current in RPS is different between intra and inter RPS specified in the config file.\n" ) ;
	    			rps->used[j] = rps_temp.used[j] ;
	    		}
	    	}
#endif
	    }
#else
	    rps->b_inter_rps_prediction = gop_entry->i_inter_rps_prediction ;
	    if (gop_entry->i_inter_rps_prediction)
	    {
	    	rps->i_delta_ridx_minus1 = 0 ;
	    	rps->i_delta_rps = gop_entry->i_delta_rps ;
	    	rps->i_num_ref_idc = gop_entry->i_num_ref_idc ;
	    	for ( j = 0; j < gop_entry->i_num_ref_idc; j++ )
	    	{
	    		rps->ref_idc [j] = gop_entry->i_ref_idc[j] ;
	    	}
#if X265_WRITE_BACK
	    	// the folowing code overwrite the i_delta_poc and Used by current values read from the config file with the ones
	    	// computed from the RefIdc.  This is not necessary if both are identical. Currently there is no check to see if they are identical.
	    	i_num_neg = 0;
	    	i_num_pos = 0;
	    	rps_ref = &rps_list->reference_picture_sets[i-1] ;

	    	for ( j = 0; j < gop_entry->i_num_ref_idc; j++ )
	    	{
	    		if (gop_entry->i_ref_idc[j])
	    		{
	    			i_delta_poc = gop_entry->i_delta_rps + ((j < rps_ref->i_number_of_pictures)? rps_ref->delta_poc[j] : 0) ;
	    			rps->delta_poc[(i_num_neg+i_num_pos)] = i_delta_poc ;
	    			rps->used[i_num_neg+i_num_pos] = gop_entry->i_ref_idc[j] == 1 ? 1 : 0 ;
	    			if (i_delta_poc < 0 )
	    			{
	    				i_num_neg++;
	    			}
	    			else
	    			{
	    				i_num_pos++;
	    			}
	    		}
	    	}
	    	rps->i_number_of_negative_pictures = i_num_neg ;
	    	rps->i_number_of_positive_pictures = i_num_pos ;
	    	sort_delta_poc ( rps ) ;
#endif
	    }
#endif //INTER_RPS_AUTO
	}
}

void x265_short_term_ref_pic_set_write ( bs_t *s,
										x265_reference_picture_set_t* rps,
										int32_t b_called_from_slice_header,
										int32_t i_idx)
{
	int32_t loop = 0 ;
	int32_t i_delta_rps = 0 ;
	int32_t i_ref_idc = 0 ;
	int32_t i_prev = 0 ;
	if (i_idx > 0)
	{
		// inter_RPS_prediction_flag
		bs_write1 ( s, rps->b_inter_rps_prediction ) ;
	}
	if (rps->b_inter_rps_prediction)
	{
		i_delta_rps = rps->i_delta_rps;
		if (b_called_from_slice_header)
		{
			// delta index of the Reference Picture Set used for prediction minus 1
			bs_write_ue ( s, rps->i_delta_ridx_minus1 ) ;
		}

		//delta_rps_sign
		bs_write ( s, 1, (i_delta_rps >=0 ? 0: 1) ) ;
		// absolute delta RPS minus 1
		bs_write_ue ( s, abs(i_delta_rps) - 1 ) ;

		for( loop = 0 ; loop < rps->i_num_ref_idc ; ++ loop )
		{
			i_ref_idc = rps->ref_idc[loop] ;
			//first bit is "1" if Idc is 1
			bs_write ( s, 1, (i_ref_idc==1? 1: 0) ) ;
			if ( i_ref_idc != 1 )
			{
				//second bit is "1" if Idc is 2, "0" otherwise.
				bs_write ( s, 1, i_ref_idc>>1  ) ;
			}
		}
	}
	else
	{
		bs_write_ue ( s, rps->i_number_of_negative_pictures ) ;
		bs_write_ue ( s, rps->i_number_of_positive_pictures ) ;
		i_prev = 0;
		for( loop = 0 ; loop < rps->i_number_of_negative_pictures ; ++ loop )
		{
			bs_write_ue ( s, i_prev - rps->delta_poc[loop] - 1 ) ;
			i_prev = rps->delta_poc[loop] ;
			bs_write1 ( s, rps->used[loop] ) ;
		}
		i_prev = 0;
		for( loop = rps->i_number_of_negative_pictures ;
				loop < rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures ;
				++ loop )
		{
			bs_write_ue ( s, rps->delta_poc[loop] - i_prev - 1 ) ;
			i_prev = rps->delta_poc[loop] ;
			bs_write1 ( s, rps->used[loop] ) ;
		}
	}
}

void print_rps_list ( x265_sps_t *sps )
{
	FILE *file = NULL ;
	int loop = 0 ;
	x265_rps_list_t * rps_list = NULL ;
	x265_reference_picture_set_t * rps = NULL ;

	file = fopen ( "RPSList2", "wb" ) ;
	rps_list = &sps->rps_list ;

	fprintf ( file, "RPS List Size = %d\n", rps_list->i_number_of_reference_picture_sets ) ;
	for( loop = 0; loop < rps_list->i_number_of_reference_picture_sets; ++ loop )
	{
		fprintf ( file, "RPS %d\n", loop + 1 ) ;
		rps = &rps_list->reference_picture_sets[loop] ;
		print_rps ( rps, file ) ;
	}

	fclose ( file ) ;
}

void x265_scaling_list_init( x265_t *h, x265_sps_t *sps, x265_param_t *param )
{
    if ( SCALING_LIST_OFF == param->sps.i_use_scaling_list_id )
    {
    	sps->b_scaling_list_present_flag = 0 ;
    }
    else if ( SCALING_LIST_DEFAULT == param->sps.i_use_scaling_list_id )
    {
    	enc_top_set_default_scaling_list ( h ) ;
    	sps->b_scaling_list_present_flag = 0 ;
    }
    else if ( SCALING_LIST_FILE_READ == param->sps.i_use_scaling_list_id )
    {
    	if ( parse_scaling_list ( &h->scaling_list, param->sps.psz_cqm_file ) )
    	{
        	enc_top_set_default_scaling_list ( h ) ;
        	sps->b_scaling_list_present_flag = 0 ;
    	}
    	else
    	{
    		if ( check_default_scaling_list ( &h->scaling_list ) )
    		{
    			sps->b_scaling_list_present_flag = 1 ;
    		}
    		else
    		{
    			sps->b_scaling_list_present_flag = 0 ;
    		}
    	}
    	check_dc_of_matrix ( &h->scaling_list ) ;
    }
}

void x265_x_scaling_list_write ( x265_t *h,
								bs_t *s,
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

	i_coef_num = x265_scaling_list_get_coef_num ( i_size_id ) ;
	src = scaling_list->scaling_list_coef[i_size_id][i_list_id] ;
	scan = (i_size_id == 0) ? h->scan.sig_last_scan [ SCAN_DIAG ] [ 1 ]
	                        : h->scan.sig_last_scan_cg_32x32 ;
    if( i_size_id > SCALING_LIST_8x8 )
    {
    	bs_write_se ( s, scaling_list->scaling_list_dc[i_size_id][i_list_id] - 8 ) ;
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
    	bs_write_se ( s, i_data ) ;
    }
}
void x265_scaling_list_write ( x265_t *h,
								bs_t *s,
								x265_scaling_list_t *scaling_list )
{
	uint32_t i_size_id = 0 ;
	uint32_t i_list_id = 0 ;
	int32_t b_scaling_list_pred_mode_flag ;

	for ( i_size_id = 0; i_size_id < SCALING_LIST_SIZE_NUM ; ++ i_size_id )
	{
		for ( i_list_id = 0; i_list_id < x265_scaling_list_get_scaling_list_num(i_size_id) ; ++ i_list_id )
		{
			b_scaling_list_pred_mode_flag = check_pred_mode ( scaling_list,
															i_size_id,
															i_list_id ) ;
			bs_write1 ( s, b_scaling_list_pred_mode_flag ) ;
			// Copy Mode
	        if ( ! b_scaling_list_pred_mode_flag )
	        {
	        	bs_write_se ( s, i_list_id - scaling_list->ref_matrix_id[i_size_id][i_list_id] ) ;
	        }
	        else// DPCM Mode
	        {
	        	x265_x_scaling_list_write ( h, s, scaling_list, i_size_id, i_list_id ) ;
	        }
		}
    }
}

void x265_sps_deinit( x265_sps_t *sps )
{
	deinit_sps ( sps ) ;
}

int x265_sps_init( x265_sps_t *sps, x265_t *h, x265_param_t *param )
{
	int loop = 0 ;
	x265_profile_tier_level_t *profile_tier_level = NULL ;

	if ( init_sps ( sps ) )
	{
		goto fail ;
	}

	profile_tier_level = & sps->ptl.p_general_ptl ;
	profile_tier_level->i_level_idc = param->sps.i_level ;
	profile_tier_level->b_tier_flag = param->sps.i_level_tier ;
	profile_tier_level->i_profile_idc = param->sps.i_profile ;
	profile_tier_level->profile_compatibility_flag[param->sps.i_profile] = 1 ;

#if X265_L0046_CONSTRAINT_FLAGS
	profile_tier_level->b_progressive_source_flag = param->sps.b_progressive_source_flag ;
	profile_tier_level->b_interlaced_source_flag = param->sps.b_interlaced_source_flag ;
	profile_tier_level->b_non_packed_constraint_flag = param->sps.b_non_packed_constraint_flag ;
	profile_tier_level->b_frame_only_constraint_flag = param->sps.b_frame_only_constraint_flag ;
#endif

	if ( PROFILE_MAIN10 == profile_tier_level->i_profile_idc
		&& 8 == param->sps.i_bit_depth_y
		&& 8 == param->sps.i_bit_depth_c )
	{
		profile_tier_level->profile_compatibility_flag[PROFILE_MAIN] = 1 ;
	}

	if ( PROFILE_MAIN == profile_tier_level->i_profile_idc )
	{
		profile_tier_level->profile_compatibility_flag[PROFILE_MAIN10] = 1 ;
	}

	sps->i_pic_width_in_luma_samples = param->i_width ;
	sps->i_pic_height_in_luma_samples = param->i_height ;
	set_window ( &sps->conformance_window,
				param->sps.i_conf_left,
				param->sps.i_conf_right,
				param->sps.i_conf_top,
				param->sps.i_conf_bottom ) ;
    sps->i_max_cu_width = h->cu.pic.i_max_cu_width ;
    sps->i_max_cu_height = h->cu.pic.i_max_cu_height ;
    sps->i_max_cu_depth = h->cu.pic.i_total_depth ;
    sps->i_add_cu_depth = h->cu.pic.i_add_cu_depth ;
	sps->i_min_tr_depth = 0 ;
	sps->i_max_tr_depth = 1 ;

	sps->b_use_pcm = param->sps.b_use_pcm ;
	sps->i_pcm_log2_min_size = param->pcm.i_pcm_log2_min_size ;
	sps->i_pcm_log2_max_size = param->pcm.i_pcm_log2_max_size ;

	sps->i_quadtree_tu_log2_max_size = param->sps.i_quadtree_tu_log2_max_size ;
	sps->i_quadtree_tu_log2_min_size = param->sps.i_quadtree_tu_log2_min_size ;
	sps->i_quadtree_tu_max_depth_inter = param->sps.i_quadtree_tu_max_depth_inter ;
	sps->i_quadtree_tu_max_depth_intra = param->sps.i_quadtree_tu_max_depth_intra ;

	sps->b_tmvp_flags_present = 0 ;
	sps->b_use_lossless = param->sps.b_use_lossless ;
	sps->i_max_tr_size = (1 << param->sps.i_quadtree_tu_log2_max_size) ;
	sps->b_use_l_comb = param->b_use_list_combination ;

    for ( loop = 0 ; loop < h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth ; ++ loop )
    {
    	sps->amp_acc[loop] = param->b_enable_amp ;
    }
	sps->b_use_amp = param->b_enable_amp ;
    for ( loop = h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth ;
    		loop < h->cu.pic.i_total_depth ; ++ loop )
    {
    	sps->amp_acc[loop] = 0 ;
    }
    sps->i_bit_depth_y = h->cu.pic.i_bit_depth_y ;
    sps->i_bit_depth_c = h->cu.pic.i_bit_depth_c ;
    sps->i_qp_bd_offset_y = 6 * (h->cu.pic.i_bit_depth_y - 8) ;
    sps->i_qp_bd_offset_c = 6 * (h->cu.pic.i_bit_depth_c - 8) ;

	sps->b_use_sao = param->sps.b_use_sao ;
	sps->i_max_t_layers = param->sps.i_max_temp_layer ;
	sps->b_temporal_id_nesting_flag = (1 == param->sps.i_max_temp_layer) ? 1 : 0 ;

	for ( loop = 0 ; loop < sps->i_max_t_layers ; ++ loop )
	{
		sps->max_dec_pic_buffering[loop] = param->gop.max_dec_pic_buffering[loop] ;
		sps->num_reorder_pics[loop] = param->gop.num_reorder_pics[loop] ;
	}
    sps->i_pcm_bit_depth_luma = h->cu.pic.i_pcm_bit_depth_luma ;
    sps->i_pcm_bit_depth_chroma = h->cu.pic.i_pcm_bit_depth_chroma ;
	sps->b_pcm_filter_disable_flag = param->pcm.b_pcm_filter_disable_flag ;
	sps->b_scaling_list_enabled_flag = ( 0 == param->sps.i_use_scaling_list_id) ? 0 : 1 ;
	sps->b_use_strong_intra_smoothing = param->sps.b_use_strong_intra_smoothing ;
	sps->b_vui_parameters_present_flag = param->sps.b_vui_parameters_present_flag ;
	if ( sps->b_vui_parameters_present_flag )
	{
		x265_vui_init ( &sps->vui_parameters, param ) ;
	}

	sps_create_rps_list ( sps, param->gop.i_gop_size + param->gop.i_extraRPSs ) ;
	x265_rps_list_init( &sps->rps_list, param ) ;
    //	print_rps_list (sps) ;

    x265_scaling_list_init( h, sps, param ) ;
	if ( 2 == param->sps.i_tmvp_mode_id )
	{
		sps->b_tmvp_flags_present = 1 ;
	}
	else if ( 1 == param->sps.i_tmvp_mode_id )
	{
		sps->b_tmvp_flags_present = 1 ;
	}
	else
	{
		sps->b_tmvp_flags_present = 0 ;
	}
	sps->i_num_long_term_ref_pic_sps = 0 ;

	return 0 ;

fail:
	x265_sps_deinit ( sps ) ;
	return -1 ;
}

void x265_sps_write( x265_t *h, bs_t *s, x265_sps_t *sps )
{
	int32_t loop = 0 ;
	int32_t b_sub_layer_ordering_info_present_flag = 0 ;
	x265_window_t *conformance_window = NULL ;
	uint32_t i_min_cu_size = 0 ;
	uint32_t log2_min_cu_size = 0;
	x265_rps_list_t *rps_list = NULL ;
	x265_reference_picture_set_t *rps = NULL ;

	bs_write ( s, 4, sps->i_vps_id ) ;
	bs_write ( s, 3, sps->i_max_t_layers - 1 ) ;
	bs_write1 ( s, sps->b_temporal_id_nesting_flag ? 1 : 0 ) ;
	x265_ptl_write ( s, &sps->ptl, 1, sps->i_max_t_layers - 1 ) ;
	bs_write_ue ( s, sps->i_sps_id ) ;
	bs_write_ue ( s, sps->i_chroma_format_idc ) ;
	assert ( sps->i_chroma_format_idc == 1) ;
	// in the first version chroma_format_idc can only be equal to 1 (4:2:0)
	if( sps->i_chroma_format_idc == 3 )
	{
		bs_write1 ( s, 0 ) ;
	}

	bs_write_ue_big ( s, sps->i_pic_width_in_luma_samples ) ;
	bs_write_ue_big ( s, sps->i_pic_height_in_luma_samples ) ;
	conformance_window = &sps->conformance_window ;

	bs_write1 ( s, conformance_window->b_enabled_flag ? 1 : 0 ) ;
	if ( conformance_window->b_enabled_flag )
	{
		bs_write_ue_big ( s, conformance_window->i_win_left_offset / 2 ) ;
		bs_write_ue_big ( s, conformance_window->i_win_right_offset / 2 ) ;
		bs_write_ue_big ( s, conformance_window->i_win_top_offset / 2 ) ;
		bs_write_ue_big ( s, conformance_window->i_win_bottom_offset / 2 ) ;
	}

	bs_write_ue ( s, sps->i_bit_depth_y - 8 ) ;
	bs_write_ue ( s, sps->i_bit_depth_c - 8 ) ;
	bs_write_ue ( s, sps->i_bits_for_poc - 4 ) ;

	b_sub_layer_ordering_info_present_flag = 1 ;
	bs_write1 ( s, b_sub_layer_ordering_info_present_flag ) ;
	for ( loop = 0 ; loop <= sps->i_max_t_layers - 1 ; ++ loop )
	{
		bs_write_ue ( s, sps->max_dec_pic_buffering[loop] ) ;
		bs_write_ue ( s, sps->num_reorder_pics[loop] ) ;
		bs_write_ue ( s, sps->max_latency_increase[loop] ) ;
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

	bs_write_ue ( s, log2_min_cu_size - 3 ) ;
	bs_write_ue ( s, sps->i_max_cu_depth - sps->i_add_cu_depth ) ;
	bs_write_ue ( s, sps->i_quadtree_tu_log2_min_size - 2 ) ;
	bs_write_ue ( s, sps->i_quadtree_tu_log2_max_size - sps->i_quadtree_tu_log2_min_size ) ;
	bs_write_ue ( s, sps->i_quadtree_tu_max_depth_inter - 1 ) ;
	bs_write_ue ( s, sps->i_quadtree_tu_max_depth_intra - 1 ) ;
	bs_write1 ( s, sps->b_scaling_list_enabled_flag ? 1 : 0 ) ;
	if ( sps->b_scaling_list_enabled_flag )
	{
		bs_write1 ( s, sps->b_scaling_list_present_flag ? 1 : 0 ) ;
		if ( sps->b_scaling_list_present_flag )
		{
			x265_scaling_list_write ( h, s, &h->scaling_list ) ;
		}
	}
	bs_write1 ( s, sps->b_use_amp ? 1 : 0 ) ;
	bs_write1 ( s, sps->b_use_sao ? 1 : 0 ) ;
	bs_write1 ( s, sps->b_use_pcm ? 1 : 0 ) ;
	if ( sps->b_use_pcm )
	{
		bs_write ( s, 4, sps->i_pcm_bit_depth_luma - 1 ) ;
		bs_write ( s, 4, sps->i_pcm_bit_depth_chroma - 1 ) ;
		bs_write_ue ( s, sps->i_pcm_log2_min_size - 3 ) ;
		bs_write_ue ( s, sps->i_pcm_log2_max_size - sps->i_pcm_log2_min_size ) ;
		bs_write1 ( s, sps->b_pcm_filter_disable_flag ? 1 : 0 ) ;
	}

	assert( sps->i_max_t_layers > 0 ) ;

	rps_list = &sps->rps_list ;

	bs_write_ue ( s,rps_list->i_number_of_reference_picture_sets ) ;
	for ( loop = 0 ; loop < rps_list->i_number_of_reference_picture_sets ; ++ loop )
	{
	    rps = &rps_list->reference_picture_sets[loop] ;
	    x265_short_term_ref_pic_set_write ( s, rps, 0, loop ) ;
	}
	bs_write1 ( s, sps->b_long_term_refs_present ? 1 : 0 ) ;
	if (sps->b_long_term_refs_present)
	{
		bs_write_ue ( s, sps->i_num_long_term_ref_pic_sps ) ;
		for ( loop = 0; loop < sps->i_num_long_term_ref_pic_sps ; ++ loop )
		{
			bs_write ( s, sps->i_bits_for_poc, sps->lt_ref_pic_poc_lsb_sps[loop] ) ;
			bs_write1 ( s, sps->used_by_curr_pic_lt_sps_flag[loop] ) ;
		}
	}

	bs_write1 ( s, sps->b_tmvp_flags_present ? 1 : 0 ) ;
	bs_write1 ( s, sps->b_use_strong_intra_smoothing ? 1 : 0 ) ;
	bs_write1 ( s, sps->b_vui_parameters_present_flag ? 1 : 0 ) ;
	if (sps->b_vui_parameters_present_flag)
	{
		x265_vui_write ( s, &sps->vui_parameters, sps ) ;
	}

	bs_write1 ( s, 0 ) ;

	bs_write1 ( s, 1 ) ;
	bs_flush ( s ) ;
}


void x265_pps_deinit( x265_pps_t *pps )
{
	deinit_pps ( pps ) ;
}

int x265_pps_init( x265_pps_t *pps, x265_sps_t *sps, x265_t *h, x265_param_t *param )
{
    int32_t i_lowest_qp = 0 ;
    int32_t i_tiles_count = 0 ;
	int32_t b_use_dqp = 0 ;
	int32_t loop = 0 ;
    int32_t histogram[X265_MAX_NUM_REF + 1] ;
    int32_t i_max_hist =-1 ;
    int32_t i_best_pos =0 ;

    if ( init_pps ( pps ) )
    {
    	goto fail ;
    }

    i_lowest_qp = - 6*(h->cu.pic.i_bit_depth_y - 8) ; // XXX: check
    if ( (0 == param->rdo.i_max_delta_qp)
    	&& (param->i_qp == i_lowest_qp)
    	&& (0 != param->sps.b_use_lossless) )
    {
    	param->b_use_adaptive_qp = 0 ;
    }
    if ( 0 == h->param.i_slice_mode )
    {
    	h->param.deblock.b_lf_cross_slice_boundary_flag = 1 ;
    }
    i_tiles_count = (h->param.pps.i_num_rows_minus_1 + 1)
					* (h->param.pps.i_num_columns_minus_1 + 1) ;
    if ( 1 == i_tiles_count )
    {
    	h->param.deblock.b_lf_cross_tile_boundary_flag = 1 ;
    }

    pps->b_constrained_intra_pred = param->b_use_constrained_intra_pred ;

    b_use_dqp = (param->rdo.i_max_cu_delta_qp_depth > 0) ? 1 : 0 ;
    i_lowest_qp = - sps->i_qp_bd_offset_y ;
    if ( param->sps.b_use_lossless )
    {
        if ((0 == param->rdo.i_max_cu_delta_qp_depth)
        	&& (0 == param->rdo.i_max_delta_qp)
        	&& ( i_lowest_qp == param->i_qp ) )
        {
        	b_use_dqp = 0 ;
        }
        else
        {
        	b_use_dqp = 1 ;
        }
    }
    else
    {
        if (0 == b_use_dqp)
        {
        	if((0 != param->rdo.i_max_delta_qp) || param->b_use_adaptive_qp)
        	{
        		b_use_dqp = 1 ;
        	}
        }
    }
    if ( b_use_dqp )
    {
    	pps->b_use_dqp = 1 ;
    	pps->i_max_cu_dqp_depth = param->rdo.i_max_cu_delta_qp_depth ;
    	pps->i_min_cu_dqp_size = (sps->i_max_cu_width >> pps->i_max_cu_dqp_depth) ;
    }
    else
    {
    	pps->b_use_dqp = 0 ;
    	pps->i_max_cu_dqp_depth = 0;
    	pps->i_min_cu_dqp_size = (sps->i_max_cu_width >> pps->i_max_cu_dqp_depth) ;
    }

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
    if ( param->rc.b_enable_rate_control )
    {
    	pps->b_use_dqp = 1 ;
    	pps->i_max_cu_dqp_depth = 0;
    	pps->i_min_cu_dqp_size = (sps->i_max_cu_width >> pps->i_max_cu_dqp_depth) ;
    }
#endif

    pps->i_chroma_cb_qp_offset = param->i_cb_qp_offset ;
    pps->i_chroma_cr_qp_offset = param->i_cr_qp_offset ;
    pps->i_num_substreams = param->pps.i_wave_front_substreams ;
    pps->b_entropy_coding_sync_enabled_flag = (param->pps.i_wave_front_synchro > 0) ? 1 : 0 ;
    pps->b_tiles_enabled_flag = ( param->pps.i_num_columns_minus_1 > 0 || param->pps.i_num_rows_minus_1 > 0 ) ? 1 : 0 ;
    pps->b_use_weight_pred = param->pps.b_use_weighted_pred ;
    pps->b_use_weighted_bi_pred = param->pps.b_use_weighted_bi_pred ;
    pps->b_output_flag_present_flag = 0 ;

    pps->i_sign_hide_flag = param->pps.i_sign_hide_flag ;
    pps->b_deblocking_filter_control_present_flag = ( param->deblock.i_deblocking_filter_control_present > 0) ? 1 : 0 ;
    pps->i_log2_parallel_merge_level_minus2 = param->pps.i_log2_parallel_merge_level - 2 ;
    pps->b_cabac_init_present_flag = X265_CABAC_INIT_PRESENT_FLAG ;
    pps->b_loop_filter_across_slices_enabled_flag = param->deblock.b_lf_cross_slice_boundary_flag ;

    for( loop = 0; loop <= X265_MAX_NUM_REF; ++ loop )
    {
    	histogram[loop] = 0 ;
    }
    for( loop = 0 ; loop < param->gop.i_gop_size; ++ loop )
    {
      assert ( param->gop.gop_list[loop].i_num_ref_pics_active >= 0
    		  && param->gop.gop_list[loop].i_num_ref_pics_active <= X265_MAX_NUM_REF) ;
      histogram[param->gop.gop_list[loop].i_num_ref_pics_active] ++ ;
    }
    i_max_hist = -1 ;
    i_best_pos = 0 ;
    for( loop = 0; loop <= X265_MAX_NUM_REF ; ++ loop )
    {
    	if (histogram[loop] > i_max_hist)
    	{
    		i_max_hist = histogram[loop] ;
    		i_best_pos = loop ;
    	}
    }
#if X265_L0323_LIMIT_DEFAULT_LIST_SIZE
    assert ( i_best_pos <= 15 ) ;
#endif

    pps->i_num_ref_idx_l0_default_active = i_best_pos ;
    pps->i_num_ref_idx_l1_default_active = i_best_pos ;
    pps->b_transquant_bypass_enable_flag = param->pps.b_transquant_bypass_enable_flag ;
    pps->b_use_transform_skip = param->b_use_transform_skip ;
    if ( param->i_slice_segment_mode )
    {
    	pps->b_dependent_slice_segments_enabled_flag = 1 ;
    }

    pps->i_uniform_spacing_flag = param->pps.i_uniform_spacing_idr ;
    pps->i_num_columns_minus1 = param->pps.i_num_columns_minus_1 ;
    pps->i_num_rows_minus1 = param->pps.i_num_rows_minus_1 ;
    if ( 0 == param->pps.i_uniform_spacing_idr )
    {
    	set_column_width ( pps, param->pps.column_width ) ;
    	set_row_height ( pps, param->pps.row_height ) ;
    }
    pps->b_loop_filter_across_tiles_enabled_flag = param->deblock.b_lf_cross_tile_boundary_flag ;
    if ( param->pps.i_wave_front_synchro )
    {
    	pps->i_num_substreams = param->pps.i_wave_front_substreams * (param->pps.i_num_columns_minus_1 + 1) ;
    }
    pps->b_scaling_list_present_flag = 0 ;

	if ( pps->b_deblocking_filter_control_present_flag )
	{
		pps->b_deblocking_filter_override_enabled_flag = !h->param.deblock.i_loop_filter_offset_in_pps ;
		pps->b_pic_disable_deblocking_filter_flag = h->param.sps.b_loop_filter_disable  ;
	}


    return 0 ;

fail:
	x265_pps_deinit ( pps ) ;
	return -1 ;
}


/*
 *   bs_write_ue ( s, pps->getPPSId(),                             "pps_pic_parameter_set_id" ) ;
  bs_write_ue ( s, pps->getSPSId(),                             "pps_seq_parameter_set_id" ) ;
  bs_write1 ( s, pps->getDependentSliceSegmentsEnabledFlag()    ? 1 : 0, "dependent_slice_segments_enabled_flag" ) ;
#if L0255_MOVE_PPS_FLAGS
  bs_write1 ( s, pps->getOutputFlagPresentFlag() ? 1 : 0,     "output_flag_present_flag" ) ;
  bs_write ( s, pps->getNumExtraSliceHeaderBits(), 3,        "num_extra_slice_header_bits") ;
#endif
  bs_write1 ( s, pps->getSignHideFlag(), "sign_data_hiding_flag" ) ;
  bs_write1 ( s, pps->getCabacInitPresentFlag() ? 1 : 0,   "cabac_init_present_flag" ) ;
  bs_write_ue ( s, pps->getNumRefIdxL0DefaultActive()-1,     "num_ref_idx_l0_default_active_minus1") ;
  bs_write_ue ( s, pps->getNumRefIdxL1DefaultActive()-1,     "num_ref_idx_l1_default_active_minus1") ;

  bs_write_se ( s, pps->getPicInitQPMinus26(),                  "init_qp_minus26") ;
  bs_write1 ( s, pps->getConstrainedIntraPred() ? 1 : 0,      "constrained_intra_pred_flag" ) ;
  bs_write1 ( s, pps->getUseTransformSkip() ? 1 : 0,  "transform_skip_enabled_flag" ) ;
  bs_write1 ( s, pps->getUseDQP() ? 1 : 0, "cu_qp_delta_enabled_flag" ) ;
  if ( pps->getUseDQP() )
  {
    bs_write_ue ( s, pps->getMaxCuDQPDepth(), "diff_cu_qp_delta_depth" ) ;
  }
  bs_write_se ( s, pps->getChromaCbQpOffset(),                   "pps_cb_qp_offset" ) ;
  bs_write_se ( s, pps->getChromaCrQpOffset(),                   "pps_cr_qp_offset" ) ;
  bs_write1 ( s, pps->getSliceChromaQpFlag() ? 1 : 0,          "pps_slice_chroma_qp_offsets_present_flag" ) ;

  bs_write1 ( s, pps->getUseWP() ? 1 : 0,  "weighted_pred_flag" ) ;   // Use of Weighting Prediction (P_SLICE)
  bs_write1 ( s, pps->getWPBiPred() ? 1 : 0, "weighted_bipred_flag" ) ;  // Use of Weighting Bi-Prediction (B_SLICE)
#if !L0255_MOVE_PPS_FLAGS
  bs_write1 ( s, pps->getOutputFlagPresentFlag() ? 1 : 0,  "output_flag_present_flag" ) ;
#endif
  bs_write1 ( s, pps->getTransquantBypassEnableFlag() ? 1 : 0, "transquant_bypass_enable_flag" ) ;
  bs_write1 ( s, pps->getTilesEnabledFlag()             ? 1 : 0, "tiles_enabled_flag" ) ;
  bs_write1 ( s, pps->getEntropyCodingSyncEnabledFlag() ? 1 : 0, "entropy_coding_sync_enabled_flag" ) ;
  if( pps->getTilesEnabledFlag() )
  {
    bs_write_ue ( s, pps->getNumColumnsMinus1(),                                    "num_tile_columns_minus1" ) ;
    bs_write_ue ( s, pps->getNumRowsMinus1(),                                       "num_tile_rows_minus1" ) ;
    bs_write1 ( s, pps->getUniformSpacingFlag(),                                  "uniform_spacing_flag" ) ;
    if( pps->getUniformSpacingFlag() == 0 )
    {
      for(UInt i=0; i<pps->getNumColumnsMinus1() ; i++)
      {
        bs_write_ue ( s, pps->getColumnWidth(i)-1,                                  "column_width_minus1" ) ;
      }
      for(UInt i=0; i<pps->getNumRowsMinus1() ; i++)
      {
        bs_write_ue ( s, pps->getRowHeight(i)-1,                                    "row_height_minus1" ) ;
      }
    }
    if(pps->getNumColumnsMinus1() !=0 || pps->getNumRowsMinus1() !=0)
    {
      bs_write1 ( s, pps->getLoopFilterAcrossTilesEnabledFlag()?1 : 0,          "loop_filter_across_tiles_enabled_flag") ;
    }
  }
  bs_write1 ( s, pps->getLoopFilterAcrossSlicesEnabledFlag()?1 : 0,        "loop_filter_across_slices_enabled_flag") ;
  bs_write1 ( s, pps->getDeblockingFilterControlPresentFlag()?1 : 0,       "deblocking_filter_control_present_flag") ;
  if(pps->getDeblockingFilterControlPresentFlag())
  {
    bs_write1 ( s, pps->getDeblockingFilterOverrideEnabledFlag() ? 1 : 0,  "deblocking_filter_override_enabled_flag" ) ;
    bs_write1 ( s, pps->getPicDisableDeblockingFilterFlag() ? 1 : 0,       "pps_disable_deblocking_filter_flag" ) ;
    if(!pps->getPicDisableDeblockingFilterFlag())
    {
      bs_write_se ( s, pps->getDeblockingFilterBetaOffsetDiv2(),             "pps_beta_offset_div2" ) ;
      bs_write_se ( s, pps->getDeblockingFilterTcOffsetDiv2(),               "pps_tc_offset_div2" ) ;
    }
  }
  bs_write1 ( s, pps->getScalingListPresentFlag() ? 1 : 0,                          "pps_scaling_list_data_present_flag" ) ;
  if( pps->getScalingListPresentFlag() )
  {
#if SCALING_LIST_OUTPUT_RESULT
    printf("PPS\n") ;
#endif
    codeScalingList( m_pcSlice->getScalingList() ) ;
  }
  bs_write1 ( s, pps->getListsModificationPresentFlag(), "lists_modification_present_flag") ;
  bs_write_ue ( s, pps->getLog2ParallelMergeLevelMinus2(), "log2_parallel_merge_level_minus2") ;
#if !L0255_MOVE_PPS_FLAGS
  bs_write ( s, pps->getNumExtraSliceHeaderBits(), 3, "num_extra_slice_header_bits") ;
#endif
  bs_write1 ( s, pps->getSliceHeaderExtensionPresentFlag() ? 1 : 0, "slice_segment_header_extension_present_flag") ;
  bs_write1 ( s, 0, "pps_extension_flag" ) ;
 */

void x265_pps_write( x265_t *h, bs_t *s, x265_pps_t *pps )
{
	int32_t loop = 0 ;

	bs_write_ue ( s, pps->i_pps_id ) ;
	bs_write_ue ( s, pps->i_sps_id ) ;
	bs_write1 ( s, pps->b_dependent_slice_segments_enabled_flag ? 1 : 0 ) ;
#if X265_L0255_MOVE_PPS_FLAGS
	bs_write1 ( s, pps->b_output_flag_present_flag ? 1 : 0 ) ;
	bs_write ( s, 3, pps->i_num_extra_slice_header_bits ) ;
#endif
	bs_write1 ( s, pps->i_sign_hide_flag ) ;
	bs_write1 ( s, pps->b_cabac_init_present_flag ? 1 : 0 ) ;
	bs_write_ue ( s, pps->i_num_ref_idx_l0_default_active - 1 ) ;
	bs_write_ue ( s, pps->i_num_ref_idx_l1_default_active - 1 ) ;

	bs_write_se ( s, pps->i_pic_init_qp_minus26 ) ;
	bs_write1 ( s, pps->b_constrained_intra_pred ? 1 : 0 ) ;
	bs_write1 ( s, pps->b_use_transform_skip ? 1 : 0 ) ;
	bs_write1 ( s, pps->b_use_dqp ? 1 : 0 ) ;
	if ( pps->b_use_dqp )
	{
		bs_write_ue ( s, pps->i_max_cu_dqp_depth ) ;
	}
	bs_write_se ( s, pps->i_chroma_cb_qp_offset ) ;
	bs_write_se ( s, pps->i_chroma_cr_qp_offset ) ;
	bs_write1 ( s, pps->b_slice_chroma_qp_flag ) ;

	// Use of Weighting Prediction (P_SLICE)
	bs_write1 ( s, pps->b_use_weight_pred ? 1 : 0 ) ;
	// Use of Weighting Bi-Prediction (B_SLICE)
	bs_write1 ( s, pps->b_use_weighted_bi_pred ? 1 : 0 ) ;
#if !X265_L0255_MOVE_PPS_FLAGS
	bs_write1 ( s, pps->b_output_flag_present_flag ? 1 : 0 ) ;
#endif
	bs_write1 ( s, pps->b_transquant_bypass_enable_flag ? 1 : 0 ) ;
	bs_write1 ( s, pps->b_tiles_enabled_flag ? 1 : 0 ) ;
	bs_write1 ( s, pps->b_entropy_coding_sync_enabled_flag ? 1 : 0 ) ;

	if( pps->b_tiles_enabled_flag )
	{
		bs_write_ue ( s, pps->i_num_columns_minus1 ) ;
		bs_write_ue ( s, pps->i_num_rows_minus1 ) ;
		bs_write1 ( s, pps->i_uniform_spacing_flag ) ;
		if( 0 == pps->i_uniform_spacing_flag )
		{
			for( loop = 0 ; loop < pps->i_num_columns_minus1 ; ++ loop )
			{
				bs_write_ue ( s, pps->column_width[loop] - 1 ) ;
			}
			for( loop = 0 ; loop < pps->i_num_rows_minus1 ; ++ loop )
			{
				bs_write_ue ( s, pps->row_height[loop] - 1 ) ;
			}
		}
		if ( 0 != pps->i_num_columns_minus1 || 0 != pps->i_num_rows_minus1 )
		{
			bs_write1 ( s, pps->b_loop_filter_across_tiles_enabled_flag ? 1 : 0 ) ;
		}
	}
	bs_write1 ( s, pps->b_loop_filter_across_slices_enabled_flag ) ;
	bs_write1 ( s, pps->b_deblocking_filter_control_present_flag ) ;
	if ( pps->b_deblocking_filter_control_present_flag )
	{
		bs_write1 ( s, pps->b_deblocking_filter_override_enabled_flag ) ;
		bs_write1 ( s, pps->b_pic_disable_deblocking_filter_flag ) ;
		if ( ! pps->b_pic_disable_deblocking_filter_flag )
		{
			bs_write_se ( s, pps->i_deblocking_filter_beta_offset_div2 ) ;
			bs_write_se ( s, pps->i_deblocking_filter_tc_offset_div2 ) ;
		}
	}

	bs_write1 ( s, pps->b_scaling_list_present_flag ) ;
	if( pps->b_scaling_list_present_flag )
	{
		x265_scaling_list_write ( h, s, &h->scaling_list ) ;
	}
	bs_write1 ( s, pps->b_lists_modification_present_flag ) ;
	bs_write_ue ( s, pps->i_log2_parallel_merge_level_minus2 ) ;
#if !X265_L0255_MOVE_PPS_FLAGS
	bs_write ( s, 3, pps->i_num_extra_slice_header_bits ) ;
#endif
	bs_write1 ( s, pps->b_slice_header_extension_present_flag ? 1 : 0 ) ;


	bs_write1 ( s, 0 ) ;

	bs_write1 ( s, 1 ) ;
	bs_flush ( s ) ;

}

