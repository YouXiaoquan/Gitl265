


#ifndef X265_ENC_GOP_H
#define X265_ENC_GOP_H


#include "enc_analyze.h"


typedef struct
{
	int32_t i_poc_cra ;
	int32_t i_last_idr ;
	int32_t b_refresh_pending ;
	x265_enc_analyze_t enc_analyze_all;
	x265_enc_analyze_t enc_analyze_i;
	x265_enc_analyze_t enc_analyze_p;
	x265_enc_analyze_t enc_analyze_b;
	x265_uint_list_t *stored_start_cu_addr_for_encoding_slice ;
	x265_uint_list_t *stored_start_cu_addr_for_encoding_slice_segment ;
} x265_enc_gop_t ;



x265_enc_gop_t *x265_enc_gop_new ( x265_t *h ) ;
void x265_enc_gop_delete ( x265_t *h, x265_enc_gop_t *enc_gop ) ;
int x265_enc_gop_init ( x265_t *h, x265_enc_gop_t *enc_gop ) ;
void x265_enc_gop_deinit ( x265_t *h, x265_enc_gop_t *enc_gop ) ;
int x265_enc_gop_initialize ( x265_t *h, x265_enc_gop_t *enc_gop ) ;
void x265_enc_gop_deinitialize ( x265_t *h, x265_enc_gop_t *enc_gop ) ;
int x265_enc_gop_create ( x265_t *h, x265_enc_gop_t *enc_gop ) ;
void x265_enc_gop_destroy ( x265_t *h, x265_enc_gop_t *enc_gop ) ;


int x265_enc_gop_encoder_slice( x265_t *h,
								x265_enc_gop_t *enc_gop,
								x265_nal_t **pp_nal, int *pi_nal,
								x265_picture_t *pic_in,
								x265_picture_t *pic_out ) ;

enum nal_unit_type_e x2565_enc_gop_get_nal_unit_type ( x265_t *h, x265_enc_gop_t *enc_gop, int32_t i_poc) ;
void x265_enc_gop_x_write_tile_location_to_slice_header ( x265_t *h,
															bs_t *s,
															x265_output_bitstream_t **ppc_bitstream_redirect,
															x265_slice_t *slice ) ;

double x265_enc_gop_x_calculate_rvm ( x265_t *h, x265_enc_gop_t *enc_gop ) ;
void x265_enc_gop_x_calculate_add_psnr( x265_t *h,
										x265_enc_gop_t *enc_gop,
										int32_t i_frame_size,
										int32_t b_long_start_code ) ;
void x265_enc_gop_print_out_summary( x265_t *h,
									x265_enc_gop_t *enc_gop,
									uint32_t i_num_all_pic_coded ) ;


#endif


