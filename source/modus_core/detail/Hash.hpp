#ifndef _ML_HASH_HPP_
#define _ML_HASH_HPP_

#include <modus_core/Standard.hpp>

namespace ml
{
	struct FNV1A final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr FNV1A() noexcept = default;

		static constexpr hash_t basis{ static_cast<hash_t>(14695981039346656037ULL) };

		static constexpr hash_t prime{ static_cast<hash_t>(1099511628211ULL) };

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD constexpr hash_t operator()(T const * arr, hash_t size, hash_t seed) const noexcept
		{
			return size
				? FNV1A{}(arr + 1, size - 1, (seed ^ static_cast<hash_t>(*arr)) * prime)
				: seed;
		}

		template <class T
		> ML_NODISCARD constexpr hash_t operator()(T const * arr, hash_t size) const noexcept
		{
			return FNV1A{}(arr, size, basis);
		}

		template <class T, hash_t N
		> ML_NODISCARD constexpr hash_t operator()(T const(&value)[N]) const noexcept
		{
			return FNV1A{}(value, N - 1);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T, class = std::enable_if_t<std::is_scalar_v<T> && !std::is_pointer_v<T>>
		> ML_NODISCARD constexpr hash_t operator()(T const value) const noexcept
		{
			return (basis ^ static_cast<hash_t>(value)) * prime;
		}

		template <class Arr, class = std::enable_if_t<std::is_object_v<Arr>>
		> ML_NODISCARD constexpr hash_t operator()(Arr const & value) const noexcept
		{
			return FNV1A{}(value.data(), static_cast<hash_t>(value.size()));
		}

		template <template <class, hash_t...> class Arr, class T, hash_t ... N
		> ML_NODISCARD constexpr hash_t operator()(Arr<T, N...> const & value) const noexcept
		{
			return FNV1A{}(value.data(), static_cast<hash_t>(value.size()));
		}

		template <template <class...> class Arr, class ... Ts
		> ML_NODISCARD constexpr hash_t operator()(Arr<Ts...> const & value) const noexcept
		{
			return FNV1A{}(value.data(), static_cast<hash_t>(value.size()));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
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
	> ML_NODISCARD constexpr hash_t hashof(Arg0 && arg0, Args && ... args)
	{
		return FNV1A{}(ML_forward(arg0), ML_forward(args)...);
	}
}

#endif // !_ML_HASH_HPP_
