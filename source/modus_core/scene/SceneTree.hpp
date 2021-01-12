#ifndef _ML_SCENE_TREE_HPP_
#define _ML_SCENE_TREE_HPP_

#include <modus_core/scene/Node.hpp>
#include <modus_core/system/EventSystem.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <entt/entt.hpp>

namespace ml
{
	struct entity;

	ML_alias entity_handle = typename entt::entity;

	ML_alias entity_registry = typename entt::registry;

	struct ML_CORE_API scene_tree : object
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~scene_tree() noexcept override {}

		scene_tree(string const & name, allocator_type alloc = {}) noexcept
			: object{ name, alloc }
			, m_reg	{}
			, m_root{ _ML make_ref<node>(name, this, nullptr, alloc) }
		{
		}

		scene_tree(scene_tree && other, allocator_type alloc = {}) noexcept
			: object{ alloc }
			, m_reg	{}
			, m_root{}
		{
			scene_tree::swap(*this, std::move(other));
		}

		scene_tree & operator=(scene_tree && other) noexcept
		{
			scene_tree::swap(*this, std::move(other));
			return (*this);
		}

		static void swap(scene_tree & a, scene_tree & b) noexcept
		{
			object::swap((object &)a, (object &)b);
			if (std::addressof(a) != std::addressof(b))
			{
				std::swap(a.m_reg, b.m_reg);
				std::swap(a.m_root, b.m_root);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_registry() const noexcept { return const_cast<entity_registry *>(&m_reg); }

		ML_NODISCARD auto get_root() const noexcept -> ref<node> const & { return m_root; }

		void set_root(ref<node> const & value) noexcept { if (m_root != value) { m_root = value; } }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class Derived = node, class Name, class ... Args
		> auto new_node(Name && name, Args && ... args) noexcept -> ref<Derived>
		{
			return m_root->new_child<Derived>(ML_forward(name), ML_forward(args)...);
		}

		template <class Name
		> auto new_entity(Name && name) noexcept -> ref<entity>
		{
			ref<entity> e{ this->new_node<entity>(ML_forward(name)) };
			e->add_component<tag_component>(e->get_name());
			e->add_component<transform_component>();
			return e;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		template <class T> void on_component_added(entity &, T &) {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend node;
		friend entity;

		entity_registry	m_reg	; // registry
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