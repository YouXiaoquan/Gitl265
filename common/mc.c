

#include "common.h"


void x265_plane_copy_c( pixel *dst, intptr_t i_dst,
                        pixel *src, intptr_t i_src, int w, int h )
{
    while( h-- )
    {
        memcpy( dst, src, w * sizeof(pixel) );
        dst += i_dst;
        src += i_src;
    }
}

void x265_plane_copy_interleave_c( pixel *dst,  intptr_t i_dst,
                                   pixel *srcu, intptr_t i_srcu,
                                   pixel *srcv, intptr_t i_srcv, int w, int h )
{
    for( int y=0; y<h; y++, dst+=i_dst, srcu+=i_srcu, srcv+=i_srcv )
        for( int x=0; x<w; x++ )
        {
            dst[2*x]   = srcu[x];
            dst[2*x+1] = srcv[x];
        }
}

void x265_mc_init( int cpu, x265_mc_functions_t *pf )
{

    pf->plane_copy = x265_plane_copy_c;
    pf->plane_copy_interleave = x265_plane_copy_interleave_c;

}


