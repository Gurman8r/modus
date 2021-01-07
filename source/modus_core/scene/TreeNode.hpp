#ifndef _ML_TREE_NODE_HPP_
#define _ML_TREE_NODE_HPP_

#include <modus_core/detail/Memory.hpp>

namespace ml
{
	struct tree_node : non_copyable, trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type			= typename pmr::polymorphic_allocator<byte>;
		using iterator					= typename ds::list<tree_node *>::iterator;
		using const_iterator			= typename ds::list<tree_node *>::const_iterator;
		using reverse_iterator			= typename ds::list<tree_node *>::reverse_iterator;
		using const_reverse_iterator	= typename ds::list<tree_node *>::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~tree_node() noexcept override
		{
			delete_children();
		}

		tree_node(tree_node * parent, ds::string const & name, allocator_type alloc = {})
			: m_parent	{ parent }
			, m_children{ alloc }
			, m_name	{ name.empty() ? "New Node" : name, alloc }
		{
		}

		tree_node(ds::string const & name, allocator_type alloc = {})
			: m_parent	{ nullptr }
			, m_children{ alloc }
			, m_name	{ name.empty() ? "New Node" : name, alloc }
		{
		}

		tree_node(tree_node && other, allocator_type alloc = {}) noexcept
			: m_parent	{ nullptr }
			, m_children{ alloc }
			, m_name	{ alloc }
		{
			this->swap(std::move(other));
		}

		tree_node & operator=(tree_node && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(tree_node & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_name, other.m_name);
				std::swap(m_parent, other.m_parent);
				std::swap(m_children, other.m_children);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator[](size_t i) noexcept -> tree_node * { return m_children[i]; }

		ML_NODISCARD auto operator[](size_t i) const noexcept -> tree_node const * { return m_children[i]; }

		ML_NODISCARD auto get_child(size_t i) noexcept -> tree_node * { return m_children[i]; }

		ML_NODISCARD auto get_child(size_t i) const noexcept -> tree_node const * { return m_children[i]; }

		ML_NODISCARD auto get_children() noexcept -> ds::list<tree_node *> & { return m_children; }

		ML_NODISCARD auto get_children() const noexcept -> ds::list<tree_node *> const & { return m_children; }

		ML_NODISCARD auto get_child_count() const noexcept -> size_t { return m_children.size(); }

		ML_NODISCARD auto get_name() const noexcept -> ds::string const & { return m_name; }

		ML_NODISCARD auto get_parent() const noexcept -> tree_node * { return m_parent; }

		ML_NODISCARD auto get_root() const noexcept -> tree_node * { return m_parent ? m_parent->get_root() : const_cast<tree_node *>(this); }

		ML_NODISCARD auto get_sibling_index() const noexcept -> size_t { return m_parent ? (size_t)std::distance(m_parent->begin(), m_parent->find(this)) : static_cast<size_t>(-1); }

		ML_NODISCARD bool is_root() const noexcept { return m_parent == nullptr; }

		ML_NODISCARD bool is_leaf() const noexcept { return m_children.empty(); }

		ML_NODISCARD bool is_child_of(tree_node const * value) const noexcept { return tree_node::is_child_of_recursive(value, this); }

		ML_NODISCARD bool is_parent_of(tree_node const * value) const noexcept { return tree_node::is_parent_of_recursive(this, value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Derived = tree_node, class ... Args
		> auto new_child(ds::string const & name, Args && ... args) -> Derived *
		{
			static_assert(std::is_base_of_v<tree_node, Derived>, "invalid node type");
			auto temp{ new Derived{ this, name, ML_forward(args)... } };
			m_children.push_back(temp);
			return temp;
		}

		void delete_child(size_t i)
		{
			if (i < get_child_count())
			{
				iterator const it{ begin() + (ptrdiff_t)i };
				
				delete (*it);
				
				m_children.erase(it);
			}
		}

		void delete_children()
		{
			std::for_each(rbegin(), rend(), [&](tree_node * e) noexcept
			{
				delete e;
			});
			m_children.clear();
		}

		void detatch_children()
		{
			std::for_each(begin(), end(), [&](tree_node * e) noexcept
			{
				e->set_parent(m_parent);
			});
			m_children.clear();
		}

		void set_name(ds::string const & value) noexcept
		{
			if (m_name != value)
			{
				m_name = !value.empty() ? value : "New Node";
			}
		}

		bool set_parent(tree_node * value)
		{
			if (this == value || is_parent_of(value)) { return false; }
			else
			{
				if (value) { value->m_children.push_back(this); }

				if (m_parent) { m_parent->m_children.erase(m_parent->find(this)); }

				m_parent = value;

				return true;
			}
		}

		void set_sibling_index(size_t i)
		{
			if (m_parent && i < get_child_count())
			{
				std::iter_swap(m_parent->find(this), m_parent->begin() + (ptrdiff_t)i);
			}
		}

		void set_as_first_sibling()
		{
			if (m_parent) { std::iter_swap(m_parent->find(this), m_parent->begin()); }
		}

		void set_as_last_sibling()
		{
			if (m_parent) { std::iter_swap(m_parent->find(this), m_parent->end() - 1); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto find(tree_node const * value) noexcept -> iterator
		{
			return std::find(begin(), end(), value);
		}

		ML_NODISCARD auto find(tree_node const * value) const noexcept -> const_iterator
		{
			return std::find(begin(), end(), value);
		}

		template <class Pr> ML_NODISCARD auto find_if(Pr && pr) noexcept -> iterator
		{
			return std::find_if(begin(), end(), ML_forward(pr));
		}

		template <class Pr> ML_NODISCARD auto find_if(Pr && pr) const noexcept -> const_iterator
		{
			return std::find_if(begin(), end(), ML_forward(pr));
		}

		ML_NODISCARD auto find_by_name(ds::string const & value) noexcept -> iterator
		{
			return this->find_if([&](tree_node const * e) noexcept
			{
				return e && e->m_name == value;
			});
		}

		ML_NODISCARD auto find_by_name(ds::string const & value) const noexcept -> const_iterator
		{
			return this->find_if([&](tree_node const * e) noexcept
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
		ML_NODISCARD static bool is_child_of_recursive(tree_node const * parent, tree_node const * child)
		{
			if (!child || child->is_root()) { return false; }
			else if (!parent || parent->is_root()) { return true; }
			else if (parent->find(child) != parent->end()) { return true; }
			else
			{
				for (tree_node * e : parent->m_children)
				{
					if (is_child_of_recursive(e, child))
					{
						return true;
					}
				}
				return false;
			}
		}

		ML_NODISCARD static bool is_parent_of_recursive(tree_node const * parent, tree_node const * child)
		{
			if (!parent || !child) { return false; }
			else if (parent->is_root()) { return true; }
			else if (child->is_root()) { return false; }
			else if (parent->find(child) != parent->end()) { return true; }
			else
			{
				for (tree_node * e : parent->m_children)
				{
					if (is_parent_of_recursive(e, child))
					{
						return true;
					}
				}
				return false;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		tree_node *				m_parent	; // parent
		ds::list<tree_node *>	m_children	; // children
		ds::string				m_name		; // name

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TREE_NODE_HPP_