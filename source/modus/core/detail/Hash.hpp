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
	> ML_NODISCARD constexpr hash_t fnv1a_hash(T const * arr, hash_t size, hash_t seed) noexcept
	{
		return size
			? fnv1a_hash(arr + 1, size - 1, (seed ^ static_cast<hash_t>(*arr)) * fnv1a_prime)
			: seed;
	}

	template <class T
	> ML_NODISCARD constexpr hash_t fnv1a_hash(T const * arr, hash_t size) noexcept
	{
		return fnv1a_hash(arr, size, fnv1a_basis);
	}

	template <class T, hash_t N
	> ML_NODISCARD constexpr hash_t fnv1a_hash(T const(&value)[N]) noexcept
	{
		return fnv1a_hash(value, N - 1);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Arr
	> ML_NODISCARD constexpr hash_t fnv1a_hash(Arr const & value) noexcept
	{
		return fnv1a_hash(value.data(), static_cast<hash_t>(value.size()));
	}

	template <template <class, hash_t...> class Arr, class T, hash_t ... N
	> ML_NODISCARD constexpr hash_t fnv1a_hash(Arr<T, N...> const & value) noexcept
	{
		return fnv1a_hash(value.data(), static_cast<hash_t>(value.size()));
	}

	template <template <class...> class Arr, class ... Ts
	> ML_NODISCARD constexpr hash_t fnv1a_hash(Arr<Ts...> const & value) noexcept
	{
		return fnv1a_hash(value.data(), static_cast<hash_t>(value.size()));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

namespace ml
{
	// hash of data
	template <class Arg0, class ... Args
	> constexpr auto hashof(Arg0 && arg0, Args && ... args)
	{
		if constexpr (0 == sizeof...(args))
		{
			using T = std::decay_t<decltype(arg0)>;

			if constexpr (std::is_same_v<T, hash_t>)
			{
				return arg0;
			}
			else if constexpr (std::is_scalar_v<T> && !std::is_pointer_v<T>)
			{
				return static_cast<hash_t>(arg0);
			}
			else
			{
				return util::fnv1a_hash(ML_forward(arg0));
			}
		}
		else
		{
			return util::fnv1a_hash(ML_forward(arg0), ML_forward(args)...);
		}
	}
}

#endif // !_ML_HASH_HPP_
