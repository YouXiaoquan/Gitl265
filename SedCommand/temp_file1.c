
#include "common/common.h"


typedef struct
{
	int32_t i_nnz_before_pos0;
	double f_coded_leveland_dist; // distortion and level cost only
	double f_uncoded_dist;    // all zero coded block distortion
	double f_sig_cost;
	double f_sig_cost_0;
} x265_coeff_group_rd_stats_t ;



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
                                        uint32_t i_abs_part_idx )
{
	int32_t loop = 0;
	int32_t i_q_bits = 0;
	uint32_t i_log2_tr_size = 0;
	int32_t i_q = 0;
	uint32_t i_bit_depth = 0;
	int32_t i_transform_shift = 0;
	uint32_t i_go_rice_param = 0;
	uint32_t i_log2_blk_size = 0;
	uint32_t i_max_num_coeff = 0;
	int32_t i_scaling_list_type = 0 ;
	int32_t *p_q_coef_org = NULL;
	int32_t *p_q_coef = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t i_q_bits_c = 0;
	int32_t i_add_c =  0;
#endif
	uint32_t i_scan_idx = 0;
	int32_t rate_inc_up   [ 32 * 32 ];
	int32_t rate_inc_down [ 32 * 32 ];
	int32_t sig_rate_delta[ 32 * 32 ];
	int32_t delta_u      [ 32 * 32 ];
	uint32_t *scan_cg = NULL;
	uint32_t i_cg_size = 0;
	uint32_t sig_coeff_group_flag[ X265_MLS_GRP_NUM ];
	uint32_t i_num_blk_side = 0;
	int32_t i_cg_last_scan_pos = 0;
	uint32_t i_ctx_set = 0;
	int32_t i_c1 = 0;
	int32_t i_c2 = 0;
	int32_t i_last_scan_pos = 0;
	uint32_t i_c1_idx = 0;
	uint32_t i_c2_idx = 0;
	int32_t i_base_level = 0;
	uint32_t *scan = NULL;
	uint32_t i_cg_num = 0;
	int32_t i_scan_po = 0;
	x265_coeff_group_rd_stats_t rd_stats;
	int32_t i_cg_scan_pos = 0 ;
	uint32_t i_cg_blk_pos = 0;
	uint32_t i_cg_pos_y = 0;
	uint32_t i_cg_pos_x = 0;
	int32_t i_pattern_sig_ctx = 0;
	int32_t i_scan_posin_cg = 0;
	uint32_t i_blk_pos = 0;
	int32_t i_level_double = 0;
	uint32_t i_max_abs_level = 0;
	uint32_t i_level = 0;
	uint32_t i_one_ctx = 0;
	uint32_t i_abs_ctx = 0;
	uint32_t i_pos_y = 0;
	uint32_t i_pos_x = 0;
	ushort i_ctx_sig = 0;
	int32_t i_rate_now = 0;
	int32_t i_scan_posin_cg = 0;
	uint32_t i_blk_pos = 0;
	int32_t i_ctx_cbf = 0;
	int32_t i_best_last_idx_p1 = 0;
	int32_t b_found_last = 0;
	int32_t i_cg_scan_pos = 0;
	uint32_t i_cg_blk_pos = 0;
	int32_t i_scan_posin_cg = 0;
	uint32_t i_blk_pos = 0;
	uint32_t i_pos_y = 0;
	uint32_t i_pos_x = 0;
	int32_t i_scan_pos = 0;
	int32_t i_rd_factor = 0;
	int32_t i_last_cg = 0;
	int32_t i_abs_sum = 0 ;
	int32_t n ;
	int32_t i_sub_set = 0;
	int32_t i_sub_pos = 0;
	int32_t i_first_nz_pos_in_cg = 0;
	uint32_t i_sign_bit = 0;
	int32_t i_min_cost_inc = 0, i_cur_cost = 0;
	int32_t i_min_pos = 0, i_final_change = 0, i_cur_change=0;
	uint32_t i_blk_pos = 0;
	int32_t i_cost_up = 0;
	int32_t i_cost_down = 0;
	uint32_t i_this_sign_bit = 0;

	double f_temp = 0.0;
	double f_block_uncoded_cost = 0.0;
	double f_err_scale = 0.0;
	double f_base_cost = 0.0;
	double f_err = 0.0;
	double f_cost_zero_cg = 0.0;
	double f_best_cost = 0.0;
	double f_cost_last= 0.0;
	double f_total_cost = 0.0;
	double *p_err_scale_org = NULL;
	double *p_err_scale = NULL;
	double cost_coeff [ 32 * 32 ];
	double cost_sig   [ 32 * 32 ];
	double cost_coeff0[ 32 * 32 ];
	double cost_coeff_group_sig[ X265_MLS_GRP_NUM ];



}


