#include <modus_core/runtime/LoopSystem.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	loop_system::loop_system(runtime_api * api, loop_condition const & loopcond) noexcept
		: runtime_listener	{ api }
		, m_bypass			{}
		, m_locked			{}
		, m_loopcond		{ loopcond }
		, m_subsystems		{ api->mem->get_allocator() }
	{
		if (!get_global<loop_system>())
		{
			set_global<loop_system>(this);
		}
	}

	loop_system::~loop_system() noexcept
	{
		if (this == get_global<loop_system>())
		{
			set_global<loop_system>(nullptr);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global loop system
namespace ml::globals
{
	static loop_system * g_loop_system{};

	template <> loop_system * get() noexcept {
		return g_loop_system;
	}

	template <> loop_system * set(loop_system * value) noexcept {
		return g_loop_system = value;
	}
}