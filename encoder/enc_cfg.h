


#ifndef X265_ENCODER_CFG_H
#define X265_ENCODER_CFG_H


typedef struct
{

} x265_enc_cfg_t ;


x265_enc_cfg_t *x265_enc_cfg_new () ;
void x265_enc_cfg_delete ( x265_enc_cfg_t *enc_cfg ) ;

int x265_enc_cfg_init ( x265_enc_cfg_t *enc_cfg );
void x265_enc_cfg_deinit ( x265_enc_cfg_t *enc_cfg );

#endif

