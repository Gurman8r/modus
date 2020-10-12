#ifndef _ML_ARRAY_HPP_
#define _ML_ARRAY_HPP_

#include <modus_core/detail/Utility.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// FIXED SIZE ARRAY
namespace ml::ds
{
	template <class _T, size_t _N
	> struct ML_NODISCARD array
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using value_type				= typename _T;
		using self_type					= typename _ML ds::array<value_type, _N>;
		using storage_type				= typename value_type[_N];
		using size_type					= typename size_t;
		using difference_type			= typename ptrdiff_t;
		using pointer					= typename value_type *;
		using const_pointer				= typename value_type const *;
		using reference					= typename value_type &;
		using const_reference			= typename value_type const &;
		using rvalue					= typename value_type &&;
		using const_rvalue				= typename value_type const &&;
		using iterator					= typename pointer;
		using const_iterator			= typename const_pointer;
		using reverse_iterator			= typename std::reverse_iterator<iterator>;
		using const_reverse_iterator	= typename std::reverse_iterator<const_iterator>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		storage_type m_data; // aggregate initializer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		constexpr auto data() noexcept -> pointer { return m_data; }
		
		constexpr auto data() const noexcept -> const_pointer { return m_data; }
		
		constexpr bool empty() const noexcept { return false; }

		constexpr auto max_size() const noexcept -> size_type { return _N; }

		constexpr auto size() const noexcept -> size_type { return _N; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr void swap(self_type & other) noexcept
		{
			if (this != std::addressof(other))
			{
				for (size_t i = 0; i < _N; ++i)
				{
					util::swap(m_data[i], other.m_data[i]);
				}
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr operator pointer() noexcept { return m_data; }

		constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto operator*() & noexcept -> reference { return (*m_data); }

		constexpr auto operator*() const & noexcept -> const_reference { return (*m_data); }

		constexpr auto operator*() && noexcept -> rvalue { return std::move(*m_data); }

		constexpr auto operator*() const && noexcept -> const_rvalue { return std::move(*m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto at(size_type const i) & noexcept -> reference { return m_data[i]; }

		constexpr auto at(size_type const i) const & noexcept -> const_reference { return m_data[i]; }

		constexpr auto at(size_type const i) && noexcept -> rvalue { return std::move(m_data[i]); }

		constexpr auto at(size_type const i) const && noexcept -> const_rvalue { return std::move(m_data[i]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto back() & noexcept -> reference { return m_data[_N - 1]; }
		
		constexpr auto back() const & noexcept -> const_reference { return m_data[_N - 1]; }

		constexpr auto back() && noexcept -> rvalue { return std::move(m_data[_N - 1]); }

		constexpr auto back() const && noexcept -> const_rvalue { return std::move(m_data[_N - 1]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		constexpr auto front() & noexcept -> reference { return m_data[0]; }

		constexpr auto front() const & noexcept -> const_reference { return m_data[0]; }

		constexpr auto front() && noexcept -> rvalue { return std::move(m_data[0]); }

		constexpr auto front() const && noexcept -> const_rvalue { return std::move(m_data[0]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto begin() noexcept -> iterator { return m_data; }

		constexpr auto begin() const noexcept -> const_iterator { return m_data; }

		constexpr auto end() noexcept -> iterator { return m_data + _N; }

		constexpr auto end() const noexcept -> const_iterator { return m_data + _N; }

		constexpr auto cbegin() const noexcept -> const_iterator { return begin(); }

		constexpr auto cend() const noexcept -> const_iterator { return end(); }

		constexpr auto crbegin() const noexcept -> const_reverse_iterator { return rbegin(); }

		constexpr auto crend() const noexcept -> const_reverse_iterator { return rend(); }

		constexpr auto rbegin() noexcept -> reverse_iterator { return std::make_reverse_iterator(end()); }

		constexpr auto rbegin() const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(cend()); }

		constexpr auto rend() noexcept -> reverse_iterator { return std::make_reverse_iterator(begin()); }

		constexpr auto rend() const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(cbegin()); }
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ZERO SIZE ARRAY
namespace ml::ds
{
	template <class _T
	> struct ML_NODISCARD array<_T, 0>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		using value_type				= typename _T;
		using self_type					= typename _ML ds::array<value_type, 0>;
		using storage_type				= typename value_type[1];
		using size_type					= typename size_t;
		using difference_type			= typename ptrdiff_t;
		using pointer					= typename value_type *;
		using const_pointer				= typename value_type const *;
		using reference					= typename value_type &;
		using const_reference			= typename value_type const &;
		using rvalue					= typename value_type &&;
		using const_rvalue				= typename value_type const &&;
		using iterator					= typename pointer;
		using const_iterator			= typename const_pointer;
		using reverse_iterator			= typename std::reverse_iterator<iterator>;
		using const_reverse_iterator	= typename std::reverse_iterator<const_iterator>;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		storage_type m_data; // aggregate initializer
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		constexpr auto data() noexcept -> pointer { return &m_data[0]; }

		constexpr auto data() const noexcept -> const_pointer { return &m_data[0]; }

		constexpr bool empty() const noexcept { return true; }

		constexpr auto max_size() const noexcept -> size_type { return 0; }

		constexpr auto size() const noexcept -> size_type { return 0; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr operator pointer() noexcept { return m_data; }

		constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr explicit operator std::array<_T, 0> & () & noexcept
		{
			using A = std::array<_T, 0>;
			return (A &)(*(A *)this);
		}

		constexpr explicit operator std::array<_T, 0> const & () const & noexcept
		{
			using A = std::array<_T, 0>;
			return (A const &)(*(A const *)this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto operator*() & noexcept -> reference { return (*m_data); }

		constexpr auto operator*() const & noexcept -> const_reference { return (*m_data); }

		constexpr auto operator*() && noexcept -> rvalue { return std::move(*m_data); }

		constexpr auto operator*() const && noexcept -> const_rvalue { return std::move(*m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto at(size_type) & noexcept -> reference { return m_data[0]; }

		constexpr auto at(size_type) const & noexcept -> const_reference { return m_data[0]; }

		constexpr auto at(size_type) && noexcept -> rvalue { return std::move(m_data[0]); }

		constexpr auto at(size_type) const && noexcept -> const_rvalue { return std::move(m_data[0]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		constexpr auto back() & noexcept -> reference { return m_data[0]; }

		constexpr auto back() const & noexcept -> const_reference { return m_data[0]; }

		constexpr auto back() && noexcept -> rvalue { return std::move(m_data[0]); }

		constexpr auto back() const && noexcept -> const_rvalue { return std::move(m_data[0]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		constexpr auto front() & noexcept -> reference { return m_data[0]; }

		constexpr auto front() const & noexcept -> const_reference { return m_data[0]; }

		constexpr auto front() && noexcept -> rvalue { return std::move(m_data[0]); }

		constexpr auto front() const && noexcept -> const_rvalue { return std::move(m_data[0]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		constexpr auto begin() noexcept -> iterator { return &m_data[0]; }

		constexpr auto begin() const noexcept -> const_iterator { return &m_data[0]; }

		constexpr auto cbegin() const noexcept -> const_iterator { return &m_data[0]; }

		constexpr auto cend() const noexcept -> const_iterator { return &m_data[0]; }

		constexpr auto crbegin() const noexcept -> const_reverse_iterator { return &m_data[0]; }

		constexpr auto crend() const noexcept -> const_reverse_iterator { return &m_data[0]; }
		
		constexpr auto end() noexcept -> iterator { return &m_data[0]; }

		constexpr auto end() const noexcept -> const_iterator { return &m_data[0]; }

		constexpr auto rbegin() noexcept -> reverse_iterator { return &m_data[0]; }

		constexpr auto rbegin() const noexcept -> const_reverse_iterator { return &m_data[0]; }

		constexpr auto rend() noexcept -> reverse_iterator { return &m_data[0]; }

		constexpr auto rend() const noexcept -> const_reverse_iterator { return &m_data[0]; }
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ARRAY OPERATORS
namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Tx, class Ty, size_t N
	> constexpr bool operator==(array<Tx, N> const & lhs, array<Ty, N> const & rhs)
	{
		return util::range_equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class Tx, class Ty, size_t N
	> constexpr bool operator!=(array<Tx, N> const & lhs, array<Ty, N> const & rhs)
	{
		return !(lhs == rhs);
	}

	template <class Tx, class Ty, size_t N
	> constexpr bool operator<(array<Tx, N> const & lhs, array<Ty, N> const & rhs)
	{
		return util::range_less(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class Tx, class Ty, size_t N
	> constexpr bool operator<=(array<Tx, N> const & lhs, array<Ty, N> const & rhs)
	{
		return (lhs < rhs) || (lhs == rhs);
	}

	template <class Tx, class Ty, size_t N
	> constexpr bool operator>(array<Tx, N> const & lhs, array<Ty, N> const & rhs)
	{
		return !(lhs < rhs) && (lhs != rhs);
	}

	template <class Tx, class Ty, size_t N
	> constexpr bool operator>=(array<Tx, N> const & lhs, array<Ty, N> const & rhs)
	{
		return (lhs > rhs) || (lhs == rhs);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// TUPLE INTERFACE
namespace std
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <size_t I, class T, size_t N
	> ML_NODISCARD constexpr T & get(_ML ds::array<T, N> & value) noexcept
	{
		static_assert(I < N, "array index out of bounds");
		return value.at(I);
	}

	template <size_t I, class T, size_t N
	> ML_NODISCARD constexpr T const & get(_ML ds::array<T, N> const & value) noexcept
	{
		static_assert(I < N, "array index out of bounds");
		return value.at(I);
	}

	template <size_t I, class T, size_t N
	> ML_NODISCARD constexpr T && get(_ML ds::array<T, N> && value) noexcept
	{
		static_assert(I < N, "array index out of bounds");
		return std::move(value.at(I));
	}

	template <size_t I, class T, size_t N
	> ML_NODISCARD constexpr T const && get(_ML ds::array<T, N> const && value) noexcept
	{
		static_assert(I < N, "array index out of bounds");
		return std::move(value.at(I));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// JSON INTERFACE
namespace ml::ds
{
	template <class T, size_t N
	> void from_json(json const & j, _ML ds::array<T, N> & value)
	{
		// sue me
		j.get_to((std::array<T, N> &)value);
	}

	template <class T, size_t N
	> void to_json(json & j, _ML ds::array<T, N> const & value)
	{
		// sue me
		j = (std::array<T, N> const &)value;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_ARRAY_HPP_