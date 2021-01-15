#ifndef _ML_ENTITY_HPP_
#define _ML_ENTITY_HPP_

#include <modus_core/scene/SceneTree.hpp>

namespace ml
{
	struct ML_CORE_API entity final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		~entity() noexcept final = default;

		entity() noexcept
			: m_tree	{}
			, m_handle	{}
		{
		}

		explicit entity(scene_tree * tree, entt::entity handle) noexcept
			: m_tree	{ tree }
			, m_handle	{ handle }
		{
		}

		explicit entity(scene_tree * tree) noexcept
			: m_tree	{ tree }
			, m_handle	{ tree ? tree->get_reg().create() : entt::null }
		{
		}

		explicit entity(entity const & other) noexcept
			: m_tree	{ other.m_tree }
			, m_handle	{ other.m_handle }
		{
		}

		explicit entity(entity && other) noexcept
			: m_tree	{}
			, m_handle	{}
		{
			this->swap(std::move(other));
		}

		entity(ref<scene_tree> const & tree, entt::entity handle) noexcept : entity{ tree.get(), handle } {}

		entity(ref<scene_tree> const & tree) noexcept : entity{ tree.get() } {}

		entity & operator=(entity const & other)
		{
			entity temp{ other };
			this->swap(temp);
			return (*this);
		}

		entity & operator=(entity && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(entity & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_tree, other.m_tree);
				std::swap(m_handle, other.m_handle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD operator bool() const noexcept { return this->is_valid(); }

		ML_NODISCARD operator entt::entity() const noexcept { return m_handle; }

		ML_NODISCARD auto get_handle() const noexcept -> entt::entity { return m_handle; }

		ML_NODISCARD auto get_tree() const noexcept -> scene_tree * { return m_tree; }

		ML_NODISCARD bool is_valid() const noexcept { return m_tree && m_tree->m_reg.valid(m_handle); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T, class ... Args
		> auto & add(Args && ... args) noexcept
		{
			ML_assert(is_valid());

			if (m_tree->m_reg.has<T>(m_handle))
			{
				return m_tree->m_reg.get<T>(m_handle);
			}
			else
			{
				T & c{ m_tree->m_reg.emplace<T>(m_handle, ML_forward(args)...) };

				m_tree->on_component_added<T>(*this, c);

				return c;
			}
		}

		template <class ... T
		> ML_NODISCARD decltype(auto) get() noexcept
		{
			ML_assert(is_valid()); return m_tree->m_reg.get<T...>(m_handle);
		}

		template <class ... T
		> ML_NODISCARD bool has() const noexcept
		{
			ML_assert(is_valid()); return m_tree->m_reg.has<T...>(m_handle);
		}

		template <class ... T
		> void remove() noexcept
		{
			ML_assert(is_valid()); m_tree->m_reg.remove<T...>(m_handle);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend scene_tree;

		scene_tree * m_tree; // tree
		entt::entity m_handle; // handle

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_ENTITY_HPP_