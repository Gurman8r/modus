#ifndef _ML_CURSOR_HPP_
#define _ML_CURSOR_HPP_

// WIP

#include <modus_core/Common.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_handle(cursor_handle); // cursor handle

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// cursor mode
	enum cursor_mode_ : int32_t
	{
		cursor_mode_normal		, // normal
		cursor_mode_hidden		, // hidden
		cursor_mode_disabled	, // disabled
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// cursor shape
	enum cursor_shape_ : int32_t
	{
		cursor_shape_arrow			, // arrow
		cursor_shape_ibeam			, // ibeam
		cursor_shape_crosshair		, // crosshair
		cursor_shape_pointing_hand	, // pointing_hand
		cursor_shape_ew				, // ew
		cursor_shape_ns				, // ns
		cursor_shape_nesw			, // nesw
		cursor_shape_nwse			, // nwse
		cursor_shape_resize_all		, // resize_all
		cursor_shape_not_allowed	, // not_allowed
		cursor_shape_hresize		, // hresize
		cursor_shape_vresize		, // vresize
		cursor_shape_hand			, // hand
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CURSOR_HPP_