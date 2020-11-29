#include <modus_core/window/Monitor.hpp>

#if defined(ML_IMPL_WINDOW_GLFW)
#include <modus_core/backends/glfw/GLFW_Window.hpp>
using impl_monitor = _ML glfw_monitor;

#else
#error "monitor unavailable"
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ds::list<monitor_handle> const & monitor::get_monitors() noexcept
	{
		return impl_monitor::get_monitors();
	}

	monitor_handle monitor::get_primary() noexcept
	{
		return impl_monitor::get_primary();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}