#ifndef _ML_TREE_NODE_HPP_
#define _ML_TREE_NODE_HPP_

#include <modus_core/system/Memory.hpp>

namespace ml
{
	struct tree_node : non_copyable, trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type					= typename tree_node;
		using allocator_type			= typename pmr::polymorphic_allocator<byte>;
		using iterator					= typename list<tree_node *>::iterator;
		using const_iterator			= typename list<tree_node *>::const_iterator;
		using reverse_iterator			= typename list<tree_node *>::reverse_iterator;
		using const_reverse_iterator	= typename list<tree_node *>::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t npos{ static_cast<size_t>(-1) };

		static constexpr cstring default_name{ "New Node" };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~tree_node() noexcept override { clear_children(); }

		tree_node(self_type * parent, string const & name, allocator_type alloc = {})
			: m_name	{ name.empty() ? default_name : name, alloc }
			, m_parent	{ parent }
			, m_children{ alloc }
		{
		}

		tree_node(string const & name, allocator_type alloc = {})
			: m_name	{ name.empty() ? default_name : name, alloc }
			, m_parent	{ nullptr }
			, m_children{ alloc }
		{
		}

		tree_node(self_type && other, allocator_type alloc = {}) noexcept
			: m_name	{ alloc }
			, m_parent	{ nullptr }
			, m_children{ alloc }
		{
			this->swap(std::move(other));
		}

		self_type & operator=(self_type && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(self_type & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_name, other.m_name);
				std::swap(m_parent, other.m_parent);
				std::swap(m_children, other.m_children);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator[](size_t i) noexcept -> self_type * { return m_children[i]; }

		ML_NODISCARD auto operator[](size_t i) const noexcept -> self_type const * { return m_children[i]; }

		ML_NODISCARD auto get_child(size_t i) noexcept -> self_type * { return m_children[i]; }

		ML_NODISCARD auto get_child(size_t i) const noexcept -> self_type const * { return m_children[i]; }

		ML_NODISCARD auto get_children() noexcept -> list<self_type *> & { return m_children; }

		ML_NODISCARD auto get_children() const noexcept -> list<self_type *> const & { return m_children; }

		ML_NODISCARD auto get_child_count() const noexcept -> size_t { return m_children.size(); }
		
		ML_NODISCARD auto get_name() const noexcept -> string const & { return m_name; }

		ML_NODISCARD auto get_parent() const noexcept -> self_type * { return m_parent; }

		ML_NODISCARD auto get_root() const noexcept -> self_type * { return m_parent ? m_parent->get_root() : const_cast<self_type *>(this); }

		ML_NODISCARD auto get_sibling_count() const noexcept -> size_t { return m_parent ? m_parent->get_child_count() : 0u; }

		ML_NODISCARD auto get_sibling_index() const noexcept -> size_t { return m_parent ? (size_t)std::distance(m_parent->begin(), m_parent->fetch(this)) : npos; }

		ML_NODISCARD bool is_root() const noexcept { return m_parent == nullptr; }

		ML_NODISCARD bool is_leaf() const noexcept { return m_children.empty(); }

		ML_NODISCARD bool is_child_of(self_type const * other) const noexcept { return is_child_of_recursive(this, other); }

		ML_NODISCARD bool is_parent_of(self_type const * other) const noexcept { return is_parent_of_recursive(this, other); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Derived = self_type, class ... Args
		> auto new_child(string const & name, Args && ... args) -> Derived *
		{
			return (Derived *)m_children.emplace_back
			(
				ML_new(Derived, this, name, ML_forward(args)...)
			);
		}

		bool add_child(self_type * value)
		{
			return value && value->set_parent(this);
		}

		void delete_child(size_t i)
		{
			if (i < get_child_count())
			{
				iterator const it{ begin() + (ptrdiff_t)i };
				
				ML_delete(*it);
				
				m_children.erase(it);
			}
		}

		void clear_children()
		{
			std::for_each(rbegin(), rend(), [&](self_type * child) noexcept
			{
				child->clear_children();

				delete child;
			});
			m_children.clear();
		}

		void detatch_children()
		{
			std::for_each(begin(), end(), [&](tree_node * child) noexcept
			{
				if (child->m_parent = m_parent)
				{
					m_parent->m_children.push_back(child);
				}
			});
			m_children.clear();
		}

		void set_name(string const & value) noexcept
		{
			if (m_name != value)
			{
				m_name = !value.empty() ? value : default_name;
			}
		}

		bool set_parent(self_type * value)
		{
			if (this == value) { return false; }
			else if (value && value->is_child_of(this)) { return false; }
			else
			{
				if (value) { value->m_children.push_back(this); }

				if (m_parent) { m_parent->m_children.erase(m_parent->fetch(this)); }

				m_parent = value;

				return true;
			}
		}

		void set_sibling_index(size_t i)
		{
			if (m_parent && i < get_sibling_count())
			{
				if (auto it{ m_parent->fetch(this) }; it != m_parent->end())
				{
					std::iter_swap(it, m_parent->begin() + (ptrdiff_t)i);
				}
			}
		}

		void set_as_first_sibling()
		{
			if (m_parent)
			{
				std::iter_swap(m_parent->fetch(this), m_parent->begin());
			}
		}

		void set_as_last_sibling()
		{
			if (m_parent)
			{
				std::iter_swap(m_parent->fetch(this), m_parent->end() - 1);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto fetch(self_type const * value) noexcept -> iterator
		{
			if (!value || !value->m_parent || value->m_parent != this) { return end(); }
			else
			{
				for (size_t i = 0, imax = m_children.size(); i < imax; ++i)
				{
					if (m_children[i] == value) { return std::next(begin(), (ptrdiff_t)i); }
				}
				return end();
			}
		}

		ML_NODISCARD auto fetch(self_type const * value) const noexcept -> const_iterator
		{
			if (!value || !value->m_parent || value->m_parent != this) { return end(); }
			else
			{
				for (size_t i = 0, imax = m_children.size(); i < imax; ++i)
				{
					if (m_children[i] == value) { return std::next(begin(), (ptrdiff_t)i); }
				}
				return end();
			}
		}

		template <class Pr> ML_NODISCARD auto find_if(Pr && pr) noexcept -> iterator
		{
			return std::find_if(begin(), end(), ML_forward(pr));
		}

		template <class Pr> ML_NODISCARD auto find_if(Pr && pr) const noexcept -> const_iterator
		{
			return std::find_if(begin(), end(), ML_forward(pr));
		}

		ML_NODISCARD auto find(string const & value) noexcept -> iterator
		{
			return this->find_if([&](self_type const * e) noexcept
			{
				return e && e->m_name == value;
			});
		}

		ML_NODISCARD auto find(string const & value) const noexcept -> const_iterator
		{
			return this->find_if([&](self_type const * e) noexcept
			{
				return e && e->m_name == value;
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> iterator { return m_children.begin(); }

		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_children.begin(); }

		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_children.cbegin(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_children.end(); }

		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_children.end(); }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_children.cend(); }

		ML_NODISCARD auto rbegin() noexcept -> reverse_iterator { return m_children.rbegin(); }

		ML_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return m_children.rbegin(); }

		ML_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return m_children.crbegin(); }

		ML_NODISCARD auto rend() noexcept -> reverse_iterator { return m_children.rend(); }

		ML_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return m_children.rend(); }

		ML_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return m_children.crend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		ML_NODISCARD static bool is_child_of_recursive(self_type const * self, self_type const * other)
		{
			if (!self || !self->m_parent) { return false; }
			else if (!other || !other->m_parent) { return true; }
			else if (self->m_parent == other) { return true; }
			else
			{
				for (self_type * child : other->m_children)
				{
					if (is_child_of_recursive(self, child))
					{
						return true;
					}
				}
				return false;
			}
		}

		ML_NODISCARD static bool is_parent_of_recursive(self_type const * self, self_type const * other)
		{
			if (!self || !other) { return false; }
			else if (!self->m_parent) { return true; }
			else if (!other->m_parent) { return false; }
			else if (other->m_parent == self) { return true; }
			else
			{
				for (self_type * child : self->m_children)
				{
					if (is_parent_of_recursive(child, other))
					{
						return true;
					}
				}
				return false;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		string				m_name		; // name
		self_type *				m_parent	; // parent
		list<self_type *>	m_children	; // children

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TREE_NODE_HPP_