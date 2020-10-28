#include <modus_core/runtime/RuntimeContext.hpp>
#include <modus_core/runtime/RuntimeEvents.hpp>
#include <modus_core/imgui/ImGui.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	runtime_context::runtime_context(runtime_api * api) noexcept
		: runtime_listener	{ api }
		, m_running			{}
		, m_loopcond		{}
		, m_plugins			{ api }
	{
		if (!get_global<runtime_context>())
		{
			set_global<runtime_context>(this);
		}
	}

	runtime_context::~runtime_context() noexcept
	{
		if (this == get_global<runtime_context>())
		{
			set_global<runtime_context>(nullptr);
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

			render_window::poll_events();
			
			on_idle();

			if (get_window()->has_hints(window_hints_doublebuffer))
			{
				render_window::swap_buffers(get_window()->get_handle());
			}

		} while (check_loop_condition());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

namespace ml::globals
{
	// global runtime context
	static runtime_context * g_runtime_context{};

	template <> runtime_context * get() noexcept
	{
		return g_runtime_context;
	}

	template <> runtime_context * set(runtime_context * value) noexcept
	{
		return g_runtime_context = value;
	}
}