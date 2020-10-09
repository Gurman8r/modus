#ifndef _ML_WRAPPER_HPP_
#define _ML_WRAPPER_HPP_

#include <Config.hpp>

namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// value wrapper
	template <class T> struct ML_NODISCARD wrapper
	{
		using value_type		= typename T;
		using pointer			= typename T *;
		using const_pointer		= typename T const *;
		using reference			= typename T &;
		using const_reference	= typename T const &;
		using rvalue			= typename T &&;
		using const_rvalue		= typename T const &&;
		
		value_type m_value;

		constexpr auto operator->() noexcept -> pointer { return &m_value; }
		
		constexpr auto operator->() const noexcept -> const_pointer { return &m_value; }

		constexpr auto operator *() & noexcept -> reference { return m_value; }
		
		constexpr auto operator *() const & noexcept -> const_reference { return m_value; }

		constexpr auto operator *() && noexcept -> rvalue { return std::move(m_value); }
		
		constexpr auto operator *() const && noexcept -> const_rvalue { return std::move(m_value); }
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_WRAPPER_HPP_