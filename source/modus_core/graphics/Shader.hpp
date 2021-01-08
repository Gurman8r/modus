#ifndef _ML_SHADER_HPP_
#define _ML_SHADER_HPP_

// WIP

#include <modus_core/graphics/RenderAPI.hpp>

namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// program source
	ML_alias program_source = array<std::optional<string>, shader_type_MAX>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// parse program source
	ML_CORE_API bool parse_source(std::istream & in, program_source & out);

	template <size_t N
	> bool parse_source(const char(&in)[N], program_source & out) noexcept
	{
		stringstream s{ in };
		return parse_source(s, out);
	}

	inline bool parse_source(string const & in, program_source & out) noexcept
	{
		stringstream s{ in };
		return parse_source(s, out);
	}

	inline bool parse_source(fs::path const & path, program_source & out) noexcept
	{
		std::ifstream f{ path };
		ML_defer(&f) { f.close(); };
		return parse_source(f, out);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// parse program
	template <class In> ref<program> parse_program(In && in)
	{
		if (program_source src{}; _ML gfx::parse_source(ML_forward(in), src))
		{
			ref<program> ptr{ program::create({}) };
			
			for (size_t i = 0; i < src.size(); ++i)
			{
				if (src[i]) { ptr->attach((uint32)i, *src[i]); }
			}
			
			if (!ptr->link()) { debug::warn(ptr->get_info_log()); }
			
			return ptr;
		}
		else
		{
			return nullptr;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// shader builder
	class ML_CORE_API shader_builder final
	{
	public:
		static std::ostream & emit_source(std::ostream & out, json const & in);
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_SHADER_HPP_