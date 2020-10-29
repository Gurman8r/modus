#ifndef _ML_DATABASE_HPP_
#define _ML_DATABASE_HPP_

#include <modus_core/detail/HashMap.hpp>
#include <modus_core/detail/Pointer.hpp>
#include <modus_core/detail/String.hpp>
#include <modus_core/detail/NonCopyable.hpp>

namespace ml
{
	struct simple_database final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using category_type = typename ds::hashmap<ds::string, ds::shared_ptr<std::any>>;

		using categories_type = typename ds::hashmap<typeof<>, category_type>;

		using iterator = typename categories_type::iterator;

		using const_iterator = typename categories_type::const_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		simple_database(allocator_type alloc = {}) noexcept : m_categories{ alloc } {}

		// get all categories
		ML_NODISCARD categories_type & all() noexcept
		{
			return m_categories;
		}

		// get category
		template <class Type
		> ML_NODISCARD category_type & category() noexcept
		{
			return this->all()[typeof_v<Type>];
		}

		// get element
		template <class Type, class Key = ds::string
		> ML_NODISCARD ds::shared_ptr<std::any> & element(Key && key) noexcept
		{
			category_type & cat{ this->category<Type>() };
			if (auto const it{ cat.find(ML_forward(key)) }
			; it != cat.end()) { return it->second; }
			else
			{
				return cat[ML_forward(key)] = std::make_shared<std::any>
				(
					std::make_any<Type>()
				);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Type, class Key = ds::string
		> ML_NODISCARD Type & get(Key && key) noexcept
		{
			return *std::any_cast<Type>(this->element<Type>(ML_forward(key)).get());
		}

		template <class Type, class Key = ds::string, class ... Args
		> Type & emplace(Key && key, Args && ... args) noexcept
		{
			return this->element<Type>(ML_forward(key))->emplace<Type>(ML_forward(args)...);
		}

		template <class Type, class Key = ds::string
		> category_type::iterator erase(Key && key) noexcept
		{
			category_type & cat{ this->category<Type>() };
			if (auto const it{ cat.find(ML_forward(key)) }
			; it != cat.end()) { return cat.erase(it); }
			else { return it; }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> iterator { return m_categories.begin(); }
		
		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_categories.begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_categories.cbegin(); }
		
		ML_NODISCARD auto end() noexcept -> iterator { return m_categories.end(); }
		
		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_categories.end(); }
		
		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_categories.cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		categories_type m_categories;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// database variable
	template <class T
	> struct db_ref final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using value_type		= typename std::_Remove_cvref_t<T>;
		using self_type			= typename db_ref<value_type>;
		using pointer			= typename value_type *;
		using const_pointer		= typename value_type const *;
		using reference			= typename value_type &;
		using const_reference	= typename value_type const &;
		using rvalue			= typename value_type &&;

		~db_ref() noexcept
		{
			ML_check(m_db)->erase<T>(m_name);
		}

		db_ref() noexcept
			: m_db	{}
			, m_name{}
			, m_ptr	{}
		{
		}

		template <class ... Args
		> db_ref(simple_database * const db, ds::string const & name, Args && ... args)
			: m_db	{ ML_check(db) }
			, m_name{ name }
			, m_ptr	{ m_db->element<T>(m_name) }
		{
			if (auto const v{ lock() }; v && !v->has_value())
			{
				v->emplace<T>(ML_forward(args)...);
			}
		}

		db_ref(self_type const & other)
			: m_db	{ other.m_db }
			, m_name{ other.m_name }
			, m_ptr	{ other.m_ptr }
		{
		}

		db_ref(self_type && other) noexcept : self_type{}
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		self_type & operator=(self_type const & other)
		{
			self_type temp{ other };
			this->swap(temp);
			return (*this);
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
				std::swap(m_db, other.m_db);
				m_name.swap(other.m_name);
				m_ptr.swap(other.m_ptr);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto type() const noexcept -> typeof<> { return typeof_v<value_type>; }

		ML_NODISCARD auto name() const noexcept -> ds::string const & { return m_name; }

		ML_NODISCARD auto database() const noexcept -> simple_database * { return m_db; }

		ML_NODISCARD auto use_count() const noexcept -> int32_t { return m_ptr.use_count(); }

		ML_NODISCARD bool expired() const noexcept { return m_ptr.expired(); }
		
		ML_NODISCARD auto lock() const noexcept -> ds::shared_ptr<std::any> { return m_ptr.lock(); }

		ML_NODISCARD auto get() const noexcept -> reference { return *std::any_cast<value_type>(ML_check(lock().get())); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator*() const noexcept -> reference { return get(); }

		ML_NODISCARD auto operator->() const noexcept -> pointer { return &get(); }

		reference operator=(const_reference value)
		{
			return get() = value;
		}

		reference operator=(rvalue value) noexcept
		{
			return get() = std::move(value);
		}

		template <class I
		> decltype(auto) operator[](I && index) noexcept
		{
			return get()[ML_forward(index)];
		}

		template <class ... Args
		> decltype(auto) operator()(Args && ... args) noexcept
		{
			return std::invoke(get(), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept { return get().begin(); }
		
		ML_NODISCARD auto begin() const noexcept { return get().begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept { return get().cbegin(); }

		ML_NODISCARD auto end() noexcept { return get().end(); }
		
		ML_NODISCARD auto end() const noexcept { return get().end(); }
		
		ML_NODISCARD auto cend() const noexcept { return get().cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		simple_database *		m_db	; // 
		ds::string				m_name	; // 
		ds::weak_ptr<std::any>	m_ptr	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_DATABASE_HPP_