


#ifndef X265_ENCODER_SEARCH_H
#define X265_ENCODER_SEARCH_H


#if X265_FASTME_SMOOTHER_MV
#define X265_FIRST_SEARCH_STOP     1
#else
#define X265_FIRST_SEARCH_STOP     0
#endif

#define X265_TZ_SEARCH_CONFIGURATION                                                                                 \
const int32_t  i_raster                  = 5;  /* TZ soll von aussen ?ergeben werden */                            \
const int32_t b_test_other_predicted_mv    = 0;                                                                      \
const int32_t b_test_zero_vector          = 1;                                                                      \
const int32_t b_test_zero_vector_start     = 0;                                                                      \
const int32_t b_test_zero_vector_stop      = 0;                                                                      \
const int32_t b_first_search_diamond      = 1;  /* 1 = xTZ8PointDiamondSearch   0 = xTZ8PointSquareSearch */        \
const int32_t b_first_search_stop         = X265_FIRST_SEARCH_STOP;                                                        \
const uint32_t i_first_search_rounds      = 3;  /* first search stop X rounds after best match (must be >=1) */     \
const int32_t b_enable_raster_search      = 1;                                                                      \
const int32_t b_always_raster_search      = 0;  /* ===== 1: BETTER but factor 2 slower ===== */                     \
const int32_t b_raster_refinement_enable  = 0;  /* enable either raster refinement or star refinement */            \
const int32_t b_raster_refinement_diamond = 0;  /* 1 = xTZ8PointDiamondSearch   0 = xTZ8PointSquareSearch */        \
const int32_t b_star_refinement_enable    = 1;  /* enable either star refinement or raster refinement */            \
const int32_t b_star_refinement_diamond   = 1;  /* 1 = xTZ8PointDiamondSearch   0 = xTZ8PointSquareSearch */        \
const int32_t b_star_refinement_stop      = 0;                                                                      \
const uint32_t i_star_refinement_rounds   = 2;  /* star refinement stop X rounds after best match (must be >=1) */  \


typedef struct
{
	pixel *ref_y;
	int32_t i_y_stride;
	int32_t i_best_x;
	int32_t i_best_y;
	uint32_t i_best_round;
	uint32_t i_best_distance;
	uint32_t i_best_sad;
	uint8_t i_point_nr;
} x265_tz_search_struct_t;

typedef struct
{
	pixel *ref_y ;
	int32_t i_y_stride ;
	int32_t i_best_x ;
	int32_t i_best_y ;
	uint32_t i_best_round ;
	uint32_t i_best_distance ;
	uint32_t i_best_sad ;
	uint8_t i_point_nr ;
} x265_int_tz_search_struct_t ;

typedef struct
{
	x265_prediction_t prediction ;
	int32_t b_has_init ;
	x265_coeff_t** ppc_qt_temp_coeff_y;
	x265_coeff_t** ppc_qt_temp_coeff_cb;
	x265_coeff_t** ppc_qt_temp_coeff_cr;
	x265_coeff_t* pc_qt_temp_coeff_y;
	x265_coeff_t* pc_qt_temp_coeff_cb;
	x265_coeff_t* pc_qt_temp_coeff_cr;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t** ppc_qt_temp_arl_coeff_y;
	int32_t** ppc_qt_temp_arl_coeff_cb;
	int32_t** ppc_qt_temp_arl_coeff_cr;
	int32_t* pc_qt_temp_arl_coeff_y;
	int32_t* pc_qt_temp_arl_coeff_cb;
	int32_t* pc_qt_temp_arl_coeff_cr;
#endif
	uint8_t* qt_temp_tr_idx;
	uint8_t* qt_temp_cbf[3];

	x265_image_t** ppc_qt_temp_image;
	x265_simage_t** ppc_qt_temp_simage;
	x265_image_t tmp_image_pred; // To be used in xGetint32_terPredictionError() to avoid constant memory allocation/deallocation
	pixel* shared_pred_transform_skip[3];
	x265_coeff_t* pc_qt_temp_tu_coeff_y;
	x265_coeff_t* pc_qt_temp_tu_coeff_cb;
	x265_coeff_t* pc_qt_temp_tu_coeff_cr;
	uint8_t* qt_temp_transform_skip_flag[3];
	x265_image_t qt_temp_transform_skip_image;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t* pc_qt_temp_tu_arl_coeff_y;
	int32_t* pc_qt_temp_tu_arl_coeff_cb;
	int32_t* pc_qt_temp_tu_arl_coeff_cr;
#endif

	// ME parameters
	int32_t i_search_range;
	int32_t i_bipred_search_range; // Search range for bi-prediction
	int32_t i_fast_search;
	int32_t adapt_sr[2][33];
	x265_mv_t srch_rng_lt;
	x265_mv_t srch_rng_rb;
	x265_mv_t mv_predictors[3];

	// RD computation
	x265_dist_param_t dist_param;

	// Misc.
	pixel* temp_pel;
	const uint32_t* d_filter;
	int32_t i_max_delta_qp;

	// AMVP cost computation
	// uint32_tMVPIdxCost[AMVP_MAX_NUM_CANDS+1][AMVP_MAX_NUM_CANDS];
	uint32_t mvp_idx_cost[X265_AMVP_MAX_NUM_CANDS+1][X265_AMVP_MAX_NUM_CANDS+1]; //th array bounds
} x265_enc_search_t ;


extern const x265_mv_t mv_refine_h[9] ;
extern const x265_mv_t mv_refine_q[9] ;


x265_enc_search_t *x265_enc_search_new (x265_t *h) ;
void x265_enc_search_delete ( x265_t *h, x265_enc_search_t *enc_search ) ;

int x265_enc_search_init ( x265_t *h, x265_enc_search_t *enc_search );
void x265_enc_search_deinit ( x265_t *h, x265_enc_search_t *enc_search );

int x265_enc_search_initialize ( x265_t *h,
								x265_enc_search_t *enc_search,
								x265_tr_quant_t*  pc_tr_quant,
								int32_t i_search_range,
								int32_t i_bipred_search_range,
								int32_t i_fast_search,
								int32_t i_max_delta_qp,
								x265_enc_entropy_t* pc_entropy_coder,
								x265_rd_cost_t* pc_rd_cost,
								x265_enc_sbac_t*** pppc_rd_sbac_coder,
								x265_enc_sbac_t* pc_rd_go_on_sbac_coder
								) ;
void x265_enc_search_deinitialize ( x265_t *h, x265_enc_search_t *enc_search ) ;

uint32_t x265_enc_search_x_pattern_refinement(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_pattern_t *pattern_key,
											x265_mv_t *base_ref_mv,
											int32_t i_frac,
											x265_mv_t *mv_frac ) ;
void x265_enc_search_x_tz_search_help(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_pattern_t *pattern_key,
										x265_int_tz_search_struct_t *int_tz_search_struct,
										const int32_t i_search_x,
										const int32_t i_search_y,
										const uint8_t i_point_nr,
										const uint32_t i_distance ) ;

void x265_enc_search_x_tz2_point_search(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_pattern_t *pattern_key,
										x265_int_tz_search_struct_t *int_tz_search_struct,
										x265_mv_t *mv_srch_rng_lt,
										x265_mv_t *mv_srch_rng_rb ) ;

void x265_enc_search_x_tz8_point_square_search(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_pattern_t *pattern_key,
												x265_int_tz_search_struct_t *int_tz_search_struct,
												x265_mv_t *mv_srch_rng_lt,
												x265_mv_t *mv_srch_rng_rb,
												const int32_t i_start_x,
												const int32_t i_start_y,
												const int32_t i_dist ) ;
void x265_enc_search_x_tz8_point_diamond_search(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_pattern_t *pattern_key,
												x265_int_tz_search_struct_t *int_tz_search_struct,
												x265_mv_t *mv_srch_rng_lt,
												x265_mv_t *mv_srch_rng_rb,
												const int32_t i_start_x,
												const int32_t i_start_y,
												const int32_t i_dist ) ;
void x265_enc_search_x_get_inter_prediction_error(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t* cu,
												x265_image_t *image_org,
												int32_t i_part_idx,
												uint32_t *p_err,
												int32_t b_hadamard ) ;
void x265_enc_search_preest_chroma_pred_mode ( x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												x265_image_t *p_org_image,
												x265_image_t *p_pred_image ) ;
void x265_enc_search_est_intra_pred_qt ( x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *p_org_image,
										x265_image_t *p_pred_image,
										x265_simage_t *p_resi_image,
										x265_image_t *p_reco_image,
										uint32_t *p_dist_c,
										int32_t b_luma_only ) ;
void x265_enc_search_est_intra_pred_chroma_qt( x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												x265_image_t *p_org_image,
												x265_image_t *p_pred_image,
												x265_simage_t *p_resi_image,
												x265_image_t *p_reco_image,
												uint32_t *p_pre_calc_dist_c ) ;

#if X265_AMP_MRG
void x265_enc_search_pred_inter_search(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *p_org_image,
										x265_image_t **pp_pred_image,
										x265_simage_t **pp_resi_image,
										x265_image_t **pp_reco_image,
										int32_t b_use_res,
										int32_t b_use_mrg ) ;
#else
void x265_enc_search_pred_inter_search(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *p_org_image,
										x265_image_t **pp_pred_image,
										x265_simage_t **pp_resi_image,
										x265_image_t **pp_reco_image,
										int32_t b_use_res ) ;
#endif

void x265_enc_search_encode_res_and_calc_rd_inter_cu(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													x265_image_t *p_image_org,
													x265_image_t *p_image_pred,
													x265_simage_t **pp_image_resi,
													x265_simage_t **pp_image_resi_best,
													x265_image_t **pp_image_reco,
													int32_t b_skip_res ) ;
void x265_enc_search_set_adaptive_search_range(x265_enc_search_t *enc_search,
												int32_t i_dir,
												int32_t i_ref_idx,
												int32_t i_search_range) ;
void x265_enc_search_x_enc_pcm(x265_t* h,
								x265_enc_search_t *enc_search,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								pixel *org,
								pixel *pcm,
								pixel *pred,
								spixel *resi,
								pixel *reco,
								uint32_t i_stride,
								uint32_t i_width,
								uint32_t i_height,
								enum text_type_e i_text_type ) ;
void x265_enc_search_ipcm_search(x265_t *h,
								x265_enc_search_t *enc_search,
								x265_data_cu_t *cu,
								x265_image_t *p_org_image,
								x265_image_t **pp_pred_image,
								x265_simage_t **pp_resi_image,
								x265_image_t **pp_reco_image ) ;

void x265_enc_search_x_enc_sub_div_cbf_qt(x265_t* h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										uint32_t i_tr_depth,
										uint32_t i_abs_part_idx,
										int32_t b_luma,
										int32_t b_chroma ) ;

void x265_enc_search_x_enc_sub_div_cbf_qt(x265_t* h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										uint32_t i_tr_depth,
										uint32_t i_abs_part_idx,
										int32_t b_luma,
										int32_t b_chroma ) ;
void x265_enc_search_x_enc_coeff_qt(x265_t *h,
									x265_enc_search_t *enc_search,
									x265_data_cu_t *cu,
									uint32_t i_tr_depth,
									uint32_t i_abs_part_idx,
									enum text_type_e i_text_type,
									int32_t b_real_coeff ) ;
void x265_enc_search_x_enc_intra_header(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										uint32_t i_tr_depth,
										uint32_t i_abs_part_idx,
										int32_t b_luma,
										int32_t b_chroma ) ;
uint32_t x265_enc_search_x_get_intra_bits_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma,
											int32_t b_chroma,
											int32_t b_real_coeff /* just for test */ ) ;
uint32_t x265_enc_search_x_get_intra_bits_qt_chroma(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													uint32_t i_tr_depth,
													uint32_t i_abs_part_idx,
													uint32_t i_chroma_id,
													int32_t b_real_coeff /* just for test */ );
void x265_enc_search_x_intra_coding_luma_blk(x265_t* h,
											x265_enc_search_t* enc_search,
											x265_data_cu_t* cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											x265_image_t *p_org_image,
											x265_image_t *p_pred_image,
											x265_simage_t *p_resi_image,
											uint32_t *p_dist,
											int32_t b_default0_save1_load2 ) ;
void x265_enc_search_x_intra_coding_chroma_blk( x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												uint32_t i_tr_depth,
												uint32_t i_abs_part_idx,
												x265_image_t *p_org_image,
												x265_image_t *p_pred_image,
												x265_simage_t *p_resi_image,
												uint32_t *p_dist,
												uint32_t i_chroma_id,
												int32_t b_default0_save1_load2 ) ;;
void x265_enc_search_x_recur_intra_coding_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma_only,
											x265_image_t *p_org_image,
											x265_image_t *p_pred_image,
											x265_simage_t *p_resi_image,
											uint32_t *p_dist_y,
											uint32_t *p_dist_c,
#if X265_HHI_RQT_INTRA_SPEEDUP
											int32_t b_check_first,
#endif
											double *p_rd_cost ) ;
void x265_enc_search_x_set_intra_result_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma_only,
											x265_image_t* p_reco_image ) ;
void x265_enc_search_x_recur_intra_chroma_coding_qt(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													uint32_t i_tr_depth,
													uint32_t i_abs_part_idx,
													x265_image_t *p_org_image,
													x265_image_t *p_pred_image,
													x265_simage_t *p_resi_image,
													uint32_t *p_dist ) ;
void x265_enc_search_x_set_intra_result_chroma_qt(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												uint32_t i_tr_depth,
												uint32_t i_abs_part_idx,
												x265_image_t *p_reco_image ) ;
void x265_enc_search_x_store_intra_result_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma_only ) ;
void x265_enc_search_x_load_intra_result_qt(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_tr_depth,
											uint32_t i_abs_part_idx,
											int32_t b_luma_only ) ;
void x265_enc_search_x_store_intra_result_chroma_qt(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													uint32_t i_tr_depth,
													uint32_t i_abs_part_idx,
													uint32_t i_state_u0_v1_both2 ) ;
void x265_enc_search_x_load_intra_result_chroma_qt(x265_t *h,
													x265_enc_search_t *enc_search,
													x265_data_cu_t *cu,
													uint32_t i_tr_depth,
													uint32_t i_abs_part_idx,
													uint32_t i_state_u0_v1_both2 ) ;

#if X265_ZERO_MVD_EST
void x265_enc_search_x_estimate_mv_pred_amvp(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											x265_image_t *p_org_image,
											uint32_t i_part_idx,
											enum ref_pic_list_e i_ref_pic_list,
											int32_t i_ref_idx,
											x265_mv_t *p_mv_pred,
											int32_t b_filled,
											uint32_t *p_dist_bip,
											uint32_t *p_dist  ) ;
#else
void x265_enc_search_x_estimate_mv_pred_amvp(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											x265_image_t *p_org_image,
											uint32_t i_part_idx,
											enum ref_pic_list_e i_ref_pic_list,
											int32_t i_ref_idx,
											x265_mv_t *p_mv_pred,
											int32_t b_filled,
											uint32_t* p_dist_bip ) ;
#endif

void x265_enc_search_x_check_best_mvp(x265_t *h,
									x265_enc_search_t *enc_search,
									x265_data_cu_t *cu,
									enum ref_pic_list_e
									e_ref_pic_list,
									x265_mv_t *p_mv,
									x265_mv_t *p_mv_pred,
									int32_t *p_mvp_idx,
									uint32_t *p_bits,
									uint32_t *p_cost ) ;
uint32_t x265_enc_search_x_get_template_cost(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_part_idx,
											uint32_t i_part_addr,
											x265_image_t *p_org_image,
											x265_image_t *template_cand,
											x265_mv_t *p_mv_cand,
											int32_t i_mvp_idx,
											int32_t i_mvp_num,
											enum ref_pic_list_e e_ref_pic_list,
											int32_t i_ref_idx,
											int32_t i_size_x,
											int32_t i_size_y
#if X265_ZERO_MVD_EST
											,uint32_t *p_dist
#endif
												) ;
void x265_enc_search_x_copy_amvp_info(x265_enc_search_t *enc_search,
									x265_amvp_info_t *p_src,
									x265_amvp_info_t *p_dst) ; ;

uint32_t x265_enc_search_x_get_mvp_idx_bits ( x265_enc_search_t *enc_search,
											int32_t i_idx, int32_t i_num ) ;

void x265_enc_search_x_get_blk_bits(x265_enc_search_t *enc_search,
									enum part_size_e i_part_size,
									int32_t b_p_slice,
									int32_t i_part_idx,
									uint32_t i_last_mode,
									uint32_t *p_blk_bit) ;
void x265_enc_search_x_merge_estimation(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *image_org,
										int32_t i_pu_idx,
										uint32_t *p_inter_dir,
										x265_mv_field_t *mv_field,
										uint32_t *p_merge_index,
										uint32_t *p_cost,
										x265_mv_field_t *mv_field_neighbours,
										uint8_t *inter_dir_neighbours,
										int32_t *p_num_valid_merge_cand ) ;
void x265_enc_search_x_restrict_bipred_merge_cand(x265_enc_search_t *enc_search,
												x265_data_cu_t* cu,
												uint32_t i_pu_idx,
												x265_mv_field_t *mv_field_neighbours,
												uint8_t *inter_dir_neighbours,
												int32_t i_num_valid_merge_cand ) ;
void x265_enc_search_x_motion_estimation(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *image_org,
										int32_t i_part_idx,
										enum ref_pic_list_e e_ref_pic_list,
										x265_mv_t *p_mv_pred,
										int32_t i_ref_idx_pred,
										x265_mv_t *p_mv,
										uint32_t *p_bits,
										uint32_t *p_cost,
										int32_t b_bi) ;
void x265_enc_search_x_tz_search(x265_t *h,
								x265_enc_search_t *enc_search,
								x265_data_cu_t *cu,
								x265_pattern_t *pattern_key,
								pixel *p_ref_y,
								int32_t i_ref_stride,
								x265_mv_t *p_mv_srch_rng_lt,
								x265_mv_t *p_mv_srch_rng_rb,
								x265_mv_t *p_mv,
								uint32_t *p_sad ) ;
void x265_enc_search_x_set_search_range(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_mv_t *p_mv_pred,
										int32_t i_srch_rng,
										x265_mv_t *p_mv_srch_rng_lt,
										x265_mv_t* p_mv_srch_rng_rb ) ;

void x265_enc_search_x_pattern_search_fast(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											x265_pattern_t *pattern_key,
											pixel *p_ref_y,
											int32_t i_ref_stride,
											x265_mv_t *p_mv_srch_rng_lt,
											x265_mv_t *p_mv_srch_rng_rb,
											x265_mv_t *p_mv,
											uint32_t *p_sad ) ;
void x265_enc_search_x_pattern_search(x265_t* h,
									x265_enc_search_t *enc_search,
									x265_pattern_t *pattern_key,
									pixel *p_ref_y,
									int32_t i_ref_stride,
									x265_mv_t *p_mv_srch_rng_lt,
									x265_mv_t *p_mv_srch_rng_rb,
									x265_mv_t *p_mv,
									uint32_t *p_sad ) ;
void x265_enc_search_x_pattern_search_frac_dif(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t *cu,
												x265_pattern_t *pattern_key,
												pixel *p_ref_y,
												int32_t i_ref_stride,
												x265_mv_t *p_mv_int,
												x265_mv_t *p_mv_half,
												x265_mv_t *p_mv_qter,
												uint32_t *p_cost,
												int32_t b_bi_pred ) ;
void x265_enc_search_x_ext_dif_up_sampling_h(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_pattern_t* pattern,
											int32_t b_bi_pred ) ;
void x265_enc_search_x_ext_dif_up_sampling_q(x265_t *h,
											x265_enc_search_t* enc_search,
											x265_pattern_t* pattern,
											x265_mv_t *half_pixel_ref,
											int32_t b_bi_pred ) ;

void x265_enc_search_x_encode_residual_qt(x265_t* h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										const uint32_t i_depth,
										int32_t b_subdiv_and_cbf,
										enum text_type_e e_type ) ;
void x265_enc_search_x_estimate_residual_qt(x265_t* h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_quadrant,
											uint32_t i_abs_part_idx,
											uint32_t abs_t_upart_idx,
											x265_simage_t *resi,
											const uint32_t i_depth,
											double *p_cost,
											uint32_t *p_bits,
											uint32_t *p_dist,
											uint32_t *p_zero_dist ) ;
void x265_enc_search_x_set_residual_qt_data(x265_t* h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_quadrant,
											uint32_t i_abs_part_idx,
											uint32_t abs_t_upart_idx,
											x265_simage_t *resi,
											uint32_t i_depth,
											int32_t b_spatial ) ;

uint32_t x265_enc_search_x_mode_bits_intra(x265_t *h,
											x265_enc_search_t* enc_search,
											x265_data_cu_t *cu,
											uint32_t i_mode,
											uint32_t i_pu,
											uint32_t i_part_offset,
											uint32_t i_depth,
											uint32_t i_init_tr_depth ) ;

uint32_t x265_enc_search_x_update_cand_list(x265_enc_search_t* enc_search,
											uint32_t i_mode,
											double f_cost,
											uint32_t i_fast_cand_num,
											uint32_t *cand_mode_list,
											double *cand_cost_list ) ;
void x265_enc_search_x_add_symbol_bits_inter(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_qp,
											uint32_t i_tr_mode,
											uint32_t *p_bits,
											x265_image_t **pp_image_rec,
											x265_image_t *p_image_pred,
											x265_image_t **pp_image_resi ) ;
void x265_enc_search_set_wp_scaling_dist_param ( x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t* cu,
												int32_t i_ref_idx,
												enum ref_pic_list_e ref_pic_list_cur ) ;

void x265_enc_search_set_dist_param_comp(x265_enc_search_t *enc_search, uint32_t i_comp) ;


#endif

