#include <modus_core/engine/CoreApplication.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	core_application::core_application(int32_t argc, char * argv[], allocator_type alloc)
		: core_object		{ &m_dispatcher }
		, m_app_file_name	{ argv[0] }
		, m_app_file_path	{ fs::current_path() }
		, m_app_name		{ fs::path{ argv[0] }.stem().string() }
		, m_app_version		{ alloc }
		, m_arguments		{ argv, argv + argc, alloc }
		, m_lib_paths		{ alloc }
		, m_uptimer			{ false }
		, m_should_close	{ false }
		, m_exit_code		{ EXIT_SUCCESS }
		, m_dispatcher		{ alloc }
		, m_loop			{ alloc_ref<loop_system>(alloc, get_bus()) }
	{
		subscribe<app_enter_event, app_exit_event, app_idle_event>();

		m_loop->set_loop_condition([&]() { return !should_close(); });
		m_loop->set_enter_callback([&]() { post_event<app_enter_event>(); });
		m_loop->set_exit_callback([&]() { post_event<app_exit_event>(); });
		m_loop->set_idle_callback([&]() { post_event<app_idle_event>(); });
	}

	core_application::~core_application() noexcept
	{
		unsubscribe();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t core_application::exec()
	{
		m_uptimer.restart();

		set_exit_code(m_loop->process());

		m_uptimer.stop();

		return exit_code();
	}

	void core_application::exit(int32_t exit_code)
	{
		set_exit_code(exit_code);

		set_should_close(true);
	}

	void core_application::quit()
	{
		set_exit_code(EXIT_SUCCESS);

		set_should_close(true);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void core_application::on_event(event const & value)
	{
		switch (value)
		{
		case app_enter_event::ID: {
			auto && ev{ (app_enter_event &&)value };
		} break;

		case app_exit_event::ID: {
			auto && ev{ (app_exit_event &&)value };
		} break;

		case app_idle_event::ID: {
			auto && ev{ (app_idle_event &&)value };
		} break;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}