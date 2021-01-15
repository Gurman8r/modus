#ifndef _ML_NODE_HPP_
#define _ML_NODE_HPP_

#include <modus_core/system/Variable.hpp>

namespace ml
{
	struct scene_tree;

	// node
	struct ML_CORE_API node : non_copyable, trackable, std::enable_shared_from_this<node>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type			= typename pmr::polymorphic_allocator<byte>;
		using iterator					= typename list<ref<node>>::iterator;
		using const_iterator			= typename list<ref<node>>::const_iterator;
		using reverse_iterator			= typename list<ref<node>>::reverse_iterator;
		using const_reverse_iterator	= typename list<ref<node>>::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		node(allocator_type alloc = {}) noexcept
			: m_name	{ "New Node", alloc }
			, m_tree	{}
			, m_parent	{}
			, m_children{ alloc }
			, m_value	{}
		{
		}

		node(string const & name, scene_tree * tree, ref<node> const & parent = {}, allocator_type alloc = {})
			: m_name	{ name.empty() ? "New Node" : name, alloc }
			, m_tree	{ tree }
			, m_parent	{ parent }
			, m_children{ alloc }
			, m_value	{}
		{
		}

		node(node && other, allocator_type alloc = {}) noexcept
			: m_name	{ alloc }
			, m_tree	{}
			, m_parent	{}
			, m_children{ alloc }
			, m_value	{}
		{
			this->swap(std::move(other));
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
				std::swap(m_name, other.m_name);
				std::swap(m_tree, other.m_tree);
				std::swap(m_parent, other.m_parent);
				std::swap(m_children, other.m_children);
				std::swap(m_value, other.m_value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_name() const noexcept -> string const & { return m_name; }

		void set_name(string const & value) noexcept { if (m_name != value) { m_name = value; } }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <class T, class ... Args
		> auto & emplace(Args && ... args) noexcept { return m_value.emplace<T>(ML_forward(args)...); }

		template <class T> ML_NODISCARD bool has() const noexcept { return m_value.has<T>(); }

		template <class T> ML_NODISCARD auto get() & noexcept -> T & { return m_value.get<T>(); }

		template <class T> ML_NODISCARD auto get() const & noexcept -> T const & { return m_value.get<T>(); }

		ML_NODISCARD auto get() noexcept -> variable & { return m_value; }

		ML_NODISCARD auto get() const noexcept -> variable const & { return m_value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_child(size_t i) noexcept -> ref<node> &
		{
			return m_children[i];
		}

		ML_NODISCARD auto get_child(size_t i) const noexcept -> ref<node> const &
		{
			return m_children[i];
		}

		ML_NODISCARD auto get_child_count() const noexcept -> size_t
		{
			return m_children.size();
		}

		ML_NODISCARD auto get_children() noexcept -> list<ref<node>> &
		{
			return m_children;
		}

		ML_NODISCARD auto get_children() const noexcept -> list<ref<node>> const &
		{
			return m_children;
		}

		ML_NODISCARD bool is_orphan() const noexcept
		{
			return m_parent.expired();
		}

		ML_NODISCARD auto get_parent() const noexcept -> ref<node>
		{
			return !is_orphan() ? m_parent.lock() : nullptr;
		}

		ML_NODISCARD auto get_root() const noexcept -> ref<node>
		{
			return !is_orphan() ? get_parent()->get_root() : std::const_pointer_cast<node>(shared_from_this());
		}

		ML_NODISCARD auto get_sibling_count() const noexcept -> size_t
		{
			return !is_orphan() ? get_parent()->get_child_count() : 0;
		}

		ML_NODISCARD auto get_sibling_index() const noexcept -> size_t
		{
			if (is_orphan()) { return static_cast<size_t>(-1); }
			else
			{
				auto const & siblings{ get_parent()->get_children() };
				return (size_t)std::distance(siblings.begin(), std::find_if(siblings.begin(), siblings.end(), [&
				](auto const & e) noexcept { return this == e.get(); }));
			}
		}

		ML_NODISCARD auto get_tree() const noexcept -> scene_tree *
		{
			return m_tree;
		}

		ML_NODISCARD bool is_child_of(ref<node> const & other) const noexcept
		{
			if (is_orphan() || !other || (this == other.get()))
			{
				return false;
			}
			else if (get_parent() == other)
			{
				return true;
			}
			else
			{
				return (bool)other->find_if<true>([&](ref<node> const & child) noexcept
				{
					return is_child_of(child);
				});
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto new_child(string const & name) -> ref<node> &
		{
			return m_children.emplace_back(_ML make_ref<node>(name, m_tree, shared_from_this()));
		}

		void add_child(ref<node> const & value)
		{
			if (value) { value->set_parent(shared_from_this()); }
		}

		void delete_child(size_t i)
		{
			if (i < m_children.size()) { m_children.erase(m_children.begin() + (ptrdiff_t)i); }
		}

		void clear_children()
		{
			m_children.clear();
		}

		void detatch_children()
		{
			for (auto const & child : m_children)
			{
				child->m_parent = m_parent;

				if (!is_orphan()) { get_parent()->m_children.push_back(child); }
			}

			m_children.clear();
		}

		bool set_parent(ref<node> const & value)
		{
			if (auto self{ shared_from_this() }; !value || value->is_child_of(self))
			{
				return false;
			}
			else
			{
				if (value) { value->m_children.push_back(self); }

				if (!is_orphan()) { get_parent()->delete_child(get_sibling_index()); }

				m_parent = value;

				return true;
			}
		}

		void set_sibling_index(size_t i)
		{
			auto self{ shared_from_this() };

			if (!self || is_orphan()) { return; }

			auto & siblings{ get_parent()->m_children };
			
			siblings.erase(siblings.begin() + get_sibling_index());
			
			siblings.insert(siblings.begin() + i, self);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <bool Recursive = false, class Pr
		> auto find_if(Pr && pr) -> ref<node>
		{
			if (auto const it{ std::find_if(begin(), end(), ML_forward(pr)) }; it != end())
			{
				return (*it);
			}
			else if constexpr (Recursive)
			{
				for (auto const & child : m_children)
				{
					if (auto const found{ child->find_if<true>(ML_forward(pr)) })
					{
						return found;
					}
				}
			}
			return nullptr;
		}

		template <bool Recursive = false, class Pr
		> auto find_if(Pr && pr) const -> ref<node>
		{
			if (auto const it{ std::find_if(cbegin(), cend(), ML_forward(pr)) }; it != cend())
			{
				return (*it);
			}
			else if constexpr (Recursive)
			{
				for (auto const & child : m_children)
				{
					if (auto const found{ child->find_if<true>(ML_forward(pr)) })
					{
						return found;
					}
				}
			}
			return nullptr;
		}

		template <bool Recursive = false
		> auto find(node const * value) -> ref<node>
		{
			return this->find_if<Recursive>([&](auto const & e) { return e && e->get() == value; });
		}

		template <bool Recursive = false
		> auto find(ref<node> const & value) const -> ref<node>
		{
			return this->find_if<Recursive>([&](auto const & e) { return e && e == value; });
		}

		template <bool Recursive = false
		> auto find(ref<node> const & value) -> ref<node>
		{
			return this->find_if<Recursive>([&](auto const & e) { return e && e == value; });
		}

		template <bool Recursive = false
		> auto find(node const * value) const -> ref<node>
		{
			return this->find_if<Recursive>([&](auto const & e) { return e && e->get() == value; });
		}

		template <bool Recursive = false
		> auto find(string const & name) -> ref<node>
		{
			return this->find_if<Recursive>([&](auto const & e) { return e && e->get_name() == name; });
		}

		template <bool Recursive = false
		> auto find(string const & name) const -> ref<node>
		{
			return this->find_if<Recursive>([&](auto const & e) { return e && e->get_name() == name; });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
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

	private:
		friend scene_tree;

		string					m_name		; // name
		scene_tree *		m_tree		; // tree
		weak<node>			m_parent	; // parent
		list<ref<node>>		m_children	; // children
		variable				m_value		; // value
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NODE_HPP_