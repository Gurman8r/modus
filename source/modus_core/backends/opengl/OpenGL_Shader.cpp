#include "./OpenGL_Shader.hpp"

namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	std::ostream & opengl_shader_builder::emit_source(std::ostream & out, json const & in)
	{
		// empty
		if (in.empty()) { return out; }

		// sections
		json const
			* const j_shader{ util::find_json(&in, "shader") },
			* const j_layout{ util::find_json(&in, "layout") },
			* const j_buffer{ util::find_json(&in, "buffer") },
			* const j_result{ util::find_json(&in, "result") };
		if (!j_shader || !j_layout || !j_buffer || !j_result) { return out; }

		// shader type
		shader_type_ type;
		if (j_shader->is_number()) { j_shader->get_to((uint32 &)type); }
		else if (j_shader->is_string())
		{
			ds::string str; j_shader->get_to(str);
			switch (hashof(util::to_lower(str))) {
			default					: return out;
			case hashof("vertex")	: type = shader_type_vertex; break;
			case hashof("pixel")	: type = shader_type_pixel; break;
			case hashof("geometry")	: type = shader_type_geometry; break;
			}
		}

		return out;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}