


#ifndef X265_ENCODER_ANALYZE_H
#define X265_ENCODER_ANALYZE_H


typedef struct
{
	uint32_t i_num_pic;
	double f_psnr_sum_y;
	double f_psnr_sum_u;
	double f_psnr_sum_v;
	double f_add_bits;
	double f_frm_rate; //--CFG_KDY

} x265_enc_analyze_t ;


x265_enc_analyze_t *x265_enc_analyze_new () ;
void x265_enc_analyze_delete ( x265_enc_analyze_t *enc_analyze ) ;

int x265_enc_analyze_init ( x265_enc_analyze_t *enc_analyze );
void x265_enc_analyze_deinit ( x265_enc_analyze_t *enc_analyze );

void x265_enc_analyze_add_result(x265_enc_analyze_t *enc_analyze,
								double f_psnr_y,
								double f_psnr_u,
								double f_psnr_v,
								double f_bits ) ;
double x265_enc_analyze_get_psnr_y( x265_enc_analyze_t *enc_analyze ) ;
double x265_enc_analyze_get_psnr_u( x265_enc_analyze_t *enc_analyze ) ;
double x265_enc_analyze_get_psnr_v( x265_enc_analyze_t *enc_analyze ) ;
double x265_enc_analyze_get_bits( x265_enc_analyze_t *enc_analyze ) ;
uint32_t x265_enc_analyze_get_num_pic ( x265_enc_analyze_t *enc_analyze ) ;
void x265_enc_analyze_set_frm_rate ( x265_enc_analyze_t *enc_analyze,
									double f_frame_rate ) ;
void x265_enc_analyze_clear( x265_enc_analyze_t *enc_analyze ) ;
void x265_enc_analyze_print_out ( x265_enc_analyze_t *enc_analyze, char i_delim ) ;
void x265_enc_analyze_print_summary_out ( x265_enc_analyze_t *enc_analyze ) ;
void x265_enc_analyze_print_summary ( x265_enc_analyze_t *enc_analyze, char ch ) ;


#endif

