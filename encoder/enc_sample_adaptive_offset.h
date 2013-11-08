


#ifndef X265_ENCODER_SAMPLE_ADAPTIVE_OFFSET_H
#define X265_ENCODER_SAMPLE_ADAPTIVE_OFFSET_H

#include "common/sample_adaptive_offset.h"


typedef struct
{
	x265_sample_adaptive_offset_t sample_adaptive_offset ;
	double f_Lambda_luma;
	double f_Lambda_chroma;

	x265_enc_sbac_t*** pppc_rd_sbac_coder;              ///< for CABAC
#if X265_FAST_BIT_EST
	x265_enc_bin_cabac_counter_t*** pppc_bin_coder_cabac;            ///< temporal CABAC state storage for RD computation
#else
	x265_enc_bin_cabac_t*** pppc_bin_coder_cabac;            ///< temporal CABAC state storage for RD computation
#endif

	int64_t ***count;      //[MAX_NUM_SAO_PART][MAX_NUM_SAO_TYPE][MAX_NUM_SAO_CLASS];
	int64_t ***offset;     //[MAX_NUM_SAO_PART][MAX_NUM_SAO_TYPE][MAX_NUM_SAO_CLASS];
	int64_t ***offset_org;  //[MAX_NUM_SAO_PART][MAX_NUM_SAO_TYPE];
	int64_t ***count_pre_dblk;      //[LCU][YCbCr][MAX_NUM_SAO_TYPE][MAX_NUM_SAO_CLASS];
	int64_t ***offset_org_pre_dblk;  //[LCU][YCbCr][MAX_NUM_SAO_TYPE][MAX_NUM_SAO_CLASS];
	int64_t **rate;        //[MAX_NUM_SAO_PART][MAX_NUM_SAO_TYPE];
	int64_t **dist;        //[MAX_NUM_SAO_PART][MAX_NUM_SAO_TYPE];
	double **cost;        //[MAX_NUM_SAO_PART][MAX_NUM_SAO_TYPE];
	double *cost_part_best; //[MAX_NUM_SAO_PART];
	int64_t *dist_org;      //[MAX_NUM_SAO_PART];
	int32_t *type_part_best; //[MAX_NUM_SAO_PART];
	int32_t i_offset_th_y;
	int32_t i_offset_th_c;
	int32_t b_use_sbac_rd;
#if X265_SAO_ENCODING_CHOICE
#if X265_SAO_ENCODING_CHOICE_CHROMA
	double depth_sao_rate[2][4];
#else
	double f_depth_0_sao_rate;
#endif
#endif


#if X265_SAO_ENCODING_CHOICE
#if X265_SAO_ENCODING_CHOICE_CHROMA
	int32_t num_no_sao[2];
#else
	int32_t i_num_no_sao = 0;
#endif
#endif


} x265_enc_sample_adaptive_offset_t ;


x265_enc_sample_adaptive_offset_t *x265_enc_sample_adaptive_offset_new () ;
void x265_enc_sample_adaptive_offset_delete ( x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset ) ;

int x265_enc_sample_adaptive_offset_init ( x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset ) ;
void x265_enc_sample_adaptive_offset_deinit ( x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset ) ;


void x265_enc_sample_adaptive_offset_dump_rec_data (x265_t *h,
													x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
													int32_t i_addr,
													int32_t i_part_idx,
													int32_t i_y_cb_cr ) ;
void x265_enc_sample_adaptive_offset_start_sao_enc ( x265_t *h, x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset ) ;
void x265_enc_sample_adaptive_offset_end_sao_enc ( x265_t *h, x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset ) ;

void x265_enc_sample_adaptive_offset_reset_stats(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset) ;

void x265_enc_sample_adaptive_offset_sao_process_cu ( x265_t *h,
													x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
													x265_sao_param_t *sao_param,
													uint32_t i_cu_addr,
													double f_lambda,
													double f_lambda_chroma,
													int32_t i_depth ) ;
void x265_enc_sample_adaptive_offset_sao_process_last_row_cu(x265_t *h,
															x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
															x265_sao_param_t *sao_param,
															double f_lambda,
															double f_lambda_chroma,
															int32_t depth ) ;
void x265_enc_sample_adaptive_offset_calc_sao_stats_cu_org(x265_t *h,
															x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
															uint32_t i_cu_addr,
															int32_t i_part_idx,
															int32_t i_y_cb_cr) ;
void x265_enc_sample_adaptive_offset_calc_sao_stats_cu_before_dblk(x265_t *h,
																	x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
																	uint32_t i_cu_addr ) ;
void x265_enc_sample_adaptive_offset_destroy_enc_buffer ( x265_t *h, x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset ) ;
int x265_enc_sample_adaptive_offset_create_enc_buffer ( x265_t *h, x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset ) ;

void x265_enc_sample_adaptive_offset_check_merge(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_unit_curr,
												x265_sao_lcu_param_t *sao_unit_check,
												int32_t dir) ;
void x265_enc_sample_adaptive_offset_rdo_sao_unit_cu(x265_t *h,
													x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
													x265_sao_param_t *sao_param,
													uint32_t i_cu_addr,
													double lambda,
													double lambda_chroma,
													int32_t depth) ;
void x265_enc_sample_adaptive_offset_sao_component_param_dist(x265_t *h,
																x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
																int32_t allow_merge_left,
																int32_t allow_merge_up,
																x265_sao_param_t *sao_param,
																uint32_t i_cu_addr,
																uint32_t i_cu_addr_up,
																uint32_t i_cu_addr_left,
																int32_t y_cb_cr,
																double lambda,
																x265_sao_lcu_param_t *comp_sao_param,
																double *comp_distortion) ;
void x265_enc_sample_adaptive_offset_sao_2_chroma_param_dist(x265_t *h,
															x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
															int32_t allow_merge_left,
															int32_t allow_merge_up,
															x265_sao_param_t *sao_param,
															uint32_t i_cu_addr,
															uint32_t i_cu_addr_up,
															uint32_t i_cu_addr_left,
															double lambda,
															x265_sao_lcu_param_t *cr_sao_param,
															x265_sao_lcu_param_t *cb_sao_param,
															double *distortion) ;
int64_t x265_enc_sample_adaptive_offset_est_sao_dist(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
													int64_t count,
													int64_t offset,
													int64_t offset_org,
													int64_t shift) ;
int64_t x265_enc_sample_adaptive_offset_est_iter_offset(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
														int32_t type_idx,
														int32_t class_idx,
														double lambda,
														int64_t offset_input,
														int64_t count,
														int64_t offset_org,
														int32_t shift,
														int32_t bit_increase,
														int32_t *current_distortion_table_bo,
														double *current_rd_cost_table_bo,
														int32_t offset_th ) ;
int64_t x265_enc_sample_adaptive_offset_est_sao_type_dist(x265_t *h,
														x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
														int32_t comp_idx,
														int32_t type_idx,
														int32_t shift,
														double lambda,
														int32_t *current_distortion_table_bo,
														double *current_rd_cost_table_bo) ;

void x265_enc_sample_adaptive_offset_set_max_num_offsets_per_pic(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset,
																int32_t i_val) ;
int32_t x265_enc_sample_adaptive_offset_get_max_num_offsets_per_pic(x265_enc_sample_adaptive_offset_t *enc_sample_adaptive_offset) ;




#endif

