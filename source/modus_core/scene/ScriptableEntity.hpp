#ifndef _ML_SCRIPTABLE_ENTITY_HPP_
#define _ML_SCRIPTABLE_ENTITY_HPP_

#include <modus_core/scene/Entity.hpp>

namespace ml
{
	struct scriptable_entity : entity
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~scriptable_entity() noexcept = default;

		using entity::entity;

	protected:
		virtual void on_create() {}

		virtual void on_destroy() {}
		
		virtual void on_update(duration) {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCRIPTABLE_ENTITY_HPP_