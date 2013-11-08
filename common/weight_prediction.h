


#ifndef X265_WEIGHT_PREDICTION_H
#define X265_WEIGHT_PREDICTION_H


typedef struct
{
	x265_wp_scaling_param_t  wp0[3], wp1[3];

} x265_weight_prediction_t ;


x265_weight_prediction_t *x265_weight_prediction_new () ;
void x265_weight_prediction_delete ( x265_weight_prediction_t *weight_prediction ) ;

int x265_weight_prediction_init ( x265_weight_prediction_t *weight_prediction );
void x265_weight_prediction_deinit ( x265_weight_prediction_t *weight_prediction );


void x265_weight_prediction_get_wp_scaling( x265_t *h,
											x265_weight_prediction_t *weight_prediction,
											x265_data_cu_t* pc_cu,
											int32_t *p_ref_idx0, int32_t *p_ref_idx1,
											x265_wp_scaling_param_t ** pp_wp0,
											x265_wp_scaling_param_t ** pp_wp1 ) ;

void x265_weight_prediction_add_weight_bi(x265_t *h,
										x265_weight_prediction_t *weight_prediction,
										x265_simage_t *p_image_src0,
										x265_simage_t *p_image_src1,
										uint32_t i_part_unit_idx,
										uint32_t i_width,
										uint32_t i_height,
										x265_wp_scaling_param_t *wp0,
										x265_wp_scaling_param_t *wp1,
										x265_image_t *p_image_dst,
										int32_t b_round ) ;
void x265_weight_prediction_add_weight_uni(x265_t *h,
											x265_weight_prediction_t *weight_prediction,
											x265_simage_t *p_image_src0,
											uint32_t i_part_unit_idx,
											uint32_t i_width,
											uint32_t i_height,
											x265_wp_scaling_param_t *wp0,
											x265_image_t* p_image_dst ) ;
void x265_weight_prediction_x_weighted_prediction_uni(x265_t *h,
													x265_weight_prediction_t *weight_prediction,
													x265_data_cu_t *cu,
													x265_simage_t *p_image_src,
													uint32_t i_part_addr,
													int32_t i_width,
													int32_t i_height,
													enum ref_pic_list_e i_ref_pic_list,
													x265_image_t **pp_image_pred,
													int32_t i_ref_idx) ;
void x265_weight_prediction_x_weighted_prediction_bi(x265_t *h,
													x265_weight_prediction_t *weight_prediction,
													x265_data_cu_t *cu,
													x265_simage_t *p_image_src0,
													x265_simage_t *p_image_src1,
													int32_t i_ref_idx0,
													int32_t i_ref_idx1,
													uint32_t i_part_idx,
													int32_t i_width,
													int32_t i_height,
													x265_image_t *p_image_dst ) ;


#endif

