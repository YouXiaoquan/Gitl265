


#ifndef X265_RD_COST_WEIGHT_PREDICTION_H
#define X265_RD_COST_WEIGHT_PREDICTION_H




typedef struct _x265_rd_cost_weight_prediction_t x265_rd_cost_weight_prediction_t ;

struct _x265_rd_cost_weight_prediction_t
{
	int32_t i_w0 ;
	int32_t i_w1 ;
	int32_t i_shift ;
	int32_t i_offset ;
	int32_t i_round ;
	int32_t b_x_set_done ;
} ;


typedef struct
{
	x265_rd_cost_weight_prediction_t *rd_cost_weight_prediction ;

} x265_rd_cost_weight_prediction_class_static_t ;


extern x265_rd_cost_weight_prediction_class_static_t *p_rd_cost_weight_prediction_class_static ;


x265_rd_cost_weight_prediction_t *x265_rd_cost_weight_prediction_new () ;
void x265_rd_cost_weight_prediction_delete ( x265_rd_cost_weight_prediction_t *rd_cost_weight_prediction ) ;
int x265_rd_cost_weight_prediction_init ( x265_rd_cost_weight_prediction_t *rd_cost_weight_prediction ) ;
void x265_rd_cost_weight_prediction_deinit ( x265_rd_cost_weight_prediction_t *rd_cost_weight_prediction ) ;

void x_set_wp_scale ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction,
						int32_t i_w0,
						int32_t i_w1,
						int32_t i_shift,
						int32_t i_offset,
						int32_t i_round) ;
uint32_t x_get_sadw ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param ) ;
uint32_t x_get_ssew ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param ) ;
uint32_t x_calc_hads2x2w ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, pixel *pi_org, pixel *pi_cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step ) ;
uint32_t x_calc_hads4x4w ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, pixel *pi_org, pixel *pi_cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step ) ;
uint32_t x_calc_hads8x8w ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, pixel *pi_org, pixel *pi_cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step ) ;
uint32_t x_get_hads4w ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param ) ;
uint32_t x_get_hads8w ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param ) ;
uint32_t x_get_hadsw ( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param ) ;

void print_rd_cost_weight_prediction_offset () ;







#endif


