#include <modus_core/embed/PluginManager.hpp>

namespace ml
{
	native_plugin::native_plugin(plugin_manager * manager, void * userptr)
		: event_listener{ manager->get_bus() }
		, m_manager		{ manager }
		, m_alloc		{ manager->get_allocator() }
		, m_userptr		{ userptr }
	{
	}

	native_plugin::~native_plugin() noexcept {}
}