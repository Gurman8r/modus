#include <modus_core/engine/Application.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin::plugin(plugin_manager * manager, void * userptr)
		: event_listener{ manager->get_bus() }
		, m_app			{ manager->get_app() }
		, m_manager		{ manager }
		, m_userptr		{ userptr }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}