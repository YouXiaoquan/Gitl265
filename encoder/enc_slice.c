

#include "common/common.h"


double x265_enc_slice_get_qp_value_according_to_lambda ( double f_lambda ) ;

x265_enc_slice_t *x265_enc_slice_new ( x265_t *h )
{
	x265_enc_slice_t *enc_slice = NULL ;

	CHECKED_MALLOC( enc_slice, sizeof(x265_enc_slice_t) );
	if ( x265_enc_slice_init ( h, enc_slice ) )
	{
		goto fail ;
	}

	return enc_slice ;
fail:
	x265_enc_slice_delete ( (void*) enc_slice ) ;
	return NULL ;
}


void x265_enc_slice_delete ( x265_enc_slice_t *enc_slice )
{
	x265_enc_slice_deinit ( enc_slice ) ;
	x265_free ( enc_slice ) ;
}


int x265_enc_slice_init ( x265_t *h, x265_enc_slice_t *enc_slice )
{
	memset ( enc_slice, 0, sizeof(x265_enc_slice_t) ) ;
	if ( x265_enc_slice_create ( h, enc_slice ) )
	{
		goto fail ;
	}
    return 0 ;
fail:
	x265_enc_slice_deinit ( (void*) enc_slice ) ;
	return -1 ;
}

void x265_enc_slice_deinit ( x265_enc_slice_t *enc_slice )
{
	x265_enc_slice_destroy ( (void*) enc_slice ) ;
}

int x265_enc_slice_create ( x265_t *h, x265_enc_slice_t *enc_slice )
{
	CHECKED_MALLOCZERO( enc_slice->rd_pic_lambda,
						(h->param.rdo.i_delta_qp_rd * 2 + 1 ) * sizeof(double) ) ;
	CHECKED_MALLOCZERO( enc_slice->f_rd_pic_qp,
						(h->param.rdo.i_delta_qp_rd * 2 + 1 ) * sizeof(double) ) ;
	CHECKED_MALLOCZERO( enc_slice->rd_pic_qp,
						(h->param.rdo.i_delta_qp_rd * 2 + 1 ) * sizeof(int32_t) ) ;
    return 0 ;
fail:
	x265_enc_slice_destroy ( (void*) enc_slice ) ;
	return -1 ;
}


void x265_enc_slice_destroy ( x265_enc_slice_t *enc_slice )
{
	x265_free ( enc_slice->rd_pic_qp ) ;
	x265_free ( enc_slice->f_rd_pic_qp ) ;
	x265_free ( enc_slice->rd_pic_lambda ) ;

	enc_slice->rd_pic_qp = NULL ;
	enc_slice->f_rd_pic_qp = NULL ;
	enc_slice->rd_pic_lambda = NULL ;
}


void x265_enc_slice_print ( x265_t *h, x265_enc_slice_t *enc_slice )
{
	int32_t loop = 0 ;
	FILE *file = NULL ;
	static int32_t b_first_print_enc_slice = 1 ;
	static int32_t i_enc_slice_number = 1 ;

	if ( b_first_print_enc_slice )
	{
		file = fopen ( "EncSlice2", "wb" ) ;
		b_first_print_enc_slice = 0 ;
	}
	else
	{
		file = fopen ( "EncSlice2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	fprintf ( file, "EncSlice%d:\n", i_enc_slice_number ) ;
	i_enc_slice_number ++ ;

	print_weight_pred_analysis ( ((x265_weight_pred_analysis_t*)(&h->enc_slice)),
								file ) ;
	fprintf ( file, "\n" ) ;


	for ( loop = 0 ; loop < 2 * h->param.rdo.i_delta_qp_rd + 1; ++ loop )
	{
		print_double ( enc_slice->rd_pic_lambda[loop], file ) ;
		fprintf ( file, "\n" ) ;
	}
	for ( loop = 0 ; loop < 2 * h->param.rdo.i_delta_qp_rd + 1; ++ loop )
	{
		print_double ( enc_slice->f_rd_pic_qp[loop], file ) ;
		fprintf ( file, "\n" ) ;
	}
	for ( loop = 0 ; loop < 2 * h->param.rdo.i_delta_qp_rd + 1; ++ loop )
	{
		fprintf ( file, "%d ", enc_slice->rd_pic_qp[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	print_double ( enc_slice->f_pic_rd_cost, file ) ;
	fprintf ( file, "\n" ) ;

	fprintf ( file, "%d ", (uint32_t)enc_slice->i_pic_total_bits ) ;
	fprintf ( file, "%d ", (uint32_t)enc_slice->i_pic_dist ) ;
	fprintf ( file, "%d ", enc_slice->i_slice_idx ) ;
	fprintf ( file, "%d ", enc_slice->i_bits_originally_in_substreams ) ;
	fprintf ( file, "%d ", enc_slice->i_start_cu_addr ) ;
	fprintf ( file, "%d ", enc_slice->i_bounding_cu_addr ) ;
	fprintf ( file, "\n" ) ;

	print_double ( h->rd_cost.f_lambda, file ) ;
	fprintf ( file, "\n" ) ;
	print_double ( h->rd_cost.f_sqrt_lambda, file ) ;
	fprintf ( file, "\n" ) ;
	fprintf ( file, "%d ", h->rd_cost.i_lambda_motion_sad ) ;
	fprintf ( file, "%d ", h->rd_cost.i_lambda_motion_sse ) ;
	fprintf ( file, "\n" ) ;

	x265_tr_quant_print ( &h->tr_quant, file ) ;


	fclose ( file ) ;
}

void x265_enc_slice_initialize ( x265_t *h, x265_frame_t *frame,
								int32_t i_poc_last, int32_t i_poc_curr, int32_t i_num_pic_rcvd,
								int32_t i_gop_id, x265_slice_t **pp_slice )
{
	double f_qp;
	double f_lambda;
	int32_t i_depth = 0 ;
	int32_t i_poc = 0 ;
	int32_t i_step = 0 ;
	int32_t i = 0, j = 0 ;
	int32_t i_qp = 0 ;
	int32_t i_dqp_idx = 0 ;
	int32_t i_number_b_frames = 0 ;
	int32_t i_shift_qp = 0 ;
    int32_t i_bitdepth_luma_qp_scale = 0;
	int32_t i_qpc = 0 ;
	int32_t i_chroma_qp_offset = 0 ;

	double f_org_qp = 0.0 ;
	double f_lambda_scale = 0.0 ;
	double f_qp_temp = 0.0 ;
#if X265_FULL_NBIT
	double f_qp_temp_orig = 0.0 ;
#endif
	double f_qp_factor = 0.0 ;
	double f_weight = 0.0;
	enum slice_type_e i_slice_type ;
	x265_slice_t *slice = NULL ;

	slice = h->slice ;
	slice->i_pps_id = h->pps[0].i_pps_id ;
	slice->i_slice_bits = 0 ;
	x265_slice_initialize ( slice ) ;
	slice->b_pic_output_flag = 1 ;

	i_poc = h->fdec->i_poc % h->param.gop.i_gop_size ;
	if ( i_poc == 0 )
	{
		i_depth = 0;
	}
	else
	{
		i_step = h->param.gop.i_gop_size;
		i_depth = 0;
		for( i = i_step >> 1 ; i >= 1 ; i>>=1 )
		{
			for ( j = i ; j < h->param.gop.i_gop_size ; j += i_step )
			{
				if ( j == i_poc )
				{
					i=0;
					break;
				}
			}
			i_step >>= 1;
			i_depth++;
		}
	}

	i_slice_type = B_SLICE ;
	i_slice_type = (i_poc_last == 0
					|| i_poc_curr % h->param.gop.i_intra_period == 0
					|| h->param.gop.i_gop_size == 0) ? I_SLICE : i_slice_type;
	slice->i_slice_type = i_slice_type ;

	if ( 0 == i_poc_last )
	{
		h->fdec->fdec_data->b_temporal_layer_non_reference_flag = 0 ;
	}
	else
	{
		h->fdec->fdec_data->b_temporal_layer_non_reference_flag = h->param.gop.gop_list[i_gop_id].b_ref_pic ? 0 : 1 ;
	}

	f_qp = h->param.f_qp ;
	if ( I_SLICE != i_slice_type )
	{
		if (!(( h->param.rdo.i_max_delta_qp == 0 )
				&& (f_qp == - h->sps[0].i_qp_bd_offset_y )
				&& (h->sps[0].b_use_lossless)))
	    {
			f_qp += h->param.gop.gop_list[i_gop_id].i_qp_Offset ;
	    }
	}

#if !X265_RATE_CONTROL_LAMBDA_DOMAIN
	if ( h->param.b_enable_rate_ctrl )
	{
		f_qp = m_pcRateCtrl->getFrameQP(slice->isReferenced(), slice->getPOC());
	}
#endif
	// ------------------------------------------------------------------------------------------------------------------
	// Lambda computation
	// ------------------------------------------------------------------------------------------------------------------
	f_org_qp = f_qp;

	// pre-compute lambda and QP values for all possible QP candidates
	for ( i_dqp_idx = 0; i_dqp_idx < 2 * h->param.rdo.i_delta_qp_rd + 1; ++ i_dqp_idx )
	{
		// compute QP value
	    f_qp = f_org_qp + ((i_dqp_idx+1)>>1)*(i_dqp_idx%2 ? -1 : 1);

	    // compute lambda value
	    i_number_b_frames = ( h->param.gop.i_gop_size - 1 );
	    i_shift_qp = 12;
	    f_lambda_scale = 1.0 - x265_clip3_double( 0.05*(double)i_number_b_frames, 0.0, 0.5 );
#if X265_FULL_NBIT
	    i_bitdepth_luma_qp_scale = 6 * (g_bitDepth - 8);
#else
	    i_bitdepth_luma_qp_scale = 0;
#endif
	    f_qp_temp = (double) f_qp + i_bitdepth_luma_qp_scale - i_shift_qp;
#if X265_FULL_NBIT
	    f_qp_temp_orig = (double) f_qp - i_shift_qp;
#endif
	    // Case #1: I or P-slices (key-frame)
	    f_qp_factor = h->param.gop.gop_list[i_gop_id].f_qp_factor ;
	    if ( I_SLICE == i_slice_type )
	    {
	    	f_qp_factor=0.57*f_lambda_scale;
	    }
	    f_lambda = f_qp_factor*pow( 2.0, f_qp_temp/3.0 );

	    if ( i_depth > 0 )
	    {
#if X265_FULL_NBIT
	        f_lambda *= x265_clip3_double( (f_qp_temp_orig / 6.0), 2.00, 4.00 ); // (j == B_SLICE && p_cur_frm->layer != 0 )
#else
	        f_lambda *= x265_clip3_double( (f_qp_temp / 6.0), 2.00, 4.00 ); // (j == B_SLICE && p_cur_frm->layer != 0 )
#endif
	    }

	    // if hadamard is used in ME process
	    if ( !h->param.b_use_had_me && I_SLICE != slice->i_slice_type )
	    {
	    	f_lambda *= 0.95;
	    }

	    i_qp = X265_MAX ( -h->sps[0].i_qp_bd_offset_y,
	    				X265_MIN ( X265_MAX_QP, (int32_t) floor( f_qp + 0.5 ) ) );

	    h->enc_slice.rd_pic_lambda[i_dqp_idx] = f_lambda;
	    h->enc_slice.f_rd_pic_qp[i_dqp_idx] = f_qp;
	    h->enc_slice.rd_pic_qp[i_dqp_idx] = i_qp;
	}

	// obtain f_qp = 0 case
	f_lambda = h->enc_slice.rd_pic_lambda[0];
	f_qp = h->enc_slice.f_rd_pic_qp[0];
	i_qp = h->enc_slice.rd_pic_qp [0];

	if ( I_SLICE != slice->i_slice_type)
	{
		f_lambda *= h->param.rdo.f_ad_lambda_modifier[
		            h->param.gop.gop_list[i_gop_id].i_temporal_id ] ;
	}

	rd_cost_set_lambda ( &h->rd_cost, f_lambda ) ;
#if X265_WEIGHTED_CHROMA_DISTORTION
	f_weight = 1.0;
	i_chroma_qp_offset = h->pps[0].i_chroma_cb_qp_offset + slice->i_slice_qp_delta_cb ;
	i_qpc = x265_clip3_int32 ( i_qp + i_chroma_qp_offset, 0, 57 );
	f_weight = pow( 2.0, (i_qp-chroma_scale[i_qpc])/3.0 );  // takes into account of the chroma qp mapping and chroma qp Offset
	h->rd_cost.f_cb_distortion_weight = f_weight ;

	i_chroma_qp_offset = h->pps[0].i_chroma_cr_qp_offset + slice->i_slice_qp_delta_cr ;
	i_qpc = x265_clip3_int32 ( i_qp + i_chroma_qp_offset, 0, 57 );
	f_weight = pow( 2.0, (i_qp-chroma_scale[i_qpc])/3.0 );  // takes into account of the chroma qp mapping and chroma qp Offset
	h->rd_cost.f_cr_distortion_weight = f_weight ;
#endif

#if X265_RDOQ_CHROMA_LAMBDA
	// for RDOQ
	x265_tr_quant_set_lambda ( &h->tr_quant, f_lambda, f_lambda / f_weight );
#else
	x265_tr_quant_set_lambda ( &h->tr_quant, f_lambda );
#endif

#if X265_SAO_CHROMA_LAMBDA
	// For SAO
	x265_slice_set_lambda ( slice, f_lambda, f_lambda / f_weight );
#else
	x265_slice_set_lambda ( slice, f_lambda );
#endif


#if X265_HB_LAMBDA_FOR_LDC
	// restore original slice type
	i_slice_type = ( i_poc_last == 0 ||
					i_poc_curr % h->param.gop.i_intra_period == 0
					|| h->param.gop.i_gop_size == 0) ? I_SLICE : i_slice_type ;
	slice->i_slice_type = i_slice_type ;
#endif

	if ( h->param.b_recalculate_qp_according_to_lambda )
	{
		f_qp = x265_enc_slice_get_qp_value_according_to_lambda( f_lambda );
		i_qp = X265_MAX( -h->sps[0].i_qp_bd_offset_y
				, X265_MIN ( X265_MAX_QP, (int32_t) floor( f_qp + 0.5 ) ) );
	}

	slice->i_slice_qp = i_qp  ;
#if X265_ADAPTIVE_QP_SELECTION
	slice->i_slice_qp_base = i_qp ;
#endif

	slice->i_slice_qp_delta = 0 ;
	slice->i_slice_qp_delta_cb = 0 ;
	slice->i_slice_qp_delta_cr = 0 ;
	h->i_ref[REF_PIC_LIST_0] = h->param.gop.gop_list[i_gop_id].i_num_ref_pics_active ;
	h->i_ref[REF_PIC_LIST_1] = h->param.gop.gop_list[i_gop_id].i_num_ref_pics_active ;
	if ( h->pps[0].b_deblocking_filter_control_present_flag )
	{
		//h->pps[0].b_deblocking_filter_override_enabled_flag = !h->param.deblock.i_loop_filter_offset_in_pps ;
		slice->b_deblocking_filter_override_flag = !h->param.deblock.i_loop_filter_offset_in_pps ;
		//h->pps[0].b_pic_disable_deblocking_filter_flag = h->param.sps.b_loop_filter_disable  ;
		slice->b_deblocking_filter_disable = h->param.sps.b_loop_filter_disable ;

	    if ( ! slice->b_deblocking_filter_disable )
	    {
	    	if ( !h->param.deblock.i_loop_filter_offset_in_pps
	    			&& I_SLICE != i_slice_type )
	    	{
	    		h->pps[0].i_deblocking_filter_beta_offset_div2 =
	    			h->param.gop.gop_list[i_gop_id].i_beta_offset_div_2 +
	    			h->param.deblock.i_loop_filter_beta_offset_div_2 ;
	    		h->pps[0].i_deblocking_filter_tc_offset_div2 =
	    			h->param.gop.gop_list[i_gop_id].i_tc_offset_div_2 +
	    			h->param.deblock.i_loop_filter_tc_offset_div_2 ;
	    		slice->i_deblocking_filter_beta_offset_div2 =
	    			h->param.gop.gop_list[i_gop_id].i_beta_offset_div_2 +
	    			h->param.deblock.i_loop_filter_beta_offset_div_2 ;
	    		slice->i_deblocking_filter_tc_offset_div2 =
	    			h->param.gop.gop_list[i_gop_id].i_tc_offset_div_2 +
	    			h->param.deblock.i_loop_filter_tc_offset_div_2 ;
	    	}
	    	else
	    	{
	    		h->pps[0].i_deblocking_filter_beta_offset_div2 =
	    			h->param.deblock.i_loop_filter_beta_offset_div_2 ;
	    		h->pps[0].i_deblocking_filter_tc_offset_div2 =
	    			h->param.deblock.i_loop_filter_tc_offset_div_2 ;
	    		slice->i_deblocking_filter_beta_offset_div2 =
	    			h->param.deblock.i_loop_filter_beta_offset_div_2 ;
	    		slice->i_deblocking_filter_tc_offset_div2 =
	    			h->param.deblock.i_loop_filter_tc_offset_div_2 ;
	    	}
	    }
	}
	else
	{
		slice->b_deblocking_filter_override_flag = 0 ;
		slice->b_deblocking_filter_disable = 0 ;
		slice->i_deblocking_filter_beta_offset_div2 = 0 ;
		slice->i_deblocking_filter_tc_offset_div2 = 0 ;
	}

	slice->i_depth = i_depth ;

	h->fdec->fdec_data->i_t_layer = h->param.gop.gop_list[i_gop_id].i_temporal_id ;
	if ( I_SLICE == i_slice_type )
	{
		h->fdec->fdec_data->i_t_layer = 0 ;
	}

	slice->i_slice_mode = h->param.i_slice_mode ;
	slice->i_slice_argument = h->param.i_slice_argument ;
	slice->i_slice_segment_mode = h->param.i_slice_segment_mode ;
	slice->i_slice_segment_argument = h->param.i_slice_segment_argument ;
	slice->i_max_num_merge_cand = h->param.i_max_num_merge_cand ;

	x_store_wp_param ( ((x265_weight_pred_analysis_t*)(&h->enc_slice)),
						h->pps[0].b_use_weight_pred,
						h->pps[0].b_use_weighted_bi_pred ) ;

	h->fdec->fdec_data->b_reference = 1 ;

	h->enc_slice.f_pic_rd_cost = 0 ;
	h->enc_slice.i_pic_total_bits = 0 ;
	h->enc_slice.i_pic_dist = 0 ;
	h->enc_slice.i_start_cu_addr = 0 ;
	h->enc_slice.i_bounding_cu_addr = 0 ;

	*pp_slice = slice ;
}

void x265_enc_slice_set_search_range ( x265_t *h,
										x265_enc_slice_t *enc_slice,
										x265_slice_t* slice )
{
	int32_t i_dir = 0 ;
	int32_t i_ref_idx = 0 ;
	int32_t i_curr_poc = 0 ;
	int32_t i_ref_poc ;
	int32_t i_gop_size = 0 ;
	int32_t i_offset = 0 ;
	int32_t i_max_sr = 0 ;
	int32_t i_num_pred_dir = 0 ;
	enum ref_pic_list_e e = 0 ;
	int32_t i_new_sr = 0 ;

	i_curr_poc = h->fenc->i_poc ;
	i_gop_size = h->param.gop.i_gop_size ;
	i_offset = (i_gop_size >> 1);
	i_max_sr = h->param.i_search_range ;
	i_num_pred_dir = slice->i_slice_type == P_SLICE ? 1 : 2 ;

	for ( i_dir = 0 ; i_dir <= i_num_pred_dir ; ++ i_dir )
	{
		e = ( i_dir ? REF_PIC_LIST_1 : REF_PIC_LIST_0 );
		for ( i_ref_idx = 0 ; i_ref_idx < h->i_ref[e] ; ++ i_ref_idx )
		{
			i_ref_poc = h->fref[e][i_ref_idx]->i_poc ;
			i_new_sr = x265_clip3_int32 ( (i_max_sr*X265_ADAPT_SR_SCALE*abs(i_curr_poc - i_ref_poc)+i_offset)/i_gop_size,
										8, i_max_sr);
			h->enc_search.adapt_sr[i_dir][i_ref_idx] = i_new_sr ;
		}
	}
}

void x265_enc_slice_x_determine_start_and_bounding_cu_addr (
	x265_t *h, x265_enc_slice_t *p_enc_slice,
	uint32_t *p_start_cu_addr, uint32_t* p_bounding_cu_addr,
	int32_t b_encode_slice )
{
	x265_slice_t* slice = NULL ;
	uint32_t i_start_cu_addr_slice, i_bounding_cu_addr_slice;
	uint32_t i_start_cu_addr_slice_segment, i_bounding_cu_addr_slice_segment;
	uint32_t i_tile_idx_increment;
	uint32_t i_tile_idx;
	uint32_t i_tile_width_in_lcu;
	uint32_t i_tile_height_in_lcu;
	uint32_t i_tile_total_count;
    uint32_t i_cu_addr_increment;
    uint32_t i_number_of_cus_in_frame = 0 ;
    int32_t b_tile_boundary = 0 ;
    uint32_t i_lcu_enc_addr = 0 ;
    uint32_t i_lcu_addr = 0 ;
    uint32_t i_start_tile_idx = 0 ;
    uint32_t i_tile_bounding_cuaddr_slice = 0;
    //calculate real dependent slice start address
    uint32_t i_internal_address = 0 ;
    uint32_t i_external_address = 0 ;
    uint32_t i_pos_x = 0 ;
    uint32_t i_pos_y = 0 ;
    uint32_t i_width = 0 ;
    uint32_t i_height = 0 ;
	uint32_t i_real_start_address = 0 ;

	slice = h->slice ;
	i_start_cu_addr_slice = slice->i_slice_cur_start_cu_addr ;
	i_number_of_cus_in_frame = h->cu.pic.i_num_cus_in_frame ;
	i_bounding_cu_addr_slice = i_number_of_cus_in_frame;
	if (b_encode_slice)
	{
		switch ( h->param.i_slice_mode )
		{
		case FIXED_NUMBER_OF_LCU :
			i_cu_addr_increment = h->param.i_slice_argument ;
			i_bounding_cu_addr_slice = ((i_start_cu_addr_slice + i_cu_addr_increment) < i_number_of_cus_in_frame * h->cu.pic.i_num_partitions )
										? (i_start_cu_addr_slice + i_cu_addr_increment)
										: i_number_of_cus_in_frame * h->cu.pic.i_num_partitions ;
			break;
		case FIXED_NUMBER_OF_BYTES :
			i_cu_addr_increment = h->cu.pic.i_num_cus_in_frame;
			i_bounding_cu_addr_slice = slice->i_slice_cur_end_cu_addr ;
			break;
		case FIXED_NUMBER_OF_TILES:
			i_tile_idx = h->tile_idx_map[
			             h->cu_order_map[i_start_cu_addr_slice/h->cu.pic.i_num_partitions]] ;
			i_cu_addr_increment        = 0;
			i_tile_total_count = (h->param.pps.i_num_columns_minus_1 + 1) * (h->param.pps.i_num_rows_minus_1 + 1);

			for(i_tile_idx_increment = 0; i_tile_idx_increment < h->param.i_slice_argument; i_tile_idx_increment++)
			{
				if((i_tile_idx + i_tile_idx_increment) < i_tile_total_count)
				{
					i_tile_width_in_lcu = h->tile[i_tile_idx + i_tile_idx_increment]->i_tile_width ;
					i_tile_height_in_lcu = h->tile[i_tile_idx + i_tile_idx_increment]->i_tile_height ;
					i_cu_addr_increment += (i_tile_width_in_lcu * i_tile_height_in_lcu * h->cu.pic.i_num_partitions);
				}
			}

			i_bounding_cu_addr_slice    = ((i_start_cu_addr_slice + i_cu_addr_increment) < i_number_of_cus_in_frame*h->cu.pic.i_num_partitions) ? (i_start_cu_addr_slice + i_cu_addr_increment) : i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		default:
			i_cu_addr_increment        = h->cu.pic.i_num_cus_in_frame;
			i_bounding_cu_addr_slice    = i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		}
		// wpp: if a slice does not start at the beginning of a ctb row, it must end within the same ctb row
		if ( h->pps[0].i_num_substreams > 1 && (i_start_cu_addr_slice % (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions) != 0))
		{
			i_bounding_cu_addr_slice = X265_MIN(i_bounding_cu_addr_slice, i_start_cu_addr_slice - (i_start_cu_addr_slice % (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions)) + (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions));
		}
		slice->i_slice_cur_end_cu_addr = i_bounding_cu_addr_slice ;
	}
	else
	{
		switch (h->param.i_slice_mode)
		{
		case FIXED_NUMBER_OF_LCU:
			i_cu_addr_increment = h->param.i_slice_argument;
			i_bounding_cu_addr_slice = ((i_start_cu_addr_slice + i_cu_addr_increment) < i_number_of_cus_in_frame * h->cu.pic.i_num_partitions)
										? (i_start_cu_addr_slice + i_cu_addr_increment)
												: i_number_of_cus_in_frame * h->cu.pic.i_num_partitions;
			break;
		case FIXED_NUMBER_OF_TILES:
			i_tile_idx = h->tile_idx_map[
			             h->cu_order_map[i_start_cu_addr_slice/h->cu.pic.i_num_partitions]] ;
			i_cu_addr_increment        = 0;
			i_tile_total_count = (h->param.pps.i_num_columns_minus_1 + 1) * (h->param.pps.i_num_rows_minus_1 + 1);

			for(i_tile_idx_increment = 0; i_tile_idx_increment < h->param.i_slice_argument; i_tile_idx_increment++)
			{
				if((i_tile_idx + i_tile_idx_increment) < i_tile_total_count)
				{
					i_tile_width_in_lcu = h->tile[i_tile_idx + i_tile_idx_increment]->i_tile_width ;
					i_tile_height_in_lcu = h->tile[i_tile_idx + i_tile_idx_increment]->i_tile_height ;
					i_cu_addr_increment += (i_tile_width_in_lcu * i_tile_height_in_lcu * h->cu.pic.i_num_partitions);
				}
			}
			i_bounding_cu_addr_slice    = ((i_start_cu_addr_slice + i_cu_addr_increment) < i_number_of_cus_in_frame*h->cu.pic.i_num_partitions) ? (i_start_cu_addr_slice + i_cu_addr_increment) : i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;

		default:
			i_cu_addr_increment        = h->cu.pic.i_num_cus_in_frame;
			i_bounding_cu_addr_slice    = i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		}
		// wpp: if a slice does not start at the beginning of a ctb row, it must end within the same ctb row
		if (h->pps[0].i_num_substreams > 1 && (i_start_cu_addr_slice % (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions) != 0))
		{
			i_bounding_cu_addr_slice = X265_MIN(i_bounding_cu_addr_slice, i_start_cu_addr_slice - (i_start_cu_addr_slice % (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions)) + (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions));
		}
		slice->i_slice_cur_end_cu_addr = i_bounding_cu_addr_slice ;
	}

	b_tile_boundary = 0 ;
	if ((h->param.i_slice_mode == FIXED_NUMBER_OF_LCU
		|| h->param.i_slice_mode == FIXED_NUMBER_OF_BYTES) &&
		(h->param.pps.i_num_rows_minus_1 > 0
		|| h->param.pps.i_num_columns_minus_1 > 0))
	{
		i_lcu_enc_addr = (i_start_cu_addr_slice+h->cu.pic.i_num_partitions-1)/h->cu.pic.i_num_partitions;
		i_lcu_addr = h->cu_order_map[i_lcu_enc_addr] ;
		i_start_tile_idx = h->tile_idx_map[i_lcu_addr] ;
		i_tile_bounding_cuaddr_slice = 0;
		while (i_lcu_enc_addr < i_number_of_cus_in_frame && h->tile_idx_map[i_lcu_addr] == i_start_tile_idx)
		{
			i_lcu_enc_addr++;
			i_lcu_addr = h->cu_order_map[i_lcu_enc_addr] ;
		}
		i_tile_bounding_cuaddr_slice = i_lcu_enc_addr*h->cu.pic.i_num_partitions;

		if (i_tile_bounding_cuaddr_slice < i_bounding_cu_addr_slice)
		{
			i_bounding_cu_addr_slice = i_tile_bounding_cuaddr_slice;
			slice->i_slice_cur_end_cu_addr = i_bounding_cu_addr_slice ;
			b_tile_boundary = 1 ;
		}
	}

	// dependent slice
	i_start_cu_addr_slice_segment    = slice->i_slice_segment_cur_start_cu_addr ;
	i_bounding_cu_addr_slice_segment = i_number_of_cus_in_frame;
	if (b_encode_slice)
	{
		switch (h->param.i_slice_segment_mode)
		{
		case FIXED_NUMBER_OF_LCU:
			i_cu_addr_increment               = h->param.i_slice_segment_argument;
			i_bounding_cu_addr_slice_segment    = ((i_start_cu_addr_slice_segment + i_cu_addr_increment) < i_number_of_cus_in_frame*h->cu.pic.i_num_partitions ) ? (i_start_cu_addr_slice_segment + i_cu_addr_increment) : i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		case FIXED_NUMBER_OF_BYTES:
			i_cu_addr_increment               = h->cu.pic.i_num_cus_in_frame;
			i_bounding_cu_addr_slice_segment    = slice->i_slice_segment_cur_end_cu_addr;
			break;
		case FIXED_NUMBER_OF_TILES:
			i_tile_idx = h->tile_idx_map[
			             h->cu_order_map[i_start_cu_addr_slice_segment/h->cu.pic.i_num_partitions]] ;
			i_cu_addr_increment        = 0;
			i_tile_total_count = (h->param.pps.i_num_columns_minus_1 + 1) * (h->param.pps.i_num_rows_minus_1 + 1);

			for(i_tile_idx_increment = 0; i_tile_idx_increment < h->param.i_slice_segment_argument; i_tile_idx_increment++)
			{
				if((i_tile_idx + i_tile_idx_increment) < i_tile_total_count)
				{
					i_tile_width_in_lcu = h->tile[i_tile_idx + i_tile_idx_increment]->i_tile_width ;
					i_tile_height_in_lcu = h->tile[i_tile_idx + i_tile_idx_increment]->i_tile_height ;
					i_cu_addr_increment += (i_tile_width_in_lcu * i_tile_height_in_lcu * h->cu.pic.i_num_partitions);
				}
			}
			i_bounding_cu_addr_slice    = ((i_start_cu_addr_slice + i_cu_addr_increment) < i_number_of_cus_in_frame*h->cu.pic.i_num_partitions) ? (i_start_cu_addr_slice + i_cu_addr_increment) : i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		default:
			i_cu_addr_increment               = h->cu.pic.i_num_cus_in_frame;
			i_bounding_cu_addr_slice_segment    = i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		}
		// wpp: if a slice segment does not start at the beginning of a ctb row, it must end within the same ctb row
		if (h->pps[0].i_num_substreams > 1 && (i_start_cu_addr_slice_segment % (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions) != 0))
		{
			i_bounding_cu_addr_slice_segment = X265_MIN(i_bounding_cu_addr_slice_segment, i_start_cu_addr_slice_segment - (i_start_cu_addr_slice_segment % (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions)) + (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions));
		}
		slice->i_slice_segment_cur_end_cu_addr = i_bounding_cu_addr_slice_segment ;
	}
	else
	{
		switch (h->param.i_slice_segment_mode)
		{
		case FIXED_NUMBER_OF_LCU:
			i_cu_addr_increment               = h->param.i_slice_segment_argument;
			i_bounding_cu_addr_slice_segment    = ((i_start_cu_addr_slice_segment + i_cu_addr_increment) < i_number_of_cus_in_frame*h->cu.pic.i_num_partitions ) ? (i_start_cu_addr_slice_segment + i_cu_addr_increment) : i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		case FIXED_NUMBER_OF_TILES:
			i_tile_idx = h->tile_idx_map[
			             h->cu_order_map[i_start_cu_addr_slice_segment/h->cu.pic.i_num_partitions]] ;
			i_cu_addr_increment        = 0;
			i_tile_total_count = (h->param.pps.i_num_columns_minus_1 + 1) * (h->param.pps.i_num_rows_minus_1 + 1);

			for(i_tile_idx_increment = 0; i_tile_idx_increment < h->param.i_slice_segment_argument; i_tile_idx_increment++)
			{
				if((i_tile_idx + i_tile_idx_increment) < i_tile_total_count)
				{
					i_tile_width_in_lcu = h->tile[i_tile_idx + i_tile_idx_increment]->i_tile_width ;
					i_tile_height_in_lcu = h->tile[i_tile_idx + i_tile_idx_increment]->i_tile_height ;
					i_cu_addr_increment += (i_tile_width_in_lcu * i_tile_height_in_lcu * h->cu.pic.i_num_partitions);
				}
			}
			i_bounding_cu_addr_slice    = ((i_start_cu_addr_slice + i_cu_addr_increment) < i_number_of_cus_in_frame*h->cu.pic.i_num_partitions) ? (i_start_cu_addr_slice + i_cu_addr_increment) : i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		default:
			i_cu_addr_increment               = h->cu.pic.i_num_cus_in_frame;
			i_bounding_cu_addr_slice_segment    = i_number_of_cus_in_frame*h->cu.pic.i_num_partitions;
			break;
		}

		// wpp: if a slice segment does not start at the beginning of a ctb row, it must end within the same ctb row
		if (h->pps[0].i_num_substreams > 1 && (i_start_cu_addr_slice_segment % (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions) != 0))
		{
			i_bounding_cu_addr_slice_segment = X265_MIN(i_bounding_cu_addr_slice_segment, i_start_cu_addr_slice_segment - (i_start_cu_addr_slice_segment % (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions)) + (h->cu.pic.i_width_in_cu*h->cu.pic.i_num_partitions));
		}
		slice->i_slice_segment_cur_end_cu_addr = i_bounding_cu_addr_slice_segment ;
	}
	if ((h->param.i_slice_segment_mode == FIXED_NUMBER_OF_LCU
		|| h->param.i_slice_segment_mode == FIXED_NUMBER_OF_BYTES) &&
		(h->param.pps.i_num_rows_minus_1 > 0
		|| h->param.pps.i_num_columns_minus_1 > 0))
	{
		i_lcu_enc_addr = (i_start_cu_addr_slice_segment+h->cu.pic.i_num_partitions-1)/h->cu.pic.i_num_partitions;
		i_lcu_addr = h->cu_order_map[i_lcu_enc_addr];
		i_start_tile_idx = h->tile_idx_map[i_lcu_addr];
		i_tile_bounding_cuaddr_slice = 0;
		while (i_lcu_enc_addr < i_number_of_cus_in_frame
				&& h->tile_idx_map[i_lcu_addr] == i_start_tile_idx)
		{
			i_lcu_enc_addr++;
			i_lcu_addr = h->cu_order_map[i_lcu_enc_addr];
		}
		i_tile_bounding_cuaddr_slice = i_lcu_enc_addr*h->cu.pic.i_num_partitions;

		if (i_tile_bounding_cuaddr_slice < i_bounding_cu_addr_slice_segment)
		{
			i_bounding_cu_addr_slice_segment = i_tile_bounding_cuaddr_slice;
			slice->i_slice_segment_cur_end_cu_addr = i_bounding_cu_addr_slice_segment ;
			b_tile_boundary = 1 ;
		}
	}

	if(i_bounding_cu_addr_slice_segment>i_bounding_cu_addr_slice)
	{
		i_bounding_cu_addr_slice_segment = i_bounding_cu_addr_slice ;
		slice->i_slice_segment_cur_end_cu_addr = i_bounding_cu_addr_slice ;
	}

	//calculate real dependent slice start address
	i_internal_address = x265_pic_sym_get_pic_scu_addr( h, slice->i_slice_segment_cur_start_cu_addr ) % h->cu.pic.i_num_partitions;
	i_external_address = x265_pic_sym_get_pic_scu_addr( h, slice->i_slice_segment_cur_start_cu_addr ) / h->cu.pic.i_num_partitions;
	i_pos_x = ( i_external_address % h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_width
				+ h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_internal_address] ];
	i_pos_y = ( i_external_address / h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_height
				+ h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_internal_address] ];
	i_width = h->sps[0].i_pic_width_in_luma_samples ;
	i_height = h->sps[0].i_pic_height_in_luma_samples ;
	while((i_pos_x>=i_width||i_pos_y>=i_height)&&!(i_pos_x>=i_width&&i_pos_y>=i_height))
	{
		i_internal_address++;
		if(i_internal_address>=h->cu.pic.i_num_partitions)
		{
			i_internal_address=0;
			i_external_address = h->cu_order_map[h->inverse_cu_order_map[i_external_address]+1];
		}
		i_pos_x = ( i_external_address % h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_width
					+ h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_internal_address] ];
		i_pos_y = ( i_external_address / h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_height
					+ h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_internal_address] ];
	}
	i_real_start_address = x265_pic_sym_get_pic_scu_enc_order( h,
																i_external_address*h->cu.pic.i_num_partitions+i_internal_address) ;
	slice->i_slice_segment_cur_start_cu_addr = i_real_start_address ;
	i_start_cu_addr_slice_segment=i_real_start_address;

	//calculate real slice start address
	i_internal_address = x265_pic_sym_get_pic_scu_addr( h, slice->i_slice_cur_start_cu_addr) % h->cu.pic.i_num_partitions;
	i_external_address = x265_pic_sym_get_pic_scu_addr( h, slice->i_slice_cur_start_cu_addr) / h->cu.pic.i_num_partitions;
	i_pos_x = ( i_external_address % h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_width
				+ h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_internal_address] ];
	i_pos_y = ( i_external_address / h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_height
				+ h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_internal_address] ];
	i_width = h->sps[0].i_pic_width_in_luma_samples ;
	i_height = h->sps[0].i_pic_height_in_luma_samples ;
	while((i_pos_x>=i_width||i_pos_y>=i_height)&&!(i_pos_x>=i_width&&i_pos_y>=i_height))
	{
		i_internal_address++;
		if(i_internal_address>=h->cu.pic.i_num_partitions)
		{
			i_internal_address=0;
			i_external_address = h->cu_order_map[h->inverse_cu_order_map[i_external_address]+1];
		}
		i_pos_x = ( i_external_address % h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_width
					+ h->scan.raster_to_pel_x[ h->scan.zscan_to_raster[i_internal_address] ];
		i_pos_y = ( i_external_address / h->cu.pic.i_width_in_cu ) * h->cu.pic.i_max_cu_height
					+ h->scan.raster_to_pel_y[ h->scan.zscan_to_raster[i_internal_address] ];
	}
	i_real_start_address = x265_pic_sym_get_pic_scu_enc_order ( h,
																i_external_address*h->cu.pic.i_num_partitions+i_internal_address ) ;
	slice->i_slice_cur_start_cu_addr = i_real_start_address ;
	i_start_cu_addr_slice=i_real_start_address;

	// make a joint decision based on reconstruction and dependent slice bounds
	*p_start_cu_addr = X265_MAX(i_start_cu_addr_slice   , i_start_cu_addr_slice_segment   );
	*p_bounding_cu_addr = X265_MIN(i_bounding_cu_addr_slice, i_bounding_cu_addr_slice_segment);


	if (!b_encode_slice)
	{
		// for fixed number of lcu within an entropy and reconstruction slice we already know whether we will encounter end of entropy and/or reconstruction slice
		// first. set the flags accordingly.
		if ( (h->param.i_slice_mode==FIXED_NUMBER_OF_LCU && h->param.i_slice_segment_mode==FIXED_NUMBER_OF_LCU)
				|| (h->param.i_slice_mode==0 && h->param.i_slice_segment_mode==FIXED_NUMBER_OF_LCU)
				|| (h->param.i_slice_mode==FIXED_NUMBER_OF_LCU && h->param.i_slice_segment_mode==0)
				|| (h->param.i_slice_mode==FIXED_NUMBER_OF_TILES && h->param.i_slice_segment_mode==FIXED_NUMBER_OF_LCU)
				|| (h->param.i_slice_mode==FIXED_NUMBER_OF_TILES && h->param.i_slice_segment_mode==0)
				|| (h->param.i_slice_segment_mode==FIXED_NUMBER_OF_TILES && h->param.i_slice_mode==0)
				|| b_tile_boundary )
		{
			if (i_bounding_cu_addr_slice < i_bounding_cu_addr_slice_segment)
			{
				slice->b_next_slice = 1 ;
				slice->b_next_slice_segment = 0 ;
			}
			else if (i_bounding_cu_addr_slice > i_bounding_cu_addr_slice_segment)
			{
				slice->b_next_slice = 0 ;
				slice->b_next_slice_segment = 1 ;
			}
			else
			{
				slice->b_next_slice = 1 ;
				slice->b_next_slice_segment = 1 ;
			}
		}
		else
		{
			slice->b_next_slice = 0 ;
			slice->b_next_slice_segment = 0 ;
		}
	}
}


void x265_enc_slice_pre_compress_slice ( x265_t *h )
{
	if ( h->param.rdo.i_delta_qp_rd == 0 )
	{
		return ;
	}

	return ;
}

int x265_enc_slice_compress_and_encode_cu ( x265_t *h,
											x265_output_bitstream_t *pc_bitstream,
											x265_output_bitstream_t **ppc_substreams )
{
	uint32_t i_cu_addr = 0 ;
	uint32_t i_enc_cu_order = 0 ;
	x265_slice_t* slice = NULL ;
	x265_enc_slice_t* enc_slice = NULL ;

	slice = h->slice ;
	enc_slice = &h->enc_slice ;
	i_cu_addr = h->cu_order_map[enc_slice->i_start_cu_addr / h->cu.pic.i_num_partitions] ;
	x265_enc_slice_start_compress_slice ( h, enc_slice ) ;
	x265_enc_slice_start_encode_slice ( h, enc_slice, ppc_substreams ) ;
	for( i_enc_cu_order = enc_slice->i_start_cu_addr / h->cu.pic.i_num_partitions ;
			i_enc_cu_order < (enc_slice->i_bounding_cu_addr + (h->cu.pic.i_num_partitions - 1)) / h->cu.pic.i_num_partitions ;
			i_cu_addr = h->cu_order_map[++i_enc_cu_order] )
	{
		x265_data_cu_init_cu ( h, h->data_cu, i_cu_addr ) ;

		x265_pic_sym_load_prediction_data ( h, i_cu_addr ) ;
	    if ( x265_enc_slice_compress_cu ( h, enc_slice, i_cu_addr ) )
	    {
	    	goto fail ;
	    }
		x265_pic_sym_store_prediction_data ( h, i_cu_addr ) ;
		x265_tmvp_data_cu_load ( h,
								h->fdec->fdec_data->tmvp_data_cu[i_cu_addr],
								h->data_cu ) ;

		/**/
	    if ( h->param.sao.i_sao_lcu_based_optimization && h->param.sao.i_sao_lcu_boundary )
	    {
	    	x265_enc_sample_adaptive_offset_calc_sao_stats_cu_before_dblk ( h,
	    																	&h->enc_sao,
	    																	i_cu_addr ) ;
	    }

	    x265_loop_filter_loop_filter_cu ( h, &h->loop_filter, h->data_cu ) ;

	    if ( h->sps[0].b_use_sao )
	    {
	    	x265_enc_sample_adaptive_offset_sao_process_cu ( h,
	    													&h->enc_sao,
	    													h->sao_param,
	    													i_cu_addr,
	    													slice->f_lambda_luma,
	    													slice->f_lambda_chroma,
	    													slice->i_depth ) ;
	    }
	    h->data_cu->b_being_encode = 1 ;
	    x265_enc_slice_encode_cu ( h, enc_slice, i_cu_addr, pc_bitstream, ppc_substreams ) ;
	    h->data_cu->b_being_encode = 0 ;
	}

	/*
	print_double_state (enc_slice->f_pic_rd_cost) ;
	print_int_state (enc_slice->i_pic_total_bits) ;
	print_int_state (enc_slice->i_pic_dist) ;
	*/

	x265_enc_slice_end_encode_slice ( h, &h->enc_slice ) ;
	x265_enc_slice_end_compress_slice ( h, &h->enc_slice ) ;
	return 0 ;
fail:
	x265_enc_slice_end_encode_slice ( h, &h->enc_slice ) ;
	x265_enc_slice_end_compress_slice ( h, &h->enc_slice ) ;
	return -1 ;

}

double x265_enc_slice_get_qp_value_according_to_lambda ( double f_lambda )
{
	return 4.2005 * log(f_lambda) + 13.7122;
}

void x265_enc_slice_start_compress_slice ( x265_t *h, x265_enc_slice_t *enc_slice)
{
	x265_enc_bin_cabac_t* pppc_rd_sbac_coder = NULL;
	x265_slice_t* slice = NULL ;
	int32_t i_num_substreams = 0;
	int32_t loop = 0 ;

	slice = h->slice ;

	slice->i_slice_segment_bits = 0 ;
	// initialize cost values
	enc_slice->i_pic_total_bits = 0 ;
	enc_slice->f_pic_rd_cost = 0 ;
	enc_slice->i_pic_dist = 0 ;
	i_num_substreams = 1;

	x265_enc_slice_x_determine_start_and_bounding_cu_addr ( h, enc_slice,
															&enc_slice->i_start_cu_addr,
															&enc_slice->i_bounding_cu_addr,
															0 ) ;
	// set entropy coder
	if( h->param.b_use_sbac_rd )
	{
		enc_sbac_init(&h->enc_sbac, (x265_enc_bin_if_t*)&h->enc_bin_cabac ) ;
		x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
											(x265_enc_entropy_if_t*)&h->enc_sbac,
											slice );
		x265_enc_entropy_reset_entropy ( h,
										&h->enc_entropy,
										slice->i_slice_type,
										slice->i_slice_qp );
		x265_enc_sbac_load ( h->pppc_rd_sbac_coder[0][CI_CURR_BEST], &h->enc_sbac ) ;
		pppc_rd_sbac_coder = (x265_enc_bin_cabac_t*) x265_enc_sbac_get_enc_bin_if ( h->pppc_rd_sbac_coder[0][CI_CURR_BEST] ) ;
		x265_enc_bin_cabac_set_bin_counting_enable_flag ( pppc_rd_sbac_coder, 0 );
		x265_enc_bin_cabac_set_bins_coded ( pppc_rd_sbac_coder, 0 );
		i_num_substreams = h->pps[0].i_num_substreams ;
		for ( loop = 0 ; loop < i_num_substreams ; ++ loop ) //init all sbac coders for rd optimization
		{
			x265_enc_sbac_load ( h->ppppc_rd_sbac_coders[loop][0][CI_CURR_BEST],
								h->pppc_rd_sbac_coder[0][CI_CURR_BEST] ) ;
		}
	}
	else
	{
		x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
											(x265_enc_entropy_if_t*)&h->enc_cavlc,
											slice );
		x265_enc_entropy_reset_entropy ( h,
										&h->enc_entropy,
										slice->i_slice_type,
										slice->i_slice_qp );
		x265_enc_entropy_set_bitstream ( &h->enc_entropy, (x265_bit_if_t*)&h->bit_counter );
	}
}


void x265_enc_slice_start_encode_slice (x265_t *h, x265_enc_slice_t *enc_slice, x265_output_bitstream_t **ppc_substreams )
{
	int32_t i_num_substreams = 0 ;
	int32_t i_substrm_idx = 0 ;
	x265_slice_t* slice = NULL ;

	slice = h->slice ;
	i_num_substreams = h->pps[0].i_num_substreams ;

	// choose entropy coder
	enc_sbac_init ( &h->enc_sbac, (x265_enc_bin_if_t*)&h->enc_bin_cabac ) ;
	x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
										(x265_enc_entropy_if_t*)&h->enc_sbac,
										slice );
	h->enc_cu.bit_counter = NULL ;
	enc_slice->i_bits_originally_in_substreams = 0 ;

	for ( i_substrm_idx = 0 ; i_substrm_idx < i_num_substreams; ++ i_substrm_idx )
	{
		enc_slice->i_bits_originally_in_substreams
			+= ppc_substreams[i_substrm_idx]->get_number_of_written_bits((x265_bit_if_t*)ppc_substreams[i_substrm_idx]) ;
	}
}

void x265_enc_slice_end_compress_slice ( x265_t *h, x265_enc_slice_t *enc_slice )
{
	x_restore_wp_param ( h, (x265_weight_pred_analysis_t*)enc_slice ) ;
}

void x265_enc_slice_end_encode_slice ( x265_t *h, x265_enc_slice_t *enc_slice )
{
	x265_slice_t *slice = NULL;

	slice = h->slice ;
	if ( h->pps[0].b_cabac_init_present_flag )
	{
		if  ( h->pps[0].b_dependent_slice_segments_enabled_flag )
		{
			h->pps[0].i_enc_cabac_table_idx = slice->i_slice_type ;		}
		else
		{
			x265_enc_entropy_determine_cabac_init_idx ( h,
														&h->enc_entropy,
														slice->i_slice_type,
														slice->i_slice_qp );
		}
	}
}

int x265_enc_slice_compress_cu ( x265_t *h, x265_enc_slice_t *enc_slice, uint32_t i_cu_addr )
{
    x265_slice_t *slice = NULL ;
    uint32_t i_width_in_lcus  = 0 ;
    uint32_t i_lin = 0, i_sub_strm = 0 ;
    int32_t  i_num_substreams = 0 ;
    int32_t i_num_substreams_per_tile = 0 ;
    x265_enc_sbac_t ****ppppc_rd_sbac_coders = NULL ;
    x265_bit_counter_t **pc_bit_counters = NULL ;
    x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;
    x265_data_cu_t *cu = NULL ;

    slice = h->slice ;
    i_width_in_lcus = h->cu.pic.i_width_in_cu ;
    i_num_substreams = 1 ;
    ppppc_rd_sbac_coders = h->ppppc_rd_sbac_coders ;
    pc_bit_counters = h->pc_bit_counters ;
    cu = h->data_cu ;

    if ( h->param.b_use_sbac_rd )
    {
    	enc_bin_cabac = (x265_enc_bin_cabac_t*)x265_enc_sbac_get_enc_bin_if(h->pppc_rd_sbac_coder[0][CI_CURR_BEST]) ;
    	i_num_substreams = h->pps[0].i_num_substreams ;
    	i_lin = i_cu_addr / i_width_in_lcus;
    	if ( h->pps[0].i_num_substreams > 1)
    	{
    		// independent tiles => substreams are "per tile".  i_num_substreams has already been multiplied.
    		i_num_substreams_per_tile = i_num_substreams /
    				(h->param.pps.i_num_columns_minus_1 + 1) * (h->param.pps.i_num_rows_minus_1 + 1) ;
    		i_sub_strm = h->tile_idx_map[i_cu_addr]*i_num_substreams_per_tile + i_lin % i_num_substreams_per_tile;
    	}
    	else
    	{
    		// dependent tiles => substreams are "per frame".
    		i_sub_strm = i_lin % i_num_substreams;
    	}
		x265_enc_sbac_load ( h->pppc_rd_sbac_coder[0][CI_CURR_BEST],
							ppppc_rd_sbac_coders[i_sub_strm][0][CI_CURR_BEST] ); //this load is used to simplify the code

		// set go-on entropy coder
		x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
											(x265_enc_entropy_if_t*)&h->rd_go_on_sbac_coder,
											slice );
		x265_enc_entropy_set_bitstream ( &h->enc_entropy,
										(x265_bit_if_t*)pc_bit_counters[i_sub_strm] );
		x265_enc_bin_cabac_set_bin_counting_enable_flag (
				(x265_enc_bin_cabac_t*)x265_enc_sbac_get_enc_bin_if(&h->rd_go_on_sbac_coder),
				1 ) ;
		x265_enc_cu_compress_cu ( h, &h->enc_cu, i_cu_addr ) ;


		// restore entropy coder to an initial stage
		x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
											(x265_enc_entropy_if_t*)h->pppc_rd_sbac_coder[0][CI_CURR_BEST],
											slice );
		x265_enc_entropy_set_bitstream ( &h->enc_entropy,
										(x265_bit_if_t*)pc_bit_counters[i_sub_strm] );
		h->enc_cu.bit_counter = pc_bit_counters[i_sub_strm] ;
		x265_enc_bin_cabac_set_bin_counting_enable_flag ( enc_bin_cabac, 1 ) ;
		pc_bit_counters[i_sub_strm]->reset_bits ((x265_bit_if_t*)pc_bit_counters[i_sub_strm]) ;
		x265_enc_bin_cabac_set_bins_coded ( enc_bin_cabac, 0 ) ;
		x265_enc_cu_encode_cu ( h, &h->enc_cu, i_cu_addr ) ;
		x265_enc_bin_cabac_set_bin_counting_enable_flag ( enc_bin_cabac, 0 ) ;

		if ( h->param.i_slice_mode == FIXED_NUMBER_OF_BYTES
				&& ( (slice->i_slice_bits + x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy))) > (h->param.i_slice_argument << 3)
				)
		{
			slice->b_next_slice = 1 ;
			goto fail ;
		}
		if ( h->param.i_slice_segment_argument == FIXED_NUMBER_OF_BYTES
				&& ( (slice->i_slice_segment_bits + x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) > (h->param.i_slice_segment_argument << 3))
					&& (slice->i_slice_cur_end_cu_addr!=slice->i_slice_segment_cur_end_cu_addr)
					) )
		{
			slice->b_next_slice_segment = 1 ;
			goto fail ;
		}
		x265_enc_sbac_load ( ppppc_rd_sbac_coders[i_sub_strm][0][CI_CURR_BEST],
							h->pppc_rd_sbac_coder[0][CI_CURR_BEST] );
    }
    // other case: encode_c_u is not called
    else
    {
		x265_enc_cu_compress_cu ( h, &h->enc_cu, i_cu_addr ) ;
		x265_enc_cu_encode_cu ( h, &h->enc_cu, i_cu_addr ) ;
    	if ( h->param.i_slice_mode == FIXED_NUMBER_OF_BYTES
    			&& ( (x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy))) > (h->param.i_slice_argument << 3)
    			)
    	{
    		slice->b_next_slice = 1 ;
    		goto fail ;
    	}
		if ( h->param.i_slice_segment_argument == FIXED_NUMBER_OF_BYTES
				&& ( (slice->i_slice_segment_bits + x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy) > (h->param.i_slice_segment_argument << 3))
					&& (slice->i_slice_cur_end_cu_addr!=slice->i_slice_segment_cur_end_cu_addr)
					) )
    	{
    		slice->b_next_slice_segment = 1 ;
    		goto fail ;
    	}
    }

    enc_slice->i_pic_total_bits += cu->i_total_bits ;
    enc_slice->f_pic_rd_cost += cu->f_total_cost ;
    enc_slice->i_pic_dist += cu->i_total_distortion ;

    return 0 ;

fail:
	return -1 ;
}


void x265_enc_slice_encode_cu ( x265_t *h,
								x265_enc_slice_t *enc_slice,
								uint32_t i_cu_addr,
								x265_output_bitstream_t *bitstream,
								x265_output_bitstream_t **pc_substreams )
{
	uint32_t i_sub_strm = 0 ;
	uint32_t i_lin = 0 ;
	uint32_t i_width_in_lcus = 0 ;
	x265_slice_t* slice = NULL ;
	x265_enc_sbac_t **pc_sbac_coders = NULL ;
	int32_t i_num_substreams = 0 ;
	uint32_t i_width_in_cu_mul_two = 0 ;
	int32_t i_num_substreams_per_tile = 0 ;
	x265_sao_param_t *sao_param = NULL ;
	int32_t i_num_cu_in_width = 0 ;
	int32_t i_cu_addr_in_slice = 0 ;
	int32_t i_cu_addr_up_in_slice  = 0 ;
	int32_t i_rx = 0 ;
	int32_t i_ry = 0 ;
	int32_t b_allow_merge_left = 0 ;
	int32_t b_allow_merge_up   = 0 ;
	int32_t b_merge_left = 0 ;
	int32_t b_merge_up = 0 ;
	int32_t i_comp_idx = 0 ;
	int32_t i_c_idx = 0 ;
	x265_sao_lcu_param_t *sao_lcu_param = NULL ;

	i_width_in_lcus = h->cu.pic.i_width_in_cu ;
	slice = h->slice ;
	pc_sbac_coders = h->pc_sbac_coders ;
	i_num_substreams = h->pps[0].i_num_substreams ;

	i_width_in_cu_mul_two = h->cu.pic.i_width_in_cu_mul_two ;
	x265_enc_entropy_set_entropy_coder ( &h->enc_entropy,
										(x265_enc_entropy_if_t*)&h->enc_sbac,
										slice );
	if( h->param.b_use_sbac_rd )
	{
		i_lin = i_cu_addr / i_width_in_lcus;
    	if ( h->pps[0].i_num_substreams > 1)
    	{
    		// independent tiles => substreams are "per tile".  i_num_substreams has already been multiplied.
    		i_num_substreams_per_tile = i_num_substreams /
    				(h->param.pps.i_num_columns_minus_1 + 1) * (h->param.pps.i_num_rows_minus_1 + 1) ;
    		i_sub_strm = h->tile_idx_map[i_cu_addr]*i_num_substreams_per_tile + i_lin % i_num_substreams_per_tile;
    	}
    	else
    	{
    		// dependent tiles => substreams are "per frame".
    		i_sub_strm = i_lin % i_num_substreams;
    	}

		x265_enc_entropy_set_bitstream ( &h->enc_entropy,
										(x265_bit_if_t*)pc_substreams[i_sub_strm] ) ;
		x265_enc_sbac_load ( &h->enc_sbac, pc_sbac_coders[i_sub_strm] ) ;
	}

	if ( h->sps[0].b_use_sao && (slice->b_sao_enabled_flag || slice->b_sao_enabled_flag_chroma ) )
	{
		sao_param = h->sao_param ;
		i_num_cu_in_width = sao_param->i_num_cu_in_width ;
		i_cu_addr_in_slice = i_cu_addr - h->cu_order_map[
		                     slice->i_slice_cur_start_cu_addr / h->cu.pic.i_num_partitions];
		i_cu_addr_up_in_slice  = i_cu_addr_in_slice - i_num_cu_in_width;
		i_rx = i_cu_addr % i_num_cu_in_width;
		i_ry = i_cu_addr / i_num_cu_in_width;
		b_allow_merge_left = 1;
		b_allow_merge_up   = 1;
		if (i_rx!=0)
		{
			if ( h->tile_idx_map[i_cu_addr-1] != h->tile_idx_map[i_cu_addr] )
			{
				b_allow_merge_left = 0;
			}
		}
		if (i_ry!=0)
		{
			if ( h->tile_idx_map[i_cu_addr-i_num_cu_in_width] != h->tile_idx_map[i_cu_addr] )
			{
				b_allow_merge_up = 0;
			}
		}
		b_allow_merge_left = b_allow_merge_left && (i_rx>0) && (i_cu_addr_in_slice!=0);
		b_allow_merge_up = b_allow_merge_up && (i_ry>0) && (i_cu_addr_up_in_slice>=0);
		if( sao_param->b_sao_flag[0] || sao_param->b_sao_flag[1] )
		{
			b_merge_left = sao_param->sao_lcu_param[0][i_cu_addr%i_width_in_cu_mul_two].b_merge_left_flag;
			b_merge_up = sao_param->sao_lcu_param[0][i_cu_addr%i_width_in_cu_mul_two].b_merge_up_flag;
			if (b_allow_merge_left)
			{
				h->enc_entropy.enc_entropy_if->code_sao_merge ( h->enc_entropy.enc_entropy_if,
																b_merge_left ) ;
			}
			else
			{
				b_merge_left = 0;
			}
			if(b_merge_left == 0)
			{
				if (b_allow_merge_up)
				{
					h->enc_entropy.enc_entropy_if->code_sao_merge ( h->enc_entropy.enc_entropy_if,
																	b_merge_up ) ;
				}
				else
				{
					b_merge_up = 0;
				}
				if(b_merge_up == 0)
				{
					for ( i_comp_idx=0 ; i_comp_idx < 3 ; i_comp_idx++ )
					{
						if( (i_comp_idx == 0 && sao_param->b_sao_flag[0])
								|| (i_comp_idx > 0 && sao_param->b_sao_flag[1]))
						{
							x265_enc_entropy_encode_sao_offset ( h,
																&h->enc_entropy,
																&sao_param->sao_lcu_param[i_comp_idx][i_cu_addr % i_width_in_cu_mul_two],
																i_comp_idx ) ;
						}
					}
				}
			}
		}
	}
	else if ( h->sps[0].b_use_sao )
	{
		sao_param = h->sao_param ;
		for ( i_c_idx=0; i_c_idx<3; i_c_idx++)
		{
			sao_lcu_param = &(sao_param->sao_lcu_param[i_c_idx][i_cu_addr%i_width_in_cu_mul_two]);
			if ( ((i_c_idx == 0) && !slice->b_sao_enabled_flag)
				|| ((i_c_idx == 1 || i_c_idx == 2) && !slice->b_sao_enabled_flag_chroma ))
			{
				sao_lcu_param->b_merge_up_flag = 0;
				sao_lcu_param->b_merge_left_flag = 0;
				sao_lcu_param->i_sub_type_idx = 0;
				sao_lcu_param->i_type_idx = -1;
				sao_lcu_param->offset[0] = 0;
				sao_lcu_param->offset[1] = 0;
				sao_lcu_param->offset[2] = 0;
				sao_lcu_param->offset[3] = 0;
			}
		}
	}

	x265_enc_cu_encode_cu ( h, &h->enc_cu, i_cu_addr ) ;

	if( h->param.b_use_sbac_rd )
	{
		x265_enc_sbac_load ( pc_sbac_coders[i_sub_strm], &h->enc_sbac ) ;
	}
}



