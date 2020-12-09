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

		template <class I, class T, class Default = T
		> bool get_json(json const & j, I && i, T & vv, Default const & dv = {})
		{
			if (j.contains(i))
			{
				j[i].get_to(vv);
				return true;
			}
			else
			{
				vv = ML_forward(dv);
				return false;
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