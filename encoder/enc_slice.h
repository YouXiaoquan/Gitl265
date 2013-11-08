


#ifndef X265_ENCODER_SLICE_H
#define X265_ENCODER_SLICE_H

#include "weight_pred_analysis.h"

typedef struct
{
	x265_weight_pred_analysis_t weight_pred_analysis ;
	uint64_t i_pic_total_bits ;
	uint64_t i_pic_dist ;
	double f_pic_rd_cost ;
	double * rd_pic_lambda ;
	double * f_rd_pic_qp ;
	int32_t *rd_pic_qp ;
	x265_enc_bin_if_t *pc_buffer_bin_coder_cabacs ;
	x265_enc_entropy_if_t *pc_buffer_sbac_coders ;
	x265_enc_bin_if_t *pc_buffer_low_lat_bin_coder_cabacs ;
	x265_enc_entropy_if_t *pc_buffer_low_lat_sbac_coders ;
	uint32_t i_slice_idx ;
	x265_wp_scaling_param_t m_wp[2][X265_MAX_NUM_REF][3];
	uint32_t i_bits_originally_in_substreams ;
	uint32_t i_start_cu_addr ;
	uint32_t i_bounding_cu_addr ;


} x265_enc_slice_t ;


x265_enc_slice_t *x265_enc_slice_new ( x265_t *h ) ;
void x265_enc_slice_delete ( x265_enc_slice_t *enc_slice ) ;
int x265_enc_slice_init ( x265_t *h, x265_enc_slice_t *enc_slice );
void x265_enc_slice_deinit ( x265_enc_slice_t *enc_slice );
int x265_enc_slice_create ( x265_t *h, x265_enc_slice_t *enc_slice ) ;
void x265_enc_slice_destroy ( x265_enc_slice_t *enc_slice ) ;

void x265_enc_slice_initialize ( x265_t *h, x265_frame_t *frame,
								int32_t i_poc_last, int32_t i_poc_curr, int32_t i_num_pic_rcvd,
								int32_t i_gop_id, x265_slice_t **pp_slice ) ;
void x265_enc_slice_print ( x265_t *h, x265_enc_slice_t *enc_slice ) ;

void x265_enc_slice_pre_compress_slice ( x265_t *h ) ;
int x265_enc_slice_compress_and_encode_cu ( x265_t *h,
											x265_output_bitstream_t *pc_bitstream,
											x265_output_bitstream_t **ppc_substreams ) ;

void x265_enc_slice_set_search_range ( x265_t *h,
										x265_enc_slice_t *enc_slice,
										x265_slice_t* slice ) ;
void x265_enc_slice_x_determine_start_and_bounding_cu_addr (
	x265_t *h, x265_enc_slice_t *p_enc_slice,
	uint32_t *p_start_cu_addr, uint32_t* p_bounding_cu_addr,
	int32_t b_encode_slice ) ;

void x265_enc_slice_start_compress_slice ( x265_t *h, x265_enc_slice_t *enc_slice) ;
void x265_enc_slice_start_encode_slice (x265_t *h, x265_enc_slice_t *enc_slice, x265_output_bitstream_t **ppc_substreams ) ;
void x265_enc_slice_end_compress_slice ( x265_t *h, x265_enc_slice_t *enc_slice ) ;
void x265_enc_slice_end_encode_slice ( x265_t *h, x265_enc_slice_t *enc_slice ) ;
int x265_enc_slice_compress_cu ( x265_t *h, x265_enc_slice_t *enc_slice, uint32_t i_cu_addr ) ;
void x265_enc_slice_encode_cu ( x265_t *h,
								x265_enc_slice_t *enc_slice,
								uint32_t i_cu_addr,
								x265_output_bitstream_t *bitstream,
								x265_output_bitstream_t **pc_substreams ) ;

#endif


