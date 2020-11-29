#ifndef _ML_JSON_HPP_
#define _ML_JSON_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/Standard.hpp>
#include <nlohmann/json.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	ML_alias json = typename nlohmann::basic_json
	<
		std::map,
		std::vector, std::string, bool, int64, uint64, float64,
		std::allocator,
		nlohmann::adl_serializer,
		std::vector<uint8>
	>;

	namespace util
	{
		template <class ID, class T, class Default = T
		> void get_from(json const & j, ID const & id, T & vv, Default const & dv = {})
		{
			if (j.contains(id))
			{
				j[id].get_to(vv);
			}
			else
			{
				vv = ML_forward(dv);
			}
		}
	}
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
		if (j.is_string()) { value = j.get<std::string>(); }
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_JSON_HPP_