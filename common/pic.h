


#ifndef X265_PIC_H
#define X265_PIC_H

#include "common_def.h"

typedef struct
{
	uint32_t i_t_layer;               //  Temporal layer
	int32_t b_used_by_curr;            //  Used by current picture
	int32_t b_is_long_term;            //  IS long term picture
	int32_t b_is_used_as_long_term;      //  long term picture is used as reference before

	x265_pic_yuv_t* pic_yuv_t[2];           //  Texture,  0:org / 1:rec

	x265_pic_yuv_t* pic_yuv_pred_t;           //  Prediction
	x265_pic_yuv_t* pic_yuv_resi_t;           //  Residual
	int32_t b_reconstructed_t;
	int32_t b_needed_for_output_t;
	uint32_t i_curr_slice_idx;         // Index of current slice
	int32_t* slice_su_map;
	int32_t* valid_slice;
	int32_t i_slice_granularity_for_ndb_filter;
	int32_t b_independent_slice_boundary_for_ndb_filter;
	int32_t b_independent_tile_boundary_for_ndb_filter;
	x265_pic_yuv_t* ndb_filter_yuv_tmp_t;    //!< temporary picture buffer when non-cross slice/tile boundary in-loop filtering is enabled
	int32_t b_check_ltmsb;

	int32_t i_num_reorder_pics[X265_MAX_TLAYER];
	x265_window_t conformance_window_t;
	x265_window_t default_display_window_t;

	//std::vector<std::vector<TComDataCU*> > m_vSliceCUDataLink;
	//SEIMessages  m_SEIs; ///< Any SEI messages that have been received.  If !NULL we own the object.

} x265_pic_t ;


#endif

