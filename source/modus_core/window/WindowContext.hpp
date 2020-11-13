#ifndef _ML_WINDOW_CONTEXT_HPP_
#define _ML_WINDOW_CONTEXT_HPP_

#include <modus_core/window/WindowAPI.hpp>

// WINDOW CONTEXT
namespace ml
{
	// window context
	struct ML_NODISCARD window_context final
	{
		~window_context() noexcept
		{
			(void)((!set_error_callback) || (set_error_callback(nullptr), 0));
		}

		int32_t (*initialize)();

		void (*finalize)();

		window_error_callback(*set_error_callback)(window_error_callback);

		int32_t (*extension_supported)(cstring);

		void * (*get_proc_address)(cstring);

		window_handle (*get_context_current)();

		ds::list<monitor_handle> const & (*get_monitors)();

		monitor_handle (*get_primary_monitor)();

		duration (*get_time)();

		void (*make_context_current)(window_handle);

		void (*poll_events)();

		void (*swap_buffers)(window_handle);

		void (*swap_interval)(int32_t);
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_WINDOW_CONTEXT_HPP_