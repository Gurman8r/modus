#ifndef _ML_ENGINE_EVENTS_HPP_
#define _ML_ENGINE_EVENTS_HPP_

#include <modus_core/detail/EventSystem.hpp>
#include <modus_core/detail/Duration.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(app_enter_event) {};

	ML_event(app_exit_event) {};

	ML_event(app_idle_event)
	{
		duration const delta_time;

		constexpr app_idle_event(duration const & delta_time) noexcept
			: delta_time{ delta_time }
		{
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_ENGINE_EVENTS_HPP_