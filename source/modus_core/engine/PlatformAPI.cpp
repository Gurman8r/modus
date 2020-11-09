#include <modus_core/engine/PlatformAPI.hpp>

#ifdef ML_os_windows
#include <modus_core/backends/windows/Windows_PlatformAPI.hpp>
using native_api = _ML windows_platform_api;

#else
#error ""
#endif

namespace ml
{
	platform_api * platform_api::create() noexcept
	{
		auto const temp{ new native_api{} };

		if (!get_global<platform_api>()) { set_global<platform_api>(temp); }

		return temp;
	}

	void platform_api::destroy(platform_api * value) noexcept
	{
		auto const g{ get_global<platform_api>() };

		if (!value) { value = g; }

		if (value == g) { set_global<platform_api>(nullptr); }

		delete value;
	}
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