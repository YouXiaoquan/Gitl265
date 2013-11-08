


#ifndef X265_PREDICTION_H
#define X265_PREDICTION_H


#include "intra_pred.h"

typedef struct
{
	x265_weight_prediction_t weight_prediction ;
	x265_intra_pred_t intra_pred ;
	pixel* image_ext_buffer;
	pixel* image_ext;
	int32_t i_image_ext_stride;
	int32_t i_image_ext_height;

	x265_image_t image_pred[2];
	x265_simage_t simage_pred[2];
	x265_simage_t image_temp_cand;
	x265_image_t image_pred_temp;
	x265_image_t filtered_block[4][4];
	x265_simage_t filtered_block_tmp[4];

	x265_interpolation_filter_t interpolation_filter;

	pixel* luma_rec_buffer;       ///< array for downsampled reconstructed luma sample
	int32_t i_luma_rec_stride;       ///< stride of #m_pLumaRecBuffer array

} x265_prediction_t ;

void x265_prediction_x_pred_intra_ang(int32_t bit_depth,
									pixel *left,
									pixel *top,
									pixel** pp_dst,
									int32_t i_dst_stride,
									uint32_t i_width,
									uint32_t i_height,
									uint32_t i_dir_mode,
									int32_t b_blk_above_available,
									int32_t b_blk_left_available,
									int32_t b_filter ) ;
void x265_prediction_x_pred_intra_planar(x265_t* h,
										pixel* p_src,
										pixel* p_dst,
										int32_t i_dst_stride,
										uint32_t i_width,
										uint32_t i_height ) ;
void x265_prediction_x_pred_inter_uni_p ( x265_t *h,
										x265_prediction_t* prediction,
										x265_data_cu_t *cu,
										uint32_t i_part_addr,
										int32_t i_width,
										int32_t i_height,
										enum ref_pic_list_e i_ref_pic_list,
										x265_image_t **pp_image_pred,
										int32_t b_bi ) ;
void x265_prediction_x_pred_inter_uni_s ( x265_t *h,
										x265_prediction_t* prediction,
										x265_data_cu_t *cu,
										uint32_t i_part_addr,
										int32_t i_width,
										int32_t i_height,
										enum ref_pic_list_e i_ref_pic_list,
										x265_simage_t **pp_simage_pred,
										int32_t b_bi ) ;
void x265_prediction_x_pred_inter_bi ( x265_t *h,
										x265_prediction_t* prediction,
										x265_data_cu_t* cu,
										uint32_t i_part_addr,
										int32_t i_width,
										int32_t i_height,
										x265_image_t **pp_image_pred ) ;
void x265_prediction_x_pred_inter_luma_blk_p( x265_t *h,
											x265_prediction_t* prediction,
											x265_data_cu_t *cu,
											x265_frame_t *ref_pic,
											uint32_t i_part_addr,
											x265_mv_t *mv,
											int32_t i_width,
											int32_t i_height,
											x265_image_t **pp_dst_image,
											int32_t b_bi ) ;
void x265_prediction_x_pred_inter_luma_blk_s( x265_t *h,
											x265_prediction_t* prediction,
											x265_data_cu_t *cu,
											x265_frame_t *ref_pic,
											uint32_t i_part_addr,
											x265_mv_t *mv,
											int32_t i_width,
											int32_t i_height,
											x265_simage_t **pp_dst_simage,
											int32_t b_bi ) ;
void x265_prediction_x_pred_inter_chroma_blk_p(x265_t *h,
											x265_prediction_t* prediction,
											x265_data_cu_t *cu,
											x265_frame_t *ref_pic,
											uint32_t i_part_addr,
											x265_mv_t *mv,
											int32_t i_width,
											int32_t i_height,
											x265_image_t **pp_dst_image,
											int32_t b_bi ) ;
void x265_prediction_x_pred_inter_chroma_blk_s(x265_t *h,
											x265_prediction_t* prediction,
											x265_data_cu_t *cu,
											x265_frame_t *ref_pic,
											uint32_t i_part_addr,
											x265_mv_t *mv,
											int32_t i_width,
											int32_t i_height,
											x265_simage_t **pp_dst_simage,
											int32_t b_bi ) ;
void x265_prediction_x_weighted_average( x265_t *h,
										x265_simage_t* p_simage_src0,
										x265_simage_t* p_simage_src1,
										int32_t i_ref_idx0,
										int32_t i_ref_idx1,
										uint32_t i_part_idx,
										int32_t i_width,
										int32_t i_height,
										x265_image_t** pp_image_dst ) ;
void x265_prediction_x_dc_pred_filtering(pixel *left,
										pixel *top,
										pixel **pp_dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height ) ;
int32_t x265_prediction_x_check_identical_motion(x265_t *h,
												x265_data_cu_t* cu,
												uint32_t i_part_addr ) ;

x265_prediction_t *x265_prediction_new (x265_t *h) ;
void x265_prediction_delete ( x265_prediction_t *prediction ) ;

int x265_prediction_init ( x265_t *h, x265_prediction_t *prediction );
void x265_prediction_deinit ( x265_prediction_t *prediction );

int x265_prediction_init_temp_buff( x265_t *h, x265_prediction_t *prediction ) ;
void x265_prediction_deinit_temp_buff( x265_prediction_t *prediction ) ;

void x265_prediction_motion_compensation(x265_t *h,
										x265_prediction_t* prediction,
										x265_data_cu_t *cu,
										x265_image_t* p_image_pred,
										enum ref_pic_list_e i_ref_pic_list,
										int32_t i_part_idx ) ;

void x265_prediction_pred_intra_luma_ang( x265_t* h,
										x265_prediction_t* prediction,
										x265_pattern_t* pattern,
										uint32_t i_dir_mode,
										pixel* p_pred,
										uint32_t i_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t b_above,
										int32_t b_left ) ;
void x265_prediction_pred_intra_chroma_ang( x265_t* h,
											x265_prediction_t* prediction,
											pixel* src,
											uint32_t i_dir_mode,
											pixel* pred,
											uint32_t i_stride,
											int32_t i_width,
											int32_t i_height,
											int32_t b_above,
											int32_t b_left ) ;
pixel x265_prediction_pred_intra_get_pred_val_dc( pixel *left,
												pixel *top,
												uint32_t i_width,
												uint32_t i_height,
												int32_t b_above,
												int32_t b_left ) ;


pixel *x265_prediction_get_predic_buf ( x265_prediction_t *prediction ) ;
int32_t x265_prediction_get_predic_buf_width ( x265_prediction_t *prediction ) ;
int32_t x265_prediction_get_predic_buf_height ( x265_prediction_t *prediction ) ;


#endif

