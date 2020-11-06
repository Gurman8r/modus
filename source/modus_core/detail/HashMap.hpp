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
	> struct hashmap : public pmr::unordered_map<Key, Val, Hasher, Equals>
	{
		using pmr::unordered_map<Key, Val, Hasher, Equals>::unordered_map;
	};
}

#endif // !_ML_HASH_MAP_HPP_