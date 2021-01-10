#ifndef _ML_NODE_HPP_
#define _ML_NODE_HPP_

#include <modus_core/scene/Object.hpp>

namespace ml
{
	struct scene_tree;

	struct ML_CORE_API node : object
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		static constexpr size_t npos{ static_cast<size_t>(-1) };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~node() noexcept override
		{
			std::for_each(m_children.rbegin(), m_children.rend(), [&](node * child) noexcept
			{
				ML_delete(child);
			});
			m_children.clear();
		}

		node(allocator_type alloc = {}) noexcept
			: object	{ "New Node", alloc }
			, m_tree	{}
			, m_parent	{}
			, m_children{ alloc }
		{
		}

		node(string const & name, scene_tree * tree, node * parent = nullptr, allocator_type alloc = {})
			: object	{ name, alloc }
			, m_tree	{ tree }
			, m_parent	{ parent }
			, m_children{ alloc }
		{
		}

		node(node && other, allocator_type alloc = {}) noexcept
			: object	{ alloc }
			, m_tree	{}
			, m_parent	{}
			, m_children{ alloc }
		{
			node::swap(*this, std::move(other));
		}

		node & operator=(node && other) noexcept
		{
			node::swap(*this, std::move(other));
			return (*this);
		}

		static void swap(node & a, node & b) noexcept
		{
			object::swap((object &)a, (object &)b);
			if (std::addressof(a) != std::addressof(b))
			{
				std::swap(a.m_tree, b.m_tree);
				std::swap(a.m_parent, b.m_parent);
				std::swap(a.m_children, b.m_children);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class Derived = node, class Name, class ... Args
		> auto new_child(Name && name, Args && ... args) -> Derived *
		{
			return (Derived *)(m_children.emplace_back
			(
				ML_new(Derived, ML_forward(name), m_tree, this, ML_forward(args)...)
			));
		}

		bool add_child(node * value) noexcept
		{
			return value && value->set_parent(this);
		}

		void delete_child(size_t i)
		{
			if (i < get_child_count())
			{
				auto const it{ m_children.begin() + (ptrdiff_t)i };

				ML_delete(*it);

				m_children.erase(it);
			}
		}

		void delete_node(node * value)
		{
			if (auto const it{ find_node(value) }; it != m_children.end())
			{
				ML_delete(*it);

				m_children.erase(it);
			}
		}

		void clear_children()
		{
			std::for_each(m_children.rbegin(), m_children.rend(), [&](node * child) noexcept
			{
				child->clear_children();

				ML_delete(child);
			});
			m_children.clear();
		}

		void detatch_children()
		{
			for (node * child : m_children)
			{
				if (child->m_parent = m_parent)
				{
					m_parent->m_children.push_back(child);
				}
			}
			m_children.clear();
		}

		void set_index(size_t i)
		{
		}

		bool set_parent(node * value)
		{
			if (!value || (this == value) || value->is_child_of(this)) { return false; }
			
			if (value) { value->m_children.push_back(this); }

			if (m_parent) { m_parent->m_children.erase(m_parent->find_node(this)); }

			m_parent = value;

			return true;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_child(size_t i) noexcept -> node * { return m_children[i]; }

		ML_NODISCARD auto get_child(size_t i) const noexcept -> node * { return m_children[i]; }

		ML_NODISCARD auto get_child_count() const noexcept -> size_t { return m_children.size(); }

		ML_NODISCARD auto get_children() noexcept -> list<node *> & { return m_children; }
		
		ML_NODISCARD auto get_children() const noexcept -> list<node *> const & { return m_children; }

		ML_NODISCARD auto get_index() const noexcept -> size_t
		{
			if (!m_parent) { return npos; }
			else if (auto const it{ m_parent->find_node(this) }
			; it == m_parent->m_children.end()) { return npos; }
			else { return (size_t)std::distance(m_parent->m_children.begin(), it); }
		}

		ML_NODISCARD auto get_parent() const noexcept -> node * { return m_parent; }

		ML_NODISCARD auto get_root() const noexcept -> node * { return m_parent ? m_parent->get_root() : const_cast<node *>(this); }

		ML_NODISCARD auto get_tree() const noexcept -> scene_tree * { return m_tree; }

		ML_NODISCARD bool is_child_of(node const * other) const noexcept
		{
			if (!m_parent) { return false; }
			else if ((m_parent == other) || (!other || !other->m_parent)) { return true; }
			else
			{
				for (node const * child : other->m_children)
				{
					if (is_child_of(child))
					{
						return true;
					}
				}
				return false;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class Pr
		> ML_NODISCARD auto find_node_if(Pr && pr) noexcept -> list<node *>::iterator
		{
			return std::find_if(m_children.begin(), m_children.end(), ML_forward(pr));
		}

		template <class Pr
		> ML_NODISCARD auto find_node_if(Pr && pr) const noexcept -> list<node *>::const_iterator
		{
			return std::find_if(m_children.begin(), m_children.end(), ML_forward(pr));
		}

		ML_NODISCARD auto find_node(node const * value) noexcept -> list<node *>::iterator
		{
			return std::find(m_children.begin(), m_children.end(), value);
		}

		ML_NODISCARD auto find_node(node const * value) const noexcept -> list<node *>::const_iterator
		{
			return std::find(m_children.begin(), m_children.end(), value);
		}

		ML_NODISCARD auto find_node(string const & value) noexcept -> list<node *>::iterator
		{
			return this->find_node_if([&](auto const & e) { return e && e->get_name() == value; });
		}

		ML_NODISCARD auto find_node(string const & value) const noexcept -> list<node *>::const_iterator
		{
			return this->find_node_if([&](auto const & e) { return e && e->get_name() == value; });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend scene_tree;

		scene_tree *	m_tree		; // tree
		node *			m_parent	; // parent
		list<node *>	m_children	; // children

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NODE_HPP_