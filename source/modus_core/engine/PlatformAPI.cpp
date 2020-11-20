#include <modus_core/engine/PlatformAPI.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/win32/Win32_PlatformAPI.hpp>
using native_api = _ML win32_platform_api;
#else
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> platform_api::open_file_name(window_handle window, cstring filter) noexcept
	{
		return native_api::open_file_name(window, filter);
	}

	std::optional<fs::path> platform_api::save_file_name(window_handle window, cstring filter) noexcept
	{
		return native_api::save_file_name(window, filter);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}