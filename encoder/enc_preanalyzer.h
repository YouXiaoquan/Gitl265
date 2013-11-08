


#ifndef X265_ENCODER_PREANALYZER_H
#define X265_ENCODER_PREANALYZER_H


typedef struct
{

} x265_enc_preanalyzer_t ;


x265_enc_preanalyzer_t *x265_enc_preanalyzer_new () ;
void x265_enc_preanalyzer_delete ( x265_enc_preanalyzer_t *enc_preanalyzer ) ;

int x265_enc_preanalyzer_init ( x265_enc_preanalyzer_t *enc_preanalyzer );
void x265_enc_preanalyzer_deinit ( x265_enc_preanalyzer_t *enc_preanalyzer );

#endif

