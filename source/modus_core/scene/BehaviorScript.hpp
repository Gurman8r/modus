#ifndef _ML_BEHAVIOR_SCRIPT_HPP_
#define _ML_BEHAVIOR_SCRIPT_HPP_

#include <modus_core/scene/Entity.hpp>

namespace ml
{
	struct behavior_script
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		behavior_script(entity const & ent) noexcept : m_entity{ ent } {}

		virtual ~behavior_script() noexcept = default;

	protected:
		virtual void on_create() = 0;

		virtual void on_destroy() = 0;
		
		virtual void on_update(duration) = 0;

	private:
		friend scene_tree;

		entity m_entity;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_BEHAVIOR_SCRIPT_HPP_