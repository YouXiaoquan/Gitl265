
#include "common/common.h"





void x_store_wp_param ( x265_weight_pred_analysis_t *weight_pred_analysis, int32_t b_weighted_pred_flag, int32_t b_weighted_bipred_flag )
{
	weight_pred_analysis->b_weighted_pred_flag = b_weighted_pred_flag ;
	weight_pred_analysis->b_weighted_bipred_flag = b_weighted_bipred_flag ;
}

void x_restore_wp_param ( x265_t *h, x265_weight_pred_analysis_t *weight_pred_analysis )
{
	h->pps[0].b_use_weight_pred = weight_pred_analysis->b_weighted_pred_flag ;
	h->pps[0].b_use_weighted_bi_pred = weight_pred_analysis->b_weighted_bipred_flag ;
}

void print_weight_pred_analysis ( x265_weight_pred_analysis_t *weight_pred_analysis, FILE *file )
{
	fprintf ( file, "%d ", weight_pred_analysis->b_weighted_pred_flag ) ;
	fprintf ( file, "%d ", weight_pred_analysis->b_weighted_bipred_flag ) ;
}

