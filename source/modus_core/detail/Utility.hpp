#ifndef _ML_UTILITY_HPP_
#define _ML_UTILITY_HPP_

#include <gcem/include/gcem.hpp>
#include <modus_core/detail/Flow.hpp>
#include <modus_core/detail/JSON.hpp>
#include <modus_core/detail/Meta.hpp>
#include <modus_core/detail/Singleton.hpp>	// NonCopyable.hpp
#include <modus_core/detail/TypeInfo.hpp>	// Hash.hpp

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// macro min
#define ML_min(a, b)				((a) < (b) ? (a) : (b))

// macro max
#define ML_max(a, b)				((a) > (b) ? (a) : (b))

// macro clamp
#define ML_clamp(v, lo, hi)			ML_min(ML_max(v, lo), hi)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// read flag at index
#define ML_flag_read(val, i)		(((i) & (val)) == (i))

// set flag at index
#define ML_flag_set(val, i)			((val) |= (i))

// clear flag at index
#define ML_flag_clear(val, i)		((val) &= ~(i))

// conditional set or clear flag
#define ML_flag_write(val, i, cond)	((cond) ? ML_flag_set(val, i) : ML_flag_clear(val, i))

// map between flag bits
#define ML_flag_map(dst, d, src, s)	ML_flag_write(dst, d, ML_flag_read(src, s))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// read bit at index
#define ML_bit_read(val, i)			ML_flag_read(val >> i, 1)

// set bit at index
#define ML_bit_set(val, i)			ML_flag_set(val, 1 << i)

// clear bit at index
#define ML_bit_clear(val, i)		ML_flag_clear(val, 1 << i)

// conditional set or clear bit
#define ML_bit_write(val, i, cond)	ML_flag_write(val, 1 << i, cond)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// flags
namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class V, class I
	> constexpr auto & flag_set(V & value, I const index) noexcept
	{
		static_assert(std::is_scalar_v<V> && std::is_scalar_v<I>);
		return value |= index;
	}

	template <class V, class I
	> constexpr auto & flag_clear(V & value, I const index) noexcept
	{
		static_assert(std::is_scalar_v<V> && std::is_scalar_v<I>);
		return value &= ~index;
	}

	template <class V, class I
	> constexpr auto & flag_write(V & value, I const index, bool cond) noexcept
	{
		return cond ? _ML util::flag_set(value, index) : _ML util::flag_clear(value, index);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class V, class I
	> constexpr auto & bit_set(V & value, I const index) noexcept
	{
		return _ML util::flag_set(value, 1 << index);
	}

	template <class V, class I
	> constexpr auto & bit_clear(V & value, I const index) noexcept
	{
		return _ML util::flag_clear(value, 1 << index);
	}

	template <class V, class I
	> constexpr auto & bit_write(V & value, I const index, bool cond) noexcept
	{
		return _ML util::flag_write(value, 1 << index, cond);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// pi
namespace ml::util
{
#define ML_pi 3.14159f

	template <class T = float32
	> struct pi_t final
	{
		static constexpr auto value{ static_cast<T>(ML_pi) };
	};

	template <class T = float32
	> static constexpr auto pi_v
	{
		pi_t<T>::value
	};

	template <class T = float32, class U = T
	> constexpr auto deg2rad(U value) noexcept
	{
		return static_cast<T>(value) * (pi_v<T> / static_cast<T>(180));
	}

	template <class T = float32, class U = T
	> constexpr T rad2deg(U value) noexcept
	{
		return static_cast<T>(value) * (static_cast<T>(180) / pi_v<T>);
	}
}

// misc
namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// constructor
	template <class T, class ... Args
	> void construct(T * ptr, Args && ... args)
	{
		::new (ptr) T{ ML_forward(args)... };
	}

	// destructor
	template <class T
	> void destruct(T * ptr)
	{
		ptr->~T();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// constexpr swap
	template <class T
	> constexpr void swap(T & lhs, T & rhs) noexcept
	{
		auto temp{ std::move(lhs) };
		lhs = std::move(rhs);
		rhs = std::move(temp);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// requires T is any of
	template <class T, class ... Ts
	> constexpr bool is_any_of_v
	{
		std::disjunction_v<std::is_same<T, Ts>...>
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class To, class From
	> constexpr bool is_trivially_convertible_v
	{
		"requires To is trivially default constructible and is copy or move constructible"
		&& sizeof(To) == sizeof(From)
		&& std::is_trivially_copyable_v<From>
		&& std::is_trivial_v<To>
		&& (std::is_copy_constructible_v<To> || std::is_move_constructible_v<To>)
	};

	// bit cast
	template <class To, class From
	> ML_NODISCARD To bit_cast(From const & value) noexcept
	{
		static_assert(is_trivially_convertible_v<To, From>);
		To temp{};
		std::memcpy(&temp, &value, sizeof(To));
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// comparison
namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class LHS, class RHS
	> ML_NODISCARD constexpr auto compare(LHS && lhs, RHS && rhs) noexcept
	{
		return ML_compare(ML_forward(lhs), ML_forward(rhs));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class LI, class RI
	> ML_NODISCARD constexpr bool range_equal(LI lBegin, LI lEnd, RI rBegin, RI rEnd)
	{
		return (lBegin != lEnd && rBegin != rEnd)
			? ((*lBegin == *rBegin) && _ML util::range_equal(lBegin + 1, lEnd, rBegin + 1, rEnd))
			: (lBegin == lEnd && rBegin == rEnd);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class LI, class RI
	> ML_NODISCARD constexpr bool range_less(LI lBegin, LI lEnd, RI rBegin, RI rEnd)
	{
		return (lBegin != lEnd && rBegin != rEnd)
			? ((*lBegin < *rBegin) && _ML util::range_less(lBegin + 1, lEnd, rBegin + 1, rEnd))
			: (lBegin == lEnd && rBegin == rEnd);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// min / max
namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class LHS, class RHS, class ... Rest
	> constexpr decltype(auto) min(LHS && lhs, RHS && rhs, Rest && ... rest)
	{
		return lhs < rhs
			? _ML util::min(ML_forward(lhs), ML_forward(rest)...)
			: _ML util::min(ML_forward(rhs), ML_forward(rest)...);
	}

	template <class Only
	> constexpr decltype(auto) min(Only && only)
	{
		return ML_forward(only);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class LHS, class RHS, class ... Rest
	> constexpr decltype(auto) max(LHS && lhs, RHS && rhs, Rest && ... rest)
	{
		return lhs > rhs
			? _ML util::max(ML_forward(lhs), ML_forward(rest)...)
			: _ML util::max(ML_forward(rhs), ML_forward(rest)...);
	}

	template <class Only
	> constexpr decltype(auto) max(Only && only)
	{
		return ML_forward(only);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// maths
namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class V2
	> ML_NODISCARD constexpr auto scale_to_fit(V2 const & lhs, V2 const & rhs) noexcept
	{
		auto const
			h{ V2{ (rhs[0] / lhs[0]), (rhs[0] / lhs[0]) } },
			v{ V2{ (rhs[1] / lhs[1]), (rhs[1] / lhs[1]) } };
		return lhs * ((h < v) ? h : v);
	}

	template <class Src, class Clbk
	> ML_NODISCARD constexpr auto maintain(Src const & source, Clbk const & target) noexcept
	{
		Clbk const temp{ target[0] / source[0], target[1] / source[1] };
		auto const ratio{ (temp[0] < temp[1]) ? temp[0] : temp[1] };
		return Src{ source[0] * ratio, source[1] * ratio };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class R, class T = float32
	> ML_NODISCARD constexpr auto ratio_cast(T v = T{ 1 }) noexcept
	{
		if constexpr (((T)R::num == (T)1) && ((T)R::den == (T)1))
		{
			return v;
		}
		else if constexpr ((T)R::num != (T)1 && (T)R::den == (T)1)
		{
			return v * (T)R::num;
		}
		else if constexpr ((T)R::num == (T)1 && (T)R::den != (T)1)
		{
			return v / (T)R::den;
		}
		else
		{
			return v * (T)R::num / (T)R::den;
		}
	}

	template <class T, int64 Num, int64 Den
	> ML_NODISCARD constexpr auto ratio_cast(T v, std::ratio<Num, Den> const & r)
	{
		auto const
			one{ static_cast<T>(1) },
			num{ static_cast<T>(r.num) },
			den{ static_cast<T>(r.den) };
		return ((num == one) && (den == one))
			? v
			: (((num != one) && (den == one))
				? v * num
				: (((num == one) && (den != one))
					? v / den
					: v * num / den));
	}

	template <class T
	> ML_NODISCARD constexpr auto power_of_2(T v)
	{
		// https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
		if constexpr (std::is_floating_point_v<T>)
		{
			return gcem::round(gcem::pow(T{ 2 }, gcem::ceil(gcem::log(v) / gcem::log(T{ 2 }))));
		}
		else
		{
			static_assert(std::is_integral_v<T>);
			v--;
			if constexpr (sizeof(T) >= 1)
				v |= v >> 1; v |= v >> 2; v |= v >> 4;	// 8-bit
			if constexpr (sizeof(T) >= 2) v |= v >> 8;	// 16-bit
			if constexpr (sizeof(T) >= 4) v |= v >> 16; // 32-bit
			if constexpr (sizeof(T) >= 8) v |= v >> 32; // 64-bit
			v++;
			return v;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// byte literals
namespace ml::byte_literals
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// kibibyte
	ML_NODISCARD constexpr uint64 operator"" _KiB(uint64 n) noexcept
	{
		return _ML util::power_of_2(_ML util::ratio_cast(n, std::kilo{}));
	}

	ML_NODISCARD constexpr uint64 operator"" _KiB(float80 n) noexcept
	{
		return static_cast<uint64>(_ML util::power_of_2(_ML util::ratio_cast(n, std::kilo{})));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// mebibyte
	ML_NODISCARD constexpr uint64 operator"" _MiB(uint64 n) noexcept
	{
		return _ML util::power_of_2(_ML util::ratio_cast(n, std::mega{}));
	}

	ML_NODISCARD constexpr uint64 operator"" _MiB(float80 n) noexcept
	{
		return static_cast<uint64>(_ML util::power_of_2(_ML util::ratio_cast(n, std::mega{})));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// gibibyte
	ML_NODISCARD constexpr uint64 operator"" _GiB(uint64 n) noexcept
	{
		return _ML util::power_of_2(_ML util::ratio_cast(n, std::giga{}));
	}

	ML_NODISCARD constexpr uint64 operator"" _GiB(float80 n) noexcept
	{
		return static_cast<uint64>(_ML util::power_of_2(_ML util::ratio_cast(n, std::giga{})));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// tebibyte
	ML_NODISCARD constexpr uint64 operator"" _TiB(uint64 n) noexcept
	{
		return _ML util::power_of_2(_ML util::ratio_cast(n, std::tera{}));
	}

	ML_NODISCARD constexpr uint64 operator"" _TiB(float80 n) noexcept
	{
		return static_cast<uint64>(_ML util::power_of_2(_ML util::ratio_cast(n, std::tera{})));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// pebibyte
	ML_NODISCARD constexpr uint64 operator"" _PiB(uint64 n) noexcept
	{
		return _ML util::power_of_2(_ML util::ratio_cast(n, std::peta{}));
	}

	ML_NODISCARD constexpr uint64 operator"" _PiB(float80 n) noexcept
	{
		return static_cast<uint64>(_ML util::power_of_2(_ML util::ratio_cast(n, std::peta{})));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// exbibyte
	ML_NODISCARD constexpr uint64 operator"" _EiB(uint64 n) noexcept
	{
		return _ML util::power_of_2(_ML util::ratio_cast(n, std::exa{}));
	}

	ML_NODISCARD constexpr uint64 operator"" _EiB(float80 n) noexcept
	{
		return static_cast<uint64>(_ML util::power_of_2(_ML util::ratio_cast(n, std::exa{})));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_UTILITY_HPP_