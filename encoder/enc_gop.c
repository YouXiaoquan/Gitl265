


#include "common/common.h"
#include "enc_gop.h"

x265_enc_gop_t *x265_enc_gop_new ( x265_t *h )
{
	x265_enc_gop_t *enc_gop = NULL ;

	CHECKED_MALLOCZERO( enc_gop, sizeof(x265_enc_gop_t) );
	if ( x265_enc_gop_init ( h, enc_gop ) )
	{
		goto fail ;
	}

	return enc_gop ;
fail:
	x265_enc_gop_delete ( h, enc_gop ) ;
	return NULL ;
}



void x265_enc_gop_delete ( x265_t *h, x265_enc_gop_t *enc_gop )
{
	x265_enc_gop_deinit ( h, enc_gop ) ;
	x265_free ( enc_gop ) ;
}

int x265_enc_gop_init ( x265_t *h, x265_enc_gop_t *enc_gop )
{
	memset ( &h->enc_gop, 0, sizeof(x265_enc_gop_t) ) ;

	if ( x265_enc_analyze_init(&enc_gop->enc_analyze_all) )
	{
		goto fail ;
	}
	if ( x265_enc_analyze_init(&enc_gop->enc_analyze_i) )
	{
		goto fail ;
	}
	if ( x265_enc_analyze_init(&enc_gop->enc_analyze_p) )
	{
		goto fail ;
	}
	if ( x265_enc_analyze_init(&enc_gop->enc_analyze_b) )
	{
		goto fail ;
	}

	enc_gop->stored_start_cu_addr_for_encoding_slice = x265_uint_list_new () ;
	if ( NULL == enc_gop->stored_start_cu_addr_for_encoding_slice )
	{
		goto fail ;
	}
	enc_gop->stored_start_cu_addr_for_encoding_slice_segment = x265_uint_list_new () ;
	if ( NULL == enc_gop->stored_start_cu_addr_for_encoding_slice_segment )
	{
		goto fail ;
	}

	return 0 ;

fail:
	x265_enc_gop_deinit ( h, &h->enc_gop ) ;
	return -1 ;
}

void x265_enc_gop_deinit ( x265_t *h, x265_enc_gop_t *enc_gop )
{
	x265_uint_list_delete ( enc_gop->stored_start_cu_addr_for_encoding_slice ) ;
	x265_uint_list_delete ( enc_gop->stored_start_cu_addr_for_encoding_slice_segment ) ;
}


int x265_enc_gop_initialize ( x265_t *h, x265_enc_gop_t *enc_gop )
{

	return 0 ;


}

void x265_enc_gop_deinitialize ( x265_t *h, x265_enc_gop_t *enc_gop )
{

}


int x265_enc_gop_create ( x265_t *h, x265_enc_gop_t *enc_gop )
{

	return 0 ;


}

void x265_enc_gop_destroy ( x265_t *h, x265_enc_gop_t *enc_gop )
{

}


enum nal_unit_type_e x265_enc_gop_get_nal_unit_type ( x265_t *h, x265_enc_gop_t *enc_gop, int32_t i_poc)
{
	if ( 0 == i_poc )
	{
		return NAL_SLICE_IDR;
	}
	if ( i_poc % h->param.gop.i_intra_period == 0 )
	{
		if ( h->param.gop.i_decoding_refresh_type == 1 )
		{
			return NAL_SLICE_CRA;
		}
		else if ( h->param.gop.i_decoding_refresh_type == 2 )
		{
			return NAL_SLICE_IDR;
		}
	}
	if ( enc_gop->i_poc_cra > 0 )
	{
		if ( i_poc < enc_gop->i_poc_cra )
		{
			return NAL_SLICE_TFD;
		}
	}
	return NAL_SLICE_TRAIL_R;
}


int x265_enc_gop_encoder_slice( x265_t *h,
								x265_enc_gop_t *enc_gop,
								x265_nal_t **pp_nal, int *pi_nal,
								x265_picture_t *pic_in,
								x265_picture_t *pic_out )
{
	int32_t loop = 0 ;
	int32_t i_poc = 0 ;
#if X265_L0044_DU_DPB_OUTPUT_DELAY_HRD
//	int32_t i_pic_spt_opb_output_du_delay = 0;
#endif
	uint32_t i_one_bitstream_per_slice_length = 0;
    uint32_t i_col_dir = 0;
//    long i_before_time = 0 ;
    int32_t i_close_left = 0, i_close_right = 0 ;
    int32_t i_ref = 0 ;
    int32_t i_left_qp = 0, i_right_qp = 0 ;
//    int32_t i_time_offset = 0 ;
    int32_t i_sstsa = 0 ;
    int32_t i = 0, j = 0, k = 0 ;
    int32_t i_l_tid = 0, i_t_tid = 0 ;
//	int32_t i_buffer_frame_list_size = 0 ;
	int32_t b_low_delay = 0 ;
	int32_t i_curr_poc = 0 ;
	int32_t i_refIdx = 0 ;
	int32_t b_gpb_check=0;
    uint32_t i_num_slices = 0 ;
    uint32_t i_internal_address = 0 ;
    uint32_t i_external_address = 0 ;
    uint32_t i_pos_x = 0 ;
    uint32_t i_pos_y = 0 ;
    uint32_t i_width = 0 ;
    uint32_t i_height = 0 ;
    uint32_t i_read_end_address = 0 ;
    int32_t i_num_substreams = 0 ;
    uint32_t i_start_cu_addr_slice_segment_idx = 0 ;
    uint32_t i_start_cu_addr_slice_segment = 0 ;
    uint32_t i_next_cu_addr = 0 ;
    uint32_t i_start_cu_addr_slice_idx = 0 ;
    uint32_t i_start_cu_addr_slice = 0 ;
    int32_t b_no_bin_bit_constraint_violated = 0 ;
    int32_t frame_size = 0 ;
    uint32_t *substream_sizes = NULL ;
    uint32_t i_total_coded_size = 0 ;
    uint32_t i_num_substreams_per_tile = 0 ;
    int32_t b_next_substream_in_new_tile = 0 ;
    int32_t i_offs = 0 ;
    int32_t i_nss = 0 ;

	x265_slice_t *slice = NULL ;
    x265_reference_picture_set_t * rps = NULL ;
	x265_ref_pic_list_modification_t *ref_pic_list_modification = NULL ;
	x265_output_bitstream_t ** pc_substreams_out = NULL ;
	x265_output_bitstream_t *pc_bitstream_redirect ;
	x265_enc_sbac_t **pc_sbac_coders = NULL ;


	pc_bitstream_redirect = (x265_output_bitstream_t*)x265_output_bitstream_new () ;
    i_col_dir = 1;

//    i_before_time = clock();
    i_close_left = 1 ;
    i_close_right =- 1;
    for( loop = 0 ;
    		loop < h->param.gop.gop_list[h->enc_top.i_gop_id].i_num_ref_pics ;
    		++ loop )
    {
    	i_ref = h->param.gop.gop_list[h->enc_top.i_gop_id].reference_pics[loop] ;
		if(i_ref>0&&(i_ref<i_close_right||i_close_right==-1))
		{
			i_close_right=i_ref;
		}
		else if(i_ref<0&&(i_ref>i_close_left||i_close_left==1))
		{
			i_close_left=i_ref;
		}
    }
    if(i_close_right>-1)
    {
    	i_close_right = i_close_right + h->param.gop.gop_list[h->enc_top.i_gop_id].i_poc - 1 ;
    }
    if(i_close_left<1)
    {
    	i_close_left = i_close_left + h->param.gop.gop_list[h->enc_top.i_gop_id].i_poc - 1 ;
    	while(i_close_left<0)
    	{
    		i_close_left+=h->enc_top.i_gop_size ;
    	}
    }

    i_left_qp = 0 ;
    i_right_qp = 0 ;
    for ( loop=0 ; loop < h->enc_top.i_gop_size ; ++ loop  )
    {
    	if(h->param.gop.gop_list[h->enc_top.i_gop_id].i_poc
    		==(i_close_left%h->enc_top.i_gop_size)+1)
    	{
    		i_left_qp = h->param.gop.gop_list[h->enc_top.i_gop_id].i_qp_Offset ;
    	}
    	if (h->param.gop.gop_list[h->enc_top.i_gop_id].i_poc
    		==(i_close_right%h->enc_top.i_gop_size)+1)
    	{
    		i_right_qp = h->param.gop.gop_list[h->enc_top.i_gop_id].i_qp_Offset ;
    	}
    }
    if(i_close_right>-1&&i_right_qp<i_left_qp)
    {
    	i_col_dir=0;
    }

    /*
    i_time_offset = h->param.gop.gop_list[h->enc_top.i_gop_id].i_poc;
    if(h->enc_top.i_poc_last == 0)
    {
    	i_time_offset = 1;
    }
    */

    if( NAL_SLICE_IDR == x265_enc_gop_get_nal_unit_type ( h, &h->enc_gop, h->fenc->i_poc )
    	|| NAL_SLICE_IDR_N_LP == x265_enc_gop_get_nal_unit_type ( h, &h->enc_gop, h->fenc->i_poc ) )
    {
    	enc_gop->i_last_idr = h->fenc->i_poc ;
    }

    x265_enc_slice_initialize ( h, h->fenc, h->enc_top.i_poc_last, h->fenc->i_poc, h->enc_top.i_num_frame_rcvd,
    							h->enc_top.i_gop_id, &slice ) ;
    slice->i_last_idr = enc_gop->i_last_idr ;
    slice->i_slice_idx = 0 ;
    slice->b_lf_cross_slice_boundary_flag = h->pps[0].b_loop_filter_across_slices_enabled_flag ;
    slice->scaling_list = &h->scaling_list ;
    slice->scaling_list->b_use_transform_skip = h->pps[0].b_use_transform_skip ;

    if ( SCALING_LIST_OFF == h->param.sps.i_use_scaling_list_id )
    {
    	x265_tr_quant_set_flat_scaling_list ( h, &h->tr_quant ) ;
    	h->tr_quant.b_scaling_list_enabled_flag = 0 ;
    }
    else if ( SCALING_LIST_DEFAULT == h->param.sps.i_use_scaling_list_id )
    {
    	x265_tr_quant_set_scaling_list ( h, &h->tr_quant, &h->scaling_list ) ;
    	h->tr_quant.b_scaling_list_enabled_flag = 1 ;
    }
    else if ( SCALING_LIST_FILE_READ == h->param.sps.i_use_scaling_list_id )
    {
    	x265_tr_quant_set_scaling_list ( h, &h->tr_quant, &h->scaling_list ) ;
    	h->tr_quant.b_scaling_list_enabled_flag = 1 ;
    }
    else
    {
    	printf("error : ScalingList == %d no support\n", h->param.sps.i_use_scaling_list_id ) ;
    	assert(0);
    }

    if ( B_SLICE == slice->i_slice_type
    		&& 'P' == h->param.gop.gop_list[h->enc_top.i_gop_id].i_slice_type )
    {
    	slice->i_slice_type = P_SLICE ;
    }
    slice->i_naul_type = x265_enc_gop_get_nal_unit_type ( h, &h->enc_gop, h->fenc->i_poc ) ;
    if ( NAL_SLICE_TRAIL_R == slice->i_naul_type )
    {
    	if ( h->fdec->fdec_data->b_temporal_layer_non_reference_flag )
    	{
    		slice->i_naul_type = NAL_SLICE_TRAIL_N ;
    	}
    }
    x265_slice_decoding_refresh_marking ( h, slice, &enc_gop->i_poc_cra, &enc_gop->b_refresh_pending ) ;
    x265_slice_select_reference_picture_set ( h, slice, h->fenc->i_poc, h->enc_top.i_gop_id ) ;
    slice->rps->i_number_of_longterm_pictures = 0 ;

    if ( x265_slice_check_that_all_ref_pics_are_available ( h, slice, slice->rps, 1, 0 ) )
    {
    	x265_slice_create_explicit_reference_picture_set_from_reference ( h, slice,
    														slice->rps ) ;
    }
    x265_slice_apply_reference_picture_set ( h, slice, slice->rps ) ;

    if ( h->fdec->fdec_data->i_t_layer > 0 )
    {
    	if ( x265_slice_is_temporal_layer_switching_point ( h, slice )
    		|| h->sps[0].b_temporal_id_nesting_flag )
    	{
    		if ( h->fdec->fdec_data->b_temporal_layer_non_reference_flag )
    		{
    			slice->i_naul_type = NAL_SLICE_TSA_N ;
    		}
    		else
    		{
    			slice->i_naul_type = NAL_SLICE_TLA ;
    		}
    	}
    	else if ( x265_slice_is_stepwise_temporal_layer_switching_point_candidate ( h, slice ) )
    	{
    		i_sstsa = 1 ;
    		for( i = h->enc_top.i_gop_id + 1 ;
        		( i < h->param.gop.i_gop_size && i_sstsa ) ;
        		++ i )
    		{
    			i_l_tid = h->param.gop.gop_list[i].i_temporal_id ;
    			if ( i_l_tid == h->fdec->fdec_data->i_t_layer )
    			{
    				rps = &h->sps[0].rps_list.reference_picture_sets[i] ;
    				for( j = 0 ; j < rps->i_number_of_pictures ; ++ j )
    				{
    					if ( rps->used[j] )
    					{
    						i_poc = h->param.gop.gop_list[i].i_poc + rps->delta_poc[j] ;
    						for ( k =0 ; k < h->param.gop.i_gop_size ; ++ k )
    						{
    							if ( i_poc == h->param.gop.gop_list[k].i_poc )
    							{
    								break ;
    							}
    						}
    						i_t_tid = h->param.gop.gop_list[k].i_temporal_id ;
    						if ( i_t_tid >= h->fdec->fdec_data->i_t_layer )
    						{
    							i_sstsa=0;
    							break;
    						}
    					}
    				}
    			}
        	}
    		if ( i_sstsa )
    		{
    			if(h->fdec->fdec_data->b_temporal_layer_non_reference_flag )
    			{
    				slice->i_naul_type = NAL_SLICE_STSA_N ;
    			}
    			else
    			{
    				slice->i_naul_type = NAL_SLICE_STSA_R ;
    			}
    		}
    	}
    }

    x265_slice_arrange_longterm_pictures_in_rps ( h, slice ) ;
    ref_pic_list_modification = &h->ref_pic_list_modification ;
    ref_pic_list_modification->b_ref_pic_list_modification_flag_l0 = 0 ;
    ref_pic_list_modification->b_ref_pic_list_modification_flag_l1 = 0 ;

    h->i_ref[REF_PIC_LIST_0] =
    		X265_MIN ( h->param.gop.gop_list[h->enc_top.i_gop_id].i_num_ref_pics_active,
    				slice->rps->i_number_of_pictures );
    h->i_ref[REF_PIC_LIST_1] =
    		X265_MIN ( h->param.gop.gop_list[h->enc_top.i_gop_id].i_num_ref_pics_active,
    				slice->rps->i_number_of_pictures );

    x265_slice_set_ref_pic_list ( h, slice ) ;

    if ( ( B_SLICE == slice->i_slice_type ) && ( 0 == h->i_ref[REF_PIC_LIST_1] ) )
    {
    	slice->i_slice_type = P_SLICE ;
    }

    if ( B_SLICE != slice->i_slice_type || !h->sps[0].b_use_l_comb )
    {
    	h->i_ref[REF_PIC_LIST_C] = 0 ;
    	h->b_ref_pic_list_combination_flag = 0 ;
    	h->b_ref_pic_list_modification_flag_lc = 0 ;
    }
    else
    {
    	h->b_ref_pic_list_combination_flag = h->sps[0].b_use_l_comb ;
    	h->i_ref[REF_PIC_LIST_C] = h->i_ref[REF_PIC_LIST_0] ;

    }

    if ( B_SLICE == slice->i_slice_type )
    {
		slice->i_col_from_l0_flag = 1 - i_col_dir ;
		b_low_delay = 1 ;
		i_curr_poc  = h->fdec->i_poc;
		i_refIdx = 0 ;

		for (i_refIdx = 0; i_refIdx < h->i_ref[REF_PIC_LIST_0] && b_low_delay; i_refIdx++)
		{
			if ( h->fref[REF_PIC_LIST_0][i_refIdx]->i_poc > i_curr_poc )
			{
				b_low_delay = 0;
			}
		}
		for (i_refIdx = 0; i_refIdx < h->i_ref[REF_PIC_LIST_1] && b_low_delay; i_refIdx++)
		{
			if ( h->fref[REF_PIC_LIST_1][i_refIdx]->i_poc > i_curr_poc )
			{
				b_low_delay = 0;
			}
		}

		slice->b_check_ldc = b_low_delay ;
    }

    i_col_dir = 1-i_col_dir;

    x265_slice_set_ref_poc_list ( h ) ;

    slice->b_no_back_pred_flag = 0  ;
    if ( B_SLICE == slice->i_slice_type && !h->b_ref_pic_list_combination_flag )
    {
    	if ( h->i_ref[REF_PIC_LIST_0] == h->i_ref[REF_PIC_LIST_1] )
    	{
    		slice->b_no_back_pred_flag = 1 ;
    		for ( loop = 0 ; loop < h->i_ref[REF_PIC_LIST_1]; ++ loop )
    		{
    			if ( h->fdec->fdec_data->ref_poc[REF_PIC_LIST_1][loop] != h->fdec->fdec_data->ref_poc[REF_PIC_LIST_0][loop] )
    			{
    				slice->b_no_back_pred_flag = 0 ;
    				break;
    			}
    		}
    	}
    }

    if ( slice->b_no_back_pred_flag )
    {
    	h->i_ref[REF_PIC_LIST_C] = 0 ;
    }
    x265_slice_generate_combined_list ( h ) ;

    if ( 2 == h->param.sps.i_tmvp_mode_id )
    {
    	if (0 == h->enc_top.i_gop_id )
    	{
    		slice->b_enable_tmvp_flag = 0 ;
    	}
    	else
    	{
    		slice->b_enable_tmvp_flag = 1 ;
    	}
    }
    else if (1 == h->param.sps.i_tmvp_mode_id )
    {
    	slice->b_enable_tmvp_flag = 1 ;
    }
    else
    {
    	slice->b_enable_tmvp_flag = 0 ;
    }

    if ( h->param.b_use_asr )
    {
    	x265_enc_slice_set_search_range ( h, &h->enc_slice, slice ) ;
    }

    if ( B_SLICE == slice->i_slice_type )
    {
    	if ( h->i_ref[REF_PIC_LIST_0] == h->i_ref[REF_PIC_LIST_1] )
    	{
    		b_gpb_check = 1 ;
    		for ( loop = 0 ; loop <  h->i_ref[REF_PIC_LIST_1] ; ++ loop )
    		{
    			if ( h->fdec->fdec_data->ref_poc[REF_PIC_LIST_0][loop] != h->fdec->fdec_data->ref_poc[REF_PIC_LIST_1][loop] )
    			{
    				b_gpb_check=0;
    				break;
    			}
    		}
    	}
    }
    if(b_gpb_check)
    {
    	slice->b_l_mvd_l1_zero = 1 ;
    }
    else
    {
    	slice->b_l_mvd_l1_zero = 0 ;
    }
    h->slice->b_l_mvd_l1_zero = slice->b_l_mvd_l1_zero ;

    i_num_slices = 1;
    i_internal_address = h->cu.pic.i_num_partitions - 4 ;
    i_external_address = h->cu.pic.i_num_cus_in_frame - 1 ;
    i_pos_x = ( i_external_address % h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_width
    		+ h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_internal_address] ];
    i_pos_y = ( i_external_address / h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_height
    		+ h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_internal_address] ];
    i_width = h->sps[0].i_pic_width_in_luma_samples ;
    i_height = h->sps[0].i_pic_height_in_luma_samples ;
    while(i_pos_x>=i_width||i_pos_y>=i_height)
    {
    	i_internal_address--;
    	i_pos_x = ( i_external_address % h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_width
    			+ h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_internal_address] ];
    	i_pos_y = ( i_external_address / h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_height
    			+ h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_internal_address] ];
    }
    i_internal_address++;
    if ( i_internal_address == h->cu.pic.i_num_partitions )
    {
    	i_internal_address = 0;
    	i_external_address++;
    }
    i_read_end_address = i_external_address * h->cu.pic.i_num_partitions + i_internal_address ;

    i_num_substreams = h->pps[0].i_num_substreams ;

    create_wpp_coders ( h, i_num_substreams ) ;
    pc_sbac_coders = h->pc_sbac_coders ;
	NOCHECKED_MALLOCZERO ( pc_substreams_out, i_num_substreams * sizeof ( x265_output_bitstream_t *) ) ;
	for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		pc_substreams_out[loop] = (x265_output_bitstream_t*)x265_output_bitstream_new () ;
	}

    i_start_cu_addr_slice_idx = 0 ;
    i_start_cu_addr_slice    = 0 ;
    slice->i_slice_cur_start_cu_addr = i_start_cu_addr_slice ;
    x265_uint_list_clear(enc_gop->stored_start_cu_addr_for_encoding_slice) ;

    i_start_cu_addr_slice_segment_idx = 0 ;
    i_start_cu_addr_slice_segment    = 0 ;
    slice->i_slice_segment_cur_start_cu_addr = i_start_cu_addr_slice_segment ;

    x265_uint_list_clear(enc_gop->stored_start_cu_addr_for_encoding_slice_segment);
    i_next_cu_addr = 0;
    x265_uint_list_push_back(enc_gop->stored_start_cu_addr_for_encoding_slice,
    						i_next_cu_addr );
    i_start_cu_addr_slice_idx++;
    x265_uint_list_push_back(enc_gop->stored_start_cu_addr_for_encoding_slice_segment,
    						i_next_cu_addr );
    i_start_cu_addr_slice_segment_idx++;


    slice->b_next_slice = 1 ;
    h->loop_filter.b_lf_cross_tile_boundary = h->pps[0].b_loop_filter_across_tiles_enabled_flag ;
    if ( h->sps[0].b_use_sao )
    {
    	x265_enc_sample_adaptive_offset_start_sao_enc ( h, &h->enc_sao ) ;
    }

    x265_nal_start ( h,
    				x265_enc_gop_get_nal_unit_type ( h, &h->enc_gop, h->fenc->i_poc ) ) ;

    x265_slice_header_write ( h, &h->out.bs, slice ) ;

    //	x265_slice_print ( h, slice ) ;
    //	print_enc_slice ( h, &h->enc_slice ) ;
    while(i_next_cu_addr<i_read_end_address)
    {
		slice->b_next_slice = 0 ;
		slice->b_next_slice_segment = 0 ;
		x265_enc_slice_pre_compress_slice ( h ) ;

		x265_slice_alloc_substream_sizes ( slice, i_num_substreams ) ;
		for ( loop = 0 ; loop < i_num_substreams ; ++loop )
		{
			x265_output_bitstream_clear ( pc_substreams_out[loop] ) ;
		}

		i_one_bitstream_per_slice_length = 0 ;
		pc_bitstream_redirect->write_alignOne ( (x265_bit_if_t*)pc_bitstream_redirect ) ;

		enc_sbac_init( &h->enc_sbac, (x265_enc_bin_if_t*)&h->enc_bin_cabac );
		x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
											(x265_enc_entropy_if_t*)&h->enc_sbac,
											slice );
		x265_enc_entropy_reset_entropy ( h,
										&h->enc_entropy,
										slice->i_slice_type,
										slice->i_slice_qp );
		for ( loop = 0 ; loop < h->pps[0].i_num_substreams ; ++ loop )
		{
			x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
												(x265_enc_entropy_if_t*)pc_sbac_coders[loop],
												slice );
			x265_enc_entropy_reset_entropy ( h,
											&h->enc_entropy,
											slice->i_slice_type,
											slice->i_slice_qp );
		}

		enc_sbac_init(&h->enc_sbac, (x265_enc_bin_if_t*)&h->enc_bin_cabac );
		for ( loop = 0 ; loop < h->pps[0].i_num_substreams ; ++ loop )
		{
			x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
												(x265_enc_entropy_if_t*)pc_sbac_coders[loop],
												slice );
			x265_enc_entropy_reset_entropy ( h,
											&h->enc_entropy,
											slice->i_slice_type,
											slice->i_slice_qp );
		}
		x265_enc_sbac_load ( pc_sbac_coders[0], &h->enc_sbac );
		x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
											(x265_enc_entropy_if_t*)pc_sbac_coders[0],
											slice );
		x265_enc_entropy_reset_entropy ( h,
										&h->enc_entropy,
										slice->i_slice_type,
										slice->i_slice_qp );
		x265_enc_entropy_set_bitstream ( &h->enc_entropy,
										(x265_bit_if_t*)pc_bitstream_redirect );
		x265_enc_entropy_set_bitstream ( &h->enc_entropy,
										(x265_bit_if_t*)pc_substreams_out[0] );

		x265_enc_sbac_load ( &h->enc_sbac, pc_sbac_coders[0] );
		slice->i_tile_offst_for_mult_es = i_one_bitstream_per_slice_length ;

		x265_enc_slice_compress_and_encode_cu (h,
												pc_bitstream_redirect,
												pc_substreams_out) ;


		b_no_bin_bit_constraint_violated = (!slice->b_next_slice && !slice->b_next_slice_segment);
		if ( slice->b_next_slice
				|| (b_no_bin_bit_constraint_violated && h->param.i_slice_mode == FIXED_NUMBER_OF_LCU ) )
		{
			i_start_cu_addr_slice = slice->i_slice_cur_end_cu_addr ;
		    x265_uint_list_push_back(enc_gop->stored_start_cu_addr_for_encoding_slice,
		    						i_start_cu_addr_slice );
		    i_start_cu_addr_slice_idx++;
			if (i_start_cu_addr_slice_segment_idx > 0
					&& x265_uint_list_get_element(enc_gop->stored_start_cu_addr_for_encoding_slice_segment,
												i_start_cu_addr_slice_segment_idx-1) != i_start_cu_addr_slice)
			{
			    x265_uint_list_push_back(enc_gop->stored_start_cu_addr_for_encoding_slice_segment,
			    						i_start_cu_addr_slice );
			    i_start_cu_addr_slice_segment_idx++;
			}

			if (i_start_cu_addr_slice < i_read_end_address)
			{
				h->enc_slice.i_slice_idx = i_start_cu_addr_slice_idx - 1 ;
				slice = h->slice ;
				x265_slice_copy_slice_info ( slice, h->slice ) ;
				slice->i_slice_idx = i_start_cu_addr_slice_idx - 1 ;
				slice->i_slice_cur_start_cu_addr = i_start_cu_addr_slice ;
				slice->i_slice_segment_cur_start_cu_addr = i_start_cu_addr_slice ;
				slice->i_slice_bits = 0 ;
				i_num_slices ++;
			}
		}
		else if (slice->b_next_slice_segment
				|| (b_no_bin_bit_constraint_violated && h->param.i_slice_segment_mode == FIXED_NUMBER_OF_LCU ) )
		{
			i_start_cu_addr_slice_segment = slice->i_slice_segment_cur_end_cu_addr ;
		    x265_uint_list_push_back(enc_gop->stored_start_cu_addr_for_encoding_slice_segment,
		    						i_start_cu_addr_slice_segment );
		    i_start_cu_addr_slice_segment_idx++;
		    slice->i_slice_segment_cur_start_cu_addr = i_start_cu_addr_slice_segment ;
		}
		else
		{
			i_start_cu_addr_slice = slice->i_slice_cur_end_cu_addr ;
			i_start_cu_addr_slice_segment = slice->i_slice_segment_cur_end_cu_addr ;
		}
		i_next_cu_addr = (i_start_cu_addr_slice > i_start_cu_addr_slice_segment)
						? i_start_cu_addr_slice : i_start_cu_addr_slice_segment;


	    substream_sizes = slice->substream_sizes ;
	    i_total_coded_size = 0 ;
	    i_num_substreams_per_tile = i_num_substreams;
	    if (i_num_substreams > 1)
	    {
	    	i_num_substreams_per_tile /= (h->param.pps.i_num_columns_minus_1 + 1) * (h->param.pps.i_num_rows_minus_1 + 1);
	    }
	    for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	    {
			x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
												(x265_enc_entropy_if_t*)pc_sbac_coders[loop],
												slice );
			x265_enc_entropy_set_bitstream ( &h->enc_entropy,
											(x265_bit_if_t*)pc_substreams_out[loop] );

			x265_enc_entropy_encode_terminating_bit ( &h->enc_entropy, 1 ) ;
			x265_enc_entropy_encode_slice_finish ( &h->enc_entropy ) ;

	    	x265_output_bitstream_write_byte_alignment ( pc_substreams_out[loop] ) ;

	    	i_total_coded_size += pc_substreams_out[loop]->get_number_of_written_bits((x265_bit_if_t*)pc_substreams_out[loop]) ;

	    	b_next_substream_in_new_tile = ((loop+1) < i_num_substreams) && ((loop+1)%i_num_substreams_per_tile == 0);
	    	if ( b_next_substream_in_new_tile )
	    	{
	    		x265_slice_set_tile_location ( slice,
	    										loop / i_num_substreams_per_tile,
	    										slice->i_tile_offst_for_mult_es + (i_total_coded_size>>3) );
	        }
	    	if (loop+1 < h->pps[0].i_num_substreams )
	    	{
	    		substream_sizes[loop] = pc_substreams_out[loop]->get_number_of_written_bits((x265_bit_if_t*)pc_substreams_out[loop]);
	    	}
	    }

		x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
											(x265_enc_entropy_if_t*)&h->enc_cavlc,
											slice );

		x265_enc_entropy_encode_tiles_wpp_entry_point( h, &h->enc_entropy, &h->out.bs, slice ) ;

	    i_offs = 0;
	    i_nss = h->pps[0].i_num_substreams ;
	    if ( h->pps[0].b_entropy_coding_sync_enabled_flag )
	    {
	    	i_offs = slice->i_slice_segment_cur_start_cu_addr / h->cu.pic.i_num_partitions / h->cu.pic.i_width_in_cu ;
	    	i_nss  = slice->i_num_entry_point_offsets + 1 ;
	    }
	    for ( loop = 0 ; loop < i_nss; ++loop )
	    {
	    	x265_output_bitstream_add_stream ( pc_bitstream_redirect,
	    										pc_substreams_out[loop + i_offs] ) ;
	    }

	    //	x265_output_bitstream_print ( &h->out.bs, "SliceHeader2" ) ;
	    x265_enc_gop_x_write_tile_location_to_slice_header ( h,
	    													&h->out.bs,
	    													&pc_bitstream_redirect,
	    													slice ) ;
	    i_one_bitstream_per_slice_length += bs_pos ( &h->out.bs ) ;
    }
    if ( h->sps[0].b_use_sao )
    {
    	x265_enc_sample_adaptive_offset_end_sao_enc ( h, &h->enc_sao ) ;
    }

    x265_frame_compress_motion ( h, h->fdec ) ;

    frame_size = x265_encoder_frame_end ( h, pp_nal, pi_nal, pic_out ) ;

    x265_enc_gop_x_calculate_add_psnr(h, &h->enc_gop, frame_size, h->out.nal[0].b_long_start_code) ;
    printf("\n");
    fflush(stdout);

    if ( frame_size < 0 )
    {
    	goto fail ;
    }

    for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		x265_output_bitstream_delete ( (x265_bit_if_t*)pc_substreams_out[loop] ) ;
	}
	x265_free ( pc_substreams_out ) ;
	x265_output_bitstream_delete ( (x265_bit_if_t*)pc_bitstream_redirect ) ;
	pc_bitstream_redirect = NULL ;

    //    fprintf ( stderr, "POC = %d\t%d\t%d\n", fenc->i_poc, (int)fenc->i_dts , (int)fenc->i_pts ) ;


	return frame_size ;

fail:
	for ( loop = 0 ; loop < i_num_substreams ; ++ loop )
	{
		x265_output_bitstream_delete ( (x265_bit_if_t*)pc_substreams_out[loop] ) ;
	}
	x265_free ( pc_substreams_out ) ;
	x265_output_bitstream_delete ( (x265_bit_if_t*)pc_bitstream_redirect ) ;
	pc_bitstream_redirect = NULL ;

	return -1 ;

}

void x265_enc_gop_print_out_summary( x265_t *h,
									x265_enc_gop_t *enc_gop,
									uint32_t i_num_all_pic_coded )
{
	assert (i_num_all_pic_coded == enc_gop->enc_analyze_all.i_num_pic );

	x265_enc_analyze_set_frm_rate(&enc_gop->enc_analyze_all, h->param.i_frame_rate) ;
	x265_enc_analyze_set_frm_rate(&enc_gop->enc_analyze_i, h->param.i_frame_rate) ;
	x265_enc_analyze_set_frm_rate(&enc_gop->enc_analyze_p, h->param.i_frame_rate) ;
	x265_enc_analyze_set_frm_rate(&enc_gop->enc_analyze_b, h->param.i_frame_rate) ;


	//-- all
	printf( "\n\nSUMMARY --------------------------------------------------------\n" );
	x265_enc_analyze_print_out(&enc_gop->enc_analyze_all, 'a') ;

	printf( "\n\nI Slices--------------------------------------------------------\n" );
	x265_enc_analyze_print_out(&enc_gop->enc_analyze_i, 'i') ;

	printf( "\n\nP Slices--------------------------------------------------------\n" );
	x265_enc_analyze_print_out(&enc_gop->enc_analyze_p, 'p') ;

	printf( "\n\nB Slices--------------------------------------------------------\n" );
	x265_enc_analyze_print_out(&enc_gop->enc_analyze_b, 'b') ;

#if X265_SUMMARY_OUT_
	x265_enc_analyze_print_summary_out(&enc_gop->enc_analyze_all) ;
#endif
#if X265_SUMMARY_PIC_
	x265_enc_analyze_print_summary(&enc_gop->enc_analyze_i, 'I') ;
	x265_enc_analyze_print_summary(&enc_gop->enc_analyze_p, 'P') ;
	x265_enc_analyze_print_summary(&enc_gop->enc_analyze_b, 'B') ;
#endif

	printf("\nRVM: %.3lf\n" ,
			x265_enc_gop_x_calculate_rvm(h, enc_gop) );
}


void x265_enc_gop_x_write_tile_location_to_slice_header ( x265_t *h,
															bs_t *s,
															x265_output_bitstream_t **ppc_bitstream_redirect,
															x265_slice_t *slice )
{
	x265_output_bitstream_t *pc_bitstream_redirect = NULL ;

	pc_bitstream_redirect = *ppc_bitstream_redirect ;
	bs_write ( s, 1, 1 ) ;
	bs_flush ( s ) ;

	bs_add_stream ( s, &pc_bitstream_redirect->bs ) ;
	x265_enc_entropy_set_bitstream ( &h->enc_entropy, NULL );

	x265_output_bitstream_delete ( (x265_bit_if_t*)pc_bitstream_redirect ) ;
	pc_bitstream_redirect = (x265_output_bitstream_t*)x265_output_bitstream_new () ;
	*ppc_bitstream_redirect = pc_bitstream_redirect ;

}


void x265_enc_gop_x_calculate_add_psnr( x265_t *h,
										x265_enc_gop_t *enc_gop,
										int32_t i_frame_size,
										int32_t b_long_start_code )
{
	char c = 0 ;
	int32_t x, y;
	uint64_t i_ssd_y  = 0;
	uint64_t i_ssd_u  = 0;
	uint64_t i_ssd_v  = 0;
	double f_y_psnr  = 0.0;
	double f_u_psnr  = 0.0;
	double f_v_psnr  = 0.0;
	pixel *p_org = NULL;
	pixel *p_rec = NULL;
	int32_t i_stride = 0;
	int32_t i_width = 0;
	int32_t i_height = 0;
	int32_t i_size = 0;
	int32_t i_diff = 0;
	int32_t i_max_val_y = 0;
	int32_t i_max_val_c = 0;
	double f_ref_value_y = 0.0;
	double f_ref_value_c = 0.0;
	uint32_t i_num_rbsp_bytes = 0;
	uint32_t i_bits = 0;
	int32_t i_ref_list = 0;
	int32_t i_ref_index = 0;
	x265_slice_t *slice = NULL;




	//===== calculate psnr =====
	p_org = x265_frame_get_luma_addr_p2(h, h->fenc);
	p_rec = x265_frame_get_luma_addr_p2(h, h->fdec);
	i_stride = h->fdec->i_stride[0];

	i_width  = h->param.i_width - h->param.i_pad[0];
	i_height = h->param.i_height - h->param.i_pad[1];

	i_size   = i_width*i_height;

	for( y = 0; y < i_height; y++ )
	{
		for( x = 0; x < i_width; x++ )
	    {
			i_diff = (int32_t)( p_org[x] - p_rec[x] );
			i_ssd_y   += i_diff * i_diff;
	    }
		p_org += i_stride;
	    p_rec += i_stride;
	}

	i_height >>= 1;
	i_width  >>= 1;
	i_stride >>= 1;
	p_org = x265_frame_get_cb_addr_p2(h, h->fenc);
	p_rec = x265_frame_get_cb_addr_p2(h, h->fdec);

	for( y = 0; y < i_height; y++ )
	{
		for( x = 0; x < i_width; x++ )
		{
			i_diff = (int32_t)( p_org[x] - p_rec[x] );
			i_ssd_u   += i_diff * i_diff;
		}
		p_org += i_stride;
		p_rec += i_stride;
	}

	p_org = x265_frame_get_cr_addr_p2(h, h->fenc);
	p_rec = x265_frame_get_cr_addr_p2(h, h->fdec);

	for( y = 0; y < i_height; y++ )
	{
		for( x = 0; x < i_width; x++ )
		{
			i_diff = (int32_t)( p_org[x] - p_rec[x] );
			i_ssd_v   += i_diff * i_diff;
		}
		p_org += i_stride;
		p_rec += i_stride;
	}

	i_max_val_y = 255 << (h->param.sps.i_bit_depth_y-8);
	i_max_val_c = 255 << (h->param.sps.i_bit_depth_c-8);
	f_ref_value_y = (double) i_max_val_y * i_max_val_y * i_size;
	f_ref_value_c = (double) i_max_val_c * i_max_val_c * i_size / 4.0;
	f_y_psnr = ( i_ssd_y ? 10.0 * log10( f_ref_value_y / (double)i_ssd_y ) : 99.99 );
	f_u_psnr = ( i_ssd_u ? 10.0 * log10( f_ref_value_c / (double)i_ssd_u ) : 99.99 );
	f_v_psnr = ( i_ssd_v ? 10.0 * log10( f_ref_value_c / (double)i_ssd_v ) : 99.99 );

	/* calculate the size of the access unit, excluding:
	 *  - any annex_b contributions (start_code_prefix, zero_byte, etc.,)
	 *  - sei nal units
	 */
	i_num_rbsp_bytes = i_frame_size;
	if ( b_long_start_code )
	{
		i_num_rbsp_bytes -= 4;
	}
	else
	{
		i_num_rbsp_bytes -= 4;
	}

	i_bits = i_num_rbsp_bytes * 8;


	//===== add psnr =====
	x265_enc_analyze_add_result(&enc_gop->enc_analyze_all,
								f_y_psnr,
								f_u_psnr,
								f_v_psnr,
								(double)i_bits);
	slice = h->slice;
	if (I_SLICE == slice->i_slice_type)
	{
		x265_enc_analyze_add_result(&enc_gop->enc_analyze_i,
									f_y_psnr,
									f_u_psnr,
									f_v_psnr,
									(double)i_bits);
	}
	if (P_SLICE == slice->i_slice_type)
	{
		x265_enc_analyze_add_result(&enc_gop->enc_analyze_p,
									f_y_psnr,
									f_u_psnr,
									f_v_psnr,
									(double)i_bits);
	}
	if (B_SLICE == slice->i_slice_type)
	{
		x265_enc_analyze_add_result(&enc_gop->enc_analyze_b,
									f_y_psnr,
									f_u_psnr,
									f_v_psnr,
									(double)i_bits);
	}

	c = ((I_SLICE == slice->i_slice_type) ? 'I' : (P_SLICE == slice->i_slice_type) ? 'P' : 'B');
	if ( !h->fdec->fdec_data->b_reference )
	{
		c += 32;
	}

#if X265_ADAPTIVE_QP_SELECTION
	printf("POC %4d TId: %1d ( %c-SLICE, nQP %d QP %d ) %10d bits",
			h->fdec->i_poc,
			h->fdec->fdec_data->i_t_layer,
			c,
			slice->i_slice_qp_base,
			slice->i_slice_qp,
			i_bits );
#else
	printf("POC %4d TId: %1d ( %c-SLICE, QP %d ) %10d bits",
			h->fdec->i_poc,
			h->fdec->fdec_data->i_t_layer,
			c,
			slice->i_slice_qp,
			i_bits );
#endif

	printf(" [Y %6.4lf dB    U %6.4lf dB    V %6.4lf dB]", f_y_psnr, f_u_psnr, f_v_psnr );
	//	printf(" [ET %5.0f ]", dEncTime );

	for ( i_ref_list = 0; i_ref_list < 2; i_ref_list++)
	{
		printf(" [L%d ", i_ref_list);
		for ( i_ref_index = 0;
				i_ref_index < h->i_ref[((enum ref_pic_list_e)(i_ref_list))];
				i_ref_index++)
		{
			printf ("%d ", h->fdec->fdec_data->ref_poc[((enum ref_pic_list_e)(i_ref_list))][i_ref_index] - slice->i_last_idr);
		}
		printf("]");
	}

}


double x265_enc_gop_x_calculate_rvm ( x265_t *h, x265_enc_gop_t *enc_gop )
{
	double f_rvm = 0.0 ;

	if( 1 == h->param.gop.i_gop_size
			&& 1 != h->param.gop.i_intra_period
			&& h->param.i_frame_total > X265_RVM_VCEGAM10_M * 2 )
	{
		// calculate RVM only for lowdelay configurations
		/*
		std::vector<Double> vRL , vB;
		size_t N = m_vRVM_RP.size();
		vRL.resize( N );
		vB.resize( N );

		Int i;
		Double dRavg = 0 , dBavg = 0;
		vB[RVM_VCEGAM10_M] = 0;
		for( i = RVM_VCEGAM10_M + 1 ; i < N - RVM_VCEGAM10_M + 1 ; i++ )
		{
		  vRL[i] = 0;
		  for( Int j = i - RVM_VCEGAM10_M ; j <= i + RVM_VCEGAM10_M - 1 ; j++ )
			vRL[i] += m_vRVM_RP[j];
		  vRL[i] /= ( 2 * RVM_VCEGAM10_M );
		  vB[i] = vB[i-1] + m_vRVM_RP[i] - vRL[i];
		  dRavg += m_vRVM_RP[i];
		  dBavg += vB[i];
		}

		dRavg /= ( N - 2 * RVM_VCEGAM10_M );
		dBavg /= ( N - 2 * RVM_VCEGAM10_M );

		Double dSigamB = 0;
		for( i = RVM_VCEGAM10_M + 1 ; i < N - RVM_VCEGAM10_M + 1 ; i++ )
		{
		  Double tmp = vB[i] - dBavg;
		  dSigamB += tmp * tmp;
		}
		dSigamB = sqrt( dSigamB / ( N - 2 * RVM_VCEGAM10_M ) );

		Double f = sqrt( 12.0 * ( RVM_VCEGAM10_M - 1 ) / ( RVM_VCEGAM10_M + 1 ) );

		dRVM = dSigamB / dRavg * f;
		*/
	}

	return( f_rvm );
}



