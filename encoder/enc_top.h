



#ifndef X265_ENC_TOP_H
#define X265_ENC_TOP_H


#include "enc_bin_coder.h"
#include "enc_entropy.h"
#include "enc_gop.h"
#include "enc_search.h"
#include "enc_sample_adaptive_offset.h"
#include "enc_slice.h"
#include "enc_cu.h"
#include "enc_preanalyzer.h"
#include "enc_rate_ctrl.h"



void enc_top_set_default_scaling_list ( x265_t *h ) ;
int x265_enc_top_init ( x265_t *h, x265_param_t *param ) ;
void x265_enc_top_deinit ( x265_t *h ) ;
int x265_enc_top_create ( x265_t *h ) ;
void x265_enc_top_destroy ( x265_t *h ) ;
int x265_enc_top_initialize ( x265_t *h ) ;
void x265_enc_top_deinitialize ( x265_t *h ) ;

int create_wpp_coders ( x265_t *h, int32_t i_num_substreams ) ;

void x265_enc_top_print_summary(x265_t *h) ;






#endif


