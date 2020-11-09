#include <modus_core/engine/Object.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	core_object::core_object(event_bus * bus) noexcept
		: event_listener{ bus }
	{
	}

	core_object::~core_object() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}