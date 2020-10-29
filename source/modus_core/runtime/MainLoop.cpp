#include <modus_core/runtime/MainLoop.hpp>
#include <modus_core/runtime/RuntimeEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	main_loop::main_loop(runtime_api * api, loop_condition const & loopcond) noexcept
		: runtime_listener	{ api }
		, m_locked			{}
		, m_loopcond		{ loopcond }
		, m_subsystems		{ api->mem->get_allocator() }
	{
		if (!get_global<main_loop>())
		{
			set_global<main_loop>(this);
		}
	}

	main_loop::~main_loop() noexcept
	{
		if (this == get_global<main_loop>())
		{
			set_global<main_loop>(nullptr);
		}
	}

	int32_t main_loop::process() noexcept
	{
		// lock / unlock
		if (locked()) { return EXIT_FAILURE * 1; }
		else { lock(); } ML_defer(&) { unlock(); };

		// enter / exit
		on_enter(); ML_defer(&) { on_exit(); };
		if (!check_condition()) { return EXIT_FAILURE * 2; }

		// idle
		auto const api{ get_api() };
		do
		{
			begin_idle(api->io);

			on_idle();

			end_idle(api->io);
		}
		while (check_condition());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global main loop
namespace ml::globals
{
	static main_loop * g_main_loop{};

	template <> main_loop * get() noexcept {
		return g_main_loop;
	}

	template <> main_loop * set(main_loop * value) noexcept {
		return g_main_loop = value;
	}
}