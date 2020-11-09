#include <modus_core/engine/Plugin.hpp>
#include <modus_core/engine/Application.hpp>

namespace ml
{
	core_plugin::core_plugin(core_application * app, void * userptr) noexcept
		: core_object	{ app->get_bus() }
		, m_userptr		{ userptr }
	{
	}
}