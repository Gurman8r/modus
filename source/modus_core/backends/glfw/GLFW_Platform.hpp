#ifndef _ML_GLFW_PLATFORM_HPP_
#define _ML_GLFW_PLATFORM_HPP_

#include <modus_core/window/BaseWindow.hpp>

// PLATFORM
namespace ml
{
	// glfw platform
	class glfw_platform final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static bool initialize();

		static bool finalize();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static window_handle get_active_window();

		static window_handle set_active_window(window_handle handle);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static void poll_events();

		static void swap_buffers(window_handle value);

		static void swap_interval(int32 value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static cursor_handle create_custom_cursor(size_t w, size_t h, byte const * p, int32 x, int32 y);

		static cursor_handle create_standard_cursor(int32 shape);

		static void destroy_cursor(cursor_handle const & handle);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_GLFW_PLATFORM_HPP_