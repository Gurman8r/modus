#ifndef _ML_ENTITY_HPP_
#define _ML_ENTITY_HPP_

#include <modus_core/scene/SceneTree.hpp>

namespace ml
{
	struct ML_CORE_API entity : node
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~entity() noexcept override
		{
			scene_tree & tree{ *ML_check(get_tree()) };

			if (tree.m_reg.valid(m_handle))
			{
				tree.m_reg.destroy(m_handle);
			}
		}

		entity(allocator_type alloc = {}) noexcept
			: node		{ "New Entity", nullptr, nullptr, alloc }
			, m_handle	{}
		{
		}

		entity(string const & name, scene_tree * scene, node * parent = nullptr, allocator_type alloc = {})
			: node		{ name, scene, parent, alloc }
			, m_handle	{ ML_check(scene)->m_reg.create() }
		{
		}

		entity(entity && other, allocator_type alloc = {}) noexcept
			: node		{ alloc }
			, m_handle	{}
		{
			entity::swap(*this, std::move(other));
		}

		entity & operator=(entity && other) noexcept
		{
			entity::swap(*this, std::move(other));
			return (*this);
		}

		static void swap(entity & a, entity & b) noexcept
		{
			node::swap((node &)a, (node &)b);
			if (std::addressof(a) != std::addressof(b))
			{
				std::swap(a.m_handle, b.m_handle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD operator entt::entity() const noexcept { return m_handle; }

		ML_NODISCARD auto get_handle() const noexcept -> entt::entity { return m_handle; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool is_valid_entity() const noexcept
		{
			scene_tree & tree{ *ML_check(get_tree()) };

			return tree.m_reg.valid(m_handle);
		}

		template <class T, class ... Args> auto add_component(Args && ... args) noexcept -> T &
		{
			scene_tree & tree{ *ML_check(get_tree()) };
			
			T & c{ tree.m_reg.emplace<T>(m_handle, ML_forward(args)...) };
			
			tree.on_component_added(*this, c);
			
			return c;
		}

		template <class ... T> ML_NODISCARD decltype(auto) get_component() noexcept
		{
			scene_tree & tree{ *ML_check(get_tree()) };

			return tree.m_reg.get<T...>(m_handle);
		}

		template <class ... T> ML_NODISCARD bool has_component() const noexcept
		{
			scene_tree & tree{ *ML_check(get_tree()) };

			return tree.m_reg.has<T...>(m_handle);
		}

		template <class ... T> void remove_component() noexcept
		{
			scene_tree & tree{ *ML_check(get_tree()) };

			tree.m_reg.remove<T...>(m_handle);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend scene_tree;

		entt::entity m_handle; // handle

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_ENTITY_HPP_