#if defined(ML_IMPL_RENDERER_OPENGL)

#include "Impl_RenderAPI_OpenGL.hpp"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// opengl loader
#ifdef ML_OPENGL_LOADER_CUSTOM
#	if ML_has_include(ML_OPENGL_LOADER_CUSTOM)
#		include ML_OPENGL_LOADER_CUSTOM
#	endif

#elif defined(ML_IMPL_OPENGL_ES2)
#	include <GLES2/gl2.h>

#elif defined(ML_IMPL_OPENGL_ES3)
#	if defined(ML_os_apple) && (TARGET_OS_IOS || TARGET_OS_TV)
#		include <OpenGLES/ES3/gl.h>
#	else
#		include <GLES3/gl3.h>
#	endif

#elif defined(ML_IMPL_OPENGL_LOADER_GLEW)
#	include <GL/glew.h>

#elif defined(ML_IMPL_OPENGL_LOADER_GL3W)
#	include <GL/gl3w.h>

#elif defined(ML_IMPL_OPENGL_LOADER_GLAD)
#	include <glad/glad.h>

#else
#	error "unknown or invalid opengl loader specified"
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// opengl init
#ifndef ML_IMPL_OPENGL_INIT
#	if defined(ML_IMPL_OPENGL_LOADER_GLEW)
//										glew
#		define ML_IMPL_OPENGL_INIT()	((glewExperimental = true) && (glewInit() == GLEW_OK))

#	elif defined(ML_IMPL_OPENGL_LOADER_GL3W)
//										gl3w
#		define ML_IMPL_OPENGL_INIT()	gl3wInit()

#	elif defined(ML_IMPL_OPENGL_LOADER_GLAD)
//										glad
#		define ML_IMPL_OPENGL_INIT()	gladLoadGL()
#	else
//										custom
#		define ML_IMPL_OPENGL_INIT()	false
#	endif
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// helpers / disambiguation

// get error
#define ML_glGetError() _ML gfx::_error<_ML gfx::to_user>( glGetError() )

// check error
#if ML_is_debug || defined(ML_IMPL_GFX_CHECK)
#	define ML_glCheck(expr) do{ (expr); gl_check_error(#expr, __FILE__, __LINE__); } while(0)
#else
#	define ML_glCheck(expr) (expr)
#endif

// enable / disable
#define ML_glEnable(id, enabled) (enabled ? &glEnable : &glDisable)( id )

// shader functions
#define ML_glCreateShader(type)						glCreateShaderObjectARB( type )
#define ML_glDeleteShader(s)						glDeleteObjectARB( s )
#define ML_glShaderSource(s, count, src, len)		glShaderSourceARB( s, count, src, len )
#define ML_glCompileShader(s)						glCompileShaderARB( s )
#define ML_glGetShaderCompileStatus(s, i)			glGetObjectParameterivARB( s, GL_OBJECT_COMPILE_STATUS_ARB, i )

// program functions
#define ML_glCreateProgram()						glCreateProgramObjectARB()
#define ML_glDeleteProgram(p)						glDeleteObjectARB( p )
#define ML_glGetProgram()							glGetHandleARB( GL_PROGRAM_OBJECT_ARB )
#define ML_glUseProgram(p)							glUseProgramObjectARB( p )
#define ML_glAttachShader(p, s)						glAttachObjectARB( p, s )
#define ML_glDetachShader(p, s)						glDetachObjectARB( p, s )
#define ML_glLinkProgram(p)							glLinkProgramARB( p )
#define ML_glGetProgramLinkStatus(p, i)				glGetObjectParameterivARB( p, GL_OBJECT_LINK_STATUS_ARB, i )
#define ML_glValidateProgram(p)						glValidateProgramARB( p )
#define ML_glGetProgramValidateStatus(p, i)			glGetObjectParameterivARB( p, GL_OBJECT_VALIDATE_STATUS_ARB, i )

// uniform functions
#define ML_glGetUniformLocation(obj, name)			glGetUniformLocationARB( obj, name )
#define ML_glUniform1i(loc, x)						glUniform1iARB( loc, x )
#define ML_glUniform1f(loc, x)						glUniform1fARB( loc, x )
#define ML_glUniform2f(loc, x, y)					glUniform2fARB( loc, x, y )
#define ML_glUniform3f(loc, x, y, z)				glUniform3fARB( loc, x, y, z )
#define ML_glUniform4f(loc, x, y, z, w)				glUniform4fARB( loc, x, y, z, w )
#define ML_glUniformMatrix2fv(loc, transpose, ptr)	glUniformMatrix2fvARB( loc, 2 * 2, transpose, ptr )
#define ML_glUniformMatrix3fv(loc, transpose, ptr)	glUniformMatrix3fvARB( loc, 3 * 3, transpose, ptr )
#define ML_glUniformMatrix4fv(loc, transpose, ptr)	glUniformMatrix4fvARB( loc, 4 * 4, transpose, ptr )

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//  enums
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _action(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default				: return value;
			case action_keep	: return GL_KEEP;
			case action_zero	: return GL_ZERO;
			case action_replace	: return GL_REPLACE;
			case action_inc		: return GL_INCR;
			case action_inc_wrap: return GL_INCR_WRAP;
			case action_dec		: return GL_DECR;
			case action_dec_wrap: return GL_DECR_WRAP;
			case action_invert	: return GL_INVERT;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default				: return value;
			case GL_KEEP		: return action_keep;
			case GL_ZERO		: return action_zero;
			case GL_REPLACE		: return action_replace;
			case GL_INCR		: return action_inc;
			case GL_INCR_WRAP	: return action_inc_wrap;
			case GL_DECR		: return action_dec;
			case GL_DECR_WRAP	: return action_dec_wrap;
			case GL_INVERT		: return action_invert;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _error(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default									: return value;
			case error_none							: return GL_NO_ERROR;
			case error_invalid_enum					: return GL_INVALID_ENUM;
			case error_invalid_value				: return GL_INVALID_VALUE;
			case error_invalid_operation			: return GL_INVALID_OPERATION;
			case error_stack_overflow				: return GL_STACK_OVERFLOW;
			case error_stack_underflow				: return GL_STACK_UNDERFLOW;
			case error_out_of_memory				: return GL_OUT_OF_MEMORY;
			case error_invalid_framebuffer_operation: return GL_INVALID_FRAMEBUFFER_OPERATION;
			case error_context_lost					: return GL_CONTEXT_LOST;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default									: return value;
			case GL_NO_ERROR						: return error_none;
			case GL_INVALID_ENUM					: return error_invalid_enum;
			case GL_INVALID_VALUE					: return error_invalid_value;
			case GL_INVALID_OPERATION				: return error_invalid_operation;
			case GL_STACK_OVERFLOW					: return error_stack_overflow;
			case GL_STACK_UNDERFLOW					: return error_stack_underflow;
			case GL_OUT_OF_MEMORY					: return error_out_of_memory;
			case GL_INVALID_FRAMEBUFFER_OPERATION	: return error_invalid_framebuffer_operation;
			case GL_CONTEXT_LOST					: return error_context_lost;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _equation(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default							: return value;
			case equation_add				: return GL_FUNC_ADD;
			case equation_subtract			: return GL_FUNC_SUBTRACT;
			case equation_reverse_subtract	: return GL_FUNC_REVERSE_SUBTRACT;
			case equation_min				: return GL_MIN;
			case equation_max				: return GL_MAX;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default							: return value;
			case GL_FUNC_ADD				: return equation_add;
			case GL_FUNC_SUBTRACT			: return equation_subtract;
			case GL_FUNC_REVERSE_SUBTRACT	: return equation_reverse_subtract;
			case GL_MIN						: return equation_min;
			case GL_MAX						: return equation_max;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _facet(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default						: return value;
			case facet_front_left		: return GL_FRONT_LEFT;
			case facet_front_right		: return GL_FRONT_RIGHT;
			case facet_back_left		: return GL_BACK_LEFT;
			case facet_back_right		: return GL_BACK_RIGHT;
			case facet_front			: return GL_FRONT;
			case facet_back				: return GL_BACK;
			case facet_left				: return GL_LEFT;
			case facet_right			: return GL_RIGHT;
			case facet_front_and_back	: return GL_FRONT_AND_BACK;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default					: return value;
			case GL_FRONT_LEFT		: return facet_front_left;
			case GL_FRONT_RIGHT		: return facet_front_right;
			case GL_BACK_LEFT		: return facet_back_left;
			case GL_BACK_RIGHT		: return facet_back_right;
			case GL_FRONT			: return facet_front;
			case GL_BACK			: return facet_back;
			case GL_LEFT			: return facet_left;
			case GL_RIGHT			: return facet_right;
			case GL_FRONT_AND_BACK	: return facet_front_and_back;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _factor(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default							: return value;
			case factor_zero				: return GL_ZERO;
			case factor_one					: return GL_ONE;
			case factor_src_color			: return GL_SRC_COLOR;
			case factor_one_minus_src_color	: return GL_ONE_MINUS_SRC_COLOR;
			case factor_src_alpha			: return GL_SRC_ALPHA;
			case factor_one_minus_src_alpha	: return GL_ONE_MINUS_SRC_ALPHA;
			case factor_dst_alpha			: return GL_DST_ALPHA;
			case factor_one_minus_dst_alpha	: return GL_ONE_MINUS_DST_ALPHA;
			case factor_dst_color			: return GL_DST_COLOR;
			case factor_one_minus_dst_color	: return GL_ONE_MINUS_DST_COLOR;
			case factor_src_alpha_saturate	: return GL_SRC_ALPHA_SATURATE;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default						: return value;
			case GL_ZERO				: return factor_zero;
			case GL_ONE					: return factor_one;
			case GL_SRC_COLOR			: return factor_src_color;
			case GL_ONE_MINUS_SRC_COLOR	: return factor_one_minus_src_color;
			case GL_SRC_ALPHA			: return factor_src_alpha;
			case GL_ONE_MINUS_SRC_ALPHA	: return factor_one_minus_src_alpha;
			case GL_DST_ALPHA			: return factor_dst_alpha;
			case GL_ONE_MINUS_DST_ALPHA	: return factor_one_minus_dst_alpha;
			case GL_DST_COLOR			: return factor_dst_color;
			case GL_ONE_MINUS_DST_COLOR	: return factor_one_minus_dst_color;
			case GL_SRC_ALPHA_SATURATE	: return factor_src_alpha_saturate;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _format(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default							: return value;
			case format_red					: return GL_RED;
			case format_green				: return GL_GREEN;
			case format_blue				: return GL_BLUE;
			case format_alpha				: return GL_ALPHA;
			case format_rgb					: return GL_RGB;
			case format_rgba				: return GL_RGBA;
			case format_luminance			: return GL_LUMINANCE;
			case format_luminance_alpha		: return GL_LUMINANCE_ALPHA;
			case format_srgb				: return GL_SRGB;
			case format_srgb8				: return GL_SRGB8;
			case format_srgb_alpha			: return GL_SRGB_ALPHA;
			case format_srgb8_alpha8		: return GL_SRGB8_ALPHA8;
			case format_sluminance_alpha	: return GL_SLUMINANCE_ALPHA;
			case format_sluminance8_alpha8	: return GL_SLUMINANCE8_ALPHA8;
			case format_sluminance			: return GL_SLUMINANCE;
			case format_sluminance8			: return GL_SLUMINANCE8;

			case format_depth_stencil		: return GL_DEPTH_STENCIL;
			case format_depth24_stencil8	: return GL_DEPTH24_STENCIL8;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default							: return value;
			case GL_RED						: return format_red;
			case GL_GREEN					: return format_green;
			case GL_BLUE					: return format_blue;
			case GL_ALPHA					: return format_alpha;
			case GL_RGB						: return format_rgb;
			case GL_RGBA					: return format_rgba;
			case GL_LUMINANCE				: return format_luminance;
			case GL_LUMINANCE_ALPHA			: return format_luminance_alpha;
			case GL_SRGB					: return format_srgb;
			case GL_SRGB8					: return format_srgb8;
			case GL_SRGB_ALPHA				: return format_srgb_alpha;
			case GL_SRGB8_ALPHA8			: return format_srgb8_alpha8;
			case GL_SLUMINANCE_ALPHA		: return format_sluminance_alpha;
			case GL_SLUMINANCE8_ALPHA8		: return format_sluminance8_alpha8;
			case GL_SLUMINANCE				: return format_sluminance;
			case GL_SLUMINANCE8				: return format_sluminance8;

			case GL_DEPTH_STENCIL			: return format_depth_stencil;
			case GL_DEPTH24_STENCIL8		: return format_depth24_stencil8;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _order(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default			: return value;
			case order_cw	: return GL_CW;
			case order_ccw	: return GL_CCW;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default		: return value;
			case GL_CW	: return order_cw;
			case GL_CCW	: return order_ccw;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _predicate(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default					: return value;
			case predicate_never	: return GL_NEVER;
			case predicate_less		: return GL_LESS;
			case predicate_equal	: return GL_EQUAL;
			case predicate_lequal	: return GL_LEQUAL;
			case predicate_greater	: return GL_GREATER;
			case predicate_notequal	: return GL_NOTEQUAL;
			case predicate_gequal	: return GL_GEQUAL;
			case predicate_always	: return GL_ALWAYS;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default			: return value;
			case GL_NEVER	: return predicate_never;
			case GL_LESS	: return predicate_less;
			case GL_EQUAL	: return predicate_equal;
			case GL_LEQUAL	: return predicate_lequal;
			case GL_GREATER	: return predicate_greater;
			case GL_NOTEQUAL: return predicate_notequal;
			case GL_GEQUAL	: return predicate_gequal;
			case GL_ALWAYS	: return predicate_always;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _primitive(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default							: return value;
			case primitive_points			: return GL_POINTS;
			case primitive_lines			: return GL_LINES;
			case primitive_line_loop		: return GL_LINE_LOOP;
			case primitive_line_strip		: return GL_LINE_STRIP;
			case primitive_triangles		: return GL_TRIANGLES;
			case primitive_triangle_strip	: return GL_TRIANGLE_STRIP;
			case primitive_triangle_fan		: return GL_TRIANGLE_FAN;
			case primitive_fill				: return GL_FILL;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default					: return value;
			case GL_POINTS			: return primitive_points;
			case GL_LINES			: return primitive_lines;
			case GL_LINE_LOOP		: return primitive_line_loop;
			case GL_LINE_STRIP		: return primitive_line_strip;
			case GL_TRIANGLES		: return primitive_triangles;
			case GL_TRIANGLE_STRIP	: return primitive_triangle_strip;
			case GL_TRIANGLE_FAN	: return primitive_triangle_fan;
			case GL_FILL			: return primitive_fill;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _shader_type(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default						: return value;
			case shader_type_vertex		: return GL_VERTEX_SHADER;
			case shader_type_fragment	: return GL_FRAGMENT_SHADER;
			case shader_type_geometry	: return GL_GEOMETRY_SHADER;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default					: return value;
			case GL_VERTEX_SHADER	: return shader_type_vertex;
			case GL_FRAGMENT_SHADER	: return shader_type_fragment;
			case GL_GEOMETRY_SHADER	: return shader_type_geometry;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _texture_type(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default					: return value;
			case texture_type_1d	: return GL_TEXTURE_1D;
			case texture_type_2d	: return GL_TEXTURE_2D;
			case texture_type_3d	: return GL_TEXTURE_3D;
			case texture_type_cube	: return GL_TEXTURE_CUBE_MAP;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default					: return value;
			case GL_TEXTURE_1D		: return texture_type_1d;
			case GL_TEXTURE_2D		: return texture_type_2d;
			case GL_TEXTURE_3D		: return texture_type_3d;
			case GL_TEXTURE_CUBE_MAP: return texture_type_cube;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _type(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default				: return value;
			case type_byte		: return GL_BYTE;
			case type_ubyte		: return GL_UNSIGNED_BYTE;
			case type_short		: return GL_SHORT;
			case type_ushort	: return GL_UNSIGNED_SHORT;
			case type_int		: return GL_INT;
			case type_uint		: return GL_UNSIGNED_INT;
			case type_float		: return GL_FLOAT;
			case type_half_float: return GL_HALF_FLOAT;
			case type_uint_24_8	: return GL_UNSIGNED_INT_24_8;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default						: return value;
			case GL_BYTE				: return type_byte;
			case GL_UNSIGNED_BYTE		: return type_ubyte;
			case GL_SHORT				: return type_short;
			case GL_UNSIGNED_SHORT		: return type_ushort;
			case GL_INT					: return type_int;
			case GL_UNSIGNED_INT		: return type_uint;
			case GL_FLOAT				: return type_float;
			case GL_HALF_FLOAT			: return type_half_float;
			case GL_UNSIGNED_INT_24_8	: return type_uint_24_8;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32_t _usage(uint32_t value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default				: return value;
			case usage_stream	: return GL_STREAM_DRAW;
			case usage_static	: return GL_STATIC_DRAW;
			case usage_dynamic	: return GL_DYNAMIC_DRAW;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default					: return value;
			case GL_STREAM_DRAW		: return usage_stream;
			case GL_STATIC_DRAW		: return usage_static;
			case GL_DYNAMIC_DRAW	: return usage_dynamic;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32_t>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// utility
namespace ml::gfx
{
	// check error
	static void gl_check_error(cstring expr, cstring file, size_t line) noexcept
	{
		if (auto const code{ ML_glGetError() })
		{
			std::cout
				<< "| An internal OpenGL error occurred ( " << code << " )\n"
				<< "|    " << fs::path{ file }.filename() << " (" << line << ")\n"
				<< "|  expression:\n"
				<< "|    " << expr << "\n"
				<< "|  description:\n"
				<< "|    " << error_NAMES[code] << "\n"
				<< "|    " << error_DESCS[code] << "\n"
				<< "\n";
		}
	}

	// get program info log
	static void gl_get_program_info_log(uint32_t obj, pmr::string & buf) noexcept
	{
		int32_t max_len{};
		ML_glCheck(glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &max_len));
		buf.resize((size_t)max_len);
		ML_glCheck(glGetProgramInfoLog(obj, max_len, &max_len, buf.data()));
		buf.push_back(0);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render device
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_render_device::opengl_render_device(allocator_type alloc) : render_device{}, m_alloc{ alloc }
	{
		static bool const opengl_init{ ML_IMPL_OPENGL_INIT() };
		ML_assert_msg(opengl_init, "failed initializing opengl device");

		// renderer
		ML_glCheck(m_data.renderer = (cstring)glGetString(GL_RENDERER));

		// vendor
		ML_glCheck(m_data.vendor = (cstring)glGetString(GL_VENDOR));

		// version
		ML_glCheck(m_data.version = (cstring)glGetString(GL_VERSION));

		// major version
		if (glGetIntegerv(GL_MAJOR_VERSION, &m_data.major_version); glGetError() == GL_INVALID_ENUM)
		{
			m_data.major_version = !m_data.version.empty() ? m_data.version[0] - '0' : 1;
		}

		// minor version
		if (glGetIntegerv(GL_MINOR_VERSION, &m_data.minor_version); glGetError() == GL_INVALID_ENUM)
		{
			m_data.minor_version = !m_data.version.empty() ? m_data.version[2] - '0' : 1;
		}

		// extensions
		{
			int32_t num{};
			ML_glCheck(glGetIntegerv(GL_NUM_EXTENSIONS, &num));
			m_data.extensions.reserve(num);

			pmr::stringstream ss{};
			ML_glCheck(ss.str((cstring)glGetString(GL_EXTENSIONS)));

			pmr::string line{};
			while (std::getline(ss, line, ' '))
			{
				m_data.extensions.push_back(line);
			}
		}

		// texture edge clamp available
#if defined(GL_EXT_texture_edge_clamp) \
|| defined(GLEW_EXT_texture_edge_clamp) \
|| defined(GL_SGIS_texture_edge_clamp)
		m_data.texture_edge_clamp_available = true;
#endif
		// max texture slots
		ML_glCheck(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (int32_t *)&m_data.max_texture_slots));

		// max color attachments
		ML_glCheck(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (int32_t *)&m_data.max_color_attachments));

		// max samples
		ML_glCheck(glGetIntegerv(GL_MAX_SAMPLES, (int32_t *)&m_data.max_samples));

		// shaders available
#if defined(GL_ARB_shading_language_100) \
|| defined(GL_ARB_shader_objects) \
|| defined(GL_ARB_vertex_shader) \
|| defined(GL_ARB_fragment_shader)
		m_data.shaders_available = true;

		// geometry shaders available
#	ifdef GL_ARB_geometry_shader4
		m_data.geometry_shaders_available = true;
#	endif

		// shading language version
		ML_glCheck(m_data.shading_language_version = (cstring)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
	}

	opengl_render_device::~opengl_render_device()
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	shared<render_context> opengl_render_device::create_context(context_settings const & cs) noexcept
	{
		return memory::alloc_ref<opengl_render_context>(m_alloc, this, cs);
	}

	shared<vertexarray> opengl_render_device::create_vertexarray(uint32_t prim) noexcept
	{
		return memory::alloc_ref<opengl_vertexarray>(m_alloc, this, prim);
	}

	shared<vertexbuffer> opengl_render_device::create_vertexbuffer(uint32_t usage, size_t count, addr_t data) noexcept
	{
		return memory::alloc_ref<opengl_vertexbuffer>(m_alloc, this, usage, count, data);
	}

	shared<indexbuffer> opengl_render_device::create_indexbuffer(uint32_t usage, size_t count, addr_t data) noexcept
	{
		return memory::alloc_ref<opengl_indexbuffer>(m_alloc, this, usage, count, data);
	}

	shared<texture2d> opengl_render_device::create_texture2d(desc_<texture2d> const & value, addr_t data) noexcept
	{
		return memory::alloc_ref<opengl_texture2d>(m_alloc, this, value, data);
	}

	shared<texturecube> opengl_render_device::create_texturecube(desc_<texturecube> const & value) noexcept
	{
		return memory::alloc_ref<opengl_texturecube>(m_alloc, this, value);
	}

	shared<framebuffer> opengl_render_device::create_framebuffer(desc_<framebuffer> const & value) noexcept
	{
		return memory::alloc_ref<opengl_framebuffer>(m_alloc, this, value);
	}

	shared<program> opengl_render_device::create_program() noexcept
	{
		return memory::alloc_ref<opengl_program>(m_alloc, this);
	}

	shared<shader> opengl_render_device::create_shader(desc_<shader> const & value) noexcept
	{
		return memory::alloc_ref<opengl_shader>(m_alloc, this, value);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render context
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_render_context::opengl_render_context(render_device * dev, context_settings const & desc)
		: render_context{ dev }, m_data{ desc }
	{
		ML_assert("invalid context api" && desc.api == context_api_opengl);
		ML_assert("major version mismatch" && dev->get_info().major_version >= desc.major);
		ML_assert("minor version mismatch" && dev->get_info().minor_version >= desc.minor);

		ML_glCheck(ML_glEnable(GL_MULTISAMPLE, desc.multisample));
		ML_glCheck(ML_glEnable(GL_FRAMEBUFFER_SRGB, desc.srgb_capable));
		ML_glCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	alpha_state * opengl_render_context::get_alpha_state(alpha_state * value) const
	{
		static alpha_state temp{
		}; if (!value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_ALPHA_TEST, (uint8_t *)&value->enabled));

		ML_glCheck(glGetIntegerv(GL_ALPHA_TEST_FUNC, (int32_t *)&value->pred));
		value->pred = _predicate<to_user>(value->pred);

		ML_glCheck(glGetFloatv(GL_ALPHA_TEST_REF, &value->ref));

		return value;
	}

	blend_state * opengl_render_context::get_blend_state(blend_state * value) const
	{
		static blend_state temp{
		}; if (!value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_BLEND, (uint8_t *)&value->enabled));

		ML_glCheck(glGetFloatv(GL_BLEND_COLOR, value->color));

		ML_glCheck(glGetIntegerv(GL_BLEND_EQUATION_RGB, (int32_t *)&value->color_equation));
		value->color_equation = _equation<to_user>(value->color_equation);

		ML_glCheck(glGetIntegerv(GL_BLEND_SRC_RGB, (int32_t *)&value->color_sfactor));
		value->color_sfactor = _factor<to_user>(value->color_sfactor);

		ML_glCheck(glGetIntegerv(GL_BLEND_DST_RGB, (int32_t *)&value->color_dfactor));
		value->color_dfactor = _factor<to_user>(value->color_dfactor);

		ML_glCheck(glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (int32_t *)&value->alpha_equation));
		value->alpha_equation = _equation<to_user>(value->alpha_equation);

		ML_glCheck(glGetIntegerv(GL_BLEND_SRC_ALPHA, (int32_t *)&value->alpha_sfactor));
		value->alpha_sfactor = _factor<to_user>(value->alpha_sfactor);

		ML_glCheck(glGetIntegerv(GL_BLEND_DST_ALPHA, (int32_t *)&value->alpha_dfactor));
		value->alpha_dfactor = _factor<to_user>(value->alpha_dfactor);

		return value;
	}

	color opengl_render_context::get_clear_color() const
	{
		color temp{};

		ML_glCheck(glGetFloatv(GL_COLOR_CLEAR_VALUE, temp));

		return temp;
	}

	cull_state * opengl_render_context::get_cull_state(cull_state * value) const
	{
		static cull_state temp{
		}; if (!value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_CULL_FACE, (uint8_t *)&value->enabled));

		ML_glCheck(glGetIntegerv(GL_CULL_FACE_MODE, (int32_t *)&value->facet));
		value->facet = _facet<to_user>(value->facet);

		ML_glCheck(glGetIntegerv(GL_FRONT_FACE, (int32_t *)&value->order));
		value->order = _order<to_user>(value->order);

		return value;
	}

	depth_state * opengl_render_context::get_depth_state(depth_state * value) const
	{
		static depth_state temp{
		}; if (!value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_DEPTH_TEST, (uint8_t *)&value->enabled));

		ML_glCheck(glGetIntegerv(GL_DEPTH_FUNC, (int32_t *)&value->pred));
		value->pred = _predicate<to_user>(value->pred);
		
		ML_glCheck(glGetFloatv(GL_DEPTH_RANGE, value->range));
		
		return value;
	}

	stencil_state * opengl_render_context::get_stencil_state(stencil_state * value) const
	{
		static stencil_state temp{
		}; if (!value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_STENCIL_TEST, (uint8_t *)&value->enabled));
		{
			ML_glCheck(glGetIntegerv(GL_STENCIL_FUNC, (int32_t *)&value->front.pred));
			value->front.pred = _predicate<to_user>(value->front.pred);

			ML_glCheck(glGetIntegerv(GL_STENCIL_REF, &value->front.ref));

			ML_glCheck(glGetIntegerv(GL_STENCIL_VALUE_MASK, (int32_t *)&value->front.mask));
		}
		{
			ML_glCheck(glGetIntegerv(GL_STENCIL_BACK_FUNC, (int32_t *)&value->back.pred));
			value->back.pred = _predicate<to_user>(value->back.pred);

			ML_glCheck(glGetIntegerv(GL_STENCIL_BACK_REF, &value->back.ref));

			ML_glCheck(glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, (int32_t *)&value->back.mask));
		}

		return value;
	}

	int_rect opengl_render_context::get_viewport() const
	{
		int_rect temp{};

		ML_glCheck(glGetIntegerv(GL_VIEWPORT, temp));

		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void opengl_render_context::set_alpha_state(alpha_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_ALPHA_TEST, value.enabled));

		ML_glCheck(glAlphaFunc(_predicate<to_impl>(value.pred), value.ref));
	}

	void opengl_render_context::set_blend_state(blend_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_BLEND, value.enabled));
		
		ML_glCheck(glBlendColor(
			value.color[0],
			value.color[1],
			value.color[2],
			value.color[3]));
		
		ML_glCheck(glBlendFuncSeparate(
			_factor<to_impl>(value.color_sfactor),
			_factor<to_impl>(value.color_dfactor),
			_factor<to_impl>(value.alpha_sfactor),
			_factor<to_impl>(value.alpha_dfactor)));

		ML_glCheck(glBlendEquationSeparate(
			_equation<to_impl>(value.color_equation),
			_equation<to_impl>(value.alpha_equation)));
	}

	void opengl_render_context::set_clear_color(color const & value)
	{
		ML_glCheck(glClearColor(value[0], value[1], value[2], value[3]));
	}

	void opengl_render_context::set_cull_state(cull_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_CULL_FACE, value.enabled));
		
		ML_glCheck(glCullFace(_facet<to_impl>(value.facet)));
		
		ML_glCheck(glFrontFace(_order<to_impl>(value.order)));
	}

	void opengl_render_context::set_depth_state(depth_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_DEPTH_TEST, value.enabled));
		
		ML_glCheck(glDepthFunc(_predicate<to_impl>(value.pred)));

		ML_glCheck(glDepthRangef(value.range[0], value.range[1]));
	}

	void opengl_render_context::set_stencil_state(stencil_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_STENCIL_TEST, value.enabled));

		ML_glCheck(glStencilFuncSeparate(
			GL_FRONT,
			_predicate<to_impl>(value.front.pred),
			value.front.ref,
			value.front.mask));
		
		ML_glCheck(glStencilFuncSeparate(
			GL_BACK,
			_predicate<to_impl>(value.back.pred),
			value.back.ref,
			value.back.mask));
	}

	void opengl_render_context::set_viewport(int_rect const & value)
	{
		ML_glCheck(glViewport(value[0], value[1], value[2], value[3]));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void opengl_render_context::clear(uint32_t mask)
	{
		uint32_t temp{};
		ML_flag_map(temp, GL_COLOR_BUFFER_BIT	, mask, clear_color		);
		ML_flag_map(temp, GL_DEPTH_BUFFER_BIT	, mask, clear_depth		);
		ML_flag_map(temp, GL_STENCIL_BUFFER_BIT	, mask, clear_stencil	);
		ML_glCheck(glClear(temp));
	}

	void opengl_render_context::draw(shared<vertexarray> const & value)
	{
		// could be moved into header file

		if (!value || value->get_vertices().empty()) { return; }

		bind_vertexarray(value.get());

		if (auto const prim{ value->get_primitive() }
		; auto const & ib{ value->get_indices() })
		{
			bind_indexbuffer(ib.get());

			for (auto const & vb : value->get_vertices())
			{
				bind_vertexbuffer(vb.get());

				draw_indexed(prim, ib->get_count());
			}
		}
		else
		{
			for (auto const & vb : value->get_vertices())
			{
				bind_vertexbuffer(vb.get());

				draw_arrays(prim, 0, vb->get_count());
			}
		}
	}

	void opengl_render_context::draw_arrays(uint32_t prim, size_t first, size_t count)
	{
		ML_glCheck(glDrawArrays(_primitive<to_impl>(prim), (uint32_t)first, (uint32_t)count));
	}

	void opengl_render_context::draw_indexed(uint32_t prim, size_t count)
	{
		ML_glCheck(glDrawElements(_primitive<to_impl>(prim), (uint32_t)count, GL_UNSIGNED_INT, nullptr));
	}

	void opengl_render_context::flush()
	{
		ML_glCheck(glFlush());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void opengl_render_context::bind_vertexarray(vertexarray const * value)
	{
		ML_glCheck(glBindVertexArray(value ? ML_handle(uint32_t, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_vertexbuffer(vertexbuffer const * value)
	{
		ML_glCheck(glBindBuffer(GL_ARRAY_BUFFER, value ? ML_handle(uint32_t, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_indexbuffer(indexbuffer const * value)
	{
		ML_glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, value ? ML_handle(uint32_t, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_texture(texture const * value, uint32_t slot)
	{
		ML_glCheck(glBindTextureUnit(slot, value ? ML_handle(uint32_t, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_framebuffer(framebuffer const * value)
	{
		if (value)
		{
			ML_glCheck(glBindFramebuffer(GL_FRAMEBUFFER, ML_handle(uint32_t, value->get_handle())));

			set_viewport({ vec2i::zero(), value->get_data().size });
		}
		else
		{
			ML_glCheck(glBindFramebuffer(GL_FRAMEBUFFER, NULL));
		}
	}

	void opengl_render_context::bind_program(program const * value)
	{
		ML_glCheck(ML_glUseProgram(value ? ML_handle(uint32_t, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_shader(shader const * value)
	{
		ML_glCheck(glUseProgramStages(
			m_handle,
			GL_VERTEX_SHADER_BIT,
			value ? ML_handle(uint32_t, value->get_handle()) : NULL));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void opengl_render_context::upload(uniform_id loc, bool value)
	{
		ML_glCheck(ML_glUniform1i(ML_handle(int32_t, loc), (int32_t)value));
	}

	void opengl_render_context::upload(uniform_id loc, int32_t value)
	{
		ML_glCheck(ML_glUniform1i(ML_handle(int32_t, loc), value));
	}

	void opengl_render_context::upload(uniform_id loc, float_t value)
	{
		ML_glCheck(ML_glUniform1f(ML_handle(int32_t, loc), value));
	}

	void opengl_render_context::upload(uniform_id loc, vec2f const & value)
	{
		ML_glCheck(ML_glUniform2f(ML_handle(int32_t, loc), value[0], value[1]));
	}

	void opengl_render_context::upload(uniform_id loc, vec3f const & value)
	{
		ML_glCheck(ML_glUniform3f(ML_handle(int32_t, loc), value[0], value[1], value[2]));
	}

	void opengl_render_context::upload(uniform_id loc, vec4f const & value)
	{
		ML_glCheck(ML_glUniform4f(ML_handle(int32_t, loc), value[0], value[1], value[2], value[3]));
	}

	void opengl_render_context::upload(uniform_id loc, mat2f const & value)
	{
		ML_glCheck(ML_glUniformMatrix2fv(ML_handle(int32_t, loc), false, value));
	}

	void opengl_render_context::upload(uniform_id loc, mat3f const & value)
	{
		ML_glCheck(ML_glUniformMatrix3fv(ML_handle(int32_t, loc), false, value));
	}

	void opengl_render_context::upload(uniform_id loc, mat4f const & value)
	{
		ML_glCheck(ML_glUniformMatrix4fv(ML_handle(int32_t, loc), false, value));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// vertexarray
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_vertexarray::opengl_vertexarray(render_device * dev, uint32_t prim)
		: vertexarray{ dev }, m_prim{ prim }
	{
		ML_glCheck(glGenVertexArrays(1, &m_handle));
		ML_glCheck(glBindVertexArray(m_handle));
	}

	opengl_vertexarray::~opengl_vertexarray()
	{
		ML_glCheck(glDeleteVertexArrays(1, &m_handle));
	}

	bool opengl_vertexarray::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteVertexArrays(1, &m_handle)); }

		m_vertices.clear(); m_indices.reset();
		
		ML_glCheck(glGenVertexArrays(1, &m_handle));

		return (bool)m_handle;
	}

	void opengl_vertexarray::add_vertices(shared<vertexbuffer> const & value)
	{
		if (!m_handle || !value) { return; }
		
		bind();
		
		m_vertices.emplace_back(value)->bind();

		for (size_t i = 0; i < m_layout.elements().size(); ++i)
		{
			auto const & e{ m_layout.elements()[i] };

			if (uint32_t const type{ std::invoke([&]() noexcept -> uint32_t
			{
				switch (e.get_base_type())
				{
				default					: return 0			; // unknown
				case hashof_v<bool>		: return GL_BOOL	; // bool
				case hashof_v<int32_t>	: return GL_INT		; // int
				case hashof_v<float_t>	: return GL_FLOAT	; // float
				}
			}) }; type == GL_INT)
			{
				ML_glCheck(glVertexAttribIPointer(
					(uint32_t)i,
					e.get_component_count(),
					type,
					m_layout.stride(),
					reinterpret_cast<addr_t>(e.offset)));
			}
			else
			{
				ML_glCheck(glVertexAttribPointer(
					(uint32_t)i,
					e.get_component_count(),
					type,
					e.normalized,
					m_layout.stride(),
					reinterpret_cast<addr_t>(e.offset)));
			}
			ML_glCheck(glEnableVertexAttribArray((uint32_t)i));
		}
	}

	void opengl_vertexarray::set_indices(shared<indexbuffer> const & value)
	{
		bind();

		if (m_indices = value) { m_indices->bind(); }
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// vertexbuffer
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_vertexbuffer::opengl_vertexbuffer(render_device * dev, uint32_t usage, size_t count, addr_t data)
		: vertexbuffer{ dev }, m_usage{ usage }, m_buffer{ bufcpy<float_t>(count, data) }
	{
		ML_glCheck(glGenBuffers(1, &m_handle));
		ML_glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_handle));
		ML_glCheck(glBufferData(
			GL_ARRAY_BUFFER,
			(uint32_t)m_buffer.size(),
			m_buffer.data(),
			_usage<to_impl>(m_usage)));
	}

	opengl_vertexbuffer::~opengl_vertexbuffer()
	{
		ML_glCheck(glDeleteBuffers(1, &m_handle));
	}

	bool opengl_vertexbuffer::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteBuffers(1, &m_handle)); }

		m_buffer.clear();

		ML_glCheck(glGenBuffers(1, &m_handle));

		return (bool)m_handle;
	}

	void opengl_vertexbuffer::set_data(size_t count, addr_t data, size_t offset)
	{
		m_buffer = bufcpy<float_t>(count, data);
		ML_glCheck(glBufferSubData(
			GL_ARRAY_BUFFER,
			offset,
			(uint32_t)m_buffer.size(),
			m_buffer.data()));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// indexbuffer
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_indexbuffer::opengl_indexbuffer(render_device * dev, uint32_t usage, size_t count, addr_t data)
		: indexbuffer{ dev }, m_usage{ usage }, m_buffer{ bufcpy<uint32_t>(count, data) }
	{
		ML_glCheck(glGenBuffers(1, &m_handle));
		ML_glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle));
		ML_glCheck(glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			(uint32_t)m_buffer.size(),
			m_buffer.data(),
			_usage<to_impl>(m_usage)));
	}

	opengl_indexbuffer::~opengl_indexbuffer()
	{
		ML_glCheck(glDeleteBuffers(1, &m_handle));
	}

	bool opengl_indexbuffer::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteBuffers(1, &m_handle)); }

		m_buffer.clear();
		
		ML_glCheck(glGenBuffers(1, &m_handle));
		
		return (bool)m_handle;
	}

	void opengl_indexbuffer::set_data(size_t count, addr_t data, size_t offset)
	{
		m_buffer = bufcpy<uint32_t>(count, data);
		ML_glCheck(glBufferSubData(
			GL_ELEMENT_ARRAY_BUFFER,
			offset,
			(uint32_t)m_buffer.size(),
			m_buffer.data()));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texture2d
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_texture2d::opengl_texture2d(render_device * dev, desc_<texture2d> const & value, addr_t data)
		: texture2d{ dev }, m_data{ value }
	{
		ML_glCheck(glGenTextures(1, &m_handle));
		ML_glCheck(glBindTexture(GL_TEXTURE_2D, m_handle));
		ML_glCheck(glTexImage2D(
			GL_TEXTURE_2D,
			0,
			_format<to_impl>(m_data.format.color),
			m_data.size[0],
			m_data.size[1],
			0,
			_format<to_impl>(m_data.format.pixel),
			_type<to_impl>(m_data.format.type),
			data));
		set_repeated(m_data.flags & texture_flags_repeat);
		set_smooth(m_data.flags & texture_flags_smooth);
		set_mipmapped(m_data.flags & texture_flags_mipmap);
	}

	opengl_texture2d::~opengl_texture2d()
	{
		ML_glCheck(glDeleteTextures(1, &m_handle));
	}

	bool opengl_texture2d::revalue()
	{
		if (!m_locked) { return debug::error("texture2d is not locked"); }

		if (m_handle) { ML_glCheck(glDeleteTextures(1, &m_handle)); }
		
		ML_glCheck(glGenTextures(1, &m_handle));

		return (bool)m_handle;
	}

	void opengl_texture2d::lock()
	{
		m_locked = true;

		debug::warning("texture lock/unlock NYI");
	}

	void opengl_texture2d::unlock()
	{
		m_locked = false;

		debug::warning("texture lock/unlock NYI");
	}

	void opengl_texture2d::update(vec2i const & size, addr_t data)
	{
		if (!m_locked) { return (void)debug::error("texture2d is not locked"); }

		if (m_handle && (m_data.size == size)) { return; }
		else { m_data.size = size; }

		revalue(); bind();
		
		ML_glCheck(glTexImage2D(
			GL_TEXTURE_2D,
			0,
			_format<to_impl>(m_data.format.color),
			m_data.size[0],
			m_data.size[1],
			0,
			_format<to_impl>(m_data.format.pixel),
			_type<to_impl>(m_data.format.type),
			data));
		
		set_repeated(m_data.flags & texture_flags_repeat);
		set_smooth(m_data.flags & texture_flags_smooth);
		set_mipmapped(m_data.flags & texture_flags_mipmap);
	}

	void opengl_texture2d::update(vec2i const & pos, vec2i const & size, addr_t data)
	{
		if (!m_locked) { return (void)debug::error("texture2d is not locked"); }

		if (m_handle && (m_data.size == size)) { return; }
		else { m_data.size = size; }

		revalue(); bind();

		ML_glCheck(glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			pos[0],
			pos[1],
			m_data.size[0],
			m_data.size[1],
			_format<to_impl>(m_data.format.color),
			_type<to_impl>(m_data.format.type),
			data));
		
		set_repeated(m_data.flags & texture_flags_repeat);
		set_smooth(m_data.flags & texture_flags_smooth);
		set_mipmapped(m_data.flags & texture_flags_mipmap);
	}

	void opengl_texture2d::set_mipmapped(bool value)
	{
		if (!m_locked) { return (void)debug::error("texture2d is not locked"); }

		ML_flag_write(m_data.flags, texture_flags_mipmap, value);

		bool const smooth{ ML_flag_read(m_data.flags, texture_flags_smooth) };

		if (smooth) { ML_glCheck(glGenerateMipmap(GL_TEXTURE_2D)); }

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			value
			? smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR
			: smooth ? GL_LINEAR : GL_NEAREST));
	}

	void opengl_texture2d::set_repeated(bool value)
	{
		if (!m_locked) { return (void)debug::error("texture2d is not locked"); }

		ML_flag_write(m_data.flags, texture_flags_repeat, value);

		static bool const edge_clamp_available
		{
			get_device()->get_info().texture_edge_clamp_available
		};

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			value
			? GL_REPEAT
			: edge_clamp_available ? GL_CLAMP_TO_EDGE : GL_CLAMP));

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			value
			? GL_REPEAT
			: edge_clamp_available ? GL_CLAMP_TO_EDGE : GL_CLAMP));
	}

	void opengl_texture2d::set_smooth(bool value)
	{
		if (!m_locked) { return (void)debug::error("texture2d is not locked"); }

		ML_flag_write(m_data.flags, texture_flags_smooth, value);

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			value ? GL_LINEAR : GL_NEAREST));

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			ML_flag_read(m_data.flags, texture_flags_mipmap)
			? value ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR
			: value ? GL_LINEAR : GL_NEAREST));
	}

	bitmap opengl_texture2d::copy_to_image() const
	{
		if (!m_locked) { debug::error("texture2d is not locked"); return bitmap{}; }

		bitmap temp{ m_data.size, calc_bits_per_pixel(m_data.format.color) };
		if (m_handle)
		{
			bind();

			ML_glCheck(glGetTexImage(GL_TEXTURE_2D, 0,
				_format<to_impl>(m_data.format.color),
				_type<to_impl>(m_data.format.type),
				temp.data()));

			unbind();
		}
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texturecube
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_texturecube::opengl_texturecube(render_device * dev, desc_<texturecube> const & value)
		: texturecube{ dev }, m_data{ value }
	{
		ML_glCheck(glGenTextures(1, &m_handle));
	}

	opengl_texturecube::~opengl_texturecube()
	{
		ML_glCheck(glDeleteTextures(1, &m_handle));
	}

	bool opengl_texturecube::revalue()
	{
		if (!m_locked) { return debug::error("texturecube is not locked"); }

		if (m_handle) { ML_glCheck(glDeleteTextures(1, &m_handle)); }
		
		ML_glCheck(glGenTextures(1, &m_handle));
		
		return (bool)m_handle;
	}

	void opengl_texturecube::lock()
	{
		m_locked = true;

		debug::warning("texture lock/unlock NYI");
	}

	void opengl_texturecube::unlock()
	{
		m_locked = false;

		debug::warning("texture lock/unlock NYI");
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// framebuffer
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_framebuffer::opengl_framebuffer(render_device * dev, desc_<framebuffer> const & value)
		: framebuffer{ dev }, m_data{ value }
	{
		resize(m_data.size);
	}

	opengl_framebuffer::~opengl_framebuffer()
	{
		ML_glCheck(glDeleteFramebuffers(1, &m_handle));
	}

	bool opengl_framebuffer::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteFramebuffers(1, &m_handle)); }
		
		ML_glCheck(glGenFramebuffers(1, &m_handle));
		
		m_attachments.clear(); m_depth.reset();

		return (bool)m_handle;
	}

	bool opengl_framebuffer::attach(shared<texture2d> const & value)
	{
		static auto const max_color_attachments
		{
			(size_t)get_device()->get_info().max_color_attachments
		};
		
		if (m_attachments.size() < max_color_attachments &&
			!std::binary_search(m_attachments.begin(), m_attachments.end(), value)
		)
		{
			m_attachments.push_back(value);
			return true;
		}
		return false;
	}

	bool opengl_framebuffer::detach(shared<texture2d> const & value)
	{
		if (auto const it{ std::find(m_attachments.begin(), m_attachments.end(), value) }
		; it != m_attachments.end())
		{
			m_attachments.erase(it);

			return true;
		}
		return false;
	}

	void opengl_framebuffer::resize(vec2i const & size)
	{
		if (m_handle && (m_data.size == size)) { return; }
		else { m_data.size = size; }

		revalue(); bind();

		// color attachments
		if (m_attachments.empty())
		{
			m_attachments.push_back(get_device()->create_texture2d({
				m_data.name,
				fs::path{},
				m_data.size,
				m_data.format,
				m_data.flags
				}));
		}
		for (size_t i = 0, imax = m_attachments.size(); i < imax; ++i)
		{
			m_attachments[i]->update(m_data.size);

			ML_glCheck(glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + (uint32_t)i,
				GL_TEXTURE_2D,
				ML_handle(uint32_t, m_attachments[i]->get_handle()),
				0));
		}

		// depth attachment
		if (m_depth)
		{
			m_depth->update(m_data.size);
		}
		else
		{
			m_depth = get_device()->create_texture2d({
				m_data.name,
				fs::path{},
				m_data.size,
				{ format_depth24_stencil8, format_depth_stencil, type_uint_24_8 },
				m_data.flags
				});
		}
		ML_glCheck(glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D,
			ML_handle(uint32_t, m_depth->get_handle()),
			0));

		// check status
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			debug::error("framebuffer is not complete");
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// program
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_program::opengl_uniform_binder::opengl_uniform_binder(opengl_program & p, cstring name) noexcept
	{
		if (!name || !*name || !(self = p.m_handle)) { return; }
		
		ML_glCheck(last = ML_glGetProgram());

		if (self != last) { ML_glCheck(ML_glUseProgram(self)); }

		location = p.m_uniforms.find_or_add_fn(util::hash(name, util::strlen(name)), [&
		]() noexcept
		{
			int32_t temp{};
			ML_glCheck(temp = ML_glGetUniformLocation(self, name));
			return ML_handle(uniform_id, temp);
		});
	}

	opengl_program::opengl_uniform_binder::~opengl_uniform_binder() noexcept
	{
		if (self && (self != last))
		{
			ML_glCheck(glUseProgramObjectARB(last));
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_program::opengl_program(render_device * dev)
		: program{ dev }
	{
		ML_glCheck(m_handle = ML_glCreateProgram());
	}

	opengl_program::~opengl_program()
	{
		ML_glCheck(ML_glDeleteProgram(m_handle));
	}

	bool opengl_program::revalue()
	{
		if (m_handle) { ML_glCheck(ML_glDeleteProgram(m_handle)); }

		m_uniforms.clear(); m_textures.clear(); m_shaders.clear();
		
		ML_glCheck(m_handle = ML_glCreateProgram());
		
		return (bool)m_handle;
	}

	bool opengl_program::attach(uint32_t type, size_t count, cstring * str, int32_t const * len)
	{
		if (!count || !str || !*str || m_shaders.contains(type)) { return false; }

		// create shader
		uint32_t temp{};
		ML_glCheck(temp = ML_glCreateShader(_shader_type<to_impl>(type)));

		// compile shader
		ML_glCheck(ML_glShaderSource(temp, (uint32_t)count, str, len));
		ML_glCheck(ML_glCompileShader(temp));

		// check compile errors
		int32_t success{};
		ML_glCheck(ML_glGetShaderCompileStatus(temp, &success));
		if (!success)
		{
			gl_get_program_info_log(m_handle, m_error_log);
			
			ML_glCheck(ML_glDeleteShader(temp));
		}
		else
		{
			// attach shader
			ML_glCheck(ML_glAttachShader(m_handle, temp));
			m_shaders.insert(type, ML_handle(object_id, temp));
			m_source[type] = { str, str + count };
		}
		return success;
	}

	bool opengl_program::detach(uint32_t type)
	{
		if (auto const it{ m_shaders.find(type) })
		{
			// detach shader
			ML_glCheck(ML_glDetachShader(m_handle, ML_handle(uint32_t, *it->second)));

			// delete shader
			ML_glCheck(ML_glDeleteShader(ML_handle(uint32_t, *it->second)));

			m_shaders.erase(it->first);

			m_source[type].clear();

			return true;
		}
		return false;
	}

	bool opengl_program::link()
	{
		// link
		ML_glCheck(ML_glLinkProgram(m_handle));

		// check linker errors
		int32_t success{};
		ML_glCheck(ML_glGetProgramLinkStatus(m_handle, &success));
		if (!success)
		{
			gl_get_program_info_log(m_handle, m_error_log);
		}
		return success;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// shader
namespace ml::gfx
{
	opengl_shader::uniform_binder::uniform_binder(opengl_shader & s, cstring name) noexcept
	{
		if (!name || !*name || !(self = s.m_handle)) { return; }

		ML_glCheck(last = ML_glGetProgram());

		if (self != last) { ML_glCheck(ML_glUseProgram(self)); }

		loc = s.m_uniforms.find_or_add_fn(util::hash(name, util::strlen(name)), [&
		]() noexcept
		{
			int32_t temp{};
			ML_glCheck(temp = ML_glGetUniformLocation(self, name));
			return ML_handle(uniform_id, temp);
		});
	}

	opengl_shader::uniform_binder::~uniform_binder() noexcept
	{
		if (self && (self != last))
		{
			ML_glCheck(ML_glUseProgram(last));
		}
	}

	opengl_shader::opengl_shader(render_device * dev, desc_<shader> const & value)
		: shader{ dev }
	{
		cstring str{ value.code.front().data() };
		compile(value.type, value.code.size(), &str);
	}

	opengl_shader::~opengl_shader()
	{
		ML_glCheck(ML_glDeleteProgram(m_handle));
	}

	bool opengl_shader::compile(uint32_t type, size_t count, cstring * str, int32_t const * len)
	{
		if (!count || !str || !*str) { return false; }

		if (m_handle)
		{
			ML_glCheck(ML_glDeleteProgram(m_handle));
			m_uniforms.clear(); m_textures.clear();
		}

		m_data.type = type;
		m_data.code = { str, str + count };

		uint32_t temp{};
		ML_glCheck(temp = ML_glCreateShader(_shader_type<to_impl>(type)));
		if (temp)
		{
			ML_glCheck(ML_glShaderSource(temp, (uint32_t)count, str, len));
			ML_glCheck(ML_glCompileShader(temp));

			ML_glCheck(m_handle = ML_glCreateProgram());
			if (m_handle)
			{
				int32_t compiled{};
				ML_glGetShaderCompileStatus(temp, &compiled);
				ML_glCheck(glProgramParameteri(m_handle, GL_PROGRAM_SEPARABLE, true));
				if (m_log.clear(); compiled)
				{
					ML_glCheck(ML_glAttachShader(m_handle, temp));
					ML_glCheck(ML_glLinkProgram(m_handle));
					ML_glCheck(ML_glDetachShader(m_handle, temp));
				}
				else
				{
					gl_get_program_info_log(temp, m_log);
				}
			}

			ML_glCheck(ML_glDeleteShader(temp));
			return m_handle;
		}
		else
		{
			return false;
		}
	}

	void opengl_shader::do_upload(uniform_id loc, bool value)
	{
		ML_glCheck(glProgramUniform1i(m_handle, ML_handle(int32_t, loc), (int32_t)value));
	}

	void opengl_shader::do_upload(uniform_id loc, int32_t value)
	{
		ML_glCheck(glProgramUniform1i(m_handle, ML_handle(int32_t, loc), value));
	}

	void opengl_shader::do_upload(uniform_id loc, uint32_t value)
	{
		ML_glCheck(glProgramUniform1i(m_handle, ML_handle(int32_t, loc), (int32_t)value));
	}

	void opengl_shader::do_upload(uniform_id loc, float_t value)
	{
		ML_glCheck(glProgramUniform1f(m_handle, ML_handle(int32_t, loc), value));
	}

	void opengl_shader::do_upload(uniform_id loc, vec2f const & value)
	{
		ML_glCheck(glProgramUniform2f(m_handle, ML_handle(int32_t, loc), value[0], value[1]));
	}

	void opengl_shader::do_upload(uniform_id loc, vec3f const & value)
	{
		ML_glCheck(glProgramUniform3f(m_handle, ML_handle(int32_t, loc), value[0], value[1], value[2]));
	}

	void opengl_shader::do_upload(uniform_id loc, vec4f const & value)
	{
		ML_glCheck(glProgramUniform4f(m_handle, ML_handle(int32_t, loc), value[0], value[1], value[2], value[3]));
	}

	void opengl_shader::do_upload(uniform_id loc, mat2f const & value, bool transpose)
	{
		ML_glCheck(glProgramUniformMatrix2fv(m_handle, ML_handle(int32_t, loc), 1, transpose, value));
	}

	void opengl_shader::do_upload(uniform_id loc, mat3f const & value, bool transpose)
	{
		ML_glCheck(glProgramUniformMatrix3fv(m_handle, ML_handle(int32_t, loc), 1, transpose, value));
	}

	void opengl_shader::do_upload(uniform_id loc, mat4f const & value, bool transpose)
	{
		ML_glCheck(glProgramUniformMatrix4fv(m_handle, ML_handle(int32_t, loc), 1, transpose, value));
	}
	
	void opengl_shader::do_upload(uniform_id loc, shared<texture> const & value, uint32_t slot)
	{
		get_context()->bind_texture(value.get(), slot);

		ML_glCheck(glProgramUniform1i(m_handle, ML_handle(int32_t, loc), (int32_t)slot));
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // ML_IMPL_RENDERER_OPENGL