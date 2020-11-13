#include <modus_core/engine/PlatformAPI.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/windows/Win32_PlatformAPI.hpp>
using native_api = _ML win32_platform_api;
#else
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::optional<fs::path> platform_api::get_open_file_name(window_handle window, cstring filter) noexcept
	{
		return native_api::get_open_file_name(window, filter);
	}

	std::optional<fs::path> platform_api::get_save_file_name(window_handle window, cstring filter) noexcept
	{
		return native_api::get_save_file_name(window, filter);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}