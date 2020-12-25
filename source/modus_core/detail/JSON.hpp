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
		> ML_NODISCARD bool json_contains(json const & j, Arg0 && arg0, Args && ... args)
		{
			if constexpr (0 < sizeof...(args))
			{
				json::const_iterator const it{ j.find(ML_forward(arg0)) };

				return (it != j.end()) && util::json_contains(*it, ML_forward(args)...);
			}
			else
			{
				return j.contains(ML_forward(arg0));
			}
		}

		template <class JsonPtr, class Arg0, class ... Args
		> ML_NODISCARD auto json_find(JsonPtr j, Arg0 && arg0, Args && ... args) -> JsonPtr
		{
			static_assert(std::is_pointer_v<decltype(j)>);
			if (!j)
			{
				return (JsonPtr)nullptr;
			}
			else if constexpr (0 < sizeof...(args))
			{
				return util::json_find(util::json_find(j, ML_forward(arg0)), ML_forward(args)...);
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

		ML_NODISCARD inline json json_load(fs::path const & path, json const & dv = {})
		{
			std::ifstream f{ path };
			ML_defer(&f) { f.close(); };
			return f ? json::parse(f) : dv;
		}

		inline bool read_json(fs::path const & path, json & j)
		{
			std::ifstream f{ path };
			ML_defer(&f) { f.close(); };
			if (!f) { return false; }
			else
			{
				j = json::parse(f);
				return true;
			}
		}

		inline bool write_json(fs::path const & path, json const & j)
		{
			std::ofstream f{ path };
			ML_defer(&f) { f.close(); };
			if (!f) { return false; }
			else
			{
				f << j;
				return true;
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