#ifndef _ML_NODE_HPP_
#define _ML_NODE_HPP_

// WIP

#include <core/Export.hpp>
#include <core/system/Memory.hpp>

namespace ml
{
	struct node : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		node(allocator_type alloc = {}) noexcept
			: m_parent	{}
			, m_children{ alloc }
		{
		}

		node(node * p, std::initializer_list<node *> const & c, allocator_type alloc = {})
			: m_parent	{ p }
			, m_children{ c, alloc }
		{
		}

		node(node * p, pmr::vector<node *> const & c, allocator_type alloc = {})
			: m_parent	{ p }
			, m_children{ c, alloc }
		{
		}

		node(node * p, pmr::vector<node *> && c, allocator_type alloc = {}) noexcept
			: m_parent	{ p }
			, m_children{ std::move(c), alloc }
		{
		}

		node(node const & other, allocator_type alloc = {})
			: node{ other.m_parent, other.m_children, alloc }
		{
		}

		node(node && other, allocator_type alloc = {}) noexcept
			: node{ alloc }
		{
			this->swap(std::move(other));
		}

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
				std::swap(m_parent	, other.m_parent);
				std::swap(m_children, other.m_children);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto get_parent() const noexcept {}

		auto set_parent(node * value) const noexcept {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto get_children() noexcept {}

		auto get_children() const noexcept {}

		auto set_children(pmr::vector<node *> const & c) noexcept {}

		auto add_child(node * value) noexcept {}

		auto remove_child(node * value) noexcept {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		node *				m_parent	; // 
		pmr::vector<node *>	m_children	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NODE_HPP_