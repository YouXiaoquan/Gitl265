


#ifndef X265_MOTION_INFO_H
#define X265_MOTION_INFO_H

#include "common_def.h"

typedef struct _x265_amvp_info_t
{
	x265_mv_t mv_cand[ X265_AMVP_MAX_NUM_CANDS_MEM ] ;  ///< array of motion vector predictor candidates
	int32_t i_n ;                                ///< number of motion vector predictor candidates
} x265_amvp_info_t ;

typedef struct
{
	x265_mv_t mv ;
	int32_t i_ref_idx ;

} x265_mv_field_t ;

typedef struct
{
	x265_mv_t* mv ;
	x265_mv_t* mvd ;
	int8_t* ref_idx ;
	uint32_t i_num_partition ;
	x265_amvp_info_t amvp_info ;

} x265_cu_mv_field_t ;





x265_mv_field_t *x265_mv_field_new () ;
void x265_mv_field_delete ( x265_mv_field_t *mv_field ) ;
int x265_mv_field_init ( x265_mv_field_t *mv_field );
void x265_mv_field_deinit ( x265_mv_field_t *mv_field );

x265_mv_t *x265_mv_field_get_mv ( x265_mv_field_t *mv_field ) ;
void x265_mv_field_set_mv( x265_mv_field_t *mv_field, x265_mv_t *mv ) ;

int32_t x265_mv_field_get_ref_idx ( x265_mv_field_t *mv_field ) ;
void x265_mv_field_set_ref_idx( x265_mv_field_t *mv_field, int32_t i_ref_idx ) ;

void x265_mv_field_set_mv_field( x265_mv_field_t *mv_field,
								x265_mv_t *mv,
								int32_t i_ref_idx ) ;


x265_cu_mv_field_t *x265_cu_mv_field_new () ;
void x265_cu_mv_field_delete ( x265_cu_mv_field_t *cu_mv_field ) ;
int x265_cu_mv_field_init ( x265_cu_mv_field_t *cu_mv_field );
void x265_cu_mv_field_deinit ( x265_cu_mv_field_t *cu_mv_field );
int x265_cu_mv_field_create ( x265_cu_mv_field_t *cu_mv_field, uint32_t i_num_partition ) ;
void x265_cu_mv_field_destroy ( x265_cu_mv_field_t *cu_mv_field ) ;

x265_mv_t *x265_cu_mv_field_get_mv ( x265_cu_mv_field_t *cu_mv_field, int32_t i_idx ) ;
x265_mv_t *x265_cu_mv_field_get_mvd ( x265_cu_mv_field_t *cu_mv_field, int32_t i_idx ) ;
int32_t x265_cu_mv_field_get_ref_idx( x265_cu_mv_field_t *cu_mv_field, int32_t i_idx ) ;



void x265_cu_mv_field_copy_from_cu_mv_field( x265_cu_mv_field_t *cu_mv_field,
											x265_cu_mv_field_t *cu_mv_field_src ) ;
void x265_cu_mv_field_copy_from( x265_cu_mv_field_t *cu_mv_field,
								x265_cu_mv_field_t *cu_mv_field_src,
								int32_t i_num_part_src,
								int32_t i_part_addr_dst ) ;
void x265_cu_mv_field_copy_to_p3 ( x265_cu_mv_field_t *cu_mv_field,
								x265_cu_mv_field_t *cu_mv_field_dst,
								int32_t i_part_addr_dst ) ;
void x265_cu_mv_field_copy_to_p5 ( x265_cu_mv_field_t *cu_mv_field,
								x265_cu_mv_field_t *cu_mv_field_dst,
								int32_t i_part_addr_dst,
								uint32_t ui_offset,
								uint32_t i_num_part ) ;
void x265_cu_mv_field_link_to_with_offset ( x265_cu_mv_field_t *cu_mv_field,
											x265_cu_mv_field_t *cu_mv_field_src,
											int32_t i_offset ) ;
void x265_cu_mv_field_clear_mv_field( x265_cu_mv_field_t *cu_mv_field ) ;

void x265_cu_mv_field_compress ( x265_cu_mv_field_t *cu_mv_field,
								int8_t * pred_mode, int32_t i_scale ) ;



void x265_cu_mv_field_set_all_mv (x265_cu_mv_field_t *cu_mv_field,
									const x265_mv_t *mv,
									enum part_size_e i_part_size,
									int32_t i_part_addr,
									uint32_t i_depth,
									int32_t i_part_idx) ;
void x265_cu_mv_field_set_all_mvd (x265_cu_mv_field_t *cu_mv_field,
									const x265_mv_t *mvd,
									enum part_size_e i_part_size,
									int32_t i_part_addr,
									uint32_t i_depth,
									int32_t i_part_idx) ;
void x265_cu_mv_field_set_all_ref_idx (x265_cu_mv_field_t *cu_mv_field,
									const int8_t i_ref_idx,
									enum part_size_e i_part_size,
									int32_t i_part_addr,
									uint32_t i_depth,
									int32_t i_part_idx) ;


void x265_cu_mv_field_set_all_mv_field_p6(x265_cu_mv_field_t *cu_mv_field,
										x265_mv_field_t *mv_field,
										enum part_size_e i_part_size,
										int32_t i_part_addr,
										uint32_t i_depth,
										int32_t i_part_idx ) ;

void x265_cu_mv_field_set_all_mv_field_p7(x265_cu_mv_field_t *cu_mv_field,
										x265_mv_t *mv,
										int8_t i_ref_idx,
										enum part_size_e i_part_size,
										int32_t i_part_addr,
										uint32_t i_depth,
										int32_t i_part_idx ) ;



#endif

