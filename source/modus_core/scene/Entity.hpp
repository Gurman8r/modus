#ifndef _ML_ENTITY_HPP_
#define _ML_ENTITY_HPP_

#include <modus_core/scene/Scene.hpp>

namespace ml
{
	struct entity : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type			= typename pmr::polymorphic_allocator<byte>;
		using self_type					= typename entity;
		using pointer					= typename self_type *;
		using const_pointer				= typename self_type const *;
		using reference					= typename self_type &;
		using const_reference			= typename self_type const &;

		using value_type				= typename pointer;
		using list_type					= typename list<value_type>;
		using size_type					= typename list_type::size_type;
		using difference_type			= typename list_type::difference_type;
		using iterator					= typename list_type::iterator;
		using const_iterator			= typename list_type::const_iterator;
		using reverse_iterator			= typename list_type::reverse_iterator;
		using const_reverse_iterator	= typename list_type::const_reverse_iterator;

		static constexpr size_type npos{ static_cast<size_type>(-1) };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual ~entity() noexcept override { clear_children(); }

		entity(allocator_type alloc = {}) noexcept
			: m_scene	{}
			, m_handle	{}
			, m_parent	{}
			, m_children{ alloc }
		{
		}

		entity(scene_tree * scene, pointer parent = nullptr, allocator_type alloc = {})
			: m_scene	{ scene }
			, m_handle	{ get_registry().create() }
			, m_parent	{ parent }
			, m_children{ alloc }
		{
		}

		entity(self_type && other, allocator_type alloc = {}) noexcept
			: m_scene	{}
			, m_handle	{}
			, m_parent	{}
			, m_children{ alloc }
		{
			self_type::swap(*this, std::move(other));
		}

		self_type & operator=(self_type && other) noexcept
		{
			self_type::swap(*this, std::move(other));
			return (*this);
		}

		static void swap(self_type & a, self_type & b) noexcept
		{
			if (std::addressof(a) != std::addressof(b))
			{
				std::swap(a.m_scene, b.m_scene);
				std::swap(a.m_handle, b.m_handle);
				std::swap(a.m_parent, b.m_parent);
				std::swap(a.m_children, b.m_children);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T, class ... Args> auto add_component(Args && ... args) noexcept -> T &
		{
			T & c{ get_registry().emplace<T>(m_handle, ML_forward(args)...) };
			ML_check(m_scene)->on_component_added(*this, c);
			return c;
		}

		template <class ... T> ML_NODISCARD decltype(auto) get_component() noexcept { return get_registry().get<T...>(m_handle); }

		template <class ... T> ML_NODISCARD bool has_component() const noexcept { return get_registry().has<T...>(m_handle); }

		template <class ... T> void remove_component() noexcept { get_registry().remove<T...>(m_handle); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD operator entity_handle() const noexcept { return m_handle; }

		ML_NODISCARD auto get_child(size_t i) noexcept -> pointer { return m_children[i]; }

		ML_NODISCARD auto get_child(size_t i) const noexcept -> self_type const * { return m_children[i]; }

		ML_NODISCARD auto get_children() noexcept -> list_type & { return m_children; }

		ML_NODISCARD auto get_children() const noexcept -> list_type const & { return m_children; }

		ML_NODISCARD auto get_child_count() const noexcept -> size_t { return m_children.size(); }

		ML_NODISCARD auto get_handle() const noexcept -> entity_handle { return m_handle; }

		ML_NODISCARD auto get_parent() const noexcept -> pointer { return m_parent; }

		ML_NODISCARD auto get_registry() noexcept -> entt::registry & { return ML_check(m_scene)->get_registry(); }

		ML_NODISCARD auto get_registry() const noexcept -> entt::registry const & { return ML_check(m_scene)->get_registry(); }

		ML_NODISCARD auto get_root() const noexcept -> pointer { return m_parent ? m_parent->get_root() : const_cast<pointer>(this); }

		ML_NODISCARD auto get_scene() const noexcept -> scene_tree * { return m_scene; }

		ML_NODISCARD auto get_sibling_index() const noexcept -> size_t
		{
			if (!m_parent) { return npos; }
			else if (auto const it{ m_parent->find(this) }; it == m_parent->end()) { return npos; }
			else { return (size_t)std::distance(m_parent->begin(), it); }
		}

		ML_NODISCARD bool is_child_of(self_type const * other) const noexcept { return self_type::is_child_of_recursive(this, other); }

		ML_NODISCARD bool is_valid() const noexcept { return get_registry().valid(m_handle); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Derived = self_type, class ... Args
		> auto new_child(Args && ... args) -> Derived *
		{
			return static_cast<Derived *>(m_children.emplace_back
			(
				ML_new(Derived, m_scene, this, ML_forward(args)...)
			));
		}

		void delete_child(pointer value)
		{
			if (auto const it{ find(value) }; it != end())
			{
				ML_delete(*it);

				m_children.erase(it);
			}
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

				ML_delete(child);
			});
			m_children.clear();
		}

		void detatch_children()
		{
			std::for_each(begin(), end(), [&](pointer child) noexcept
			{
				if (child->m_parent = m_parent)
				{
					m_parent->m_children.push_back(child);
				}
			});
			m_children.clear();
		}

		bool set_parent(pointer value)
		{
			if (this == value || (value && value->is_child_of(this))) { return false; }
			
			if (value) { value->m_children.push_back(this); }

			if (m_parent) { m_parent->m_children.erase(m_parent->find(this)); }

			m_parent = value;

			return true;
		}

		void set_sibling_index(size_t i)
		{
		}

		void set_as_first_sibling()
		{
		}

		void set_as_last_sibling()
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool contains(const_pointer value) const noexcept { return this->find(value) != end(); }

		ML_NODISCARD auto find(const_pointer value) noexcept -> iterator { return std::find(begin(), end(), value); }

		ML_NODISCARD auto find(const_pointer value) const noexcept -> const_iterator { return std::find(begin(), end(), value); }

		template <class Pr> ML_NODISCARD bool contains_if(Pr && pr) const noexcept { return this->find_if(ML_forward(pr)) != end(); }

		template <class Pr> ML_NODISCARD auto find_if(Pr && pr) noexcept -> iterator { return std::find_if(begin(), end(), ML_forward(pr)); }

		template <class Pr> ML_NODISCARD auto find_if(Pr && pr) const noexcept -> const_iterator { return std::find_if(begin(), end(), ML_forward(pr)); }

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

	public:
		template <class U = self_type
		> ML_NODISCARD auto compare(U const & other) const noexcept
		{
			if constexpr (std::is_same_v<U, self_type>)
			{
				return ML_compare(m_handle, other.m_handle);
			}
		}

		template <class U> ML_NODISCARD bool operator==(U && other) const noexcept { return this->compare(ML_forward(other)) == 0; }

		template <class U> ML_NODISCARD bool operator!=(U && other) const noexcept { return this->compare(ML_forward(other)) != 0; }

		template <class U> ML_NODISCARD bool operator<(U && other) const noexcept { return this->compare(ML_forward(other)) < 0; }

		template <class U> ML_NODISCARD bool operator>(U && other) const noexcept { return this->compare(ML_forward(other)) > 0; }

		template <class U> ML_NODISCARD bool operator<=(U && other) const noexcept { return this->compare(ML_forward(other)) <= 0; }

		template <class U> ML_NODISCARD bool operator>=(U && other) const noexcept { return this->compare(ML_forward(other)) >= 0; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		ML_NODISCARD static bool is_child_of_recursive(self_type const * self, self_type const * other)
		{
			if (!self || !self->m_parent) { return false; }
			else if (!other || !other->m_parent) { return true; }
			else if (self->m_parent == other) { return true; }
			else
			{
				for (const_pointer child : other->m_children)
				{
					if (is_child_of_recursive(self, child))
					{
						return true;
					}
				}
				return false;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		friend scene_tree;

		scene_tree *		m_scene		; // scene
		entity_handle		m_handle	; // handle
		pointer				m_parent	; // parent
		list_type			m_children	; // children

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_ENTITY_HPP_