

#include "common.h"








void x265_enc_qp_adaptation_unit_delete ( x265_enc_qp_adaptation_unit_t *enc_qp_adaptation_unit )
{
	x265_free ( enc_qp_adaptation_unit ) ;
}

x265_enc_qp_adaptation_unit_t *x265_enc_qp_adaptation_unit_new ()
{
	x265_enc_qp_adaptation_unit_t *enc_qp_adaptation_unit = NULL ;

	CHECKED_MALLOCZERO( enc_qp_adaptation_unit, sizeof(x265_enc_qp_adaptation_unit_t) );

	return enc_qp_adaptation_unit ;
fail:
	x265_enc_qp_adaptation_unit_delete ( (void*) enc_qp_adaptation_unit ) ;
	return NULL ;
}

void x265_enc_pic_qp_adaptation_layer_delete ( x265_enc_pic_qp_adaptation_layer_t *enc_pic_qp_adaptation_layer )
{
	x265_free ( enc_pic_qp_adaptation_layer ) ;
}

x265_enc_pic_qp_adaptation_layer_t *x265_enc_pic_qp_adaptation_layer_new ()
{
	x265_enc_pic_qp_adaptation_layer_t *enc_pic_qp_adaptation_layer = NULL ;

	CHECKED_MALLOCZERO( enc_pic_qp_adaptation_layer, sizeof(x265_enc_pic_qp_adaptation_layer_t) );

	return enc_pic_qp_adaptation_layer ;
fail:
	x265_enc_pic_qp_adaptation_layer_delete ( (void*) enc_pic_qp_adaptation_layer ) ;
	return NULL ;
}

void x265_enc_pic_delete ( x265_enc_pic_t *enc_pic )
{
	x265_free ( enc_pic ) ;
}

x265_enc_pic_t *x265_enc_pic_new ()
{
	x265_enc_pic_t *enc_pic = NULL ;

	CHECKED_MALLOCZERO( enc_pic, sizeof(x265_enc_pic_t) );

	return enc_pic ;
fail:
	x265_enc_pic_delete ( (void*) enc_pic ) ;
	return NULL ;
}


int x265_enc_qp_adaptation_unit_init ( x265_enc_qp_adaptation_unit_t *enc_qp_adaptation_unit )
{

    return 0 ;
}

void x265_enc_qp_adaptation_unit_deinit ( x265_enc_qp_adaptation_unit_t *enc_qp_adaptation_unit )
{

}

int x265_enc_pic_qp_adaptation_layer_init ( x265_enc_pic_qp_adaptation_layer_t *enc_pic_qp_adaptation_layer )
{

    return 0 ;
}

void x265_enc_pic_qp_adaptation_layer_deinit ( x265_enc_pic_qp_adaptation_layer_t *enc_pic_qp_adaptation_layer )
{

}

int x265_enc_pic_init ( x265_enc_pic_t *enc_pic )
{

    return 0 ;
}

void x265_enc_pic_deinit ( x265_enc_pic_t *enc_pic )
{

}

