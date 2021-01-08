#ifndef _ML_SCENE_HPP_
#define _ML_SCENE_HPP_

#include <modus_core/system/EventSystem.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <entt/entt.hpp>

namespace ml
{
	struct entity;

	struct ML_CORE_API scene_tree : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		scene_tree(allocator_type alloc = {}) noexcept;

		virtual ~scene_tree() noexcept override;

		scene_tree(scene_tree && other, allocator_type alloc = {}) noexcept : scene_tree{ alloc }
		{
			this->swap(std::move(other));
		}

		scene_tree & operator=(scene_tree && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(scene_tree & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_reg, other.m_reg);
				std::swap(m_root, other.m_root);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD entity * new_entity(string const & name = "New Entity") noexcept;

		void delete_entity(entity * value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_registry() noexcept -> entt::registry & { return m_reg; }

		ML_NODISCARD auto get_registry() const noexcept -> entt::registry const & { return m_reg; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_root() const noexcept { return const_cast<entity *>(m_root); }

		void set_root(entity * value) noexcept { if (m_root != value) { m_root = value; } }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		template <class T> void on_component_added(entity &, T &) {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend entity;

		entt::registry	m_reg	; // registry
		entity *		m_root	; // root node

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	inline void from_json(json const & j, scene_tree & v) noexcept {}

	inline void to_json(json & j, scene_tree const & v) noexcept {}
}

#endif // !_ML_SCENE_HPP_