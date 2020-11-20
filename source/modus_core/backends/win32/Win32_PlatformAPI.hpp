#ifndef _ML_WIN32_PLATFORM_API_HPP_
#define _ML_WIN32_PLATFORM_API_HPP_

#include <modus_core/engine/PlatformAPI.hpp>

static_assert(ML_os_windows);

namespace ml
{
	// win32 platform api
	class win32_platform_api final : public platform_api
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		static std::optional<fs::path> open_file_name(window_handle window, cstring filter);

		static std::optional<fs::path> save_file_name(window_handle window, cstring filter);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // _ML_WIN32_PLATFORM_API_HPP_