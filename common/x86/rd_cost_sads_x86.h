


#ifndef X265_RD_COST_SADS_X86_H
#define X265_RD_COST_SADS_X86_H

#define DECLARE_ONE_PIXEL_N_SADS_FUNCTION(i_rows, i_cols, type, extent) \
uint32_t x265_pixel_sads##type##i_rows##x##i_cols##_##extent(pixel *cur, \
											uint32_t i_stride_cur, \
											pixel *org, \
											uint32_t i_stride_org, \
											int32_t i_bit_depth ) ;

#define DECLARE_ALL_PIXEL_N_SADS_FUNCTION(type,extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 4,  4, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 4,  8, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 4, 12, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 4, 16, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 4, 24, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 4, 32, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 4, 48, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 4, 64, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 8,  4, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 8,  8, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 8, 12, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 8, 16, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 8, 24, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 8, 32, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 8, 48, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION( 8, 64, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(12,  4, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(12,  8, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(12, 12, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(12, 16, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(12, 24, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(12, 32, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(12, 48, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(12, 64, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(16,  4, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(16,  8, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(16, 12, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(16, 16, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(16, 24, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(16, 32, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(16, 48, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(16, 64, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(24,  4, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(24,  8, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(24, 12, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(24, 16, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(24, 24, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(24, 32, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(24, 48, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(24, 64, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(32,  4, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(32,  8, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(32, 12, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(32, 16, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(32, 24, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(32, 32, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(32, 48, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(32, 64, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(48,  4, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(48,  8, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(48, 12, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(48, 16, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(48, 24, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(48, 32, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(48, 48, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(48, 64, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(64,  4, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(64,  8, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(64, 12, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(64, 16, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(64, 24, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(64, 32, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(64, 48, type, extent) \
	DECLARE_ONE_PIXEL_N_SADS_FUNCTION(64, 64, type, extent) \


DECLARE_ALL_PIXEL_N_SADS_FUNCTION(_,ssse3)
DECLARE_ALL_PIXEL_N_SADS_FUNCTION(_half_,ssse3)






#endif



