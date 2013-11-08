


#ifndef X265_TR_QUANT_H
#define X265_TR_QUANT_H

#include "transform.h"

#define X265_QUANT_IQUANT_SHIFT    20 // Q(QP%6) * IQ(QP%6) = 2^20
#define X265_QUANT_SHIFT           14 // Q(4) = 2^14



#define X265_SHIFT_INV_1ST          7 // Shift after first inverse transform stage
#define X265_SHIFT_INV_2ND         12 // Shift after second inverse transform stage

#define X265_SCALE_BITS            15 // Inherited from TMuC, pressumably for fractional bit estimates in RDOQ
#define X265_MAX_TR_DYNAMIC_RANGE  15 // Maximum transform dynamic range (excluding sign bit)

#define X265_QP_BITS                 15
#define X265_IC_RATE_COST_TABLE_LENGTH				1024




int32_t et_table[4];


typedef struct
{
	int32_t significant_coeff_group_bits[X265_NUM_SIG_CG_FLAG_CTX][2];
	int32_t significant_bits[X265_NUM_SIG_FLAG_CTX][2];
	double significant_bits_cost[X265_NUM_SIG_FLAG_CTX][2];
	int32_t last_x_bits[32];
	int32_t last_y_bits[32];
	int32_t greater_one_bits[X265_NUM_ONE_FLAG_CTX][2];
	int32_t level_abs_bits[X265_NUM_ABS_FLAG_CTX][2];

	int32_t block_cbp_bits[3*X265_NUM_QT_CBF_CTX][2];
	int32_t block_root_cbp_bits[4][2];
} x265_est_bits_sbac_struct_t ;


typedef struct _x265_qp_param_t
{
	int32_t i_qp ;
	int32_t i_per ;
	int32_t i_rem ;
	int32_t i_bits ;

} x265_qp_param_t ;

typedef struct
{
	x265_est_bits_sbac_struct_t* est_bits_sbac;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t qp_delta[X265_MAX_QP+1] ;
	int32_t slice_n_samples[X265_LEVEL_RANGE+1];
	double  slice_sum_c[X265_LEVEL_RANGE+1] ;
#endif
	int32_t *temp_coeff;

	x265_qp_param_t qp_param ;

#if X265_RDOQ_CHROMA_LAMBDA
	double f_lambda_luma ;
	double f_lambda_chroma ;
#endif
	double f_lambda ;

	uint32_t i_rdoq_offset ;
	uint32_t i_max_tr_size ;
	int32_t b_enc ;
	int32_t b_use_rdoq ;
	int32_t b_use_rdoq_ts ;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t b_use_adapt_qp_select ;
#endif

	int32_t ic_rate_cost[5][X265_IC_RATE_COST_TABLE_LENGTH] ;
	int32_t *ctx_sig_list[4][4][3][6] ;
	int32_t b_use_transform_skip_fast ;
	int32_t b_scaling_list_enabled_flag ;
	int32_t *quant_coef [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of quantization matrix coefficient 4x4
	int32_t *dequant_coef [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of dequantization matrix coefficient 4x4
	double *err_scale [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of quantization matrix coefficient 4x4

	x265_transform_t transform ;

} x265_tr_quant_t ;


typedef struct
{
	int32_t i_nnz_before_pos0;
	double f_coded_leveland_dist; // distortion and level cost only
	double f_uncoded_dist;    // all zero coded block distortion
	double f_sig_cost;
	double f_sig_cost_0;
} x265_coeff_group_rd_stats_t ;



extern int32_t inv_quant_scales[6] ;
extern const uint32_t go_rice_range[5] ;
extern const uint32_t go_rice_prefix_len[5] ;


x265_est_bits_sbac_struct_t *x265_est_bits_sbac_struct_new () ;
void x265_est_bits_sbac_struct_delete ( x265_est_bits_sbac_struct_t *est_bits_sbac_struct ) ;
int x265_est_bits_sbac_struct_init ( x265_est_bits_sbac_struct_t *est_bits_sbac_struct ) ;
void x265_est_bits_sbac_struct_deinit ( x265_est_bits_sbac_struct_t *est_bits_sbac_struct ) ;

void x265_qp_param_set_qp_param( x265_qp_param_t *qp_param, int32_t i_qp_scaled ) ;
void x265_qp_param_clear_qp_param( x265_qp_param_t *qp_param ) ;

uint32_t x265_tr_quant_x_get_coded_level ( x265_tr_quant_t *tr_quant,
									double *p_rd64_coded_cost,
									double *p_rd64_coded_cost0,
									double *p_rd64_coded_cost_sig,
									int32_t i_level_double,
									uint32_t i_max_abs_level,
									uint16_t i_ctx_num_sig,
									uint16_t i_ctx_num_one,
									uint16_t i_ctx_num_abs,
									uint16_t i_abs_go_rice,
									uint32_t i_c1_idx,
									uint32_t i_c2_idx,
									int32_t i_q_bits,
									double f_temp,
									int32_t b_last ) ;

double x265_tr_quant_x_get_ic_rate_cost ( x265_tr_quant_t *tr_quant,
										uint32_t i_abs_level,
										uint16_t i_ctx_num_one,
										uint16_t i_ctx_num_abs,
										uint16_t i_abs_go_rice,
										uint32_t i_c1_idx,
										uint32_t i_c2_idx ) ;

x265_tr_quant_t *x265_tr_quant_new (x265_t *h) ;
void x265_tr_quant_delete ( x265_tr_quant_t *tr_quant ) ;

int x265_tr_quant_init ( x265_t *h, x265_tr_quant_t *tr_quant ) ;
void x265_tr_quant_deinit ( x265_tr_quant_t *tr_quant ) ;
int32_t x265_tr_quant_ctx_sig_list_init ( x265_tr_quant_t *tr_quant ) ;
void x265_tr_quant_ctx_sig_list_deinit ( x265_tr_quant_t *tr_quant ) ;
void x265_tr_quant_ic_rate_cost_init ( x265_tr_quant_t *tr_quant ) ;
int32_t x265_tr_quant_get_init_ic_rate_cost_value(x265_tr_quant_t *tr_quant,
												int32_t i_go_rice_param,
												int32_t i_symbol ) ;

int x265_tr_quant_initialize ( x265_t *h, x265_tr_quant_t *tr_quant ) ;
void x265_tr_quant_deinitialize ( x265_t *h, x265_tr_quant_t *tr_quant ) ;


void x265_tr_quant_transform_nxn(x265_t *h,
									x265_tr_quant_t *tr_quant,
									x265_data_cu_t* cu,
									spixel *p_residual,
									uint32_t i_stride,
									x265_coeff_t *p_coeff,
#if X265_ADAPTIVE_QP_SELECTION
									int32_t **pp_arl_coeff,
#endif
									uint32_t i_width,
									uint32_t i_height,
									uint32_t *p_abs_sum,
									enum text_type_e i_text_type,
									uint32_t i_abs_part_idx,
									int32_t b_use_transform_skip
									) ;

void x265_tr_quant_inv_transform_nxn(x265_t *h,
									x265_tr_quant_t *tr_quant,
									int32_t b_trans_quant_bypass,
									enum text_type_e i_text_type,
									uint32_t i_mode,
									spixel *p_residual,
									uint32_t i_stride,
									x265_coeff_t *p_coeff,
									uint32_t i_width,
									uint32_t i_height,
									int32_t scaling_list_type,
									int32_t b_use_transform_skip ) ;

void x265_tr_quant_inv_recur_transform_nxn(x265_t *h,
											x265_tr_quant_t *tr_quant,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx,
											enum text_type_e i_text_type,
											spixel* p_residual,
											uint32_t i_addr,
											uint32_t i_stride,
											uint32_t i_width,
											uint32_t i_height,
											uint32_t i_max_tr_mode,
											uint32_t i_tr_mode,
											x265_coeff_t* p_coeff ) ;

void x265_tr_quant_set_qp_for_quant( x265_tr_quant_t *tr_quant,
									int32_t i_qp_y,
									enum text_type_e i_text_type,
									int32_t i_qp_bd_offset,
									int32_t i_chroma_qp_offset) ;

#if X265_RDOQ_CHROMA_LAMBDA

void x265_tr_quant_set_lambda ( x265_tr_quant_t *tr_quant, double f_lambda_luma, double f_lambda_chroma ) ;
void x265_tr_quant_select_lambda ( x265_tr_quant_t *tr_quant, enum text_type_e i_text_type ) ;

#else

void x265_tr_quant_set_lambda ( x265_tr_quant_t* tr_quant, double f_lambda ) ;

#endif

void x265_tr_quant_set_rdoq_offset ( x265_tr_quant_t *tr_quant, uint32_t i_rdoq_offset ) ;


int32_t x265_tr_quant_calc_pattern_sig_ctx( x265_tr_quant_t *tr_quant,
											const uint32_t* sig_coeff_group_flag,
											uint32_t i_pos_x_cg,
											uint32_t i_pos_y_cg,
											int32_t i_width,
											int32_t i_height ) ;

int32_t x265_tr_quant_get_sig_ctx_inc( x265_tr_quant_t *tr_quant,
										int32_t i_pattern_sig_ctx,
										uint32_t i_scan_idx,
										int32_t i_pos_x,
										int32_t i_pos_y,
										int32_t i_log2_block_size,
										enum text_type_e i_text_type ) ;

uint32_t x265_tr_quant_get_sig_coeff_group_ctx_inc( x265_tr_quant_t *tr_quant,
													const uint32_t* sig_coeff_group_flag,
													const uint32_t i_cg_pos_x,
													const uint32_t i_cg_pos_y,
													int32_t i_width,
													int32_t i_height) ;


int x265_tr_quant_init_scaling_list ( x265_tr_quant_t *tr_quant ) ;
void x265_tr_quant_destroy_scaling_list ( x265_tr_quant_t *tr_quant ) ;

void x265_tr_quant_set_err_scale_coeff( x265_t* h,
										x265_tr_quant_t *tr_quant,
										uint32_t list,
										uint32_t size,
										uint32_t qp ) ;

//!< get Error Scale Coefficent
double *x265_tr_quant_get_err_scale_coeff ( x265_tr_quant_t *tr_quant,
											uint32_t i_list,
											uint32_t i_size,
											uint32_t i_qp ) ;

//!< get Quant Coefficent
int32_t *x265_tr_quant_get_quant_coeff ( x265_tr_quant_t *tr_quant,
										uint32_t i_list,
										uint32_t i_qp,
										uint32_t i_size ) ;

//!< get DeQuant Coefficent
int32_t *x265_tr_quant_get_dequant_coeff ( x265_tr_quant_t *tr_quant,
											uint32_t i_list,
											uint32_t i_qp,
											uint32_t i_size ) ;
void x265_tr_quant_set_use_scaling_list ( x265_tr_quant_t *tr_quant,
											int32_t b_use_scaling_list ) ;

int32_t x265_tr_quant_get_use_scaling_list ( x265_tr_quant_t *tr_quant ) ;


void x265_tr_quant_set_flat_scaling_list( x265_t* h, x265_tr_quant_t *tr_quant ) ;
void x265_tr_quant_x_set_flat_scaling_list( x265_tr_quant_t *tr_quant, uint32_t list, uint32_t size, uint32_t qp ) ;

void x265_tr_quant_x_set_scaling_list_enc( x265_t* h,
											x265_tr_quant_t *tr_quant,
											x265_scaling_list_t *scaling_list,
											uint32_t list_id,
											uint32_t size_id,
											uint32_t qp ) ;
void x265_tr_quant_x_set_scaling_list_dec( x265_t* h,
											x265_tr_quant_t *tr_quant,
											x265_scaling_list_t *scaling_list,
											uint32_t list_id,
											uint32_t size_id,
											uint32_t qp ) ;



void x265_tr_quant_set_scaling_list( x265_t* h, x265_tr_quant_t *tr_quant, x265_scaling_list_t *scaling_list ) ;
void x265_tr_quant_set_scaling_list_dec( x265_t* h, x265_tr_quant_t *tr_quant, x265_scaling_list_t *scaling_list ) ;
void x265_tr_quant_process_scaling_list_enc( int32_t *coeff, int32_t *quant_coeff, int32_t quant_scale, uint32_t height, uint32_t width, uint32_t ratio, int32_t sizu_num, uint32_t dc) ;
void x265_tr_quant_process_scaling_list_dec( int32_t *coeff, int32_t *dequant_coeff, int32_t inv_quant_scale, uint32_t height, uint32_t width, uint32_t ratio, int32_t sizu_num, uint32_t dc) ;


#if X265_ADAPTIVE_QP_SELECTION

void x265_tr_quant_init_slice_qp_delta ( x265_tr_quant_t *tr_quant ) ;
void x265_tr_quant_store_slice_qp_next(x265_tr_quant_t *tr_quant,
										x265_slice_t *slice) ;
void x265_tr_quant_clear_slice_arl_cnt ( x265_tr_quant_t *tr_quant ) ;
int32_t x265_tr_quant_get_qp_delta ( x265_tr_quant_t *tr_quant, int32_t i_qp ) ;
int32_t *x265_tr_quant_get_slice_n_samples ( x265_tr_quant_t *tr_quant ) ;
double *x265_tr_quant_get_slice_sum_c ( x265_tr_quant_t *tr_quant ) ;

#endif




void x265_tr_quant_x_t(x265_t *h,
						x265_tr_quant_t *tr_quant,
						int32_t bit_depth,
						uint32_t i_mode,
						spixel* p_blk_resi,
						uint32_t i_stride,
						int32_t* p_coeff,
						int32_t i_width,
						int32_t i_height ) ;


void x265_tr_quant_x_transform_skip(x265_t *h,
									x265_tr_quant_t *tr_quant,
									int32_t bit_depth,
									spixel* p_blk_resi,
									uint32_t i_stride,
									int32_t* ps_coeff,
									int32_t width,
									int32_t height ) ;

void x265_tr_quant_sign_bit_hiding_hdq(x265_tr_quant_t *tr_quant,
										x265_coeff_t* p_q_coef,
										x265_coeff_t* p_coef,
										uint32_t const *scan,
										int32_t* delta_u,
										int32_t i_width,
										int32_t i_height ) ;

void x265_tr_quant_x_quant(x265_t *h,
							x265_tr_quant_t *tr_quant,
							x265_data_cu_t* cu,
							int32_t *p_src,
							x265_coeff_t *p_des,
#if X265_ADAPTIVE_QP_SELECTION
							int32_t **pp_arl_des,
#endif
							int32_t i_width,
							int32_t i_height,
							uint32_t *p_ac_sum,
							enum text_type_e i_text_type,
							uint32_t i_abs_part_idx ) ;

void x265_tr_quant_x_rate_dist_opt_quant (x265_t *h,
										x265_tr_quant_t *tr_quant,
										x265_data_cu_t* cu,
										int32_t* p_src_coeff,
										x265_coeff_t *p_dst_coeff,
#if X265_ADAPTIVE_QP_SELECTION
                                        int32_t **pp_arl_dst_coeff,
#endif
                                        uint32_t i_width,
                                        uint32_t i_height,
                                        uint32_t *p_i_abs_sum,
                                        enum text_type_e i_text_type,
                                        uint32_t i_abs_part_idx ) ;

void x265_tr_quant_x_rate_dist_opt_quant_ssse3 (x265_t *h,
												x265_tr_quant_t *tr_quant,
												x265_data_cu_t* cu,
												int32_t* p_src_coeff,
												x265_coeff_t *p_dst_coeff,
#if X265_ADAPTIVE_QP_SELECTION
												int32_t **pp_arl_dst_coeff,
#endif
												uint32_t i_width,
												uint32_t i_height,
												uint32_t *p_i_abs_sum,
												enum text_type_e i_text_type,
												uint32_t i_abs_part_idx ) ;

int32_t x265_tr_quant_x_get_ic_rate ( x265_tr_quant_t *tr_quant,
									uint32_t i_abs_level,
									uint16_t i_ctx_num_one,
									uint16_t i_ctx_num_abs,
									uint16_t i_abs_go_rice,
									uint32_t i_c1_idx,
									uint32_t i_c2_idx );


double x265_tr_quant_x_get_rate_last ( x265_tr_quant_t *tr_quant,
										const uint32_t i_pos_x,
										const uint32_t i_pos_y ) ;
double x265_tr_quant_x_get_rate_sig_coeff_group ( x265_tr_quant_t *tr_quant,
												uint16_t i_significance_coeff_group,
                                                uint16_t i_ctx_num_sig ) ;
double x265_tr_quant_x_get_rate_sig_coef  ( x265_tr_quant_t *tr_quant,
											uint16_t i_significance,
											uint16_t i_ctx_num_sig ) ;
double x265_tr_quant_x_get_i_cost ( x265_tr_quant_t *tr_quant, double f_rate ) ;
double x265_tr_quant_x_get_iep_rate ( x265_tr_quant_t *tr_quant ) ;







void x265_tr_quant_x_dequant(x265_t *h,
							x265_tr_quant_t *tr_quant,
							int32_t i_bit_depth,
							const x265_coeff_t* p_src,
							int32_t* p_des,
							int32_t i_width,
							int32_t i_height,
							int32_t i_scaling_list_type ) ;

void x265_tr_quant_x_it(x265_tr_quant_t *tr_quant,
						int32_t i_bit_depth,
						uint32_t i_mode,
						int32_t* p_coef,
						spixel* p_residual,
						uint32_t i_stride,
						int32_t i_width,
						int32_t i_height ) ;


void x265_tr_quant_x_i_transform_skip(x265_t *h,
										x265_tr_quant_t *tr_quant,
										int32_t i_bit_depth,
										int32_t* p_coef,
										spixel* p_residual,
										uint32_t i_stride,
										int32_t i_width,
										int32_t i_height ) ;



#if X265_MATRIX_MULT

void x_tr(x265_t *h,
		int32_t i_bit_depth,
		pixel *block,
		int32_t *coeff,
		uint32_t i_stride,
		uint32_t i_tr_size,
		uint32_t i_mode) ;

void x_i_tr(x265_t *h,
		int32_t *coeff,
		pixel *block,
		uint32_t i_stride,
		uint32_t i_tr_size,
		uint32_t i_mode) ;


#else


void partial_butterfly4(int16_t *src,
						int16_t *dst,
						int32_t i_shift,
						int32_t i_line) ;
void fast_forward_dst(int16_t *block,
						int16_t *coeff,
						int32_t i_shift) ;
void fast_inverse_dst(int16_t *tmp,
						int16_t *block,
						int32_t i_shift) ;
void partial_butterfly_inverse4(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line) ;
void partial_butterfly8(int16_t *src,
						int16_t *dst,
						int32_t i_shift,
						int32_t i_line) ;
void partial_butterfly_inverse8(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line) ;
void partial_butterfly16(int16_t *src,
						int16_t *dst,
						int32_t i_shift,
						int32_t i_line) ;
void partial_butterfly_inverse16(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line) ;
void partial_butterfly32(int16_t *src,
						int16_t *dst,
						int32_t i_shift,
						int32_t i_line) ;
void partial_butterfly_inverse32(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line) ;


void x_tr_mxn(x265_t *h,
				int32_t bit_depth,
				int16_t *block,
				int16_t *coeff,
				int32_t i_width,
				int32_t i_height,
				uint32_t i_mode) ;
void x_i_tr_mxn(int32_t bit_depth,
				int16_t *coeff,
				int16_t *block,
				int32_t i_width,
				int32_t i_height,
				uint32_t i_mode) ;

#endif







void x265_tr_quant_print ( x265_tr_quant_t *tr_quant, FILE *file ) ;



void x265_partial_butterfly32(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line) ;
void x265_partial_butterfly_inverse32(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line) ;




#endif

