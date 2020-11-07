#include <modus_core/engine/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(engine_context * const ctx) noexcept
		: engine_base	{ ctx }
		, loop_system	{ get_memory()->get_allocator() }
		, m_on_event	{}
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
			set_global<application>(nullptr);
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