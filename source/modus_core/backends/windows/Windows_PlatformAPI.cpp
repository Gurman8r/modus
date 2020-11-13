#include "Windows_PlatformAPI.hpp"

#include <Windows.h>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	windows_platform_api::windows_platform_api(event_bus * bus, allocator_type alloc)
		: platform_api{ bus }
	{
	}

	windows_platform_api::~windows_platform_api() noexcept
	{
	}

	void windows_platform_api::on_event(event const & value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}