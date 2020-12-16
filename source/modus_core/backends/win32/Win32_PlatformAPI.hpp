#ifndef _ML_WIN32_PLATFORM_API_HPP_
#define _ML_WIN32_PLATFORM_API_HPP_

#include <modus_core/window/WindowAPI.hpp>

static_assert(ML_os_windows);

namespace ml
{
	// win32 platform api
	class win32_platform_api final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		static std::optional<fs::path> get_open_file_name(window_handle window, cstring filter);

		static std::optional<fs::path> get_save_file_name(window_handle window, cstring filter);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // _ML_WIN32_PLATFORM_API_HPP_