#ifndef _ML_WINDOW_CONTEXT_HPP_
#define _ML_WINDOW_CONTEXT_HPP_

#include <modus_core/window/WindowAPI.hpp>

namespace ml
{
	// window context (WIP)
	class ML_CORE_API window_context final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static int32 initialize();

		static void finalize();

		static window_error_callback set_error_callback(window_error_callback value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static window_handle get_active_window();

		static void set_active_window(window_handle value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static void poll_events();

		static void swap_buffers(window_handle value);

		static void set_swap_interval(int32 value);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_WINDOW_CONTEXT_HPP_