#ifndef _ML_NODE_HPP_
#define _ML_NODE_HPP_

// WIP

#include <modus_core/detail/Memory.hpp>

namespace ml
{
	struct node : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type			= typename pmr::polymorphic_allocator<byte>;
		using node_ref					= typename ds::ref<node>;
		using node_list					= typename ds::list<node_ref>;
		using iterator					= typename node_list::iterator;
		using const_iterator			= typename node_list::const_iterator;
		using reverse_iterator			= typename node_list::reverse_iterator;
		using const_reverse_iterator	= typename node_list::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		node(allocator_type alloc = {}) noexcept
			: m_parent	{}
			, m_children{ alloc }
		{
		}

		node(node * parent, allocator_type alloc = {})
			: m_parent	{ parent }
			, m_children{ alloc }
		{
		}

		node(node_list const & children, allocator_type alloc = {})
			: m_parent	{}
			, m_children{ children, alloc }
		{
		}

		node(node * parent, node_list const & children, allocator_type alloc = {})
			: m_parent	{ parent }
			, m_children{ children, alloc }
		{
		}

		node(node const & other, allocator_type alloc = {})
			: m_parent	{ other.m_parent }
			, m_children{ other.m_children, alloc }
		{
		}

		node(node && other, allocator_type alloc = {}) noexcept
			: m_parent	{}
			, m_children{ alloc }
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		node & operator=(node const & other)
		{
			node temp{ other };
			this->swap(temp);
			return (*this);
		}

		node & operator=(node && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(node & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_parent, other.m_parent);
				m_children.swap(other.m_children);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_parent() const noexcept -> node *
		{
			return m_parent;
		}

		auto set_parent(node * value) noexcept -> node *
		{
			return (value != this && value != m_parent) ? (m_parent = value) : nullptr;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto add_child(node_ref const & value) noexcept -> node_ref
		{
			if (this == value.get()) { return nullptr; }
			else if (auto const it{ find_child(value) }
			; it != m_children.end()) { return *it; }
			else { return m_children.emplace_back(value); }
		}

		auto delete_child(node_ref const & value) noexcept -> iterator
		{
			if (this == value.get()) { return m_children.end(); }
			else if (auto const it{ find_child(value) }
			; it == m_children.end()) { return it; }
			else { return m_children.erase(it); }
		}

		template <class Derived = node, class ... Args
		> auto new_child(Args && ... args) noexcept -> ds::ref<Derived>
		{
			static_assert(std::is_base_of_v<node, Derived>, "?");

			return std::static_pointer_cast<Derived>(m_children.emplace_back
			(
				_ML make_ref<Derived>(ML_forward(args)...)
			));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_children() & noexcept -> node_list &
		{
			return m_children;
		}

		ML_NODISCARD auto get_children() const & noexcept -> node_list const &
		{
			return m_children;
		}

		ML_NODISCARD auto get_children() && noexcept -> node_list &&
		{
			return std::move(m_children);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool has_child(node const * value) const noexcept
		{
			return m_children.end() != find_child_if([value](node_ref const & e) noexcept
			{
				return value == e.get();
			});
		}

		ML_NODISCARD bool has_child(node_ref const & value) const noexcept
		{
			return m_children.end() != find_child(value);
		}

		ML_NODISCARD auto find_child(node_ref const & value) noexcept -> iterator
		{
			return std::find(m_children.begin(), m_children.end(), value);
		}

		ML_NODISCARD auto find_child(node_ref const & value) const noexcept -> const_iterator
		{
			return std::find(m_children.cbegin(), m_children.cend(), value);
		}

		template <class Pr
		> ML_NODISCARD auto find_child_if(Pr && pr) noexcept -> iterator
		{
			return std::find_if(m_children.begin(), m_children.end(), ML_forward(pr));
		}

		template <class Pr
		> ML_NODISCARD auto find_child_if(Pr && pr) const noexcept -> const_iterator
		{
			return std::find_if(m_children.cbegin(), m_children.cend(), ML_forward(pr));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		node *		m_parent	;
		node_list	m_children	;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NODE_HPP_