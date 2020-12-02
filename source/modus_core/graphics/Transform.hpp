#ifndef _ML_TRANSFORM_HPP_
#define _ML_TRANSFORM_HPP_

// WIP

#include <modus_core/detail/Matrix.hpp>

namespace ml
{
	struct transform final
	{
		using value_type				= typename float32;
		using self_type					= typename _ML transform;
		using size_type					= typename mat4::size_type;
		using difference_type			= typename mat4::difference_type;
		using pointer					= typename mat4::pointer;
		using reference					= typename mat4::reference;
		using const_pointer				= typename mat4::const_pointer;
		using const_reference			= typename mat4::const_reference;
		using rvalue					= typename mat4::rvalue;
		using iterator					= typename mat4::iterator;
		using const_iterator			= typename mat4::const_iterator;
		using reverse_iterator			= typename mat4::reverse_iterator;
		using const_reverse_iterator	= typename mat4::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr transform() noexcept : m_data{ mat4::identity() }
		{
		}

		constexpr transform(mat4 const & value) : m_data{ value }
		{
		}

		constexpr transform(mat4 && value) noexcept : m_data{ std::move(value) }
		{
		}

		constexpr transform(self_type const & other) : m_data{ other.m_data }
		{
		}

		constexpr transform(self_type && other) noexcept : m_data{ std::move(other.m_data) }
		{
		}

		transform(vec3 const & position, vec4 const & rotation, vec3 const & scale)
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr self_type & operator=(self_type const & other)
		{
			self_type temp{ other };
			swap(temp);
			return (*this);
		}

		constexpr self_type & operator=(self_type && other) noexcept
		{
			swap(std::move(other));
			return (*this);
		}

		constexpr void swap(self_type & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_data.swap(other.m_data);
			}
		}

		constexpr void swap(mat4 & other) noexcept
		{
			if (std::addressof(m_data) != std::addressof(other))
			{
				m_data.swap(other);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr operator mat4 & () & noexcept { return m_data; }

		constexpr operator mat4 const & () const & noexcept { return m_data; }

		constexpr operator mat4 && () && noexcept { return std::move(m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr operator pointer() noexcept { return m_data; }

		constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr auto operator*() & noexcept -> reference { return (*m_data); }

		constexpr auto operator*() const & noexcept -> const_reference { return (*m_data); }

		constexpr auto operator*() && noexcept -> rvalue { return std::move(*m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		constexpr auto begin() noexcept -> iterator { return m_data.begin(); }

		constexpr auto begin() const noexcept -> const_iterator { return m_data.begin(); }

		constexpr auto cbegin() const noexcept -> const_iterator { return m_data.cbegin(); }

		constexpr auto cend() const noexcept -> const_iterator { return m_data.cend(); }

		constexpr auto crbegin() const noexcept -> const_reverse_iterator { return m_data.crbegin(); }

		constexpr auto crend() const noexcept -> const_reverse_iterator { return m_data.crend(); }
		
		constexpr auto end() noexcept -> iterator { return m_data.end(); }

		constexpr auto end() const noexcept -> const_iterator { return m_data.end(); }

		constexpr auto rbegin() noexcept -> reverse_iterator { return m_data.rbegin(); }

		constexpr auto rbegin() const noexcept -> const_reverse_iterator { return m_data.rbegin(); }

		constexpr auto rend() noexcept -> reverse_iterator { return m_data.rend(); }

		constexpr auto rend() const noexcept -> const_reverse_iterator { return m_data.rend(); }
		
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
				static_assert(std::is_same_v<U, mat4>);
				return ML_compare(m_data, value.m_data);
			}
		}

		template <class U = self_type
		> ML_NODISCARD bool operator==(U const & value) const noexcept { return this->compare(value) == 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator!=(U const & value) const noexcept { return this->compare(value) != 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator<(U const & value) const noexcept { return this->compare(value) < 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator>(U const & value) const noexcept { return this->compare(value) > 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator<=(U const & value) const noexcept { return this->compare(value) <= 0; }

		template <class U = self_type
		> ML_NODISCARD bool operator>=(U const & value) const noexcept { return this->compare(value) >= 0; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		mat4 m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TRANSFORM_HPP_