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
		ML_NODISCARD inline json load_json(fs::path const & path, json const & dv = {})
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

		template <class Arg0, class ... Args
		> ML_NODISCARD bool has_json(json const & j, Arg0 && arg0, Args && ... args)
		{
			if constexpr (0 < sizeof...(args))
			{
				return _ML util::has_json(j, ML_forward(arg0))
					&& _ML util::has_json(j[ML_forward(arg0)], ML_forward(args)...);
			}
			else
			{
				return j.contains(ML_forward(arg0));
			}
		}

		template <class JsonPtr, class Arg0, class ... Args
		> ML_NODISCARD auto json_ptr(JsonPtr j, Arg0 && arg0, Args && ... args) -> JsonPtr
		{
			if constexpr (0 < sizeof...(args))
			{
				return util::json_ptr(util::json_ptr(j, ML_forward(arg0)), ML_forward(args)...);
			}
			else if (j && j->contains(ML_forward(arg0)))
			{
				return std::addressof(j->at(ML_forward(arg0)));
			}
			else
			{
				return nullptr;
			}
		}

		template <class J, class Arg0, class ... Args
		> ML_NODISCARD auto json_ref(std::optional<std::reference_wrapper<J>> j, Arg0 && arg0, Args && ... args) -> std::optional<std::reference_wrapper<J>>
		{
			if constexpr (0 < sizeof...(args))
			{
				return util::json_ref(util::json_ref(j, ML_forward(arg0)), ML_forward(args)...);
			}
			else if (j && j->get().contains(ML_forward(arg0)))
			{
				return j->get().at(ML_forward(arg0));
			}
			else
			{
				return std::nullopt;
			}
		}
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