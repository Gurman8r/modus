#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/engine/EngineEvents.hpp>
#include <modus_core/embed/Python.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	core_application::core_application(int32 argc, char * argv[], allocator_type alloc)
		: m_exit_code		{ EXIT_SUCCESS }
		, m_app_file_name	{ argv[0] }
		, m_app_file_path	{ fs::current_path() }
		, m_app_name		{ fs::path{ argv[0] }.stem().string(), alloc }
		, m_app_version		{ alloc }
		, m_arguments		{ argv, argv + argc, alloc }
		, m_library_paths	{ alloc }
	{
		ML_assert(begin_singleton<core_application>(this));
	}

	core_application::~core_application() noexcept
	{
		finalize_interpreter();

		ML_assert(end_singleton<core_application>(this));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool core_application::has_interpreter() const
	{
		return Py_IsInitialized();
	}

	bool core_application::initialize_interpreter()
	{
		if (Py_IsInitialized()) { return false; }
		PyObject_SetArenaAllocator(std::invoke([]() noexcept
		{
			static PyObjectArenaAllocator al
			{
				pmr::get_default_resource(),
				[](auto mres, size_t s)
				{
					return ((pmr::memory_resource *)mres)->allocate(s);
				},
				[](auto mres, void * p, size_t s)
				{
					return ((pmr::memory_resource *)mres)->deallocate(p, s);
				}
			};
			return &al;
		}));
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
}

// global core_application
namespace ml::globals
{
	static core_application * g_core_application{};

	ML_impl_global(core_application) get() noexcept
	{
		return g_core_application;
	}

	ML_impl_global(core_application) set(core_application * value) noexcept
	{
		return g_core_application = value;
	}
}