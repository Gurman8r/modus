#include <modus_core/graphics/Shader.hpp>

namespace ml
{
	bool shader_parser::parse(std::istream & in, gfx::program_source & out)
	{
		if (!in) { return false; }

		ds::array<ds::stringstream, gfx::shader_type_MAX> src{};

		ds::stringstream * dst{ &src[0] };

		ds::string line{};
		while (std::getline(in, line))
		{
			if (line.empty() || util::trim(line).empty()) { continue; }
			else if (line.front() == '#')
			{
				auto const toks{ util::tokenize(line, "# ") };

				if ((toks.size() == 3) && (toks[0] == "pragma") && (toks[1] == "shader"))
				{
					switch (hashof(toks[2]))
					{
					case hashof("vertex")	: dst = &src[gfx::shader_type_vertex]; break;
					case hashof("pixel")	: dst = &src[gfx::shader_type_pixel]; break;
					case hashof("geometry")	: dst = &src[gfx::shader_type_geometry]; break;
					}
				}
				else
				{
					(*dst) << line << '\n';
				}
			}
			else
			{
				(*dst) << line << '\n';
			}
		}

		for (size_t i = 0; i < src.size(); ++i)
		{
			if (ds::string str{ src[i].str() }; !str.empty())
			{
				out[i] = str;
			}
		}

		return true;
	}
}

namespace ml
{
	shader_library::~shader_library() noexcept {}
}