

#include "common/common.h"
#include "enc_global.h"


void x265_global_deinit ( x265_t *h )
{
	x265_free ( h->global.convert_to_bit ) ;
}

int x265_global_init ( x265_t *h )
{
	int32_t loop = 0, i_bit = 0 ;

    CHECKED_MALLOC( h->global.convert_to_bit, (X265_MAX_CU_SIZE + 1) * sizeof(int8_t) );
    memset ( h->global.convert_to_bit, -1, (X265_MAX_CU_SIZE + 1)* sizeof(int8_t) ) ;

    i_bit = 0 ;
    for ( loop = 4 ; loop < X265_MAX_CU_SIZE ; loop *= 2 )
    {
    	h->global.convert_to_bit[loop] = i_bit ;
    	++ i_bit ;
    }
    h->global.convert_to_bit [loop] = i_bit ;

	return 0 ;

fail:
	x265_global_deinit ( h ) ;
	return -1 ;
}

