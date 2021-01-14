#ifndef _ML_TREE_NODE_HPP_
#define _ML_TREE_NODE_HPP_

#include <modus_core/scene/Variable.hpp>

namespace ml
{
	struct scene_tree;

	// tree node
	struct ML_CORE_API tree_node : non_copyable, trackable, std::enable_shared_from_this<tree_node>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~tree_node() noexcept override = default;

		tree_node(allocator_type alloc = {}) noexcept
			: m_name	{ "New Node", alloc }
			, m_tree	{}
			, m_parent	{}
			, m_children{ alloc }
			, m_value	{}
		{
		}

		tree_node(string const & name, scene_tree * tree, tree_node * parent = {}, allocator_type alloc = {})
			: m_name	{ name.empty() ? "New Node" : name, alloc }
			, m_tree	{ tree }
			, m_parent	{ parent }
			, m_children{ alloc }
			, m_value	{}
		{
		}

		tree_node(tree_node && other, allocator_type alloc = {}) noexcept
			: m_name	{ alloc }
			, m_tree	{}
			, m_parent	{}
			, m_children{ alloc }
			, m_value	{}
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
		template <class T
		> ML_NODISCARD bool has_value() const noexcept { return m_value.holds<T>(); }

		ML_NODISCARD auto get_value() noexcept -> variable & { return m_value; }

		ML_NODISCARD auto get_value() const noexcept -> variable const & { return m_value; }

		template <class T
		> ML_NODISCARD auto get_value() noexcept -> T & { return m_value.get<T>(); }
		
		template <class T
		> ML_NODISCARD auto get_value() const noexcept -> T const & { return m_value.get<T>(); }

		template <class T
		> auto set_value(T && value) noexcept -> T & { return m_value.emplace<T>(ML_forward(value)); }

		template <class T, class ... Args
		> auto set_value(Args && ... args) noexcept -> T & { return m_value.emplace<T>(ML_forward(args)...); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_child(size_t i) noexcept -> ref<tree_node> &
		{
			return m_children[i];
		}

		ML_NODISCARD auto get_child(size_t i) const noexcept -> ref<tree_node> const &
		{
			return m_children[i];
		}

		ML_NODISCARD auto get_child_count() const noexcept -> size_t
		{
			return m_children.size();
		}

		ML_NODISCARD auto get_children() noexcept -> list<ref<tree_node>> &
		{
			return m_children;
		}

		ML_NODISCARD auto get_children() const noexcept -> list<ref<tree_node>> const &
		{
			return m_children;
		}

		ML_NODISCARD auto get_parent() const noexcept -> tree_node *
		{
			return m_parent;
		}

		ML_NODISCARD auto get_root() const noexcept -> tree_node *
		{
			return m_parent ? m_parent->get_root() : const_cast<tree_node *>(this);
		}

		ML_NODISCARD auto get_sibling_count() const noexcept -> size_t
		{
			return m_parent ? m_parent->get_child_count() : 0;
		}

		ML_NODISCARD auto get_sibling_index() const noexcept -> size_t
		{
			if (!m_parent) { return static_cast<size_t>(-1); }
			else
			{
				list<ref<tree_node>> const & v{ m_parent->m_children };
				
				auto const it{ std::find_if(v.begin(), v.end(), [&
				](auto const & e) noexcept { return this == e.get(); }) };
				
				ML_assert(it != v.end());
				
				return (size_t)std::distance(v.begin(), it);
			}
		}

		ML_NODISCARD auto get_tree() const noexcept -> scene_tree *
		{
			return m_tree;
		}

		ML_NODISCARD bool is_child_of(tree_node const * other) const noexcept
		{
			if (!m_parent)
			{
				return false;
			}
			else if ((m_parent == other) || !other || !other->m_parent)
			{
				return true;
			}
			else
			{
				for (auto const & child : other->m_children)
				{
					if (is_child_of(child))
					{
						return true;
					}
				}
				return false;
			}
		}

		ML_NODISCARD bool is_child_of(ref<tree_node> const & other) const noexcept
		{
			return is_child_of(other.get());
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto new_child(string const & name) -> ref<tree_node> &
		{
			return m_children.emplace_back(_ML make_ref<tree_node>(name, m_tree, this));
		}

		bool add_child(ref<tree_node> const & value) noexcept
		{
			return value && value->set_parent(this);
		}

		void delete_child(size_t i)
		{
			if (i < m_children.size())
			{
				m_children.erase(m_children.begin() + (ptrdiff_t)i);
			}
		}

		void clear_children()
		{
			m_children.clear();
		}

		void detatch_children()
		{
			for (auto const & child : m_children)
			{
				if (child->m_parent = m_parent)
				{
					m_parent->m_children.push_back(child);
				}
			}
			m_children.clear();
		}

		bool set_parent(tree_node * value)
		{
			if (!value || (this == value) || value->is_child_of(this)) { return false; }
			
			if (value) { value->m_children.push_back(shared_from_this()); }

			if (m_parent)
			{
				list<ref<tree_node>> & v{ m_parent->m_children };

				auto const it{ std::find_if(v.begin(), v.end(), [&
				](auto const & e) noexcept { return this == e.get(); }) };

				ML_assert(it != v.end());

				v.erase(it);
			}

			m_parent = value;

			return true;
		}

		bool set_parent(ref<tree_node> const & value) noexcept
		{
			return value && set_parent(value.get());
		}

		void set_sibling_index(size_t i)
		{
			if (!m_parent) { return; }
			size_t index{ get_sibling_index() };
			if (index == i) { return; }
			
			static ref<tree_node> temp{ nullptr };
			auto & v{ m_parent->m_children };
			temp = *(v.begin() + (ptrdiff_t)index);
			v.erase(v.begin() + index);
			v.insert(v.begin() + i, temp);
			temp = nullptr;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		template <bool Recursive = false, class Pr
		> ML_NODISCARD auto find_if(Pr && pr) noexcept -> ref<tree_node>
		{
			if (auto const it{ std::find_if(m_children.begin(), m_children.end(), ML_forward(pr)) }
			; it != m_children.end())
			{
				return *it;
			}
			else
			{
				return nullptr;
			}
		}
		
		template <bool Recursive = false, class Pr
		> ML_NODISCARD auto find_if(Pr && pr) const noexcept -> ref<tree_node>
		{
			if (auto const it{ std::find_if(m_children.begin(), m_children.end(), ML_forward(pr)) }
			; it != m_children.end())
			{
				return *it;
			}
			else
			{
				return nullptr;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend scene_tree;

		string					m_name		; // name
		scene_tree *			m_tree		; // tree
		tree_node *				m_parent	; // parent
		list<ref<tree_node>>	m_children	; // children
		variable				m_value		; // value
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TREE_NODE_HPP_