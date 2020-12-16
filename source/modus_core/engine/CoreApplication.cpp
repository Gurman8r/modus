#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/engine/EngineEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	core_application::core_application(int32 argc, char * argv[], json const & attributes, allocator_type alloc)
		: m_exit_code		{ EXIT_SUCCESS }
		, m_app_file_name	{ argv[0] }
		, m_app_file_path	{ fs::current_path() }
		, m_app_name		{ fs::path{ argv[0] }.stem().string(), alloc }
		, m_app_version		{ alloc }
		, m_arguments		{ argv, argv + argc, alloc }
		, m_attributes		{ json{ attributes } }
		, m_library_paths	{ alloc }
	{
		ML_verify(begin_singleton<core_application>(this));

		if (has_attr("app_name")) { set_app_name(attr("app_name")); }
		if (has_attr("app_version")) { set_app_version(attr("app_version")); }
		if (has_attr("library_paths")) { set_library_paths(attr("library_paths")); }
	}

	core_application::~core_application() noexcept
	{
		ML_verify(end_singleton<core_application>(this));
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