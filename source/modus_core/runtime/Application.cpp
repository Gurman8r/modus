#include <modus_core/runtime/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(runtime_context * const ctx) noexcept
		: runtime_base	{ ctx }
		, loop_system	{ get_memory()->get_allocator() }
		, m_plugins		{ this }
	{
		if (!get_global<application>())
		{
			set_global<application>(this);
		}
	}

	application::~application() noexcept
	{
		if (this == get_global<application>())
		{
			release_global<application>();
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global application
namespace ml::globals
{
	static application * g_application{};

	ML_impl_global(application) get() noexcept
	{
		return g_application;
	}

	ML_impl_global(application) set(application * value) noexcept
	{
		return g_application = value;
	}
}