#ifndef _ML_HASH_HPP_
#define _ML_HASH_HPP_

#include <core/Common.hpp>

namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static constexpr auto fnv1a_basis{ static_cast<hash_t>(14695981039346656037ULL) };

	static constexpr auto fnv1a_prime{ static_cast<hash_t>(1099511628211ULL) };

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T
	> ML_NODISCARD constexpr hash_t hash(T const * arr, hash_t size, hash_t seed) noexcept
	{
		return size
			? hash(arr + 1, size - 1, (seed ^ static_cast<hash_t>(*arr)) * fnv1a_prime)
			: seed;
	}

	template <class T
	> ML_NODISCARD constexpr hash_t hash(T const * arr, hash_t size) noexcept
	{
		return hash(arr, size, fnv1a_basis);
	}

	template <class T, hash_t N
	> ML_NODISCARD constexpr hash_t hash(T const(&value)[N]) noexcept
	{
		return hash(value, N - 1);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Arr
	> ML_NODISCARD constexpr hash_t hash(Arr const & value) noexcept
	{
		return hash(value.data(), static_cast<hash_t>(value.size()));
	}

	template <template <class, hash_t...> class Arr, class T, hash_t ... N
	> ML_NODISCARD constexpr hash_t hash(Arr<T, N...> const & value) noexcept
	{
		return hash(value.data(), static_cast<hash_t>(value.size()));
	}

	template <template <class...> class Arr, class ... Ts
	> ML_NODISCARD constexpr hash_t hash(Arr<Ts...> const & value) noexcept
	{
		return hash(value.data(), static_cast<hash_t>(value.size()));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_HASH_HPP_
