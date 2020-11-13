#include <modus_core/engine/PlatformAPI.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/windows/Windows_PlatformAPI.hpp>
using native_platform = _ML windows_platform_api;
#endif

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	platform_api * platform_api::create(event_bus * bus, allocator_type alloc) noexcept
	{
		auto const temp{ new native_platform{ bus, alloc } };

		begin_singleton<platform_api>(temp);

		return temp;
	}

	void platform_api::destroy(platform_api * value) noexcept
	{
		if (!value) { value = get_global<platform_api>(); }

		end_singleton<platform_api>(value);

		delete value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global platform_api
namespace ml::globals
{
	static platform_api * g_platform_api{};

	ML_impl_global(platform_api) get() noexcept
	{
		return g_platform_api;
	}

	ML_impl_global(platform_api) set(platform_api * value) noexcept
	{
		return g_platform_api = value;
	}
}