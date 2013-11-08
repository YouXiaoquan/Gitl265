

#include <signal.h>
#define _GNU_SOURCE
#include <getopt.h>
#include "common/common.h"
#include "x265cli.h"
#include "frame_input/frame_input.h"
#include "frame_output/frame_output.h"
#include "input_filters/input_filters.h"
#include "output_filters/output_filters.h"
#include "bitstream_output/bitstream_output.h"
#include "common/x86/mb8_x86.h"




#define FAIL_IF_ERROR( cond, ... ) FAIL_IF_ERR( cond, "x265", __VA_ARGS__ )
#ifdef _WIN32
#include <windows.h>
#else
#define GetConsoleTitle(t,n)
#define SetConsoleTitle(t)
#endif

static volatile int b_ctrl_c = 0;
static int          b_exit_on_ctrl_c = 0;
static void sigint_handler( int a )
{
    if( b_exit_on_ctrl_c )
        exit(0);
    b_ctrl_c = 1;
}

#if SYS_WINDOWS
static char UNUSED originalCTitle[200] = "";
#endif

typedef struct
{
    int b_progress;
    int i_seek;
    hnd_t hin;
    hnd_t hout;
    hnd_t hrecon;
    FILE *qpfile;
    FILE *tcfile_out;
    double timebase_convert_multiplier;
    int i_pulldown;
} cli_opt_t;

cli_frame_input_t cli_frame_input;
cli_input_filter_t input_filter ;
cli_frame_output_t cli_frame_output;
cli_output_filter_t output_filter ;
static cli_bitstream_output_t cli_bitstream_output;

static const char * const demuxer_names[] =
{
    "auto",
    "raw",
    0
};

static const char * const muxer_names[] =
{
    "auto",
    "raw",
    0
};

static int setting_default_config_type ( x265_param_t *param, const char *config_type ) ;
static int  parse( int argc, char **argv, x265_param_t *param, cli_opt_t *opt );
static int  encode( x265_param_t *param, cli_opt_t *opt );

/* logging and printing for within the cli system */
static int cli_log_level;
void x265_cli_log( const char *name, int i_level, const char *fmt, ... )
{
    if( i_level > cli_log_level )
        return;
    char *s_level;
    switch( i_level )
    {
        case X265_LOG_ERROR:
            s_level = "error";
            break;
        case X265_LOG_WARNING:
            s_level = "warning";
            break;
        case X265_LOG_INFO:
            s_level = "info";
            break;
        case X265_LOG_DEBUG:
            s_level = "debug";
            break;
        default:
            s_level = "unknown";
            break;
    }
    fprintf( stderr, "%s [%s]: ", name, s_level );
    va_list arg;
    va_start( arg, fmt );
    vfprintf( stderr, fmt, arg );
    va_end( arg );
}

void x265_cli_printf( int i_level, const char *fmt, ... )
{
    if( i_level > cli_log_level )
        return;
    va_list arg;
    va_start( arg, fmt );
    vfprintf( stderr, fmt, arg );
    va_end( arg );
}

void a()
{
	for ( int loop = 0 ; loop < 32 ; ++ loop )
	{
		fprintf ( stderr, "\t\t\t\t\t\t\t\t\tDB\t\t\t" ) ;
		for ( int j = 0 ; j < 3 ; ++ j )
		{
			fprintf ( stderr, "%2d, %2d, ", 32 - loop, loop) ;
		}
		fprintf ( stderr, "%2d, %2d\n", 32 - loop, loop ) ;

		fprintf ( stderr, "\t\t\t\t\t\t\t\t\tDB\t\t\t" ) ;
		for ( int j = 0 ; j < 3 ; ++ j )
		{
			fprintf ( stderr, "%2d, %2d, ", 32 - loop, loop) ;
		}
		fprintf ( stderr, "%2d, %2d\n", 32 - loop, loop ) ;
	}
}

int main( int argc, char **argv )
{
	/*
	int offset = 119951 - (416 * 240);
	int x = offset % 208 ;
	int y = offset / 208 ;

	fprintf ( stderr, "%d %d\n", x, y ) ;
	print_enc_bin_if_offset () ;
	fprintf ( stderr, "\n" ) ;
	print_enc_bin_cabac_offset () ;
	fprintf ( stderr, "\n" ) ;
	print_enc_bin_cabac_counter_offset () ;
	return 0 ;
	*/
	/*
	double f = 0 ;

	f = 0.57 * 0.85 ;
	f *= pow ( 2, 20.0 / 3 ) ;
	fprintf ( stderr, "%f\n", f ) ;
	return 0 ;
	*/
    x265_param_t param;
    cli_opt_t opt = {0};
    int ret = 0;

    reset_timer_status () ;
    open_state_file () ;
    FAIL_IF_ERROR( x265_threading_init(), "unable to initialize threading\n" )

#ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    GetConsoleTitle( originalCTitle, sizeof(originalCTitle) );

    /* Parse command line */
    if( parse( argc, argv, &param, &opt ) < 0 )
    {
        ret = -1;
        fprintf ( stderr, "Parameter Has Error !\n") ;
    }
    else
    {
        fprintf ( stderr, "Parse Success !\n") ;
        //	x265_print_parameters ( &param ) ;
    }


    /* Restore title; it can be changed by input modules */
    SetConsoleTitle( originalCTitle );
    /* Control-C handler */
    signal( SIGINT, sigint_handler );

    double dResult;
    long lBefore = clock();

    if( !ret )
    {
    	ret = encode( &param, &opt );
    }
    dResult = (double)(clock()-lBefore) / CLOCKS_PER_SEC;
    printf("\n Total Time: %12.3f sec.\n", dResult);

    if( input_filter.free )
    {
    	input_filter.free( opt.hin );
    }
    else if( opt.hin )
    {
        cli_frame_input.close_file( opt.hin );
    }
    if ( param.psz_dump_yuv )
    {
		if( output_filter.free )
		{
			output_filter.free( opt.hrecon );
		}
		else if( opt.hrecon )
		{
			cli_frame_output.close_file( opt.hrecon );
		}
    }

    if( opt.hout )
    {
        cli_bitstream_output.close_file( opt.hout, 0, 0 );
    }

    if( opt.tcfile_out )
    {
        fclose( opt.tcfile_out );
    }
    if( opt.qpfile )
    {
        fclose( opt.qpfile );
    }
    SetConsoleTitle( originalCTitle );

    x265_param_destroy ( &param ) ;


    fprintf ( stderr, "X265 Encoder Exit.\n") ;

    print_timer_status () ;
    return ret;
}

static void help( x265_param_t *defaults )
{
#define H0 printf
    H0( "x265 core:%d%s\n"
    	"Syntax: x265 [options] -o outfile infile\n"
    	"\n"
    	"Infile should be raw (in which case resolution is required).\n"
		"Outfile type is selected by filename:\n"
		" .bin -> Raw bytestream\n"
		"\n"
		"Options:\n"
		"\n"
		"  -h, help                                  List all options\n"
		"  -c                                        Configuration file name\n"
		"\n"
		"Rate Distoration Optimizatoion:\n"
		"\n"
		"      lambda-modifier0                      Lambda modifier for temporal layer 0\n"
		"      lambda-modifier1                      Lambda modifier for temporal layer 1\n"
		"      lambda-modifier2                      Lambda modifier for temporal layer 2\n"
		"      lambda-modifier3                      Lambda modifier for temporal layer 3\n"
		"      lambda-modifier4                      Lambda modifier for temporal layer 4\n"
		"      lambda-modifier5                      Lambda modifier for temporal layer 5\n"
		"      lambda-modifier6                      Lambda modifier for temporal layer 6\n"
		"      lambda-modifier7                      Lambda modifier for temporal layer 7\n"
		"      max-delta-qp                          max dQp offset for block\n"
		"      delta-qp-rd                           max dQp offset for slice\n"
		"      max-cu-dqp-depth                      max depth for a minimum CuDQP\n"
		"      rdo-q                                 flag for using RD optimized quantization\n"
		"      rdo-q-ts                              flag for using RD optimized quantization for transform skip\n"
#if X265_L0232_RD_PENALTY
		"      rd-penalty                            RD-penalty for 32x32 TU for intra in non-intra slices.\n"
		"                                            0:disbaled\n"
		"                                            1:RD-penalty\n"
		"                                            2:maximum RD-penalty\n"
#endif
#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
		"\n"
		"Video Parameter Set:\n"
		"      bit-rate-pic-rate-max-t-layers        Maximum number of sub-layers signalled; can be inferred otherwise; here for easy parsing of config. file\n"
		"      bit-rate-info-present                 Control signalling of bit rate information of avg. bit rate and max. bit rate in VPS\n"
		"                                            0: Do not sent bit rate info\n"
		"                                            N (N > 0): Send bit rate info for N sub-layers. N should equal maxTempLayers.\n"
		"      pic-rate-info-present                 Control signalling of picture rate information of avg. bit rate and max. bit rate in VPS\n"
		"                                            0: Do not sent picture rate info\n"
		"                                            N (N > 0): Send picture rate info for N sub-layers. N should equal maxTempLayers.\n"
		"      avg-bit-rate                          List of avg. bit rates for the different sub-layers; include non-negative number even if corresponding flag is 0\n"
		"      max-bit-rate                          List of max. bit rates for the different sub-layers; include non-negative number even if corresponding flag is 0\n"
		"      avg-pic-rate                          List of avg. picture rates for the different sub-layers; include non-negative number even if corresponding flag is 0\n"
		"      constant-pic-rate-idc                 List of constant picture rate IDCs; include non-negative number even if corresponding flag is 0\n"
#endif
		"\n"
		"Sequence Parameter Set:\n"
		"\n"
		"      conformance-mode                      Window conformance mode (0: no window,\n"
		"                                                                     1:automatic padding,\n"
		"                                                                     2:padding,\n"
		"                                                                     3:conformance\n"
		"      conf-left                             Left offset for window conformance mode 3\n"
		"      conf-right                            Right offset for window conformance mode 3\n"
		"      conf-top                              Top offset for window conformance mode 3\n"
		"      conf-bottom                           Bottom offset for window conformance mode 3\n"
		"      profile                               Profile to be used when encoding (Incomplete)\n"
		"      level                                 Level limit to be used, eg 5.1 (-incomplete)\n"
		"      tier                                  Tier to use for interpretation of --Level\n"
#if X265_L0046_CONSTRAINT_FLAGS
		"      progressive-source                    Indicate that source is progressive\n"
		"      interlaced-source                     Indicate that source is interlaced\n"
		"      non-packedSource                      Indicate that source does not contain frame packing\n"
		"      frame-only                            Indicate that the bitstream contains only frames\n"
#endif
		"      max-cu-width                          Maximum CU width\n"
		"      max-cu-height                         Maximum CU height\n"
		"      max-cu-size                           Maximum CU size\n"
		"      max-partitionDepth                    CU depth\n"
		"      quadtree-tu-log2-max-size             Maximum TU size in logarithm base 2\n"
		"      quadtree-tU-log2-min-size             Minimum TU size in logarithm base 2\n"
		"      quadtree-tu-max-depth-intra           Depth of TU tree for intra CUs\n"
		"      quadtree-tu-max-depth-inter           Depth of TU tree for inter CUs\n"
		"      internal-bit-depth                    Bit-depth the codec operates at. (default:input-bit-depth)\n"
		"                                            If different to input-bit-depth, source data will be converted\n"
		"      internal-bit-depthc                   As per internal-bit-depth but for chroma component. (default:intrenal-bit-depth)\n"
		"      lossless-cu-enable                    flag for using lossless coding\n"
		"      sao                                   Enable Sample Adaptive Offset\n"
		"      loop-filter-disable                   flag for using deblocking filter\n"
		"      pcm-enabled-flag                      flag for using IPCM\n"
		"      tmvp-mode                             TMVP mode 0: TMVP disable for all slices.\n"
		"                                                      1: TMVP enable for all slices (default)\n"
		"                                                      2: TMVP enable for certain slices only\n"
		"      scaling-list                          0: no scaling list,\n"
		"                                            1: default scaling lists,\n"
		"                                            2: scaling lists specified in cqmfile\n"
        "      cqmfile                               File that contains HM-compatible scaling lists\n"
		"      strong-intra-smoothing                Enable strong intra smoothing for 32x32 block\n"
		"      vui-parameters-present                Enable generation of vui_parameters()\n"
		"\n"
		"Picture Parameter Set:\n"
		"\n"
		"      uniform-spacing-idc                   Indicates if the column and row boundaries are distributed uniformly\n"
		"      num-tile-columns-minus1               Number of columns in a picture minus 1\n"
		"      column-width-array                    Array containing ColumnWidth values in units of LCU\n"
		"      num-tile-rows-minus1                  Number of rows in a picture minus 1\n"
		"      row-height-array                      Array containing RowHeight values in units of LCU\n"
		"      wave-front-synchro                    0: no synchro; 1 synchro with TR; 2 TRR etc\n"
		"      weighted-pred-p                       Use weighted prediction in P slices\n"
		"      weighted-pred-b                       Use weighted (bidirectional) prediction in B slices\n"
		"      log2-parallel-merge-level             Parallel merge estimation region\n"
		"      transquant-bypass-enable-flag         transquant_bypass_enable_flag indicator in PPS\n"
		"\n"
		"GOP:\n"
		"\n"
		"      intra-period                          Intra period in frames, (-1: only first frame)\n"
		"      decoding-refreshType                  Intra refresh type (0:none 1:CRA 2:IDR)\n"
		"      gop-size                              GOP size of temporal structure\n"
		"\n"
		"Sample Adaptive Offset:\n"
		"\n"
		"      max-num-offsets-per-pic               Max number of SAO offset per picture (Default: 2048)\n"
		"      sao-lcu-boundary                      0: right/bottom LCU boundary areas skipped from SAO parameter estimation,\n"
		"                                            1: non-deblocked pixels are used for those areas\n"
		"      sao-lcu-based-optimization            0: SAO picture-based optimization,\n"
		"                                            1: SAO LCU-based optimization\n"
		"\n"
		"Deblocking Filter:\n"
		"\n"
		"      loop-filter-offset-in-pps             offset for deblocking filter in 0 = slice header, 1 = PPS\n"
		"      loop-filter-beta-offset-div2          beta offset for deblocking filter\n"
		"      loop-filter-tc-offset-div2            tc offset for deblocking filter\n"
		"      deblocking-filter-control-present     deblocking filter control present flag in PPS\n"
		"      lf-cross-slice-boundary-flag          1: cross-slice-boundary loop filtering. 0:non-cross-slice-boundary loop filtering\n"
		"      lf-cross-tile-boundary-flag           1: cross-tile-boundary loop filtering. 0:non-cross-tile-boundary loop filtering\n"
		"\n"
		"IPCM:\n"
		"\n"
		"      pcm-log2-max-size                     log2 of maximum PCM block size\n"
		"      pcm-log2-min-size                     log2 of minimum PCM block size\n"
		"      pcm-filter-disable-flag               PCM filter disable flag\n"
		"\n"
		"Supplemental Enhancement Information:\n"
		"\n"
		"      sei-decoded-picture-hash              Control generation of decode picture hash SEI messages\n"
		"                                            0: disable\n"
		"                                            1: use MD5\n"
		"                                            2: CRC\n"
		"                                            3: checksum\n"
		"      sei-picture-digest                    deprecated alias for SEIDecodedPictureHash\n"
		"      sei-recovery-point                    Control generation of recovery point SEI messages\n"
		"      sei-buffering-period                  Control generation of buffering period SEI messages\n"
		"      sei-picture-timing                    Control generation of picture timing SEI messages\n"
		"      sei-frame-packing                     Control generation of frame packing SEI messages\n"
		"      sei-frame-packing-type                Define frame packing arrangement\n"
		"                                            0: checkerboard - pixels alternatively represent either frames\n"
		"                                            1: column alternation - frames are interlaced by column\n"
		"                                            2: row alternation - frames are interlaced by row\n"
		"                                            3: side by side - frames are displayed horizontally\n"
		"                                            4: top bottom - frames are displayed vertically\n"
		"                                            5: frame alternation - one frame is alternated with the other\n"
		"      sei-frame-packing-id                  Id of frame packing SEI message for a given session\n"
		"      sei-frame-packing-quincunx            Indicate the presence of a Quincunx type video frame\n"
		"      sei-frame-packing-interpretation      Indicate the interpretation of the frame pair\n"
		"                                            0: unspecified\n"
		"                                            1: stereo pair, frame0 represents left view\n"
		"                                            2: stereo pair, frame0 represents right view\n"
		"      sei-display-orientation               Control generation of display orientation SEI messages\n"
		"                                            n: 0 < -n < (2^16 - 1) enable display orientation SEI message with anticlockwise_rotation = N and display_orientation_repetition_period = 1\n"
		"                                            0: disable\n"
		"      sei-temporal-level0-index             Control generation of temporal level 0 index SEI messages\n"
		"      sei-gradual-decoding-refresh-info     Control generation of gradual decoding refresh information SEI message\n"
		"      sei-decoding-unit-info                Control generation of decoding unit information SEI message.\n"
		"      sei-active-parameter-sets             Enable generation of active parameter sets SEI messages\n"
#if X265_RATE_CONTROL_LAMBDA_DOMAIN
		"\n"
		"Rate Control:\n"
		"\n"
		"      rate-control                          Rate control: enable rate control\n"
		"      target-bitrate                        Rate control: target bitrate\n"
		"      keep-hierarchical-bit                 Rate control: keep hierarchical bit allocation in rate control algorithm\n"
		"      lcu-level-rate-control                Rate control: true: LCU level RC; false: picture level RC\n"
		"      rc-luc-separate-model                 Rate control: use LCU level separate R-lambda model\n"
		"      initial-qp                            Rate control: initial QP\n"
		"      rc-force-intra-qp                     Rate control: force intra QP to be equal to initial QP\n"        "\n"
#endif
		"\n"
		"Video Usability Information:\n"
		"\n"
		"      aspect-ratio-info-present             Signals whether aspect_ratio_idc is present\n"
		"      aspect-ratio-idc                      aspect_ratio_idc\n"
		"      sar-width                             Horizontal size of the sample aspect ratio\n"
		"      sar-height                            Vertical size of the sample aspect ratio\n"
		"      overscan-info-present                 Indicates whether conformant decoded pictures are suitable for display using overscan\n"
		"      overscan-appropriate                  Indicates whether conformant decoded pictures are suitable for display using overscan\n"
		"      video-signal-type-present             Signals whether video_format, video_full_range_flag, and colour_description_present_flag are present\n"
		"      video-format                          Indicates representation of pictures\n"
		"      video-full-range                      Indicates the black level and range of luma and chroma signals\n"
		"      colour-description-present            Signals whether colour_primaries, transfer_characteristics and matrix_coefficients are present\n"
		"      colour-primaries                      Indicates chromaticity coordinates of the source primaries\n"
		"      transfer-charateristics               Indicates the opto-electronic transfer characteristics of the source\n"
		"      matrix-coefficients                   Describes the matrix coefficients used in deriving luma and chroma from RGB primaries\n"
		"      chroma-loc-info-present               Signals whether chroma_sample_loc_type_top_field and chroma_sample_loc_type_bottom_field are present\n"
		"      chroma-sample-loc-type-top-field      Specifies the location of chroma samples for top field\n"
		"      chroma-sample-loc-type-bottom-field   Specifies the location of chroma samples for bottom field\n"
		"      neutral-chroma-indication             Indicates that the value of all decoded chroma samples is equal to 1<<(BitDepthCr-1)\n"
		"      default-display-windowFlag            Indicates the presence of the Default Window parameters\n"
		"      def-disp-win-left-offset              Specifies the left offset of the default display window from the conformance window\n"
		"      def-disp-win-right-offset             Specifies the right offset of the default display window from the conformance window\n"
		"      def-disp-win-top-offset               Specifies the top offset of the default display window from the conformance window\n"
		"      def-disp-win-bottom-offset            Specifies the bottom offset of the default display window from the conformance window\n"
		"      frame-field-info-present-flag         Indicates that pic_struct and field coding related values are present in picture timing SEI messages\n"
		"      poc-proportional-to-timing-flag       Indicates that the POC value is proportional to the output time w.r.t. first picture in CVS\n"
		"      num-ticks-poc-diff-one-minus1         Number of ticks minus 1 that for a POC difference of one\n"
		"      bitstream-restriction                 Signals whether bitstream restriction parameters are present\n"
		"      tiles-fixed-structure                 Indicates that each active picture parameter set has the same values of the syntax elements related to tiles\n"
		"      motion-vectors-over-pic-boundaries    Indicates that no samples outside the picture boundaries are used for inter prediction\n"
		"      max-bytes-per-pic-denom               Indicates a number of bytes not exceeded by the sum of the sizes of the VCL NAL units associated with any coded picture\n"
		"      max-bits-per-min-cu-denom             Indicates an upper bound for the number of bits of coding_unit() data\n"
		"      log2-max-mv-length-horizontal         Indicate the maximum absolute value of a decoded horizontal MV component in quarter-pel luma units\n"
		"      log2-max-mv-length-vertical           Indicate the maximum absolute value of a decoded vertical MV component in quarter-pel luma units\n"
		"\n"
		"Other:\n"
		"\n"
		"      frame-rate                            Frame rate\n"
		"      frame-skip                            Number of frames to skip at start of input YUV\n"
		"      frames-to-be-encoded                  Number of frames to be encoded (default=all)\n"
		"      horizontal-padding                    Horizontal source padding for conformance window mode 2\n"
		"      vertical-padding                      Vertical source padding for conformance window mode 2\n"
		"      list-combination                      Combined reference list for uni-prediction estimation in B-slices\n"
		"      transform-skip                        Intra transform skipping\n"
		"      transform-skipFast                    Fast intra transform skipping\n"
		"      amp                                   Enable asymmetric motion partitions\n"
		"      qp                                    Qp value, if value is float, QP is switched once during encoding\n"
		"      cb-qp-offset                          Chroma Cb QP Offset\n"
		"      cr-qp-offset                          Chroma Cr QP Offset\n"

#if X265_ADAPTIVE_QP_SELECTION
		"      adaptive-qp-selection                 AdaptiveQpSelection\n"
#endif

		"      adaptive-qp                           QP adaptation based on a psycho-visual model\n"
		"      max-qp-adaptation-range               QP adaptation range\n"
    	"      input-bit-depth                       Bit-depth of input file\n"
		"      output-bit-depth                      Bit-depth of output file (default:internal-bit-depth)\n"
		"      input-bit-depthc                      As per input-bit-depth but for chroma component. (default:input-bit-depth)\n"
		"      output-bit-depthc                     As per output-bit-depth but for chroma component. (default:internal-bit-depthc)\n"
		"      pcm_input_bit_depth_flag              0: PCM bit-depth is internal bit-depth.\n"
		"                                            1: PCM bit-depth is input bit-depth.\n"
		"      sabc-rd                               SBAC based RD estimation\n"
		"      asr                                   Adaptive motion search range\n"
		"      hadamard-me                           Hadamard ME for fractional-pel\n"
		"      fast-search                           0:Full search  1:Diamond  2:PMVFAST\n"
		"      search-range                          Motion search range\n"
		"      bipred-search-range                   Motion search range for bipred refinement\n"
		"      fen                                   fast encoder setting\n"
		"      ecu                                   Early CU setting\n"
		"      fdm                                   Fast decision for Merge RD Cost\n"
		"      cfm                                   Cbf fast mode setting\n"
		"      esd                                   Early SKIP detection setting\n"
		"      slice-mode                            0: Disable all Recon slice limits,\n"
		"                                            1: Enforce max # of LCUs,\n"
		"                                            2: Enforce max # of bytes,\n"
		"                                            3:specify tiles per dependent slice\n"
		"      slice-argument                        Depending on SliceMode being:\n"
		"                                            1: max number of CTUs per slice\n"
		"                                            2: max number of bytes per slice\n"
		"                                            3: max number of tiles per slice\n"
		"      slice-segment-mode                    0: Disable all slice segment limits,\n"
		"                                            1: Enforce max # of LCUs,\n"
		"                                            2: Enforce max # of bytes,\n"
		"                                            3:specify tiles per dependent slice\n"
		"      slice-segment-argument                Depending on SliceSegmentMode being:\n"
		"                                            1: max number of CTUs per slice segment\n"
		"                                            2: max number of bytes per slice segment\n"
		"                                            3: max number of tiles per slice segment\n"
		"      constrained-intra-pred                Constrained Intra Prediction\n"
		"      max-num-merge-cand                    Maximum number of merge candidates\n"
#if !X265_RATE_CONTROL_LAMBDA_DOMAIN
		"      rate-ctrl                             Rate control on/off\n"
		"      target-bitrate                        Input target bitrate\n"
		"      num-lCU-in-unit                       Number of LCUs in an Unit\n"
#endif
		"      cu-transquant-bypass-flag-value       Fixed cu_transquant_bypass_flag value, when transquant_bypass_enable_flag is enabled\n"
		"      recalculate-qp-according-to-lambda    Recalculate QP values according to lambda values. Do not suggest to be enabled in all intra case\n"
		"\n",
		X265_BUILD, X265_VERSION ) ;


}

typedef enum
{
    OPT_CONFIG_TYPE = 256,
} OptionsOPT ;

static char short_options[] = "c:h:o:";
static struct option long_options[] =
{
    { "help", no_argument, NULL, 'h' },
	{ "output", required_argument, NULL, 'o' },
	{ "config-file", required_argument, NULL, 'c' },
	{ "config-type", required_argument, NULL, OPT_CONFIG_TYPE },
	{ "dump-yuv", required_argument, NULL, 0 },
    { "cqmfile", required_argument, NULL, 0 },
	{ "lambda-modifier0", required_argument, NULL, 0 },
	{ "lambda-modifier1", required_argument, NULL, 0 },
	{ "lambda-modifier2", required_argument, NULL, 0 },
	{ "lambda-modifier3", required_argument, NULL, 0 },
	{ "lambda-modifier4", required_argument, NULL, 0 },
	{ "lambda-modifier5", required_argument, NULL, 0 },
	{ "lambda-modifier6", required_argument, NULL, 0 },
	{ "lambda-modifier7", required_argument, NULL, 0 },
	{ "frame-rate", required_argument, NULL, 0 },
	{ "frame-skip", required_argument, NULL, 0 },
	{ "conformance-mode", required_argument, NULL, 0 },
	{ "frames-to-be-encoded", required_argument, NULL, 0 },
	{ "conf-left", required_argument, NULL, 0 },
	{ "conf-right", required_argument, NULL, 0 },
	{ "conf-top", required_argument, NULL, 0 },
	{ "conf-bottom", required_argument, NULL, 0 },
	{ "horizontal-padding", required_argument, NULL, 0 },
	{ "vertical-padding", required_argument, NULL, 0 },
	{ "profile", required_argument, NULL, 0 },
	{ "level", required_argument, NULL, 0 },
	{ "tier", required_argument, NULL, 0 },

#if X265_L0046_CONSTRAINT_FLAGS
	{ "progressive-source", required_argument, NULL, 0 },
	{ "interlaced-source", required_argument, NULL, 0 },
	{ "non-packed-source", required_argument, NULL, 0 },
	{ "frame-only", required_argument, NULL, 0 },
#endif

	{ "intra-period", required_argument, NULL, 0 },
	{ "decoding-refresh-type", required_argument, NULL, 0 },
	{ "gop-size", required_argument, NULL, 0 },
	{ "frame1", required_argument, NULL, 0 },
	{ "frame2", required_argument, NULL, 0 },
	{ "frame3", required_argument, NULL, 0 },
	{ "frame4", required_argument, NULL, 0 },
	{ "frame5", required_argument, NULL, 0 },
	{ "frame6", required_argument, NULL, 0 },
	{ "frame7", required_argument, NULL, 0 },
	{ "frame8", required_argument, NULL, 0 },
	{ "frame9", required_argument, NULL, 0 },
	{ "frame10", required_argument, NULL, 0 },
	{ "frame11", required_argument, NULL, 0 },
	{ "frame12", required_argument, NULL, 0 },
	{ "frame13", required_argument, NULL, 0 },
	{ "frame14", required_argument, NULL, 0 },
	{ "frame15", required_argument, NULL, 0 },
	{ "frame16", required_argument, NULL, 0 },
	{ "frame17", required_argument, NULL, 0 },
	{ "frame18", required_argument, NULL, 0 },
	{ "frame19", required_argument, NULL, 0 },
	{ "frame20", required_argument, NULL, 0 },
	{ "frame21", required_argument, NULL, 0 },
	{ "frame22", required_argument, NULL, 0 },
	{ "frame23", required_argument, NULL, 0 },
	{ "frame24", required_argument, NULL, 0 },
	{ "frame25", required_argument, NULL, 0 },
	{ "frame26", required_argument, NULL, 0 },
	{ "frame27", required_argument, NULL, 0 },
	{ "frame28", required_argument, NULL, 0 },
	{ "frame29", required_argument, NULL, 0 },
	{ "frame30", required_argument, NULL, 0 },
	{ "frame31", required_argument, NULL, 0 },
	{ "frame32", required_argument, NULL, 0 },
	{ "frame33", required_argument, NULL, 0 },
	{ "frame34", required_argument, NULL, 0 },
	{ "frame35", required_argument, NULL, 0 },
	{ "frame36", required_argument, NULL, 0 },
	{ "frame37", required_argument, NULL, 0 },
	{ "frame38", required_argument, NULL, 0 },
	{ "frame39", required_argument, NULL, 0 },
	{ "frame40", required_argument, NULL, 0 },
	{ "frame41", required_argument, NULL, 0 },
	{ "frame42", required_argument, NULL, 0 },
	{ "frame43", required_argument, NULL, 0 },
	{ "frame44", required_argument, NULL, 0 },
	{ "frame45", required_argument, NULL, 0 },
	{ "frame46", required_argument, NULL, 0 },
	{ "frame47", required_argument, NULL, 0 },
	{ "frame48", required_argument, NULL, 0 },
	{ "frame49", required_argument, NULL, 0 },
	{ "frame50", required_argument, NULL, 0 },
	{ "frame51", required_argument, NULL, 0 },
	{ "frame52", required_argument, NULL, 0 },
	{ "frame53", required_argument, NULL, 0 },
	{ "frame54", required_argument, NULL, 0 },
	{ "frame55", required_argument, NULL, 0 },
	{ "frame56", required_argument, NULL, 0 },
	{ "frame57", required_argument, NULL, 0 },
	{ "frame58", required_argument, NULL, 0 },
	{ "frame59", required_argument, NULL, 0 },
	{ "frame60", required_argument, NULL, 0 },
	{ "frame61", required_argument, NULL, 0 },
	{ "frame62", required_argument, NULL, 0 },
	{ "frame63", required_argument, NULL, 0 },
	{ "frame64", required_argument, NULL, 0 },
	{ "list-combination", required_argument, NULL, 0 },
	{ "transform-skip", required_argument, NULL, 0 },
	{ "transform-skip-fast", required_argument, NULL, 0 },
	{ "amp", required_argument, NULL, 0 },
	{ "qp", required_argument, NULL, 0 },
	{ "max-delta-qp", required_argument, NULL, 0 },
	{ "delta-qp-rd", required_argument, NULL, 0 },
	{ "max-cu-dqp-depth", required_argument, NULL, 0 },
	{ "cb-qp-offset", required_argument, NULL, 0 },
	{ "cr-qp-offset", required_argument, NULL, 0 },
#if X265_ADAPTIVE_QP_SELECTION
	{ "adaptive-qp-selection", required_argument, NULL, 0 },
#endif
	{ "adaptive-qp", required_argument, NULL, 0 },
	{ "max-qp-adaptation-range", required_argument, NULL, 0 },
	{ "max-cu-width", required_argument, NULL, 0 },
	{ "max-cu-height", required_argument, NULL, 0 },
	{ "max-cu-size", required_argument, NULL, 0 },
	{ "max-partition-depth", required_argument, NULL, 0 },
	{ "quadtree-tu-log2-max-size", required_argument, NULL, 0 },
	{ "quadtree-tu-log2-min-size", required_argument, NULL, 0 },
	{ "quadtree-tu-max-depth-intra", required_argument, NULL, 0 },
	{ "quadtree-tu-max-depth-inter", required_argument, NULL, 0 },
	{ "input-bit-depth", required_argument, NULL, 0 },
	{ "output-bit-depth", required_argument, NULL, 0 },
	{ "internal-bit-depth", required_argument, NULL, 0 },
	{ "input-bit-depthc", required_argument, NULL, 0 },
	{ "output-bit-depthc", required_argument, NULL, 0 },
	{ "internal-bit-depthc", required_argument, NULL, 0 },
	{ "pcm-input-bit-depth-flag", required_argument, NULL, 0 },
	{ "lossless-cu-enabled", required_argument, NULL, 0 },
	{ "sao", required_argument, NULL, 0 },
	{ "max-num-offsets-per-pic", required_argument, NULL, 0 },
	{ "sao-lcu-boundary", required_argument, NULL, 0 },
	{ "sao-lcu-based-optimization", required_argument, NULL, 0 },
	{ "loop-filter-disable", required_argument, NULL, 0 },
	{ "loop-filter-offset-in-pps", required_argument, NULL, 0 },
	{ "loop-filter-beta-offset-div2", required_argument, NULL, 0 },
	{ "loop-filter-tc-offset-div2", required_argument, NULL, 0 },
	{ "deblocking-filter-control-present", required_argument, NULL, 0 },
	{ "pcm-enabled-flag", required_argument, NULL, 0 },
	{ "pcm-log2-max-size", required_argument, NULL, 0 },
	{ "pcm-log2-min-size", required_argument, NULL, 0 },
	{ "pcm-filter-disable-flag", required_argument, NULL, 0 },
	{ "sbac-rd", required_argument, NULL, 0 },
	{ "asr", required_argument, NULL, 0 },
	{ "hadamard-me", required_argument, NULL, 0 },
	{ "rdo-q", required_argument, NULL, 0 },
	{ "rdo-q-ts", required_argument, NULL, 0 },

#if X265_L0232_RD_PENALTY
	{ "rd-penalty", required_argument, NULL, 0 },
#endif
	{ "fast-search", required_argument, NULL, 0 },
	{ "search-range", required_argument, NULL, 0 },
	{ "bipred-search-range", required_argument, NULL, 0 },
	{ "fen", required_argument, NULL, 0 },
	{ "ecu", required_argument, NULL, 0 },
	{ "fdm", required_argument, NULL, 0 },
	{ "cfm", required_argument, NULL, 0 },
	{ "esd", required_argument, NULL, 0 },
	{ "slice-mode", required_argument, NULL, 0 },
	{ "slice-argument", required_argument, NULL, 0 },
	{ "slice-segment-mode", required_argument, NULL, 0 },
	{ "slice-segment-argument", required_argument, NULL, 0 },
	{ "lf-cross-slice-boundary-flag", required_argument, NULL, 0 },
	{ "lf-cross-tile-boundary-flag", required_argument, NULL, 0 },
	{ "uniform-spacing-idc", required_argument, NULL, 0 },
	{ "num-tile-columns-minus1", required_argument, NULL, 0 },
	{ "column-width-array", required_argument, NULL, 0 },
	{ "num-tile-rows-minus1", required_argument, NULL, 0 },
	{ "row-height-array", required_argument, NULL, 0 },
	{ "wave-front-synchro", required_argument, NULL, 0 },
	{ "constrained-intra-pred", required_argument, NULL, 0 },
	{ "sei-decoded-picture-hash", required_argument, NULL, 0 },
	{ "sei-picture-digest", required_argument, NULL, 0 },
	{ "sei-recovery-point", required_argument, NULL, 0 },
	{ "sei-buffering-period", required_argument, NULL, 0 },
	{ "sei-picture-timing", required_argument, NULL, 0 },
	{ "sei-frame-packing", required_argument, NULL, 0 },
	{ "sei-frame-packing-type", required_argument, NULL, 0 },
	{ "sei-frame-packing-id", required_argument, NULL, 0 },
	{ "sei-frame-packing-quincunx", required_argument, NULL, 0 },
	{ "sei-frame-packing-interpretation", required_argument, NULL, 0 },
	{ "sei-display-orientation", required_argument, NULL, 0 },
	{ "sei-temporal-level0-index", required_argument, NULL, 0 },
	{ "sei-gradual-decoding-refresh-info", required_argument, NULL, 0 },
	{ "sei-decoding-unit-info", required_argument, NULL, 0 },
	{ "weighted-pred-p", required_argument, NULL, 0 },
	{ "weighted-pred-b", required_argument, NULL, 0 },
	{ "log2-parallel-merge-level", required_argument, NULL, 0 },
	{ "max-num-merge-cand", required_argument, NULL, 0 },
	{ "tmvp-mode", required_argument, NULL, 0 },
	{ "sign-hide-flag", required_argument, NULL, 0 },
#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	{ "rate-control", required_argument, NULL, 0 },
	{ "target-bitrate", required_argument, NULL, 0 },
	{ "keep-hierarchical-bit", required_argument, NULL, 0 },
	{ "lcu-level-rate-control", required_argument, NULL, 0 },
	{ "rc-lcu-separate-model", required_argument, NULL, 0 },
	{ "initial-qp", required_argument, NULL, 0 },
	{ "rc-force-intra-qp", required_argument, NULL, 0 },
#else
	{ "rate-ctrl", required_argument, NULL, 0 },
	{ "target-bitrate", required_argument, NULL, 0 },
	{ "num-lcu-in-unit", required_argument, NULL, 0 },
#endif
	{ "scaling-list", required_argument, NULL, 0 },
	{ "transquant-bypass-enable-flag", required_argument, NULL, 0 },
	{ "cu-transquant-bypass-flag-value", required_argument, NULL, 0 },
	{ "recalculate-qp-according-to-lambda", required_argument, NULL, 0 },
	{ "strong-intra-smoothing", required_argument, NULL, 0 },
	{ "sei-active-parameter-sets", required_argument, NULL, 0 },

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	{ "bit-rate-pic-rate-max-t-layers", required_argument, NULL, 0 },
	{ "bit-rate-info-present", required_argument, NULL, 0 },
	{ "pic-rate-info-present", required_argument, NULL, 0 },
	{ "avg-bit-rate", required_argument, NULL, 0 },
	{ "max-bit-rate", required_argument, NULL, 0 },
	{ "avg-pic-rate", required_argument, NULL, 0 },
	{ "constant-pic-rate-idc", required_argument, NULL, 0 },
#endif

	{ "vui-parameters-present", required_argument, NULL, 0 },
	{ "aspect-ratio-info-present", required_argument, NULL, 0 },
	{ "aspect-ratio-idc", required_argument, NULL, 0 },
	{ "sar-width", required_argument, NULL, 0 },
	{ "sar-height", required_argument, NULL, 0 },
	{ "overscan-info-present", required_argument, NULL, 0 },
	{ "overscan-appropriate", required_argument, NULL, 0 },
	{ "video-signal-type-present", required_argument, NULL, 0 },
	{ "video-format", required_argument, NULL, 0 },
	{ "video-full-range", required_argument, NULL, 0 },
	{ "colour-description-present", required_argument, NULL, 0 },
	{ "colour-primaries", required_argument, NULL, 0 },
	{ "transfer-charateristics", required_argument, NULL, 0 },
	{ "matrix-coefficients", required_argument, NULL, 0 },
	{ "chroma-loc-info-present", required_argument, NULL, 0 },
	{ "chroma-sample-loc-type-top-field", required_argument, NULL, 0 },
	{ "chroma-sample-loc-type-bottom-field", required_argument, NULL, 0 },
	{ "neutral-chroma-indication", required_argument, NULL, 0 },
	{ "default-display-window-flag", required_argument, NULL, 0 },
	{ "def-disp-win-left-offset", required_argument, NULL, 0 },
	{ "def-disp-win-right-offset", required_argument, NULL, 0 },
	{ "def-disp-win-top-offset", required_argument, NULL, 0 },
	{ "def-disp-win-bottom-offset", required_argument, NULL, 0 },
	{ "frame-field-info-present-flag", required_argument, NULL, 0 },
	{ "poc-proportional-to-timing-flag", required_argument, NULL, 0 },
	{ "num-ticks-poc-diff-one-minus1", required_argument, NULL, 0 },
	{ "bitstream-restriction", required_argument, NULL, 0 },
	{ "tiles-fixed-structure", required_argument, NULL, 0 },
	{ "motion-vectors-over-pic-boundaries", required_argument, NULL, 0 },
	{ "max-bytes-per-pic-denom", required_argument, NULL, 0 },
	{ "max-bits-per-min-cu-denom", required_argument, NULL, 0 },
	{ "log2-max-mv-length-horizontal", required_argument, NULL, 0 },
	{ "log2-max-mv-length-vertical", required_argument, NULL, 0 },

	{0, 0, 0, 0}
};

static int select_bitstream_output( const char *muxer, char *filename, x265_param_t *param )
{
    cli_bitstream_output = raw_bitstream_output;
    return 0;
}


static int select_frame_input( const char *demuxer, char *used_demuxer, char *filename,
                         hnd_t *p_handle, video_info_t *info, cli_frame_input_opt_t *opt )
{
    int b_auto = !strcasecmp( demuxer, "auto" );
    const char *ext = b_auto ? get_filename_extension( filename ) : "";
    int b_regular = strcmp( filename, "-" );
    if( !b_regular && b_auto )
    {
        ext = "raw";
    }
    b_regular = b_regular && x265_is_regular_file_path( filename );
    if( b_regular )
    {
        FILE *f = fopen( filename, "r" );
        if( f )
        {
            b_regular = x265_is_regular_file( f );
            fclose( f );
        }
    }
    const char *module = b_auto ? ext : demuxer;

    cli_frame_input = raw_frame_input;
    strcpy( used_demuxer, module );

    return 0;
}

static int select_frame_output()
{
    cli_frame_output = raw_frame_output;

    return 0;
}

static int init_input_filters( char *sequence, hnd_t *handle, video_info_t *info, x265_param_t *param )
{
    x265_register_input_filters();

    /* intialize baseline filters */
    if( x265_init_input_filter( "source", handle, &input_filter, info, param ) ) /* wrap demuxer into a filter */
    {
        return -1;
    }
    if( x265_init_input_filter( "resize", handle, &input_filter, info, param ) ) /* normalize csps to be of a known/supported format */
    {
        return -1;
    }

    /* parse filter chain */
    for( char *p = sequence; p && *p; )
    {
        int tok_len = strcspn( p, "/" );
        int p_len = strlen( p );
        p[tok_len] = 0;
        int name_len = strcspn( p, ":" );
        p[name_len] = 0;
        name_len += name_len != tok_len;
        if( x265_init_input_filter( p, handle, &input_filter, info, param ) )
        {
            return -1;
        }
        p += X265_MIN( tok_len+1, p_len );
    }

    if( x265_init_input_filter( "depth", handle, &input_filter, info, param ) )
    {
        return -1;
    }

    return 0;
}

static int init_output_filters( char *sequence, hnd_t *handle, video_info_t *info, x265_param_t *param )
{
    x265_register_output_filters();

    /* intialize baseline filters */
    if( x265_init_output_filter( "source", handle, &output_filter, info, param ) ) /* wrap demuxer into a filter */
    {
        return -1;
    }
    if( x265_init_output_filter( "resize", handle, &output_filter, info, param ) ) /* normalize csps to be of a known/supported format */
    {
        return -1;
    }

    /* parse filter chain */
    for( char *p = sequence; p && *p; )
    {
        int tok_len = strcspn( p, "/" );
        int p_len = strlen( p );
        p[tok_len] = 0;
        int name_len = strcspn( p, ":" );
        p[name_len] = 0;
        name_len += name_len != tok_len;
        if( x265_init_output_filter( p, handle, &output_filter, info, param ) )
        {
            return -1;
        }
        p += X265_MIN( tok_len+1, p_len );
    }

    if( x265_init_output_filter( "depth", handle, &output_filter, info, param ) )
    {
        return -1;
    }

    return 0;
}


static int setting_default_config_type ( x265_param_t *param, const char *config_type )
{
	char **argv = NULL ;
	int argc = 0 ;
	int temp_optind = 0 ;

	if ( !strcmp(config_type, "low_delay") )
	{
		argv = low_delay_config ;
		while ( NULL != argv[argc] )
		{
			++ argc ;
		}
	}
	else
	{
		x265_cli_log( "x265", X265_LOG_ERROR, "invalid argument: %s = %s\n", "config-type", config_type );
		return -1 ;
	}

	temp_optind = optind ;
	for( optind = 0;; )
    {
        int c = getopt_long( argc, argv, short_options, long_options, NULL );
        if( c == -1 )
        {
            break;
        }
        else if( c == '?' )
        {
        	optind = temp_optind ;
            return -1;
        }
    }

    /* Parse command line options */
    for( optind = 0;; )
    {
        int b_error = 0;
        int long_options_index = -1;

        int c = getopt_long( argc, argv, short_options, long_options, &long_options_index );

        if( c == -1 )
        {
            break;
        }

		if( long_options_index < 0 )
		{
			for( int i = 0; long_options[i].name; i++ )
				if( long_options[i].val == c )
				{
					long_options_index = i;
					break;
				}
			if( long_options_index < 0 )
			{
				/* getopt_long already printed an error message */
				return -1;
			}
		}

		b_error |= x265_param_parse( param, long_options[long_options_index].name, optarg );
		if( b_error )
		{
			const char *name = long_options_index > 0 ? long_options[long_options_index].name : argv[optind-2];
			x265_cli_log( "x265", X265_LOG_ERROR, "invalid argument: %s = %s\n", name, optarg );
        	optind = temp_optind ;
			return -1;
		}
    }

	optind = temp_optind ;
    return 0 ;
}

static int parse( int argc, char **argv, x265_param_t *param, cli_opt_t *opt )
{
    int i_min_cu_size = 0 ;
    char *input_filename = NULL;
    const char *demuxer = demuxer_names[0];
    char *output_filename = NULL;
    const char *muxer = muxer_names[0];
    char *config_filename = NULL;
    x265_param_t defaults;
    char *input_filters = NULL;
    char *output_filters = NULL;
    cli_bitstream_output_opt_t bitstream_output_opt;
    cli_frame_input_opt_t frame_input_opt;
    cli_frame_output_opt_t frame_output_opt;
    char demuxername[5];
    video_info_t info = {0};


    x265_param_default( &defaults );
    cli_log_level = defaults.i_log_level;

    opt->b_progress = 1;

    /* Presets are applied before all other options. */
    for( optind = 0;; )
    {
        int c = getopt_long( argc, argv, short_options, long_options, NULL );
        if( c == -1 )
        {
            break;
        }
        else if( c == '?' )
        {
            return -1;
        }
    }

    if( x265_param_default_preset( param ) < 0 )
    {
        return -1;
    }

    /* Parse command line options */
    for( optind = 0;; )
    {
        int b_error = 0;
        int long_options_index = -1;

        int c = getopt_long( argc, argv, short_options, long_options, &long_options_index );

        if( c == -1 )
        {
            break;
        }

        switch( c )
        {
			case 'h':
				help( &defaults );
				exit(0);
				break ;

			case 'o':
				output_filename = optarg;
				break;

			case 'c':
				config_filename = optarg;
				break ;

			case OPT_CONFIG_TYPE :
				b_error = setting_default_config_type ( param, optarg ) ;
				if ( b_error )
				{
					return b_error ;
				}
				break ;

			default:
			{
				if( long_options_index < 0 )
				{
					for( int i = 0; long_options[i].name; i++ )
						if( long_options[i].val == c )
						{
							long_options_index = i;
							break;
						}
					if( long_options_index < 0 )
					{
						/* getopt_long already printed an error message */
						return -1;
					}
				}

				b_error |= x265_param_parse( param, long_options[long_options_index].name, optarg );
			}
			break ;
        }

		if( b_error )
		{
			const char *name = long_options_index > 0 ? long_options[long_options_index].name : argv[optind-2];
			x265_cli_log( "x265", X265_LOG_ERROR, "invalid argument: %s = %s\n", name, optarg );
			return -1;
		}
    }

    if ( config_filename )
    {
    	if ( x265_config_file_parse ( param, config_filename ) )
    	{
    		return -1 ;
    	}
    }

    if ( 0 == param->sps.i_bit_depth_y )
    {
    	param->sps.i_bit_depth_y = param->i_input_bit_depth_y ;
    }
    if ( 0 == param->sps.i_bit_depth_c )
    {
    	param->sps.i_bit_depth_c = param->sps.i_bit_depth_y ;
    }
    if ( 0 == param->i_input_bit_depth_c )
    {
    	param->i_input_bit_depth_c = param->i_input_bit_depth_y ;
    }
    if ( 0 == param->i_output_bit_depth_y )
    {
    	param->i_output_bit_depth_y = param->sps.i_bit_depth_y ;
    }
    if ( 0 == param->i_output_bit_depth_c )
    {
    	param->i_output_bit_depth_c = param->sps.i_bit_depth_c ;
    }

    /* Get the file name */
    FAIL_IF_ERROR( optind > argc - 1 || !output_filename, "No %s file. Run x265 --help for a list of options.\n",
                   optind > argc - 1 ? "input" : "output" )

    input_filename = argv[optind++];
    frame_input_opt.resolution = NULL ;
    frame_input_opt.i_input_bit_depth_y = param->i_input_bit_depth_y ;
    frame_input_opt.i_input_bit_depth_c = param->i_input_bit_depth_c ;

    if( select_bitstream_output( muxer, output_filename, param ) )
    {
        return -1;
    }
    FAIL_IF_ERROR( cli_bitstream_output.open_file( output_filename, &opt->hout, &bitstream_output_opt ), "could not open output file `%s'\n", output_filename )

    if( select_frame_input( demuxer, demuxername, input_filename, &opt->hin, &info, &frame_input_opt ) )
    {
        return -1;
    }

    FAIL_IF_ERROR( !opt->hin && cli_frame_input.open_file( input_filename, &opt->hin, &info, &frame_input_opt ),
                   "could not open input file `%s'\n", input_filename )

    param->i_width = info.width ;
    param->i_height = info.height ;

    switch (param->sps.i_conformance_mode)
    {
    case 0:
	{
		// no conformance or padding
		param->sps.i_conf_left = param->sps.i_conf_right = param->sps.i_conf_top = param->sps.i_conf_bottom = 0;
		param->i_pad[1] = param->i_pad[0] = 0;
		break;
	}
    case 1:
    {
        // automatic padding to minimum CU size
    	i_min_cu_size = param->sps.i_max_cu_height >> (param->sps.i_max_cu_depth - 1);
        if (param->i_width % i_min_cu_size)
        {
        	param->i_pad[0] = param->sps.i_conf_right  = ((param->i_width / i_min_cu_size) + 1) * i_min_cu_size - param->i_width;
        	param->i_width  += param->sps.i_conf_right;
        }
        if (param->i_height % i_min_cu_size)
        {
        	param->i_pad[1] = param->sps.i_conf_bottom = ((param->i_height / i_min_cu_size) + 1) * i_min_cu_size - param->i_height;
        	param->i_height += param->sps.i_conf_bottom;
        }
        if (param->i_pad[0] % 2 != 0)
        {
        	fprintf(stderr, "Error: picture width is not an integer multiple of the specified chroma subsampling\n");
        	exit(EXIT_FAILURE);
        }
        if (param->i_pad[1] % 2 != 0)
        {
        	fprintf(stderr, "Error: picture height is not an integer multiple of the specified chroma subsampling\n");
        	exit(EXIT_FAILURE);
        }
        break;
    }
    case 2:
	{
		//padding
		param->i_width  += param->i_pad[0];
		param->i_height += param->i_pad[1];
		param->sps.i_conf_right  = param->i_pad[0];
		param->sps.i_conf_bottom = param->i_pad[1];
		break;
	}
    case 3:
	{
		// conformance
		if ((param->sps.i_conf_left == 0) && (param->sps.i_conf_right == 0)
				&& (param->sps.i_conf_top == 0) && (param->sps.i_conf_bottom == 0))
		{
			fprintf(stderr, "Warning: Conformance window enabled, but all conformance window parameters set to zero\n");
		}
		if ((param->i_pad[1] != 0) || (param->i_pad[0]!=0))
		{
			fprintf(stderr, "Warning: Conformance window enabled, padding parameters will be ignored\n");
		}
		param->i_pad[1] = param->i_pad[0] = 0;
		break;
	}
    }

    param->i_qp = ((int32_t) param->f_qp) ;
    param->pps.i_wave_front_substreams = param->pps.i_wave_front_synchro ?
    									(param->i_height + param->sps.i_max_cu_height - 1) / param->sps.i_max_cu_height
    									: 1 ;

    if( init_input_filters( input_filters, &opt->hin, &info, param ) )
    {
    	return -1;
    }

    if ( param->psz_dump_yuv )
    {
        if( select_frame_output() )
        {
            return -1;
        }

        frame_output_opt.i_width = param->i_width ;
        frame_output_opt.i_height = param->i_height ;
        frame_output_opt.i_output_bit_depth_y = param->i_output_bit_depth_y ;
        frame_output_opt.i_output_bit_depth_c = param->i_output_bit_depth_c ;
        FAIL_IF_ERROR( !opt->hrecon && cli_frame_output.open_file( param->psz_dump_yuv, &opt->hrecon, &frame_output_opt ),
                       "could not open input file `%s'\n", input_filename )

        if( init_output_filters( output_filters, &opt->hrecon, &info, param ) )
        {
        	return -1;
        }
    }

    if ( x265_check_parameters ( param ) )
    {
    	return -1 ;
    }

    return 0 ;
}


static void convert_cli_to_lib_pic( x265_picture_t *lib, cli_pic_t *cli )
{
	lib->img.i_width = cli->img.width ;
	lib->img.i_height = cli->img.height ;
    memcpy( lib->img.i_stride, cli->img.stride, sizeof(cli->img.stride) );
    memcpy( lib->img.plane, cli->img.plane, sizeof(cli->img.plane) );
    lib->i_pts = cli->pts;
}

static void convert_lib_to_cli_pic( x265_picture_t *lib, cli_pic_t *cli )
{
	cli->img.width = lib->img.i_width ;
	cli->img.height = lib->img.i_height ;
    memcpy( cli->img.stride, lib->img.i_stride, sizeof(lib->img.i_stride) );
    memcpy( cli->img.plane, lib->img.plane, sizeof(lib->img.plane) );
    cli->pts = lib->i_pts;
}

static int encode_frame( x265_t *h, hnd_t hout, x265_picture_t *pic, cli_opt_t *opt )
{
    x265_picture_t pic_out;
    x265_nal_t *nal;
    int i_nal;
    int i_frame_size = 0;
    cli_pic_t cli_pic;

    i_frame_size = x265_encoder_encode( h, &nal, &i_nal, pic, &pic_out );

    FAIL_IF_ERROR( i_frame_size < 0, "x265_encoder_encode failed\n" );

    if( i_frame_size )
    {
        i_frame_size = cli_bitstream_output.write_frame( hout, nal[0].p_payload, i_frame_size, &pic_out );
        if ( h->param.psz_dump_yuv )
        {
        	convert_lib_to_cli_pic ( &pic_out, &cli_pic ) ;
        	output_filter.write_frame ( opt->hrecon, &cli_pic ) ;
        }
    }

    return i_frame_size;
}


#define FAIL_IF_ERROR2( cond, ... )\
if( cond )\
{\
    x265_cli_log( "x265", X265_LOG_ERROR, __VA_ARGS__ );\
    retval = -1;\
    goto fail;\
}

static int encode( x265_param_t *param, cli_opt_t *opt )
{
    x265_t *h = NULL;
    x265_picture_t pic;
    cli_pic_t cli_pic;
    int i_frame = 0;
    int i_frame_output = 0 ;
    int i_file_size = 0 ;
    int i_size = 0 ;
    int i_frame_size ;
    x265_nal_t *headers;
    int i_nal;
    int retval = 0 ;
    double f_time = 0.0 ;

    h = x265_encoder_open( param );
    FAIL_IF_ERROR2( !h, "x265_encoder_open failed\n" );


    if( !param->b_repeat_headers )
    {
    	FAIL_IF_ERROR2( x265_encoder_headers( h, &headers, &i_nal ) < 0, "x265_encoder_headers failed\n" )
    	FAIL_IF_ERROR2( (i_size = cli_bitstream_output.write_headers( opt->hout, headers )) < 0, "error writing headers to output file\n" );
    }
    i_file_size += i_size ;

    for( ; !b_ctrl_c && (i_frame < param->i_frame_total || !param->i_frame_total); i_frame++ )
	{
		if( input_filter.get_frame( opt->hin, &cli_pic, i_frame + opt->i_seek ) )
			break;
		x265_picture_init( &pic );
		convert_cli_to_lib_pic( &pic, &cli_pic );

		pic.i_pts = i_frame ;

		i_frame_size = encode_frame( h, opt->hout, &pic, opt );
		if( i_frame_size < 0 )
		{
			b_ctrl_c = 1; /* lie to exit the loop */
			retval = -1;
		}
		else if( i_frame_size )
		{
			i_file_size += i_frame_size;
			i_frame_output++;
		}

		if( input_filter.release_frame( opt->hin, &cli_pic ) )
			break;
	}

    while( !b_ctrl_c && x265_encoder_delayed_frames( h ) )
    {
        i_frame_size = encode_frame( h, opt->hout, NULL, opt );
        if( i_frame_size < 0 )
        {
            b_ctrl_c = 1; /* lie to exit the loop */
            retval = -1;
        }
        else if( i_frame_size )
        {
        	i_file_size += i_frame_size;
            i_frame_output++;
        }
    }

    x265_encoder_print_summary (h) ;

    f_time = ((double)h->enc_top.i_num_all_pic_coded) / ((double)h->param.i_frame_rate);

    printf("Bytes written to file: %u (%.3f kbps)\n", i_file_size, 0.008 * i_file_size / f_time);
#if X265_VERBOSE_RATE
    printf("Bytes for SPS/PPS/Slice (Incl. Annex B): %u (%.3f kbps)\n", m_essentialBytes, 0.008 * m_essentialBytes / time);
#endif


    close_state_file () ;

    if( h )
    {
    	x265_encoder_close( h );
    }

fail:
	return retval ;
}
