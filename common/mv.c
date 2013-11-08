

#include "common.h"



x265_mv_t *x265_mv_new ()
{
	x265_mv_t *mv = NULL ;

	CHECKED_MALLOCZERO( mv, sizeof(x265_mv_t) );
	if ( x265_mv_init (mv) )
	{
		goto fail ;
	}

	return mv ;
fail:
	x265_mv_delete ( (void*) mv ) ;
	return NULL ;
}

void x265_mv_delete ( x265_mv_t *mv )
{
	if ( mv )
	{
		x265_mv_deinit ( mv ) ;
		x265_free ( mv ) ;
	}
}

int x265_mv_init ( x265_mv_t *mv )
{
	memset ( mv, 0, sizeof(x265_mv_t) ) ;
    return 0 ;
}

void x265_mv_deinit ( x265_mv_t *mv )
{

}

int x265_mv_is_equal_mv ( x265_mv_t *mv, x265_mv_t *mv_compared )
{
	if ( (mv->i_hor == mv_compared->i_hor)
		&& (mv->i_ver == mv_compared->i_ver) )
	{
		return 1 ;
	}
	return 0 ;
}

void x265_mv_set_zero ( x265_mv_t *mv )
{
	mv->i_hor = 0 ;
	mv->i_ver = 0 ;
}

void x265_mv_scale_mv( x265_mv_t *mv, x265_mv_t *mv_dst, int32_t i_scale )
{
	mv_dst->i_hor = x265_clip3_int32((i_scale * mv->i_hor + 127 + (i_scale * mv->i_hor < 0)) >> 8,
									-32768, 32767) ;
	mv_dst->i_ver = x265_clip3_int32((i_scale * mv->i_ver + 127 + (i_scale * mv->i_ver < 0)) >> 8,
									-32768, 32767) ;
}

void x265_mv_add_to (x265_mv_t *mv, x265_mv_t *mv_src)
{
	mv->i_hor += mv_src->i_hor ;
	mv->i_ver += mv_src->i_ver ;
}

void x265_mv_sub (x265_mv_t *mv, x265_mv_t *mv1, x265_mv_t *mv2)
{
	mv->i_hor = mv1->i_hor - mv2->i_hor;
	mv->i_ver = mv1->i_ver - mv2->i_ver ;
}


void x265_mv_print (const x265_mv_t *mv)
{
	fprintf ( stderr, "( %d, %d )\n", mv->i_hor, mv->i_ver ) ;
}

