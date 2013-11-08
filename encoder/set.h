

#ifndef X265_ENCODER_SET_H
#define X265_ENCODER_SET_H

void x265_vps_init( x265_vps_t *vps, x265_param_t *param ) ;
void x265_vps_deinit( x265_vps_t *vps ) ;
void x265_vps_write( bs_t *s, x265_vps_t *vps );

int x265_sps_init( x265_sps_t *sps, x265_t *h, x265_param_t *param ) ;
void x265_sps_deinit( x265_sps_t *sps ) ;
void x265_sps_write( x265_t *h, bs_t *s, x265_sps_t *sps );

int x265_pps_init( x265_pps_t *pps, x265_sps_t *sps, x265_t *h, x265_param_t *param ) ;
void x265_pps_deinit( x265_pps_t *pps ) ;
void x265_pps_write( x265_t *h, bs_t *s, x265_pps_t *pps );

void x265_short_term_ref_pic_set_write ( bs_t *s,
										x265_reference_picture_set_t* rps,
										int32_t b_called_from_slice_header,
										int32_t i_idx) ;
#endif

