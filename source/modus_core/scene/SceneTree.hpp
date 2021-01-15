#ifndef _ML_SCENE_TREE_HPP_
#define _ML_SCENE_TREE_HPP_

#include <modus_core/detail/Duration.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <modus_core/scene/Node.hpp>
#include <modus_core/system/EventSystem.hpp>
#include <entt/entt.hpp>

namespace ml
{
	struct entity;

	struct ML_CORE_API scene_tree : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~scene_tree() noexcept override;

		scene_tree(string const & name, allocator_type alloc = {}) noexcept
			: m_name{ name.empty() ? "New Scene" : name, alloc }
			, m_reg	{}
			, m_root{ _ML make_ref<node>(name, this, nullptr, alloc) }
		{
		}

		scene_tree(scene_tree && other, allocator_type alloc = {}) noexcept
			: m_name{ alloc }
			, m_reg	{}
			, m_root{}
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
				std::swap(m_name, other.m_name);
				std::swap(m_reg, other.m_reg);
				std::swap(m_root, other.m_root);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_name() const noexcept -> string const & { return m_name; }

		ML_NODISCARD auto get_reg() noexcept -> entt::registry & { return m_reg; }

		ML_NODISCARD auto get_reg() const noexcept -> entt::registry const & { return m_reg; }

		ML_NODISCARD auto get_root() noexcept -> ref<node> & { return m_root; }

		ML_NODISCARD auto get_root() const noexcept -> ref<node> const & { return m_root; }

		void set_name(string const & name) noexcept { if (m_name != name) { m_name = name; } }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		void on_runtime_update(duration dt);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		template <class T> void on_component_added(entity &, T &) {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend node;
		friend entity;

		string			m_name	; // name
		entt::registry	m_reg	; // registry
		ref<node>		m_root	; // root node

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	inline void from_json(json const & j, scene_tree & v) noexcept {}

	inline void to_json(json & j, scene_tree const & v) noexcept {}
}

#endif // !_ML_SCENE_TREE_HPP_