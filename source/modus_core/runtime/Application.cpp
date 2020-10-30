#include <modus_core/runtime/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	application::application(runtime_api * api)
		: runtime_listener	{ api }
		, loop_system		{ api->memory->get_allocator() }
		, m_plugins			{ this }
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

	template <> application * get() noexcept {
		return g_application;
	}

	template <> application * set(application * value) noexcept {
		return g_application = value;
	}
}