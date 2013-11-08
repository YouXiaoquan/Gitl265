

#ifndef X265_BASE_DATA_CU_H
#define X265_BASE_DATA_CU_H



extern const uint8_t convert_txt_type_to_idx [4] ;

enum ndbf_block_border_tag_t
{
	SGU_L = 0,
	SGU_R,
	SGU_T,
	SGU_B,
	SGU_TL,
	SGU_TR,
	SGU_BL,
	SGU_BR,
	NUM_SGU_BORDER
} ;

struct _x265_ndbf_block_info_t
{
	int32_t i_tile_id ;
	int32_t i_slice_id ;
	uint32_t i_start_su ;
	uint32_t i_end_su ;
	uint32_t i_width_su ;
	uint32_t i_height_su ;
	uint32_t i_pos_x ;
	uint32_t i_pos_y ;
	uint32_t i_width ;
	uint32_t i_height ;
	int32_t is_border_available[NUM_SGU_BORDER] ;
	int32_t b_all_borders_available;
};

struct _x265_base_data_cu_t
{
	x265_base_data_cu_t* cu_above;				///< pointer of above CU
	x265_base_data_cu_t* cu_left;				///< pointer of left CU
	uint32_t* slice_start_cu;					///< Start CU address of current slice

	int8_t* pred_mode;         ///< array of prediction modes
	uint8_t* depth;           ///< array of depths
	int8_t* qp;               ///< array of QP values
	uint8_t* inter_dir;        ///< array of inter directions
	uint8_t* luma_intra_dir;    ///< array of intra directions (luma)

	int32_t* skip_flag;           ///< array of skip flags
	uint8_t* cbf[3];          ///< array of coded block flags (CBF)
	uint8_t* tr_idx;           ///< array of transform indices
	int32_t* ipcm_flag;         ///< array of intra_pcm flags
	int32_t* cu_transquant_bypass;   ///< array of cu_transquant_bypass flags

	x265_cu_mv_field_t cu_mv_field[2];     ///< array of motion vectors

	uint32_t i_cu_addr;           ///< CU address in a slice
	uint32_t i_abs_idx_in_lcu;      ///< absolute address in a CU. It's Z scan order
	uint32_t i_num_partition;     ///< total number of minimum partitions in a CU


} ;


x265_ndbf_block_info_t *x265_ndbf_block_info_new () ;
void x265_ndbf_block_info_delete ( x265_ndbf_block_info_t *ndbf_block_info ) ;
int x265_ndbf_block_info_init ( x265_ndbf_block_info_t *ndbf_block_info ) ;
void x265_ndbf_block_info_deinit ( x265_ndbf_block_info_t *ndbf_block_info ) ;

void x265_ndbf_block_info_copy_from ( x265_ndbf_block_info_t *ndbf_block_info,
										x265_ndbf_block_info_t *src_ndbf_block_info ) ;

x265_base_data_cu_t *x265_base_data_cu_new () ;
void x265_base_data_cu_delete ( x265_base_data_cu_t *base_data_cu ) ;
int x265_base_data_cu_init ( x265_base_data_cu_t *base_data_cu ) ;
void x265_base_data_cu_deinit ( x265_base_data_cu_t *base_data_cu ) ;
int x265_base_data_cu_create ( x265_base_data_cu_t *base_data_cu,
								uint32_t i_num_partition,
								int32_t i_width,
								int32_t i_height,
								int32_t i_unit_size
#if X265_ADAPTIVE_QP_SELECTION
								, int32_t b_global_rmarl_buffer
#endif
								) ;
void x265_base_data_cu_destroy ( x265_base_data_cu_t *base_data_cu ) ;
void x265_base_data_cu_load ( x265_base_data_cu_t *base_data_cu,
							x265_base_data_cu_t *base_data_cu_src ) ;


uint32_t x265_base_data_cu_get_total_num_part( x265_base_data_cu_t *base_data_cu ) ;
uint32_t x265_base_data_cu_get_addr( x265_base_data_cu_t *base_data_cu ) ;
uint32_t x265_base_data_cu_get_zorder_idx_in_cu( x265_base_data_cu_t *base_data_cu ) ;
uint32_t x265_base_data_cu_get_scu_addr( x265_t *h, x265_base_data_cu_t *base_data_cu ) ;

x265_base_data_cu_t *x265_base_data_cu_get_cu_left( x265_base_data_cu_t *base_data_cu ) ;
x265_base_data_cu_t *x265_base_data_cu_get_cu_above( x265_base_data_cu_t *base_data_cu ) ;





int8_t *x265_base_data_cu_get_qp_p1 ( x265_base_data_cu_t *base_data_cu ) ;
int8_t x265_base_data_cu_get_qp_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
int8_t *x265_base_data_cu_get_prediction_mode_p1 ( x265_base_data_cu_t *base_data_cu ) ;
enum pred_mode_e x265_base_data_cu_get_prediction_mode_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
uint8_t *x265_base_data_cu_get_depth_p1 ( x265_base_data_cu_t *base_data_cu ) ;
uint8_t x265_base_data_cu_get_depth_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
int8_t *x265_base_data_cu_get_prediction_mode_p1 ( x265_base_data_cu_t *base_data_cu ) ;
enum pred_mode_e x265_base_data_cu_get_prediction_mode_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
uint8_t *x265_base_data_cu_get_depth_p1 ( x265_base_data_cu_t *base_data_cu ) ;
uint8_t x265_base_data_cu_get_depth_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
uint8_t *x265_base_data_cu_get_luma_intra_dir_p1 ( x265_base_data_cu_t *base_data_cu ) ;
uint8_t x265_base_data_cu_get_luma_intra_dir_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
int32_t *x265_base_data_cu_get_skip_flag_p1 ( x265_base_data_cu_t *base_data_cu ) ;
int32_t x265_base_data_cu_get_skip_flag_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
uint8_t *x265_base_data_cu_get_cbf_p2 ( x265_base_data_cu_t *base_data_cu,
										enum text_type_e i_text_type ) ;
uint8_t x265_base_data_cu_get_cbf_p3( x265_base_data_cu_t *base_data_cu,
								uint32_t i_idx,
								enum text_type_e i_text_type ) ;
uint8_t x265_base_data_cu_get_cbf_p4 ( x265_base_data_cu_t *base_data_cu,
										uint32_t i_idx,
										enum text_type_e i_text_type,
										uint32_t i_tr_depth ) ;
uint8_t *x265_base_data_cu_get_transform_idx_p1 ( x265_base_data_cu_t *base_data_cu ) ;
uint8_t x265_base_data_cu_get_transform_idx_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
int32_t *x265_base_data_cu_get_ipcm_flag_p1 ( x265_base_data_cu_t *base_data_cu ) ;
int32_t x265_base_data_cu_get_ipcm_flag_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
int32_t *x265_base_data_cu_get_cu_transquant_bypass_p1 ( x265_base_data_cu_t *base_data_cu ) ;
int32_t x265_base_data_cu_get_cu_transquant_bypass_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
uint8_t *x265_base_data_cu_get_inter_dir_p1 ( x265_base_data_cu_t *base_data_cu ) ;
uint8_t x265_base_data_cu_get_inter_dir_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
uint32_t *x265_base_data_cu_get_slice_start_cu_p1 ( x265_base_data_cu_t *base_data_cu ) ;
uint32_t x265_base_data_cu_get_slice_start_cu_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;





int32_t x265_base_data_cu_get_last_valid_part_idx ( x265_base_data_cu_t *base_data_cu,
													int32_t i_abs_part_idx ) ;
int8_t x265_base_data_cu_get_last_coded_qp ( x265_t *h,
											x265_base_data_cu_t *base_data_cu,
											uint32_t i_abs_part_idx ) ;
x265_cu_mv_field_t *x265_base_data_cu_get_cu_mv_field( x265_base_data_cu_t *base_data_cu,
														enum ref_pic_list_e i_ref_pic_list ) ;
uint8_t x265_base_data_cu_get_qt_root_cbf ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx ) ;
int x265_base_data_cu_get_mv_field ( x265_base_data_cu_t *base_data_cu,
										x265_base_data_cu_t *base_data_cu_src,
										uint32_t i_abs_part_idx,
										enum ref_pic_list_e i_ref_pic_list,
										x265_mv_field_t *mv_field ) ;
x265_base_data_cu_t* x265_base_data_cu_get_pu_left( x265_t *h,
												x265_base_data_cu_t* base_data_cu,
												uint32_t* p_left_part_unit_idx,
												uint32_t i_curr_part_unit_idx,
												int32_t b_enforce_slice_restriction,
												int32_t b_enforce_tile_restriction ) ;
x265_base_data_cu_t* x265_base_data_cu_get_pu_above( x265_t *h,
													x265_base_data_cu_t* base_data_cu,
													uint32_t* p_above_part_unit_idx,
													uint32_t i_curr_part_unit_idx,
													int32_t b_enforce_slice_restriction,
													int32_t b_planar_at_lcu_boundary ,
													int32_t b_enforce_tile_restriction ) ;




int32_t x265_base_data_cu_is_intra ( x265_base_data_cu_t *base_data_cu,
									uint32_t i_part_idx ) ;
int32_t x265_base_data_cu_is_skipped ( x265_base_data_cu_t *base_data_cu, uint32_t i_part_idx ) ;
int32_t x265_base_data_cu_is_diff_mer ( x265_t *h,
										x265_base_data_cu_t *base_data_cu,
										int32_t i_x_n,
										int32_t i_y_n,
										int32_t i_x_p,
										int32_t i_y_p ) ;
int32_t x265_base_data_cu_is_lossless_coded ( x265_t *h,
											x265_base_data_cu_t *base_data_cu,
											uint32_t i_abs_part_idx ) ;
int32_t x265_base_data_cu_is_first_abs_zorder_idx_in_depth ( x265_t *h,
															x265_base_data_cu_t *base_data_cu,
															uint32_t i_abs_part_idx,
															uint32_t i_depth ) ;

int32_t x265_base_data_cu_has_equal_motion ( x265_base_data_cu_t *base_data_cu,
											uint32_t i_abs_part_idx,
											x265_base_data_cu_t *base_data_cu_cand,
											uint32_t i_cand_abs_part_idx ) ;


uint32_t x265_base_data_cu_get_cu_pel_x_by_cu_addr ( x265_t *h, uint32_t i_cu_addr ) ;
uint32_t x265_base_data_cu_get_cu_pel_y_by_cu_addr ( x265_t *h, uint32_t i_cu_addr ) ;


void x265_base_data_set_cbf ( x265_base_data_cu_t *base_data_cu,
								uint32_t i_idx,
								enum text_type_e i_text_type,
								uint8_t i_cbf ) ;
void x265_base_data_cu_set_depth ( x265_base_data_cu_t *base_data_cu,
									uint32_t i_idx,
									uint8_t i_depth ) ;
void x265_base_data_cu_set_prediction_mode ( x265_base_data_cu_t *base_data_cu,
											uint32_t i_idx,
											enum pred_mode_e i_pred_mode) ;
void x265_base_data_cu_set_skip_flag ( x265_base_data_cu_t *base_data_cu,
										uint32_t i_idx,
										int32_t b_skip) ;
void x265_base_data_cu_set_qp ( x265_base_data_cu_t *base_data_cu,
								uint32_t i_idx,
								int8_t i_value ) ;
void x265_base_data_cu_set_luma_intra_dir ( x265_base_data_cu_t *base_data_cu,
											uint32_t i_idx,
											uint8_t i_dir ) ;
void x265_base_data_cu_set_inter_dir ( x265_base_data_cu_t *base_data_cu,
										uint32_t i_idx,
										uint8_t i_dir ) ;
void x265_base_data_cu_set_ipcm_flag( x265_base_data_cu_t* base_data_cu,
										uint32_t i_idx,
										int32_t b_ipcm_flag ) ;
void x265_base_data_cu_set_depth_sub_parts ( x265_t *h,
											x265_base_data_cu_t *base_data_cu,
											uint32_t i_depth,
											uint32_t i_abs_part_idx ) ;
void x265_base_data_cu_set_pred_mode_sub_parts ( x265_t *h,
												x265_base_data_cu_t *base_data_cu,
												enum pred_mode_e i_pred_mode,
												uint32_t i_abs_part_idx,
												uint32_t i_depth );
void x265_base_data_cu_set_cu_transquant_bypass_sub_parts ( x265_t *h,
															x265_base_data_cu_t *base_data_cu,
															int32_t b_flag,
															uint32_t i_abs_part_idx,
															uint32_t i_depth );
void x265_base_data_cu_set_skip_flag_sub_parts ( x265_t *h,
												x265_base_data_cu_t *base_data_cu,
												int32_t b_skip,
												uint32_t i_abs_part_idx,
												uint32_t i_depth ) ;
void x265_base_data_cu_set_qp_sub_parts( x265_t *h,
									x265_base_data_cu_t *base_data_cu,
									int32_t i_qp,
									uint32_t i_abs_part_idx,
									uint32_t i_depth ) ;

void x265_base_data_cu_set_qp_sub_cus( x265_t *h,
										x265_base_data_cu_t* cu,
										int32_t i_qp,
										x265_base_data_cu_t* base_data_cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth,
										int32_t* p_found_non_zero_cbf ) ;
void x265_base_data_cu_set_luma_intra_dir_sub_parts ( x265_t *h,
														x265_base_data_cu_t *base_data_cu,
														uint32_t i_dir,
														uint32_t i_abs_part_idx,
														uint32_t i_depth ) ;
void x265_base_data_cu_set_tr_idx_sub_parts ( x265_t *h,
											x265_base_data_cu_t *base_data_cu,
											uint32_t i_tr_idx,
											uint32_t i_abs_part_idx,
											uint32_t i_depth ) ;
void x265_base_data_cu_set_ipcm_flag_sub_parts ( x265_t *h,
												x265_base_data_cu_t* base_data_cu,
												int32_t b_pcm_flag,
												uint32_t i_abs_part_idx,
												uint32_t i_depth) ;





int32_t x265_raster_address_is_equal_col( int32_t i_addr_a, int32_t i_addr_b, int32_t i_num_units_per_row ) ;
int32_t x265_raster_address_is_equal_row( int32_t i_addr_a, int32_t i_addr_b, int32_t i_num_units_per_row ) ;
int32_t x265_raster_address_is_equal_row_or_col( int32_t i_addr_a, int32_t i_addr_b, int32_t i_num_units_per_row ) ;
int32_t x265_raster_address_is_zero_col( int32_t i_addr, int32_t i_num_units_per_row ) ;
int32_t x265_raster_address_is_zero_row( int32_t i_addr, int32_t i_num_units_per_row ) ;
int32_t x265_raster_address_less_than_col( int32_t i_addr,
											int32_t i_val,
											int32_t i_num_units_per_row ) ;
int32_t x265_raster_address_less_than_row( int32_t i_addr,
											int32_t i_val,
											int32_t i_num_units_per_row ) ;



#endif


