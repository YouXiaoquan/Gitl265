

#include "common.h"








void x265_enc_cfg_delete ( x265_enc_cfg_t *enc_cfg )
{
	x265_free ( enc_cfg ) ;
}

x265_enc_cfg_t *x265_enc_cfg_new ()
{
	x265_enc_cfg_t *enc_cfg = NULL ;

	CHECKED_MALLOCZERO( enc_cfg, sizeof(x265_enc_cfg_t) );

	return enc_cfg ;
fail:
	x265_enc_cfg_delete ( (void*) enc_cfg ) ;
	return NULL ;
}

int x265_enc_cfg_init ( x265_enc_cfg_t *enc_cfg )
{

    return 0 ;
}

void x265_enc_cfg_deinit ( x265_enc_cfg_t *enc_cfg )
{

}
