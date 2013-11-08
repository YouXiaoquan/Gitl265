


#ifndef X265_ENCODER_RATE_CTRL_H
#define X265_ENCODER_RATE_CTRL_H


typedef struct
{

} x265_enc_rc_seq_t ;

typedef struct
{

} x265_enc_rc_gop_t ;

typedef struct
{

} x265_enc_rc_pic_t ;

typedef struct
{
	int32_t i_rc_qp ;

} x265_enc_rate_ctrl_t ;


x265_enc_rc_seq_t *x265_enc_rc_seq_new () ;
void x265_enc_rc_seq_delete ( x265_enc_rc_seq_t *enc_rc_seq ) ;

x265_enc_rc_gop_t *x265_enc_rc_gop_new () ;
void x265_enc_rc_gop_delete ( x265_enc_rc_gop_t *enc_rc_gop ) ;

x265_enc_rc_pic_t *x265_enc_rc_pic_new () ;
void x265_enc_rc_pic_delete ( x265_enc_rc_pic_t *enc_rc_pic ) ;

x265_enc_rate_ctrl_t *x265_enc_rate_ctrl_new () ;
void x265_enc_rate_ctrl_delete ( x265_enc_rate_ctrl_t *enc_rate_ctrl ) ;

int x265_enc_rc_seq_init ( x265_enc_rc_seq_t *enc_rc_seq );
void x265_enc_rc_seq_deinit ( x265_enc_rc_seq_t *enc_rc_seq );

int x265_enc_rc_gop_init ( x265_enc_rc_gop_t *enc_rc_gop );
void x265_enc_rc_gop_deinit ( x265_enc_rc_gop_t *enc_rc_gop );

int x265_enc_rc_pic_init ( x265_enc_rc_pic_t *enc_rc_pic );
void x265_enc_rc_pic_deinit ( x265_enc_rc_pic_t *enc_rc_pic );

int x265_enc_rate_ctrl_init ( x265_enc_rate_ctrl_t *enc_rate_ctrl );
void x265_enc_rate_ctrl_deinit ( x265_enc_rate_ctrl_t *enc_rate_ctrl );


#if X265_RATE_CONTROL_LAMBDA_DOMAIN
int x265_enc_rate_ctrl_initialize ( x265_t *h, x265_enc_rate_ctrl_t *enc_rate_ctrl ) ;
void x265_enc_rate_ctrl_deinitialize ( x265_t *h, x265_enc_rate_ctrl_t *enc_rate_ctrl ) ;
#else
int x265_enc_rate_ctrl_create ( x265_t *h, x265_enc_rate_ctrl_t *enc_rate_ctrl ) ;
void x265_enc_rate_ctrl_destroy ( x265_t *h, x265_enc_rate_ctrl_t *enc_rate_ctrl ) ;
#endif

#endif

