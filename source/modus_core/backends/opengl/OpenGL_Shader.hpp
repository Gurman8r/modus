#ifndef _ML_OPENGL_SHADER_HPP_
#define _ML_OPENGL_SHADER_HPP_

#include "./OpenGL_RenderAPI.hpp"

namespace ml::gfx
{
	class opengl_shader_builder final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		static std::ostream & emit_source(std::ostream & out, json const & in);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_OPENGL_SHADER_HPP_