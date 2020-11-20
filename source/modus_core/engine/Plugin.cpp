#include <modus_core/engine/Application.hpp>
#include <modus_core/engine/PluginManager.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin::plugin(plugin_manager * manager, void * userptr)
		: event_listener{ manager->get_bus() }
		, m_alloc		{ manager->get_allocator() }
		, m_app			{ manager->get_app() }
		, m_manager		{ manager }
		, m_userptr		{ userptr }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}