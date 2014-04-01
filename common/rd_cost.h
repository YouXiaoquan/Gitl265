


#ifndef X265_RD_COST_H
#define X265_RD_COST_H

#define X265_FIX203 1


typedef struct _x265_rd_cost_t x265_rd_cost_t ;
typedef struct _x265_dist_param_t x265_dist_param_t ;
typedef uint32_t (*x265_fp_dist_func) ( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param) ;

#include "rd_cost_weight_prediction.h"
#include "sad.h"
#include "sse.h"
#include "satd.h"

typedef struct _x265_dist_param_t
{
	pixel* org;
	pixel* cur;
	int32_t i_stride_org;
	int32_t i_stride_cur;
	int32_t i_rows;
	int32_t i_cols;
	int32_t i_step;
	x265_fp_dist_func dist_func;
	int32_t i_bit_depth;

	int32_t b_apply_weight;     // whether weithed prediction is used or not
	x265_wp_scaling_param_t *wp_scaling_param;           // weithed prediction scaling parameters for current ref
	uint32_t i_comp;           // uiComp = 0 (luma Y), 1 (chroma U), 2 (chroma V)

#if X265_NS_HAD
	int32_t b_use_ns_had;
#endif

	// (vertical) subsampling shift (for reducing complexity)
	// - 0 = no subsampling, 1 = even rows, 2 = every 4th, etc.
	int32_t i_sub_shift;

} x265_dist_param_t ;


struct _x265_rd_cost_t
{
	x265_rd_cost_weight_prediction_t rd_cost_weight_prediction ;

	// for distortion
	// for distortion
	int32_t i_blk_width ;
	int32_t i_blk_height ;

#if X265_WEIGHTED_CHROMA_DISTORTION
	double f_cb_distortion_weight ;
	double f_cr_distortion_weight ;
#endif
	double f_lambda ;
	double f_sqrt_lambda ;
	uint32_t i_lambda_motion_sad ;
	uint32_t i_lambda_motion_sse ;
	double f_frame_lambda ;

  // for motion cost
#if X265_FIX203
	x265_mv_t predictor ;
#else
	uint32_t *component_cost_origin_p ;
	uint32_t *component_cost ;
	uint32_t *ver_cost ;
	uint32_t *hor_cost ;
#endif
	uint32_t i_cost ;
	int32_t i_cost_scale ;
#if !X265_FIX203
	int32_t i_search_limit ;
#endif

	x265_sads_func sads_func[64] ;
	x265_sads_func sads_half_func[64] ;
	x265_satd_func satd_func[64] ;
	x265_sses_p_p_func sse_p_p_func[64] ;
	x265_sses_p_s_func sse_p_s_func[64] ;
	x265_sses_s_s_func sse_s_s_func[64] ;
} ;

x265_dist_param_t *x265_dist_param_new () ;
void x265_dist_param_delete ( x265_dist_param_t *dist_param ) ;
int x265_dist_param_init ( x265_dist_param_t *dist_param ) ;
void x265_dist_param_deinit ( x265_dist_param_t *dist_param ) ;

x265_rd_cost_t *x265_rd_cost_new (int cpu) ;
void x265_rd_cost_delete ( x265_rd_cost_t *rd_cost ) ;
int x265_rd_cost_init ( x265_rd_cost_t *rd_cost, int cpu ) ;
void x265_rd_cost_deinit ( x265_rd_cost_t *rd_cost ) ;


void x265_rd_cost_get_motion_cost(x265_rd_cost_t *rd_cost,
									int32_t b_sad,
									int32_t i_add ) ;

void rd_cost_set_lambda( x265_rd_cost_t *rd_cost, double f_ambda ) ;

void x265_rd_cost_set_predictor( x265_rd_cost_t *rd_cost, x265_mv_t *mv ) ;
uint32_t x265_rd_cost_get_cost_p2 ( x265_rd_cost_t *rd_cost, uint32_t i_bit ) ;
uint32_t x265_rd_cost_get_cost_p3 ( x265_rd_cost_t *rd_cost, int32_t x, int32_t y ) ;
uint32_t x265_rd_cost_get_bits( x265_rd_cost_t *rd_cost, int32_t x, int32_t y ) ;
double x265_rd_cost_calc_rd_cost( x265_rd_cost_t *rd_cost, uint32_t i_bits, uint32_t i_distortion, int32_t b_flag, enum d_func_e d_func ) ;
double x265_rd_cost_calc_rd_cost64( x265_rd_cost_t *rd_cost, uint64_t i_bits, uint64_t i_distortion, int32_t b_flag, enum d_func_e d_func ) ;
#if !X265_FIX203
void rd_cost_init_rate_distortion_model( x265_rd_cost_t *rd_cost, int32_t i_sub_pel_search_limit ) ;
#endif
uint32_t rd_cost_x_get_component_bits( x265_rd_cost_t *rd_cost, int32_t i_val ) ;

#endif
