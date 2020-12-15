#ifndef _ML_OPENGL_HPP_
#define _ML_OPENGL_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/graphics/RenderAPI.hpp>

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

// get error
#define ML_glGetError() _ML gfx::_error<_ML gfx::to_user>( glGetError() )

// enable error checking
#if ML_is_debug && !defined(ML_IMPL_GFX_CHECK)
#define ML_IMPL_GFX_CHECK
#endif

// check error
#ifdef ML_IMPL_GFX_CHECK
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
#define ML_glUniformMatrix2fv(loc, transpose, ptr)	glUniformMatrix2fvARB( loc, 1, transpose, ptr )
#define ML_glUniformMatrix3fv(loc, transpose, ptr)	glUniformMatrix3fvARB( loc, 1, transpose, ptr )
#define ML_glUniformMatrix4fv(loc, transpose, ptr)	glUniformMatrix4fvARB( loc, 1, transpose, ptr )

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//  enums
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _action(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _error(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _equation(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _facet(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _factor(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _format(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _order(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _predicate(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _primitive(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _shader_type(uint32 value) noexcept
	{
		if constexpr (convt{} == to_impl{})
		{
			switch (value)
			{
			default						: return value;
			case shader_type_vertex		: return GL_VERTEX_SHADER;
			case shader_type_pixel	: return GL_FRAGMENT_SHADER;
			case shader_type_geometry	: return GL_GEOMETRY_SHADER;
			}
		}
		else if constexpr (convt{} == to_user{})
		{
			switch (value)
			{
			default					: return value;
			case GL_VERTEX_SHADER	: return shader_type_vertex;
			case GL_FRAGMENT_SHADER	: return shader_type_pixel;
			case GL_GEOMETRY_SHADER	: return shader_type_geometry;
			}
		}
		else
		{
			static_assert(0, "invalid enum conversion");
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _texture_type(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _type(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class convt> constexpr uint32 _usage(uint32 value) noexcept
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
			return static_cast<uint32>(-1);
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// utility
namespace ml::gfx
{
	// check error
	inline void gl_check_error(cstring expr, cstring file, size_t line) noexcept
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
	inline void gl_get_program_info_log(uint32 obj, ds::string & buf) noexcept
	{
		int32 max_len{};
		ML_glCheck(glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &max_len));
		buf.resize((size_t)max_len);
		ML_glCheck(glGetProgramInfoLog(obj, max_len, &max_len, buf.data()));
		buf.push_back(0);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_OPENGL_HPP_