#ifndef _ML_WINDOW_EVENTS_HPP_
#define _ML_WINDOW_EVENTS_HPP_

#include <modus_core/detail/EventSystem.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_close_event)
	{
		constexpr window_close_event() noexcept
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_content_scale_event)
	{
		float32 x, y;

		constexpr window_content_scale_event(float32 x, float32 y) noexcept
			: x{ x }, y{ y }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_drop_event)
	{
		int32 count;
		cstring const * paths;

		constexpr window_drop_event(int32 count, cstring const * paths) noexcept
			: count{ count }, paths{ paths }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_error_event)
	{
		int32 code;
		cstring desc;

		constexpr window_error_event(int32 code, cstring desc) noexcept
			: code{ code }, desc{ desc }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_focus_event)
	{
		int32 focused;

		constexpr window_focus_event(int32 focused) noexcept
			: focused{ focused }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_framebuffer_resize_event)
	{
		int32 width, height;

		constexpr window_framebuffer_resize_event(int32 width, int32 height) noexcept
			: width{ width }, height{ height }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_iconify_event)
	{
		int32 iconified;

		constexpr window_iconify_event(int32 iconified) noexcept
			: iconified{ iconified }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_maximize_event)
	{
		int32 maximized;

		constexpr window_maximize_event(int32 maximized) noexcept
			: maximized{ maximized }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_position_event)
	{
		int32 x, y;

		constexpr window_position_event(int32 x, int32 y) noexcept
			: x{ x }, y{ y }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_refresh_event)
	{
		constexpr window_refresh_event() noexcept
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_scroll_event)
	{
		float64 x, y;

		constexpr window_scroll_event(float64 x, float64 y) noexcept
			: x{ x }, y{ y }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(window_resize_event)
	{
		int32 width, height;

		constexpr window_resize_event(int32 width, int32 height) noexcept
			: width{ width }, height{ height }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_WINDOW_EVENTS_HPP_