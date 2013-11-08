
#include "common/common.h"


void x265_print_gop_entry ( gop_entry_t *gop_entry, FILE *file )
{
	int loop = 0 ;

	fprintf ( file, "%c ", gop_entry->i_slice_type ) ;
	fprintf ( file, "%d ", gop_entry->i_poc ) ;
	fprintf ( file, "%d ", gop_entry->i_qp_Offset ) ;
	print_double ( gop_entry->f_qp_factor, file ) ;
	fprintf ( file, "%d ", gop_entry->i_tc_offset_div_2 ) ;
	fprintf ( file, "%d ", gop_entry->i_beta_offset_div_2 ) ;
	fprintf ( file, "%d ", gop_entry->i_temporal_id ) ;
	fprintf ( file, "%d ", gop_entry->b_ref_pic ) ;
	fprintf ( file, "%d ", gop_entry->i_num_ref_pics_active ) ;
	fprintf ( file, "%d ", gop_entry->i_num_ref_pics ) ;
	fprintf ( file, "%d ", gop_entry->i_inter_rps_prediction ) ;
	fprintf ( file, "%d ", gop_entry->i_delta_rps ) ;
	fprintf ( file, "%d ", gop_entry->i_num_ref_idc ) ;
	fprintf ( file, "\n" ) ;

	fprintf ( file, "ReferencePics: ") ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", gop_entry->reference_pics[loop] ) ;
	}
	fprintf ( file, "\n") ;

	fprintf ( file, "UsedByCurrPic: ") ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", gop_entry->used_by_curr_pic[loop] ) ;
	}
	fprintf ( file, "\n") ;

	fprintf ( file, "RefIdc: ") ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS + 1; ++ loop )
	{
		fprintf ( file, "%d ", gop_entry->i_ref_idc[loop] ) ;
	}
	fprintf ( file, "\n") ;

}

void x265_print_parameters ( x265_param_t *p )
{
	int32_t loop = 0 ;
	FILE *file = NULL ;

	file = fopen ( "ParameterSet2", "wb" ) ;

	fprintf ( file, "SourceWidth:                         %d\n", p->i_width ) ;
	fprintf ( file, "SourceHeight:                        %d\n", p->i_height ) ;
	for ( loop = 0 ; loop < X265_MAX_TLAYER ; ++ loop )
	{
		fprintf ( file, "LambdaModifier%d:                     %f\n",
				loop,
				p->rdo.f_ad_lambda_modifier[loop] ) ;
	}
	fprintf ( file, "FrameRate:                           %d\n", p->i_frame_rate ) ;
	fprintf ( file, "FrameSkip:                           %d\n", p->i_frame_skip ) ;
	fprintf ( file, "ConformanceMode:                     %d\n", p->sps.i_conformance_mode ) ;
	fprintf ( file, "FramesToBeEncoded:                   %d\n", p->i_frame_total ) ;
	fprintf ( file, "ConfLeft:                            %d\n", p->sps.i_conf_left ) ;
	fprintf ( file, "ConfRight:                           %d\n", p->sps.i_conf_right ) ;
	fprintf ( file, "ConfTop:                             %d\n", p->sps.i_conf_top ) ;
	fprintf ( file, "ConfBottom:                          %d\n", p->sps.i_conf_bottom ) ;
	fprintf ( file, "HorizontalPadding:                   %d\n", p->i_pad[0] ) ;
	fprintf ( file, "VerticalPadding:                     %d\n", p->i_pad[1] ) ;
	fprintf ( file, "Profile:                             %d\n", p->sps.i_profile ) ;
	fprintf ( file, "Level:                               %d\n", p->sps.i_level ) ;
	fprintf ( file, "Tier:                                %d\n", p->sps.i_level_tier ) ;

#if X265_L0046_CONSTRAINT_FLAGS
	fprintf ( file, "ProgressiveSource:                   %d\n", p->sps.b_progressive_source_flag ) ;
	fprintf ( file, "InterlacedSource:                    %d\n", p->sps.b_interlaced_source_flag ) ;
	fprintf ( file, "NonPackedSource:                     %d\n", p->sps.b_non_packed_constraint_flag ) ;
	fprintf ( file, "FrameOnly:                           %d\n", p->sps.b_frame_only_constraint_flag ) ;
#endif

	fprintf ( file, "IntraPeriod:                         %d\n", p->gop.i_intra_period ) ;
	fprintf ( file, "DecodingRefreshType:                 %d\n", p->gop.i_decoding_refresh_type ) ;
	fprintf ( file, "GOPSize:                             %d\n", p->gop.i_gop_size ) ;

	fprintf ( file, "MaxTempLayer:                        %d\n", p->sps.i_max_temp_layer ) ;
	for ( loop = 0 ; loop < X265_MAX_GOP ; ++ loop )
	{
		fprintf ( file, "GOPEntry%d:\n", loop ) ;
		x265_print_gop_entry ( &p->gop.gop_list[loop], file ) ;
	}

	fprintf ( file, "NumReorderPics: ") ;
	for ( loop = 0 ; loop < X265_MAX_TLAYER ; ++ loop )
	{
		fprintf ( file, "%d ", p->gop.num_reorder_pics[loop] ) ;
	}
	fprintf ( file, "\n") ;

	fprintf ( file, "DecPicBuffering: ") ;
	for ( loop = 0 ; loop < X265_MAX_TLAYER ; ++ loop )
	{
		fprintf ( file, "%d ", p->gop.max_dec_pic_buffering[loop] ) ;
	}
	fprintf ( file, "\n") ;


	fprintf ( file, "ListCombination:                     %d\n", p->b_use_list_combination ) ;
	fprintf ( file, "TransformSkip:                       %d\n", p->b_use_transform_skip ) ;
	fprintf ( file, "TransformSkipFast:                   %d\n", p->b_use_transform_skip_fast ) ;
	fprintf ( file, "AMP:                                 %d\n", p->b_enable_amp ) ;
	fprintf ( file, "QP:                                  %f\n", p->f_qp ) ;
	fprintf ( file, "MaxDeltaQP:                          %d\n", p->rdo.i_max_delta_qp ) ;
	fprintf ( file, "DeltaQpRD:                           %d\n", p->rdo.i_delta_qp_rd ) ;
	fprintf ( file, "MaxCuDQPDepth:                       %d\n", p->rdo.i_max_cu_delta_qp_depth ) ;
	fprintf ( file, "CbQpOffset:                          %d\n", p->i_cb_qp_offset ) ;
	fprintf ( file, "CrQpOffset:                          %d\n", p->i_cr_qp_offset ) ;
#if X265_ADAPTIVE_QP_SELECTION
	fprintf ( file, "AdaptiveQpSelection:                 %d\n", p->b_use_adapt_qp_select ) ;
#endif
	fprintf ( file, "AdaptiveQP:                          %d\n", p->b_use_adaptive_qp ) ;
	fprintf ( file, "MaxQPAdaptationRange:                %d\n", p->i_qp_adaptation_range ) ;
	fprintf ( file, "MaxCUWidth:                          %d\n", p->sps.i_max_cu_width ) ;
	fprintf ( file, "MaxCUHeight:                         %d\n", p->sps.i_max_cu_height ) ;
	fprintf ( file, "MaxPartitionDepth:                   %d\n", p->sps.i_max_cu_depth ) ;
	fprintf ( file, "QuadtreeTULog2MaxSize:               %d\n", p->sps.i_quadtree_tu_log2_max_size ) ;
	fprintf ( file, "QuadtreeTULog2MinSize:               %d\n", p->sps.i_quadtree_tu_log2_min_size ) ;
	fprintf ( file, "QuadtreeTUMaxDepthIntra:             %d\n", p->sps.i_quadtree_tu_max_depth_intra ) ;
	fprintf ( file, "QuadtreeTUMaxDepthInter:             %d\n", p->sps.i_quadtree_tu_max_depth_inter ) ;
	fprintf ( file, "InputBitDepth:                       %d\n", p->i_input_bit_depth_y ) ;
	fprintf ( file, "OutputBitDepth:                      %d\n", p->i_output_bit_depth_y ) ;
	fprintf ( file, "InternalBitDepth:                    %d\n", p->sps.i_bit_depth_y ) ;
	fprintf ( file, "InputBitDepthC:                      %d\n", p->i_input_bit_depth_c ) ;
	fprintf ( file, "OutputBitDepthC:                     %d\n", p->i_output_bit_depth_c ) ;
	fprintf ( file, "InternalBitDepthC:                   %d\n", p->sps.i_bit_depth_c ) ;
	fprintf ( file, "PCMInputBitDepthFlag:                %d\n", p->b_pcm_input_bit_depth_flag ) ;
	fprintf ( file, "LosslessCuEnabled:                   %d\n", p->sps.b_use_lossless ) ;
	fprintf ( file, "SAO:                                 %d\n", p->sps.b_use_sao ) ;
	fprintf ( file, "MaxNumOffsetsPerPic:                 %d\n", p->sao.i_max_num_offsets_per_pic ) ;
	fprintf ( file, "SAOLcuBoundary:                      %d\n", p->sao.i_sao_lcu_boundary ) ;
	fprintf ( file, "SAOLcuBasedOptimization:             %d\n", p->sao.i_sao_lcu_based_optimization ) ;
	fprintf ( file, "LoopFilterDisable:                   %d\n", p->sps.b_loop_filter_disable ) ;
	fprintf ( file, "LoopFilterOffsetInPPS:               %d\n", p->deblock.i_loop_filter_offset_in_pps ) ;
	fprintf ( file, "LoopFilterBetaOffset-div2:           %d\n", p->deblock.i_loop_filter_beta_offset_div_2 ) ;
	fprintf ( file, "LoopFilterTcOffset-div2:             %d\n", p->deblock.i_loop_filter_tc_offset_div_2 ) ;
	fprintf ( file, "DeblockingFilterControlPresent:      %d\n", p->deblock.i_deblocking_filter_control_present ) ;
	fprintf ( file, "PCMEnabledFlag:                      %d\n", p->sps.b_use_pcm ) ;
	fprintf ( file, "PCMLog2MaxSize:                      %d\n", p->pcm.i_pcm_log2_max_size ) ;
	fprintf ( file, "PCMLog2MinSize:                      %d\n", p->pcm.i_pcm_log2_min_size ) ;
	fprintf ( file, "PCMFilterDisableFlag:                %d\n", p->pcm.b_pcm_filter_disable_flag ) ;
	fprintf ( file, "SBACRD:                              %d\n", p->b_use_sbac_rd ) ;
	fprintf ( file, "ASR:                                 %d\n", p->b_use_asr ) ;
	fprintf ( file, "HadamardME:                          %d\n", p->b_use_had_me ) ;
	fprintf ( file, "RDOQ:                                %d\n", p->rdo.b_use_rdo_q ) ;
	fprintf ( file, "RDOQTS:                              %d\n", p->rdo.b_use_rdo_q_ts ) ;

#if X265_L0232_RD_PENALTY
	fprintf ( file, "RDpenalty:                           %d\n", p->rdo.i_rd_penalty ) ;
#endif
	fprintf ( file, "FastSearch:                          %d\n", p->i_fast_search ) ;
	fprintf ( file, "SearchRange:                         %d\n", p->i_search_range ) ;
	fprintf ( file, "BipredSearchRange:                   %d\n", p->i_bipred_search_range ) ;
	fprintf ( file, "FEN:                                 %d\n", p->b_use_fast_enc ) ;
	fprintf ( file, "ECU:                                 %d\n", p->b_use_early_cu ) ;
	fprintf ( file, "FDM:                                 %d\n", p->b_use_fast_decision_for_merge ) ;
	fprintf ( file, "CFM:                                 %d\n", p->b_use_cbf_fast_mode ) ;
	fprintf ( file, "ESD:                                 %d\n", p->b_use_early_skip_detection ) ;
	fprintf ( file, "SliceMode:                           %d\n", p->i_slice_mode ) ;
	fprintf ( file, "SliceArgument:                       %d\n", p->i_slice_argument ) ;
	fprintf ( file, "SliceSegmentMode:                    %d\n", p->i_slice_segment_mode ) ;
	fprintf ( file, "SliceSegmentArgument:                %d\n", p->i_slice_segment_argument ) ;
	fprintf ( file, "LFCrossSliceBoundaryFlag:            %d\n", p->deblock.b_lf_cross_slice_boundary_flag ) ;
	fprintf ( file, "LFCrossTileBoundaryFlag:             %d\n", p->deblock.b_lf_cross_tile_boundary_flag ) ;
	fprintf ( file, "UniformSpacingIdc:                   %d\n", p->pps.i_uniform_spacing_idr ) ;
	fprintf ( file, "NumTileColumnsMinus1:                %d\n", p->pps.i_num_columns_minus_1 ) ;
	fprintf ( file, "NumTileRowsMinus1:                   %d\n", p->pps.i_num_rows_minus_1 ) ;
	fprintf ( file, "WaveFrontSynchro:                    %d\n", p->pps.i_wave_front_synchro ) ;
	fprintf ( file, "ConstrainedIntraPred:                %d\n", p->b_use_constrained_intra_pred ) ;
	fprintf ( file, "SEIDecodedPictureHash                %d\n", p->sei.i_decoded_picture_hash_sei_enabled ) ;
	fprintf ( file, "SEIRecoveryPoint:                    %d\n", p->sei.i_recovery_point_sei_enabled ) ;
	fprintf ( file, "SEIBufferingPeriod:                  %d\n", p->sei.i_buffering_period_sei_enabled ) ;
	fprintf ( file, "SEIPictureTiming:                    %d\n", p->sei.i_picture_timing_sei_enabled ) ;
	fprintf ( file, "SEIFramePacking:                     %d\n", p->sei.i_framePacking_sei_enabled ) ;
	fprintf ( file, "SEIFramePackingType:                 %d\n", p->sei.i_framePacking_sei_type ) ;
	fprintf ( file, "SEIFramePackingId:                   %d\n", p->sei.i_framePacking_sei_id ) ;
	fprintf ( file, "SEIFramePackingQuincunx:             %d\n", p->sei.i_framePacking_sei_quincunx ) ;
	fprintf ( file, "SEIFramePackingInterpretation:       %d\n", p->sei.i_framePacking_sei_interpretation ) ;
	fprintf ( file, "SEIDisplayOrientation:               %d\n", p->sei.i_display_orientation_sei_Angle ) ;
	fprintf ( file, "SEITemporalLevel0Index:              %d\n", p->sei.i_temporal_level0_index_sei_enabled ) ;
	fprintf ( file, "SEIGradualDecodingRefreshInfo:       %d\n", p->sei.i_gradual_decoding_refresh_info_enabled ) ;
	fprintf ( file, "SEIDecodingUnitInfo:                 %d\n", p->sei.i_decoding_unit_info_sei_enabled ) ;
	fprintf ( file, "WeightedPredP:                       %d\n", p->pps.b_use_weighted_pred ) ;
	fprintf ( file, "WeightedPredB:                       %d\n", p->pps.b_use_weighted_bi_pred ) ;
	fprintf ( file, "Log2ParallelMergeLevel:              %d\n", p->pps.i_log2_parallel_merge_level ) ;
	fprintf ( file, "MaxNumMergeCand:                     %d\n", p->i_max_num_merge_cand ) ;
	fprintf ( file, "TMVPMode:                            %d\n", p->sps.i_tmvp_mode_id ) ;
	fprintf ( file, "SignHideFlag:                        %d\n", p->pps.i_sign_hide_flag ) ;
#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	fprintf ( file, "RateControl:                         %d\n", p->rc.b_enable_rate_control ) ;
	fprintf ( file, "TargetBitrate:                       %d\n", p->rc.i_target_bitrate ) ;
	fprintf ( file, "KeepHierarchicalBit:                 %d\n", p->rc.b_keep_hierarchical_bit ) ;
	fprintf ( file, "LCULevelRateControl:                 %d\n", p->rc.b_lcu_level_rc ) ;
	fprintf ( file, "RCLCUSeparateModel:                  %d\n", p->rc.b_use_lcu_separate_model ) ;
	fprintf ( file, "InitialQP:                           %d\n", p->rc.i_initial_qp ) ;
	fprintf ( file, "RCForceIntraQP:                      %d\n", p->rc.b_force_intra_qp ) ;
#else
	fprintf ( file, "RateCtrl:                            %d\n", p->b_enable_rate_ctrl ) ;
	fprintf ( file, "TargetBitrate:                       %d\n", p->i_target_bitrate ) ;
	fprintf ( file, "NumLcuInUnit:                        %d\n", p->i_num_lcu_in_unit ) ;
#endif
	fprintf ( file, "ScalingList:                         %d\n", p->sps.i_use_scaling_list_id ) ;
	fprintf ( file, "TransquantBypassEnableFlag:          %d\n", p->pps.b_transquant_bypass_enable_flag ) ;
	fprintf ( file, "CUTransquantBypassFlagValue:         %d\n", p->b_cu_transquant_bypass_flag_value ) ;
	fprintf ( file, "RecalculateQPAccordingToLambda:      %d\n", p->b_recalculate_qp_according_to_lambda ) ;
	fprintf ( file, "StrongIntraSmoothing:                %d\n", p->sps.b_use_strong_intra_smoothing ) ;
	fprintf ( file, "SEIActiveParameterSets:              %d\n", p->sei.i_active_parameter_sets_sei_enabled ) ;

	fprintf ( file, "VuiParametersPresent:                %d\n", p->sps.b_vui_parameters_present_flag ) ;
	fprintf ( file, "AspectRatioInfoPresent:              %d\n", p->sps.vui.b_aspec_ratio_info_present_flag ) ;
	fprintf ( file, "AspectRatioIdc:                      %d\n", p->sps.vui.i_aspect_ratio_idc ) ;
	fprintf ( file, "SarWidth:                            %d\n", p->sps.vui.i_sar_width ) ;
	fprintf ( file, "SarHeight:                           %d\n", p->sps.vui.i_sar_height ) ;
	fprintf ( file, "OverscanInfoPresent:                 %d\n", p->sps.vui.b_overscan_info_present_flag ) ;
	fprintf ( file, "OverscanAppropriate:                 %d\n", p->sps.vui.b_overscan_appropriate_flag ) ;
	fprintf ( file, "VideoSignalTypePresent:              %d\n", p->sps.vui.b_video_signal_type_present_flag ) ;
	fprintf ( file, "VideoFormat:                         %d\n", p->sps.vui.i_video_format ) ;
	fprintf ( file, "VideoFullRange:                      %d\n", p->sps.vui.b_video_full_range_flag ) ;
	fprintf ( file, "ColourDescriptionPresent:            %d\n", p->sps.vui.b_colour_description_present_flag ) ;
	fprintf ( file, "ColourPrimaries:                     %d\n", p->sps.vui.i_colour_primaries ) ;
	fprintf ( file, "TransferCharateristics:              %d\n", p->sps.vui.i_transfer_characteristics ) ;
	fprintf ( file, "MatrixCoefficients:                  %d\n", p->sps.vui.i_matrix_coefficients ) ;
	fprintf ( file, "ChromaLocInfoPresent:                %d\n", p->sps.vui.b_chroma_loc_info_present_flag ) ;
	fprintf ( file, "ChromaSampleLocTypeTopField:         %d\n", p->sps.vui.i_chroma_sample_loc_type_top_field ) ;
	fprintf ( file, "ChromaSampleLocTypeBottomField:      %d\n", p->sps.vui.i_chroma_sample_loc_type_bottom_field ) ;
	fprintf ( file, "NeutralChromaIndication:             %d\n", p->sps.vui.b_neutral_chroma_indication_flag ) ;
	fprintf ( file, "DefaultDisplayWindowFlag:            %d\n", p->sps.vui.b_default_display_window_flag ) ;
	fprintf ( file, "DefDispWinLeftOffset:                %d\n", p->sps.vui.i_def_disp_win_left_offset ) ;
	fprintf ( file, "DefDispWinRightOffset:               %d\n", p->sps.vui.i_def_disp_win_right_offset ) ;
	fprintf ( file, "DefDispWinTopOffset:                 %d\n", p->sps.vui.i_def_disp_win_top_offset ) ;
	fprintf ( file, "DefDispWinBottomOffset:              %d\n", p->sps.vui.i_def_disp_win_bottom_offset ) ;
	fprintf ( file, "FrameFieldInfoPresentFlag:           %d\n", p->sps.vui.b_frame_field_info_present_flag ) ;
	fprintf ( file, "PocProportionalToTimingFlag:         %d\n", p->sps.vui.b_poc_proportional_to_timing_flag ) ;
	fprintf ( file, "NumTicksPocDiffOneMinus1:            %d\n", p->sps.vui.i_num_ticks_poc_diff_one_minus1 ) ;
	fprintf ( file, "BitstreamRestriction:                %d\n", p->sps.vui.b_bitstream_restriction_flag ) ;
	fprintf ( file, "TilesFixedStructure:                 %d\n", p->sps.vui.b_tiles_fixed_structure_flag ) ;
	fprintf ( file, "MotionVectorsOverPicBoundaries:      %d\n", p->sps.vui.b_motion_vectors_over_pic_boundaries_flag ) ;
	fprintf ( file, "MaxBytesPerPicDenom:                 %d\n", p->sps.vui.i_max_bytes_per_pic_denom ) ;
	fprintf ( file, "MaxBitsPerMinCuDenom:                %d\n", p->sps.vui.i_max_bits_per_min_cu_denom ) ;
	fprintf ( file, "Log2MaxMvLengthHorizontal:           %d\n", p->sps.vui.i_log2_max_mv_length_horizontal ) ;
	fprintf ( file, "Log2MaxMvLengthVertical:             %d\n", p->sps.vui.i_log2_max_mv_length_vertical ) ;

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	fprintf ( file, "BitRatePicRateMaxTLayers:                           %d\n", p->vps.i_bit_rate_pic_rate_max_t_layers ) ;
	for ( loop = 0 ; loop < p->vps.i_bit_rate_pic_rate_max_t_layers ; ++ loop )
	{
		fprintf ( file, "BitRateInfoPresent%d:                                %d\n", loop, p->vps.bit_rate_info_present_flag[loop]  ) ;
	}
	for ( loop = 0 ; loop < p->vps.i_bit_rate_pic_rate_max_t_layers ; ++ loop )
	{
		fprintf ( file, "PicRateInfoPresent%d:                                %d\n", loop, p->vps.pic_rate_info_present_flag[loop]  ) ;
	}
	for ( loop = 0 ; loop < p->vps.i_bit_rate_pic_rate_max_t_layers ; ++ loop )
	{
		fprintf ( file, "AvgBitRate%d:                                        %d\n", loop, p->vps.avg_bit_rate[loop]  ) ;
	}
	for ( loop = 0 ; loop < p->vps.i_bit_rate_pic_rate_max_t_layers ; ++ loop )
	{
		fprintf ( file, "MaxBitRate%d:                                        %d\n", loop, p->vps.max_bit_rate[loop]  ) ;
	}
	for ( loop = 0 ; loop < p->vps.i_bit_rate_pic_rate_max_t_layers ; ++ loop )
	{
		fprintf ( file, "AvgPicRate%d:                                        %d\n", loop, p->vps.avg_pic_rate[loop]  ) ;
	}
	for ( loop = 0 ; loop < p->vps.i_bit_rate_pic_rate_max_t_layers ; ++ loop )
	{
		fprintf ( file, "ConstantPicRateIdc%d:                                %d\n", loop, p->vps.constant_pic_rate_idc[loop]  ) ;
	}
#endif

	fprintf ( file, "\n" ) ;

	fclose ( file ) ;
}
