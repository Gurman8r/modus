#include <modus_core/window/Cursor.hpp>

#if defined(ML_IMPL_WINDOW_GLFW)
#include <modus_core/backends/glfw/GLFW_Window.hpp>
using impl_cursor = _ML glfw_cursor;

#else
#error "cursor unavailable"
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	cursor_handle cursor::create_custom(size_t w, size_t h, byte const * p, int32 x, int32 y) noexcept
	{
		return impl_cursor::create_custom(w, h, p, x, y);
	}

	cursor_handle cursor::create_standard(cursor_shape_ shape) noexcept
	{
		return impl_cursor::create_standard(shape);
	}

	void cursor::destroy(cursor_handle value) noexcept
	{
		impl_cursor::destroy(value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}