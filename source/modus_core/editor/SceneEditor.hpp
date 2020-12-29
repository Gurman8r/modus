#ifndef _ML_SCENE_EDITOR_HPP_
#define _ML_SCENE_EDITOR_HPP_

#include <modus_core/scene/Scene.hpp>

namespace ml
{
	struct ML_CORE_API scene_editor final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		scene_editor(allocator_type alloc = {}) noexcept;

		~scene_editor() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ref<scene> m_active_scene;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCENE_EDITOR_HPP_