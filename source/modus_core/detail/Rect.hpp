#ifndef _ML_RECT_HPP_
#define _ML_RECT_HPP_

#include <detail/Matrix.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T> struct basic_rect final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		using value_type		= typename T;
		using self_type			= typename basic_rect<value_type>;
		using storage_type		= typename tvec4<value_type>;
		using coord_type		= typename tvec2<value_type>;
		using pointer			= typename storage_type::pointer;
		using reference			= typename storage_type::reference;
		using const_pointer		= typename storage_type::const_pointer;
		using const_reference	= typename storage_type::const_reference;
		using iterator			= typename storage_type::iterator;
		using const_iterator	= typename storage_type::const_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr basic_rect() noexcept
			: m_data{}
		{
		}

		constexpr basic_rect(value_type value) noexcept
			: m_data{ value }
		{
		}
		
		template <class W, class H
		> constexpr basic_rect(W w, H h) noexcept
			: m_data{ 0, 0, static_cast<T>(w), static_cast<T>(h) }
		{
		}
		
		template <class X, class Y, class W, class H
		> constexpr basic_rect(X x, Y y, W w, H h) noexcept
			: m_data{ static_cast<T>(x), static_cast<T>(y), static_cast<T>(w), static_cast<T>(h) }
		{
		}

		constexpr basic_rect(self_type const & value) noexcept
			: m_data{ value.m_data }
		{
		}

		constexpr basic_rect(storage_type const & value) noexcept
			: m_data{ value }
		{
		}
		
		constexpr basic_rect(coord_type const & pos, coord_type const & size) noexcept
			: m_data{ pos[0], pos[1], size[0], size[1] }
		{
		}
		
		constexpr basic_rect(coord_type const & size) noexcept
			: m_data{ 0, 0, size[0], size[1] }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator storage_type & () & noexcept { return m_data; }

		ML_NODISCARD constexpr operator storage_type const & () const & noexcept { return m_data; }

		ML_NODISCARD constexpr operator storage_type && () && noexcept { return std::move(m_data); }

		ML_NODISCARD constexpr operator storage_type const && () const && noexcept { return std::move(m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator pointer() noexcept { return m_data; }

		ML_NODISCARD constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto left() const noexcept -> value_type { return m_data.at(0); }
		
		ML_NODISCARD constexpr auto top() const noexcept -> value_type { return m_data.at(1); }
		
		ML_NODISCARD constexpr auto width() const noexcept -> value_type { return m_data.at(2); }
		
		ML_NODISCARD constexpr auto height() const noexcept -> value_type { return m_data.at(3); }
		
		ML_NODISCARD constexpr auto bot() const noexcept -> value_type { return (top() + height()); }
		
		ML_NODISCARD constexpr auto right() const noexcept -> value_type { return (left() + width()); }
		
		ML_NODISCARD constexpr auto position() const noexcept -> coord_type { return { left(), top() }; }
		
		ML_NODISCARD constexpr auto size() const noexcept -> coord_type { return { width(), height() }; }
		
		ML_NODISCARD constexpr auto center() const noexcept -> coord_type { return (position() + (size() / (T)2)); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto left(value_type value) noexcept -> self_type & { m_data.at(0) = value; return (*this); }
		
		constexpr auto top(value_type value) noexcept -> self_type & { m_data.at(1) = value; return (*this); }
		
		constexpr auto width(value_type value) noexcept -> self_type & { m_data.at(2) = value; return (*this); }
		
		constexpr auto height(value_type value) noexcept -> self_type & { m_data.at(3) = value; return (*this); }
		
		constexpr auto bot(value_type value) noexcept -> self_type & { return height(value - top()); }
		
		constexpr auto right(value_type value) noexcept -> self_type & { return width(value - left()); }
		
		constexpr auto position(coord_type const & value) noexcept -> self_type & { return left(value[0]).top(value[1]); }
		
		constexpr auto size(coord_type const & value) noexcept -> self_type & { return width(value[0]).height(value[1]); }
		
		constexpr auto center(coord_type const & value) noexcept -> self_type & { return position(value - (size() / (T)2)); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias float_rect = basic_rect<float_t>;
	ML_alias int_rect	= basic_rect<int32_t>;
	ML_alias uint_rect	= basic_rect<uint32_t>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_RECT_HPP_