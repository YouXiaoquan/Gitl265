#ifndef X265_MC_H
#define X265_MC_H

typedef struct
{

    void (*plane_copy)( pixel *dst, intptr_t i_dst, pixel *src, intptr_t i_src, int w, int h );
    void (*plane_copy_interleave)( pixel *dst,  intptr_t i_dst, pixel *srcu, intptr_t i_srcu,
                                   pixel *srcv, intptr_t i_srcv, int w, int h );


} x265_mc_functions_t;

void x265_mc_init( int cpu, x265_mc_functions_t *pf );

#endif
