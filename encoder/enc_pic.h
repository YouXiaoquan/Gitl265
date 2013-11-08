


#ifndef X265_ENCODER_PIC_H
#define X265_ENCODER_PIC_H


typedef struct
{

} x265_enc_qp_adaptation_unit_t ;

typedef struct
{

} x265_enc_pic_qp_adaptation_layer_t ;

typedef struct
{

} x265_enc_pic_t ;


x265_enc_qp_adaptation_unit_t *x265_enc_qp_adaptation_unit_new () ;
void x265_enc_qp_adaptation_unit_delete ( x265_enc_qp_adaptation_unit_t *enc_qp_adaptation_unit ) ;

x265_enc_pic_qp_adaptation_layer_t *x265_enc_pic_qp_adaptation_layer_new () ;
void x265_enc_pic_qp_adaptation_layer_delete ( x265_enc_pic_qp_adaptation_layer_t *enc_pic_qp_adaptation_layer ) ;

x265_enc_pic_t *x265_enc_pic_new () ;
void x265_enc_pic_delete ( x265_enc_pic_t *enc_pic ) ;

int x265_enc_qp_adaptation_unit_init ( x265_enc_qp_adaptation_unit_t *enc_qp_adaptation_unit );
void x265_enc_qp_adaptation_unit_deinit ( x265_enc_qp_adaptation_unit_t *enc_qp_adaptation_unit );

int x265_enc_pic_qp_adaptation_layer_init ( x265_enc_pic_qp_adaptation_layer_t *enc_pic_qp_adaptation_layer );
void x265_enc_pic_qp_adaptation_layer_deinit ( x265_enc_pic_qp_adaptation_layer_t *enc_pic_qp_adaptation_layer );

int x265_enc_pic_init ( x265_enc_pic_t *enc_pic );
void x265_enc_pic_deinit ( x265_enc_pic_t *enc_pic );

#endif

