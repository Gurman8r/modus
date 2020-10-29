#include <modus_core/system/OS.hpp>

namespace ml
{
	OS::OS() noexcept
	{
		ML_assert(!get_global<OS>());
		set_global<OS>(this);
	}
	
	OS::~OS() noexcept
	{
		ML_assert(this == get_global<OS>());
		set_global<OS>(nullptr);
	}
}

namespace ml::globals
{
	static OS * g_os{};

	template <> OS * get() noexcept {
		return g_os;
	}

	template <> OS * set(OS * value) noexcept {
		return g_os = value;
	}
}