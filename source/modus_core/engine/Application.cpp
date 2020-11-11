#include <modus_core/engine/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(int32_t argc, char * argv[], allocator_type alloc)
		: gui_application{ argc, argv, alloc }
	{
		ML_assert(begin_global<application>(this));
	}

	application::~application() noexcept
	{
		ML_assert(end_global<application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void application::on_event(event const & value)
	{
		gui_application::on_event(value);
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