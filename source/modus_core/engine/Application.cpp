#include <modus_core/engine/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(int32_t argc, char * argv[], allocator_type alloc)
		: gui_application{ argc, argv, alloc }
	{
		if (!get_global<application>()) { set_global<application>(this); }
	}

	application::~application() noexcept
	{
		if (this == get_global<application>()) { set_global<application>(nullptr); }
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global application
namespace ml::globals
{
	static application * g_app{};

	ML_impl_global(application) get() noexcept
	{
		return g_app;
	}

	ML_impl_global(application) set(application * value) noexcept
	{
		return g_app = value;
	}
}