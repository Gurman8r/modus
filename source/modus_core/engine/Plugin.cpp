#include <modus_core/engine/PluginManager.hpp>
#include <modus_core/engine/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin::plugin(plugin_manager * manager, void * userptr)
		: core_object	{ manager->get_app()->get_bus() }
		, m_app			{ manager->get_app() }
		, m_manager		{ manager }
		, m_userptr		{ userptr }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}