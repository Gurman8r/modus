#include <modus_core/runtime/Library.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_Platform.hpp>
using impl_platform = _ML win32_platform;

#else
#error "library is unavailable"
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	library_context const library::native_context
	{
		&impl_platform::load_library,
		&impl_platform::free_library,
		&impl_platform::get_proc_address
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}