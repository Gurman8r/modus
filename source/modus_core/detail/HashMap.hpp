#ifndef _ML_HASH_MAP_HPP_
#define _ML_HASH_MAP_HPP_

// WIP

#include <modus_core/detail/Utility.hpp>

namespace ml::ds
{
	// hashmap (PLACEHOLDER)
	template <
		class Key,
		class Value,
		class Hash = std::hash<Key>,
		class Equal = std::equal_to<Key>
	> struct hashmap : public pmr::unordered_map<Key, Value, Hash, Equal>
	{
		using pmr::unordered_map<Key, Value, Hash, Equal>::unordered_map;
	};
}

#endif // !_ML_HASH_MAP_HPP_