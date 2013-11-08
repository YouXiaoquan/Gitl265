

#include "common.h"


#define X265_EDGE_VER							0
#define X265_EDGE_HOR							1
#define x265_qp_uv(i_qp_y)  (((i_qp_y) < 0) ? (i_qp_y) : (((i_qp_y) > 57) ? ((i_qp_y)-6) : chroma_scale[(i_qp_y)]) )

#define X265_DEFAULT_INTRA_TC_OFFSET 2 ///< default intra tc offset

const uint8_t tc_table_8x8[54] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,5,5,6,6,7,8,9,10,11,13,14,16,18,20,22,24
};

const uint8_t beta_table_8x8[52] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,7,8,9,10,11,12,13,14,15,16,17,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64
};

x265_loop_filter_t *x265_loop_filter_new ()
{
	x265_loop_filter_t *loop_filter = NULL ;

	CHECKED_MALLOCZERO( loop_filter, sizeof(x265_loop_filter_t) );
	if ( x265_loop_filter_init (loop_filter) )
	{
		goto fail ;
	}

	return loop_filter ;
fail:
	x265_loop_filter_delete ( (void*) loop_filter ) ;
	return NULL ;
}



void x265_loop_filter_delete ( x265_loop_filter_t *loop_filter )
{
	x265_loop_filter_deinit ( loop_filter ) ;
	x265_free ( loop_filter ) ;
}


int x265_loop_filter_init ( x265_loop_filter_t *loop_filter )
{
	int32_t loop = 0 ;

	loop_filter->i_num_partitions = 0 ;
	loop_filter->b_lf_cross_tile_boundary = 1 ;
	for ( loop = 0 ; loop < 2 ; ++ loop )
	{
		loop_filter->bs[loop] = NULL ;
		loop_filter->edge_filter[loop] = NULL ;
	}

	return 0 ;
}

void x265_loop_filter_deinit ( x265_loop_filter_t *loop_filter )
{

}

void x265_loop_filter_set_cfg( x265_loop_filter_t *loop_filter, int32_t b_lf_cross_tile_boundary )
{
	loop_filter->b_lf_cross_tile_boundary = b_lf_cross_tile_boundary;
}

int x265_loop_filter_create ( x265_loop_filter_t *loop_filter, uint32_t i_max_cu_depth )
{
	uint32_t loop = 0 ;

	x265_loop_filter_destroy (loop_filter);
	loop_filter->i_num_partitions = 1 << ( i_max_cu_depth<<1 );
	for( loop = 0; loop < 2; ++ loop )
	{
		CHECKED_MALLOCZERO(loop_filter->bs[loop],
							sizeof(uint8_t) * loop_filter->i_num_partitions);
		CHECKED_MALLOCZERO(loop_filter->edge_filter[loop],
							sizeof(int32_t) * loop_filter->i_num_partitions);
	}

	return 0 ;

fail:
	x265_loop_filter_destroy (loop_filter);
	return -1 ;
}

void x265_loop_filter_destroy ( x265_loop_filter_t *loop_filter )
{
	uint32_t loop = 0 ;

	for( loop = 0; loop < 2; ++ loop )
	{
		x265_free (loop_filter->bs[loop]) ;
		loop_filter->bs[loop] = NULL ;

		x265_free (loop_filter->edge_filter[loop]) ;
		loop_filter->edge_filter[loop] = NULL ;
	}
}


uint32_t x265_loop_filter_x_calc_bs_idx_ver(x265_t *h,
											x265_loop_filter_t *loop_filter,
											x265_base_data_cu_t *cu,
											uint32_t i_abs_zorder_idx,
											int32_t i_edge_idx,
											int32_t i_base_unit_idx )
{
	uint32_t i_lcu_width_in_base_units = 0;

	i_lcu_width_in_base_units = h->cu.pic.i_num_part_in_width ;
	return h->scan.raster_to_zscan[h->scan.zscan_to_raster[i_abs_zorder_idx]
	                               + i_base_unit_idx * i_lcu_width_in_base_units
	                               + i_edge_idx ];
}

uint32_t x265_loop_filter_x_calc_bs_idx_hor(x265_t *h,
											x265_loop_filter_t *loop_filter,
											x265_base_data_cu_t *cu,
											uint32_t i_abs_zorder_idx,
											int32_t i_edge_idx,
											int32_t i_base_unit_idx )
{
	uint32_t i_lcu_width_in_base_units = 0 ;

	i_lcu_width_in_base_units = h->cu.pic.i_num_part_in_width ;
	return h->scan.raster_to_zscan[h->scan.zscan_to_raster[i_abs_zorder_idx]
	                               + i_edge_idx * i_lcu_width_in_base_units
	                               + i_base_unit_idx ];
}


void x265_loop_filter_loop_filter_pic(x265_t *h,
									x265_loop_filter_t *loop_filter,
									x265_frame_t *frame )
{
	uint32_t i_cua_ddr = 0;
	x265_data_cu_t *cu = NULL;

	// horizontal filtering
	for ( i_cua_ddr = 0; i_cua_ddr < h->cu.pic.i_num_cus_in_frame; ++ i_cua_ddr )
	{
		cu = h->data_cu;

		memset( loop_filter->bs[X265_EDGE_VER], 0, sizeof(uint8_t) * loop_filter->i_num_partitions );
		memset( loop_filter->edge_filter[X265_EDGE_VER], 0, sizeof(int32_t) * loop_filter->i_num_partitions );

		// cu-based deblocking
		x265_loop_filter_x_deblock_cu_ver(h, loop_filter, cu, 0, 0 );
	}

	// vertical filtering
	for ( i_cua_ddr = 0; i_cua_ddr < h->cu.pic.i_num_cus_in_frame; ++ i_cua_ddr )
	{
		cu = h->data_cu;

		memset( loop_filter->bs[X265_EDGE_HOR], 0, sizeof(uint8_t) * loop_filter->i_num_partitions );
		memset( loop_filter->edge_filter[X265_EDGE_HOR], 0, sizeof(int32_t) * loop_filter->i_num_partitions );


		// cu-based deblocking
		x265_loop_filter_x_deblock_cu_hor(h, loop_filter, cu, 0, 0 );
	}
}

void x265_loop_filter_loop_filter_cu(x265_t *h,
									x265_loop_filter_t *loop_filter,
									x265_data_cu_t *cu )
{
	x265_base_data_cu_t *left = NULL ;
	uint32_t i_a_part_unit_idx = 0 ;

	memset( loop_filter->bs[X265_EDGE_VER], 0, sizeof(uint8_t) * loop_filter->i_num_partitions );
	memset( loop_filter->edge_filter[X265_EDGE_VER], 0, sizeof(int32_t) * loop_filter->i_num_partitions );

	// cu-based deblocking
	x265_loop_filter_x_deblock_cu_ver(h, loop_filter, cu, 0, 0 );

	if ( loop_filter->b_lf_cross_tile_boundary )
	{
		if ( !x265_pic_sym_is_pic_left_edge(h, cu->base_data_cu.i_cu_addr) )
		{
			left = x265_base_data_cu_get_pu_left(h,
												(x265_base_data_cu_t*)cu,
												&i_a_part_unit_idx,
												0,
												!h->slice->b_lf_cross_slice_boundary_flag,
												!loop_filter->b_lf_cross_tile_boundary);

			// cu-based deblocking
			x265_loop_filter_x_deblock_last_row_or_column_hor(h, loop_filter, left );
		}
	}
	else
	{
		if ( !x265_pic_sym_is_tile_left_edge(h, cu->base_data_cu.i_cu_addr) )
		{
			left = x265_base_data_cu_get_pu_left(h,
												(x265_base_data_cu_t*)cu,
												&i_a_part_unit_idx,
												0,
												!h->slice->b_lf_cross_slice_boundary_flag,
												!loop_filter->b_lf_cross_tile_boundary);

			// cu-based deblocking
			x265_loop_filter_x_deblock_last_row_or_column_hor(h, loop_filter, left );
		}
	}

	memset( loop_filter->bs[X265_EDGE_HOR], 0, sizeof(uint8_t) * loop_filter->i_num_partitions );
	memset( loop_filter->edge_filter[X265_EDGE_HOR], 0, sizeof(int32_t) * loop_filter->i_num_partitions );
	x265_loop_filter_x_get_deblock_cu_parameter_hor(h, loop_filter, cu, 0, 0 );
	x265_loop_filter_x_deblock_front_rows_or_columns_hor(h, loop_filter, cu );

	if ( x265_pic_sym_is_pic_right_edge(h, cu->base_data_cu.i_cu_addr) )
	{
		// cu-based deblocking
		x265_loop_filter_x_deblock_last_row_or_column_hor(h, loop_filter, (x265_base_data_cu_t*)cu );
	}
	else if ( (!loop_filter->b_lf_cross_tile_boundary)
			&& x265_pic_sym_is_tile_right_edge(h, cu->base_data_cu.i_cu_addr) )
	{
		// cu-based deblocking
		x265_loop_filter_x_deblock_last_row_or_column_hor(h, loop_filter, (x265_base_data_cu_t*)cu );
	}

}

void x265_loop_filter_x_get_deblock_cu_parameter_ver(x265_t *h,
													x265_loop_filter_t *loop_filter,
													x265_data_cu_t *cu,
													uint32_t i_abs_zorder_idx,
													uint32_t i_depth )
{
	uint32_t i_cur_num_parts = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_part_idx = 0;
	uint32_t i_l_pixel_x = 0;
	uint32_t i_t_pixel_y = 0;
	uint32_t i_b_scheck = 0;

	if(x265_data_cu_get_partition_size_p2(cu, i_abs_zorder_idx) == SIZE_NONE)
	{
		return;
	}
	i_cur_num_parts = h->cu.pic.i_num_partitions >> (i_depth<<1);
	i_q_num_parts = i_cur_num_parts>>2;

	if( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) > i_depth )
	{
		for ( i_part_idx = 0; i_part_idx < 4; i_part_idx++, i_abs_zorder_idx+=i_q_num_parts )
		{
			i_l_pixel_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			i_t_pixel_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			if(( i_l_pixel_x < h->sps[0].i_pic_width_in_luma_samples)
					&& (i_t_pixel_y < h->sps[0].i_pic_height_in_luma_samples))
			{
				x265_loop_filter_x_get_deblock_cu_parameter_ver(h,
																loop_filter,
																cu,
																i_abs_zorder_idx,
																i_depth + 1 );
			}
		}
		return;
	}

	x265_loop_filter_x_set_loop_filter_param_ver(h,
												loop_filter,
												cu,
												i_abs_zorder_idx );

	x265_loop_filter_x_set_edge_filter_tu_ver (h,
												loop_filter,
												cu,
												i_abs_zorder_idx,
												i_abs_zorder_idx,
												i_depth );
	x265_loop_filter_x_set_edge_filter_pu_ver (h,
												loop_filter,
												cu,
												i_abs_zorder_idx );

	for( i_part_idx = i_abs_zorder_idx; i_part_idx < i_abs_zorder_idx + i_cur_num_parts; i_part_idx++ )
	{
		if((h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth) == 4 )
		{
			i_b_scheck = (i_part_idx%2 == 0) ;
		}
		else
		{
			i_b_scheck = 1;
		}

		if ( loop_filter->edge_filter[X265_EDGE_VER][i_part_idx] && i_b_scheck )
		{
			x265_loop_filter_x_get_boundary_strength_single_ver(h,
																loop_filter,
																cu,
																i_part_idx );
		}
	}
}


void x265_loop_filter_x_get_deblock_cu_parameter_hor(x265_t *h,
													x265_loop_filter_t *loop_filter,
													x265_data_cu_t *cu,
													uint32_t i_abs_zorder_idx,
													uint32_t i_depth )
{
	uint32_t i_cur_num_parts = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_part_idx = 0;
	uint32_t i_l_pixel_x = 0;
	uint32_t i_t_pixel_y = 0;
	uint32_t i_b_scheck = 0;

	if(x265_data_cu_get_partition_size_p2(cu, i_abs_zorder_idx) == SIZE_NONE)
	{
		return;
	}
	i_cur_num_parts = h->cu.pic.i_num_partitions >> (i_depth<<1);
	i_q_num_parts = i_cur_num_parts>>2;

	if( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) > i_depth )
	{
		for ( i_part_idx = 0; i_part_idx < 4; i_part_idx++, i_abs_zorder_idx+=i_q_num_parts )
		{
			i_l_pixel_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			i_t_pixel_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			if(( i_l_pixel_x < h->sps[0].i_pic_width_in_luma_samples)
					&& (i_t_pixel_y < h->sps[0].i_pic_height_in_luma_samples))
			{
				x265_loop_filter_x_get_deblock_cu_parameter_hor(h,
																loop_filter,
																cu,
																i_abs_zorder_idx,
																i_depth + 1 );
			}
		}
		return;
	}

	x265_loop_filter_x_set_loop_filter_param_hor(h,
												loop_filter,
												cu,
												i_abs_zorder_idx );

	x265_loop_filter_x_set_edge_filter_tu_hor (h,
												loop_filter,
												cu,
												i_abs_zorder_idx,
												i_abs_zorder_idx,
												i_depth );
	x265_loop_filter_x_set_edge_filter_pu_hor (h,
												loop_filter,
												cu,
												i_abs_zorder_idx );

	for( i_part_idx = i_abs_zorder_idx; i_part_idx < i_abs_zorder_idx + i_cur_num_parts; i_part_idx++ )
	{
		if((h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth) == 4 )
		{
			i_b_scheck = ((i_part_idx-((i_part_idx>>2)<<2))/2 == 0) ;
		}
		else
		{
			i_b_scheck = 1;
		}

		if ( loop_filter->edge_filter[X265_EDGE_HOR][i_part_idx] && i_b_scheck )
		{
			x265_loop_filter_x_get_boundary_strength_single_hor(h,
																loop_filter,
																cu,
																i_part_idx );
		}
	}
}



// ====================================================================================================================
// protected member functions
// ====================================================================================================================

/**
 - deblocking filter process in cu-based (the same function as conventional's)
 .
 \param edge          the direction of the edge in block boundary (horizonta/vertical), which is added newly
*/
void x265_loop_filter_x_deblock_cu_ver(x265_t *h,
										x265_loop_filter_t *loop_filter,
										x265_data_cu_t *cu,
										uint32_t i_abs_zorder_idx,
										uint32_t i_depth )
{
	uint32_t i_cur_num_parts = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_part_idx = 0;
	uint32_t i_l_pixel_x = 0;
	uint32_t i_t_pixel_y = 0;
	uint32_t i_b_scheck = 0;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_part_idx_incr = 0;
	uint32_t i_size_in_pu = 0;
	uint32_t i_edge = 0;

	if(x265_data_cu_get_partition_size_p2(cu, i_abs_zorder_idx) == SIZE_NONE)
	{
		return;
	}
	i_cur_num_parts = h->cu.pic.i_num_partitions >> (i_depth<<1);
	i_q_num_parts = i_cur_num_parts>>2;

	if( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) > i_depth )
	{
		for ( i_part_idx = 0; i_part_idx < 4; i_part_idx++, i_abs_zorder_idx+=i_q_num_parts )
		{
			i_l_pixel_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			i_t_pixel_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			if(( i_l_pixel_x < h->sps[0].i_pic_width_in_luma_samples)
					&& (i_t_pixel_y < h->sps[0].i_pic_height_in_luma_samples))
			{
				x265_loop_filter_x_deblock_cu_ver(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_depth + 1 );
			}
		}
		return;
	}

	x265_loop_filter_x_set_loop_filter_param_ver(h,
												loop_filter,
												cu,
												i_abs_zorder_idx );

	x265_loop_filter_x_set_edge_filter_tu_ver (h,
												loop_filter,
												cu,
												i_abs_zorder_idx,
												i_abs_zorder_idx,
												i_depth );
	x265_loop_filter_x_set_edge_filter_pu_ver (h,
												loop_filter,
												cu,
												i_abs_zorder_idx );

	for( i_part_idx = i_abs_zorder_idx; i_part_idx < i_abs_zorder_idx + i_cur_num_parts; i_part_idx++ )
	{
		if((h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth) == 4 )
		{
			i_b_scheck = (i_part_idx%2 == 0) ;
		}
		else
		{
			i_b_scheck = 1;
		}

		if ( loop_filter->edge_filter[X265_EDGE_VER][i_part_idx] && i_b_scheck )
		{
			x265_loop_filter_x_get_boundary_strength_single_ver(h,
																loop_filter,
																cu,
																i_part_idx );
		}
	}


	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth;
	i_part_idx_incr = X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part ? X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part : 1 ;
	i_size_in_pu = h->cu.pic.i_num_part_in_width>>(i_depth);
	for ( i_edge = 0; i_edge < i_size_in_pu ; i_edge += i_part_idx_incr)
	{
		x265_loop_filter_x_edge_filter_luma_ver(h,
												loop_filter,
												cu,
												i_abs_zorder_idx,
												i_depth,
												i_edge );
		if ( (i_pixels_in_part>X265_DEBLOCK_SMALLEST_BLOCK)
				|| (i_edge % ( (X265_DEBLOCK_SMALLEST_BLOCK<<1)/i_pixels_in_part ) ) == 0 )
		{
			x265_loop_filter_x_edge_filter_chroma_ver(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_depth,
													i_edge );
		}
	}
}

/**
 - deblocking filter process in cu-based (the same function as conventional's)
 .
 \param edge          the direction of the edge in block boundary (horizonta/vertical), which is added newly
*/
void x265_loop_filter_x_deblock_cu_hor(x265_t *h,
										x265_loop_filter_t *loop_filter,
										x265_data_cu_t *cu,
										uint32_t i_abs_zorder_idx,
										uint32_t i_depth )
{
	uint32_t i_cur_num_parts = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_part_idx = 0;
	uint32_t i_l_pixel_x = 0;
	uint32_t i_t_pixel_y = 0;
	uint32_t i_b_scheck = 0;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_part_idx_incr = 0;
	uint32_t i_size_in_pu = 0;
	uint32_t i_edge = 0;


	if(x265_data_cu_get_partition_size_p2(cu, i_abs_zorder_idx) == SIZE_NONE)
	{
		return;
	}
	i_cur_num_parts = h->cu.pic.i_num_partitions >> (i_depth<<1);
	i_q_num_parts = i_cur_num_parts>>2;

	if( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) > i_depth )
	{
		for ( i_part_idx = 0; i_part_idx < 4; i_part_idx++, i_abs_zorder_idx+=i_q_num_parts )
		{
			i_l_pixel_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			i_t_pixel_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			if(( i_l_pixel_x < h->sps[0].i_pic_width_in_luma_samples)
					&& (i_t_pixel_y < h->sps[0].i_pic_height_in_luma_samples))
			{
				x265_loop_filter_x_deblock_cu_hor(h,
												loop_filter,
												cu,
												i_abs_zorder_idx,
												i_depth + 1 );
			}
		}
		return;
	}

	x265_loop_filter_x_set_loop_filter_param_hor(h,
												loop_filter,
												cu,
												i_abs_zorder_idx );

	x265_loop_filter_x_set_edge_filter_tu_hor (h,
												loop_filter,
												cu,
												i_abs_zorder_idx,
												i_abs_zorder_idx,
												i_depth );
	x265_loop_filter_x_set_edge_filter_pu_hor (h,
												loop_filter,
												cu,
												i_abs_zorder_idx );

	for( i_part_idx = i_abs_zorder_idx; i_part_idx < i_abs_zorder_idx + i_cur_num_parts; i_part_idx++ )
	{
		if((h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth) == 4 )
		{
			i_b_scheck = ((i_part_idx-((i_part_idx>>2)<<2))/2 == 0) ;
		}
		else
		{
			i_b_scheck = 1;
		}

		if ( loop_filter->edge_filter[X265_EDGE_HOR][i_part_idx] && i_b_scheck )
		{
			x265_loop_filter_x_get_boundary_strength_single_hor(h,
																loop_filter,
																cu,
																i_part_idx );
		}
	}



	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth;
	i_part_idx_incr = X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part ? X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part : 1 ;
	i_size_in_pu = h->cu.pic.i_num_part_in_width>>(i_depth);

	for ( i_edge = 0; i_edge < i_size_in_pu ; i_edge += i_part_idx_incr)
	{
		x265_loop_filter_x_edge_filter_luma_hor(h,
												loop_filter,
												cu,
												i_abs_zorder_idx,
												i_depth,
												i_edge );
		if ( (i_pixels_in_part>X265_DEBLOCK_SMALLEST_BLOCK)
				|| (i_edge % ( (X265_DEBLOCK_SMALLEST_BLOCK<<1)/i_pixels_in_part ) ) == 0 )
		{
			x265_loop_filter_x_edge_filter_chroma_hor(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_depth,
													i_edge );
		}
	}
}

void x265_loop_filter_x_deblock_last_row_or_column_ver(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_base_data_cu_t *cu )
{
	uint32_t i_abs_zorder_idx = 0 ;
	uint32_t i_depth = 0 ;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_part_idx_incr = 0;
	uint32_t i_size_in_pu = 0;
	uint32_t i_part_idx_xory = 0 ;
	uint32_t i_edge = 0;

	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth;
	i_part_idx_incr = X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part ? X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part : 1 ;
	i_size_in_pu = h->cu.pic.i_num_part_in_width;

	for ( i_edge = 0; i_edge < i_size_in_pu ; i_edge += i_part_idx_incr)
	{
		i_part_idx_xory = h->cu.pic.i_num_part_in_width - 1 ;
		i_abs_zorder_idx = h->scan.raster_to_zscan [i_edge
						 + i_part_idx_xory * h->cu.pic.i_num_part_in_width ] ;
		i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) ;
		x265_loop_filter_x_edge_filter_luma_pu_ver(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_depth,
													0 );
		if ( (i_pixels_in_part>X265_DEBLOCK_SMALLEST_BLOCK)
				|| (i_edge % ( (X265_DEBLOCK_SMALLEST_BLOCK<<1)/i_pixels_in_part ) ) == 0 )
		{
			x265_loop_filter_x_edge_filter_chroma_pu_ver(h,
														loop_filter,
														cu,
														i_abs_zorder_idx,
														i_depth,
														0 );
		}
	}
}

void x265_loop_filter_x_deblock_last_row_or_column_hor(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_base_data_cu_t *cu )
{
	uint32_t i_abs_zorder_idx = 0 ;
	uint32_t i_depth = 0 ;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_part_idx_incr = 0;
	uint32_t i_size_in_pu = 0;
	uint32_t i_part_idx_xory = 0 ;
	uint32_t i_edge = 0;

	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth;
	i_part_idx_incr = X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part ? X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part : 1 ;
	i_size_in_pu = h->cu.pic.i_num_part_in_width;

	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth;
	i_part_idx_incr = X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part ? X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part : 1 ;
	i_size_in_pu = h->cu.pic.i_num_part_in_width;
	i_part_idx_xory = 0 ;
	for ( i_edge = 0; i_edge < i_size_in_pu ; i_edge += i_part_idx_incr)
	{
		i_part_idx_xory = h->cu.pic.i_num_part_in_width - 1 ;
		i_abs_zorder_idx = h->scan.raster_to_zscan [i_edge* h->cu.pic.i_num_part_in_width
						 + i_part_idx_xory ] ;
		i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) ;
		x265_loop_filter_x_edge_filter_luma_pu_hor(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_depth,
													0 );
		if ( (i_pixels_in_part>X265_DEBLOCK_SMALLEST_BLOCK)
				|| (i_edge % ( (X265_DEBLOCK_SMALLEST_BLOCK<<1)/i_pixels_in_part ) ) == 0 )
		{
			x265_loop_filter_x_edge_filter_chroma_pu_hor(h,
														loop_filter,
														cu,
														i_abs_zorder_idx,
														i_depth,
														0 );
		}
	}
}

void x265_loop_filter_x_deblock_front_rows_or_columns_ver(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_data_cu_t *cu )
{
	uint32_t i_abs_zorder_idx = 0 ;
	uint32_t i_depth = 0 ;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_part_idx_incr = 0;
	uint32_t i_size_in_pu = 0;
	uint32_t i_part_idx_xory = 0 ;
	uint32_t i_edge = 0;

	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth;
	i_part_idx_incr = X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part ? X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part : 1 ;
	i_size_in_pu = h->cu.pic.i_num_part_in_width;

	for ( i_edge = 0; i_edge < i_size_in_pu ; i_edge += i_part_idx_incr)
	{
		for ( i_part_idx_xory = 0 ;
				i_part_idx_xory < h->cu.pic.i_num_part_in_width - 1 ;
				++ i_part_idx_xory )
		{
			i_abs_zorder_idx = h->scan.raster_to_zscan [i_edge
							 + i_part_idx_xory * h->cu.pic.i_num_part_in_width ] ;
			i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) ;
			x265_loop_filter_x_edge_filter_luma_pu_ver(h,
														loop_filter,
														(x265_base_data_cu_t*)cu,
														i_abs_zorder_idx,
														i_depth,
														0 );
			if ( (i_pixels_in_part>X265_DEBLOCK_SMALLEST_BLOCK)
					|| (i_edge % ( (X265_DEBLOCK_SMALLEST_BLOCK<<1)/i_pixels_in_part ) ) == 0 )
			{
				x265_loop_filter_x_edge_filter_chroma_pu_ver(h,
															loop_filter,
															(x265_base_data_cu_t*)cu,
															i_abs_zorder_idx,
															i_depth,
															0 );
			}
		}
	}

}

void x265_loop_filter_x_deblock_front_rows_or_columns_hor(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_data_cu_t *cu)
{
	uint32_t i_abs_zorder_idx = 0 ;
	uint32_t i_depth = 0 ;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_part_idx_incr = 0 ;
	uint32_t i_size_in_pu = 0;
	uint32_t i_part_idx_xory = 0 ;
	uint32_t i_edge = 0;

	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth;
	i_part_idx_incr = X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part ? X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part : 1 ;
	i_size_in_pu = h->cu.pic.i_num_part_in_width;

	for ( i_edge = 0; i_edge < i_size_in_pu ; i_edge += i_part_idx_incr)
	{
		for ( i_part_idx_xory = 0 ;
				i_part_idx_xory < h->cu.pic.i_num_part_in_width - 1 ;
				++ i_part_idx_xory )
		{
			i_abs_zorder_idx = h->scan.raster_to_zscan [i_edge* h->cu.pic.i_num_part_in_width
							 + i_part_idx_xory ] ;
			i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) ;
			x265_loop_filter_x_edge_filter_luma_pu_hor(h,
														loop_filter,
														(x265_base_data_cu_t*)cu,
														i_abs_zorder_idx,
														i_depth,
														0 );
			if ( (i_pixels_in_part>X265_DEBLOCK_SMALLEST_BLOCK)
					|| (i_edge % ( (X265_DEBLOCK_SMALLEST_BLOCK<<1)/i_pixels_in_part ) ) == 0 )
			{
				x265_loop_filter_x_edge_filter_chroma_pu_hor(h,
															loop_filter,
															(x265_base_data_cu_t*)cu,
															i_abs_zorder_idx,
															i_depth,
															0 );
			}
		}
	}

}


void x265_loop_filter_x_set_edge_filter_multiple_ver(x265_t *h,
													x265_loop_filter_t *loop_filter,
													x265_data_cu_t *cu,
													uint32_t i_scan_idx,
													uint32_t i_depth,
													int32_t i_edge_idx,
													int32_t b_value,
													uint32_t i_width_in_base_units,
													uint32_t i_height_in_base_units )
{
	uint32_t loop = 0;
	uint32_t i_num_elem = 0;
	uint32_t i_bs_idx = 0;

	if ( i_width_in_base_units == 0 )
	{
		i_width_in_base_units = h->cu.pic.i_num_part_in_width >> i_depth;
	}
	if ( i_height_in_base_units == 0 )
	{
		i_height_in_base_units = h->cu.pic.i_num_part_in_height >> i_depth;
	}
	i_num_elem = i_height_in_base_units;
	assert( i_num_elem > 0 );
	assert( i_width_in_base_units > 0 );
	assert( i_height_in_base_units > 0 );
	for( loop = 0; loop < i_num_elem; ++ loop )
	{
		i_bs_idx = x265_loop_filter_x_calc_bs_idx_ver(h,
													loop_filter,
													(x265_base_data_cu_t*)cu,
													i_scan_idx,
													i_edge_idx,
													loop );
		loop_filter->edge_filter[X265_EDGE_VER][i_bs_idx] = b_value;
		if (i_edge_idx == 0)
		{
			loop_filter->bs[X265_EDGE_VER][i_bs_idx] = b_value;
		}
	}
}

void x265_loop_filter_x_set_edge_filter_multiple_hor(x265_t *h,
													x265_loop_filter_t *loop_filter,
													x265_data_cu_t *cu,
													uint32_t i_scan_idx,
													uint32_t i_depth,
													int32_t i_edge_idx,
													int32_t b_value,
													uint32_t i_width_in_base_units,
													uint32_t i_height_in_base_units )
{
	uint32_t loop = 0;
	uint32_t i_num_elem = 0;
	uint32_t i_bs_idx = 0;

	if ( i_width_in_base_units == 0 )
	{
		i_width_in_base_units = h->cu.pic.i_num_part_in_width >> i_depth;
	}
	if ( i_height_in_base_units == 0 )
	{
		i_height_in_base_units = h->cu.pic.i_num_part_in_height >> i_depth;
	}
	i_num_elem = i_width_in_base_units;
	assert( i_num_elem > 0 );
	assert( i_width_in_base_units > 0 );
	assert( i_height_in_base_units > 0 );
	for( loop = 0; loop < i_num_elem; ++loop )
	{
		i_bs_idx = x265_loop_filter_x_calc_bs_idx_hor(h,
													loop_filter,
													(x265_base_data_cu_t*)cu,
													i_scan_idx,
													i_edge_idx,
													loop );
		loop_filter->edge_filter[X265_EDGE_HOR][i_bs_idx] = b_value;
		if (i_edge_idx == 0)
		{
			loop_filter->bs[X265_EDGE_HOR][i_bs_idx] = b_value;
		}
	}
}

void x265_loop_filter_x_set_edge_filter_tu_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_tu_part_idx,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth )
{
	uint32_t i_cur_num_parts = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_part_idx = 0;
	uint32_t ns_addr = 0;
	int32_t i_tr_width = 0;;
	int32_t i_tr_height = 0;
	uint32_t i_width_in_base_units = 0;
	uint32_t i_height_in_base_units = 0;



	if( x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx )
		+ x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) > i_depth )
	{
		i_cur_num_parts = h->cu.pic.i_num_partitions >> (i_depth<<1);
		i_q_num_parts = i_cur_num_parts >> 2;
		for ( i_part_idx = 0; i_part_idx < 4; i_part_idx++, i_abs_zorder_idx+=i_q_num_parts )
		{
			ns_addr = i_abs_zorder_idx;
			x265_loop_filter_x_set_edge_filter_tu_ver(h,
													loop_filter,
													cu,
													ns_addr,
													i_abs_zorder_idx,
													i_depth + 1 );
		}
		return;
	}

	i_tr_width = x265_data_cu_get_width_p2(cu, i_abs_zorder_idx )
					>> x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx );
	i_tr_height = x265_data_cu_get_height_p2(cu, i_abs_zorder_idx )
					>> x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx );

	i_width_in_base_units = i_tr_width / (h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth);
	i_height_in_base_units = i_tr_height / (h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth);

	x265_loop_filter_x_set_edge_filter_multiple_ver(h,
													loop_filter,
													cu,
													i_abs_tu_part_idx,
													i_depth,
													0,
													loop_filter->lf_cu_param.b_internal_edge,
													i_width_in_base_units,
													i_height_in_base_units );
}


void x265_loop_filter_x_set_edge_filter_tu_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_tu_part_idx,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth )
{
	uint32_t i_cur_num_parts = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_part_idx = 0;
	uint32_t ns_addr = 0;
	int32_t i_tr_width = 0;;
	int32_t i_tr_height = 0;
	uint32_t i_width_in_base_units = 0;
	uint32_t i_height_in_base_units = 0;



	if( x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx )
		+ x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) > i_depth )
	{
		i_cur_num_parts = h->cu.pic.i_num_partitions >> (i_depth<<1);
		i_q_num_parts = i_cur_num_parts >> 2;
		for ( i_part_idx = 0; i_part_idx < 4; i_part_idx++, i_abs_zorder_idx+=i_q_num_parts )
		{
			ns_addr = i_abs_zorder_idx;
			x265_loop_filter_x_set_edge_filter_tu_hor(h,
													loop_filter,
													cu,
													ns_addr,
													i_abs_zorder_idx,
													i_depth + 1 );
		}
		return;
	}

	i_tr_width = x265_data_cu_get_width_p2(cu, i_abs_zorder_idx )
					>> x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx );
	i_tr_height = x265_data_cu_get_height_p2(cu, i_abs_zorder_idx )
					>> x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx );

	i_width_in_base_units = i_tr_width / (h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth);
	i_height_in_base_units = i_tr_height / (h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth);

	x265_loop_filter_x_set_edge_filter_multiple_hor(h,
													loop_filter,
													cu,
													i_abs_tu_part_idx,
													i_depth,
													0,
													loop_filter->lf_cu_param.b_internal_edge,
													i_width_in_base_units,
													i_height_in_base_units );
}



void x265_loop_filter_x_set_edge_filter_pu_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx )
{
	uint32_t i_depth = 0;
	uint32_t i_width_in_base_units = 0;
	uint32_t i_h_width_in_base_units = 0;
	uint32_t i_q_width_in_base_units = 0;


	i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx );
	i_width_in_base_units = h->cu.pic.i_num_part_in_width >> i_depth;
	i_h_width_in_base_units  = i_width_in_base_units  >> 1;
	i_q_width_in_base_units  = i_width_in_base_units  >> 2;

	x265_loop_filter_x_set_edge_filter_multiple_ver(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_depth,
													0,
													loop_filter->lf_cu_param.b_left_edge,
													0,
													0);

	switch ( x265_data_cu_get_partition_size_p2(cu, i_abs_zorder_idx ) )
	{
	case SIZE_2Nx2N:
    {
    	break;
    }
    case SIZE_Nx2N:
    {
    	x265_loop_filter_x_set_edge_filter_multiple_ver(h,
    													loop_filter,
    													cu,
    													i_abs_zorder_idx,
    													i_depth,
    													i_h_width_in_base_units,
    													loop_filter->lf_cu_param.b_internal_edge,
    													0,
    													0);
    	break;
    }
    case SIZE_NxN:
    {
    	x265_loop_filter_x_set_edge_filter_multiple_ver(h,
    													loop_filter,
    													cu,
    													i_abs_zorder_idx,
    													i_depth,
    													i_h_width_in_base_units,
    													loop_filter->lf_cu_param.b_internal_edge,
    													0,
    													0);
    	break;
    }
    case SIZE_nLx2N:
    {
    	x265_loop_filter_x_set_edge_filter_multiple_ver(h,
    													loop_filter,
    													cu,
    													i_abs_zorder_idx,
    													i_depth,
    													i_q_width_in_base_units,
    													loop_filter->lf_cu_param.b_internal_edge,
    													0,
    													0);
    	break;
    }
    case SIZE_nRx2N:
    {
    	x265_loop_filter_x_set_edge_filter_multiple_ver(h,
    													loop_filter,
    													cu,
    													i_abs_zorder_idx,
    													i_depth,
    													i_width_in_base_units - i_q_width_in_base_units,
    													loop_filter->lf_cu_param.b_internal_edge,
    													0,
    													0);
    	break;
    }
    default:
    {
    	break;
    }
	}
}

void x265_loop_filter_x_set_edge_filter_pu_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx )
{
	uint32_t i_depth = 0;
	uint32_t i_height_in_base_units = 0;
	uint32_t i_h_height_in_base_units = 0;
	uint32_t i_q_height_in_base_units = 0;

	i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx );
	i_height_in_base_units = h->cu.pic.i_num_part_in_height >> i_depth;
	i_h_height_in_base_units = i_height_in_base_units >> 1;
	i_q_height_in_base_units = i_height_in_base_units >> 2;


	x265_loop_filter_x_set_edge_filter_multiple_hor(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_depth,
													0,
													loop_filter->lf_cu_param.b_top_edge,
													0,
													0);

	switch ( x265_data_cu_get_partition_size_p2(cu, i_abs_zorder_idx ) )
	{
    case SIZE_2Nx2N:
    {
    	break;
    }
    case SIZE_2NxN:
    {
    	x265_loop_filter_x_set_edge_filter_multiple_hor(h,
    													loop_filter,
    													cu,
    													i_abs_zorder_idx,
    													i_depth,
    													i_h_height_in_base_units,
    													loop_filter->lf_cu_param.b_internal_edge,
    													0,
    													0);
    	break;
    }
    case SIZE_NxN:
    {
    	x265_loop_filter_x_set_edge_filter_multiple_hor(h,
    													loop_filter,
    													cu,
    													i_abs_zorder_idx,
    													i_depth,
    													i_h_height_in_base_units,
    													loop_filter->lf_cu_param.b_internal_edge,
    													0,
    													0);
    	break;
    }
    case SIZE_2NxnU:
    {
    	x265_loop_filter_x_set_edge_filter_multiple_hor(h,
    													loop_filter,
    													cu,
    													i_abs_zorder_idx,
    													i_depth,
    													i_q_height_in_base_units,
    													loop_filter->lf_cu_param.b_internal_edge,
    													0,
    													0);
    	break;
    }
    case SIZE_2NxnD:
    {
    	x265_loop_filter_x_set_edge_filter_multiple_hor(h,
    													loop_filter,
    													cu,
    													i_abs_zorder_idx,
    													i_depth,
    													i_height_in_base_units - i_q_height_in_base_units,
    													loop_filter->lf_cu_param.b_internal_edge,
    													0,
    													0);
    	break;
    }
    default:
    {
    	break;
    }
	}
}


void x265_loop_filter_x_set_loop_filter_param_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx )
{
	uint32_t i_x = 0;
	x265_base_data_cu_t *temp_cu = NULL;
	uint32_t i_temp_part_idx = 0;

	i_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_abs_zorder_idx]];

	loop_filter->lf_cu_param.b_internal_edge = !h->slice->b_deblocking_filter_disable ;

	if ( (i_x == 0) || h->slice->b_deblocking_filter_disable )
	{
		loop_filter->lf_cu_param.b_left_edge = 0;
	}
	else
	{
		loop_filter->lf_cu_param.b_left_edge = 1;
	}
	if ( loop_filter->lf_cu_param.b_left_edge )
	{
		temp_cu = x265_base_data_cu_get_pu_left(h,
												(x265_base_data_cu_t*)cu,
												&i_temp_part_idx,
												i_abs_zorder_idx,
												!h->slice->b_lf_cross_slice_boundary_flag,
												!loop_filter->b_lf_cross_tile_boundary);
		if ( temp_cu )
		{
			loop_filter->lf_cu_param.b_left_edge = 1;
		}
		else
		{
			loop_filter->lf_cu_param.b_left_edge = 0;
		}
	}
}


void x265_loop_filter_x_set_loop_filter_param_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx )
{
	uint32_t i_y = 0;
	x265_base_data_cu_t *temp_cu = NULL;
	uint32_t i_temp_part_idx = 0;

	i_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_abs_zorder_idx]];

	loop_filter->lf_cu_param.b_internal_edge = !h->slice->b_deblocking_filter_disable ;
	if ( (i_y == 0 ) || h->slice->b_deblocking_filter_disable )
	{
		loop_filter->lf_cu_param.b_top_edge = 0;
	}
	else
	{
		loop_filter->lf_cu_param.b_top_edge = 1;
	}
	if ( loop_filter->lf_cu_param.b_top_edge )
	{
		temp_cu = x265_base_data_cu_get_pu_above(h,
												(x265_base_data_cu_t*)cu,
												&i_temp_part_idx,
												i_abs_zorder_idx,
												!h->slice->b_lf_cross_slice_boundary_flag,
												0,
												!loop_filter->b_lf_cross_tile_boundary);

		if ( temp_cu )
		{
			loop_filter->lf_cu_param.b_top_edge = 1;
		}
		else
		{
			loop_filter->lf_cu_param.b_top_edge = 0;
		}
	}
}




void x265_loop_filter_x_get_boundary_strength_single_ver(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_data_cu_t *cu,
														uint32_t i_abs_part_idx )
{
	x265_slice_t *slice = NULL;
	uint32_t i_part_q = 0;
	x265_data_cu_t *cu_q = NULL;
	uint32_t i_part_p = 0;
	x265_base_data_cu_t *cu_p = NULL;
	uint32_t i_bs = 0;
    uint32_t ns_part_q = 0;
    uint32_t ns_part_p = 0;
    int32_t i_ref_idx = 0;
    x265_frame_t *p_ref_p0, *p_ref_p1, *p_ref_q0, *p_ref_q1;
    x265_mv_t mv_p0 ;
    x265_mv_t mv_p1 ;
    x265_mv_t mv_q0 ;
    x265_mv_t mv_q1 ;

    slice = h->slice;
    i_part_q = i_abs_part_idx;
    cu_q = cu;

    //-- calculate block index
    cu_p = x265_base_data_cu_get_pu_left(h,
    									(x265_base_data_cu_t*)cu_q,
    									&i_part_p,
    									i_part_q,
    									!h->slice->b_lf_cross_slice_boundary_flag,
    									!loop_filter->b_lf_cross_tile_boundary);

    //-- set bs for int32_tra mb : bs = 4 or 3
    if (x265_base_data_cu_is_intra(cu_p, i_part_p)
    		|| x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu_q, i_part_q))
    {
    	i_bs = 2;
    }

    //-- set bs for not int32_tra mb : bs = 2 or 1 or 0
    if ((!x265_base_data_cu_is_intra(cu_p, i_part_p))
    		&& (!x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu_q, i_part_q)))
    {
    	ns_part_q = i_part_q;
    	ns_part_p = i_part_p;

    	if ( loop_filter->bs[X265_EDGE_VER][i_abs_part_idx]
    	     && (x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu_q,
    	    		 	 	 	 	 	 	 ns_part_q,
    	    		 	 	 	 	 	 	 TEXT_LUMA,
    	    		 	 	 	 	 	 	 x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu_q,
    	    		 	 	 	 	 	 			 	 	 	 	 	 	 	 	 ns_part_q)) != 0
    	    	|| x265_base_data_cu_get_cbf_p4(cu_p,
    	    									ns_part_p,
    	    									TEXT_LUMA,
    	    									x265_base_data_cu_get_transform_idx_p2(cu_p,
    	    																		ns_part_p) ) != 0))
    	{
    		i_bs = 1;
    	}
    	else
    	{
    		if (B_SLICE == slice->i_slice_type)
    		{
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_0),
    													i_part_p);
    			p_ref_p0 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_0][i_ref_idx];
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_1),
    													i_part_p);
    			p_ref_p1 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_1][i_ref_idx];
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																						REF_PIC_LIST_0),
    													i_part_q);
    			p_ref_q0 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_0][i_ref_idx];
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																						REF_PIC_LIST_1),
    													i_part_q);
    			p_ref_q1 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_1][i_ref_idx];

    			memcpy(&mv_p0,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_0),
    											i_part_p),
    					sizeof(x265_mv_t));
    			memcpy(&mv_p1,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_1),
    											i_part_p),
    					sizeof(x265_mv_t));
    			memcpy(&mv_q0,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																				REF_PIC_LIST_0),
    											i_part_q),
    					sizeof(x265_mv_t));
    			memcpy(&mv_q1,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																				REF_PIC_LIST_1),
    											i_part_q),
    					sizeof(x265_mv_t));

    			if (p_ref_p0 == NULL)
    			{
    				x265_mv_set_zero(&mv_p0);
    			}
    			if (p_ref_p1 == NULL)
    			{
    				x265_mv_set_zero(&mv_p1);
    			}
    			if (p_ref_q0 == NULL)
    			{
    				x265_mv_set_zero(&mv_q0);
    			}
    			if (p_ref_q1 == NULL)
    			{
    				x265_mv_set_zero(&mv_q1);
    			}
    			if (((p_ref_p0 == p_ref_q0) && (p_ref_p1==p_ref_q1))
    				|| ((p_ref_p0 == p_ref_q1) && (p_ref_p1 == p_ref_q0)))
    			{
    				i_bs = 0;
    				if ( p_ref_p0 != p_ref_p1 )   // different l0 & l1
    				{
    					if ( p_ref_p0 == p_ref_q0 )
    					{
    						i_bs = ((abs(mv_q0.i_hor - mv_p0.i_hor) >= 4) ||
    								(abs(mv_q0.i_ver - mv_p0.i_ver) >= 4) ||
    								(abs(mv_q1.i_hor - mv_p1.i_hor) >= 4) ||
    								(abs(mv_q1.i_ver - mv_p1.i_ver) >= 4)) ? 1 : 0;
    					}
    					else
    					{
    						i_bs = ((abs(mv_q1.i_hor - mv_p0.i_hor) >= 4) ||
    								(abs(mv_q1.i_ver - mv_p0.i_ver) >= 4) ||
    								(abs(mv_q0.i_hor - mv_p1.i_hor) >= 4) ||
    								(abs(mv_q0.i_ver - mv_p1.i_ver) >= 4)) ? 1 : 0;
    					}
    				}
    				else    // same l0 & l1
    				{
    					i_bs = ((abs(mv_q0.i_hor - mv_p0.i_hor) >= 4) ||
    							(abs(mv_q0.i_ver - mv_p0.i_ver) >= 4) ||
    							(abs(mv_q1.i_hor - mv_p1.i_hor) >= 4) ||
    							(abs(mv_q1.i_ver - mv_p1.i_ver) >= 4))
    						&& ((abs(mv_q1.i_hor - mv_p0.i_hor) >= 4) ||
    							(abs(mv_q1.i_ver - mv_p0.i_ver) >= 4) ||
    							(abs(mv_q0.i_hor - mv_p1.i_hor) >= 4) ||
    							(abs(mv_q0.i_ver - mv_p1.i_ver) >= 4)) ? 1 : 0;
    				}
    			}
    			else // for all different ref_idx
    			{
    				i_bs = 1;
    			}
    		}
    		else  // slice->isint32_ter_p()
    		{
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_0),
    													i_part_p);
    			p_ref_p0 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_0][i_ref_idx];
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																						REF_PIC_LIST_0),
    													i_part_q);
    			p_ref_q0 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_0][i_ref_idx];
    			memcpy(&mv_p0,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_0),
    											i_part_p),
    					sizeof(x265_mv_t));
    			memcpy(&mv_q0,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																				REF_PIC_LIST_0),
    											i_part_q),
    					sizeof(x265_mv_t));
    			if (p_ref_p0 == NULL)
    			{
    				x265_mv_set_zero(&mv_p0);
    			}
    			if (p_ref_q0 == NULL)
    			{
    				x265_mv_set_zero(&mv_q0);
    			}

    			i_bs = ((p_ref_p0 != p_ref_q0) ||
    					(abs(mv_q0.i_hor - mv_p0.i_hor) >= 4) ||
    					(abs(mv_q0.i_ver - mv_p0.i_ver) >= 4)) ? 1 : 0;
    		}
    	}   // enf of "if( one of bcbp == 0 )"
    }   // enf of "if( not int32_tra )"
    loop_filter->bs[X265_EDGE_VER][i_abs_part_idx] = i_bs;
}



void x265_loop_filter_x_get_boundary_strength_single_hor(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_data_cu_t *cu,
														uint32_t i_abs_part_idx )
{
	x265_slice_t *slice = NULL;
	uint32_t i_part_q = 0;
	x265_data_cu_t *cu_q = NULL;
	uint32_t i_part_p = 0;
	x265_base_data_cu_t *cu_p = NULL;
	uint32_t i_bs = 0;
    uint32_t ns_part_q = 0;
    uint32_t ns_part_p = 0;
    int32_t i_ref_idx = 0;
    x265_frame_t *p_ref_p0, *p_ref_p1, *p_ref_q0, *p_ref_q1;
    x265_mv_t mv_p0 ;
    x265_mv_t mv_p1 ;
    x265_mv_t mv_q0 ;
    x265_mv_t mv_q1 ;

    slice = h->slice;
    i_part_q = i_abs_part_idx;
    cu_q = cu;

    //-- calculate block index
    cu_p = x265_base_data_cu_get_pu_above(h,
    									(x265_base_data_cu_t*)cu_q,
    									&i_part_p,
    									i_part_q,
    									!h->slice->b_lf_cross_slice_boundary_flag,
    									0,
    									!loop_filter->b_lf_cross_tile_boundary);
    //-- set bs for int32_tra mb : bs = 4 or 3
    if (x265_base_data_cu_is_intra(cu_p, i_part_p)
    		|| x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu_q, i_part_q))
    {
    	i_bs = 2;
    }

    //-- set bs for not int32_tra mb : bs = 2 or 1 or 0
    if ((!x265_base_data_cu_is_intra(cu_p, i_part_p))
    		&& (!x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu_q, i_part_q)))
    {
    	ns_part_q = i_part_q;
    	ns_part_p = i_part_p;

    	if ( loop_filter->bs[X265_EDGE_HOR][i_abs_part_idx]
    	     && (x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu_q,
    	    		 	 	 	 	 	 	 ns_part_q,
    	    		 	 	 	 	 	 	 TEXT_LUMA,
    	    		 	 	 	 	 	 	 x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu_q,
    	    		 	 	 	 	 	 			 	 	 	 	 	 	 	 	 ns_part_q)) != 0
    	    	|| x265_base_data_cu_get_cbf_p4(cu_p,
    	    									ns_part_p,
    	    									TEXT_LUMA,
    	    									x265_base_data_cu_get_transform_idx_p2(cu_p,
    	    																		ns_part_p) ) != 0))
    	{
    		i_bs = 1;
    	}
    	else
    	{
    	    cu_p = x265_base_data_cu_get_pu_above(h,
    	    									(x265_base_data_cu_t*)cu_q,
    	    									&i_part_p,
    	    									i_part_q,
    	    									!h->slice->b_lf_cross_slice_boundary_flag,
    	    									0,
    	    									!loop_filter->b_lf_cross_tile_boundary);
    		if (B_SLICE == slice->i_slice_type)
    		{
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_0),
    													i_part_p);
    			p_ref_p0 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_0][i_ref_idx];
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_1),
    													i_part_p);
    			p_ref_p1 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_1][i_ref_idx];
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																						REF_PIC_LIST_0),
    													i_part_q);
    			p_ref_q0 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_0][i_ref_idx];
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																						REF_PIC_LIST_1),
    													i_part_q);
    			p_ref_q1 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_1][i_ref_idx];

    			memcpy(&mv_p0,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_0),
    											i_part_p),
    					sizeof(x265_mv_t));
    			memcpy(&mv_p1,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_1),
    											i_part_p),
    					sizeof(x265_mv_t));
    			memcpy(&mv_q0,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																				REF_PIC_LIST_0),
    											i_part_q),
    					sizeof(x265_mv_t));
    			memcpy(&mv_q1,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																				REF_PIC_LIST_1),
    											i_part_q),
    					sizeof(x265_mv_t));

    			if (p_ref_p0 == NULL)
    			{
    				x265_mv_set_zero(&mv_p0);
    			}
    			if (p_ref_p1 == NULL)
    			{
    				x265_mv_set_zero(&mv_p1);
    			}
    			if (p_ref_q0 == NULL)
    			{
    				x265_mv_set_zero(&mv_q0);
    			}
    			if (p_ref_q1 == NULL)
    			{
    				x265_mv_set_zero(&mv_q1);
    			}
    			if (((p_ref_p0 == p_ref_q0) && (p_ref_p1==p_ref_q1))
    				|| ((p_ref_p0 == p_ref_q1) && (p_ref_p1 == p_ref_q0)))
    			{
    				i_bs = 0;
    				if ( p_ref_p0 != p_ref_p1 )   // different l0 & l1
    				{
    					if ( p_ref_p0 == p_ref_q0 )
    					{
    						i_bs = ((abs(mv_q0.i_hor - mv_p0.i_hor) >= 4) ||
    								(abs(mv_q0.i_ver - mv_p0.i_ver) >= 4) ||
    								(abs(mv_q1.i_hor - mv_p1.i_hor) >= 4) ||
    								(abs(mv_q1.i_ver - mv_p1.i_ver) >= 4)) ? 1 : 0;
    					}
    					else
    					{
    						i_bs = ((abs(mv_q1.i_hor - mv_p0.i_hor) >= 4) ||
    								(abs(mv_q1.i_ver - mv_p0.i_ver) >= 4) ||
    								(abs(mv_q0.i_hor - mv_p1.i_hor) >= 4) ||
    								(abs(mv_q0.i_ver - mv_p1.i_ver) >= 4)) ? 1 : 0;
    					}
    				}
    				else    // same l0 & l1
    				{
    					i_bs = ((abs(mv_q0.i_hor - mv_p0.i_hor) >= 4) ||
    							(abs(mv_q0.i_ver - mv_p0.i_ver) >= 4) ||
    							(abs(mv_q1.i_hor - mv_p1.i_hor) >= 4) ||
    							(abs(mv_q1.i_ver - mv_p1.i_ver) >= 4))
    						&& ((abs(mv_q1.i_hor - mv_p0.i_hor) >= 4) ||
    							(abs(mv_q1.i_ver - mv_p0.i_ver) >= 4) ||
    							(abs(mv_q0.i_hor - mv_p1.i_hor) >= 4) ||
    							(abs(mv_q0.i_ver - mv_p1.i_ver) >= 4)) ? 1 : 0;
    				}
    			}
    			else // for all different ref_idx
    			{
    				i_bs = 1;
    			}
    		}
    		else  // slice->isint32_ter_p()
    		{
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_0),
    													i_part_p);
    			p_ref_p0 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_0][i_ref_idx];
    			i_ref_idx = x265_cu_mv_field_get_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																						REF_PIC_LIST_0),
    													i_part_q);
    			p_ref_q0 = (i_ref_idx < 0) ? NULL : h->fref[REF_PIC_LIST_0][i_ref_idx];
    			memcpy(&mv_p0,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field(cu_p, REF_PIC_LIST_0),
    											i_part_p),
    					sizeof(x265_mv_t));
    			memcpy(&mv_q0,
    					x265_cu_mv_field_get_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_q,
    																				REF_PIC_LIST_0),
    											i_part_q),
    					sizeof(x265_mv_t));
    			if (p_ref_p0 == NULL)
    			{
    				x265_mv_set_zero(&mv_p0);
    			}
    			if (p_ref_q0 == NULL)
    			{
    				x265_mv_set_zero(&mv_q0);
    			}

    			i_bs = ((p_ref_p0 != p_ref_q0) ||
    					(abs(mv_q0.i_hor - mv_p0.i_hor) >= 4) ||
    					(abs(mv_q0.i_ver - mv_p0.i_ver) >= 4)) ? 1 : 0;
    		}
    	}   // enf of "if( one of bcbp == 0 )"
    }   // enf of "if( not int32_tra )"
    loop_filter->bs[X265_EDGE_HOR][i_abs_part_idx] = i_bs;
}


void x265_loop_filter_x_edge_filter_luma_ver(x265_t *h,
											x265_loop_filter_t *loop_filter,
											x265_data_cu_t *cu,
											uint32_t i_abs_zorder_idx,
											uint32_t i_depth,
											int32_t i_edge  )
{
	int32_t loop = 0;
	int32_t i_stride = 0;
	int32_t i_qp = 0;
	int32_t i_qp_p = 0;
	int32_t i_qp_q = 0;
	uint32_t i_num_parts = 0;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_bs_abs_idx = 0 ;
	uint32_t i_bs = 0;
	int32_t i_offset = 0;
	int32_t i_src_step;
	int32_t b_pcm_filter = 0;
	int32_t b_part_p_no_filter = 0;
	int32_t b_part_q_no_filter = 0;
	uint32_t i_part_p_idx = 0;
	uint32_t  i_part_q_idx = 0;
	int32_t i_beta_offset_div2 = 0;
	int32_t i_tc_offset_div2 = 0;
	uint32_t i_idx = 0;
	int32_t i_bit_depth_scale = 0;
	int32_t i_index_tc = 0;
	int32_t i_index_b = 0;
	int32_t i_tc = 0;
	int32_t i_beta = 0;
	int32_t i_side_threshold = 0;
	int32_t i_thr_cut = 0;
	uint32_t i_blocks_in_part = 0;
	uint32_t i_blk_idx = 0;
	int32_t i_dp0 = 0;
	int32_t i_dq0 = 0;
	int32_t i_dp3 = 0;
	int32_t i_dq3 = 0;
	int32_t i_d0 = 0;
	int32_t i_d3 = 0;
	int32_t i_dp = 0;
	int32_t i_dq = 0;
	int32_t i_d =  0;
	int32_t b_filter_p = 0;
	int32_t b_filter_q = 0;
	int32_t b_sw = 0;
	pixel *p_src = NULL;
	pixel *p_tmp_src = NULL;
	x265_frame_t *frame = NULL;
	x265_base_data_cu_t *cu_p = NULL;
	x265_data_cu_t *cu_q = NULL;

	frame = h->fdec;
	p_src = x265_frame_get_luma_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										i_abs_zorder_idx );
	p_tmp_src = p_src;
	i_stride = frame->i_stride[0];
	i_num_parts = h->cu.pic.i_num_part_in_width >> i_depth;
	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth ;
	b_pcm_filter = (h->sps[0].b_use_pcm && h->sps[0].b_pcm_filter_disable_flag)? 1 : 0;
	cu_p = (x265_base_data_cu_t*)cu;
	cu_q = cu;
	i_beta_offset_div2 = h->slice->i_deblocking_filter_beta_offset_div2 ;
	i_tc_offset_div2 = h->slice->i_deblocking_filter_tc_offset_div2 ;
	i_offset = 1;
	i_src_step = i_stride;
	p_tmp_src += i_edge * i_pixels_in_part;

	for ( i_idx = 0; i_idx < i_num_parts; i_idx++ )
	{
		i_bs_abs_idx = x265_loop_filter_x_calc_bs_idx_ver(h,
														loop_filter,
														(x265_base_data_cu_t*)cu,
														i_abs_zorder_idx,
														i_edge,
														i_idx);
		i_bs = loop_filter->bs[X265_EDGE_VER][i_bs_abs_idx];
		if ( i_bs )
		{
			i_qp_q = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, i_bs_abs_idx );
			i_part_q_idx = i_bs_abs_idx;
			// derive neighboring pu index
			cu_p = x265_base_data_cu_get_pu_left(h,
												(x265_base_data_cu_t*)cu_q,
												&i_part_p_idx,
												i_part_q_idx,
												!h->slice->b_lf_cross_slice_boundary_flag,
												!loop_filter->b_lf_cross_tile_boundary);

			i_qp_p = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu_p, i_part_p_idx);
			i_qp = (i_qp_p + i_qp_q + 1) >> 1;
			i_bit_depth_scale = 1 << (h->param.sps.i_bit_depth_y-8);

			i_index_tc = x265_clip3_int32((int32_t)(i_qp + X265_DEFAULT_INTRA_TC_OFFSET*(i_bs - 1) + (i_tc_offset_div2 << 1)),
											0, X265_MAX_QP + X265_DEFAULT_INTRA_TC_OFFSET);
			i_index_b = x265_clip3_int32(i_qp + (i_beta_offset_div2 << 1), 0, X265_MAX_QP);
			i_tc =  tc_table_8x8[i_index_tc]*i_bit_depth_scale;
			i_beta = beta_table_8x8[i_index_b]*i_bit_depth_scale;
			i_side_threshold = (i_beta+(i_beta>>1))>>3;
			i_thr_cut = i_tc*10;
			i_blocks_in_part = i_pixels_in_part / 4 ? i_pixels_in_part / 4 : 1;
			for ( i_blk_idx = 0; i_blk_idx<i_blocks_in_part; i_blk_idx ++)
			{
				i_dp0 = x265_loop_filter_x_calc_dp(loop_filter,
													p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+0),
													i_offset);
				i_dq0 = x265_loop_filter_x_calc_dq(loop_filter,
													p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+0),
													i_offset);
				i_dp3 = x265_loop_filter_x_calc_dp( loop_filter,
													p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+3),
													i_offset);
				i_dq3 = x265_loop_filter_x_calc_dq( loop_filter,
													p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+3),
													i_offset);
				i_d0 = i_dp0 + i_dq0;
				i_d3 = i_dp3 + i_dq3;

				i_dp = i_dp0 + i_dp3;
				i_dq = i_dq0 + i_dq3;
				i_d =  i_d0 + i_d3;

				if (b_pcm_filter || h->pps[0].b_transquant_bypass_enable_flag)
				{
					// check if each of pus is i_pcm with lf disabling
					b_part_p_no_filter = (b_pcm_filter
											&& x265_base_data_cu_get_ipcm_flag_p2(cu_p, i_part_p_idx));
					b_part_q_no_filter = (b_pcm_filter
											&& x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu_q,
																					i_part_q_idx));

					// check if each of pus is lossless coded
					b_part_p_no_filter = b_part_p_no_filter
											|| (x265_base_data_cu_is_lossless_coded(h,
																					cu_p,
																					i_part_p_idx));
					b_part_q_no_filter = b_part_q_no_filter
											|| (x265_base_data_cu_is_lossless_coded(h,
																					(x265_base_data_cu_t*)cu_q,
																					i_part_q_idx));
				}

				if (i_d < i_beta)
				{
					b_filter_p = (i_dp < i_side_threshold);
					b_filter_q = (i_dq < i_side_threshold);

					b_sw = x265_loop_filter_x_use_strong_filtering(loop_filter, i_offset, 2*i_d0, i_beta, i_tc, p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+0))
        						&& x265_loop_filter_x_use_strong_filtering(loop_filter, i_offset, 2*i_d3, i_beta, i_tc, p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+3));

					for ( loop = 0; loop < X265_DEBLOCK_SMALLEST_BLOCK / 2; ++ loop )
					{
						x265_loop_filter_x_pixel_filter_luma(h,
															loop_filter,
															p_tmp_src + i_src_step * (i_idx * i_pixels_in_part + i_blk_idx * 4 + loop),
															i_offset,
															i_tc,
															b_sw,
															b_part_p_no_filter,
															b_part_q_no_filter,
															i_thr_cut,
															b_filter_p,
															b_filter_q);
					}
				}
			}
		}
	}
}


void x265_loop_filter_x_edge_filter_luma_hor(x265_t *h,
											x265_loop_filter_t *loop_filter,
											x265_data_cu_t *cu,
											uint32_t i_abs_zorder_idx,
											uint32_t i_depth,
											int32_t i_edge  )
{
	int32_t loop = 0;
	int32_t i_stride = 0;
	int32_t i_qp = 0;
	int32_t i_qp_p = 0;
	int32_t i_qp_q = 0;
	uint32_t i_num_parts = 0;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_bs_abs_idx = 0 ;
	uint32_t i_bs = 0;
	int32_t i_offset = 0;
	int32_t i_src_step;
	int32_t b_pcm_filter = 0;
	int32_t b_part_p_no_filter = 0;
	int32_t b_part_q_no_filter = 0;
	uint32_t i_part_p_idx = 0;
	uint32_t  i_part_q_idx = 0;
	int32_t i_beta_offset_div2 = 0;
	int32_t i_tc_offset_div2 = 0;
	uint32_t i_idx = 0;
	int32_t i_bit_depth_scale = 0;
	int32_t i_index_tc = 0;
	int32_t i_index_b = 0;
	int32_t i_tc = 0;
	int32_t i_beta = 0;
	int32_t i_side_threshold = 0;
	int32_t i_thr_cut = 0;
	uint32_t i_blocks_in_part = 0;
	uint32_t i_blk_idx = 0;
	int32_t i_dp0 = 0;
	int32_t i_dq0 = 0;
	int32_t i_dp3 = 0;
	int32_t i_dq3 = 0;
	int32_t i_d0 = 0;
	int32_t i_d3 = 0;
	int32_t i_dp = 0;
	int32_t i_dq = 0;
	int32_t i_d =  0;
	int32_t b_filter_p = 0;
	int32_t b_filter_q = 0;
	int32_t b_sw = 0;
	pixel *p_src = NULL;
	pixel *p_tmp_src = NULL;
	x265_frame_t *frame = NULL;
	x265_base_data_cu_t *cu_p = NULL;
	x265_data_cu_t *cu_q = NULL;

	frame = h->fdec;
	p_src = x265_frame_get_luma_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										i_abs_zorder_idx );
	p_tmp_src = p_src;
	i_stride = frame->i_stride[0];
	i_num_parts = h->cu.pic.i_num_part_in_width >> i_depth;
	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth ;
	b_pcm_filter = (h->sps[0].b_use_pcm && h->sps[0].b_pcm_filter_disable_flag)? 1 : 0;
	cu_p = (x265_base_data_cu_t*)cu;
	cu_q = cu;
	i_beta_offset_div2 = h->slice->i_deblocking_filter_beta_offset_div2 ;
	i_tc_offset_div2 = h->slice->i_deblocking_filter_tc_offset_div2 ;
	i_offset = i_stride;
	i_src_step = 1;
	p_tmp_src += i_edge*i_pixels_in_part*i_stride;

	for ( i_idx = 0; i_idx < i_num_parts; i_idx++ )
	{
		i_bs_abs_idx = x265_loop_filter_x_calc_bs_idx_hor(h,
														loop_filter,
														(x265_base_data_cu_t*)cu,
														i_abs_zorder_idx,
														i_edge,
														i_idx);
		i_bs = loop_filter->bs[X265_EDGE_HOR][i_bs_abs_idx];
		if ( i_bs )
		{
			i_qp_q = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, i_bs_abs_idx );
			i_part_q_idx = i_bs_abs_idx;
			// derive neighboring pu index
			cu_p = x265_base_data_cu_get_pu_above(h,
												(x265_base_data_cu_t*)cu_q,
												&i_part_p_idx,
												i_part_q_idx,
												!h->slice->b_lf_cross_slice_boundary_flag,
												0,
												!loop_filter->b_lf_cross_tile_boundary);
			i_qp_p = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu_p, i_part_p_idx);
			i_qp = (i_qp_p + i_qp_q + 1) >> 1;
			i_bit_depth_scale = 1 << (h->param.sps.i_bit_depth_y-8);

			i_index_tc = x265_clip3_int32((int32_t)(i_qp + X265_DEFAULT_INTRA_TC_OFFSET*(i_bs - 1) + (i_tc_offset_div2 << 1)),
											0, X265_MAX_QP + X265_DEFAULT_INTRA_TC_OFFSET);
			i_index_b = x265_clip3_int32(i_qp + (i_beta_offset_div2 << 1), 0, X265_MAX_QP);
			i_tc =  tc_table_8x8[i_index_tc]*i_bit_depth_scale;
			i_beta = beta_table_8x8[i_index_b]*i_bit_depth_scale;
			i_side_threshold = (i_beta+(i_beta>>1))>>3;
			i_thr_cut = i_tc*10;
			i_blocks_in_part = i_pixels_in_part / 4 ? i_pixels_in_part / 4 : 1;
			for ( i_blk_idx = 0; i_blk_idx<i_blocks_in_part; i_blk_idx ++)
			{
				i_dp0 = x265_loop_filter_x_calc_dp(loop_filter,
													p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+0),
													i_offset);
				i_dq0 = x265_loop_filter_x_calc_dq(loop_filter,
													p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+0),
													i_offset);
				i_dp3 = x265_loop_filter_x_calc_dp( loop_filter,
													p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+3),
													i_offset);
				i_dq3 = x265_loop_filter_x_calc_dq( loop_filter,
													p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+3),
													i_offset);
				i_d0 = i_dp0 + i_dq0;
				i_d3 = i_dp3 + i_dq3;

				i_dp = i_dp0 + i_dp3;
				i_dq = i_dq0 + i_dq3;
				i_d =  i_d0 + i_d3;

				if (b_pcm_filter || h->pps[0].b_transquant_bypass_enable_flag)
				{
					// check if each of pus is i_pcm with lf disabling
					b_part_p_no_filter = (b_pcm_filter
											&& x265_base_data_cu_get_ipcm_flag_p2(cu_p, i_part_p_idx));
					b_part_q_no_filter = (b_pcm_filter
											&& x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu_q,
																					i_part_q_idx));

					// check if each of pus is lossless coded
					b_part_p_no_filter = b_part_p_no_filter
											|| (x265_base_data_cu_is_lossless_coded(h,
																					cu_p,
																					i_part_p_idx));
					b_part_q_no_filter = b_part_q_no_filter
											|| (x265_base_data_cu_is_lossless_coded(h,
																					(x265_base_data_cu_t*)cu_q,																					i_part_q_idx));
				}

				if (i_d < i_beta)
				{
					b_filter_p = (i_dp < i_side_threshold);
					b_filter_q = (i_dq < i_side_threshold);

					b_sw = x265_loop_filter_x_use_strong_filtering(loop_filter, i_offset, 2*i_d0, i_beta, i_tc, p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+0))
        						&& x265_loop_filter_x_use_strong_filtering(loop_filter, i_offset, 2*i_d3, i_beta, i_tc, p_tmp_src+i_src_step*(i_idx*i_pixels_in_part+i_blk_idx*4+3));

					for ( loop = 0; loop < X265_DEBLOCK_SMALLEST_BLOCK / 2; ++ loop )
					{
						x265_loop_filter_x_pixel_filter_luma(h,
															loop_filter,
															p_tmp_src + i_src_step * (i_idx * i_pixels_in_part + i_blk_idx * 4 + loop),
															i_offset,
															i_tc,
															b_sw,
															b_part_p_no_filter,
															b_part_q_no_filter,
															i_thr_cut,
															b_filter_p,
															b_filter_q);
					}
				}
			}
		}
	}
}


void x265_loop_filter_x_edge_filter_luma_pu_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_base_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge )
{
	int32_t loop = 0;
	int32_t i_stride = 0;
	int32_t i_qp = 0;
	int32_t i_qp_p = 0;
	int32_t i_qp_q = 0;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_bs_abs_idx = 0 ;
	uint32_t i_bs = 0;
	int32_t i_offset = 0;
	int32_t i_src_step;
	int32_t b_pcm_filter = 0;
	int32_t b_part_p_no_filter = 0;
	int32_t b_part_q_no_filter = 0;
	uint32_t i_part_p_idx = 0;
	uint32_t  i_part_q_idx = 0;
	int32_t i_beta_offset_div2 = 0;
	int32_t i_tc_offset_div2 = 0;
	int32_t i_bit_depth_scale = 0;
	int32_t i_index_tc = 0;
	int32_t i_index_b = 0;
	int32_t i_tc = 0;
	int32_t i_beta = 0;
	int32_t i_side_threshold = 0;
	int32_t i_thr_cut = 0;
	uint32_t i_blocks_in_part = 0;
	uint32_t i_blk_idx = 0;
	int32_t i_dp0 = 0;
	int32_t i_dq0 = 0;
	int32_t i_dp3 = 0;
	int32_t i_dq3 = 0;
	int32_t i_d0 = 0;
	int32_t i_d3 = 0;
	int32_t i_dp = 0;
	int32_t i_dq = 0;
	int32_t i_d =  0;
	int32_t b_filter_p = 0;
	int32_t b_filter_q = 0;
	int32_t b_sw = 0;
	pixel *p_src = NULL;
	pixel *p_tmp_src = NULL;
	x265_frame_t *frame = NULL;
	x265_base_data_cu_t *cu_p = NULL;
	x265_base_data_cu_t *cu_q = NULL;

	frame = h->fdec;
	p_src = x265_frame_get_luma_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr(cu),
										i_abs_zorder_idx );
	p_tmp_src = p_src;
	i_stride = frame->i_stride[0];
	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth ;
	b_pcm_filter = (h->sps[0].b_use_pcm && h->sps[0].b_pcm_filter_disable_flag)? 1 : 0;
	cu_p = cu;
	cu_q = cu;
	i_beta_offset_div2 = h->slice->i_deblocking_filter_beta_offset_div2 ;
	i_tc_offset_div2 = h->slice->i_deblocking_filter_tc_offset_div2 ;
	i_offset = 1;
	i_src_step = i_stride;
	p_tmp_src += i_edge * i_pixels_in_part;


	i_bs_abs_idx = x265_loop_filter_x_calc_bs_idx_ver(h,
														loop_filter,
														cu,
														i_abs_zorder_idx,
														i_edge,
														0);
	i_bs = loop_filter->bs[X265_EDGE_VER][i_bs_abs_idx];
	if ( i_bs )
	{
		i_qp_q = x265_base_data_cu_get_qp_p2(cu, i_bs_abs_idx );
		i_part_q_idx = i_bs_abs_idx;
		// derive neighboring pu index
		cu_p = x265_base_data_cu_get_pu_left(h,
											cu_q,
											&i_part_p_idx,
											i_part_q_idx,
											!h->slice->b_lf_cross_slice_boundary_flag,
											!loop_filter->b_lf_cross_tile_boundary);

		i_qp_p = x265_base_data_cu_get_qp_p2(cu_p, i_part_p_idx);
		i_qp = (i_qp_p + i_qp_q + 1) >> 1;
		i_bit_depth_scale = 1 << (h->param.sps.i_bit_depth_y-8);

		i_index_tc = x265_clip3_int32((int32_t)(i_qp + X265_DEFAULT_INTRA_TC_OFFSET*(i_bs - 1) + (i_tc_offset_div2 << 1)),
										0, X265_MAX_QP + X265_DEFAULT_INTRA_TC_OFFSET);
		i_index_b = x265_clip3_int32(i_qp + (i_beta_offset_div2 << 1), 0, X265_MAX_QP);
		i_tc =  tc_table_8x8[i_index_tc]*i_bit_depth_scale;
		i_beta = beta_table_8x8[i_index_b]*i_bit_depth_scale;
		i_side_threshold = (i_beta+(i_beta>>1))>>3;
		i_thr_cut = i_tc*10;
		i_blocks_in_part = i_pixels_in_part / 4 ? i_pixels_in_part / 4 : 1;
		for ( i_blk_idx = 0; i_blk_idx<i_blocks_in_part; i_blk_idx ++)
		{
			i_dp0 = x265_loop_filter_x_calc_dp(loop_filter,
												p_tmp_src+i_src_step*(i_blk_idx*4+0),
												i_offset);
			i_dq0 = x265_loop_filter_x_calc_dq(loop_filter,
												p_tmp_src+i_src_step*(i_blk_idx*4+0),
												i_offset);
			i_dp3 = x265_loop_filter_x_calc_dp( loop_filter,
												p_tmp_src+i_src_step*(i_blk_idx*4+3),
												i_offset);
			i_dq3 = x265_loop_filter_x_calc_dq( loop_filter,
												p_tmp_src+i_src_step*(i_blk_idx*4+3),
												i_offset);
			i_d0 = i_dp0 + i_dq0;
			i_d3 = i_dp3 + i_dq3;

			i_dp = i_dp0 + i_dp3;
			i_dq = i_dq0 + i_dq3;
			i_d =  i_d0 + i_d3;

			if (b_pcm_filter || h->pps[0].b_transquant_bypass_enable_flag)
			{
				// check if each of pus is i_pcm with lf disabling
				b_part_p_no_filter = (b_pcm_filter
										&& x265_base_data_cu_get_ipcm_flag_p2(cu_p, i_part_p_idx));
				b_part_q_no_filter = (b_pcm_filter
										&& x265_base_data_cu_get_ipcm_flag_p2(cu_q,
																				i_part_q_idx));

				// check if each of pus is lossless coded
				b_part_p_no_filter = b_part_p_no_filter
										|| (x265_base_data_cu_is_lossless_coded(h,
																				cu_p,
																				i_part_p_idx));
				b_part_q_no_filter = b_part_q_no_filter
										|| (x265_base_data_cu_is_lossless_coded(h,
																				cu_q,
																				i_part_q_idx));
			}

			if (i_d < i_beta)
			{
				b_filter_p = (i_dp < i_side_threshold);
				b_filter_q = (i_dq < i_side_threshold);

				b_sw = x265_loop_filter_x_use_strong_filtering(loop_filter, i_offset, 2*i_d0, i_beta, i_tc, p_tmp_src+i_src_step*(i_blk_idx*4+0))
    						&& x265_loop_filter_x_use_strong_filtering(loop_filter, i_offset, 2*i_d3, i_beta, i_tc, p_tmp_src+i_src_step*(i_blk_idx*4+3));

				for ( loop = 0; loop < X265_DEBLOCK_SMALLEST_BLOCK / 2; ++ loop )
				{
					x265_loop_filter_x_pixel_filter_luma(h,
														loop_filter,
														p_tmp_src + i_src_step * (i_blk_idx * 4 + loop),
														i_offset,
														i_tc,
														b_sw,
														b_part_p_no_filter,
														b_part_q_no_filter,
														i_thr_cut,
														b_filter_p,
														b_filter_q);
				}
			}
		}
	}
}


void x265_loop_filter_x_edge_filter_luma_pu_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_base_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge )
{
	int32_t loop = 0;
	int32_t i_stride = 0;
	int32_t i_qp = 0;
	int32_t i_qp_p = 0;
	int32_t i_qp_q = 0;
	uint32_t i_pixels_in_part = 0;
	uint32_t i_bs_abs_idx = 0 ;
	uint32_t i_bs = 0;
	int32_t i_offset = 0;
	int32_t i_src_step;
	int32_t b_pcm_filter = 0;
	int32_t b_part_p_no_filter = 0;
	int32_t b_part_q_no_filter = 0;
	uint32_t i_part_p_idx = 0;
	uint32_t i_part_q_idx = 0;
	int32_t i_beta_offset_div2 = 0;
	int32_t i_tc_offset_div2 = 0;
	int32_t i_bit_depth_scale = 0;
	int32_t i_index_tc = 0;
	int32_t i_index_b = 0;
	int32_t i_tc = 0;
	int32_t i_beta = 0;
	int32_t i_side_threshold = 0;
	int32_t i_thr_cut = 0;
	uint32_t i_blocks_in_part = 0;
	uint32_t i_blk_idx = 0;
	int32_t i_dp0 = 0;
	int32_t i_dq0 = 0;
	int32_t i_dp3 = 0;
	int32_t i_dq3 = 0;
	int32_t i_d0 = 0;
	int32_t i_d3 = 0;
	int32_t i_dp = 0;
	int32_t i_dq = 0;
	int32_t i_d =  0;
	int32_t b_filter_p = 0;
	int32_t b_filter_q = 0;
	int32_t b_sw = 0;
	pixel *p_src = NULL;
	pixel *p_tmp_src = NULL;
	x265_frame_t *frame = NULL;
	x265_base_data_cu_t *cu_p = NULL;
	x265_base_data_cu_t *cu_q = NULL;

	frame = h->fdec;
	p_src = x265_frame_get_luma_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr(cu),
										i_abs_zorder_idx );
	p_tmp_src = p_src;
	i_stride = frame->i_stride[0];
	i_pixels_in_part = h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth ;
	b_pcm_filter = (h->sps[0].b_use_pcm && h->sps[0].b_pcm_filter_disable_flag)? 1 : 0;
	cu_p = cu;
	cu_q = cu;
	i_beta_offset_div2 = h->slice->i_deblocking_filter_beta_offset_div2 ;
	i_tc_offset_div2 = h->slice->i_deblocking_filter_tc_offset_div2 ;
	i_offset = i_stride;
	i_src_step = 1;
	p_tmp_src += i_edge*i_pixels_in_part*i_stride;

	i_bs_abs_idx = x265_loop_filter_x_calc_bs_idx_hor(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_edge,
													0);
	i_bs = loop_filter->bs[X265_EDGE_HOR][i_bs_abs_idx];
	if ( i_bs )
	{
		i_qp_q = x265_base_data_cu_get_qp_p2(cu, i_bs_abs_idx );
		i_part_q_idx = i_bs_abs_idx;
		// derive neighboring pu index
		cu_p = x265_base_data_cu_get_pu_above(h,
											cu_q,
											&i_part_p_idx,
											i_part_q_idx,
											!h->slice->b_lf_cross_slice_boundary_flag,
											0,
											!loop_filter->b_lf_cross_tile_boundary);
		i_qp_p = x265_base_data_cu_get_qp_p2(cu_p, i_part_p_idx);
		i_qp = (i_qp_p + i_qp_q + 1) >> 1;
		i_bit_depth_scale = 1 << (h->param.sps.i_bit_depth_y-8);

		i_index_tc = x265_clip3_int32((int32_t)(i_qp + X265_DEFAULT_INTRA_TC_OFFSET*(i_bs - 1) + (i_tc_offset_div2 << 1)),
										0, X265_MAX_QP + X265_DEFAULT_INTRA_TC_OFFSET);
		i_index_b = x265_clip3_int32(i_qp + (i_beta_offset_div2 << 1), 0, X265_MAX_QP);
		i_tc =  tc_table_8x8[i_index_tc]*i_bit_depth_scale;
		i_beta = beta_table_8x8[i_index_b]*i_bit_depth_scale;
		i_side_threshold = (i_beta+(i_beta>>1))>>3;
		i_thr_cut = i_tc*10;
		i_blocks_in_part = i_pixels_in_part / 4 ? i_pixels_in_part / 4 : 1;
		for ( i_blk_idx = 0; i_blk_idx<i_blocks_in_part; i_blk_idx ++)
		{
			i_dp0 = x265_loop_filter_x_calc_dp(loop_filter,
												p_tmp_src+i_src_step*(i_blk_idx*4+0),
												i_offset);
			i_dq0 = x265_loop_filter_x_calc_dq(loop_filter,
												p_tmp_src+i_src_step*(i_blk_idx*4+0),
												i_offset);
			i_dp3 = x265_loop_filter_x_calc_dp( loop_filter,
												p_tmp_src+i_src_step*(i_blk_idx*4+3),
												i_offset);
			i_dq3 = x265_loop_filter_x_calc_dq( loop_filter,
												p_tmp_src+i_src_step*(i_blk_idx*4+3),
												i_offset);
			i_d0 = i_dp0 + i_dq0;
			i_d3 = i_dp3 + i_dq3;

			i_dp = i_dp0 + i_dp3;
			i_dq = i_dq0 + i_dq3;
			i_d =  i_d0 + i_d3;

			if (b_pcm_filter || h->pps[0].b_transquant_bypass_enable_flag)
			{
				// check if each of pus is i_pcm with lf disabling
				b_part_p_no_filter = (b_pcm_filter
										&& x265_base_data_cu_get_ipcm_flag_p2(cu_p, i_part_p_idx));
				b_part_q_no_filter = (b_pcm_filter
										&& x265_base_data_cu_get_ipcm_flag_p2(cu_q,
																				i_part_q_idx));

				// check if each of pus is lossless coded
				b_part_p_no_filter = b_part_p_no_filter
										|| (x265_base_data_cu_is_lossless_coded(h,
																				cu_p,
																				i_part_p_idx));
				b_part_q_no_filter = b_part_q_no_filter
										|| (x265_base_data_cu_is_lossless_coded(h,
																				cu_q,
																				i_part_q_idx));
			}

			if (i_d < i_beta)
			{
				b_filter_p = (i_dp < i_side_threshold);
				b_filter_q = (i_dq < i_side_threshold);

				b_sw = x265_loop_filter_x_use_strong_filtering(loop_filter, i_offset, 2*i_d0, i_beta, i_tc, p_tmp_src+i_src_step*(i_blk_idx*4+0))
    						&& x265_loop_filter_x_use_strong_filtering(loop_filter, i_offset, 2*i_d3, i_beta, i_tc, p_tmp_src+i_src_step*(i_blk_idx*4+3));

				for ( loop = 0; loop < X265_DEBLOCK_SMALLEST_BLOCK / 2; ++ loop )
				{
					x265_loop_filter_x_pixel_filter_luma(h,
														loop_filter,
														p_tmp_src + i_src_step * (i_blk_idx * 4 + loop),
														i_offset,
														i_tc,
														b_sw,
														b_part_p_no_filter,
														b_part_q_no_filter,
														i_thr_cut,
														b_filter_p,
														b_filter_q);
				}
			}
		}
	}
}

void x265_loop_filter_x_edge_filter_chroma_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge )
{
	int32_t i_stride = 0;
	int32_t i_qp = 0;
	int32_t i_qp_p = 0;
	int32_t i_qp_q = 0;
	uint32_t i_pixels_in_part_chroma = 0;
	int32_t i_offset = 0;
	int32_t i_src_step;
	uint32_t i_lcu_width_in_base_units = 0;
	int32_t b_pcm_filter = 0;
	int32_t b_part_p_no_filter = 0;
	int32_t b_part_q_no_filter = 0;
	uint32_t i_part_p_idx = 0;
	uint32_t i_part_q_idx = 0;
	int32_t i_tc_offset_div2 = 0;
	uint32_t i_edge_num_in_lcu_vert = 0;
	uint32_t  i_num_parts = 0;
	uint32_t i_bs_abs_idx = 0 ;
	uint32_t i_bs = 0;
	uint32_t i_idx = 0;
	uint32_t i_chroma_idx = 0;
	int32_t i_chroma_qp_offset = 0;
	int32_t i_bit_depth_scale = 0;
	int32_t i_index_tc = 0;
	int32_t i_tc = 0;
	uint32_t i_step = 0;
	pixel *p_src_cb = NULL;
	pixel *p_src_cr = NULL;
	pixel* p_tmp_src_cb = NULL;
	pixel* p_tmp_src_cr = NULL;
	pixel* p_tmp_src_chroma = NULL;
	x265_frame_t *frame = NULL;
	x265_base_data_cu_t *cu_p = NULL;
	x265_data_cu_t *cu_q = NULL;

	frame = h->fdec;
	i_stride = frame->i_stride[1];
	p_src_cb = x265_frame_get_cb_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										i_abs_zorder_idx );
	p_src_cr = x265_frame_get_cr_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										i_abs_zorder_idx );
	i_pixels_in_part_chroma = h->cu.pic.i_max_cu_width >> (h->cu.pic.i_total_depth + 1);
	i_lcu_width_in_base_units = h->cu.pic.i_num_part_in_width ;
	b_pcm_filter = (h->sps[0].b_use_pcm && h->sps[0].b_pcm_filter_disable_flag)? 1 : 0;
	cu_q = cu;
	i_tc_offset_div2 = h->slice->i_deblocking_filter_tc_offset_div2 ;

	// vertical position
	i_edge_num_in_lcu_vert = h->scan.zscan_to_raster[i_abs_zorder_idx] % i_lcu_width_in_base_units + i_edge;

	if ((i_pixels_in_part_chroma < X265_DEBLOCK_SMALLEST_BLOCK)
			&& (i_edge_num_in_lcu_vert % (X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part_chroma)))
	{
		return;
	}
	i_num_parts = h->cu.pic.i_num_part_in_width ;
	p_tmp_src_cb = p_src_cb;
	p_tmp_src_cr = p_src_cr;
	i_offset = 1;
	i_src_step  = i_stride;
	p_tmp_src_cb += i_edge*i_pixels_in_part_chroma;
	p_tmp_src_cr += i_edge*i_pixels_in_part_chroma;

	for ( i_idx = 0; i_idx < i_num_parts; i_idx++ )
	{
		i_bs = 0;
		i_bs_abs_idx = x265_loop_filter_x_calc_bs_idx_ver(h,
														loop_filter,
														(x265_base_data_cu_t*)cu,
														i_abs_zorder_idx,
														i_edge,
														i_idx);
		i_bs = loop_filter->bs[X265_EDGE_VER][i_bs_abs_idx];
		if ( i_bs > 1)
		{
			i_qp_q = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, i_bs_abs_idx );
			i_part_q_idx = i_bs_abs_idx;
			// derive neighboring pu index
			cu_p = x265_base_data_cu_get_pu_left(h,
												(x265_base_data_cu_t*)cu_q,
												&i_part_p_idx,
												i_part_q_idx,
												!h->slice->b_lf_cross_slice_boundary_flag,
												!loop_filter->b_lf_cross_tile_boundary);

			i_qp_p = x265_base_data_cu_get_qp_p2(cu_p, i_part_p_idx);

			if (b_pcm_filter || h->pps[0].b_transquant_bypass_enable_flag)
			{
				// check if each of pus is i_pcm with lf disabling
				b_part_p_no_filter = (b_pcm_filter
										&& x265_base_data_cu_get_ipcm_flag_p2(cu_p, i_part_p_idx));
				b_part_q_no_filter = (b_pcm_filter
										&& x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu_q,
																				i_part_q_idx));

				// check if each of pus is lossless coded
				b_part_p_no_filter = b_part_p_no_filter
										|| (x265_base_data_cu_is_lossless_coded(h,
																				cu_p,
																				i_part_p_idx));
				b_part_q_no_filter = b_part_q_no_filter
										|| (x265_base_data_cu_is_lossless_coded(h,
																				(x265_base_data_cu_t*)cu_q,
																				i_part_q_idx));
			}
			for ( i_chroma_idx = 0; i_chroma_idx < 2; i_chroma_idx++ )
			{
				i_chroma_qp_offset = (i_chroma_idx == 0) ? h->pps[0].i_chroma_cb_qp_offset
														: h->pps[0].i_chroma_cr_qp_offset ;
				p_tmp_src_chroma = (i_chroma_idx == 0) ? p_tmp_src_cb : p_tmp_src_cr;
				i_qp = x265_qp_uv( ((i_qp_p + i_qp_q + 1) >> 1) + i_chroma_qp_offset );
				i_bit_depth_scale = 1 << (h->param.sps.i_bit_depth_c-8);
				i_index_tc = x265_clip3_int32(i_qp + X265_DEFAULT_INTRA_TC_OFFSET*(i_bs - 1) + (i_tc_offset_div2 << 1),
												0, X265_MAX_QP+X265_DEFAULT_INTRA_TC_OFFSET);
				i_tc =  tc_table_8x8[i_index_tc]*i_bit_depth_scale;
				for ( i_step = 0; i_step < i_pixels_in_part_chroma; i_step++ )
				{
					x265_loop_filter_x_pixel_filter_chroma(h,
															loop_filter,
															p_tmp_src_chroma + i_src_step*(i_step+i_idx*i_pixels_in_part_chroma),
															i_offset,
															i_tc,
															b_part_p_no_filter,
															b_part_q_no_filter);
				}
			}
		}
	}
}


void x265_loop_filter_x_edge_filter_chroma_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge )
{
	int32_t i_stride = 0;
	int32_t i_qp = 0;
	int32_t i_qp_p = 0;
	int32_t i_qp_q = 0;
	uint32_t i_pixels_in_part_chroma = 0;
	int32_t i_offset = 0;
	int32_t i_src_step;
	uint32_t i_lcu_width_in_base_units = 0;
	int32_t b_pcm_filter = 0;
	int32_t b_part_p_no_filter = 0;
	int32_t b_part_q_no_filter = 0;
	uint32_t i_part_p_idx = 0;
	uint32_t i_part_q_idx = 0;
	int32_t i_tc_offset_div2 = 0;
	uint32_t i_edge_num_in_lcu_hor = 0;
	uint32_t  i_num_parts = 0;
	uint32_t i_bs_abs_idx = 0 ;
	uint32_t i_bs = 0;
	uint32_t i_idx = 0;
	uint32_t i_chroma_idx = 0;
	int32_t i_chroma_qp_offset = 0;
	int32_t i_bit_depth_scale = 0;
	int32_t i_index_tc = 0;
	int32_t i_tc = 0;
	uint32_t i_step = 0;
	pixel *p_src_cb = NULL;
	pixel *p_src_cr = NULL;
	pixel* p_tmp_src_cb = NULL;
	pixel* p_tmp_src_cr = NULL;
	pixel* p_tmp_src_chroma = NULL;
	x265_frame_t *frame = NULL;
	x265_base_data_cu_t *cu_p = NULL;
	x265_data_cu_t *cu_q = NULL;

	frame = h->fdec;
	i_stride = frame->i_stride[1];
	p_src_cb = x265_frame_get_cb_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										i_abs_zorder_idx );
	p_src_cr = x265_frame_get_cr_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
										i_abs_zorder_idx );
	i_pixels_in_part_chroma = h->cu.pic.i_max_cu_width >> (h->cu.pic.i_total_depth + 1);
	i_lcu_width_in_base_units = h->cu.pic.i_num_part_in_width ;
	b_pcm_filter = (h->sps[0].b_use_pcm && h->sps[0].b_pcm_filter_disable_flag)? 1 : 0;
	cu_q = cu;
	i_tc_offset_div2 = h->slice->i_deblocking_filter_tc_offset_div2 ;

	// vertical position
	i_edge_num_in_lcu_hor = h->scan.zscan_to_raster[i_abs_zorder_idx] / i_lcu_width_in_base_units + i_edge;

	if ((i_pixels_in_part_chroma < X265_DEBLOCK_SMALLEST_BLOCK)
			&& (i_edge_num_in_lcu_hor % (X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part_chroma)))
	{
		return;
	}
	i_num_parts = h->cu.pic.i_num_part_in_width ;
	p_tmp_src_cb = p_src_cb;
	p_tmp_src_cr = p_src_cr;
	i_offset = i_stride;
	i_src_step = 1;
	p_tmp_src_cb += i_edge*i_pixels_in_part_chroma;
	p_tmp_src_cr += i_edge*i_pixels_in_part_chroma;

	for ( i_idx = 0; i_idx < i_num_parts; i_idx++ )
	{
		i_bs = 0;
		i_bs_abs_idx = x265_loop_filter_x_calc_bs_idx_hor(h,
														loop_filter,
														(x265_base_data_cu_t*)cu,
														i_abs_zorder_idx,
														i_edge,
														i_idx);
		i_bs = loop_filter->bs[X265_EDGE_HOR][i_bs_abs_idx];
		if ( i_bs > 1)
		{
			i_qp_q = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, i_bs_abs_idx );
			i_part_q_idx = i_bs_abs_idx;
			// derive neighboring pu index
			cu_p = x265_base_data_cu_get_pu_above(h,
												(x265_base_data_cu_t*)cu_q,
												&i_part_p_idx,
												i_part_q_idx,
												!h->slice->b_lf_cross_slice_boundary_flag,
												0,
												!loop_filter->b_lf_cross_tile_boundary);

			i_qp_p = x265_base_data_cu_get_qp_p2(cu_p, i_part_p_idx);

			if (b_pcm_filter || h->pps[0].b_transquant_bypass_enable_flag)
			{
				// check if each of pus is i_pcm with lf disabling
				b_part_p_no_filter = (b_pcm_filter
										&& x265_base_data_cu_get_ipcm_flag_p2(cu_p, i_part_p_idx));
				b_part_q_no_filter = (b_pcm_filter
										&& x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu_q,
																				i_part_q_idx));

				// check if each of pus is lossless coded
				b_part_p_no_filter = b_part_p_no_filter
										|| (x265_base_data_cu_is_lossless_coded(h,
																				cu_p,
																				i_part_p_idx));
				b_part_q_no_filter = b_part_q_no_filter
										|| (x265_base_data_cu_is_lossless_coded(h,
																				(x265_base_data_cu_t*)cu_q,
																				i_part_q_idx));
			}
			for ( i_chroma_idx = 0; i_chroma_idx < 2; i_chroma_idx++ )
			{
				i_chroma_qp_offset = (i_chroma_idx == 0) ? h->pps[0].i_chroma_cb_qp_offset
														: h->pps[0].i_chroma_cr_qp_offset ;
				p_tmp_src_chroma = (i_chroma_idx == 0) ? p_tmp_src_cb : p_tmp_src_cr;
				i_qp = x265_qp_uv( ((i_qp_p + i_qp_q + 1) >> 1) + i_chroma_qp_offset );
				i_bit_depth_scale = 1 << (h->param.sps.i_bit_depth_c-8);
				i_index_tc = x265_clip3_int32(i_qp + X265_DEFAULT_INTRA_TC_OFFSET*(i_bs - 1) + (i_tc_offset_div2 << 1),
												0, X265_MAX_QP+X265_DEFAULT_INTRA_TC_OFFSET);
				i_tc =  tc_table_8x8[i_index_tc]*i_bit_depth_scale;
				for ( i_step = 0; i_step < i_pixels_in_part_chroma; i_step++ )
				{
					x265_loop_filter_x_pixel_filter_chroma(h,
															loop_filter,
															p_tmp_src_chroma + i_src_step*(i_step+i_idx*i_pixels_in_part_chroma),
															i_offset,
															i_tc,
															b_part_p_no_filter,
															b_part_q_no_filter);
				}
			}
		}
	}
}


void x265_loop_filter_x_edge_filter_chroma_pu_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_base_data_cu_t* cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge )
{
	int32_t i_stride = 0;
	int32_t i_qp = 0;
	int32_t i_qp_p = 0;
	int32_t i_qp_q = 0;
	uint32_t i_pixels_in_part_chroma = 0;
	int32_t i_offset = 0;
	int32_t i_src_step;
	uint32_t i_lcu_width_in_base_units = 0;
	int32_t b_pcm_filter = 0;
	int32_t b_part_p_no_filter = 0;
	int32_t b_part_q_no_filter = 0;
	uint32_t i_part_p_idx = 0;
	uint32_t i_part_q_idx = 0;
	int32_t i_tc_offset_div2 = 0;
	uint32_t i_edge_num_in_lcu_vert = 0;
	uint32_t i_bs_abs_idx = 0 ;
	uint32_t i_bs = 0;
	uint32_t i_chroma_idx = 0;
	int32_t i_chroma_qp_offset = 0;
	int32_t i_bit_depth_scale = 0;
	int32_t i_index_tc = 0;
	int32_t i_tc = 0;
	uint32_t i_step = 0;
	pixel *p_src_cb = NULL;
	pixel *p_src_cr = NULL;
	pixel* p_tmp_src_cb = NULL;
	pixel* p_tmp_src_cr = NULL;
	pixel* p_tmp_src_chroma = NULL;
	x265_frame_t *frame = NULL;
	x265_base_data_cu_t *cu_p = NULL;
	x265_base_data_cu_t *cu_q = NULL;

	frame = h->fdec;
	i_stride = frame->i_stride[1];
	p_src_cb = x265_frame_get_cb_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr(cu),
										i_abs_zorder_idx );
	p_src_cr = x265_frame_get_cr_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr(cu),
										i_abs_zorder_idx );
	i_pixels_in_part_chroma = h->cu.pic.i_max_cu_width >> (h->cu.pic.i_total_depth + 1);
	i_lcu_width_in_base_units = h->cu.pic.i_num_part_in_width ;
	b_pcm_filter = (h->sps[0].b_use_pcm && h->sps[0].b_pcm_filter_disable_flag)? 1 : 0;
	cu_q = cu;
	i_tc_offset_div2 = h->slice->i_deblocking_filter_tc_offset_div2 ;

	// vertical position
	i_edge_num_in_lcu_vert = h->scan.zscan_to_raster[i_abs_zorder_idx] % i_lcu_width_in_base_units + i_edge;

	if ((i_pixels_in_part_chroma < X265_DEBLOCK_SMALLEST_BLOCK)
			&& (i_edge_num_in_lcu_vert % (X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part_chroma)))
	{
		return;
	}
	p_tmp_src_cb = p_src_cb;
	p_tmp_src_cr = p_src_cr;
	i_offset = 1;
	i_src_step  = i_stride;
	p_tmp_src_cb += i_edge*i_pixels_in_part_chroma;
	p_tmp_src_cr += i_edge*i_pixels_in_part_chroma;

	i_bs = 0;
	i_bs_abs_idx = x265_loop_filter_x_calc_bs_idx_ver(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_edge,
													0);
	i_bs = loop_filter->bs[X265_EDGE_VER][i_bs_abs_idx];
	if ( i_bs > 1)
	{
		i_qp_q = x265_base_data_cu_get_qp_p2(cu, i_bs_abs_idx );
		i_part_q_idx = i_bs_abs_idx;
		// derive neighboring pu index
		cu_p = x265_base_data_cu_get_pu_left(h,
											cu_q,
											&i_part_p_idx,
											i_part_q_idx,
											!h->slice->b_lf_cross_slice_boundary_flag,
											!loop_filter->b_lf_cross_tile_boundary);

		i_qp_p = x265_base_data_cu_get_qp_p2(cu_p, i_part_p_idx);

		if (b_pcm_filter || h->pps[0].b_transquant_bypass_enable_flag)
		{
			// check if each of pus is i_pcm with lf disabling
			b_part_p_no_filter = (b_pcm_filter
									&& x265_base_data_cu_get_ipcm_flag_p2(cu_p, i_part_p_idx));
			b_part_q_no_filter = (b_pcm_filter
									&& x265_base_data_cu_get_ipcm_flag_p2(cu_q,
																			i_part_q_idx));

			// check if each of pus is lossless coded
			b_part_p_no_filter = b_part_p_no_filter
									|| (x265_base_data_cu_is_lossless_coded(h,
																			cu_p,
																			i_part_p_idx));
			b_part_q_no_filter = b_part_q_no_filter
									|| (x265_base_data_cu_is_lossless_coded(h,
																			cu_q,
																			i_part_q_idx));
		}
		for ( i_chroma_idx = 0; i_chroma_idx < 2; i_chroma_idx++ )
		{
			i_chroma_qp_offset = (i_chroma_idx == 0) ? h->pps[0].i_chroma_cb_qp_offset
													: h->pps[0].i_chroma_cr_qp_offset ;
			p_tmp_src_chroma = (i_chroma_idx == 0) ? p_tmp_src_cb : p_tmp_src_cr;
			i_qp = x265_qp_uv( ((i_qp_p + i_qp_q + 1) >> 1) + i_chroma_qp_offset );
			i_bit_depth_scale = 1 << (h->param.sps.i_bit_depth_c-8);
			i_index_tc = x265_clip3_int32(i_qp + X265_DEFAULT_INTRA_TC_OFFSET*(i_bs - 1) + (i_tc_offset_div2 << 1),
											0, X265_MAX_QP+X265_DEFAULT_INTRA_TC_OFFSET);
			i_tc =  tc_table_8x8[i_index_tc]*i_bit_depth_scale;
			for ( i_step = 0; i_step < i_pixels_in_part_chroma; i_step++ )
			{
				x265_loop_filter_x_pixel_filter_chroma(h,
														loop_filter,
														p_tmp_src_chroma + i_src_step*i_step,
														i_offset,
														i_tc,
														b_part_p_no_filter,
														b_part_q_no_filter);
			}
		}
	}
}

void x265_loop_filter_x_edge_filter_chroma_pu_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_base_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge )
{
	int32_t i_stride = 0;
	int32_t i_qp = 0;
	int32_t i_qp_p = 0;
	int32_t i_qp_q = 0;
	uint32_t i_pixels_in_part_chroma = 0;
	int32_t i_offset = 0;
	int32_t i_src_step;
	uint32_t i_lcu_width_in_base_units = 0;
	int32_t b_pcm_filter = 0;
	int32_t b_part_p_no_filter = 0;
	int32_t b_part_q_no_filter = 0;
	uint32_t i_part_p_idx = 0;
	uint32_t i_part_q_idx = 0;
	int32_t i_tc_offset_div2 = 0;
	uint32_t i_edge_num_in_lcu_hor = 0;
	uint32_t i_bs_abs_idx = 0 ;
	uint32_t i_bs = 0;
	uint32_t i_chroma_idx = 0;
	int32_t i_chroma_qp_offset = 0;
	int32_t i_bit_depth_scale = 0;
	int32_t i_index_tc = 0;
	int32_t i_tc = 0;
	uint32_t i_step = 0;
	pixel *p_src_cb = NULL;
	pixel *p_src_cr = NULL;
	pixel* p_tmp_src_cb = NULL;
	pixel* p_tmp_src_cr = NULL;
	pixel* p_tmp_src_chroma = NULL;
	x265_frame_t *frame = NULL;
	x265_base_data_cu_t *cu_p = NULL;
	x265_base_data_cu_t *cu_q = NULL;

	frame = h->fdec;
	i_stride = frame->i_stride[1];
	p_src_cb = x265_frame_get_cb_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr(cu),
										i_abs_zorder_idx );
	p_src_cr = x265_frame_get_cr_addr_p4(h,
										frame,
										x265_base_data_cu_get_addr(cu),
										i_abs_zorder_idx );
	i_pixels_in_part_chroma = h->cu.pic.i_max_cu_width >> (h->cu.pic.i_total_depth + 1);
	i_lcu_width_in_base_units = h->cu.pic.i_num_part_in_width ;
	b_pcm_filter = (h->sps[0].b_use_pcm && h->sps[0].b_pcm_filter_disable_flag)? 1 : 0;
	cu_q = cu;
	i_tc_offset_div2 = h->slice->i_deblocking_filter_tc_offset_div2 ;

	// vertical position
	i_edge_num_in_lcu_hor = h->scan.zscan_to_raster[i_abs_zorder_idx] / i_lcu_width_in_base_units + i_edge;

	if ((i_pixels_in_part_chroma < X265_DEBLOCK_SMALLEST_BLOCK)
			&& (i_edge_num_in_lcu_hor % (X265_DEBLOCK_SMALLEST_BLOCK / i_pixels_in_part_chroma)))
	{
		return;
	}
	p_tmp_src_cb = p_src_cb;
	p_tmp_src_cr = p_src_cr;
	i_offset = i_stride;
	i_src_step = 1;
	p_tmp_src_cb += i_edge*i_pixels_in_part_chroma;
	p_tmp_src_cr += i_edge*i_pixels_in_part_chroma;

	i_bs = 0;
	i_bs_abs_idx = x265_loop_filter_x_calc_bs_idx_hor(h,
													loop_filter,
													cu,
													i_abs_zorder_idx,
													i_edge,
													0);
	i_bs = loop_filter->bs[X265_EDGE_HOR][i_bs_abs_idx];
	if ( i_bs > 1)
	{
		i_qp_q = x265_base_data_cu_get_qp_p2(cu, i_bs_abs_idx );
		i_part_q_idx = i_bs_abs_idx;
		// derive neighboring pu index
		cu_p = x265_base_data_cu_get_pu_above(h,
											cu_q,
											&i_part_p_idx,
											i_part_q_idx,
											!h->slice->b_lf_cross_slice_boundary_flag,
											0,
											!loop_filter->b_lf_cross_tile_boundary);

		i_qp_p = x265_base_data_cu_get_qp_p2(cu_p, i_part_p_idx);

		if (b_pcm_filter || h->pps[0].b_transquant_bypass_enable_flag)
		{
			// check if each of pus is i_pcm with lf disabling
			b_part_p_no_filter = (b_pcm_filter
									&& x265_base_data_cu_get_ipcm_flag_p2(cu_p, i_part_p_idx));
			b_part_q_no_filter = (b_pcm_filter
									&& x265_base_data_cu_get_ipcm_flag_p2(cu_q,
																			i_part_q_idx));

			// check if each of pus is lossless coded
			b_part_p_no_filter = b_part_p_no_filter
									|| (x265_base_data_cu_is_lossless_coded(h,
																			cu_p,
																			i_part_p_idx));
			b_part_q_no_filter = b_part_q_no_filter
									|| (x265_base_data_cu_is_lossless_coded(h,
																			cu_q,
																			i_part_q_idx));
		}
		for ( i_chroma_idx = 0; i_chroma_idx < 2; i_chroma_idx++ )
		{
			i_chroma_qp_offset = (i_chroma_idx == 0) ? h->pps[0].i_chroma_cb_qp_offset
													: h->pps[0].i_chroma_cr_qp_offset ;
			p_tmp_src_chroma = (i_chroma_idx == 0) ? p_tmp_src_cb : p_tmp_src_cr;
			i_qp = x265_qp_uv( ((i_qp_p + i_qp_q + 1) >> 1) + i_chroma_qp_offset );
			i_bit_depth_scale = 1 << (h->param.sps.i_bit_depth_c-8);
			i_index_tc = x265_clip3_int32(i_qp + X265_DEFAULT_INTRA_TC_OFFSET*(i_bs - 1) + (i_tc_offset_div2 << 1),
											0, X265_MAX_QP+X265_DEFAULT_INTRA_TC_OFFSET);
			i_tc =  tc_table_8x8[i_index_tc]*i_bit_depth_scale;
			for ( i_step = 0; i_step < i_pixels_in_part_chroma; i_step++ )
			{
				x265_loop_filter_x_pixel_filter_chroma(h,
														loop_filter,
														p_tmp_src_chroma + i_src_step*i_step,
														i_offset,
														i_tc,
														b_part_p_no_filter,
														b_part_q_no_filter);
			}
		}
	}
}



void x265_loop_filter_x_pixel_filter_luma(x265_t *h,
										x265_loop_filter_t *loop_filter,
										pixel* p_src,
										int32_t i_offset,
										int32_t i_tc,
										int32_t b_sw,
										int32_t b_part_p_no_filter,
										int32_t b_part_q_no_filter,
										int32_t i_thr_cut,
										int32_t b_filter_second_p,
										int32_t b_filter_second_q)
{
	int32_t i_delta;
	pixel i_m4 = 0;
	pixel i_m3 = 0;
	pixel i_m5 = 0;
	pixel i_m2 = 0;
	pixel i_m6 = 0;
	pixel i_m1 = 0;
	pixel i_m7 = 0;
	pixel i_m0 = 0;
	int32_t i_tc2 = 0;
	int32_t i_delta1 = 0;
    int32_t i_delta2 = 0;

	i_m4 = p_src[0];
	i_m3 = p_src[-i_offset];
	i_m5 = p_src[ i_offset];
	i_m2 = p_src[-i_offset*2];
	i_m6 = p_src[ i_offset*2];
	i_m1 = p_src[-i_offset*3];
	i_m7 = p_src[ i_offset*3];
	i_m0 = p_src[-i_offset*4];

	if (b_sw)
	{
		p_src[-i_offset] = x265_clip3_pixel(((i_m1 + 2*i_m2 + 2*i_m3 + 2*i_m4 + i_m5 + 4) >> 3), i_m3-2*i_tc, i_m3+2*i_tc);
		p_src[0] = x265_clip3_pixel(((i_m2 + 2*i_m3 + 2*i_m4 + 2*i_m5 + i_m6 + 4) >> 3), i_m4-2*i_tc, i_m4+2*i_tc);
		p_src[-i_offset*2] = x265_clip3_pixel(((i_m1 + i_m2 + i_m3 + i_m4 + 2)>>2), i_m2-2*i_tc, i_m2+2*i_tc);
		p_src[ i_offset] = x265_clip3_pixel(((i_m3 + i_m4 + i_m5 + i_m6 + 2)>>2), i_m5-2*i_tc, i_m5+2*i_tc);
		p_src[-i_offset*3] = x265_clip3_pixel(((2*i_m0 + 3*i_m1 + i_m2 + i_m3 + i_m4 + 4 )>>3), i_m1-2*i_tc, i_m1+2*i_tc);
		p_src[ i_offset*2] = x265_clip3_pixel(((i_m3 + i_m4 + i_m5 + 3*i_m6 + 2*i_m7 +4 )>>3), i_m6-2*i_tc, i_m6+2*i_tc);
	}
	else
	{
		/* weak filter */
		i_delta = (9*(i_m4-i_m3) -3*(i_m5-i_m2) + 8)>>4 ;

		if ( abs(i_delta) < i_thr_cut )
		{
			i_delta = x265_clip3_int32(i_delta, -i_tc, i_tc );
			p_src[-i_offset] = x265_clip_y(h, (i_m3+i_delta));
			p_src[0] = x265_clip_y(h, (i_m4-i_delta));

			i_tc2 = i_tc>>1;
			if(b_filter_second_p)
			{
				i_delta1 = x265_clip3_int32(((((i_m1+i_m3+1)>>1)- i_m2+i_delta)>>1), -i_tc2, i_tc2);
				p_src[-i_offset*2] = x265_clip_y(h, (i_m2+i_delta1));
			}
			if(b_filter_second_q)
			{
				i_delta2 = x265_clip3_int32(((((i_m6+i_m4+1)>>1)- i_m5-i_delta)>>1), -i_tc2, i_tc2);
				p_src[ i_offset] = x265_clip_y(h, (i_m5+i_delta2));
			}
		}
	}

	if(b_part_p_no_filter)
	{
		p_src[-i_offset] = i_m3;
		p_src[-i_offset*2] = i_m2;
		p_src[-i_offset*3] = i_m1;
	}
	if(b_part_q_no_filter)
	{
		p_src[0] = i_m4;
		p_src[ i_offset] = i_m5;
		p_src[ i_offset*2] = i_m6;
	}
}

void x265_loop_filter_x_pixel_filter_chroma(x265_t *h,
											x265_loop_filter_t *loop_filter,
											pixel* p_src,
											int32_t i_offset,
											int32_t i_tc,
											int32_t b_part_p_no_filter,
											int32_t b_part_q_no_filter)
{
	int32_t i_delta = 0;
	pixel i_m4 = 0;
	pixel i_m3 = 0;
	pixel i_m5 = 0;
	pixel i_m2 = 0;

	i_m4 = p_src[0];
	i_m3 = p_src[-i_offset];
	i_m5 = p_src[ i_offset];
	i_m2 = p_src[-i_offset*2];

	i_delta = x265_clip3_int32((((( i_m4 - i_m3 ) << 2) + i_m2 - i_m5 + 4) >> 3), -i_tc, i_tc );
	p_src[-i_offset] = x265_clip_c(h, i_m3 + i_delta);
	p_src[0] = x265_clip_c(h, i_m4 - i_delta);

	if(b_part_p_no_filter)
	{
		p_src[-i_offset] = i_m3;
	}
	if(b_part_q_no_filter)
	{
		p_src[0] = i_m4;
	}
}

int32_t x265_loop_filter_x_use_strong_filtering(x265_loop_filter_t *loop_filter,
												int32_t i_offset,
												int32_t i_d,
												int32_t i_beta,
												int32_t i_tc, pixel* p_src)
{
	pixel i_m4 = 0;
	pixel i_m3 = 0;
	pixel i_m7 = 0;
	pixel i_m0 = 0;
	int32_t i_strong = 0;

	i_m4 = p_src[0];
	i_m3 = p_src[-i_offset];
	i_m7 = p_src[ i_offset * 3];
	i_m0 = p_src[-i_offset * 4];
	i_strong = abs(i_m0-i_m3) + abs(i_m7-i_m4);

	return ((i_strong < (i_beta>>3)) && (i_d<(i_beta>>2))
			&& (abs(i_m3 - i_m4) < ((i_tc * 5 + 1) >> 1)));
}

int32_t x265_loop_filter_x_calc_dp(x265_loop_filter_t *loop_filter,
								pixel* p_src,
								int32_t i_offset)
{
	return abs( p_src[-i_offset*3] - 2*p_src[-i_offset*2] + p_src[-i_offset] ) ;
}

int32_t x265_loop_filter_x_calc_dq(x265_loop_filter_t *loop_filter,
								pixel* p_src,
								int32_t i_offset)
{
	return abs( p_src[0] - 2*p_src[i_offset] + p_src[i_offset*2] );
}




