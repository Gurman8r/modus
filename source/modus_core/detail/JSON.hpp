#ifndef _ML_JSON_HPP_
#define _ML_JSON_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/Standard.hpp>
#include <nlohmann/json.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	// json
	ML_alias json = typename nlohmann::basic_json
	<
		std::map,
		std::vector, std::string, bool, int64, uint64, float64,
		std::allocator,
		nlohmann::adl_serializer,
		std::vector<uint8>
	>;

	// helpers
	namespace util
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Arg0, class ... Args
		> ML_NODISCARD bool has_json(json const & j, Arg0 && arg0, Args && ... args)
		{
			if constexpr (0 < sizeof...(args))
			{
				json::const_iterator const it{ j.find(ML_forward(arg0)) };

				return (it != j.end()) && util::has_json(*it, ML_forward(args)...);
			}
			else
			{
				return j.contains(ML_forward(arg0));
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class JsonPtr, class Arg0, class ... Args
		> ML_NODISCARD auto find_json(JsonPtr j, Arg0 && arg0, Args && ... args) -> JsonPtr
		{
			static_assert(std::is_pointer_v<decltype(j)>);
			if (!j)
			{
				return (JsonPtr)nullptr;
			}
			else if constexpr (0 < sizeof...(args))
			{
				return util::find_json(util::find_json(j, ML_forward(arg0)), ML_forward(args)...);
			}
			else if (auto const it{ j->find(ML_forward(arg0)) }; it != j->end())
			{
				return (JsonPtr)std::addressof(*it);
			}
			else
			{
				return (JsonPtr)nullptr;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace std::filesystem
{
	inline void to_json(_ML json & j, path const & value) noexcept
	{
		j = value.native();
	}

	inline void from_json(_ML json const & j, path & value) noexcept
	{
		if (j.is_string()) { value = j.get<std::string>(); }
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_JSON_HPP_