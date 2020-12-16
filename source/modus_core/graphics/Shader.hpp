#ifndef _ML_SHADER_HPP_
#define _ML_SHADER_HPP_

// WIP

#include <modus_core/graphics/RenderAPI.hpp>

namespace ml::gfx
{
	// program source
	ML_alias program_source = ds::array
	<
		std::optional<ds::string>, shader_type_MAX
	>;
}

namespace ml
{
	// shader parser (WIP)
	class ML_CORE_API shader_parser final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static bool parse(std::istream & in, gfx::program_source & out);

		template <size_t N
		> static bool parse(const char(&in)[N], gfx::program_source & out) noexcept
		{
			ds::stringstream s{ in };
			return shader_parser::parse(s, out);
		}

		static bool parse(ds::string const & in, gfx::program_source & out) noexcept
		{
			ds::stringstream s{ in };
			return shader_parser::parse(s, out);
		}

		static bool parse(fs::path const & path, gfx::program_source & out) noexcept
		{
			std::ifstream f{ path }; ML_defer(&f) { f.close(); };
			return shader_parser::parse(f, out);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class In> static auto make_program(In && in) -> ds::ref<gfx::program>
		{
			if (gfx::program_source src{}
			; !shader_parser::parse(ML_forward(in), src)) { return nullptr; }
			else
			{
				auto temp{ gfx::program::create({}) };
				for (size_t i = 0; i < src.size(); ++i)
				{
					if (src[i])
					{
						temp->attach(i, *src[i]);
					}
				}
				if (!temp->link())
				{
					debug::warning(temp->get_info_log());
				}
				return temp;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	// shader library (WIP)
	struct ML_CORE_API shader_library final
	{
		~shader_library() noexcept;
	};
}

#endif // !_ML_SHADER_HPP_