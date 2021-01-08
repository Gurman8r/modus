#ifndef _ML_RENDER_ENUM_HPP_
#define _ML_RENDER_ENUM_HPP_

#include <modus_core/detail/FlatMap.hpp>
#include <modus_core/detail/Method.hpp>
#include <modus_core/graphics/Bitmap.hpp>

// types
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_handle(object_id); // object handle

	ML_decl_handle(uniform_id); // uniform location

	ML_alias addr_t = typename void const *; // data address
	
	ML_alias buffer_t = typename list<byte>; // byte buffer

	template <class ...> struct spec; // object specification

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// enums
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// internal enum conversion helpers

	struct to_impl : std::false_type {}; // frontend -> backend
	
	struct to_user : std::true_type {}; // backend -> frontend

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum action_ : uint32
	{
		action_keep,
		action_zero,
		action_replace,
		action_inc,
		action_inc_wrap,
		action_dec,
		action_dec_wrap,
		action_invert,
	};

	constexpr cstring action_NAMES[] =
	{
		"keep",
		"zero",
		"replace",
		"increment",
		"increment wrap",
		"decrement",
		"decrement wrap",
		"invert",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum error_ : uint32
	{
		error_none,
		error_invalid_enum,
		error_invalid_value,
		error_invalid_operation,
		error_stack_overflow,
		error_stack_underflow,
		error_out_of_memory,
		error_invalid_framebuffer_operation,
		error_context_lost,
	};

	constexpr cstring error_NAMES[] = // error names
	{
		"no error",
		"invalid enum",
		"invalid value",
		"invalid operation",
		"stack overflow",
		"stack underflow",
		"out of memory",
		"invalid framebuffer operation",
		"context lost",
	};

	constexpr cstring error_DESCS[] = // error descriptions
	{
		"no description",
		"an unacceptable value has been specified for an enumerated argument",
		"a numeric argument is out of range",
		"the specified operation is not allowed in the current state",
		"this command would cause a stack overflow",
		"this command would cause a stack underflow",
		"there is not enough memory left to execute the command",
		"attempting to read from or write/render to a framebuffer that is not complete",
		"the context has been lost due to a graphics card reset",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum equation_ : uint32
	{
		equation_add,
		equation_subtract,
		equation_reverse_subtract,
		equation_min,
		equation_max,
	};

	constexpr cstring equation_NAMES[] =
	{
		"add",
		"subtract",
		"reverse subtract",
		"min",
		"max",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum facet_ : uint32
	{
		facet_front_left,
		facet_front_right,
		facet_back_left,
		facet_back_right,
		facet_front,
		facet_back,
		facet_left,
		facet_right,
		facet_front_and_back,
	};

	constexpr cstring facet_NAMES[] =
	{
		"front left",
		"front right",
		"back left",
		"back right",
		"front",
		"back",
		"left",
		"right",
		"front and back",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum factor_ : uint32
	{
		factor_zero,
		factor_one,
		factor_src_color,
		factor_one_minus_src_color,
		factor_src_alpha,
		factor_one_minus_src_alpha,
		factor_dst_alpha,
		factor_one_minus_dst_alpha,
		factor_dst_color,
		factor_one_minus_dst_color,
		factor_src_alpha_saturate,
	};

	constexpr cstring factor_NAMES[] =
	{
		"zero",
		"one",
		"src color",
		"one minus src color",
		"src alpha",
		"one minus src alpha",
		"dst alpha",
		"one minus dst alpha",
		"dst color",
		"one minus dst color",
		"src alpha saturate",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum format_ : uint32
	{
		format_red,
		format_green,
		format_blue,
		format_alpha,
		format_rgb,
		format_rgba,
		format_luminance,
		format_luminance_alpha,

		format_srgb,
		format_srgb8,
		format_srgb_alpha,
		format_srgb8_alpha8,
		format_sluminance_alpha,
		format_sluminance8_alpha8,
		format_sluminance,
		format_sluminance8,

		format_depth_stencil,
		format_depth24_stencil8,
	};

	constexpr cstring format_NAMES[] =
	{
		"red",
		"green",
		"blue",
		"alpha",
		"rgb",
		"rgba",
		"luminance",
		"luminance alpha",

		"srgb",
		"srgb8",
		"srgb alpha",
		"srgb8 alpha8",
		"sluminance alpha",
		"sluminance8 alpha8",
		"sluminance",
		"sluminance8",

		"depth stencil",
		"depth24 stencil8",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum order_
	{
		order_cw,
		order_ccw,
	};

	constexpr cstring order_NAMES[] =
	{
		"cw",
		"ccw",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum predicate_ : uint32
	{
		predicate_never,
		predicate_less,
		predicate_equal,
		predicate_lequal,
		predicate_greater,
		predicate_notequal,
		predicate_gequal,
		predicate_always,
	};

	constexpr cstring predicate_NAMES[] =
	{
		"never",
		"less",
		"equal",
		"lequal",
		"greater",
		"not equal",
		"gequal",
		"always",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum primitive_ : uint32
	{
		primitive_points,
		primitive_lines,
		primitive_line_loop,
		primitive_line_strip,
		primitive_triangles,
		primitive_triangle_strip,
		primitive_triangle_fan,
		primitive_fill,
	};

	constexpr cstring primitive_NAMES[] =
	{
		"points",
		"lines",
		"line loop",
		"line strip",
		"triangles",
		"triangle strip",
		"triangle fan",
		"fill",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum shader_type_ : uint32
	{
		shader_type_vertex,
		shader_type_pixel,
		shader_type_geometry,

		shader_type_MAX,
	};

	constexpr cstring shader_type_NAMES[] =
	{
		"vertex",
		"pixel",
		"geometry",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// texture types
	enum texture_type_ : uint32
	{
		texture_type_1d,
		texture_type_2d,
		texture_type_3d,
		texture_type_cube,

		texture_type_MAX,
	};

	// texture type names
	constexpr cstring texture_type_NAMES[] =
	{
		"texture1d",
		"texture2d",
		"texture3d",
		"texturecube",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum type_ : uint32
	{
		type_byte,
		type_ubyte,
		type_short,
		type_ushort,
		type_int,
		type_uint,
		type_float,
		type_half_float,
		type_uint_24_8,
	};

	constexpr cstring type_NAMES[] =
	{
		"byte",
		"unsigned byte",
		"short",
		"unsigned short",
		"int",
		"unsigned int",
		"float",
		"half float",
		"unsigned int 24 8",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum usage_ : uint32
	{
		usage_stream,
		usage_static,
		usage_dynamic,
	};

	constexpr cstring usage_NAMES[] =
	{
		"stream",
		"static",
		"dynamic",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// util
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// buffer copy
	template <class Elem = byte
	> buffer_t bufcpy(size_t count, addr_t data, pmr::polymorphic_allocator<byte> alloc = {}) noexcept
	{
		return data
			? buffer_t{ (byte *)data, (byte *)data + count * sizeof(Elem), alloc }
			: buffer_t{ count * sizeof(Elem), (byte)0, alloc };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_NODISCARD constexpr uint32 calc_channel_format(size_t value) noexcept
	{
		switch (value)
		{
		default	: return 0;
		case 1	: return format_red;
		case 3	: return format_rgb;
		case 4	: return format_rgba;
		}
	}

	ML_NODISCARD constexpr size_t calc_bits_per_pixel(uint32 value) noexcept
	{
		switch (value)
		{
		default			: return 0;
		case format_red	: return 1;
		case format_rgb	: return 3;
		case format_rgba: return 4;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// get element base type
	ML_NODISCARD constexpr hash_t get_element_base_type(hash_t type) noexcept
	{
		switch (type)
		{
		default					: return 0;
		case hashof_v<bool>		: return hashof_v<bool>;
		case hashof_v<int32>	:
		case hashof_v<vec2i>	:
		case hashof_v<vec3i>	:
		case hashof_v<vec4i>	:
		case hashof_v<mat2i>	:
		case hashof_v<mat3i>	:
		case hashof_v<mat4i>	: return hashof_v<int32>;
		case hashof_v<float32>	:
		case hashof_v<vec2f>	:
		case hashof_v<vec3f>	:
		case hashof_v<vec4f>	:
		case hashof_v<mat2f>	:
		case hashof_v<mat3f>	:
		case hashof_v<mat4f>	: return hashof_v<float32>;
		}
	}

	template <class T> ML_NODISCARD constexpr hash_t get_element_base_type() noexcept
	{
		return _ML gfx::get_element_base_type(hashof_v<T>);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// get element component count
	ML_NODISCARD constexpr uint32 get_element_component_count(hash_t type) noexcept
	{
		switch (type)
		{
		default					: return 0;
		case hashof_v<bool>		:
		case hashof_v<int32>	:
		case hashof_v<float32>	: return 1;
		case hashof_v<vec2i>	:
		case hashof_v<vec2f>	: return 2;
		case hashof_v<vec3i>	:
		case hashof_v<vec3f>	: return 3;
		case hashof_v<vec4i>	:
		case hashof_v<vec4f>	: return 4;
		case hashof_v<mat2i>	:
		case hashof_v<mat2f>	: return 2 * 2;
		case hashof_v<mat3i>	:
		case hashof_v<mat3f>	: return 3 * 3;
		case hashof_v<mat4i>	:
		case hashof_v<mat4f>	: return 4 * 4;
		}
	}

	template <class T> ML_NODISCARD constexpr uint32 get_element_component_count() noexcept
	{
		return _ML gfx::get_element_component_count(hashof_v<T>);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// layout
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// buffer layout element
	struct ML_NODISCARD buffer_element final
	{
		template <class T
		> static constexpr bool is_valid_type
		{
			util::is_any_of_v<T,
				bool,
				int32, vec2i, vec3i, vec4i, mat2i, mat3i, mat4i,
				float32, vec2f, vec3f, vec4f, mat2f, mat3f, mat4f
			>
		};

		string	name		{};
		hash_t		type		{};
		uint32		size		{};
		bool		normalized	{};
		uint32		offset		{};

		buffer_element(string const & name, hash_t type, uint32 size, bool normalized) noexcept
			: name{ name }, type{ type }, size{ size }, normalized{ normalized }, offset{}
		{
		}

		template <class Elem
		> buffer_element(Elem, cstring name, bool normalized = false) noexcept
			: buffer_element{ name, hashof_v<Elem>, sizeof(Elem), normalized }
		{
			static_assert(is_valid_type<Elem>);
		}

		ML_NODISCARD hash_t get_base_type() const noexcept
		{
			return _ML gfx::get_element_base_type(type);
		}

		ML_NODISCARD uint32 get_component_count() const noexcept
		{
			return _ML gfx::get_element_component_count(type);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// buffer layout
	struct ML_NODISCARD buffer_layout final
	{
		using storage_type				= typename list<buffer_element>;
		using iterator					= typename storage_type::iterator;
		using const_iterator			= typename storage_type::const_iterator;
		using reverse_iterator			= typename storage_type::reverse_iterator;
		using const_reverse_iterator	= typename storage_type::const_reverse_iterator;

		template <class It
		> buffer_layout(It first, It last) noexcept
			: m_elements{ first, last }
		{
			uint32 offset{};
			for (auto & e : m_elements)
			{
				e.offset = offset;
				offset += e.size;
				m_stride += e.size;
			}
		}

		buffer_layout(std::initializer_list<buffer_element> init) noexcept
			: buffer_layout{ init.begin(), init.end() }
		{
		}

		template <size_t N
		> buffer_layout(const buffer_element(&arr)[N]) noexcept
			: buffer_layout{ &arr[0], &arr[N] }
		{
		}

		buffer_layout() noexcept : buffer_layout{ {
			{ vec3{}, "a_position"	},
			{ vec3{}, "a_normal"	},
			{ vec2{}, "a_texcoord"	},
		} }
		{
		}

		ML_NODISCARD auto elements() const noexcept -> storage_type const & { return m_elements; }

		ML_NODISCARD auto stride() const noexcept -> uint32 { return m_stride; }

	private:
		uint32			m_stride	{}; // stride
		storage_type	m_elements	{}; // elements
	};
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

// states
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// clear flags
	enum clear_flags_ : uint32
	{
		clear_flags_none		= 0,		// no buffer
		clear_flags_color		= 1 << 0,	// color buffer bit
		clear_flags_depth		= 1 << 1,	// depth buffer bit
		clear_flags_stencil		= 1 << 2,	// stencil buffer bit

		// color / depth / stencil
		clear_flags_all
			= clear_flags_color
			| clear_flags_depth
			| clear_flags_stencil,
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// alpha state
	struct ML_NODISCARD alpha_state final
	{
		bool		enabled		{ true }						; // enable alpha test
		uint32		pred		{ predicate_greater }			; // alpha test predicate
		float32		ref			{ 0.001f }						; // alpha test reference
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// blend state
	struct ML_NODISCARD blend_state final
	{
		bool		enabled		{ true }						; // enable blend
		color		color		{ colors::white }				; // blend color
		uint32
				color_equation	{ equation_add }				, // color equation
				color_sfactor	{ factor_src_alpha }			, // color src factor
				color_dfactor	{ factor_one_minus_src_alpha }	, // color dst factor
				alpha_equation	{ equation_add }				, // alpha equation
				alpha_sfactor	{ factor_src_alpha }			, // alpha src factor
				alpha_dfactor	{ factor_one_minus_src_alpha }	; // alpha dst factor
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// cull state
	struct ML_NODISCARD cull_state final
	{
		bool		enabled	{ true }						; // enable face culling
		uint32		facet	{ facet_back }					; // front / back / front&back
		uint32		order	{ order_ccw }					; // cull order
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// depth state
	struct ML_NODISCARD depth_state final
	{
		bool		enabled	{ true }						; // enable depth test
		uint32		pred	{ predicate_less }				; // depth test predicate
		vec2		range	{ 0.f, 1.f }					; // depth test range
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// stencil state
	struct ML_NODISCARD stencil_state final
	{
		bool		enabled		{ true }					; // enable stencil test
		uint32		front_pred	{ predicate_always }		; // front test predicate
		int32		front_ref	{ 0 }						; // front test reference
		uint32		front_mask	{ 0xffffffff }				; // front test mask
		uint32		back_pred	{ predicate_always }		; // back test predicate
		int32		back_ref	{ 0 }						; // back test reference
		uint32		back_mask	{ 0xffffffff }				; // back test mask
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_RENDER_ENUM_HPP_