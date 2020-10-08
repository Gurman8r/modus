#include <core/client/PluginManager.hpp>
#include <core/client/ClientEvents.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin::plugin(plugin_manager * manager, void * user) noexcept
		: client_object	{ ML_check(manager)->get_context() }
		, m_manager		{ manager }
		, m_userptr		{ user }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}