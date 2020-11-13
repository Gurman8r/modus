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

		explicit windows_platform_api(event_bus * bus, allocator_type alloc);

		~windows_platform_api() noexcept override;

	private:
		void on_event(event const & value) final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // _ML_WINDOWS_PLATFORM_API_HPP_