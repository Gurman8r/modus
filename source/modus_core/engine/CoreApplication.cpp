#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/embed/Python.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	core_application::core_application(int32_t argc, char * argv[], allocator_type alloc)
		: core_object		{ std::addressof(m_dispatcher) }
		, m_app_file_name	{ argv[0] }
		, m_app_file_path	{ fs::current_path() }
		, m_app_name		{ fs::path{ argv[0] }.stem().string(), alloc }
		, m_app_version		{ alloc }
		, m_arguments		{ argv, argv + argc, alloc }
		, m_lib_paths		{ alloc }

		, m_exit_code		{ EXIT_SUCCESS }
		, m_dispatcher		{ alloc }
		, m_loop			{ alloc_ref<loop_system>(alloc, get_bus()) }
	{
		ML_assert(begin_singleton<core_application>(this));
		
		subscribe<app_enter_event, app_exit_event, app_idle_event>();
	}

	core_application::~core_application() noexcept
	{
		unsubscribe();

		finalize_interpreter();

		ML_assert(end_singleton<core_application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool core_application::initialize_interpreter()
	{
		if (Py_IsInitialized()) { return false; }

		PyObjectArenaAllocator alloc
		{
			get_global<memory_manager>()->get_resource(),
			[](auto mres, size_t s) { return ((pmr::memory_resource *)mres)->allocate(s); },
			[](auto mres, void * p, size_t s) { return ((pmr::memory_resource *)mres)->deallocate(p, s); }
		};
		PyObject_SetArenaAllocator(&alloc);

		Py_SetProgramName(app_file_name().c_str());

		Py_SetPythonHome(library_paths(0).c_str());
		
		Py_InitializeEx(1);

		return Py_IsInitialized();
	}

	void core_application::finalize_interpreter()
	{
		if (Py_IsInitialized())
		{
			Py_FinalizeEx();
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void core_application::on_event(event const & value)
	{
		switch (value)
		{
		case app_enter_event::ID: {
			auto && ev{ (app_enter_event &&)value };
			ML_assert(initialize_interpreter());
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