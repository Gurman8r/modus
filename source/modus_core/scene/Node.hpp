#ifndef _ML_NODE_HPP_
#define _ML_NODE_HPP_

// WIP

#include <modus_core/Export.hpp>
#include <modus_core/system/Memory.hpp>

namespace ml
{
	struct node : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using child = typename unique<node>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~node() override
		{
			this->clear_children();
		}

		node(allocator_type alloc = {}) noexcept
			: m_parent	{}
			, m_children{ alloc }
		{
		}

		node(std::initializer_list<node *> const & c, allocator_type alloc = {})
			: m_parent	{}
			, m_children{ c, alloc }
		{
			this->set_children(c);
		}

		explicit node(node * p, pmr::vector<node *> const & c, allocator_type alloc = {})
			: m_parent	{ p }
			, m_children{ alloc }
		{
			this->set_children(c);
		}

		explicit node(node * p, pmr::vector<node *> && c, allocator_type alloc = {}) noexcept
			: m_parent	{ p }
			, m_children{ alloc }
		{
			this->set_children(std::move(c));
		}

		node(node && other, allocator_type alloc = {}) noexcept
			: m_parent	{ other.m_parent }
			, m_children{ std::move(other.m_children), alloc }
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

		ML_NODISCARD auto get_parent() const noexcept -> node * { return m_parent; }

		void set_parent(node * value) noexcept { m_parent = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_children() noexcept -> pmr::vector<node *> & { return m_children; }

		ML_NODISCARD auto get_children() const noexcept -> pmr::vector<node *> const & { return m_children; }

		void set_children(pmr::vector<node *> const & value) noexcept
		{
			for (node * c : (m_children = value))
			{
				c->set_parent(this);
			}
		}

		ML_NODISCARD bool has_child(node * value) const noexcept
		{
			return end() != std::find(begin(), end(), value);
		}

		void add_child(node * value) noexcept
		{
			if (!has_child(value)) { m_children.push_back(value); }
		}

		void remove_child(node * value) noexcept
		{
			if (auto const it{ std::find(begin(), end(), value) }; it != end())
			{
				delete *it;
				m_children.erase(it);
			}
		}

		void clear_children() noexcept
		{
			for (node * c : m_children) { delete c; }
			m_children.clear();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto begin() noexcept -> pmr::vector<node *>::iterator { return m_children.begin(); }
		
		auto begin() const noexcept -> pmr::vector<node *>::const_iterator { return m_children.begin(); }
		
		auto cbegin() const noexcept -> pmr::vector<node *>::const_iterator { return m_children.cbegin(); }
		
		auto end() noexcept -> pmr::vector<node *>::iterator { return m_children.end(); }
		
		auto end() const noexcept -> pmr::vector<node *>::const_iterator { return m_children.end(); }
		
		auto cend() const noexcept -> pmr::vector<node *>::const_iterator { return m_children.cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		node *				m_parent	; // 
		pmr::vector<node *>	m_children	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NODE_HPP_