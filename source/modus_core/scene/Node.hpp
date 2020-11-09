#ifndef _ML_NODE_HPP_
#define _ML_NODE_HPP_

// WIP

#include <modus_core/Export.hpp>
#include <modus_core/detail/Memory.hpp>

namespace ml
{
	struct node : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type	= typename pmr::polymorphic_allocator<byte_t>;
		using iterator			= typename ds::list<node *>::iterator;
		using const_iterator	= typename ds::list<node *>::const_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~node() noexcept override
		{
			clear_children();
		}

		node(allocator_type alloc = {}) noexcept
			: m_name	{ alloc }
			, m_parent	{}
			, m_children{ alloc }
		{
		}

		node(ds::string const & n, allocator_type alloc = {})
			: m_name	{ alloc }
			, m_parent	{}
			, m_children{ alloc }
		{
		}

		node(ds::string const & n, std::initializer_list<node *> const & c, allocator_type alloc = {})
			: m_name	{ n, alloc }
			, m_parent	{}
			, m_children{ alloc }
		{
			set_children(c);
		}

		node(node * p, ds::string const & n, ds::list<node *> const & c, allocator_type alloc = {}) noexcept
			: m_name	{ alloc }
			, m_parent	{ p }
			, m_children{ alloc }
		{
			set_children(c);
		}

		node(node && other, allocator_type alloc = {}) noexcept
			: m_name	{ alloc }
			, m_parent	{}
			, m_children{ alloc }
		{
			swap(std::move(other));
		}

		node & operator=(node && other) noexcept
		{
			swap(std::move(other));
			return (*this);
		}

		void swap(node & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_name.swap(other.m_name);
				std::swap(m_parent, other.m_parent);
				m_children.swap(other.m_children);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto child_count() const noexcept -> size_t { return m_children.size(); }

		ML_NODISCARD auto get_child(size_t i) noexcept -> node * { return m_children[i]; }
		
		ML_NODISCARD auto get_child(size_t i) const noexcept -> node const * { return m_children[i]; }

		ML_NODISCARD auto get_children() noexcept -> ds::list<node *> & { return m_children; }

		ML_NODISCARD auto get_children() const noexcept -> ds::list<node *> const & { return m_children; }

		ML_NODISCARD auto get_name() const noexcept -> ds::string { return m_name; }

		ML_NODISCARD auto get_parent() const noexcept -> node * { return m_parent; }

		ML_NODISCARD bool is_child_of(node const * value) const noexcept
		{
			return m_parent == value;
		}

		ML_NODISCARD bool is_parent_of(node const * value) const noexcept
		{
			return value && value->is_child_of(this);
		}

		node * add_child(node * value) noexcept
		{
			return !is_parent_of(value) ? m_children.emplace_back(value) : nullptr;
		}

		void clear_children() noexcept
		{
			for (node * c : m_children)
			{
				delete c;
				c = nullptr;
			}
			m_children.clear();
		}

		void detach_children()
		{
			for (node * c : m_children)
			{
				c->set_parent(m_parent);
			}
		}

		node * find(ds::string const & name) noexcept
		{
			if (auto const it{ std::find_if(begin(), end(), [&name
			](auto c) { return c && c->m_name == name; }) }
			; it != end())
			{
				return (*it);
			}
			else
			{
				return nullptr;
			}
		}

		void remove_child(node * value) noexcept
		{
			if (auto const it{ std::find(begin(), end(), value) }; it != end())
			{
				delete (*it);
				(*it) = nullptr;
				m_children.erase(it);
			}
		}

		void set_children(ds::list<node *> const & value) noexcept
		{
			for (node * c : (m_children = value))
			{
				c->set_parent(this);
			}
		}

		void set_name(ds::string const & value) noexcept
		{
			m_name = value;
		}

		void set_parent(node * value) noexcept
		{
			m_parent = value;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator[](size_t i) noexcept -> node * { return get_child(i); }

		ML_NODISCARD auto operator[](size_t i) const noexcept -> node const * { return get_child(i); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> iterator { return m_children.begin(); }
		
		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_children.begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_children.cbegin(); }
		
		ML_NODISCARD auto end() noexcept -> iterator { return m_children.end(); }
		
		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_children.end(); }
		
		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_children.cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::string			m_name		; // 
		node *				m_parent	; // 
		ds::list<node *>	m_children	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NODE_HPP_