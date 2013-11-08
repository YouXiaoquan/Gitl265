


#ifndef X265_LOOP_FILTER_H
#define X265_LOOP_FILTER_H



#define X265_DEBLOCK_SMALLEST_BLOCK  8


typedef struct
{
	uint32_t i_num_partitions;
	uint8_t* bs[2];			///< Bs for [Ver/Hor][Y/U/V][Blk_Idx]
	int32_t* edge_filter[2];
	x265_lf_cu_param_t lf_cu_param;		///< status structure

	int32_t b_lf_cross_tile_boundary;

} x265_loop_filter_t ;

void x265_loop_filter_x_deblock_cu_ver(x265_t *h,
										x265_loop_filter_t *loop_filter,
										x265_data_cu_t *cu,
										uint32_t i_abs_zorder_idx,
										uint32_t i_depth ) ;
void x265_loop_filter_x_deblock_cu_hor(x265_t *h,
										x265_loop_filter_t *loop_filter,
										x265_data_cu_t *cu,
										uint32_t i_abs_zorder_idx,
										uint32_t i_depth ) ;
void x265_loop_filter_x_get_deblock_cu_parameter_ver(x265_t *h,
													x265_loop_filter_t *loop_filter,
													x265_data_cu_t *cu,
													uint32_t i_abs_zorder_idx,
													uint32_t i_depth ) ;
void x265_loop_filter_x_get_deblock_cu_parameter_hor(x265_t *h,
													x265_loop_filter_t *loop_filter,
													x265_data_cu_t *cu,
													uint32_t i_abs_zorder_idx,
													uint32_t i_depth ) ;
void x265_loop_filter_x_deblock_last_row_or_column_ver(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_base_data_cu_t *cu ) ;
void x265_loop_filter_x_deblock_last_row_or_column_hor(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_base_data_cu_t *cu ) ;
void x265_loop_filter_x_deblock_front_rows_or_columns_ver(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_data_cu_t *cu ) ;
void x265_loop_filter_x_deblock_front_rows_or_columns_hor(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_data_cu_t *cu) ;



void x265_loop_filter_x_set_loop_filter_param_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx ) ;
void x265_loop_filter_x_set_loop_filter_param_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx ) ;
void x265_loop_filter_x_set_edge_filter_tu_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_tu_part_idx,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth ) ;
void x265_loop_filter_x_set_edge_filter_tu_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_tu_part_idx,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth ) ;
void x265_loop_filter_x_set_edge_filter_pu_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx ) ;
void x265_loop_filter_x_set_edge_filter_pu_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx ) ;
void x265_loop_filter_x_get_boundary_strength_single_ver(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_data_cu_t *cu,
														uint32_t i_abs_part_idx ) ;
void x265_loop_filter_x_get_boundary_strength_single_hor(x265_t *h,
														x265_loop_filter_t *loop_filter,
														x265_data_cu_t *cu,
														uint32_t i_abs_part_idx ) ;
uint32_t x265_loop_filter_x_calc_bs_idx_ver(x265_t *h,
											x265_loop_filter_t *loop_filter,
											x265_base_data_cu_t *cu,
											uint32_t i_abs_zorder_idx,
											int32_t i_edge_idx,
											int32_t i_base_unit_idx ) ;
uint32_t x265_loop_filter_x_calc_bs_idx_hor(x265_t *h,
											x265_loop_filter_t *loop_filter,
											x265_base_data_cu_t *cu,
											uint32_t i_abs_zorder_idx,
											int32_t i_edge_idx,
											int32_t i_base_unit_idx ) ;
void x265_loop_filter_x_set_edge_filter_multiple_ver(x265_t *h,
													x265_loop_filter_t *loop_filter,
													x265_data_cu_t *cu,
													uint32_t i_scan_idx,
													uint32_t i_depth,
													int32_t i_edge_idx,
													int32_t b_value,
													uint32_t i_width_in_base_units,
													uint32_t i_height_in_base_units ) ;
void x265_loop_filter_x_set_edge_filter_multiple_hor(x265_t *h,
													x265_loop_filter_t *loop_filter,
													x265_data_cu_t *cu,
													uint32_t i_scan_idx,
													uint32_t i_depth,
													int32_t i_edge_idx,
													int32_t b_value,
													uint32_t i_width_in_base_units,
													uint32_t i_height_in_base_units ) ;

void x265_loop_filter_x_edge_filter_luma_ver(x265_t *h,
											x265_loop_filter_t *loop_filter,
											x265_data_cu_t *cu,
											uint32_t i_abs_zorder_idx,
											uint32_t i_depth,
											int32_t i_edge  ) ;
void x265_loop_filter_x_edge_filter_luma_hor(x265_t *h,
											x265_loop_filter_t *loop_filter,
											x265_data_cu_t *cu,
											uint32_t i_abs_zorder_idx,
											uint32_t i_depth,
											int32_t i_edge  ) ;
void x265_loop_filter_x_edge_filter_luma_pu_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_base_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge ) ;
void x265_loop_filter_x_edge_filter_luma_pu_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_base_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge ) ;
void x265_loop_filter_x_edge_filter_chroma_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge ) ;
void x265_loop_filter_x_edge_filter_chroma_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge ) ;
void x265_loop_filter_x_edge_filter_chroma_pu_ver(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_base_data_cu_t* cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge ) ;
void x265_loop_filter_x_edge_filter_chroma_pu_hor(x265_t *h,
												x265_loop_filter_t *loop_filter,
												x265_base_data_cu_t *cu,
												uint32_t i_abs_zorder_idx,
												uint32_t i_depth,
												int32_t i_edge ) ;


void x265_loop_filter_x_pixel_filter_luma(x265_t *h,
										x265_loop_filter_t *loop_filter,
										pixel* p_src,
										int32_t i_offset,
										int32_t i_tc,
										int32_t b_sw,
										int32_t b_part_pno_filter,
										int32_t b_part_qno_filter,
										int32_t i_thr_cut,
										int32_t b_filter_second_p,
										int32_t b_filter_second_q);
void x265_loop_filter_x_pixel_filter_chroma(x265_t *h,
											x265_loop_filter_t *loop_filter,
											pixel* p_src,
											int32_t i_offset,
											int32_t i_tc,
											int32_t b_part_pno_filter,
											int32_t b_part_qno_filter);
int32_t x265_loop_filter_x_use_strong_filtering(x265_loop_filter_t *loop_filter,
												int32_t i_offset,
												int32_t i_d,
												int32_t i_beta,
												int32_t i_tc, pixel* p_src);
int32_t x265_loop_filter_x_calc_dp(x265_loop_filter_t *loop_filter,
								pixel* p_src,
								int32_t i_offset);
int32_t x265_loop_filter_x_calc_dq(x265_loop_filter_t *loop_filter,
								pixel* p_src,
								int32_t i_offset);






x265_loop_filter_t *x265_loop_filter_new () ;
void x265_loop_filter_delete ( x265_loop_filter_t *loop_filter ) ;
int x265_loop_filter_init ( x265_loop_filter_t *loop_filter ) ;
void x265_loop_filter_deinit ( x265_loop_filter_t *loop_filter ) ;
int x265_loop_filter_create ( x265_loop_filter_t *loop_filter, uint32_t i_max_cu_depth ) ;
void x265_loop_filter_destroy ( x265_loop_filter_t *loop_filter ) ;


void x265_loop_filter_set_cfg( x265_loop_filter_t *loop_filter, int32_t b_lf_cross_tile_boundary ) ;

void x265_loop_filter_loop_filter_pic(x265_t *h,
									x265_loop_filter_t *loop_filter,
									x265_frame_t *frame ) ;
void x265_loop_filter_loop_filter_cu ( x265_t *h,
										x265_loop_filter_t *loop_filter,
										x265_data_cu_t *cu ) ;

#endif

