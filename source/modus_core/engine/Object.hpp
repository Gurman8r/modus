#ifndef _ML_OBJECT_HPP_
#define _ML_OBJECT_HPP_

// WIP

#include <modus_core/detail/Events.hpp>
#include <modus_core/detail/Timer.hpp>

namespace ml
{
	struct ML_CORE_API core_object : event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		explicit core_object(event_bus * bus) noexcept;

		virtual ~core_object() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using event_listener::get_bus;

	protected:
		using event_listener::subscribe;

		using event_listener::unsubscribe;

		virtual void on_event(event const &) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_OBJECT_HPP_