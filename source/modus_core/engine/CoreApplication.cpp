#include <modus_core/engine/CoreApplication.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	core_application::core_application(int32 argc, char * argv[], json const & j, allocator_type alloc)
		: m_exit_code		{ EXIT_SUCCESS }
		, m_app_data_path	{}
		, m_app_file_name	{ argv[0] }
		, m_app_file_path	{ fs::current_path() }
		, m_app_name		{ fs::path{ argv[0] }.stem().string(), alloc }
		, m_app_version		{ alloc }
		, m_arguments		{ argv, argv + argc, alloc }
		, m_attributes		{ json{ j } }
		, m_library_paths	{ alloc }
		, m_interpreter		{}
	{
		ML_verify(begin_singleton<core_application>(this));

		if (has_attr("app_data_path"))	{ attr("app_data_path")	.get_to(m_app_data_path); }
		if (has_attr("app_name"))		{ attr("app_name")		.get_to(m_app_name); }
		if (has_attr("app_version"))	{ attr("app_version")	.get_to(m_app_version); }
		if (has_attr("arguments"))		{ attr("arguments")		.get_to(m_arguments); }
		if (has_attr("library_paths"))	{ attr("library_paths")	.get_to(m_library_paths); }
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