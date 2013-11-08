


#ifndef X265_SCAN_H
#define X265_SCAN_H


enum coef_scan_type_e
{
	///< up-right diagonal scan
	SCAN_DIAG = 0,
	///< horizontal first scan
	SCAN_HOR,
	///< vertical first scan
	SCAN_VER
} ;

typedef struct
{
	uint32_t **sig_last_scan_8x8 ;
	uint32_t *sig_last_scan_cg_32x32 ;
	uint32_t *sig_last_scan[ 3 ][ X265_MAX_CU_DEPTH ] ;
	uint32_t *zscan_to_raster ;
	uint32_t *raster_to_zscan ;
	uint32_t *raster_to_pel_x ;
	uint32_t *raster_to_pel_y ;

} x265_scan_t ;

int x265_scan_init ( x265_scan_t *scan, x265_t *h ) ;
void x265_scan_deinit ( x265_scan_t *scan ) ;



#endif


