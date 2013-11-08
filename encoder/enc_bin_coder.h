

#ifndef X265_ENCODER_BIN_CODER_H
#define X265_ENCODER_BIN_CODER_H

#include "common/context_model.h"
#include "common/bitstream.h"


#define CNU                          154      ///< dummy initialization value for unused context models 'Context model Not Used'
#define X265_CABAC_STATE_NUMBER		 184


extern const uint8_t x265_cabac_context_init_I[X265_CABAC_STATE_NUMBER] ;
extern const uint8_t x265_cabac_context_init_PB1[X265_CABAC_STATE_NUMBER] ;
extern const uint8_t x265_cabac_context_init_PB2[X265_CABAC_STATE_NUMBER] ;
extern const int32_t x265_entropy_bits[128] ;
uint8_t x265_cabac_transition[128][2] ;

typedef struct _x265_enc_bin_if_t x265_enc_bin_if_t ;
typedef struct _x265_enc_bin_cabac_t x265_enc_bin_cabac_t ;
typedef struct _x265_enc_bin_cabac_counter_t x265_enc_bin_cabac_counter_t ;

#define X265_ENC_BIN_IF_FUNC \
	void  (*enc_bin_if_delete) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*init) ( x265_enc_bin_if_t *enc_bin_if, x265_bit_if_t* bit_if ) ; \
	void (*uninit) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*start) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*finish) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*copy_state)  ( x265_enc_bin_if_t *src_enc_bin_if, x265_enc_bin_if_t *dst_enc_bin_if ) ; \
	void (*flush) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*reset_bac) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*encode_pcm_align_bits) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*x_write_pcm_code) ( x265_enc_bin_if_t *enc_bin_if, uint32_t i_code, uint32_t i_length ) ; \
	void (*reset_bits) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*clear_bits) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	uint32_t (*get_num_written_bits) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*encode_bin) ( x265_enc_bin_if_t *enc_bin_if, int32_t b, int32_t i_ctx ) ; \
	void (*encode_bin_ep) ( x265_enc_bin_if_t *enc_bin_if, int32_t b ) ; \
	void (*encode_bins_ep) ( x265_enc_bin_if_t *enc_bin_if, int32_t i_bin_values, uint32_t i_num_bins ) ; \
	void (*encode_bin_trm) ( x265_enc_bin_if_t *enc_bin_if, int32_t b ) ; \
	x265_enc_bin_cabac_t* (*get_enc_bin_cabac) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	int32_t (*get_entropy_bits) ( x265_enc_bin_if_t *enc_bin_if, int32_t b, int32_t i_ctx ) ; \
	void (*reset_entropy) ( x265_t *h, \
							x265_enc_bin_if_t *enc_bin_if, \
							enum slice_type_e i_slice_type, \
							int32_t i_slice_qp ) ; \
	void (*print_cabac_state) ( x265_enc_bin_if_t *enc_bin_if ) ; \
	void (*determine_cabac_init_idx) ( x265_t *h, \
										x265_enc_bin_if_t *enc_bin_if, \
										enum slice_type_e i_slice_type, \
										int32_t i_slice_qp ) ; \
	void (*update_context_table) ( x265_enc_bin_if_t *enc_bin_if, \
									enum slice_type_e i_slice_type, \
									int32_t i_slice_qp ) ; \
	void (*load_intra_dir_mode_luma) ( x265_enc_bin_if_t *enc_bin_if, \
									x265_enc_bin_if_t *src_enc_bin_if ) ; \
	void (*x_copy_contexts_from) ( x265_enc_bin_if_t *enc_bin_if, \
									x265_enc_bin_if_t *src_enc_bin_if )

struct _x265_enc_bin_if_t
{
	X265_ENC_BIN_IF_FUNC ;
} ;

#include "enc_bin_coder_cabac.h"
#include "enc_bin_coder_cabac_counter.h"

void x265_enc_bin_if_build_cabac_transition_table () ;
void x265_enc_bin_if_init ( x265_enc_bin_if_t * enc_bin_if ) ;
void x265_enc_bin_if_delete ( x265_enc_bin_if_t * enc_bin_if ) ;
uint8_t x265_enc_bin_if_get_state ( uint8_t i_slice_qp, uint8_t i_init_value ) ;
void x265_enc_bin_if_print_offset () ;


#endif


