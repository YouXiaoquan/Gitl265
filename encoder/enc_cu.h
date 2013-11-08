


#ifndef X265_ENCODER_CU_H
#define X265_ENCODER_CU_H


typedef struct
{
	x265_data_cu_t** best_cu ;      ///< best CUs in each depth
	x265_data_cu_t** temp_cu ;      ///< temporary CUs in each depth
	uint8_t i_total_depth ;

	x265_image_t** pred_image_best ; ///< best Prediction _image for each depth
	x265_simage_t** resi_image_best ; ///< best Residual _image for each depth
	x265_image_t** reco_image_best ; ///< best Reconstruction _image for each depth
	x265_image_t** pred_image_temp ; ///< temporary Prediction _image for each depth
	x265_simage_t** resi_image_temp ; ///< temporary Residual _image for each depth
	x265_image_t** reco_image_temp ; ///< temporary Reconstruction _image for each depth
	x265_image_t** orig_image ;     ///< Original _image for each depth

	//  Data : encoder control
	int32_t b_encode_dqp;

	x265_bit_counter_t *bit_counter ;

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	uint32_t i_lcu_prediction_sad ;
	int32_t i_add_sad_depth ;
	int32_t i_temporal_sad ;
#endif


} x265_enc_cu_t ;


x265_enc_cu_t *x265_enc_cu_new () ;
void x265_enc_cu_delete ( x265_enc_cu_t *enc_cu ) ;

int x265_enc_cu_init ( x265_enc_cu_t *enc_cu );
void x265_enc_cu_deinit ( x265_enc_cu_t *enc_cu );


int x265_enc_cu_create ( x265_t *h, x265_enc_cu_t *enc_cu ) ;
void x265_enc_cu_destroy ( x265_t *h, x265_enc_cu_t *enc_cu ) ;
int x265_enc_cu_initialize ( x265_t *h, x265_enc_cu_t *enc_cu ) ;
void x265_enc_cu_deinitialize ( x265_t *h, x265_enc_cu_t *enc_cu ) ;


void x265_enc_cu_compress_cu ( x265_t *h, x265_enc_cu_t *enc_cu, uint32_t i_cu_addr ) ;
void x265_enc_cu_encode_cu ( x265_t *h, x265_enc_cu_t *enc_cu, uint32_t i_cu_addr ) ;
void x265_enc_cu_finish_cu ( x265_t *h,
							x265_enc_cu_t *enc_cu,
							x265_data_cu_t *cu,
							uint32_t i_abs_part_idx,
							uint32_t i_depth ) ;
#if X265_AMP_ENC_SPEEDUP
void x265_enc_cu_x_compress_cu ( x265_t *h,
								x265_enc_cu_t *enc_cu,
								x265_data_cu_t **ppc_best_cu,
								x265_data_cu_t **ppc_temp_cu,
								uint32_t i_depth,
								enum part_size_e i_parent_part_size ) ;
#else
void x265_enc_cu_x_compress_cu ( x265_t *h,
								x265_enc_cu_t *enc_cu,
								x265_data_cu_t **ppc_best_cu,
								x265_data_cu_t **ppc_temp_cu,
								uint32_t i_depth ) ;
#endif

void x265_enc_cu_x_encode_cu ( x265_t *h,
								x265_enc_cu_t *enc_cu,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								uint32_t i_depth ) ;

int x265_enc_cu_x_tu_collect_arl_stats( x265_enc_cu_t *enc_cu,
										x265_coeff_t* rpcCoeff,
										int32_t* arl_coeff,
										int32_t i_num_coeff_in_cu,
										double* f_c_sum,
										uint32_t* i_num_samples ) ;
void x265_enc_cu_x_lcu_collect_arl_stats ( x265_t *h, x265_enc_cu_t *enc_cu, x265_data_cu_t *cu ) ;

#if X265_AMP_ENC_SPEEDUP
#if X265_AMP_MRG
void x265_enc_cu_derive_test_mode_amp (x265_enc_cu_t *enc_cu,
										x265_data_cu_t *cu,
										enum part_size_e i_parent_part_size,
										int32_t *p_test_amp_hor,
										int32_t *p_test_amp_ver,
										int32_t *p_test_merge_amp_hor,
										int32_t *p_test_merge_amp_ver ) ;
#else
void x265_enc_cu_derive_test_mode_amp (x265_enc_cu_t *enc_cu,
										x265_data_cu_t *cu,
										enum part_size_e i_parent_part_size,
										int32_t *p_test_amp_hor,
										int32_t *p_test_amp_ver ) ;
#endif
#endif

void x265_enc_cu_x_check_rd_cost_merge_2nx2n ( x265_t *h,
												x265_enc_cu_t *enc_cu,
												x265_data_cu_t **ppc_best_cu,
												x265_data_cu_t **ppc_temp_cu,
												int32_t *p_early_detection_skip_mode ) ;
#if X265_AMP_MRG
void x265_enc_cu_x_check_rd_cost_inter ( x265_t *h,
										x265_enc_cu_t *enc_cu,
										x265_data_cu_t **ppc_best_cu,
										x265_data_cu_t **ppc_temp_cu,
										enum part_size_e i_part_size,
										int32_t b_use_mrg ) ;
#else
void x265_enc_cu_x_check_rd_cost_inter ( x265_t *h,
										x265_enc_cu_t *enc_cu,
										x265_data_cu_t **ppc_best_cu,
										x265_data_cu_t **ppc_temp_cu,
										enum part_size_e i_part_size ) ;
#endif
void x265_enc_cu_x_check_rd_cost_intra ( x265_t *h,
										x265_enc_cu_t *enc_cu,
										x265_data_cu_t **ppc_best_cu,
										x265_data_cu_t **ppc_temp_cu,
										enum part_size_e i_part_size ) ;
void x265_enc_cu_x_check_intra_pcm ( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_data_cu_t **ppc_best_cu,
									x265_data_cu_t **ppc_temp_cu ) ;
void x265_enc_cu_x_check_best_mode ( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_data_cu_t **ppc_best_cu,
									x265_data_cu_t **ppc_temp_cu,
									uint32_t i_depth ) ;
void x265_enc_cu_x_check_dqp( x265_t *h, x265_enc_cu_t *enc_cu, x265_data_cu_t* cu ) ;

int32_t x265_enc_cu_x_compute_qp ( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_data_cu_t *cu,
									uint32_t i_depth ) ;

void x265_enc_cu_x_copy_amvp_info (x265_enc_cu_t *enc_cu,
								x265_amvp_info_t* p_src,
								x265_amvp_info_t* p_dst) ;

void x265_enc_cu_x_copy_image2frame( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_frame_t *frame,
									uint32_t i_cu_addr,
									uint32_t i_abs_part_idx,
									uint32_t i_depth,
									uint32_t i_src_depth,
									x265_data_cu_t* cu,
									uint32_t i_l_pixel_x,
									uint32_t i_t_pixel_y ) ;
void x265_enc_cu_x_copy_image2tmp( x265_t *h,
								x265_enc_cu_t *enc_cu,
								uint32_t i_part_unit_idx,
								uint32_t i_next_depth ) ;
void x265_enc_cu_x_fill_pcm_buffer ( x265_t *h,
									x265_enc_cu_t *enc_cu,
									x265_data_cu_t *cu,
									x265_image_t* p_org_image ) ;

#endif

