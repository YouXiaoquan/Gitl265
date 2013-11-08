

#include "common/common.h"








void x265_enc_analyze_delete ( x265_enc_analyze_t *enc_analyze )
{
	x265_free ( enc_analyze ) ;
}

x265_enc_analyze_t *x265_enc_analyze_new ()
{
	x265_enc_analyze_t *enc_analyze = NULL ;

	CHECKED_MALLOCZERO( enc_analyze, sizeof(x265_enc_analyze_t) );

	return enc_analyze ;
fail:
	x265_enc_analyze_delete ( (void*) enc_analyze ) ;
	return NULL ;
}

int x265_enc_analyze_init ( x265_enc_analyze_t *enc_analyze )
{
	enc_analyze->i_num_pic = 0 ;
	enc_analyze->f_psnr_sum_y = 0.0 ;
	enc_analyze->f_psnr_sum_u = 0.0 ;
	enc_analyze->f_psnr_sum_v = 0.0 ;
	enc_analyze->f_add_bits = 0.0 ;
	enc_analyze->f_frm_rate = 0.0 ;

    return 0 ;
}

void x265_enc_analyze_deinit ( x265_enc_analyze_t *enc_analyze )
{

}


void x265_enc_analyze_add_result(x265_enc_analyze_t *enc_analyze,
								double f_psnr_y,
								double f_psnr_u,
								double f_psnr_v,
								double f_bits )
{
	enc_analyze->f_psnr_sum_y += f_psnr_y ;
	enc_analyze->f_psnr_sum_u += f_psnr_u ;
	enc_analyze->f_psnr_sum_v += f_psnr_v ;
	enc_analyze->f_add_bits += f_bits ;
	enc_analyze->i_num_pic ++ ;
}

double x265_enc_analyze_get_psnr_y( x265_enc_analyze_t *enc_analyze )
{
	return  enc_analyze->f_psnr_sum_y;
}

double x265_enc_analyze_get_psnr_u( x265_enc_analyze_t *enc_analyze )
{
	return  enc_analyze->f_psnr_sum_u;
}

double x265_enc_analyze_get_psnr_v( x265_enc_analyze_t *enc_analyze )
{
	return  enc_analyze->f_psnr_sum_v;
}

double x265_enc_analyze_get_bits( x265_enc_analyze_t *enc_analyze )
{
	return  enc_analyze->f_add_bits;
}

uint32_t x265_enc_analyze_get_num_pic ( x265_enc_analyze_t *enc_analyze )
{
	return  enc_analyze->i_num_pic;
}

void x265_enc_analyze_set_frm_rate ( x265_enc_analyze_t *enc_analyze,
									double f_frame_rate )
{
	enc_analyze->f_frm_rate = f_frame_rate;
}
//--CFG_KDY

void x265_enc_analyze_clear( x265_enc_analyze_t *enc_analyze )
{
	enc_analyze->i_num_pic = 0 ;
	enc_analyze->f_psnr_sum_y = 0.0 ;
	enc_analyze->f_psnr_sum_u = 0.0 ;
	enc_analyze->f_psnr_sum_v = 0.0 ;
	enc_analyze->f_add_bits = 0.0 ;
	enc_analyze->f_frm_rate = 0.0 ;
}


void x265_enc_analyze_print_out ( x265_enc_analyze_t *enc_analyze, char i_delim )
{
	double f_fps = 0.0;
	double f_scale = 0.0;

	f_fps = enc_analyze->f_frm_rate ;
	//--CFG_KDY
    f_scale = f_fps / 1000 / (double)enc_analyze->i_num_pic;

    printf( "\tTotal Frames |  "   "Bitrate    "  "Y-PSNR    "  "U-PSNR    "  "V-PSNR \n" );
    //printf( "\t------------ "  " ----------"   " -------- "  " -------- "  " --------\n" );
    printf( "\t %8d    %c"          "%12.4lf  "    "%8.4lf  "   "%8.4lf  "    "%8.4lf\n",
    		enc_analyze->i_num_pic, i_delim,
    		enc_analyze->f_add_bits * f_scale,
    		enc_analyze->f_psnr_sum_y / ((double)enc_analyze->i_num_pic),
    		enc_analyze->f_psnr_sum_u / ((double)enc_analyze->i_num_pic),
    		enc_analyze->f_psnr_sum_v / ((double)enc_analyze->i_num_pic));
}

void x265_enc_analyze_print_summary_out ( x265_enc_analyze_t *enc_analyze )
{
	FILE* file = NULL;
	double f_fps = 0.0; //--CFG_KDY
	double f_scale = 0.0;

	file = fopen ("summaryTotal.txt", "at");
	f_fps = enc_analyze->f_frm_rate; //--CFG_KDY
	f_scale   = f_fps / 1000 / ((double)(enc_analyze->i_num_pic));

	fprintf(file, "%f\t %f\t %f\t %f\n", enc_analyze->f_add_bits * f_scale,
    		enc_analyze->f_psnr_sum_y / ((double)enc_analyze->i_num_pic),
    		enc_analyze->f_psnr_sum_u / ((double)enc_analyze->i_num_pic),
    		enc_analyze->f_psnr_sum_v / ((double)enc_analyze->i_num_pic));
	fclose(file);
}

void x265_enc_analyze_print_summary ( x265_enc_analyze_t *enc_analyze, char ch )
{
	FILE *file = NULL;
	double f_fps = 0.0;
	double f_scale = 0.0;

	switch( ch )
	{
	case 'I':
		file = fopen ("summary_I.txt", "at");
		break;
	case 'P':
		file = fopen ("summary_P.txt", "at");
		break;
	case 'B':
		file = fopen ("summary_B.txt", "at");
		break;
	default:
		assert(0);
		return;
		break;
	}

	f_fps = enc_analyze->f_frm_rate; //--CFG_KDY
	f_scale = f_fps / 1000 / ((double)(enc_analyze->i_num_pic));

    fprintf(file, "%f\t %f\t %f\t %f\n",
    		enc_analyze->f_add_bits * f_scale,
    		enc_analyze->f_psnr_sum_y / ((double)enc_analyze->i_num_pic),
    		enc_analyze->f_psnr_sum_u / ((double)enc_analyze->i_num_pic),
    		enc_analyze->f_psnr_sum_v / ((double)enc_analyze->i_num_pic));

    fclose(file);
}


