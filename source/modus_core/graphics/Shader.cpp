#include <modus_core/graphics/Shader.hpp>

#ifdef ML_IMPL_RENDERER_OPENGL
#include <modus_core/backends/opengl/OpenGL_Shader.hpp>
using impl_builder	= _ML gfx::opengl_shader_builder;
using impl_program	= _ML gfx::opengl_program;
using impl_shader	= _ML gfx::opengl_shader;
#else
#error "shader is unavailable"
#endif

namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool parse_source(std::istream & in, program_source & out)
	{
		if (!in) { return false; }

		array<stringstream, shader_type_MAX> src{};
		stringstream * dst{ &src[0] };
		string line{};
		
		while (std::getline(in, line))
		{
			if (line.empty() || util::trim(line).empty()) { continue; }
			else if (line.front() == '#')
			{
				auto const toks{ util::tokenize(line, "# ") };

				if ((3 == toks.size()) && (toks[0] == "pragma") && (toks[1] == "shader"))
				{
					switch (hashof(toks[2]))
					{
					case hashof("vertex")	: dst = &src[shader_type_vertex]; break;
					case hashof("pixel")	: dst = &src[shader_type_pixel]; break;
					case hashof("geometry")	: dst = &src[shader_type_geometry]; break;
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
			if (string str{ src[i].str() }; !str.empty())
			{
				out[i] = str;
			}
		}

		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	std::ostream & shader_builder::emit_source(std::ostream & out, json const & in)
	{
		return impl_builder::emit_source(out, in);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}