#ifndef _ML_CURSOR_HPP_
#define _ML_CURSOR_HPP_

#include <modus_core/detail/Matrix.hpp>

namespace ml
{
	// cursor handle
	ML_decl_handle(cursor_handle);

	// cursor mode
	enum cursor_mode_ : int32
	{
		cursor_mode_normal		, // normal
		cursor_mode_hidden		, // hidden
		cursor_mode_disabled	, // disabled
	};

	// cursor shape
	enum cursor_shape_ : int32
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
}

#endif // !_ML_CURSOR_HPP_