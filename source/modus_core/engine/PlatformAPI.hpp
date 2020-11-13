#ifndef _ML_PLATFORM_API_HPP_
#define _ML_PLATFORM_API_HPP_

#include <modus_core/window/WindowAPI.hpp>

namespace ml
{
	// base platform api
	class ML_CORE_API platform_api
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		static std::optional<fs::path> open_filename(window_handle window, cstring filter) noexcept;

		static std::optional<fs::path> save_filename(window_handle window, cstring filter) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLATFORM_API_HPP_