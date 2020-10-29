#include <modus_core/runtime/MainLoop.hpp>
#include <modus_core/runtime/RuntimeEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	main_loop::main_loop(runtime_api * api) noexcept
		: runtime_listener	{ api }
		, m_running			{}
		, m_loopcond		{}
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

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t main_loop::process() noexcept
	{
		// lock
		if (m_running) { return EXIT_FAILURE; }
		else { m_running = true; } ML_defer(&) { m_running = false; };

		// main sequence
		on_enter(); ML_defer(&) { on_exit(); };
		if (!check_condition()) { return EXIT_FAILURE; }
		do
		{
			auto ML_anon{ do_benchmarks(get_io()) };

			get_window()->poll_events();

			on_idle();

			get_window()->swap_buffers();
		}
		while (check_condition());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

namespace ml::globals
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// global main loop
	static main_loop * g_main_loop{};

	template <> main_loop * get() noexcept {
		return g_main_loop;
	}

	template <> main_loop * set(main_loop * value) noexcept {
		return g_main_loop = value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}