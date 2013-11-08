

#include "common/common.h"








void x265_enc_rc_seq_delete ( x265_enc_rc_seq_t *enc_rc_seq )
{
	x265_free ( enc_rc_seq ) ;
}

x265_enc_rc_seq_t *x265_enc_rc_seq_new ()
{
	x265_enc_rc_seq_t *enc_rc_seq = NULL ;

	CHECKED_MALLOCZERO( enc_rc_seq, sizeof(x265_enc_rc_seq_t) );

	return enc_rc_seq ;
fail:
	x265_enc_rc_seq_delete ( (void*) enc_rc_seq ) ;
	return NULL ;
}

void x265_enc_rc_gop_delete ( x265_enc_rc_gop_t *enc_rc_gop )
{
	x265_free ( enc_rc_gop ) ;
}

x265_enc_rc_gop_t *x265_enc_rc_gop_new ()
{
	x265_enc_rc_gop_t *enc_rc_gop = NULL ;

	CHECKED_MALLOCZERO( enc_rc_gop, sizeof(x265_enc_rc_gop_t) );

	return enc_rc_gop ;
fail:
	x265_enc_rc_gop_delete ( (void*) enc_rc_gop ) ;
	return NULL ;
}

void x265_enc_rc_pic_delete ( x265_enc_rc_pic_t *enc_rc_pic )
{
	x265_free ( enc_rc_pic ) ;
}

x265_enc_rc_pic_t *x265_enc_rc_pic_new ()
{
	x265_enc_rc_pic_t *enc_rc_pic = NULL ;

	CHECKED_MALLOCZERO( enc_rc_pic, sizeof(x265_enc_rc_pic_t) );

	return enc_rc_pic ;
fail:
	x265_enc_rc_pic_delete ( (void*) enc_rc_pic ) ;
	return NULL ;
}

void x265_enc_rate_ctrl_delete ( x265_enc_rate_ctrl_t *enc_rate_ctrl )
{
	x265_free ( enc_rate_ctrl ) ;
}

x265_enc_rate_ctrl_t *x265_enc_rate_ctrl_new ()
{
	x265_enc_rate_ctrl_t *enc_rate_ctrl = NULL ;

	CHECKED_MALLOCZERO( enc_rate_ctrl, sizeof(x265_enc_rate_ctrl_t) );

	return enc_rate_ctrl ;
fail:
	x265_enc_rate_ctrl_delete ( (void*) enc_rate_ctrl ) ;
	return NULL ;
}

int x265_enc_rc_seq_init ( x265_enc_rc_seq_t *enc_rc_seq )
{

    return 0 ;
}

void x265_enc_rc_seq_deinit ( x265_enc_rc_seq_t *enc_rc_seq )
{

}

int x265_enc_rc_gop_init ( x265_enc_rc_gop_t *enc_rc_gop )
{

    return 0 ;
}

void x265_enc_rc_gop_deinit ( x265_enc_rc_gop_t *enc_rc_gop )
{

}

int x265_enc_rc_pic_init ( x265_enc_rc_pic_t *enc_rc_pic )
{

    return 0 ;
}

void x265_enc_rc_pic_deinit ( x265_enc_rc_pic_t *enc_rc_pic )
{

}

int x265_enc_rate_ctrl_init ( x265_enc_rate_ctrl_t *enc_rate_ctrl )
{

    return 0 ;
}

void x265_enc_rate_ctrl_deinit ( x265_enc_rate_ctrl_t *enc_rate_ctrl )
{

}


#if X265_RATE_CONTROL_LAMBDA_DOMAIN
int x265_enc_rate_ctrl_initialize ( x265_t *h, x265_enc_rate_ctrl_t *enc_rate_ctrl )
{

	return 0 ;


}

void x265_enc_rate_ctrl_deinitialize ( x265_t *h, x265_enc_rate_ctrl_t *enc_rate_ctrl )
{

}

#else

int x265_enc_rate_ctrl_create ( x265_t *h, x265_enc_rate_ctrl_t *enc_rate_ctrl )
{

	return 0 ;


}

void x265_enc_rate_ctrl_destroy ( x265_t *h, x265_enc_rate_ctrl_t *enc_rate_ctrl )
{

}

#endif


