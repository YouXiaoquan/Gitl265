


#ifndef X265_MV_H
#define X265_MV_H


typedef struct
{
	int16_t i_hor;     ///< horizontal component of motion vector
	int16_t i_ver;     ///< vertical component of motion vector

} x265_mv_t ;


x265_mv_t *x265_mv_new () ;
void x265_mv_delete ( x265_mv_t *mv ) ;
int x265_mv_init ( x265_mv_t *mv );
void x265_mv_deinit ( x265_mv_t *mv );

int x265_mv_is_equal_mv ( x265_mv_t *mv, x265_mv_t *mv_compared ) ;
void x265_mv_set_zero ( x265_mv_t *mv ) ;

void x265_mv_scale_mv( x265_mv_t *mv, x265_mv_t *mv_dst, int32_t i_scale ) ;

void x265_mv_add_to (x265_mv_t *mv, x265_mv_t *mv_src) ;
void x265_mv_sub (x265_mv_t *mv, x265_mv_t *mv1, x265_mv_t *mv2) ;



void x265_mv_print (const x265_mv_t *mv) ;


#endif

