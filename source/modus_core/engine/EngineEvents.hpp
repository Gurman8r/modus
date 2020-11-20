#ifndef _ML_ENGINE_EVENTS_HPP_
#define _ML_ENGINE_EVENTS_HPP_

#include <modus_core/detail/EventSystem.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct core_application;
	struct gui_application;
	struct application;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_event(app_enter_event) {};

	ML_event(app_exit_event) {};

	ML_event(app_idle_event) {};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_ENGINE_EVENTS_HPP_