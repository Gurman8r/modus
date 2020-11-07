#include <modus_core/engine/Plugin.hpp>
#include <modus_core/engine/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin::plugin(plugin_manager * manager, void * userptr) noexcept
		: engine_base	{ manager->get_context() }
		, m_app			{ manager->get_app() }
		, m_manager		{ manager }
		, m_userptr		{ userptr }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}