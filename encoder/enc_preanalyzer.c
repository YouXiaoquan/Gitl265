

#include "common/common.h"








void x265_enc_preanalyzer_delete ( x265_enc_preanalyzer_t *enc_preanalyzer )
{
	x265_free ( enc_preanalyzer ) ;
}

x265_enc_preanalyzer_t *x265_enc_preanalyzer_new ()
{
	x265_enc_preanalyzer_t *enc_preanalyzer = NULL ;

	CHECKED_MALLOCZERO( enc_preanalyzer, sizeof(x265_enc_preanalyzer_t) );

	return enc_preanalyzer ;
fail:
	x265_enc_preanalyzer_delete ( (void*) enc_preanalyzer ) ;
	return NULL ;
}

int x265_enc_preanalyzer_init ( x265_enc_preanalyzer_t *enc_preanalyzer )
{

    return 0 ;
}

void x265_enc_preanalyzer_deinit ( x265_enc_preanalyzer_t *enc_preanalyzer )
{

}
