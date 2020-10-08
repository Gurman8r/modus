#ifndef _ML_SCENE_MANAGER_HPP_
#define _ML_SCENE_MANAGER_HPP_

#include <core/scene/Entity.hpp>

namespace ml
{
	struct ML_CORE_API scene_manager final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using scene_list = typename pmr::vector<shared<scene>>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		scene_manager(allocator_type alloc = {}) noexcept;

		~scene_manager() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shared<scene> & create_scene(allocator_type alloc = {}) noexcept;

		void destroy_scene(shared<scene> const & value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_scenes() & noexcept -> scene_list & { return m_scenes; }

		ML_NODISCARD auto get_scenes() const & noexcept -> scene_list const & { return m_scenes; }

		ML_NODISCARD auto get_active_scene() const noexcept -> scene * { return m_active_scene; }

		auto set_active_scene(scene * value) noexcept -> scene * { return (m_active_scene = value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		scene_list	m_scenes		; // scenes
		scene *		m_active_scene	; // active scene

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCENE_MANAGER_HPP_