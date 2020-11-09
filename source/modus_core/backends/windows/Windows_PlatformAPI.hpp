#ifndef _ML_WINDOWS_PLATFORM_API_HPP_
#define _ML_WINDOWS_PLATFORM_API_HPP_

// WIP

#include <modus_core/engine/PlatformAPI.hpp>

namespace ml
{
	// windows platform api
	struct windows_platform_api final : platform_api
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		windows_platform_api() noexcept;

		~windows_platform_api() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // _ML_WINDOWS_PLATFORM_API_HPP_