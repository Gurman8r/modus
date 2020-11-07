#ifndef _ML_HASH_MAP_HPP_
#define _ML_HASH_MAP_HPP_

// WIP / PLACEHOLDER

#include <modus_core/detail/Utility.hpp>

namespace ml::ds
{
	// hashmap
	template <
		class Key,
		class Val,
		class Hasher = std::hash<Key>,
		class Equals = std::equal_to<Key>
	> ML_alias hashmap = typename pmr::unordered_map
	<
		Key, Val, Hasher, Equals
	>;
}

#endif // !_ML_HASH_MAP_HPP_