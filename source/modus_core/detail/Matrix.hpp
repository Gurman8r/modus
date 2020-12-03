#ifndef _ML_MATRIX_HPP_
#define _ML_MATRIX_HPP_

#include <modus_core/detail/Array.hpp>
#include <modus_core/detail/GLM.hpp>

// MATRIX
namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// fixed size matrix
	template <class _T, size_t _Width, size_t _Height
	> struct ML_NODISCARD matrix
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static_assert(0 < _Width, "matrix width negative or zero");

		static_assert(0 < _Height, "matrix height negative or zero");

		static_assert(std::is_scalar_v<_T>, "matrix only supports scalar types");

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using value_type				= typename _T;
		using self_type					= typename _ML ds::matrix<value_type, _Width, _Height>;
		using storage_type				= typename _ML ds::array<value_type, _Width * _Height>;
		using size_type					= typename storage_type::size_type;
		using difference_type			= typename storage_type::difference_type;
		using coord_type				= typename _ML ds::array<size_type, 2>;
		using pointer					= typename storage_type::pointer;
		using reference					= typename storage_type::reference;
		using const_pointer				= typename storage_type::const_pointer;
		using const_reference			= typename storage_type::const_reference;
		using rvalue					= typename storage_type::rvalue;
		using iterator					= typename storage_type::iterator;
		using const_iterator			= typename storage_type::const_iterator;
		using reverse_iterator			= typename storage_type::reverse_iterator;
		using const_reverse_iterator	= typename storage_type::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		storage_type m_data; // aggregate initializer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr void swap(self_type & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_data.swap(other.m_data);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD constexpr auto data() noexcept -> pointer { return m_data.data(); }

		ML_NODISCARD constexpr auto data() const noexcept -> const_pointer { return m_data.data(); }

		ML_NODISCARD constexpr bool empty() const noexcept { return false; }

		ML_NODISCARD constexpr auto height() const noexcept -> size_t { return _Height; }

		ML_NODISCARD constexpr auto max_size() const noexcept -> size_t { return m_data.max_size(); }

		ML_NODISCARD constexpr auto size() const noexcept -> size_t { return m_data.size(); }

		ML_NODISCARD constexpr auto width() const noexcept -> size_t { return _Width; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator storage_type & () & noexcept { return m_data; }

		ML_NODISCARD constexpr operator storage_type const & () const & noexcept { return m_data; }

		ML_NODISCARD constexpr operator storage_type && () && noexcept { return std::move(m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator pointer() noexcept { return m_data; }

		ML_NODISCARD constexpr operator const_pointer() const noexcept { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto operator*() & noexcept -> reference { return (*m_data); }

		ML_NODISCARD constexpr auto operator*() const & noexcept -> const_reference { return (*m_data); }

		ML_NODISCARD constexpr auto operator*() && noexcept -> rvalue { return std::move(*m_data); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto at(size_t const i) & noexcept -> reference { return m_data.at(i); }
		
		ML_NODISCARD constexpr auto at(size_t const i) const & noexcept -> const_reference { return m_data.at(i); }

		ML_NODISCARD constexpr auto at(size_t const i) && noexcept -> rvalue { return std::move(m_data.at(i)); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto at(size_t const x, size_t const y) & noexcept -> reference { return at(y * _Width + x); }

		ML_NODISCARD constexpr auto at(size_t const x, size_t const y) const & noexcept -> const_reference { return at(y * _Width + x); }

		ML_NODISCARD constexpr auto at(size_t const x, size_t const y) && noexcept -> rvalue { return std::move(at(y * _Width + x)); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto at(coord_type const & loc) & noexcept -> reference { return at(loc[0], loc[1]); }

		ML_NODISCARD constexpr auto at(coord_type const & loc) const & noexcept -> const_reference { return at(loc[0], loc[1]); }

		ML_NODISCARD constexpr auto at(coord_type const & loc) && noexcept -> rvalue { return std::move(at(loc[0], loc[1])); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto back() & noexcept -> reference { return m_data.back(); }

		ML_NODISCARD constexpr auto back() const & noexcept -> const_reference { return m_data.back(); }

		ML_NODISCARD constexpr auto back() && noexcept -> rvalue { return std::move(m_data.back()); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr auto front() & noexcept -> reference { return m_data.front(); }

		ML_NODISCARD constexpr auto front() const & noexcept -> const_reference { return m_data.front(); }

		ML_NODISCARD constexpr auto front() && noexcept -> rvalue { return std::move(m_data.front()); }

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

		ML_NODISCARD constexpr bool nonzero() const noexcept
		{
			for (auto const & e : m_data)
				if (e != (value_type)0)
					return true;
			return false;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr auto map_coord(size_t i) noexcept -> coord_type
		{
			return { i % _Width, i / _Height };
		}

		static constexpr auto map_coord(size_t x, size_t y) noexcept -> size_t
		{
			return y * _Width + x;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class U
		> self_type & copy_from(U const * value)
		{
			std::memcpy(m_data, value, sizeof(m_data));

			return (*this);
		}

		static constexpr self_type zero() noexcept
		{
			return self_type{};
		}

		static constexpr self_type fill(value_type value) noexcept
		{
			self_type temp{};
			for (auto & e : temp) { e = value; }
			return temp;
		}

		static constexpr self_type one() noexcept
		{
			return fill((value_type)1);
		}

		static constexpr self_type identity() noexcept
		{
			self_type temp{};
			for (size_t i = 0; i < (_Width * _Height); ++i)
			{
				if ((i / _Width) == (i % _Width))
				{
					temp[i] = (value_type)1;
				}
			}
			return temp;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class U, size_t W, size_t H
		> ML_NODISCARD constexpr operator matrix<U, W, H>() const noexcept
		{
			if constexpr (std::is_same_v<matrix<U, W, H>, self_type>)
			{
				return (*this); // same type
			}
			else
			{
				matrix<U, W, H> temp{};
				if constexpr ((W == _Width) && (H == _Height))
				{
					// same dimensions
					for (size_t i = 0; i < (W * H); ++i)
					{
						temp[i] = static_cast<U>(this->at(i));
					}
				}
				else
				{
					// different dimensions
					for (size_t i = 0; i < (W * H); ++i)
					{
						if (size_t const x{ i % W }, y{ i / W }; (x < _Width && y < _Height))
						{
							temp[i] = static_cast<U>(this->at(x, y));
						}
					}
				}
				return temp;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t L, class U, glm::qualifier Q = glm::defaultp
		> ML_NODISCARD operator glm::vec<L, U, Q>() const noexcept
		{
			using V = glm::vec<L, U, Q>;
			if constexpr (util::is_trivially_convertible_v<V, self_type>)
			{
				return util::bit_cast<V>(*this);
			}
			else
			{
				V temp{};
				for (size_t i = 0; i < L; ++i)
				{
					if (i < (_Width * _Height))
					{
						temp[i] = (U)this->at(i);
					}
				}
				return temp;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t W, size_t H, class U, glm::qualifier Q = glm::defaultp
		> ML_NODISCARD operator glm::mat<W, H, U, Q>() const noexcept
		{
			using M = glm::mat<W, H, U, Q>;
			if constexpr (util::is_trivially_convertible_v<M, self_type>)
			{
				return util::bit_cast<M>(*this);
			}
			else
			{
				M temp{};
				for (size_t i = 0; i < (W * H); ++i)
				{
					if (size_t const x{ i % W }, y{ i / W }; (x < _Width) && (y < _Height))
					{
						temp[x][y] = (U)this->at(x, y);
					}
				}
				return temp;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// define additional code
#ifdef ML_MATRIX_CLASS_EXTRA
		ML_MATRIX_CLASS_EXTRA
#endif // ML_MATRIX_CLASS_EXTRA

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// GLM
namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <size_t L, class T, glm::qualifier Q = glm::defaultp
	> ML_NODISCARD ds::matrix<T, L, 1> from_glm(glm::vec<L, T, Q> const & value)
	{
		return ds::matrix<T, L, 1>{}.copy_from(glm::value_ptr(value));
	}

	template <size_t W, size_t H, class T, glm::qualifier Q = glm::defaultp
	> ML_NODISCARD ds::matrix<T, W, H> from_glm(glm::mat<W, H, T, Q> const & value)
	{
		return ds::matrix<T, W, H>{}.copy_from(glm::value_ptr(value));
	}

	template <class T, glm::qualifier Q = glm::defaultp
	> ML_NODISCARD ds::matrix<T, 4, 1> from_glm(glm::qua<T, Q> const & value)
	{
		return ds::matrix<T, 4, 1>{}.copy_from(glm::value_ptr(value));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// ALIASES
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// matrix<T, N, N>
	template <class T, size_t N
	> ML_alias tmatnxn = ds::matrix<T, N, N>;

	// matrix<T, N, 1>
	template <class T, size_t N
	> ML_alias tvector = ds::matrix<T, N, 1>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// matrix<T, 2, 2>
	template <class T
	> ML_alias	tmat2	= tmatnxn<T, 2>;
	ML_alias	mat2b	= tmat2<byte>;
	ML_alias	mat2i	= tmat2<int32>;
	ML_alias	mat2u	= tmat2<uint32>;
	ML_alias	mat2f	= tmat2<float32>;
	ML_alias	mat2d	= tmat2<float64>;
	ML_alias	mat2s	= tmat2<size_t>;
	ML_alias	mat2	= mat2f;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// matrix<T, 3, 3>
	template <class T
	> ML_alias	tmat3	= tmatnxn<T, 3>;
	ML_alias	mat3b	= tmat3<byte>;
	ML_alias	mat3i	= tmat3<int32>;
	ML_alias	mat3u	= tmat3<uint32>;
	ML_alias	mat3f	= tmat3<float32>;
	ML_alias	mat3d	= tmat3<float64>;
	ML_alias	mat3s	= tmat3<size_t>;
	ML_alias	mat3	= mat3f;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// matrix<T, 4 ,4>
	template <class T
	> ML_alias	tmat4	= tmatnxn<T, 4>;
	ML_alias	mat4b	= tmat4<byte>;
	ML_alias	mat4i	= tmat4<int32>;
	ML_alias	mat4u	= tmat4<uint32>;
	ML_alias	mat4f	= tmat4<float32>;
	ML_alias	mat4d	= tmat4<float64>;
	ML_alias	mat4s	= tmat4<size_t>;
	ML_alias	mat4	= mat4f;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// matrix<T, 2, 1>
	template <class T
	> ML_alias	tvec2	= tvector<T, 2>;
	ML_alias	vec2b	= tvec2<byte>;
	ML_alias	vec2i	= tvec2<int32>;
	ML_alias	vec2u	= tvec2<uint32>;
	ML_alias	vec2f	= tvec2<float32>;
	ML_alias	vec2d	= tvec2<float64>;
	ML_alias	vec2s	= tvec2<size_t>;
	ML_alias	vec2	= vec2f;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// matrix<T, 3, 1>
	template <class T
	> ML_alias	tvec3	= tvector<T, 3>;
	ML_alias	vec3b	= tvec3<byte>;
	ML_alias	vec3i	= tvec3<int32>;
	ML_alias	vec3u	= tvec3<uint32>;
	ML_alias	vec3f	= tvec3<float32>;
	ML_alias	vec3d	= tvec3<float64>;
	ML_alias	vec3s	= tvec3<size_t>;
	ML_alias	vec3	= vec3f;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// matrix<T, 4, 1>
	template <class T
	> ML_alias	tvec4	= tvector<T, 4>;
	ML_alias	vec4b	= tvec4<byte>;
	ML_alias	vec4i	= tvec4<int32>;
	ML_alias	vec4u	= tvec4<uint32>;
	ML_alias	vec4f	= tvec4<float32>;
	ML_alias	vec4d	= tvec4<float64>;
	ML_alias	vec4s	= tvec4<size_t>;
	ML_alias	vec4	= vec4f;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// MATRIX OPERATORS
namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr bool operator==(matrix<Tx, W, H> const & lhs, matrix<Ty, W, H> const & rhs) noexcept
	{
		return (lhs.m_data == rhs.m_data);
	}

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr bool operator!=(matrix<Tx, W, H> const & lhs, matrix<Ty, W, H> const & rhs) noexcept
	{
		return (lhs.m_data != rhs.m_data);
	}

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr bool operator<(matrix<Tx, W, H> const & lhs, matrix<Ty, W, H> const & rhs) noexcept
	{
		return (lhs.m_data < rhs.m_data);
	}

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr bool operator<=(matrix<Tx, W, H> const & lhs, matrix<Ty, W, H> const & rhs) noexcept
	{
		return (lhs.m_data <= rhs.m_data);
	}

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr bool operator>(matrix<Tx, W, H> const & lhs, matrix<Ty, W, H> const & rhs) noexcept
	{
		return (lhs.m_data > rhs.m_data);
	}

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr bool operator>=(matrix<Tx, W, H> const & lhs, matrix<Ty, W, H> const & rhs) noexcept
	{
		return (lhs.m_data >= rhs.m_data);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Tx, class Ty, size_t W, size_t H
	> constexpr auto operator+=(matrix<Tx, W, H> & lhs, Ty const rhs) noexcept -> auto &
	{
		for (auto & e : lhs)
		{
			e += static_cast<Tx>(rhs);
		}
		return lhs;
	}

	template <class Tx, class Ty, size_t W, size_t H
	> constexpr auto operator-=(matrix<Tx, W, H> & lhs, Ty const rhs) noexcept -> auto &
	{
		for (auto & e : lhs)
		{
			e -= static_cast<Tx>(rhs);
		}
		return lhs;
	}

	template <class Tx, class Ty, size_t W, size_t H
	> constexpr auto operator*=(matrix<Tx, W, H> & lhs, Ty const rhs) noexcept -> auto &
	{
		for (auto & e : lhs)
		{
			e *= static_cast<Tx>(rhs);
		}
		return lhs;
	}

	template <class Tx, class Ty, size_t W, size_t H
	> constexpr auto operator/=(matrix<Tx, W, H> & lhs, Ty const rhs) noexcept -> auto &
	{
		for (auto & e : lhs)
		{
			e /= static_cast<Tx>(rhs);
		}
		return lhs;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr auto operator+(matrix<Tx, W, H> const & lhs, Ty const rhs) noexcept
	{
		auto temp{ lhs };
		return temp += static_cast<Tx>(rhs);
	}

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr auto operator-(matrix<Tx, W, H> const & lhs, Ty const rhs) noexcept
	{
		auto temp{ lhs };
		return temp -= static_cast<Tx>(rhs);
	}

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr auto operator*(matrix<Tx, W, H> const & lhs, Ty const rhs) noexcept
	{
		auto temp{ lhs };
		return temp *= static_cast<Tx>(rhs);
	}

	template <class Tx, class Ty, size_t W, size_t H
	> ML_NODISCARD constexpr auto operator/(matrix<Tx, W, H> const & lhs, Ty const rhs) noexcept
	{
		auto temp{ lhs };
		return temp /= static_cast<Tx>(rhs);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, size_t W, size_t H
	> ML_NODISCARD constexpr auto operator-(matrix<T, W, H> const & lhs) noexcept
	{
		return (lhs * static_cast<T>(-1));
	}

	template <class T, size_t W, size_t H
	> ML_NODISCARD constexpr auto operator+(matrix<T, W, H> const & lhs) noexcept
	{
		return -(-(lhs));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// VECTOR OPERATORS
namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Tx, class Ty, size_t N
	> ML_NODISCARD constexpr auto & operator+=(tvector<Tx, N> & lhs, tvector<Ty, N> const & rhs) noexcept
	{
		for (size_t i = 0; i < N; ++i)
		{
			lhs[i] += static_cast<Tx>(rhs[i]);
		}
		return lhs;
	}

	template <class Tx, class Ty, size_t N
	> ML_NODISCARD constexpr auto & operator-=(tvector<Tx, N> & lhs, tvector<Ty, N> const & rhs) noexcept
	{
		for (size_t i = 0; i < N; ++i)
		{
			lhs[i] -= static_cast<Tx>(rhs[i]);
		}
		return lhs;
	}

	template <class Tx, class Ty, size_t N
	> ML_NODISCARD constexpr auto & operator*=(tvector<Tx, N> & lhs, tvector<Ty, N> const & rhs) noexcept
	{
		for (size_t i = 0; i < N; ++i)
		{
			lhs[i] *= static_cast<Tx>(rhs[i]);
		}
		return lhs;
	}

	template <class Tx, class Ty, size_t N
	> ML_NODISCARD constexpr auto & operator/=(tvector<Tx, N> & lhs, tvector<Ty, N> const & rhs) noexcept
	{
		for (size_t i = 0; i < N; ++i)
		{
			lhs[i] /= static_cast<Tx>(rhs[i]);
		}
		return lhs;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Tx, class Ty, size_t N
	> ML_NODISCARD constexpr auto operator+(tvector<Tx, N> const & lhs, tvector<Ty, N> const & rhs) noexcept
	{
		auto temp{ lhs };
		return temp += rhs;
	}

	template <class Tx, class Ty, size_t N
	> ML_NODISCARD constexpr auto operator-(tvector<Tx, N> const & lhs, tvector<Ty, N> const & rhs) noexcept
	{
		auto temp{ lhs };
		return temp -= rhs;
	}

	template <class Tx, class Ty, size_t N
	> ML_NODISCARD constexpr auto operator*(tvector<Tx, N> const & lhs, tvector<Ty, N> const & rhs) noexcept
	{
		auto temp{ lhs };
		return temp *= rhs;
	}

	template <class Tx, class Ty, size_t N
	> ML_NODISCARD constexpr auto operator/(tvector<Tx, N> const & lhs, tvector<Ty, N> const & rhs) noexcept
	{
		auto temp{ lhs };
		return temp /= rhs;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// TUPLE INTERFACE
namespace std
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <size_t I, class T, size_t W, size_t H
	> ML_NODISCARD constexpr T & get(_ML ds::matrix<T, W, H> & value) noexcept
	{
		static_assert(I < W * H, "matrix index out of bounds");
		return value.at(I);
	}

	template <size_t I, class T, size_t W, size_t H
	> ML_NODISCARD constexpr T const & get(_ML ds::matrix<T, W, H> const & value) noexcept
	{
		static_assert(I < W * H, "matrix index out of bounds");
		return value.at(I);
	}

	template <size_t I, class T, size_t W, size_t H
	> ML_NODISCARD constexpr T && get(_ML ds::matrix<T, W, H> && value) noexcept
	{
		static_assert(I < W * H, "matrix index out of bounds");
		return std::move(value.at(I));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <size_t X, size_t Y, class T, size_t W, size_t H
	> ML_NODISCARD constexpr T & get(_ML ds::matrix<T, W, H> & value) noexcept
	{
		static_assert(X * Y < W * H, "matrix index out of bounds");
		return value.at(X, Y);
	}

	template <size_t X, size_t Y, class T, size_t W, size_t H
	> ML_NODISCARD constexpr T const & get(_ML ds::matrix<T, W, H> const & value) noexcept
	{
		static_assert(X * Y < W * H, "matrix index out of bounds");
		return value.at(X, Y);
	}

	template <size_t X, size_t Y, class T, size_t W, size_t H
	> ML_NODISCARD constexpr T && get(_ML ds::matrix<T, W, H> && value) noexcept
	{
		static_assert(X * Y < W * H, "matrix index out of bounds");
		return std::move(value.at(X, Y));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// JSON INTERFACE
namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, size_t W, size_t H
	> void from_json(json const & j, matrix<T, W, H> & value)
	{
		j.get_to(value.m_data);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, size_t W, size_t H
	> void to_json(json & j, matrix<T, W, H> const & value)
	{
		j = value.m_data;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// STREAM INTERFACE
namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, size_t W, size_t H
	> inline std::ostream & operator<<(std::ostream & out, matrix<T, W, H> const & value)
	{
		for (auto const & e : value)
			if (out.good())
				out << e << ' ';
		return out;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, size_t W, size_t H
	> inline std::istream & operator>>(std::istream & in, matrix<T, W, H> & value)
	{
		for (auto & e : value)
			if (in.good())
				in >> e;
		return in;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_MATRIX_HPP_