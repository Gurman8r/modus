#ifndef _ML_ARRAY_HPP_
#define _ML_ARRAY_HPP_

#include <modus_core/detail/Utility.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// FIXED SIZE ARRAY
namespace ml
{
	template <class _T, size_t _Size
	> struct ML_NODISCARD array
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using value_type				= typename _T;
		using self_type					= typename _ML array<value_type, _Size>;
		using storage_type				= typename value_type[_Size];
		using size_type					= typename size_t;
		using difference_type			= typename ptrdiff_t;
		using pointer					= typename value_type *;
		using const_pointer				= typename value_type const *;
		using reference					= typename value_type &;
		using const_reference			= typename value_type const &;
		using rvalue					= typename value_type &&;
		using iterator					= typename pointer;
		using const_iterator			= typename const_pointer;
		using reverse_iterator			= typename std::reverse_iterator<iterator>;
		using const_reverse_iterator	= typename std::reverse_iterator<const_iterator>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		storage_type m_data; // aggregate initializer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr void swap(self_type & other) noexcept
		{
			if (this != std::addressof(other))
			{
				for (size_t i = 0; i < _Size; ++i)
				{
					util::swap(m_data[i], other.m_data[i]);
				}
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD constexpr auto data() noexcept -> pointer { return m_data; }
		
		ML_NODISCARD constexpr auto data() const noexcept -> const_pointer { return m_data; }
		
		ML_NODISCARD constexpr bool empty() const noexcept { return false; }

		ML_NODISCARD constexpr auto max_size() const noexcept -> size_type { return _Size; }

		ML_NODISCARD constexpr auto size() const noexcept -> size_type { return _Size; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator pointer() noexcept { return m_data; }

		ML_NODISCARD constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto operator*() & noexcept -> reference { return (*m_data); }

		ML_NODISCARD constexpr auto operator*() const & noexcept -> const_reference { return (*m_data); }

		ML_NODISCARD constexpr auto operator*() && noexcept -> rvalue { return std::move(*m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto at(size_type const i) & noexcept -> reference { return m_data[i]; }

		ML_NODISCARD constexpr auto at(size_type const i) const & noexcept -> const_reference { return m_data[i]; }

		ML_NODISCARD constexpr auto at(size_type const i) && noexcept -> rvalue { return std::move(m_data[i]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto back() & noexcept -> reference { return m_data[_Size - 1]; }
		
		ML_NODISCARD constexpr auto back() const & noexcept -> const_reference { return m_data[_Size - 1]; }

		ML_NODISCARD constexpr auto back() && noexcept -> rvalue { return std::move(m_data[_Size - 1]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD constexpr auto front() & noexcept -> reference { return m_data[0]; }

		ML_NODISCARD constexpr auto front() const & noexcept -> const_reference { return m_data[0]; }

		ML_NODISCARD constexpr auto front() && noexcept -> rvalue { return std::move(m_data[0]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto begin() noexcept -> iterator { return m_data; }

		ML_NODISCARD constexpr auto begin() const noexcept -> const_iterator { return m_data; }

		ML_NODISCARD constexpr auto end() noexcept -> iterator { return m_data + _Size; }

		ML_NODISCARD constexpr auto end() const noexcept -> const_iterator { return m_data + _Size; }

		ML_NODISCARD constexpr auto cbegin() const noexcept -> const_iterator { return begin(); }

		ML_NODISCARD constexpr auto cend() const noexcept -> const_iterator { return end(); }

		ML_NODISCARD constexpr auto crbegin() const noexcept -> const_reverse_iterator { return rbegin(); }

		ML_NODISCARD constexpr auto crend() const noexcept -> const_reverse_iterator { return rend(); }

		ML_NODISCARD constexpr auto rbegin() noexcept -> reverse_iterator { return std::make_reverse_iterator(end()); }

		ML_NODISCARD constexpr auto rbegin() const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(cend()); }

		ML_NODISCARD constexpr auto rend() noexcept -> reverse_iterator { return std::make_reverse_iterator(begin()); }

		ML_NODISCARD constexpr auto rend() const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(cbegin()); }
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ZERO SIZE ARRAY
namespace ml
{
	template <class _T
	> struct ML_NODISCARD array<_T, 0>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		using value_type				= typename _T;
		using self_type					= typename _ML array<value_type, 0>;
		using storage_type				= typename value_type[1];
		using size_type					= typename size_t;
		using difference_type			= typename ptrdiff_t;
		using pointer					= typename value_type *;
		using const_pointer				= typename value_type const *;
		using reference					= typename value_type &;
		using const_reference			= typename value_type const &;
		using rvalue					= typename value_type &&;
		using iterator					= typename pointer;
		using const_iterator			= typename const_pointer;
		using reverse_iterator			= typename std::reverse_iterator<iterator>;
		using const_reverse_iterator	= typename std::reverse_iterator<const_iterator>;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		storage_type m_data; // aggregate initializer
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD constexpr auto data() noexcept -> pointer { return &m_data[0]; }

		ML_NODISCARD constexpr auto data() const noexcept -> const_pointer { return &m_data[0]; }

		ML_NODISCARD constexpr bool empty() const noexcept { return true; }

		ML_NODISCARD constexpr auto max_size() const noexcept -> size_type { return 0; }

		ML_NODISCARD constexpr auto size() const noexcept -> size_type { return 0; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator pointer() noexcept { return m_data; }

		ML_NODISCARD constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr explicit operator std::array<_T, 0> & () & noexcept
		{
			using A = std::array<_T, 0>;
			return (A &)(*(A *)this);
		}

		ML_NODISCARD constexpr explicit operator std::array<_T, 0> const & () const & noexcept
		{
			using A = std::array<_T, 0>;
			return (A const &)(*(A const *)this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto operator*() & noexcept -> reference { return (*m_data); }

		ML_NODISCARD constexpr auto operator*() const & noexcept -> const_reference { return (*m_data); }

		ML_NODISCARD constexpr auto operator*() && noexcept -> rvalue { return std::move(*m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto at(size_type) & noexcept -> reference { return m_data[0]; }

		ML_NODISCARD constexpr auto at(size_type) const & noexcept -> const_reference { return m_data[0]; }

		ML_NODISCARD constexpr auto at(size_type) && noexcept -> rvalue { return std::move(m_data[0]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD constexpr auto back() & noexcept -> reference { return m_data[0]; }

		ML_NODISCARD constexpr auto back() const & noexcept -> const_reference { return m_data[0]; }

		ML_NODISCARD constexpr auto back() && noexcept -> rvalue { return std::move(m_data[0]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD constexpr auto front() & noexcept -> reference { return m_data[0]; }

		ML_NODISCARD constexpr auto front() const & noexcept -> const_reference { return m_data[0]; }

		ML_NODISCARD constexpr auto front() && noexcept -> rvalue { return std::move(m_data[0]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD constexpr auto begin() noexcept -> iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto begin() const noexcept -> const_iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto cbegin() const noexcept -> const_iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto cend() const noexcept -> const_iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto crbegin() const noexcept -> const_reverse_iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto crend() const noexcept -> const_reverse_iterator { return &m_data[0]; }
		
		ML_NODISCARD constexpr auto end() noexcept -> iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto end() const noexcept -> const_iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto rbegin() noexcept -> reverse_iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto rbegin() const noexcept -> const_reverse_iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto rend() noexcept -> reverse_iterator { return &m_data[0]; }

		ML_NODISCARD constexpr auto rend() const noexcept -> const_reverse_iterator { return &m_data[0]; }
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ARRAY OPERATORS
namespace ml
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
	> ML_NODISCARD constexpr T & get(_ML array<T, N> & value) noexcept
	{
		static_assert(I < N, "array index out of bounds");
		return value.at(I);
	}

	template <size_t I, class T, size_t N
	> ML_NODISCARD constexpr T const & get(_ML array<T, N> const & value) noexcept
	{
		static_assert(I < N, "array index out of bounds");
		return value.at(I);
	}

	template <size_t I, class T, size_t N
	> ML_NODISCARD constexpr T && get(_ML array<T, N> && value) noexcept
	{
		static_assert(I < N, "array index out of bounds");
		return std::move(value.at(I));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// JSON INTERFACE
namespace ml
{
	template <class T, size_t N
	> void from_json(json const & j, _ML array<T, N> & value)
	{
		j.get_to(reinterpret_cast<std::array<T, N> &>(value));
	}

	template <class T, size_t N
	> void to_json(json & j, _ML array<T, N> const & value)
	{
		j = reinterpret_cast<std::array<T, N> const &>(value);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_ARRAY_HPP_