#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	plugin::plugin(plugin_manager * manager, void * user) noexcept
		: runtime_listener	{ ML_check(manager)->get_api() }
		, m_manager			{ manager }
		, m_userptr			{ user }
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}