#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin::plugin(plugin_manager * manager, void * userptr)
		: event_listener{ manager->get_bus() }
		, m_manager		{ manager }
		, m_alloc		{ manager->get_allocator() }
		, m_userptr		{ userptr }
	{
	}

	plugin::~plugin() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}