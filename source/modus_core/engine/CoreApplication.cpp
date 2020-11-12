#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/embed/Python.hpp>

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
		, m_exit_code		{ EXIT_SUCCESS }
		, m_loop			{ alloc_ref<loop_system>(alloc, get_bus()) }
		, m_dispatcher		{ alloc }
	{
		ML_assert(begin_global<core_application>(this));
		
		subscribe<app_enter_event, app_exit_event, app_idle_event>();
	}

	core_application::~core_application() noexcept
	{
		ML_assert(end_global<core_application>(this));
		
		unsubscribe(); // do this manually because we own the bus
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	int32_t core_application::exec()
	{
		m_uptimer.restart();

		m_loop->process();

		m_uptimer.stop();

		return m_exit_code;
	}

	void core_application::exit(int32_t exit_code)
	{
		m_exit_code = exit_code;

		m_loop->set_loop_condition(nullptr); // FIXME: this is a hack
	}

	void core_application::quit()
	{
		this->exit(EXIT_SUCCESS);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool core_application::initialize_interpreter()
	{
		if (Py_IsInitialized()) { return false; }

		PyObjectArenaAllocator al;
		al.ctx = pmr::get_default_resource();
		al.alloc = [](auto mres, size_t s) {
			return ((pmr::memory_resource *)mres)->allocate(s);
		};
		al.free = [](auto mres, void * p, size_t s) {
			return ((pmr::memory_resource *)mres)->deallocate(p, s);
		};
		PyObject_SetArenaAllocator(&al);

		Py_SetProgramName(get_app_file_name().c_str());
		
		Py_SetPythonHome(get_library_paths()[0].c_str());
		
		Py_InitializeEx(1);

		return Py_IsInitialized();
	}

	bool core_application::finalize_interpreter()
	{
		return Py_IsInitialized() && (Py_FinalizeEx() == EXIT_SUCCESS);
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

// global core_application
namespace ml::globals
{
	static core_application * g_core_app{};

	ML_impl_global(core_application) get() noexcept
	{
		return g_core_app;
	}

	ML_impl_global(core_application) set(core_application * value) noexcept
	{
		return g_core_app = value;
	}
}