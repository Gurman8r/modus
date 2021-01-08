#include <modus_core/runtime/CoreApplication.hpp>
#include <modus_core/embed/Python.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	core_application::core_application(int32 argc, char * argv[], json const & argj, allocator_type alloc)
		: event_listener	{ &m_dispatcher, LLONG_MIN }
		, m_main_timer		{ true }
		, m_exit_code		{ EXIT_SUCCESS }
		, m_app_data_path	{}
		, m_app_file_name	{ argv[0] }
		, m_app_file_path	{ fs::current_path() }
		, m_app_name		{ m_app_file_name.stem().string(), alloc }
		, m_app_version		{ alloc }
		, m_arguments		{ argv, argv + argc, alloc }
		, m_library_paths	{ alloc }
		, m_attributes		{ json{ argj } }

		, m_dispatcher		{ alloc }
		, m_libraries		{ alloc }
		, m_plugins			{ &m_dispatcher, alloc }
	{
		ML_ctor_global(core_application);

		if (auto j{ attr().find("app_data_path") }; j != attr().end()) { j->get_to(m_app_data_path); }
		if (auto j{ attr().find("app_name") }; j != attr().end()) { j->get_to(m_app_name); }
		if (auto j{ attr().find("app_version") }; j != attr().end()) { j->get_to(m_app_version); }
		if (auto j{ attr().find("arguments") }; j != attr().end()) { j->get_to(m_arguments); }
		if (auto j{ attr().find("library_paths") }; j != attr().end()) { j->get_to(m_library_paths); }
	}

	core_application::~core_application() noexcept
	{
		ML_dtor_global(core_application);

		if (is_interpreter_initialized()) { ML_verify(finalize_interpreter()); }

		unsubscribe(); // manual unsubscribe required because we own the bus
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void core_application::on_event(event const & value)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// global core_application
namespace ml::globals
{
	static core_application * g_core_application{};

	ML_impl_global(core_application) get_global() { return g_core_application; }

	ML_impl_global(core_application) set_global(core_application * value) { return g_core_application = value; }
}