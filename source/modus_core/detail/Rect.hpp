#ifndef _ML_RECT_HPP_
#define _ML_RECT_HPP_

#include <modus_core/detail/Matrix.hpp>

namespace ml
{
	template <class _T> struct rectangle
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using value_type				= typename _T;
		using self_type					= typename rectangle;
		using coord_type				= typename tvec2<value_type>;
		using storage_type				= typename tvec4<value_type>;
		using pointer					= typename storage_type::pointer;
		using reference					= typename storage_type::reference;
		using const_pointer				= typename storage_type::const_pointer;
		using const_reference			= typename storage_type::const_reference;
		using iterator					= typename storage_type::iterator;
		using const_iterator			= typename storage_type::const_iterator;
		using reverse_iterator			= typename storage_type::reverse_iterator;
		using const_reverse_iterator	= typename storage_type::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr rectangle() noexcept : m_data{} {}

		template <class X1, class Y1, class X2, class Y2
		> constexpr rectangle(X1 x1, Y1 y1, X2 x2, Y2 y2)
			: m_data{ (value_type)x1, (value_type)y1, (value_type)x2, (value_type)y2 }
		{
		}

		constexpr rectangle(coord_type const & min, coord_type const & max)
			: m_data{ min[0], min[1], max[0], max[1] }
		{
		}

		constexpr rectangle(storage_type const & value)
			: m_data{ value }
		{
		}

		constexpr rectangle(self_type const & other)
			: m_data{ other.m_data }
		{
		}

		constexpr rectangle(self_type && other) noexcept
			: m_data{}
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr self_type & operator=(self_type const & other)
		{
			rectangle temp{ other };
			this->swap(temp);
			return (*this);
		}

		constexpr self_type & operator=(self_type && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		constexpr void swap(self_type & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_data.swap(other.m_data);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator storage_type & () & noexcept { return m_data; }

		ML_NODISCARD constexpr operator storage_type const & () const & noexcept { return m_data; }

		ML_NODISCARD constexpr operator storage_type && () && noexcept { return std::move(m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator pointer() noexcept { return m_data; }

		ML_NODISCARD constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr coord_type & min() noexcept { return *(coord_type *)&m_data[0]; }

		ML_NODISCARD constexpr coord_type const & min() const noexcept { return *(coord_type const *)&m_data[0]; }

		template <class Value = coord_type
		> coord_type & min(Value && value) noexcept { return min() = ML_forward(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr coord_type & max() noexcept { return *(coord_type *)&m_data[2]; }

		ML_NODISCARD constexpr coord_type const & max() const noexcept { return *(coord_type const *)&m_data[2]; }

		template <class Value = coord_type
		> coord_type & max(Value && value) noexcept { return max() = ML_forward(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr value_type & at(size_t i) & noexcept { return m_data[i]; }

		ML_NODISCARD constexpr value_type & left() & noexcept { return m_data[0]; }

		ML_NODISCARD constexpr value_type & top() & noexcept { return m_data[1]; }

		ML_NODISCARD constexpr value_type & right() & noexcept { return m_data[2]; }

		ML_NODISCARD constexpr value_type & bottom() & noexcept { return m_data[3]; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr value_type const & at(size_t i) const & noexcept { return m_data[i]; }

		ML_NODISCARD constexpr value_type const & left() const & noexcept { return m_data[0]; }

		ML_NODISCARD constexpr value_type const & top() const & noexcept { return m_data[1]; }

		ML_NODISCARD constexpr value_type const & right() const & noexcept { return m_data[2]; }

		ML_NODISCARD constexpr value_type const & bottom() const & noexcept { return m_data[3]; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Value = value_type
		> constexpr value_type & set(size_t i, Value && value) & noexcept { m_data[i] = ML_forward(value); }

		template <class Value = value_type
		> constexpr value_type & left(Value && value) & noexcept { m_data[0] = ML_forward(value); }

		template <class Value = value_type
		> constexpr value_type & top(Value && value) & noexcept { m_data[1] = ML_forward(value); }

		template <class Value = value_type
		> constexpr value_type & right(Value && value) & noexcept { m_data[2] = ML_forward(value); }

		template <class Value = value_type
		> constexpr value_type & bottom(Value && value) & noexcept { m_data[3] = ML_forward(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr value_type width() const noexcept { return m_data[2] - m_data[0]; }

		ML_NODISCARD constexpr value_type height() const noexcept { return m_data[3] - m_data[1]; }

		ML_NODISCARD constexpr coord_type size() const noexcept { return { m_data[2] - m_data[0], m_data[3] - m_data[1] }; }

		ML_NODISCARD constexpr coord_type center() const noexcept { return (min() + max()) / 2; }

		ML_NODISCARD constexpr coord_type position() const noexcept { return min(); }

		ML_NODISCARD constexpr coord_type top_left() const noexcept { return min(); }

		ML_NODISCARD constexpr coord_type top_right() const noexcept { return { m_data[2], m_data[1] }; }

		ML_NODISCARD constexpr coord_type bottom_left() const noexcept { return { m_data[0], m_data[3] }; }

		ML_NODISCARD constexpr coord_type bottom_right() const noexcept { return max(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD constexpr auto begin() noexcept -> iterator { return m_data.begin(); }

		ML_NODISCARD constexpr auto begin() const noexcept -> const_iterator { return m_data.begin(); }

		ML_NODISCARD constexpr auto cbegin() const noexcept -> const_iterator { return m_data.cbegin(); }

		ML_NODISCARD constexpr auto cend() const noexcept -> const_iterator { return m_data.cend(); }

		ML_NODISCARD constexpr auto crbegin() const noexcept -> const_reverse_iterator { return m_data.crbegin(); }

		ML_NODISCARD constexpr auto crend() const noexcept -> const_reverse_iterator { return m_data.crend(); }

		ML_NODISCARD constexpr auto end() noexcept -> iterator { return m_data.end(); }

		ML_NODISCARD constexpr auto end() const noexcept -> const_iterator { return m_data.end(); }

		ML_NODISCARD constexpr auto rbegin() noexcept -> reverse_iterator { return m_data.rbegin(); }

		ML_NODISCARD constexpr auto rbegin() const noexcept -> const_reverse_iterator { return m_data.rbegin(); }

		ML_NODISCARD constexpr auto rend() noexcept -> reverse_iterator { return m_data.rend(); }

		ML_NODISCARD constexpr auto rend() const noexcept -> const_reverse_iterator { return m_data.rend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	ML_alias float_rect		= _ML rectangle<float32>;
	ML_alias double_rect	= _ML rectangle<float64>;
	ML_alias int_rect		= _ML rectangle<int32>;
	ML_alias uint_rect		= _ML rectangle<uint32>;
	ML_alias size_rect		= _ML rectangle<size_t>;
}

#endif // !_ML_RECT_HPP_