#include <modus_core/client/PluginManager.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin::plugin(plugin_manager * manager, void * user) noexcept
		: client_listener	{ ML_check(manager)->get_context() }
		, m_manager		{ manager }
		, m_userptr		{ user }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}