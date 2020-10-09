#ifndef _ML_SCENE_HPP_
#define _ML_SCENE_HPP_

#include <Export.hpp>
#include <system/Memory.hpp>
#include <detail/Matrix.hpp>
#include <entt/entt.hpp>

namespace ml
{
	struct entity;
	struct scene_manager;

	struct ML_CORE_API scene final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using entity_list = typename pmr::vector<shared<entity>>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		scene(scene_manager * manager, allocator_type alloc) noexcept;

		~scene() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shared<entity> & create_entity(pmr::string const & name, allocator_type alloc = {}) noexcept;

		void destroy_entity(shared<entity> const & value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_from_file(fs::path const & path);

		bool load_from_memory(json const & j);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_entities() noexcept -> entity_list & { return m_entities; }

		ML_NODISCARD auto get_manager() const noexcept -> scene_manager * { return m_manager; }

		ML_NODISCARD auto get_registry() noexcept -> entt::registry & { return m_registry; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		scene_manager * const	m_manager	; // manager
		entity_list				m_entities	; // entities
		entt::registry			m_registry	; // registry

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SCENE_HPP_