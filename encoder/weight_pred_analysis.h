


#ifndef X265_ENCODER_WEIGHT_PRED_ANALYSIS_H
#define X265_ENCODER_WEIGHT_PRED_ANALYSIS_H


typedef struct
{
	int32_t b_weighted_pred_flag ;
	int32_t b_weighted_bipred_flag;
	x265_wp_scaling_param_t wp[2][X265_MAX_NUM_REF][3];
} x265_weight_pred_analysis_t ;


x265_weight_pred_analysis_t *x265_weight_pred_analysis_new () ;
void x265_weight_pred_analysis_delete ( x265_weight_pred_analysis_t *weight_pred_analysis ) ;

int x265_weight_pred_analysis_init ( x265_weight_pred_analysis_t *weight_pred_analysis );
void x265_weight_pred_analysis_deinit ( x265_weight_pred_analysis_t *weight_pred_analysis );

void x_store_wp_param ( x265_weight_pred_analysis_t *weight_pred_analysis, int32_t b_weighted_pred_flag, int32_t b_weighted_bipred_flag ) ;
void x_restore_wp_param ( x265_t *h, x265_weight_pred_analysis_t *weight_pred_analysis ) ;
void print_weight_pred_analysis ( x265_weight_pred_analysis_t *weight_pred_analysis, FILE *file ) ;

#endif

