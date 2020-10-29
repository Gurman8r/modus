#ifndef _ML_SET_HPP_
#define _ML_SET_HPP_

#include <modus_core/detail/Utility.hpp>
#include <modus_core/detail/List.hpp>

namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// FLAT SET TRAITS
	template <
		class	_Ty,	// value type
		class	_Pr,	// comparator predicate type
		bool	_Mt,	// true if multiple equivalent values are permitted
		size_t	_Th		// search heuristic
	> struct flat_set_traits final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using value_type		= typename _Ty;
		using compare_type		= typename _Pr;
		using allocator_type	= typename pmr::polymorphic_allocator<byte_t>;
		using difference_type	= typename ptrdiff_t;
		using size_type			= typename size_t;

		static constexpr bool multi{ _Mt };

		static constexpr size_type thresh{ _Th };

		template <class T = value_type
		> using storage_type = typename ds::list<T>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// BASIC FLAT SET
	template <class _Traits
	> struct basic_flat_set
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using traits_type					= typename _Traits;
		using self_type						= typename basic_flat_set<traits_type>;
		using value_type					= typename traits_type::value_type;
		using compare_type					= typename traits_type::compare_type;
		using allocator_type				= typename traits_type::allocator_type;
		using difference_type				= typename traits_type::difference_type;
		using size_type						= typename traits_type::size_type;
		using storage_type					= typename traits_type::template storage_type<>;

		using init_type						= typename std::initializer_list<value_type>;
		using pointer						= typename storage_type::pointer;
		using const_pointer					= typename storage_type::const_pointer;
		using reference						= typename storage_type::reference;
		using const_reference				= typename storage_type::const_reference;
		using rvalue						= typename value_type &&;
		using const_rvalue					= typename value_type const &&;
		
		using iterator						= typename storage_type::iterator;
		using const_iterator				= typename storage_type::const_iterator;
		using reverse_iterator				= typename storage_type::reverse_iterator;
		using const_reverse_iterator		= typename storage_type::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		basic_flat_set(allocator_type alloc = {}) noexcept
			: m_data{ alloc }
		{
		}

		basic_flat_set(init_type value, allocator_type alloc = {})
			: m_data{ value, alloc }
		{
			this->impl_sort();
		}

		template <class It
		> basic_flat_set(It first, It last, allocator_type alloc = {})
			: m_data{ first, last, alloc }
		{
			this->impl_sort();
		}

		basic_flat_set(storage_type const & value, allocator_type alloc = {})
			: m_data{ value, alloc }
		{
			this->impl_sort();
		}

		basic_flat_set(storage_type && value, allocator_type alloc = {}) noexcept
			: m_data{ std::move(value), alloc }
		{
			this->impl_sort();
		}

		basic_flat_set(self_type const & value, allocator_type alloc = {})
			: m_data{ value.m_data, alloc }
		{
		}

		basic_flat_set(self_type && value, allocator_type alloc = {}) noexcept
			: m_data{ std::move(value.m_data), alloc }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		self_type & operator=(init_type value)
		{
			self_type temp{ value };
			this->swap(temp);
			return (*this);
		}

		self_type & operator=(self_type const & value)
		{
			self_type temp{ value };
			this->swap(temp);
			return (*this);
		}

		self_type & operator=(storage_type const & value)
		{
			self_type temp{ value };
			this->swap(temp);
			return (*this);
		}

		self_type & operator=(self_type && value) noexcept
		{
			this->swap(std::move(value));
			return (*this);
		}

		self_type & operator=(storage_type && value) noexcept
		{
			self_type temp{ std::move(value) };
			this->swap(temp);
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class It
		> void assign(It first, It last)
		{
			m_data.assign(first, last);

			this->impl_sort();
		}

		void assign(init_type value)
		{
			m_data.assign(value);

			this->impl_sort();
		}

		void assign(storage_type const & value)
		{
			if (std::addressof(m_data) != std::addressof(value))
			{
				m_data = value;

				this->impl_sort();
			}
		}

		void assign(self_type const & value)
		{
			if (this != std::addressof(value))
			{
				m_data = value.m_data;
			}
		}

		void clear() noexcept
		{
			m_data.clear();
		}

		void pop_back() noexcept
		{
			m_data.pop_back();
		}

		void reserve(size_type const cap)
		{
			m_data.reserve(cap);
		}

		void resize(size_type const cap)
		{
			m_data.resize(cap);
		}

		void shrink_to_fit()
		{
			m_data.shrink_to_fit();
		}

		void swap(self_type & value) noexcept
		{
			if (this != std::addressof(value))
			{
				m_data.swap(value.m_data);
			}
		}

		void swap(storage_type & value) noexcept
		{
			if (std::addressof(m_data) != std::addressof(value))
			{
				m_data.swap(value);

				this->impl_sort();
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Out = difference_type
		> ML_NODISCARD Out index_of(const_iterator it) const noexcept
		{
			return static_cast<Out>(std::distance(cbegin(), it));
		}

		template <class Out = difference_type
		> ML_NODISCARD Out index_of(const_reverse_iterator it) const noexcept
		{
			return static_cast<Out>(std::distance(crbegin(), it));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		iterator erase(iterator it)
		{
			return m_data.erase(it);
		}

		iterator erase(iterator first, iterator last)
		{
			return m_data.erase(first, last);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Value = value_type
		> ML_NODISCARD bool contains(Value && value) const noexcept
		{
			return self_type::impl_contains(begin(), end(), ML_forward(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Value = value_type
		> ML_NODISCARD iterator find(Value && value) noexcept
		{
			return self_type::impl_find(begin(), end(), ML_forward(value));
		}

		template <class Value = value_type
		> ML_NODISCARD const_iterator find(Value && value) const noexcept
		{
			return self_type::impl_find(cbegin(), cend(), ML_forward(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn
		> ML_NODISCARD iterator find_if(Fn && fn) noexcept
		{
			return std::find_if(begin(), end(), ML_forward(fn));
		}

		template <class Fn
		> ML_NODISCARD const_iterator find_if(Fn && fn) const noexcept
		{
			return std::find_if(cbegin(), cend(), ML_forward(fn));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Value = value_type
		> auto insert(Value && value) noexcept
		{
			return this->impl_insert(ML_forward(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Value = value_type
		> ML_NODISCARD auto & find_or_add(Value && value) noexcept
		{
			if (auto const it{ this->find(ML_forward(value)) }; it != this->end())
			{
				return (*it);
			}
			else if constexpr (traits_type::multi)
			{
				return *this->insert(ML_forward(value));
			}
			else
			{
				return *this->insert(ML_forward(value)).first;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator[](size_type const i) noexcept -> reference { return m_data[i]; }

		ML_NODISCARD auto operator[](size_type const i) const noexcept -> const_reference { return m_data[i]; }

		ML_NODISCARD auto at(size_type const i) -> reference { return m_data.at(i); }

		ML_NODISCARD auto at(size_type const i) const -> const_reference { return m_data.at(i); }

		ML_NODISCARD auto back() noexcept -> reference { return m_data.back(); }

		ML_NODISCARD auto back() const noexcept -> const_reference { return m_data.back(); }

		ML_NODISCARD auto begin() noexcept -> iterator { return m_data.begin(); }

		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_data.begin(); }

		ML_NODISCARD auto capacity() const noexcept -> size_type { return m_data.capacity(); }

		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_data.cbegin(); }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_data.cend(); }

		ML_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return m_data.crbegin(); }

		ML_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return m_data.crend(); }

		ML_NODISCARD auto data() noexcept -> pointer { return m_data.data(); }

		ML_NODISCARD auto data() const noexcept -> const_pointer { return m_data.data(); }

		ML_NODISCARD auto elements() const noexcept -> storage_type const & { return m_data; }

		ML_NODISCARD bool empty() const noexcept { return m_data.empty(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_data.end(); }

		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_data.end(); }

		ML_NODISCARD auto front() noexcept -> reference { return m_data.front(); }

		ML_NODISCARD auto front() const noexcept -> const_reference { return m_data.front(); }
		
		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return m_data.get_allocator(); }

		ML_NODISCARD auto max_size() const noexcept -> size_type { return m_data.max_size(); }

		ML_NODISCARD auto rbegin() noexcept -> reverse_iterator { return m_data.rbegin(); }

		ML_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return m_data.rbegin(); }

		ML_NODISCARD auto rend() noexcept -> reverse_iterator { return m_data.rend(); }

		ML_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return m_data.rend(); }

		ML_NODISCARD auto size() const noexcept -> size_type { return m_data.size(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class U = self_type
		> ML_NODISCARD auto compare(U const & value) const noexcept
		{
			if constexpr (std::is_same_v<U, self_type>)
			{
				return this->compare(value.m_data);
			}
			else
			{
				static_assert(std::is_same_v<U, storage_type>);
				return ML_compare(m_data, value.m_data);
			}
		}

		template <class U = self_type
		> ML_NODISCARD bool operator==(U const & value) const noexcept
		{
			return this->compare(value) == 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator!=(U const & value) const noexcept
		{
			return this->compare(value) != 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator<(U const & value) const noexcept
		{
			return this->compare(value) < 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator>(U const & value) const noexcept
		{
			return this->compare(value) > 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator<=(U const & value) const noexcept
		{
			return this->compare(value) <= 0;
		}

		template <class U = self_type
		> ML_NODISCARD bool operator>=(U const & value) const noexcept
		{
			return this->compare(value) >= 0;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:

		// contains implementation
		template <class It, class Value
		> static bool impl_contains(It first, It last, Value && value) noexcept
		{
			auto linear = [&
			]() noexcept { return std::find(first, last, ML_forward(value)) != last; };

			auto binary = [&
			]() noexcept { return std::binary_search(first, last, ML_forward(value), compare_type{}); };

			if constexpr (traits_type::thresh == 0)
			{
				return linear(); // always linear
			}
			else if ((size_t)std::distance(first, last) < traits_type::thresh)
			{
				return linear(); // linear
			}
			else
			{
				return binary(); // binary
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// find implementation
		template <class It, class Value
		> static auto impl_find(It first, It last, Value && value) noexcept
		{
			auto linear = [&
			]() noexcept { return std::find(first, last, ML_forward(value)); };

			auto binary = [&, it = std::equal_range(first, last, ML_forward(value), compare_type{})
			]() noexcept { return (it.first != it.second) ? it.first : last; };

			if constexpr (traits_type::thresh == 0)
			{
				return linear(); // always linear
			}
			else if ((size_t)std::distance(first, last) < traits_type::thresh)
			{
				return linear(); // linear
			}
			else
			{
				return binary(); // binary
			}
		}
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// sort implementation
		void impl_sort() noexcept
		{
			// empty
			if (this->empty()) { return; }

			// sort
			if constexpr (0 < traits_type::thresh)
			{
				std::sort(begin(), end(), compare_type{});
			}

			// remove duplicates
			if constexpr (!traits_type::multi)
			{
				this->erase(std::unique(begin(), end()), end());
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// insert implementation
		template <class Value
		> auto impl_insert(Value && value) noexcept -> std::conditional_t<traits_type::multi,
			iterator,
			std::pair<iterator, bool>
		>
		{
			if constexpr (traits_type::multi)
			{
				// insert multi
				return m_data.emplace(
					std::upper_bound(begin(), end(), ML_forward(value), compare_type{}),
					ML_forward(value));
			}
			else
			{
				// insert unique
				if (auto const it{ std::equal_range(begin(), end(), ML_forward(value), compare_type{}) }
				; it.first == it.second)
				{
					return { m_data.emplace(it.second, ML_forward(value)), true };
				}
				else
				{
					return { it.second, false };
				}
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// FLAT SET | sorted vector of unique elements
	template <
		class	_Ty,					// value type
		size_t	_Th = 42,				// search heuristic
		class	_Pr = std::less<_Ty>	// comparator predicate type
	> ML_alias set = typename basic_flat_set
	<
		flat_set_traits<_Ty, _Pr, false, _Th>
	>;

	// FLAT MULTISET | sorted vector of elements
	template <
		class	_Ty,					// value type
		size_t	_Th = 42,				// search heuristic
		class	_Pr = std::less<_Ty>	// comparator predicate type
	> ML_alias multiset = typename basic_flat_set
	<
		flat_set_traits<_Ty, _Pr, true, _Th>
	>;
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// JSON INTERFACE
namespace ml::ds
{
	template <
		class	Ty,
		class	Pr = std::less<Ty>,
		bool	Mt = false,
		size_t	Th = 42
	> void to_json(json & j, basic_flat_set<flat_set_traits<Ty, Pr, Mt, Th>> const & value)
	{
		using storage_type = typename basic_flat_set<
			flat_set_traits<Ty, Pr, Mt, Th>
		>::storage_type;
		j = *reinterpret_cast<storage_type const *>(&value);
	}

	template <
		class	Ty,
		class	Pr = std::less<Ty>,
		bool	Mt = false,
		size_t	Th = 42
	> void from_json(json const & j, basic_flat_set<flat_set_traits<Ty, Pr, Mt, Th>> & value)
	{
		using storage_type = typename basic_flat_set<
			flat_set_traits<Ty, Pr, Mt, Th>
		>::storage_type;
		if (j.is_array())
		{
			storage_type temp{};
			j.get_to(temp);
			value = temp;
		}
	}
}

#endif // !_ML_SET_HPP_