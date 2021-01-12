#include <modus_core/embed/AddonManager.hpp>

namespace ml
{
	addon::addon(addon_manager * manager, void * userptr)
		: event_listener{ manager->get_bus() }
		, m_manager		{ manager }
		, m_userptr		{ userptr }
	{
	}

	addon::~addon() noexcept
	{
	}
}