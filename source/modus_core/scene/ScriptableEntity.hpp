#ifndef _ML_SCRIPTABLE_ENTITY_HPP_
#define _ML_SCRIPTABLE_ENTITY_HPP_

#include <modus_core/scene/Entity.hpp>

namespace ml
{
	struct scriptable_entity : entity
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using entity::entity;

		scriptable_entity() noexcept = default;
		
		scriptable_entity(scriptable_entity &&) noexcept = default;
		
		scriptable_entity & operator=(scriptable_entity &&) noexcept = default;

		virtual ~scriptable_entity() noexcept override = default;

	protected:
		virtual void on_create() {}

		virtual void on_destroy() {}
		
		virtual void on_update(duration) {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		entity m_entity;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCRIPTABLE_ENTITY_HPP_