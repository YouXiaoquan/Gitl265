

#ifndef X265_DATA_CU_H
#define X265_DATA_CU_H



struct _x265_data_cu_t
{
	x265_base_data_cu_t base_data_cu ;

	x265_pattern_t* pattern;          ///< neighbour access class pointer

	// -------------------------------------------------------------------------------------------------------------------
	//CU description
	//-------------------------------------------------------------------------------------------------------------------

	uint32_t i_cu_pel_x;           ///< CU position in a pixel (X)
	uint32_t i_cu_pel_y;           ///< CU position in a pixel (Y)
	uint8_t* width;           ///< array of widths
	uint8_t* height;          ///< array of heights
	int32_t i_unit_size;           ///< size of a "minimum partition"

	// -------------------------------------------------------------------------------------------------------------------
	//CU data
	//-------------------------------------------------------------------------------------------------------------------
	int32_t b_being_encode ;

	int8_t* part_size;         ///< array of partition sizes
	uint8_t* transform_skip[3];///< array of transform skipping flags
	x265_coeff_t* tr_coeff_y ;         ///< transformed coefficient buffer (Y)
	x265_coeff_t* tr_coeff_cb ;        ///< transformed coefficient buffer (Cb)
	x265_coeff_t* tr_coeff_cr ;        ///< transformed coefficient buffer (Cr)
#if X265_ADAPTIVE_QP_SELECTION
	int32_t* arl_coeff_y;        ///< ARL coefficient buffer (Y)
	int32_t* arl_coeff_cb;       ///< ARL coefficient buffer (Cb)
	int32_t* arl_coeff_cr;       ///< ARL coefficient buffer (Cr)
	int32_t b_arl_coeff_is_aliased_allocation; ///< ARL coefficient buffer is an alias of the global buffer and must not be free()'d

	int32_t* glb_arl_coeff_y;     ///< ARL coefficient buffer (Y)
	int32_t* glb_arl_coeff_cb;    ///< ARL coefficient buffer (Cb)
	int32_t* glb_arl_coeff_cr;    ///< ARL coefficient buffer (Cr)
#endif

	pixel* ipcm_sample_y ;      ///< PCM sample buffer (Y)
	pixel* ipcm_sample_cb ;     ///< PCM sample buffer (Cb)
	pixel* ipcm_sample_cr ;     ///< PCM sample buffer (Cr)

	int32_t* slice_su_map;       ///< pointer of slice ID map
	//	std::vector<NDBFBlockInfo> m_vNDFBlock;

	// -------------------------------------------------------------------------------------------------------------------
	// neighbour access variables
	// -------------------------------------------------------------------------------------------------------------------

	x265_base_data_cu_t* cu_above_left;      ///< pointer of above-left CU
	x265_base_data_cu_t* cu_above_right;     ///< pointer of above-right CU
	x265_mv_field_t mv_field_a;          ///< motion vector of position A
	x265_mv_field_t mv_field_b;          ///< motion vector of position B
	x265_mv_field_t mv_field_c;          ///< motion vector of position C
	x265_mv_t mv_pred;            ///< motion vector predictor

	// -------------------------------------------------------------------------------------------------------------------
	// coding tool information
	// -------------------------------------------------------------------------------------------------------------------

	int32_t* merge_flag;        ///< array of merge flags
	uint8_t* merge_index;      ///< array of merge candidate indices
#if X265_AMP_MRG
	int32_t b_is_merge_amp;
#endif
	uint8_t* chroma_intra_dir;  ///< array of intra directions (chroma)
	int8_t* mvp_idx[2];       ///< array of motion vector predictor candidates
	int8_t* mvp_num[2];       ///< array of number of possible motion vectors predictors

	// -------------------------------------------------------------------------------------------------------------------
	// misc. variables
	// -------------------------------------------------------------------------------------------------------------------

	double f_total_cost;         ///< sum of partition RD costs
	uint32_t i_total_distortion;  ///< sum of partition distortion
	uint32_t i_total_bits;        ///< sum of partition bits
	uint32_t i_total_bins;       ///< sum of partition bins
	uint32_t* slice_segment_start_cu; ///< Start CU address of current slice
	int8_t i_coded_qp;

} ;

#if X265_ADAPTIVE_QP_SELECTION

typedef struct
{
	int32_t *glb_arl_coeff_y ; ;
	int32_t *glb_arl_coeff_cb ; ;
	int32_t *glb_arl_coeff_cr ; ;

} x265_data_cu_class_static_t ;

extern x265_data_cu_class_static_t *p_data_cu_class_static ;

int x265_data_cu_class_static_init () ;
void x265_data_cu_class_static_deinit () ;

#endif


int32_t x265_data_cu_x_add_mvp_cand( x265_t *h,
									x265_data_cu_t* cu,
									x265_amvp_info_t* p_amp_info,
									enum ref_pic_list_e i_ref_pic_list,
									int32_t i_ref_idx,
									uint32_t i_part_unit_idx,
									enum mvp_dir_e i_mvp_dir );
int32_t x265_data_cu_x_add_mvp_cand_order( x265_t *h,
											x265_data_cu_t* cu,
											x265_amvp_info_t* p_amp_info,
											enum ref_pic_list_e i_ref_pic_list,
											int32_t i_ref_idx,
											uint32_t i_part_unit_idx,
											enum mvp_dir_e i_mvp_dir );

void x265_data_cu_derive_right_bottom_idx ( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_part_idx,
											uint32_t* p_part_idx_rb );
int32_t x265_data_cu_x_get_col_mvp( x265_t *h,
									x265_data_cu_t* cu,
									enum ref_pic_list_e i_ref_pic_list,
									int32_t i_cu_addr,
									int32_t i_part_unit_idx,
									x265_mv_t* p_mv,
									int32_t* p_ref_idx );

/// compute required bits to encode mvd (used in amvp)
uint32_t x265_data_cu_x_get_mvd_bits( x265_data_cu_t* cu, x265_mv_t *mvd );
uint32_t x265_data_cu_x_get_component_bits ( x265_data_cu_t* cu, int32_t i_val );

/// compute scaling factor from poc difference
int32_t x265_data_cu_x_get_dist_scale_factor ( x265_data_cu_t* cu,
												int32_t i_curr_poc,
												int32_t i_curr_ref_poc,
												int32_t i_col_poc,
												int32_t i_col_ref_poc );

void x265_data_cu_x_derive_center_idx( x265_t* h,
										x265_data_cu_t* cu,
										uint32_t i_part_idx,
										uint32_t* p_part_idx_center );

x265_data_cu_t *x265_data_cu_new () ;
void x265_data_cu_delete ( x265_data_cu_t* cu ) ;
int x265_data_cu_init ( x265_data_cu_t* cu ) ;
void x265_data_cu_deinit ( x265_data_cu_t* cu ) ;

int32_t x265_data_cu_get_being_encode ( x265_data_cu_t *cu ) ;
void x265_data_cu_set_being_encode ( x265_data_cu_t *cu, int32_t b_being_encode ) ;


int x265_data_cu_create ( x265_data_cu_t *data_cu,
								uint32_t i_num_partition,
								int32_t i_width,
								int32_t i_height,
								int32_t i_unit_size
#if X265_ADAPTIVE_QP_SELECTION
								, int32_t b_global_rmarl_buffer
#endif
								) ;
void x265_data_cu_destroy ( x265_data_cu_t *data_cu ) ;

void x265_data_cu_init_cu ( x265_t *h, x265_data_cu_t *cu, uint32_t i_cu_addr ) ;
void x265_data_cu_init_est_data( x265_t *h,
								x265_data_cu_t* cu,
								uint32_t i_depth,
								int32_t i_qp ) ;
void x265_data_cu_init_sub_cu( x265_t *h,
								x265_data_cu_t* cu,
								x265_data_cu_t* cu_src,
								uint32_t i_part_unit_idx,
								uint32_t i_depth,
								int32_t i_qp ) ;
void x265_data_cu_set_outside_cu_part( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth ) ;

void x265_data_cu_copy_sub_cu( x265_t *h,
								x265_data_cu_t* cu,
								x265_data_cu_t* cu_src,
								uint32_t i_abs_part_idx,
								uint32_t i_depth ) ;
void x265_data_cu_copy_inter_pred_info_from ( x265_t *h,
											x265_data_cu_t* cu,
											x265_data_cu_t* cu_src,
											uint32_t i_abs_part_idx,
											enum ref_pic_list_e i_ref_pic_list ) ;
void x265_data_cu_copy_part_from ( x265_t *h,
								x265_data_cu_t* cu,
								x265_data_cu_t* cu_src,
								uint32_t i_part_unit_idx,
								uint32_t i_depth ) ;
void x265_data_cu_copy_to_pic_p3( x265_t *h, x265_data_cu_t* cu, uint8_t depth ) ;
void x265_data_cu_copy_to_pic_p5 ( x265_t *h,
								x265_data_cu_t* cu,
								uint8_t i_depth,
								uint32_t i_part_idx,
								uint32_t i_part_depth ) ;

uint32_t x265_data_cu_get_cu_pel_x( x265_data_cu_t *cu ) ;
uint32_t x265_data_cu_get_cu_pel_y( x265_data_cu_t *cu ) ;
x265_pattern_t *x265_data_cu_get_pattern( x265_data_cu_t *cu ) ;


int8_t *x265_data_cu_get_partition_size_p1 ( x265_data_cu_t *cu ) ;
enum part_size_e x265_data_cu_get_partition_size_p2 ( x265_data_cu_t *cu, uint32_t i_idx ) ;
void x265_data_cu_set_partition_size ( x265_data_cu_t *cu,
										uint32_t i_idx,
										enum part_size_e i_part_size ) ;
void x265_data_cu_set_part_size_sub_parts ( x265_t *h,
											x265_data_cu_t *cu,
											enum part_size_e i_part_size,
											uint32_t i_abs_part_idx,
											uint32_t i_depth ) ;

uint8_t *x265_data_cu_get_width_p1 ( x265_data_cu_t *cu ) ;
uint8_t x265_data_cu_get_width_p2 ( x265_data_cu_t *cu, uint32_t i_idx ) ;
void x265_data_cu_set_width ( x265_data_cu_t *cu, uint32_t i_idx, uint8_t i_width ) ;

uint8_t *x265_data_cu_get_height_p1 ( x265_data_cu_t *cu ) ;
uint8_t x265_data_cu_get_height_p2 ( x265_data_cu_t *cu, uint32_t i_idx ) ;
void x265_data_cu_set_height ( x265_data_cu_t *cu, uint32_t i_idx, uint8_t i_height ) ;

void x265_data_cu_set_size_sub_parts( x265_t *h,
									x265_data_cu_t* cu,
									uint32_t i_width,
									uint32_t i_height,
									uint32_t i_abs_part_idx,
									uint32_t i_depth );
void x265_data_cu_set_coded_qp ( x265_data_cu_t* cu, int8_t i_coded_qp ) ;
int8_t x265_data_cu_get_coded_qp ( x265_data_cu_t* cu ) ;

uint8_t *x265_data_cu_get_transform_skip_p2 ( x265_data_cu_t *cu,
											enum text_type_e i_text_type ) ;
uint8_t x265_data_cu_get_transform_skip_p3( x265_data_cu_t *cu,
											uint32_t i_idx,
											enum text_type_e i_text_type ) ;


void x265_data_cu_set_transform_skip_sub_parts_p6( x265_t *h,
												x265_data_cu_t* cu,
												uint32_t b_use_transform_skip,
												enum text_type_e i_text_type,
												uint32_t i_abs_part_idx,
												uint32_t i_depth);
void x265_data_cu_set_transform_skip_sub_parts_p7( x265_t *h,
												x265_data_cu_t* cu,
												uint32_t b_use_transform_skip_y,
												uint32_t b_use_transform_skip_u,
												uint32_t b_use_transform_skip_v,
												uint32_t i_abs_part_idx,
												uint32_t i_depth );

uint32_t x265_data_cu_get_quadtree_tu_log2_min_size_in_cu( x265_t *h,
															x265_data_cu_t *cu,
															uint32_t i_abs_part_idx ) ;

x265_coeff_t *x265_data_cu_get_coeff_y ( x265_data_cu_t *cu ) ;
x265_coeff_t *x265_data_cu_get_coeff_cb ( x265_data_cu_t *cu ) ;
x265_coeff_t *x265_data_cu_get_coeff_cr ( x265_data_cu_t *cu ) ;


#if X265_ADAPTIVE_QP_SELECTION

int32_t *x265_data_cu_get_arl_coeff_y ( x265_data_cu_t *cu ) ;
int32_t *x265_data_cu_get_arl_coeff_cb ( x265_data_cu_t *cu ) ;
int32_t *x265_data_cu_get_arl_coeff_cr ( x265_data_cu_t *cu ) ;

#endif

pixel *x265_data_cu_get_pcm_sample_y ( x265_data_cu_t *cu ) ;
pixel *x265_data_cu_get_pcm_sample_cb ( x265_data_cu_t *cu ) ;
pixel *x265_data_cu_get_pcm_sample_cr ( x265_data_cu_t *cu ) ;

void x265_data_cu_clear_cbf( x265_t* h,
							x265_data_cu_t* cu,
							uint32_t i_idx,
							enum text_type_e i_text_type,
							uint32_t i_num_parts );

void x265_data_cu_set_cbf_sub_parts_p7_1 ( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_cbf_y,
										uint32_t i_cbf_u,
										uint32_t i_cbf_v,
										uint32_t i_abs_part_idx,
										uint32_t i_depth );
void x265_data_cu_set_cbf_sub_parts_p6 ( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_cbf,
										enum text_type_e i_text_type,
										uint32_t i_abs_part_idx,
										uint32_t i_depth);
void x265_data_cu_set_cbf_sub_parts_p7_2 ( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_cbf,
										enum text_type_e i_text_type,
										uint32_t i_abs_part_idx,
										uint32_t i_part_idx,
										uint32_t i_depth);

void x265_data_cu_set_inter_dir_sub_parts ( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_dir,
											uint32_t i_abs_part_idx,
											uint32_t i_part_idx,
											uint32_t i_depth );


int32_t *x265_data_cu_get_merge_flag_p1 ( x265_data_cu_t *cu ) ;
int32_t x265_data_cu_get_merge_flag_p2 ( x265_data_cu_t *cu, uint32_t i_idx ) ;
void x265_data_cu_set_merge_flag ( x265_data_cu_t* cu,
									uint32_t i_idx,
									int32_t b_merge_flag ) ;
void x265_data_cu_set_merge_flag_sub_parts( x265_t *h,
											x265_data_cu_t* cu,
											int32_t b_merge_flag,
											uint32_t i_abs_part_idx,
											uint32_t i_part_idx,
											uint32_t i_depth ) ;


uint8_t *x265_data_cu_get_merge_index_p1 ( x265_data_cu_t *cu ) ;
uint8_t x265_data_cu_get_merge_index_p2 ( x265_data_cu_t *cu, uint32_t i_idx ) ;
void x265_data_cu_set_merge_index( x265_data_cu_t* cu,
									uint32_t i_idx,
									uint32_t i_merge_index ) ;
void x265_data_cu_set_merge_index_sub_parts ( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_merge_index,
											uint32_t i_abs_part_idx,
											uint32_t i_part_idx,
											uint32_t i_depth );
void x265_data_cu_set_sub_part_int8 ( x265_t *h,
									x265_data_cu_t* cu,
									int8_t i_parameter,
									int8_t *p_base_lcu,
									uint32_t i_cu_addr,
									uint32_t i_cu_depth,
									uint32_t i_pu_idx ) ;
void x265_data_cu_set_sub_part_uint8 ( x265_t *h,
									x265_data_cu_t* cu,
									uint8_t i_parameter,
									uint8_t *p_base_lcu,
									uint32_t i_cu_addr,
									uint32_t i_cu_depth,
									uint32_t i_pu_idx ) ;

void x265_data_cu_set_sub_part_int32 ( x265_t *h,
									x265_data_cu_t* cu,
									int32_t i_parameter,
									int32_t *p_base_lcu,
									uint32_t i_cu_addr,
									uint32_t i_cu_depth,
									uint32_t i_pu_idx ) ;


#if X265_AMP_MRG

void x265_data_cu_set_merge_amp (x265_data_cu_t* cu, int32_t b_is_merge_amp) ;
int32_t x265_data_cu_get_merge_amp (x265_data_cu_t* cu) ;

#endif


uint8_t *x265_data_cu_get_chroma_intra_dir_p1 ( x265_data_cu_t *cu ) ;
uint8_t x265_data_cu_get_chroma_intra_dir_p2 ( x265_data_cu_t *cu, uint32_t i_idx ) ;
void x265_data_cu_set_chroma_intra_dir ( x265_data_cu_t* cu,
										uint32_t i_idx,
										uint8_t i_chroma_intra_dir ) ;
void x265_data_cu_set_chrom_intra_dir_sub_parts ( x265_t *h,
												x265_data_cu_t* cu,
												uint32_t i_dir,
												uint32_t i_abs_part_idx,
												uint32_t i_depth ) ;

int32_t x265_data_cu_get_su_slice_id ( x265_data_cu_t* cu,
										uint32_t i_idx ) ;
int32_t *x265_data_cu_get_slice_su_map( x265_data_cu_t* cu ) ;
void x265_data_cu_set_slice_su_map( x265_data_cu_t* cu, int32_t *slice_su_map ) ;


void x265_data_cu_get_part_index_and_size ( x265_data_cu_t* cu,
											uint32_t i_part_idx,
											uint32_t* p_part_addr,
											int32_t* p_width,
											int32_t* p_height );
uint8_t x265_data_cu_get_num_part_inter( x265_data_cu_t* cu );



void x265_data_cu_fill_mvp_cand( x265_t* h,
								x265_data_cu_t* cu,
								uint32_t i_part_idx,
								uint32_t i_part_addr,
								enum ref_pic_list_e i_ref_pic_list,
								int32_t i_ref_idx,
								x265_amvp_info_t* p_amvp_info );
void x265_data_cu_get_part_position( x265_data_cu_t* cu,
									uint32_t i_part_idx,
									int32_t* p_x_p,
									int32_t* p_y_p,
									int32_t* p_psw,
									int32_t* p_psh) ;
void x265_data_cu_set_mvp_idx( x265_data_cu_t* cu,
								enum ref_pic_list_e i_ref_pic_list,
								uint32_t i_idx,
								int32_t i_mvp_idx ) ;

int8_t *x265_data_cu_get_mvp_idx_p2 (x265_data_cu_t *cu,
									enum ref_pic_list_e i_ref_pic_list ) ;
int32_t x265_data_cu_get_mvp_idx_p3 (x265_data_cu_t *cu,
									enum ref_pic_list_e i_ref_pic_list,
									uint32_t i_idx ) ;

void x265_data_cu_set_mvp_num( x265_data_cu_t* cu,
								enum ref_pic_list_e i_ref_pic_list,
								uint32_t i_idx,
								int32_t i_mvp_num ) ;
int8_t *x265_data_cu_get_mvp_num_p2 (x265_data_cu_t *cu,
									enum ref_pic_list_e i_ref_pic_list ) ;
int32_t x265_data_cu_get_mvp_num_p3 (x265_data_cu_t *cu,
									enum ref_pic_list_e i_ref_pic_list,
									uint32_t i_idx ) ;



void x265_data_cu_set_mvp_idx_sub_parts ( x265_t *h,
										x265_data_cu_t* cu,
										int32_t i_mvp_idx,
										enum ref_pic_list_e i_ref_pic_list,
										uint32_t i_abs_part_idx,
										uint32_t i_part_idx,
										uint32_t i_depth );
void x265_data_cu_set_mvp_num_sub_parts ( x265_t *h,
										x265_data_cu_t* cu,
										int32_t i_mvp_num,
										enum ref_pic_list_e i_ref_pic_list,
										uint32_t i_abs_part_idx,
										uint32_t i_part_idx,
										uint32_t i_depth );


void x265_data_cu_clip_mv (x265_t* h, x265_data_cu_t* cu, x265_mv_t* p_mv );
void x265_data_cu_get_mv_pred_left ( x265_data_cu_t* cu, x265_mv_t* p_mv_pred ) ;
void x265_data_cu_get_mv_pred_above ( x265_data_cu_t* cu, x265_mv_t* p_mv_pred ) ;
void x265_data_cu_get_mv_pred_above_right ( x265_data_cu_t* cu, x265_mv_t* p_mv_pred ) ;


x265_base_data_cu_t *x265_data_cu_get_cu_above_left( x265_data_cu_t *cu ) ;
x265_base_data_cu_t *x265_data_cu_get_cu_above_right( x265_data_cu_t *cu ) ;

x265_base_data_cu_t* x265_data_cu_get_pu_above_left ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t *p_al_part_unit_idx,
													uint32_t i_curr_part_unit_idx,
													int32_t b_enforce_slice_restriction );
x265_base_data_cu_t* x265_data_cu_get_pu_above_right ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t *p_ar_part_unit_idx,
													uint32_t i_curr_part_unit_idx,
													int32_t b_enforce_slice_restriction );
x265_base_data_cu_t* x265_data_cu_get_pu_below_left ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t *p_bl_part_unit_idx,
													uint32_t i_curr_part_unit_idx,
													int32_t b_enforce_slice_restriction );

x265_data_cu_t *x265_data_cu_get_qp_min_cu_left ( x265_t *h,
												x265_data_cu_t *cu,
												uint32_t *p_part_unit_idx,
												uint32_t i_curr_abs_idx_in_lcu ) ;
x265_data_cu_t *x265_data_cu_get_qp_min_cu_above ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t *p_part_unit_idx,
													uint32_t i_curr_abs_idx_in_lcu ) ;
int8_t x265_data_cu_get_ref_qp( x265_t *h, x265_data_cu_t *cu, uint32_t i_curr_abs_idx_in_lcu ) ;

x265_base_data_cu_t* x265_data_cu_get_pu_above_right_adi ( x265_t *h,
														x265_data_cu_t *cu,
														uint32_t *p_ar_part_unit_idx,
														uint32_t i_curr_part_unit_idx,
														uint32_t i_part_unit_offset,
														int32_t b_enforce_slice_restriction );
x265_base_data_cu_t* x265_data_cu_get_pu_below_left_adi ( x265_t *h,
														x265_data_cu_t *cu,
														uint32_t *p_bl_part_unit_idx,
														uint32_t i_curr_part_unit_idx,
														uint32_t i_part_unit_offset,
														int32_t b_enforce_slice_restriction );

void x265_data_cu_derive_left_right_top_idx( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_part_idx,
											uint32_t* p_part_idx_lt,
											uint32_t* p_part_idx_rt );
void x265_data_cu_derive_left_bottom_idx( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_part_idx,
										uint32_t* p_part_idx_lb );

void x265_data_cu_derive_left_right_top_idx_adi( x265_t *h,
												x265_data_cu_t* cu,
												uint32_t* p_part_idx_lt,
												uint32_t* p_part_idx_rt,
												uint32_t i_part_offset,
												uint32_t i_part_depth );
void x265_data_cu_derive_left_bottom_idx_adi( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t* p_part_idx_lb,
											uint32_t i_part_offset,
											uint32_t i_part_depth );

void x265_data_cu_get_inter_merge_candidates( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_abs_part_idx,
											uint32_t i_pu_idx,
											x265_mv_field_t* p_mv_field_neighbours,
											uint8_t* p_inter_dir_neighbours,
											int32_t* p_num_valid_merge_cand,
											int32_t i_mrg_cand_idx );
void x265_data_cu_derive_left_right_top_idx_general( x265_t *h,
													x265_data_cu_t* cu,
													uint32_t i_abs_part_idx,
													uint32_t i_part_idx,
													uint32_t* p_part_idx_lt,
													uint32_t* p_part_idx_rt );
void x265_data_cu_derive_left_bottom_idx_general( x265_t *h,
													x265_data_cu_t* cu,
													uint32_t i_abs_part_idx,
													uint32_t i_part_idx,
													uint32_t* p_part_idx_lb );

int32_t x265_data_cu_is_bipred_restriction( x265_data_cu_t* cu, uint32_t i_pu_idx );

uint32_t x265_data_cu_get_intra_size_idx ( x265_data_cu_t* cu, uint32_t i_abs_part_idx );
void x265_data_cu_get_allowed_chroma_dir( x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										uint32_t* mode_list ) ;
int32_t x265_data_cu_get_intra_dir_luma_predictor ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t i_abs_part_idx,
													int32_t* intra_dir_pred,
													int32_t* pi_mode ) ;

uint32_t x265_data_cu_get_ctx_split_flag( x265_t *h, x265_data_cu_t* cu,
										uint32_t i_abs_part_idx, uint32_t i_depth ) ;
uint32_t x265_data_cu_get_ctx_qt_cbf( x265_data_cu_t* cu,
									enum text_type_e i_text_type,
									uint32_t i_tr_depth ) ;
uint32_t x265_data_cu_get_ctx_skip_flag( x265_t *h,
										x265_data_cu_t* cu, uint32_t i_abs_part_idx ) ;
uint32_t x265_data_cu_get_ctx_inter_dir( x265_data_cu_t* cu, uint32_t i_abs_part_idx ) ;

uint32_t *x265_data_cu_get_slice_segment_start_cu_p1 ( x265_data_cu_t *cu ) ;
uint32_t x265_data_cu_get_slice_segment_start_cu_p2 ( x265_data_cu_t *cu, uint32_t i_idx ) ;



uint32_t x265_data_cu_get_total_bins( x265_data_cu_t *cu ) ;
double x265_data_cu_get_total_cost( x265_data_cu_t *cu ) ;
uint32_t x265_data_cu_get_total_distortion( x265_data_cu_t *cu ) ;
uint32_t x265_data_cu_get_total_bits( x265_data_cu_t *cu ) ;

uint32_t x265_data_cu_get_coef_scan_idx( x265_t *h,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										uint32_t i_width,
										int32_t b_is_luma,
										int32_t b_is_intra ) ;

void x265_data_cu_compress_mv ( x265_data_cu_t *cu ) ;

#endif

