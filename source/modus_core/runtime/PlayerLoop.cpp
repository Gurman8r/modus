#include <modus_core/runtime/PlayerLoop.hpp>
#include <modus_core/runtime/RuntimeEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	player_loop::player_loop(runtime_api * api) noexcept
		: runtime_listener	{ api }
		, m_running			{}
		, m_loopcond		{}
	{
		if (!get_global<player_loop>())
		{
			set_global<player_loop>(this);
		}
	}

	player_loop::~player_loop() noexcept
	{
		if (this == get_global<player_loop>())
		{
			set_global<player_loop>(nullptr);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t player_loop::process() noexcept
	{
		// run check
		if (m_running) { return EXIT_FAILURE; }
		else { m_running = true; } ML_defer(&) { m_running = false; };

		// main sequence
		on_enter(); ML_defer(&) { on_exit(); };
		if (!check_loop_condition()) { return EXIT_FAILURE; }
		do
		{
			auto ML_anon{ do_benchmarks(get_io()) };

			on_idle();
		}
		while (check_loop_condition());
		return EXIT_SUCCESS;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global player loop
namespace ml::globals
{
	static player_loop * g_main_loop{};

	template <> player_loop * get() noexcept {
		return g_main_loop;
	}

	template <> player_loop * set(player_loop * value) noexcept {
		return g_main_loop = value;
	}
}