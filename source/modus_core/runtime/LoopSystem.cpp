#include <modus_core/runtime/LoopSystem.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	loop_system::loop_system(runtime_api * api, loop_condition const & loopcond) noexcept
		: runtime_listener	{ api }
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

	int32_t loop_system::process() noexcept
	{
		if (locked()) { return EXIT_FAILURE * 1; }
		else { lock(); } ML_defer(&) { unlock(); };

		on_process_enter(); ML_defer(&) { on_process_exit(); };

		if (!check_condition()) { return EXIT_FAILURE * 2; }
		
		do { on_process_idle(); } while (check_condition());
		
		return EXIT_SUCCESS;
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