


#ifndef X265_ENCODER_BIN_CODER_CABAC_COUNTER_H
#define X265_ENCODER_BIN_CODER_CABAC_COUNTER_H


struct _x265_enc_bin_cabac_counter_t
{
	X265_ENC_BIN_IF_FUNC ;
    x265_bit_if_t *bit_if ;
    uint32_t i_bins_coded ;
    int32_t i_bin_count_increment ;
    uint8_t *state ;
    uint32_t *bins_coded ;

    /* state */
    int i_low ;
    int i_range ;

    /* bit stream */
    int i_queue ; //stored with an offset of -8 for faster asm
    int i_num_buffered_bytes ;
    uint32_t i_buffered_byte ;

    uint64_t i_frac_bits;

} ;

x265_enc_bin_if_t *x265_enc_bin_cabac_counter_new () ;
void x265_enc_bin_cabac_counter_delete ( x265_enc_bin_if_t *enc_bin_if ) ;
int x265_enc_bin_cabac_counter_init ( x265_enc_bin_if_t *enc_bin_if ) ;
void x265_enc_bin_cabac_counter_deinit ( x265_enc_bin_if_t *enc_bin_if ) ;

void enc_bin_cabac_counter_init ( x265_enc_bin_if_t *enc_bin_if, x265_bit_if_t *bit_if ) ;
void enc_bin_cabac_counter_uninit ( x265_enc_bin_if_t *enc_bin_if ) ;
void enc_bin_cabac_counter_start ( x265_enc_bin_if_t *enc_bin_if ) ;
void enc_bin_cabac_counter_finish ( x265_enc_bin_if_t *enc_bin_if ) ;
void enc_bin_cabac_counter_copy_state ( x265_enc_bin_if_t *src_enc_bin_if,
										x265_enc_bin_if_t *dst_enc_bin_if ) ;
void enc_bin_cabac_counter_flush ( x265_enc_bin_if_t *enc_bin_if ) ;

void enc_bin_cabac_counter_reset_bac ( x265_enc_bin_if_t *enc_bin_if ) ;
void enc_bin_cabac_counter_encode_pcm_align_bits ( x265_enc_bin_if_t *enc_bin_if ) ;
void enc_bin_cabac_counter_x_write_pcm_code ( x265_enc_bin_if_t *enc_bin_if, uint32_t i_code, uint32_t i_length ) ;

void enc_bin_cabac_counter_reset_bits ( x265_enc_bin_if_t *enc_bin_if ) ;
void enc_bin_cabac_counter_clear_bits ( x265_enc_bin_if_t *enc_bin_if ) ;
uint32_t enc_bin_cabac_counter_get_num_written_bits ( x265_enc_bin_if_t *enc_bin_if ) ;

void enc_bin_cabac_counter_encode_bin ( x265_enc_bin_if_t *enc_bin_if, int32_t b, int32_t i_ctx ) ;
void enc_bin_cabac_counter_encode_bin_ep ( x265_enc_bin_if_t *enc_bin_if, int32_t b ) ;
void enc_bin_cabac_counter_encode_bins_ep ( x265_enc_bin_if_t *enc_bin_if, int32_t i_bin_values, uint32_t i_num_bins ) ;
void enc_bin_cabac_counter_encode_bin_trm ( x265_enc_bin_if_t *enc_bin_if, int32_t b ) ;

x265_enc_bin_cabac_t* enc_bin_cabac_counter_get_enc_bin_cabac ( x265_enc_bin_if_t *enc_bin_if ) ;


void x265_enc_bin_cabac_counter_set_bins_coded ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter, uint32_t i_val ) ;
uint32_t x265_enc_bin_cabac_counter_get_bins_coded ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter ) ;
void x265_enc_bin_cabac_counter_set_bin_counting_enable_flag ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter, int32_t b_flag ) ;
int32_t x265_enc_bin_cabac_counter_get_bin_counting_enable_flag ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter ) ;

int32_t enc_bin_cabac_counter_get_entropy_bits ( x265_enc_bin_if_t *enc_bin_if, int32_t b, int32_t i_ctx ) ;

void enc_bin_cabac_counter_reset_entropy ( x265_t *h,
											x265_enc_bin_if_t *enc_bin_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp ) ;
void enc_bin_cabac_counter_determine_cabac_init_idx ( x265_t *h,
											x265_enc_bin_if_t *enc_bin_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp ) ;
void enc_bin_cabac_counter_update_context_table( x265_enc_bin_if_t *enc_bin_if,
												enum slice_type_e i_slice_type,
												int32_t i_slice_qp ) ;

void enc_bin_cabac_counter_load_intra_dir_mode_luma ( x265_enc_bin_if_t *enc_bin_if,
													x265_enc_bin_if_t *src_enc_bin_if ) ;
void enc_bin_cabac_counter_x_copy_contexts_from ( x265_enc_bin_if_t *enc_bin_if,
													x265_enc_bin_if_t *src_enc_bin_if ) ;

uint32_t enc_bin_cabac_counter_calc_cost ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter,
											int32_t i_slice_qp,
											uint8_t i_init_value,
											int32_t i_index ) ;

void enc_bin_cabac_counter_print_cabac_state ( x265_enc_bin_if_t *enc_bin_if ) ;

void x265_enc_bin_cabac_counter_print_offset () ;



#endif




