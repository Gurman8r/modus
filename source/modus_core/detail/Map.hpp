#ifndef _ML_MAP_HPP_
#define _ML_MAP_HPP_

#include <modus_core/detail/Set.hpp>

namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// FLAT MAP TRAITS
	template <
		class	_Kt,	// key type
		class	_Vt,	// value type
		class	_Pr,	// key comparator predicate type
		size_t	_Th		// search heuristic
	> struct flat_map_traits final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using key_type			= typename _Kt;
		using value_type		= typename _Vt;
		using compare_type		= typename _Pr;
		using allocator_type	= typename pmr::polymorphic_allocator<byte_t>;
		using difference_type	= typename ptrdiff_t;
		using size_type			= typename size_t;

		static constexpr size_type thresh{ _Th };

		template <class T = key_type
		> using key_storage = typename ds::set<T, thresh, compare_type>;

		template <class T = value_type
		> using value_storage = typename pmr::vector<T>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// BASIC FLAT MAP
	template <class _Traits
	> struct basic_flat_map final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using traits_type						= typename _Traits;
		using self_type							= typename basic_flat_map<traits_type>;
		using key_type							= typename traits_type::key_type;
		using value_type						= typename traits_type::value_type;
		using compare_type						= typename traits_type::compare_type;
		using allocator_type					= typename traits_type::allocator_type;
		using difference_type					= typename traits_type::difference_type;
		using size_type							= typename traits_type::size_type;
		using key_storage						= typename traits_type::template key_storage<>;
		using value_storage						= typename traits_type::template value_storage<>;

		using key_pointer						= typename key_storage::pointer;
		using key_const_pointer					= typename key_storage::const_pointer;
		using key_reference						= typename key_storage::reference;
		using key_const_reference				= typename key_storage::const_reference;
		using key_rvalue						= typename key_storage::rvalue;
		using key_const_rvalue					= typename key_storage::const_rvalue;
		using key_iterator						= typename key_storage::iterator;
		using key_const_iterator				= typename key_storage::const_iterator;
		using key_reverse_iterator				= typename key_storage::reverse_iterator;
		using key_const_reverse_iterator		= typename key_storage::const_reverse_iterator;

		using value_pointer						= typename value_storage::pointer;
		using value_const_pointer				= typename value_storage::const_pointer;
		using value_reference					= typename value_storage::reference;
		using value_const_reference				= typename value_storage::const_reference;
		using value_rvalue						= typename value_type &&;
		using value_const_rvalue				= typename value_type const &&;
		using value_iterator					= typename value_storage::iterator;
		using value_const_iterator				= typename value_storage::const_iterator;
		using value_reverse_iterator			= typename value_storage::reverse_iterator;
		using value_const_reverse_iterator		= typename value_storage::const_reverse_iterator;

		using storage_type						= typename std::pair<key_storage, value_storage>;
		using keyval_pair						= typename std::pair<key_type, value_type>;
		using init_type							= typename std::initializer_list<keyval_pair>;

		using pointer_pair						= typename std::pair<key_pointer, value_pointer>;
		using const_pointer_pair				= typename std::pair<key_const_pointer, value_const_pointer>;
		using reference_pair					= typename std::pair<key_reference, value_reference>;
		using const_reference_pair				= typename std::pair<key_const_reference, value_const_reference>;
		using rvalue_pair						= typename std::pair<key_rvalue, value_rvalue>;
		using const_rvalue_pair					= typename std::pair<key_const_rvalue, value_const_rvalue>;

		using iterator_pair						= typename std::pair<key_iterator, value_iterator>;
		using const_iterator_pair				= typename std::pair<key_const_iterator, value_const_iterator>;
		using reverse_iterator_pair				= typename std::pair<key_reverse_iterator, value_reverse_iterator>;
		using const_reverse_iterator_pair		= typename std::pair<key_const_reverse_iterator, value_const_reverse_iterator>;

		using optl_iterator_pair				= typename std::optional<iterator_pair>;
		using optl_const_iterator_pair			= typename std::optional<const_iterator_pair>;
		using optl_reverse_iterator_pair		= typename std::optional<iterator_pair>;
		using optl_const_reverse_iterator_pair	= typename std::optional<const_iterator_pair>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		basic_flat_map(allocator_type alloc = {})
			: m_pair{ key_storage{ alloc }, value_storage{ alloc } }
		{
		}

		basic_flat_map(pmr::vector<key_type> && k, pmr::vector<value_type> && v, allocator_type alloc = {}) noexcept
			: self_type{ alloc }
		{
			for (size_t i = 0, imax = ML_min(k.size(), v.size()); i < imax; ++i)
			{
				this->insert(ML_forward(k[i]), ML_forward(v[i]));
			}
		}

		basic_flat_map(init_type value, allocator_type alloc = {})
			: self_type{ alloc }
		{
			this->assign(value);
		}

		basic_flat_map(self_type const & value, allocator_type alloc = {})
			: self_type{ alloc }
		{
			this->assign(value);
		}

		basic_flat_map(self_type && value, allocator_type alloc = {}) noexcept
			: self_type{ alloc }
		{
			this->swap(std::move(value));
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

		self_type & operator=(self_type && value) noexcept
		{
			this->swap(std::move(value));
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void assign(init_type value)
		{
			this->clear();

			for (auto it = value.begin(); it != value.end(); ++it)
			{
				this->insert(it->first, it->second);
			}
		}

		void assign(self_type const & value)
		{
			if (this != std::addressof(value))
			{
				m_pair = value.m_pair;
			}
		}

		void clear() noexcept
		{
			m_pair.first.clear();
			m_pair.second.clear();
		}

		void pop_back() noexcept
		{
			m_pair.first.pop_back();
			m_pair.second.pop_back();
		}

		void reserve(size_type const count)
		{
			m_pair.first.reserve(count);
			m_pair.second.reserve(count);
		}

		void resize(size_type const count)
		{
			m_pair.first.resize(count);
			m_pair.second.resize(count);
		}

		void resize(size_type const count, value_type const & value)
		{
			m_pair.first.resize(count);
			m_pair.second.resize(count, value);
		}

		void shrink_to_fit() noexcept
		{
			m_pair.first.shrink_to_fit();
			m_pair.second.shrink_to_fit();
		}

		void swap(self_type & value) noexcept
		{
			if (this != std::addressof(value))
			{
				m_pair.swap(value.m_pair);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD key_storage const & keys() const & noexcept
		{
			return m_pair.first;
		}

		ML_NODISCARD value_storage const & values() const & noexcept
		{
			return m_pair.second;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD size_type capacity() const noexcept
		{
			return m_pair.first.capacity();
		}

		ML_NODISCARD bool empty() const noexcept
		{
			return m_pair.first.empty();
		}

		ML_NODISCARD size_type max_size() const noexcept
		{
			return m_pair.first.max_size();
		}

		ML_NODISCARD size_type size() const noexcept
		{
			return m_pair.first.size();
		}

		ML_NODISCARD allocator_type get_allocator() const noexcept
		{
			return m_pair.first.get_allocator();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD reference_pair back() noexcept
		{
			return { m_pair.first.back(), m_pair.second.back() };
		}

		ML_NODISCARD const_reference_pair back() const noexcept
		{
			return { m_pair.first.back(), m_pair.second.back() };
		}

		ML_NODISCARD reference_pair front() noexcept
		{
			return { m_pair.first.front(), m_pair.second.front() };
		}

		ML_NODISCARD const_reference_pair front() const noexcept
		{
			return { m_pair.first.front(), m_pair.second.front() };
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Out = difference_type
		> ML_NODISCARD Out index_of(key_const_iterator it) const noexcept
		{
			return m_pair.first.index_of<Out>(it);
		}

		template <class Out = difference_type
		> ML_NODISCARD Out index_of(key_const_reverse_iterator it) const noexcept
		{
			return m_pair.first.index_of<Out>(it);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD value_iterator fetch(key_const_iterator it) noexcept
		{
			return std::next(m_pair.second.begin(), this->index_of(it));
		}

		ML_NODISCARD value_const_iterator fetch(key_const_iterator it) const noexcept
		{
			return std::next(m_pair.second.cbegin(), this->index_of(it));
		}

		ML_NODISCARD value_const_iterator cfetch(key_const_iterator it) const noexcept
		{
			return this->fetch(it);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD value_reverse_iterator rfetch(key_const_reverse_iterator it) noexcept
		{
			return std::next(m_pair.second.rbegin(), this->index_of(it));
		}

		ML_NODISCARD value_const_reverse_iterator rfetch(key_const_reverse_iterator it) const noexcept
		{
			return std::next(m_pair.second.crbegin(), this->index_of(it));
		}

		ML_NODISCARD value_const_reverse_iterator crfetch(key_const_reverse_iterator it) const noexcept
		{
			return this->rfetch(it);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		iterator_pair erase(key_iterator it)
		{
			auto const result
			{
				m_pair.second.erase(this->fetch(it))
			};
			return { m_pair.first.erase(it), result };
		}

		iterator_pair erase(key_iterator first, key_iterator last)
		{
			auto const result
			{
				m_pair.second.erase(this->fetch(first), this->fetch(last))
			};
			return { m_pair.first.erase(first, last), result };
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Key = key_type
		> ML_NODISCARD bool contains(Key && key) const noexcept
		{
			return m_pair.first.contains(ML_forward(key));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Key = key_type
		> ML_NODISCARD optl_iterator_pair find(Key && key) noexcept
		{
			if (auto const k{ m_pair.first.find(ML_forward(key)) }; k != m_pair.first.end())
			{
				return std::make_optional(iterator_pair{ k, this->fetch(k) });
			}
			else
			{
				return std::nullopt;
			}
		}

		template <class Key = key_type
		> ML_NODISCARD optl_const_iterator_pair find(Key && key) const noexcept
		{
			if (auto const k{ m_pair.first.find(ML_forward(key)) }; k != m_pair.first.cend())
			{
				return std::make_optional(const_iterator_pair{ k, this->fetch(k) });
			}
			else
			{
				return std::nullopt;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Key, class ... Args
		> std::pair<iterator_pair, bool> try_emplace(Key && key, Args && ... args) noexcept
		{
			if (auto const k{ m_pair.first.insert(ML_forward(key)) }; k.second)
			{
				return { this->impl_emplace_hint(k.first, ML_forward(args)...), true };
			}
			else
			{
				return { { k.first, this->fetch(k.first) }, false };
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Key = key_type, class Value = value_type
		> iterator_pair insert(Key && key, Value && value) noexcept
		{
			return this->try_emplace(ML_forward(key), ML_forward(value)).first;
		}

		iterator_pair insert(keyval_pair const & kvp) noexcept
		{
			return this->try_emplace(kvp.first, kvp.second).first;
		}

		iterator_pair insert(keyval_pair && kvp) noexcept
		{
			return this->try_emplace(std::move(kvp.first), std::move(kvp.second)).first;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Key = key_type
		> ML_NODISCARD value_type & operator[](Key && key) & noexcept
		{
			return this->at(ML_forward(key));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Key = key_type
		> ML_NODISCARD value_type & at(Key && key) noexcept
		{
			return this->find_or_add(ML_forward(key), value_type{});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Key = key_type, class Value = value_type
		> ML_NODISCARD value_type & find_or_add(Key && key, Value && value) noexcept
		{
			if (auto const it{ this->find(ML_forward(key)) })
			{
				return (*it->second);
			}
			else
			{
				return *this->insert(ML_forward(key), ML_forward(value)).second;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Key, class Fn, class ... Args
		> ML_NODISCARD value_type & find_or_add_fn(Key && key, Fn && fn, Args && ... args) noexcept
		{
			if (auto const it{ this->find(ML_forward(key)) })
			{
				return (*it->second);
			}
			else
			{
				return *this->insert(ML_forward(key), std::invoke(ML_forward(fn), ML_forward(args)...)).second;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn
		> void for_each(key_const_iterator first, key_const_iterator last, Fn && fn)
		{
			for (; first != last; ++first)
			{
				std::invoke(ML_forward(fn), *first, *this->fetch(first));
			}
		}

		template <class Fn
		> void for_each(key_const_iterator first, Fn && fn)
		{
			this->for_each(first, m_pair.first.cend(), ML_forward(fn));
		}

		template <class Fn
		> void for_each(Fn && fn)
		{
			this->for_each(m_pair.first.cbegin(), ML_forward(fn));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn
		> void for_each(key_const_iterator first, key_const_iterator last, Fn && fn) const
		{
			for (; first != last; ++first)
			{
				std::invoke(ML_forward(fn), *first, *this->cfetch(first));
			}
		}

		template <class Fn
		> void for_each(key_const_iterator first, Fn && fn) const
		{
			this->for_each(first, m_pair.first.cend(), ML_forward(fn));
		}

		template <class Fn
		> void for_each(Fn && fn) const
		{
			this->for_each(m_pair.first.cbegin(), ML_forward(fn));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn
		> void for_each_n(key_const_iterator first, difference_type count, Fn && fn)
		{
			if (0 < count)
			{
				do
				{
					std::invoke(ML_forward(fn), *first, *this->fetch(first));
					
					--count; ++first;
				}
				while (0 < count);
			}
		}

		template <class Fn
		> void for_each_n(difference_type count, Fn && fn)
		{
			this->for_each_n(m_pair.first.cbegin(), count, ML_forward(fn));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn
		> void for_each_n(key_const_iterator first, difference_type count, Fn && fn) const
		{
			if (0 < count)
			{
				do
				{
					std::invoke(ML_forward(fn), *first, *this->cfetch(first));

					--count; ++first;
				}
				while (0 < count);
			}
		}

		template <class Fn
		> void for_each_n(difference_type count, Fn && fn) const
		{
			this->for_each_n(m_pair.first.cbegin(), count, ML_forward(fn));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class U = self_type
		> ML_NODISCARD auto compare(U const & value) const noexcept
		{
			if constexpr (std::is_same_v<U, self_type>)
			{
				return this->compare(value.m_pair);
			}
			else
			{
				static_assert(std::is_same_v<U, storage_type>);
				return ML_compare(m_pair, value.m_pair);
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

		// emplace_hint implementation
		template <class ... Args
		> iterator_pair impl_emplace_hint(key_const_iterator it, Args && ... args)
		{
			return // must be private or the map could become unsorted
			{
				std::next(m_pair.first.begin(), this->index_of(it)),
				m_pair.second.emplace(this->fetch(it), ML_forward(args)...)
			};
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_pair;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// FLAT MAP | flat ordered associative container
	template <
		class	_Kt,					// key type
		class	_Vt,					// value type
		size_t	_Th = 42,				// search heuristic
		class	_Pr = std::less<_Kt>	// key comparator predicate type
	> ML_alias map = basic_flat_map
	<
		flat_map_traits<_Kt, _Vt, _Pr, _Th>
	>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class K, class V, size_t T = 42, class P = std::less<K>
	> void to_json(json & j, ds::map<K, V, T, P> const & v)
	{
		j["keys"	] = v.keys();
		j["values"	] = v.values();
	}

	template <class K, class V, size_t T = 42, class P = std::less<K>
	> void from_json(json const & j, ds::map<K, V, T, P> & v)
	{
		using M = typename ds::map<K, V, T, P>;
		using K = typename M::key_storage;
		using V = typename M::value_storage;

		v = M{ (K)j["keys"], (V)j["values"] };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_MAP_HPP_