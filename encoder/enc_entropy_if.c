

#include "common/common.h"



void x265_enc_entropy_if_delete ( x265_enc_entropy_if_t *enc_entropy_if )
{

}

void x265_enc_entropy_if_init ( x265_enc_entropy_if_t * enc_entropy_if )
{
	memset ( enc_entropy_if, 0, sizeof (x265_enc_entropy_if_t) ) ;

	enc_entropy_if->enc_entropy_if_delete = x265_enc_entropy_if_delete ;
}


