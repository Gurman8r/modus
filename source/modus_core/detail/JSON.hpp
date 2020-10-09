#ifndef _ML_JSON_HPP_
#define _ML_JSON_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <Common.hpp>
#include <nlohmann/json.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	ML_alias json = typename nlohmann::json;

	//ML_alias json = typename nlohmann::basic_json
	//<
	//	std::map,
	//	std::vector, std::string, bool, int64_t, uint64_t, float64_t,
	//	std::allocator,
	//	nlohmann::adl_serializer
	//>;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace std::filesystem
{
	inline void to_json(_ML json & j, path const & value)
	{
		j = value.native();
	}

	inline void from_json(_ML json const & j, path & value)
	{
		if (j.is_string()) { value = j.get<pmr::string>(); }
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_JSON_HPP_