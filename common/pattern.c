

#include "common.h"



x265_pattern_class_static_t *p_pattern_class_static ;





x265_pattern_param_t *x265_pattern_param_new ()
{
	x265_pattern_param_t *pattern_param = NULL ;

	CHECKED_MALLOCZERO( pattern_param, sizeof(x265_pattern_param_t) );

	return pattern_param ;
fail:
	x265_pattern_param_delete ( (void*) pattern_param ) ;
	return NULL ;
}

void x265_pattern_param_delete ( x265_pattern_param_t *pattern_param )
{
	x265_free ( pattern_param ) ;
}

pixel *x265_pattern_param_get_gattern_origin ( x265_pattern_param_t *pattern_param )
{
	return  pattern_param->pattern_origin;
}



void x265_pattern_delete ( x265_pattern_t *pattern )
{
	x265_free ( pattern ) ;
}

x265_pattern_t *x265_pattern_new ()
{
	x265_pattern_t *pattern = NULL ;

	CHECKED_MALLOCZERO( pattern, sizeof(x265_pattern_t) );

	return pattern ;
fail:
	x265_pattern_delete ( (void*) pattern ) ;
	return NULL ;
}

int x265_pattern_param_init ( x265_pattern_param_t *pattern_param )
{

    return 0 ;
}

void x265_pattern_param_deinit ( x265_pattern_param_t *pattern_param )
{

}

pixel *x265_pattern_param_get_roi_origin ( x265_pattern_param_t *pattern_param )
{
	return  pattern_param->pattern_origin
			+ pattern_param->i_pattern_stride * pattern_param->i_offset_above
			+ pattern_param->i_offset_left ;
}




int x265_pattern_class_static_init ()
{
	CHECKED_MALLOCZERO( p_pattern_class_static, sizeof(x265_pattern_class_static_t) );

	p_pattern_class_static->intra_filter[0] = 10 ;
	p_pattern_class_static->intra_filter[1] = 7 ;
	p_pattern_class_static->intra_filter[2] = 1 ;
	p_pattern_class_static->intra_filter[3] = 0 ;
	p_pattern_class_static->intra_filter[4] = 10 ;

	return 0 ;

fail:
	x265_pattern_class_static_deinit () ;
	return -1 ;
}

void x265_pattern_class_static_deinit ()
{
	x265_free ( p_pattern_class_static ) ;
}


// ====================================================================================================================
// Public member functions (TComPatternParam)
// ====================================================================================================================

/** \param  piTexture     pixel data
 \param  iRoiWidth     pattern width
 \param  iRoiHeight    pattern height
 \param  iStride       buffer stride
 \param  iOffsetLeft   neighbour offset (left)
 \param  iOffsetAbove  neighbour offset (above)
 */
void x265_pattern_param_set_pattern_param_pel ( x265_pattern_param_t *pattern_param,
												pixel *p_texture,
												int32_t i_roi_width,
												int32_t i_roi_height,
												int32_t i_stride,
												int32_t i_offset_left,
												int32_t i_offset_above )
{
	pattern_param->pattern_origin = p_texture;
	pattern_param->i_roi_width = i_roi_width;
	pattern_param->i_roi_height = i_roi_height;
	pattern_param->i_pattern_stride = i_stride;
	pattern_param->i_offset_left = i_offset_left;
	pattern_param->i_offset_above = i_offset_above;

}


/**
 \param  pcCU          CU data structure
 \param  iComp         component index (0=Y, 1=Cb, 2=Cr)
 \param  iRoiWidth     pattern width
 \param  iRoiHeight    pattern height
 \param  iStride       buffer stride
 \param  iOffsetLeft   neighbour offset (left)
 \param  iOffsetAbove  neighbour offset (above)
 \param  uiAbsPartIdx  part index
 */
void x265_pattern_param_set_pattern_param_cu ( x265_t *h,
												x265_pattern_param_t *pattern_param,
												x265_data_cu_t *cu,
												uint8_t i_comp,
												uint8_t i_roi_width,
												uint8_t i_roi_height,
												int32_t i_offset_left,
												int32_t i_offset_above,
												uint32_t i_abs_part_idx )
{
	uint32_t i_abs_zorder_idx = 0;


	pattern_param->i_offset_left = i_offset_left;
	pattern_param->i_offset_above = i_offset_above;
	pattern_param->i_roi_width = i_roi_width;
	pattern_param->i_roi_height = i_roi_height;



	i_abs_zorder_idx = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_abs_part_idx;

	if ( i_comp == 0 )
	{
		pattern_param->i_pattern_stride = h->fdec->i_stride[0] ;
		pattern_param->pattern_origin = x265_frame_get_luma_addr_p4(h,
																	h->fdec,
																	x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
																	i_abs_zorder_idx)
										- pattern_param->i_offset_above * pattern_param->i_pattern_stride
										- pattern_param->i_offset_left;
	}
	else
	{
		pattern_param->i_pattern_stride = h->fdec->i_stride[1] ;
		if ( i_comp == 1 )
		{
			pattern_param->pattern_origin = x265_frame_get_cb_addr_p4(h,
																		h->fdec,
																		x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
																		i_abs_zorder_idx)
											- pattern_param->i_offset_above * pattern_param->i_pattern_stride
											- pattern_param->i_offset_left;
		}
		else
		{
			pattern_param->pattern_origin = x265_frame_get_cr_addr_p4(h,
																		h->fdec,
																		x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
																		i_abs_zorder_idx)
											- pattern_param->i_offset_above * pattern_param->i_pattern_stride
											- pattern_param->i_offset_left;
		}
	}
}

int x265_pattern_init ( x265_pattern_t *pattern )
{

    return 0 ;
}

void x265_pattern_deinit ( x265_pattern_t *pattern )
{

}


pixel *x265_pattern_get_roi_y ( x265_pattern_t *pattern )
{
	return x265_pattern_param_get_roi_origin ( &pattern->pattern_y ) ;
}

int32_t x265_pattern_get_roi_width ( x265_pattern_t *pattern )
{
	return pattern->pattern_y.i_roi_width ;
}

int32_t x265_pattern_get_roi_height ( x265_pattern_t *pattern )
{
	return pattern->pattern_y.i_roi_height ;
}

int32_t x265_pattern_get_pattern_l_stride ( x265_pattern_t *pattern )
{
	return pattern->pattern_y.i_pattern_stride ;
}




void x265_pattern_init_pattern_p9 (x265_pattern_t *pattern,
								pixel *p_y,
								pixel *p_cb,
								pixel *p_cr,
								int32_t i_roi_width,
								int32_t i_roi_height,
								int32_t i_stride,
								int32_t i_offset_left,
								int32_t i_offset_above )
{
	x265_pattern_param_set_pattern_param_pel(&pattern->pattern_y,
											p_y,
											i_roi_width,
											i_roi_height,
											i_stride,
											i_offset_left,
											i_offset_above ) ;
	x265_pattern_param_set_pattern_param_pel(&pattern->pattern_cb,
											p_cb,
											i_roi_width >> 1,
											i_roi_height >> 1,
											i_stride >> 1,
											i_offset_left >> 1,
											i_offset_above >> 1 );
	x265_pattern_param_set_pattern_param_pel(&pattern->pattern_cr,
											p_cr,
											i_roi_width >> 1,
											i_roi_height >> 1,
											i_stride >> 1,
											i_offset_left >> 1,
											i_offset_above >> 1 );

	return;
}


void x265_pattern_init_pattern_p5(x265_t *h,
								x265_pattern_t *pattern,
								x265_data_cu_t *cu,
								uint32_t i_part_depth,
								uint32_t i_abs_part_idx )
{
	int32_t i_offset_left  = 0;
	int32_t i_offset_above = 0;

	uint8_t i_width = 0;
	uint8_t i_height = 0;

	uint32_t i_abs_zorder_idx = 0;
	uint32_t i_curr_pic_pel_x = 0;
	uint32_t i_curr_pic_pel_y = 0;

	i_width = x265_data_cu_get_width_p2(cu, 0) >> i_part_depth;
	i_height = x265_data_cu_get_height_p2(cu, 0) >> i_part_depth;

	i_abs_zorder_idx = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) ;
	i_curr_pic_pel_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_abs_zorder_idx]];
	i_curr_pic_pel_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_abs_zorder_idx]];

	if( i_curr_pic_pel_x != 0 )
	{
		i_offset_left = 1;
	}

	if( i_curr_pic_pel_y != 0 )
	{
		i_offset_above = 1;
	}

	x265_pattern_param_set_pattern_param_cu(h,
											&pattern->pattern_y,
											cu,
											0,
											i_width,
											i_height,
											i_offset_left,
											i_offset_above,
											i_abs_part_idx );
	x265_pattern_param_set_pattern_param_cu(h,
											&pattern->pattern_cb,
											cu,
											1,
											i_width >> 1,
											i_height >> 1,
											i_offset_left,
											i_offset_above,
											i_abs_part_idx );
	x265_pattern_param_set_pattern_param_cu(h,
											&pattern->pattern_cr,
											cu,
											2,
											i_width >> 1,
											i_height >> 1,
											i_offset_left,
											i_offset_above,
											i_abs_part_idx );
}

void x265_pattern_init_adi_pattern(x265_t *h,
									x265_pattern_t *pattern,
									x265_data_cu_t *cu,
									x265_image_t *p_image,
									uint32_t i_zorder_idx_in_part,
									uint32_t i_part_depth,
									pixel *adi_buf,
									int32_t i_org_buf_stride,
									int32_t i_org_buf_height,
									int32_t *p_above,
									int32_t *p_left,
									int32_t b_l_mmode )
{
	int32_t loop = 0 ;
	pixel *p_roi_origin = NULL;
	pixel *left1 = NULL;
	pixel *top1 = NULL;
	pixel *left2 = NULL;
	pixel *top2 = NULL;
	uint32_t i_cu_width = 0;
	uint32_t i_cu_height = 0;
	uint32_t i_cu_width2 = 0;
	uint32_t i_cu_height2 = 0;
	uint32_t i_width;
	uint32_t i_height;
	int32_t i_pic_stride = 0;
	int32_t i_unit_size = 0;
	int32_t i_num_units_in_cu = 0;
	int32_t i_total_units = 0;
	int32_t b_neighbor_flags[4 * X265_MAX_NUM_SPU_W + 1];
	int32_t i_num_intra_neighbor = 0;
	uint32_t i_part_idx_lt, i_part_idx_rt, i_part_idx_lb;
	// generate filtered intra prediction samples
	int32_t i_blk_size = 0;
	pixel i_bottom_left = 0;
	pixel i_top_left = 0;
	pixel i_top_right = 0;
	int32_t i_threshold = 0;
	int32_t b_bilinear_left = 0;
	int32_t b_bilinear_above  = 0;
	int32_t i_shift = 0;

	i_cu_width = x265_data_cu_get_width_p2(cu, 0) >> i_part_depth;
	i_cu_height = x265_data_cu_get_height_p2(cu, 0)>> i_part_depth;
	i_cu_width2 = i_cu_width<<1;
	i_cu_height2 = i_cu_height<<1;
	i_pic_stride = x265_image_get_stride(p_image);

	x265_data_cu_derive_left_right_top_idx_adi(h,
												cu,
												&i_part_idx_lt,
												&i_part_idx_rt,
												i_zorder_idx_in_part,
												i_part_depth );
	x265_data_cu_derive_left_bottom_idx_adi(h,
											cu,
											&i_part_idx_lb,
											i_zorder_idx_in_part,
											i_part_depth );

	i_unit_size = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth ;
	i_num_units_in_cu = i_cu_width / i_unit_size;
	i_total_units = (i_num_units_in_cu << 2) + 1;

	b_neighbor_flags[i_num_units_in_cu*2] = x265_pattern_is_above_left_available(h,
																				pattern,
																				cu,
																				i_part_idx_lt );
	i_num_intra_neighbor += (int32_t)(b_neighbor_flags[i_num_units_in_cu*2]);
	i_num_intra_neighbor += x265_pattern_is_above_available(h,
															pattern,
															cu,
															i_part_idx_lt,
															i_part_idx_rt,
															b_neighbor_flags+(i_num_units_in_cu*2)+1 );
	i_num_intra_neighbor += x265_pattern_is_above_right_available(h,
																	pattern,
																	cu,
																	i_part_idx_lt,
																	i_part_idx_rt,
																	b_neighbor_flags+(i_num_units_in_cu*3)+1 );
	i_num_intra_neighbor += x265_pattern_is_left_available(h,
															pattern,
															cu,
															i_part_idx_lt,
															i_part_idx_lb,
															b_neighbor_flags+(i_num_units_in_cu*2)-1 );
	i_num_intra_neighbor += x265_pattern_is_below_left_available(h,
																pattern,
																cu,
																i_part_idx_lt,
																i_part_idx_lb,
																b_neighbor_flags+ i_num_units_in_cu -1 );

	*p_above = 1;
	*p_left  = 1;
	i_width = i_cu_width2 + 1;
	i_height = i_cu_height2 + 1;

	p_roi_origin = x265_image_get_luma_addr_p3(h,
												p_image,
												x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu)
												+ i_zorder_idx_in_part);


	x265_pattern_fill_reference_samples(pattern,
										h->param.sps.i_bit_depth_y,
										p_roi_origin,
										adi_buf,
										i_org_buf_stride,
										b_neighbor_flags,
										i_num_intra_neighbor,
										i_unit_size,
										i_num_units_in_cu,
										i_total_units,
										i_cu_width,
										i_cu_height,
										i_width,
										i_height,
										i_pic_stride,
										b_l_mmode);

	// generate filtered intra prediction samples

	left1 = adi_buf ;
	top1 = left1 + i_org_buf_stride ;
	left2 = left1 + i_org_buf_stride * 2 ;
	top2 = left2 + i_org_buf_stride ;

	if(h->sps[0].b_use_strong_intra_smoothing)
	{
		i_blk_size = 32;
		i_bottom_left = left1[i_cu_height2-1];
		i_top_left = left1[-1];
		i_top_right = top1[i_cu_width2-1];
		i_threshold = 1 << (h->param.sps.i_bit_depth_y - 5);
		b_bilinear_left = abs(i_bottom_left+i_top_left-2*left1[i_cu_height-1]) < i_threshold;
		b_bilinear_above  = abs(i_top_left+i_top_right-2*top1[i_cu_height-1]) < i_threshold;

		if (i_cu_width>=i_blk_size && (b_bilinear_left && b_bilinear_above))
		{
			i_shift = h->global.convert_to_bit[i_cu_width] + 3;  // log2(i_cu_height2)
			left2[-1] = left1[-1];
			left2[i_cu_height2-1] = left1[i_cu_height2-1];
			top2[-1] = top1[-1];
			top2[i_cu_width2 - 1] = top1[i_cu_width2 - 1];
			for (loop = 1; loop < i_cu_height2; loop++)
			{
				left2[loop-1] = ((i_cu_height2 - loop) * i_top_left
										+ loop * i_bottom_left
										+ i_cu_height) >> i_shift;
			}

			for (loop = 1; loop < i_cu_width2; loop++)
			{
				top2[loop-1] = ((i_cu_width2-loop) * i_top_left
										+ loop*i_top_right
										+ i_cu_width) >> i_shift;
			}
		}
		else
		{
			// 1. filtering with [1 2 1]
			left2[-1] = (left1[0] + 2 * left1[-1] + top1[0] + 2 ) >> 2 ;
			for (loop = 0; loop < i_cu_height2-1; loop++)
			{
				left2[loop] = (left1[loop - 1] + 2 * left1[loop] + left1[loop + 1] + 2) >> 2;
			}
			left2[i_cu_height2-1] = left1[i_cu_height2-1];

			top2[-1] = left2[-1] ;
			for (loop = 0; loop < i_cu_width2 - 1; loop++)
			{
				top2[loop] = (top1[loop - 1] + 2 * top1[loop] + top1[loop + 1] + 2) >> 2;
			}
			top2[i_cu_width2-1] = top1[i_cu_width2-1];
		}
	}
	else
	{
		// 1. filtering with [1 2 1]
		left2[-1] = (left1[0] + 2 * left1[-1] + top1[0] + 2 ) >> 2 ;
		for (loop = 0; loop < i_cu_height2-1; loop++)
		{
			left2[loop] = (left1[loop - 1] + 2 * left1[loop] + left1[loop + 1] + 2) >> 2;
		}
		left2[i_cu_height2-1] = left1[i_cu_height2-1];

		top2[-1] = left2[-1] ;
		for (loop = 0; loop < i_cu_width2 - 1; loop++)
		{
			top2[loop] = (top1[loop - 1] + 2 * top1[loop] + top1[loop + 1] + 2) >> 2;
		}
		top2[i_cu_width2-1] = top1[i_cu_width2-1];
	}

}

void x265_pattern_init_adi_pattern_chroma ( x265_t *h,
										x265_pattern_t *pattern,
										x265_data_cu_t *cu,
										x265_image_t *p_image,
										uint32_t i_zorder_idx_in_part,
										uint32_t i_part_depth,
										pixel *adi_buf,
										int32_t i_org_buf_stride,
										int32_t i_org_buf_height,
										int32_t *p_above,
										int32_t *p_left )
{
	pixel *p_roi_origin = NULL;
	pixel *p_adi_temp = NULL;
	uint32_t i_cu_width = 0;
	uint32_t i_cu_height = 0;
	uint32_t i_width;
	uint32_t i_height;
	int32_t i_pic_stride = 0;
	int32_t i_unit_size = 0;
	int32_t i_num_units_in_cu = 0;
	int32_t i_total_units = 0;
	int32_t b_neighbor_flags[4 * X265_MAX_NUM_SPU_W + 1];
	int32_t i_num_intra_neighbor = 0;
	uint32_t i_part_idx_lt, i_part_idx_rt, i_part_idx_lb;

	i_cu_width  = x265_data_cu_get_width_p2(cu, 0) >> i_part_depth;
	i_cu_height = x265_data_cu_get_height_p2(cu, 0) >> i_part_depth;
	i_pic_stride = x265_image_get_c_stride(p_image);

	x265_data_cu_derive_left_right_top_idx_adi(h,
												cu,
												&i_part_idx_lt,
												&i_part_idx_rt,
												i_zorder_idx_in_part,
												i_part_depth );
	x265_data_cu_derive_left_bottom_idx_adi(h,
											cu,
											&i_part_idx_lb,
											i_zorder_idx_in_part,
											i_part_depth );

	i_unit_size = (h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth) >> 1; // for chroma
	i_num_units_in_cu  = (i_cu_width / i_unit_size) >> 1;            // for chroma
	i_total_units    = (i_num_units_in_cu << 2) + 1;

	b_neighbor_flags[i_num_units_in_cu*2] = x265_pattern_is_above_left_available(h,
																				pattern,
																				cu,
																				i_part_idx_lt );
	i_num_intra_neighbor += (int32_t)(b_neighbor_flags[i_num_units_in_cu*2]);
	i_num_intra_neighbor += x265_pattern_is_above_available(h,
															pattern,
															cu,
															i_part_idx_lt,
															i_part_idx_rt,
															b_neighbor_flags+(i_num_units_in_cu*2)+1 );
	i_num_intra_neighbor += x265_pattern_is_above_right_available(h,
																pattern,
																cu,
																i_part_idx_lt,
																i_part_idx_rt,
																b_neighbor_flags+(i_num_units_in_cu*3)+1 );
	i_num_intra_neighbor += x265_pattern_is_left_available(h,
															pattern,
															cu,
															i_part_idx_lt,
															i_part_idx_lb,
															b_neighbor_flags+(i_num_units_in_cu*2)-1 );
	i_num_intra_neighbor += x265_pattern_is_below_left_available(h,
																pattern,
																cu,
																i_part_idx_lt,
																i_part_idx_lb,
																b_neighbor_flags+ i_num_units_in_cu -1 );

	*p_above = 1;
	*p_left = 1;

	i_cu_width = i_cu_width >> 1;  // for chroma
	i_cu_height = i_cu_height >> 1;  // for chroma

	i_width = i_cu_width * 2 + 1;
	i_height = i_cu_height * 2 + 1;

	// get cb pattern
	p_roi_origin = x265_image_get_cb_addr_p3(h,
											p_image,
											x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu)
											+ i_zorder_idx_in_part);
	p_adi_temp = adi_buf;

	x265_pattern_fill_reference_samples (pattern,
										h->param.sps.i_bit_depth_c,
										p_roi_origin,
										p_adi_temp,
										i_org_buf_stride,
										b_neighbor_flags,
										i_num_intra_neighbor,
										i_unit_size,
										i_num_units_in_cu,
										i_total_units,
										i_cu_width,
										i_cu_height,
										i_width,
										i_height,
										i_pic_stride,
										0);

	// get cr pattern
	p_roi_origin = x265_image_get_cr_addr_p3(h,
											p_image,
											x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu)
											+ i_zorder_idx_in_part);
	p_adi_temp = adi_buf + i_org_buf_stride * 2;

	x265_pattern_fill_reference_samples(pattern,
										h->param.sps.i_bit_depth_c,
										p_roi_origin,
										p_adi_temp,
										i_org_buf_stride,
										b_neighbor_flags,
										i_num_intra_neighbor,
										i_unit_size,
										i_num_units_in_cu,
										i_total_units,
										i_cu_width,
										i_cu_height,
										i_width,
										i_height,
										i_pic_stride,
										0);
}

void x265_pattern_fill_reference_samples(x265_pattern_t *pattern,
										int32_t bit_depth,
										pixel *p_roi_origin,
										pixel *p_adi_temp,
										int32_t i_stride,
										int32_t *p_neighbor_flags,
										int32_t i_num_intra_neighbor,
										int32_t i_unit_size,
										int32_t i_num_units_in_cu,
										int32_t i_total_units,
										uint32_t i_cu_width,
										uint32_t i_cu_height,
										uint32_t i_width,
										uint32_t i_height,
										int32_t i_pic_stride,
										int32_t b_l_mmode )
{
	pixel* p_roi_temp;
	int32_t i, j;
	pixel i_dc_value = 0;
	int32_t i_num_units2 = 0;
	int32_t i_total_samples = 0;
	pixel p_adi_line[5 * X265_MAX_CU_SIZE];
	pixel *p_adi_line_temp;
	int32_t *pb_neighbor_flags;
	int32_t i_next, i_curr;
	pixel p_ref = 0;
	pixel *left = NULL ;
	pixel *top = NULL ;

	left = p_adi_temp ;
	top = left + i_stride ;


	i_dc_value = 1 << (bit_depth - 1);
	if (i_num_intra_neighbor == 0)
	{
		// fill border with dc value
		left [-1] = i_dc_value ;
		for (i=0; i<i_width-1; i++)
		{
			left[i] = i_dc_value;
		}
		top [-1] = i_dc_value ;
		for (i=0; i<i_height-1; i++)
		{
			top[i] = i_dc_value;
		}
	}
	else if (i_num_intra_neighbor == i_total_units)
	{
		// fill top-left border with rec. samples
		p_roi_temp = p_roi_origin - i_pic_stride - 1;
		left[-1] = p_roi_temp[0];
		top[-1] = p_roi_temp[0];

		// fill left border with rec. samples
		p_roi_temp = p_roi_origin - 1;

		if (b_l_mmode)
		{
			p_roi_temp --; // move to the second left column
		}

		for (i=0; i<i_cu_height; i++)
		{
			left[i] = p_roi_temp[0];
			p_roi_temp += i_pic_stride;
		}

		// fill below left border with rec. samples
		for (i=0; i<i_cu_height; i++)
		{
			left[i_cu_height+i] = p_roi_temp[0];
			p_roi_temp += i_pic_stride;
		}

		// fill top border with rec. samples
		p_roi_temp = p_roi_origin - i_pic_stride;
		for (i=0; i<i_cu_width; i++)
		{
			top[i] = p_roi_temp[i];
		}

		// fill top right border with rec. samples
		p_roi_temp = p_roi_origin - i_pic_stride + i_cu_width;
		for (i=0; i<i_cu_width; i++)
		{
			top[i_cu_width+i] = p_roi_temp[i];
		}
	}
	else // reference samples are partially available
	{
		i_num_units2 = i_num_units_in_cu<<1;
		i_total_samples = i_total_units*i_unit_size;

		// initialize
		for (i=0; i<i_total_samples; i++)
		{
			p_adi_line[i] = i_dc_value;
		}

		// fill top-left sample
		p_roi_temp = p_roi_origin - i_pic_stride - 1;
		p_adi_line_temp = p_adi_line + (i_num_units2*i_unit_size);
		pb_neighbor_flags = p_neighbor_flags + i_num_units2;
		if (*pb_neighbor_flags)
		{
			p_adi_line_temp[0] = p_roi_temp[0];
			for (i=1; i<i_unit_size; i++)
			{
				p_adi_line_temp[i] = p_adi_line_temp[0];
			}
		}

		// fill left & below-left samples
		p_roi_temp += i_pic_stride;
		if (b_l_mmode)
		{
			p_roi_temp --; // move the second left column
		}
		p_adi_line_temp--;
		pb_neighbor_flags--;
		for (j=0; j<i_num_units2; j++)
		{
			if (*pb_neighbor_flags)
			{
				for (i=0; i<i_unit_size; i++)
				{
					p_adi_line_temp[-i] = p_roi_temp[i*i_pic_stride];
				}
			}
			p_roi_temp += i_unit_size*i_pic_stride;
			p_adi_line_temp -= i_unit_size;
			pb_neighbor_flags--;
		}

		// fill above & above-right samples
		p_roi_temp = p_roi_origin - i_pic_stride;
		p_adi_line_temp = p_adi_line + ((i_num_units2+1)*i_unit_size);
		pb_neighbor_flags = p_neighbor_flags + i_num_units2 + 1;
		for (j=0; j<i_num_units2; j++)
		{
			if (*pb_neighbor_flags)
			{
				for (i=0; i<i_unit_size; i++)
				{
					p_adi_line_temp[i] = p_roi_temp[i];
				}
			}
			p_roi_temp += i_unit_size;
			p_adi_line_temp += i_unit_size;
			pb_neighbor_flags++;
		}

		// pad reference samples when necessary
		i_curr = 0;
		i_next = 1;
		p_adi_line_temp = p_adi_line;
		while (i_curr < i_total_units)
		{
			if (!p_neighbor_flags[i_curr])
			{
				if(i_curr == 0)
				{
					while (i_next < i_total_units && !p_neighbor_flags[i_next])
					{
						i_next++;
					}
					p_ref = p_adi_line[i_next*i_unit_size];
					// pad unavailable samples with new value
					while (i_curr < i_next)
					{
						for (i=0; i<i_unit_size; i++)
						{
							p_adi_line_temp[i] = p_ref;
						}
						p_adi_line_temp += i_unit_size;
						i_curr++;
					}
				}
				else
				{
					p_ref = p_adi_line[i_curr*i_unit_size-1];
					for (i=0; i<i_unit_size; i++)
					{
						p_adi_line_temp[i] = p_ref;
					}
					p_adi_line_temp += i_unit_size;
					i_curr++;
				}
			}
			else
			{
				p_adi_line_temp += i_unit_size;
				i_curr++;
			}
		}

		// copy processed samples
		p_adi_line_temp = p_adi_line + i_height + i_unit_size - 1;
		top[-1] = p_adi_line_temp[-1];
		for (i=0; i<i_width-1; i++)
		{
			top[i] = p_adi_line_temp[i];
		}
		left[-1] = p_adi_line_temp[-1];
		p_adi_line_temp = p_adi_line + i_height - 2;
		for (i=0; i<i_height-1; i++)
		{
			left[i] = p_adi_line_temp[-i];
		}
	}
}


pixel *x265_pattern_get_adi_org_buf ( x265_pattern_t *pattern,
										int32_t i_cu_width,
										int32_t i_cu_height,
										pixel *adi_buf,
										int32_t i_stride )
{
	return adi_buf;
}

pixel *x265_pattern_get_adi_cb_buf ( x265_pattern_t *pattern,
										int32_t i_cu_width,
										int32_t i_cu_height,
										pixel *adi_buf,
										int32_t i_stride )
{
	return adi_buf;
}

pixel *x265_pattern_get_adi_cr_buf ( x265_pattern_t *pattern,
										int32_t i_cu_width,
										int32_t i_cu_height,
										pixel *adi_buf,
										int32_t i_stride )
{
	return adi_buf + i_stride * 2 ;
}

/** get pointer to reference samples for intra prediction
 * \param i_dir_mode   prediction mode index
 * \param log2blk_size size of block (2 = 4x4, 3 = 8x8, 4 = 16x16, 5 = 32x32, 6 = 64x64)
 * \param p_adi_buf    pointer to unfiltered reference samples
 * \return            pointer to (possibly filtered) reference samples
 *
 * the prediction mode index is used to determine whether a smoothed reference sample buffer is returned.
 */


pixel* x265_pattern_get_predictor_ptr(x265_pattern_t *pattern,
										uint32_t i_dir_mode,
										uint32_t i_log2_blk_size,
										pixel *adi_buf,
										int32_t i_stride )
{
	pixel *p_src = NULL;
	int32_t i_diff = 0;
	uint8_t i_filt_idx = 0;
	int32_t i_width = 0;
	int32_t i_height = 0;

	assert(i_log2_blk_size >= 2 && i_log2_blk_size < 7);
	i_diff = X265_MIN (abs((int32_t) i_dir_mode - X265_HOR_IDX),
						abs((int32_t)i_dir_mode - X265_VER_IDX));
	i_filt_idx = i_diff > p_pattern_class_static->intra_filter[i_log2_blk_size - 2] ? 1 : 0;
	if (i_dir_mode == X265_DC_IDX)
	{
		i_filt_idx = 0; //no smoothing for dc or lm chroma
	}

	assert( i_filt_idx <= 1 );

	i_width = 1 << i_log2_blk_size;
	i_height = 1 << i_log2_blk_size;

	p_src = x265_pattern_get_adi_org_buf(pattern,
										i_width,
										i_height,
										adi_buf,
										i_stride );

	if ( i_filt_idx )
	{
		p_src += i_stride * 2;
	}

	return p_src;
}


int32_t x265_pattern_is_above_left_available(x265_t *h,
											x265_pattern_t *pattern,
											x265_data_cu_t *cu,
											uint32_t i_part_idx_lt )
{
	int32_t b_above_left_flag;
	uint32_t i_part_above_left;
	x265_base_data_cu_t* cu_above_left = NULL;

	cu_above_left = x265_data_cu_get_pu_above_left(h,
													cu,
													&i_part_above_left,
													i_part_idx_lt,
													1 );
	if(h->pps[0].b_constrained_intra_pred)
	{
		b_above_left_flag = ( cu_above_left
				&& x265_base_data_cu_get_prediction_mode_p2(cu_above_left, i_part_above_left ) == MODE_INTRA );
	}
	else
	{
		b_above_left_flag = (cu_above_left ? 1 : 0);
	}
	return b_above_left_flag;
}

int32_t x265_pattern_is_above_available(x265_t *h,
										x265_pattern_t *pattern,
										x265_data_cu_t* cu,
										uint32_t i_part_idx_lt,
										uint32_t i_part_idx_rt,
										int32_t *p_valid_flags )
{
	uint32_t i_raster_part_begin = 0;
	uint32_t i_raster_part_end = 0;
	uint32_t i_idx_step = 0;
	int32_t *pb_valid_flags = NULL;
	int32_t i_num_intra = 0;
	uint32_t i_raster_part = 0 ;
	uint32_t i_part_above;
    x265_base_data_cu_t* cu_above = NULL;

	i_raster_part_begin = h->scan.zscan_to_raster[i_part_idx_lt];
	i_raster_part_end = h->scan.zscan_to_raster[i_part_idx_rt]+1;
	i_idx_step = 1;
	pb_valid_flags = p_valid_flags;


	for ( i_raster_part = i_raster_part_begin;
			i_raster_part < i_raster_part_end;
			i_raster_part += i_idx_step )
	{
		cu_above = x265_base_data_cu_get_pu_above(h,
												(x265_base_data_cu_t*)cu,
												&i_part_above,
												h->scan.raster_to_zscan[i_raster_part],
												1, 0, 1);
		if(h->pps[0].b_constrained_intra_pred)
		{
			if ( cu_above
				&& x265_base_data_cu_get_prediction_mode_p2(cu_above, i_part_above ) == MODE_INTRA )
			{
				i_num_intra++;
				*pb_valid_flags = 1;
			}
			else
			{
				*pb_valid_flags = 0;
			}
		}
		else
		{
			if (cu_above)
			{
				i_num_intra++;
				*pb_valid_flags = 1;
			}
			else
			{
				*pb_valid_flags = 0;
			}
		}
		pb_valid_flags++;
	}
	return i_num_intra;
}

int32_t x265_pattern_is_left_available(x265_t *h,
										x265_pattern_t *pattern,
										x265_data_cu_t *cu,
										uint32_t i_part_idx_lt,
										uint32_t i_part_idx_lb,
										int32_t *p_valid_flags )
{
	uint32_t i_raster_part_begin = 0;
	uint32_t i_raster_part_end = 0;
	uint32_t i_idx_step = 0;
	int32_t *pb_valid_flags = 0;
	int32_t i_num_intra = 0;
	uint32_t i_raster_part = 0;
    uint32_t i_part_left;
    x265_base_data_cu_t* cu_left = NULL;

	i_raster_part_begin = h->scan.zscan_to_raster[i_part_idx_lt];
	i_raster_part_end = h->scan.zscan_to_raster[i_part_idx_lb]+1;
	i_idx_step = h->cu.pic.i_num_part_in_width ;
	pb_valid_flags = p_valid_flags;


	for ( i_raster_part = i_raster_part_begin;
			i_raster_part < i_raster_part_end;
			i_raster_part += i_idx_step )
	{
		cu_left = x265_base_data_cu_get_pu_left(h,
												(x265_base_data_cu_t*)cu,
												&i_part_left,
												h->scan.raster_to_zscan[i_raster_part],
												1, 1 ) ;
		if(h->pps[0].b_constrained_intra_pred)
		{
			if ( cu_left
					&& x265_base_data_cu_get_prediction_mode_p2(cu_left, i_part_left ) == MODE_INTRA )
			{
				i_num_intra++;
				*pb_valid_flags = 1;
			}
			else
			{
				*pb_valid_flags = 0;
			}
		}
		else
		{
			if ( cu_left )
			{
				i_num_intra++;
				*pb_valid_flags = 1;
			}
			else
			{
				*pb_valid_flags = 0;
			}
		}
		pb_valid_flags--; // opposite direction
	}

	return i_num_intra;
}

int32_t x265_pattern_is_above_right_available(x265_t *h,
												x265_pattern_t *pattern,
												x265_data_cu_t *cu,
												uint32_t i_part_idx_lt,
												uint32_t i_part_idx_rt,
												int32_t *p_valid_flags )
{
	uint32_t i_num_units_in_pu = 0;
	int32_t *pb_valid_flags = NULL;
	int32_t i_num_intra = 0;
	uint32_t i_offset = 1;
	uint32_t i_part_above_right;
	x265_base_data_cu_t* cu_above_right = NULL;

	i_num_units_in_pu = h->scan.zscan_to_raster[i_part_idx_rt] - h->scan.zscan_to_raster[i_part_idx_lt] + 1;
	pb_valid_flags = p_valid_flags;

	for ( i_offset = 1; i_offset <= i_num_units_in_pu; i_offset++ )
	{
		cu_above_right = x265_data_cu_get_pu_above_right_adi(h,
														cu,
														&i_part_above_right,
														i_part_idx_rt,
														i_offset,
														1);
		if(h->pps[0].b_constrained_intra_pred)
		{
			if ( cu_above_right
					&& x265_base_data_cu_get_prediction_mode_p2(cu_above_right, i_part_above_right ) == MODE_INTRA )
			{
				i_num_intra++;
				*pb_valid_flags = 1;
			}
			else
			{
				*pb_valid_flags = 0;
			}
		}
		else
		{
			if ( cu_above_right )
			{
				i_num_intra++;
				*pb_valid_flags = 1;
			}
			else
			{
				*pb_valid_flags = 0;
			}
		}
		pb_valid_flags++;
	}

	return i_num_intra;
}


int32_t x265_pattern_is_below_left_available(x265_t *h,
											x265_pattern_t *pattern,
											x265_data_cu_t *cu,
											uint32_t i_part_idx_lt,
											uint32_t i_part_idx_lb,
											int32_t *p_valid_flags )
{
	uint32_t i_num_units_in_pu = 0;
	int32_t *pb_valid_flags = 0;
	int32_t i_num_intra = 0;
	uint32_t i_offset = 0;
	uint32_t i_part_below_left = 0;
	x265_base_data_cu_t* cu_below_left = NULL;


	i_num_units_in_pu = (h->scan.zscan_to_raster[i_part_idx_lb] - h->scan.zscan_to_raster[i_part_idx_lt]) / h->cu.pic.i_num_part_in_width + 1;
	pb_valid_flags = p_valid_flags;
	for ( i_offset = 1; i_offset <= i_num_units_in_pu; i_offset++ )
	{
		cu_below_left = x265_data_cu_get_pu_below_left_adi(h,
															cu,
															&i_part_below_left,
															i_part_idx_lb,
															i_offset,
															1);

		if(h->pps[0].b_constrained_intra_pred)
		{
			if ( cu_below_left
					&& x265_base_data_cu_get_prediction_mode_p2(cu_below_left, i_part_below_left ) == MODE_INTRA )
			{
				i_num_intra++;
				*pb_valid_flags = 1;
			}
			else
			{
				*pb_valid_flags = 0;
			}
		}
		else
		{
			if ( cu_below_left )
			{
				i_num_intra++;
				*pb_valid_flags = 1;
			}
			else
			{
				*pb_valid_flags = 0;
			}
		}
		pb_valid_flags--; // opposite direction
	}

	return i_num_intra;
}

//! \}






