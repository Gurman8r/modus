#include <modus_core/runtime/RuntimeContext.hpp>
#include <modus_core/runtime/RuntimeEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// global runtime context
	static runtime_context * g_runtime{};

	runtime_context * get_global_runtime() noexcept {
		return g_runtime;
	}

	runtime_context * set_global_runtime(runtime_context * value) noexcept {
		return g_runtime = value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	runtime_context::runtime_context(runtime_api * api) noexcept
		: runtime_listener	{ api }
		, m_running			{}
		, m_loopcond		{}
		, m_plugins			{ api }
	{
		if (!get_global_runtime())
		{
			set_global_runtime(this);
		}
	}

	runtime_context::~runtime_context() noexcept
	{
		if (this == get_global_runtime())
		{
			set_global_runtime(nullptr);
		}
	}

	int32_t runtime_context::idle()
	{
		// run check
		if (m_running) { return EXIT_FAILURE; }
		else { m_running = true; } ML_defer(&) { m_running = false; };

		// enter/exit
		on_enter(); ML_defer(&) { on_exit(); };

		// main loop
		if (!check_loop_condition()) { return EXIT_FAILURE; }
		do {

			auto ML_anon{ do_benchmarks(get_io()) };
			
			on_idle();

		} while (check_loop_condition());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}