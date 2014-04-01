

#ifndef X265_RD_COST_SSES_X86_H
#define X265_RD_COST_SSES_X86_H

#define DECLARE_ONE_PIXEL_N_SSES_FUNCTION(i_cols, i_rows, src_type, src_type_name, dst_type, dst_type_name, extent) \
uint32_t x265_pixel_sses_##src_type_name##_##dst_type_name##_##i_cols##x##i_rows##_##extent(src_type *cur, \
																							uint32_t i_stride_cur, \
																							dst_type *org, \
																							uint32_t i_stride_org, \
																							int32_t i_bit_depth ) ;


#define DECLARE_ALL_PIXEL_N_SSES_FUNCTION(src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 4,  4, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 4,  8, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 4, 12, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 4, 16, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 4, 24, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 4, 32, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 4, 48, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 4, 64, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 8,  4, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 8,  8, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 8, 12, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 8, 16, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 8, 24, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 8, 32, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 8, 48, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION( 8, 64, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(12,  4, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(12,  8, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(12, 12, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(12, 16, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(12, 24, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(12, 32, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(12, 48, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(12, 64, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(16,  4, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(16,  8, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(16, 12, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(16, 16, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(16, 24, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(16, 32, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(16, 48, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(16, 64, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(24,  4, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(24,  8, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(24, 12, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(24, 16, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(24, 24, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(24, 32, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(24, 48, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(24, 64, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(32,  4, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(32,  8, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(32, 12, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(32, 16, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(32, 24, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(32, 32, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(32, 48, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(32, 64, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(48,  4, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(48,  8, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(48, 12, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(48, 16, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(48, 24, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(48, 32, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(48, 48, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(48, 64, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(64,  4, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(64,  8, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(64, 12, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(64, 16, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(64, 24, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(64, 32, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(64, 48, src_type, src_type_name, dst_type, dst_type_name, extent) \
	DECLARE_ONE_PIXEL_N_SSES_FUNCTION(64, 64, src_type, src_type_name, dst_type, dst_type_name, extent) \


DECLARE_ALL_PIXEL_N_SSES_FUNCTION(pixel,p,pixel,p,ssse3)
DECLARE_ALL_PIXEL_N_SSES_FUNCTION(pixel,p,short_pixel,s,ssse3)
DECLARE_ALL_PIXEL_N_SSES_FUNCTION(short_pixel,s,short_pixel,s,ssse3)








#endif


