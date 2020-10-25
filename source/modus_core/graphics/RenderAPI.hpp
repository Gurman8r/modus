#ifndef _ML_RENDER_API_HPP_
#define _ML_RENDER_API_HPP_

#include <modus_core/detail/Method.hpp>
#include <modus_core/detail/Map.hpp>
#include <modus_core/graphics/Bitmap.hpp>
#include <modus_core/window/WindowContext.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// types
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	class	render_device	; // 
	class	render_context	; // 
	class	vertexarray		; // 
	class	vertexbuffer	; // 
	class	indexbuffer		; // 
	class	texture			; // 
	class	texture2d		; // 
	class	texturecube		; // 
	class	framebuffer		; // 
	class	program			; // 
	class	shader			; // WIP

	ML_decl_handle(	object_id	)	; // object handle
	ML_decl_handle(	uniform_id	)	; // register handle

	template <class ...> struct spec; // object specification

	ML_alias addr_t		= typename void const *			; // data address
	ML_alias buffer_t	= typename pmr::vector<byte_t>	; // byte buffer

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// enums
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// internal conversion helpers

	struct to_impl : std::false_type	{}; // frontend -> backend
	
	struct to_user : std::true_type		{}; // backend -> frontend

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum action_ : uint32_t
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

	enum error_ : uint32_t
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

	enum equation_ : uint32_t
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

	enum facet_ : uint32_t
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

	enum factor_ : uint32_t
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

	enum format_ : uint32_t
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

	enum predicate_ : uint32_t
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

	enum primitive_ : uint32_t
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

	enum shader_type_ : uint32_t
	{
		shader_vertex,
		shader_fragment,
		shader_geometry,

		shader_MAX,
	};

	constexpr cstring shader_type_NAMES[] =
	{
		"vertex shader",
		"fragment shader",
		"geometry shader",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// texture types
	enum texture_type_ : uint32_t
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
		"texture 1d",
		"texture 2d",
		"texture 3d",
		"texture cube map",
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	enum type_ : uint32_t
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

	enum usage_ : uint32_t
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// util
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// buffer copy
	template <class Elem = byte_t
	> buffer_t bufcpy(size_t count, addr_t data, buffer_t::allocator_type alloc = {}) noexcept
	{
		return data
			? buffer_t{ (byte_t *)data, (byte_t *)data + count * sizeof(Elem), alloc }
			: buffer_t{ count * sizeof(Elem), (byte_t)0, alloc };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_NODISCARD constexpr uint32_t calc_channel_format(size_t value) noexcept
	{
		switch (value)
		{
		default	: return 0;
		case 1	: return format_red;
		case 3	: return format_rgb;
		case 4	: return format_rgba;
		}
	}

	ML_NODISCARD constexpr size_t calc_bits_per_pixel(uint32_t value) noexcept
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
		case hashof_v<int32_t>	:
		case hashof_v<vec2i>	:
		case hashof_v<vec3i>	:
		case hashof_v<vec4i>	:
		case hashof_v<mat2i>	:
		case hashof_v<mat3i>	:
		case hashof_v<mat4i>	: return hashof_v<int32_t>;
		case hashof_v<float_t>	:
		case hashof_v<vec2f>	:
		case hashof_v<vec3f>	:
		case hashof_v<vec4f>	:
		case hashof_v<mat2f>	:
		case hashof_v<mat3f>	:
		case hashof_v<mat4f>	: return hashof_v<float_t>;
		}
	}

	template <class T> ML_NODISCARD constexpr hash_t get_element_base_type() noexcept
	{
		return _ML gfx::get_element_base_type(hashof_v<T>);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// get element component count
	ML_NODISCARD constexpr uint32_t get_element_component_count(hash_t type) noexcept
	{
		switch (type)
		{
		default					: return 0;
		case hashof_v<bool>		:
		case hashof_v<int32_t>	:
		case hashof_v<float_t>	: return 1;
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

	template <class T> ML_NODISCARD constexpr uint32_t get_element_component_count() noexcept
	{
		return _ML gfx::get_element_component_count(hashof_v<T>);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// layout
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// layout element
	struct ML_NODISCARD vertex_element final
	{
		template <class T
		> static constexpr bool is_valid_type
		{
			util::is_any_of_v<T,
				bool,
				int32_t, vec2i, vec3i, vec4i, mat2i, mat3i, mat4i,
				float_t, vec2f, vec3f, vec4f, mat2f, mat3f, mat4f
			>
		};

		cstring		name		{};
		hash_t		type		{};
		uint32_t	size		{};
		bool		normalized	{};
		uint32_t	offset		{};

		constexpr vertex_element(cstring name, hash_t type, uint32_t size, bool normalized) noexcept
			: name{ name }, type{ type }, size{ size }, normalized{ normalized }, offset{}
		{
		}

		template <class Elem
		> constexpr vertex_element(Elem, cstring name, bool normalized = false) noexcept
			: vertex_element{ name, hashof_v<Elem>, sizeof(Elem), normalized }
		{
			static_assert(is_valid_type<Elem>);
		}

		constexpr hash_t get_base_type() const noexcept
		{
			return _ML gfx::get_element_base_type(type);
		}

		constexpr uint32_t get_component_count() const noexcept
		{
			return _ML gfx::get_element_component_count(type);
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// vertex layout
	struct ML_NODISCARD vertex_layout final
	{
		using storage_type				= typename pmr::vector<vertex_element>;
		using iterator					= typename storage_type::iterator;
		using const_iterator			= typename storage_type::const_iterator;
		using reverse_iterator			= typename storage_type::reverse_iterator;
		using const_reverse_iterator	= typename storage_type::const_reverse_iterator;

		static constexpr vertex_element default_3d[] =
		{
			{ vec3{}, "a_position"	},
			{ vec3{}, "a_normal"	},
			{ vec2{}, "a_texcoord"	},
		};

		template <class It
		> vertex_layout(It first, It last) noexcept
			: m_elements{ first, last }
		{
			uint32_t offset{};
			for (auto & e : m_elements)
			{
				e.offset = offset;
				offset += e.size;
				m_stride += e.size;
			}
		}

		vertex_layout(std::initializer_list<vertex_element> init) noexcept
			: vertex_layout{ init.begin(), init.end() }
		{
		}

		template <size_t N
		> vertex_layout(const vertex_element(&arr)[N]) noexcept
			: vertex_layout{ &arr[0], &arr[N] }
		{
		}

		vertex_layout() noexcept : vertex_layout{ default_3d }
		{
		}

		ML_NODISCARD auto elements() const noexcept -> storage_type const & { return m_elements; }

		ML_NODISCARD auto stride() const noexcept -> uint32_t { return m_stride; }

	private:
		uint32_t		m_stride	{}; // stride
		storage_type	m_elements	{}; // elements
	};
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render device
namespace ml::gfx
{
	// device data_desc settings
	struct ML_NODISCARD device_properties final
	{
		// version
		ds::string renderer, vendor, version;
		int32_t major_version, minor_version;
		pmr::vector<ds::string> extensions;

		// textures
		bool texture_edge_clamp_available;
		uint32_t max_texture_slots;

		// framebuffers
		uint32_t max_color_attachments;
		uint32_t max_samples;

		// shaders
		bool shaders_available;
		bool geometry_shaders_available;
		ds::string shading_language_version;
	};


	ML_NODISCARD ML_CORE_API render_device * get_default_device() noexcept;

	ML_CORE_API render_device * set_default_device(render_device * value) noexcept;

	// base device
	class ML_CORE_API render_device : public non_copyable, public trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		virtual ~render_device() override = default;

		ML_NODISCARD static render_device * create(int32_t api, allocator_type alloc = {}) noexcept;

		static void destroy(render_device * value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual shared<render_context> const & get_context() const noexcept = 0;

		virtual void set_context(shared<render_context> const & value) noexcept = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual object_id get_handle() const noexcept = 0;

		ML_NODISCARD virtual device_properties const & get_info() const noexcept = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual shared<render_context> create_context(
			spec<render_context> const & desc,
			allocator_type alloc = {}
		) noexcept = 0;

		ML_NODISCARD virtual shared<vertexarray> create_vertexarray(
			spec<vertexarray> const & desc,
			allocator_type alloc = {}
		) noexcept = 0;

		ML_NODISCARD virtual shared<vertexbuffer> create_vertexbuffer(
			spec<vertexbuffer> const & desc,
			addr_t data,
			allocator_type alloc = {}
		) noexcept = 0;

		ML_NODISCARD virtual shared<indexbuffer> create_indexbuffer(
			spec<indexbuffer> const & desc,
			addr_t data,
			allocator_type alloc = {}
		) noexcept = 0;

		ML_NODISCARD virtual shared<texture2d> create_texture2d(
			spec<texture2d> const & desc,
			addr_t data = {},
			allocator_type alloc = {}
		) noexcept = 0;

		ML_NODISCARD virtual shared<texturecube> create_texturecube(
			spec<texturecube> const & desc,
			allocator_type alloc = {}
		) noexcept = 0;

		ML_NODISCARD virtual shared<framebuffer> create_framebuffer(
			spec<framebuffer> const & desc,
			allocator_type alloc = {}
		) noexcept = 0;

		ML_NODISCARD virtual shared<program> create_program(
			allocator_type alloc = {}
		) noexcept = 0;

		ML_NODISCARD virtual shared<shader> create_shader(
			spec<shader> const & desc,
			allocator_type alloc = {}
		) noexcept = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render object
namespace ml::gfx
{
	// base render object
	template <class Derived
	> class render_object : public non_copyable, public trackable
	{
	private:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr typeof<> s_base_type{ typeof_v<Derived> };

		render_device * const m_parent;

	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		explicit render_object(render_device * parent) noexcept : m_parent{ parent }
		{
			ML_assert_msg(m_parent, "BAD RENDER DEVICE");
		}

		virtual ~render_object() override = default;

		ML_NODISCARD virtual object_id get_handle() const noexcept = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD inline auto get_base_type() const noexcept -> typeof<> const & { return s_base_type; }

		ML_NODISCARD inline auto get_device() const noexcept -> render_device * { return m_parent; }

		ML_NODISCARD inline auto get_context() const noexcept -> shared<render_context> const & { return ML_check(m_parent)->get_context(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render states
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// clear mask
	enum clear_ : uint32_t
	{
		clear_none		= 0,		// no buffer
		clear_color		= 1 << 0,	// color buffer bit
		clear_depth		= 1 << 1,	// depth buffer bit
		clear_stencil	= 1 << 2,	// stencil buffer bit

		// color / depth / stencil
		clear_all
			= clear_color
			| clear_depth
			| clear_stencil,
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// alpha state
	struct ML_NODISCARD alpha_state final
	{
		bool		enabled		{ true }						; // enable alpha test
		uint32_t	pred		{ predicate_greater }			; // alpha test predicate
		float_t		ref			{ 0.001f }						; // alpha test reference
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// blend state
	struct ML_NODISCARD blend_state final
	{
		bool		enabled		{ true }						; // enable blend
		color		color		{ colors::white }				; // blend color
		uint32_t
				color_equation	{ equation_add }				, // color equation
				color_sfactor	{ factor_src_alpha }			, // color src factor
				color_dfactor	{ factor_one_minus_src_alpha }	, // color dst factor
				alpha_equation	{ color_equation }				, // alpha equation
				alpha_sfactor	{ color_sfactor }				, // alpha src factor
				alpha_dfactor	{ color_dfactor }				; // alpha dst factor
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// cull state
	struct ML_NODISCARD cull_state final
	{
		bool			enabled	{ true }						; // enable face culling
		uint32_t		facet	{ facet_back }					; // front / back / front&back
		uint32_t		order	{ order_ccw }					; // cull order
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// depth state
	struct ML_NODISCARD depth_state final
	{
		bool			enabled	{ true }						; // enable depth test
		uint32_t		pred	{ predicate_less }				; // depth test predicate
		vec2			range	{ 0.f, 1.f }					; // depth test range
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// stencil state
	struct ML_NODISCARD stencil_state final
	{
		bool			enabled	{ true }						; // enable stencil test
		struct
		{
			uint32_t	pred	{ predicate_always }			; // stencil test predicate
			int32_t		ref		{ 0 }							; // stencil test reference
			uint32_t	mask	{ 0xffffffff }					; // stencil test mask
		}
		front, back{ front };
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render context
namespace ml::gfx
{
	// render context settings
	template <> struct ML_NODISCARD spec<render_context> final
	{
		int32_t		api				{ context_api_unknown };
		int32_t		major			{},
					minor			{};
		int32_t		profile			{ context_profile_any };
		int32_t		depth_bits		{};
		int32_t		stencil_bits	{};
		bool		multisample		{};
		bool		srgb_capable	{};
	};

	static void from_json(json const & j, spec<render_context> & v)
	{
		j["api"			].get_to((context_api_ & )v.api);
		j["major"		].get_to(v.major);
		j["minor"		].get_to(v.minor);
		j["profile"		].get_to((context_profile_ &)v.profile);
		j["depth_bits"	].get_to(v.depth_bits);
		j["stencil_bits"].get_to(v.stencil_bits);
		j["multisample"	].get_to(v.multisample);
		j["srgb_capable"].get_to(v.srgb_capable);
	}

	static void to_json(json & j, spec<render_context> const & v)
	{
		j["api"			] = (context_api_)v.api;
		j["major"		] = v.major;
		j["minor"		] = v.minor;
		j["profile"		] = (context_profile_)v.profile;
		j["depth_bits"	] = v.depth_bits;
		j["stencil_bits"] = v.stencil_bits;
		j["multisample"	] = v.multisample;
		j["srgb_capable"] = v.srgb_capable;
	}


	// base render context
	class ML_CORE_API render_context : public render_object<render_context>
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using spec_type = typename spec<render_context>;

		ML_NODISCARD static auto create(spec_type const & desc, allocator_type alloc = {}) noexcept
		{
			return get_default_device()->create_context(desc, alloc);
		}

	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit render_context(render_device * parent) noexcept : render_object{ parent }
		{
		}

		virtual ~render_context() override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual spec<render_context> const & get_settings() const noexcept = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual alpha_state * get_alpha_state(alpha_state * value = {}) const = 0;
		
		virtual blend_state * get_blend_state(blend_state * value = {}) const = 0;

		virtual color get_clear_color() const = 0;

		virtual cull_state * get_cull_state(cull_state * value = {}) const = 0;

		virtual depth_state * get_depth_state(depth_state * value = {}) const = 0;

		virtual stencil_state * get_stencil_state(stencil_state * value = {}) const = 0;

		virtual int_rect * get_viewport(int_rect * value = {}) const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void set_alpha_state(alpha_state const & value) = 0;
		
		virtual void set_blend_state(blend_state const & value) = 0;

		virtual void set_clear_color(color const & value) = 0;

		virtual void set_cull_state(cull_state const & value) = 0;

		virtual void set_depth_state(depth_state const & value) = 0;

		virtual void set_stencil_state(stencil_state const & value) = 0;

		virtual void set_viewport(int_rect const & value) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void clear(uint32_t mask) = 0;

		virtual void draw(shared<vertexarray> const & value) = 0;

		virtual void draw_arrays(uint32_t prim, size_t first, size_t count) = 0;

		virtual void draw_indexed(uint32_t prim, size_t count) = 0;

		virtual void flush() = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void bind_vertexarray(vertexarray const * value) = 0;

		virtual void bind_vertexbuffer(vertexbuffer const * value) = 0;

		virtual void bind_indexbuffer(indexbuffer const * value) = 0;

		virtual void bind_texture(texture const * value, uint32_t slot = 0) = 0;

		virtual void bind_framebuffer(framebuffer const * value) = 0;

		virtual void bind_program(program const * value) = 0;

		virtual void bind_shader(shader const * value) = 0; // WIP

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void upload(uniform_id loc, bool value) = 0;

		virtual void upload(uniform_id loc, int32_t value) = 0;

		virtual void upload(uniform_id loc, float_t value) = 0;

		virtual void upload(uniform_id loc, vec2f const & value) = 0;

		virtual void upload(uniform_id loc, vec3f const & value) = 0;

		virtual void upload(uniform_id loc, vec4f const & value) = 0;

		virtual void upload(uniform_id loc, mat2f const & value) = 0;

		virtual void upload(uniform_id loc, mat3f const & value) = 0;

		virtual void upload(uniform_id loc, mat4f const & value) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// vertexarray
namespace ml::gfx
{
	// vertexarray settings
	template <> struct ML_NODISCARD spec<vertexarray> final
	{
		uint32_t	prim{ primitive_triangles };
	};

	static void from_json(json const & j, spec<vertexarray> & v)
	{
		j["prim"].get_to(v.prim);
	}

	static void to_json(json & j, spec<vertexarray> const & v)
	{
		j["prim"] = v.prim;
	}


	// base vertexarray
	class ML_CORE_API vertexarray : public render_object<vertexarray>
	{
	public:
		ML_NODISCARD static auto create(spec<vertexarray> const & desc = {}, allocator_type alloc = {}) noexcept
		{
			return get_default_device()->create_vertexarray(desc, alloc);
		}

	public:
		explicit vertexarray(render_device * parent) noexcept : render_object{ parent }
		{
		}

		virtual ~vertexarray() override = default;

		virtual bool revalue() = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual void add_vertices(shared<vertexbuffer> const & value) = 0;

		virtual void set_layout(vertex_layout const & value) = 0;

		virtual void set_indices(shared<indexbuffer> const & value) = 0;

		ML_NODISCARD virtual vertex_layout const & get_layout() const noexcept = 0;

		ML_NODISCARD virtual shared<indexbuffer> const & get_indices() const noexcept = 0;

		ML_NODISCARD virtual uint32_t get_mode() const noexcept = 0;
		
		ML_NODISCARD virtual pmr::vector<shared<vertexbuffer>> const & get_vertices() const noexcept = 0;

	public:
		inline void bind() const noexcept
		{
			get_context()->bind_vertexarray(this);
		}

		inline void unbind() const noexcept
		{
			get_context()->bind_vertexarray(nullptr);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// vertexbuffer
namespace ml::gfx
{
	// vertexbuffer settings
	template <> struct ML_NODISCARD spec<vertexbuffer> final
	{
		uint32_t	usage	{ usage_static };
		size_t		count	{};
	};

	static void from_json(json const & j, spec<vertexbuffer> & v)
	{
		j["usage"].get_to(v.usage);
	}

	static void to_json(json & j, spec<vertexbuffer> const & v)
	{
		j["usage"] = v.usage;
	}


	// base vertexbuffer
	class ML_CORE_API vertexbuffer : public render_object<vertexbuffer>
	{
	public:
		ML_NODISCARD static auto create(spec<vertexbuffer> const & desc = {}, addr_t data = {}, allocator_type alloc = {}) noexcept
		{
			return get_default_device()->create_vertexbuffer(desc, data, alloc);
		}

	public:
		explicit vertexbuffer(render_device * parent) noexcept : render_object{ parent }
		{
		}

		virtual ~vertexbuffer() override = default;

		virtual bool revalue() = 0;

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual void set_data(size_t count, addr_t data, size_t offset = 0) = 0;

		ML_NODISCARD virtual buffer_t const & get_buffer() const noexcept = 0;

		ML_NODISCARD virtual size_t get_count() const noexcept = 0;

		ML_NODISCARD virtual uint32_t get_usage() const noexcept = 0;

	public:
		inline void bind() const noexcept
		{
			get_context()->bind_vertexbuffer(this);
		}

		inline void unbind() const noexcept
		{
			get_context()->bind_vertexbuffer(nullptr);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// indexbuffer
namespace ml::gfx
{
	// indexbuffer settings
	template <> struct ML_NODISCARD spec<indexbuffer> final
	{
		uint32_t	usage	{ usage_static };
		size_t		count	{};
	};

	static void from_json(json const & j, spec<indexbuffer> & v)
	{
		j["usage"].get_to(v.usage);
		j["count"].get_to(v.count);
	}

	static void to_json(json & j, spec<indexbuffer> const & v)
	{
		j["usage"] = v.usage;
		j["count"] = v.count;
	}


	// base indexbuffer
	class ML_CORE_API indexbuffer : public render_object<indexbuffer>
	{
	public:
		ML_NODISCARD static auto create(spec<indexbuffer> const & desc = {}, addr_t data = {}, allocator_type alloc = {}) noexcept
		{
			return get_default_device()->create_indexbuffer(desc, data, alloc);
		}

	public:
		explicit indexbuffer(render_device * parent) noexcept : render_object{ parent }
		{
		}

		virtual ~indexbuffer() override = default;

		virtual bool revalue() = 0;

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual void set_data(size_t count, addr_t data, size_t offset = 0) = 0;

		ML_NODISCARD virtual buffer_t const & get_buffer() const noexcept = 0;

		ML_NODISCARD virtual size_t get_count() const noexcept = 0;

		ML_NODISCARD virtual uint32_t get_usage() const noexcept = 0;

	public:
		inline void bind() const noexcept
		{
			get_context()->bind_indexbuffer(this);
		}

		inline void unbind() const noexcept
		{
			get_context()->bind_indexbuffer(nullptr);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texture
namespace ml::gfx
{
	// texture flags
	enum texture_flags_ : int32_t
	{
		texture_flags_none		= 0 << 0, // none
		texture_flags_smooth	= 1 << 0, // smooth
		texture_flags_repeat	= 1 << 1, // repeat
		texture_flags_mipmap	= 1 << 2, // mipmap

		// smooth / repeat
		texture_flags_default
			= texture_flags_smooth
			| texture_flags_repeat,
	};

	ML_NODISCARD constexpr auto & operator&=(texture_flags_ & lhs, int32_t const rhs) noexcept
	{
		return (texture_flags_ &)((int32_t &)lhs &= rhs);
	}

	ML_NODISCARD constexpr auto & operator|=(texture_flags_ & lhs, int32_t const rhs) noexcept
	{
		return (texture_flags_ &)((int32_t &)lhs |= rhs);
	}

	static void from_json(json const & j, texture_flags_ & v)
	{
		ML_flag_write((int32_t &)v, texture_flags_repeat, j["repeat"].get<bool>());
		ML_flag_write((int32_t &)v, texture_flags_smooth, j["smooth"].get<bool>());
		ML_flag_write((int32_t &)v, texture_flags_mipmap, j["mipmap"].get<bool>());
	}

	static void to_json(json & j, texture_flags_ const & v)
	{
		j["repeat"] = ML_flag_read(v, texture_flags_repeat);
		j["smooth"] = ML_flag_read(v, texture_flags_smooth);
		j["mipmap"] = ML_flag_read(v, texture_flags_mipmap);
	}


	// texture format
	struct ML_NODISCARD texture_format final
	{
		uint32_t
			color	{ format_rgba },
			pixel	{ color },
			type	{ type_ubyte };
	};

	static void from_json(json const & j, texture_format & v)
	{
		j["color"	].get_to(v.color);
		j["pixel"	].get_to(v.pixel);
		j["type"	].get_to(v.type);
	}

	static void to_json(json & j, texture_format const & v)
	{
		j["color"	] = v.color;
		j["pixel"	] = v.pixel;
		j["type"	] = v.type;
	}

	// base texture
	class ML_CORE_API texture : public render_object<texture>
	{
	public:
		explicit texture(render_device * parent) noexcept : render_object{ parent }
		{
		}

		virtual ~texture() override = default;

		virtual bool revalue() = 0;

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual void lock() = 0;

		virtual void unlock() = 0;

		ML_NODISCARD virtual uint32_t get_type() const noexcept = 0;

		ML_NODISCARD virtual vec2i const & get_size() const noexcept = 0;
		
		ML_NODISCARD virtual texture_format const & get_format() const noexcept = 0;
		
		ML_NODISCARD virtual texture_flags_ get_flags() const noexcept = 0;

	public:
		inline void bind(uint32_t slot = 0) const noexcept
		{
			get_context()->bind_texture(this, slot);
		}

		inline void unbind(uint32_t slot = 0) const noexcept
		{
			get_context()->bind_texture(nullptr, slot);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texture2d
namespace ml::gfx
{
	// texture2d settings
	template <> struct ML_NODISCARD spec<texture2d> final
	{
		vec2i			size	{};
		texture_format	format	{ format_rgba };
		texture_flags_	flags	{ texture_flags_default };
	};

	static void from_json(json const & j, spec<texture2d> & v)
	{
		j["size"	].get_to(v.size);
		j["format"	].get_to(v.format);
		j["flags"	].get_to(v.flags);
	}

	static void to_json(json & j, spec<texture2d> const & v)
	{
		j["size"	] = v.size;
		j["format"	] = v.format;
		j["flags"	] = v.flags;
	}


	// base texture2d
	class ML_CORE_API texture2d : public texture
	{
	public:
		ML_NODISCARD static auto create(spec<texture2d> const & desc, addr_t data = {}, allocator_type alloc = {}) noexcept
		{
			return get_default_device()->create_texture2d(desc, data);
		}

		ML_NODISCARD static auto create(bitmap const & img, texture_flags_ flags = texture_flags_default) noexcept
		{
			return create({ img.size(), { calc_channel_format(img.channels()) }, flags }, img.data());
		}

		ML_NODISCARD static auto create(fs::path const & path, texture_flags_ flags = texture_flags_default) noexcept
		{
			return create(bitmap{ path }, flags);
		}

	public:
		explicit texture2d(render_device * parent) noexcept : texture{ parent }
		{
		}

		virtual ~texture2d() override = default;

		virtual bool revalue() = 0;

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual void lock() override = 0;

		virtual void unlock() override = 0;

		virtual void update(vec2i const & size, addr_t data = {}) = 0;

		virtual void update(vec2i const & pos, vec2i const & size, addr_t data) = 0;

		virtual void set_mipmapped(bool value) = 0;

		virtual void set_repeated(bool value) = 0;

		virtual void set_smooth(bool value) = 0;

		ML_NODISCARD virtual bitmap copy_to_image() const = 0;

		ML_NODISCARD inline uint32_t get_type() const noexcept { return texture_type_2d; }

		ML_NODISCARD virtual vec2i const & get_size() const noexcept = 0;

		ML_NODISCARD virtual texture_format const & get_format() const noexcept = 0;

		ML_NODISCARD virtual texture_flags_ get_flags() const noexcept = 0;
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texturecube
namespace ml::gfx
{
	// texturecube settings
	template <> struct ML_NODISCARD spec<texturecube> final
	{
		vec2i			size	{};
		texture_format	format	{ format_rgba };
		texture_flags_	flags	{ texture_flags_default };
	};

	static void from_json(json const & j, spec<texturecube> & v)
	{
		j["size"	].get_to(v.size);
		j["format"	].get_to(v.format);
		j["flags"	].get_to(v.flags);
		
	}

	static void to_json(json & j, spec<texturecube> const & v)
	{
		j["size"	] = v.size;
		j["format"	] = v.format;
		j["flags"	] = v.flags;
	}


	// base texturecube
	class ML_CORE_API texturecube : public texture
	{
	public:
		ML_NODISCARD static auto create(spec<texturecube> const & desc, allocator_type alloc = {}) noexcept
		{
			return get_default_device()->create_texturecube(desc, alloc);
		}

	public:
		explicit texturecube(render_device * parent) noexcept : texture{ parent }
		{
		}

		virtual ~texturecube() override = default;

		virtual bool revalue() = 0;

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual void lock() override = 0;

		virtual void unlock() override = 0;

		ML_NODISCARD inline uint32_t get_type() const noexcept override { return texture_type_cube; }

		ML_NODISCARD virtual vec2i const & get_size() const noexcept = 0;

		ML_NODISCARD virtual texture_format const & get_format() const noexcept = 0;

		ML_NODISCARD virtual texture_flags_ get_flags() const noexcept = 0;
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// framebuffer
namespace ml::gfx
{
	// framebuffer settings
	template <> struct ML_NODISCARD spec<framebuffer> final
	{
		vec2i			size			{};
		texture_format	format			{ format_rgba };
		texture_flags_	flags			{ texture_flags_default };
		vec4i			bits_per_pixel	{ 8, 8, 8, 8 };
		int32_t			stencil_bits	{ 24 },
						depth_bits		{ 8 };
		int32_t			samples			{};
		bool			stereo			{};
	};

	static void from_json(json const & j, spec<framebuffer> & v)
	{
		j["size"			].get_to(v.size);
		j["format"			].get_to(v.format);
		j["flags"			].get_to(v.flags);
		j["bits_per_pixel"	].get_to(v.bits_per_pixel);
		j["stencil_bits"	].get_to(v.stencil_bits);
		j["depth_bits"		].get_to(v.depth_bits);
		j["samples"			].get_to(v.samples);
		j["stereo"			].get_to(v.stereo);
	}

	static void to_json(json & j, spec<framebuffer> const & v)
	{
		j["size"			] = v.size;
		j["format"			] = v.format;
		j["flags"			] = v.flags;
		j["bits_per_pixel"	] = v.bits_per_pixel;
		j["stencil_bits"	] = v.stencil_bits;
		j["depth_bits"		] = v.depth_bits;
		j["samples"			] = v.samples;
		j["stereo"			] = v.stereo;
	}


	// base framebuffer
	class ML_CORE_API framebuffer : public render_object<framebuffer>
	{
	public:
		ML_NODISCARD static auto create(spec<framebuffer> const & desc, allocator_type alloc = {}) noexcept
		{
			return get_default_device()->create_framebuffer(desc, alloc);
		}

	public:
		explicit framebuffer(render_device * parent) noexcept : render_object{ parent }
		{
		}

		virtual ~framebuffer() override = default;

		virtual bool revalue() = 0;

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual bool attach(shared<texture2d> const & value) = 0;

		virtual bool detach(shared<texture2d> const & value) = 0;

		virtual void resize(vec2i const & value) = 0;

		ML_NODISCARD virtual pmr::vector<shared<texture2d>> const & get_color_attachments() const noexcept = 0;

		ML_NODISCARD virtual shared<texture2d> const & get_depth_attachment() const noexcept = 0;

		ML_NODISCARD virtual vec2i const & get_size() const noexcept = 0;

		ML_NODISCARD virtual texture_format const & get_format() const noexcept = 0;

		ML_NODISCARD virtual texture_flags_ get_flags() const noexcept = 0;

		ML_NODISCARD virtual vec4i const & get_bits_per_pixel() const noexcept = 0;

		ML_NODISCARD virtual int32_t get_stencil_bits() const noexcept = 0;
		
		ML_NODISCARD virtual int32_t get_depth_bits() const noexcept = 0;
		
		ML_NODISCARD virtual int32_t get_samples() const noexcept = 0;
		
		ML_NODISCARD virtual bool is_stereo() const noexcept = 0;


	public:
		inline void bind() const noexcept
		{
			get_context()->bind_framebuffer(this);
		}

		inline void unbind() const noexcept
		{
			get_context()->bind_framebuffer(nullptr);
		}

		inline void bind_texture(uint32_t slot = 0) const noexcept
		{
			get_context()->bind_texture
			(
				get_color_attachments()[(size_t)slot].get(), slot
			);
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// program
namespace ml::gfx
{
	// base program
	class ML_CORE_API program : public render_object<program>
	{
	public:
		ML_NODISCARD static auto create() noexcept
		{
			return get_default_device()->create_program();
		}

	public:
		explicit program(render_device * parent) noexcept : render_object{ parent }
		{
		}

		virtual ~program() override = default;

		virtual bool revalue() = 0;

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual bool attach(uint32_t type, size_t count, cstring * str, int32_t const * len = {}) = 0;

		inline bool attach(uint32_t type, ds::string const & str) noexcept
		{
			if (str.empty()) { return false; }
			cstring temp{ str.c_str() };
			return attach(type, 1, &temp, nullptr);
		}

		inline bool attach(uint32_t type, pmr::vector<ds::string> const & str) noexcept
		{
			if (str.empty()) { return false; }
			cstring temp{ str.front().c_str() };
			return attach(type, str.size(), &temp, nullptr);
		}

		virtual bool detach(uint32_t type) = 0;

		ML_NODISCARD virtual bool link() = 0;

		virtual bool bind_uniform(cstring name, ds::method<void(uniform_id)> const & fn) = 0;

		ML_NODISCARD virtual ds::string const & get_error_log() const noexcept = 0;

		ML_NODISCARD virtual ds::map<uint32_t, object_id> const & get_shaders() const noexcept = 0;

		ML_NODISCARD virtual ds::map<uint32_t, pmr::vector<ds::string>> const & get_source() const noexcept = 0;

		ML_NODISCARD virtual ds::map<uniform_id, shared<texture>> const & get_textures() const noexcept = 0;

		ML_NODISCARD virtual ds::map<hash_t, uniform_id> const & get_uniforms() const noexcept = 0;

	public:
		inline void bind() const noexcept
		{
			get_context()->bind_program(this);
		}

		inline void unbind() const noexcept
		{
			get_context()->bind_program(nullptr);
		}

		inline void bind_textures() const noexcept
		{
			uint32_t slot{};
			get_textures().for_each([&, &ctx = get_device()->get_context()
			](uniform_id loc, shared<texture> const & tex) noexcept
			{
				ctx->bind_texture(tex.get(), slot);

				ctx->upload(loc, (int32_t)slot++);
			});
		}

		template <class T> bool set_uniform(cstring name, T && value) noexcept
		{
			return bind_uniform(name, [&](uniform_id loc) noexcept
			{
				if constexpr (std::is_convertible_v<T, shared<texture>>)
				{
					do_cache_texture(loc, ML_forward(value));
				}
				else
				{
					get_context()->upload(loc, ML_forward(value));
				}
			});
		}

	protected:
		virtual void do_cache_texture(uniform_id loc, shared<texture> const & value) noexcept = 0;
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// shader
namespace ml::gfx
{
	// shader settings
	template <> struct ML_NODISCARD spec<shader> final
	{
		using source_t = pmr::vector<ds::string>;

		uint32_t	type	{ shader_vertex };
		source_t	code	{};
	};

	static void from_json(json const & j, spec<shader> & v)
	{
		j["type"].get_to(v.type);
		j["code"].get_to(v.code);
	}

	static void to_json(json & j, spec<shader> const & v)
	{
		j["type"] = v.type;
		j["code"] = v.code;
	}


	// base shader
	class ML_CORE_API shader : public render_object<shader>
	{
	public:
		ML_NODISCARD static auto create(spec<shader> const & desc, allocator_type alloc = {}) noexcept
		{
			return get_default_device()->create_shader(desc, alloc);
		}

	public:
		explicit shader(render_device * parent) noexcept : render_object{ parent }
		{
		}

		virtual ~shader() override = default;

		ML_NODISCARD virtual object_id get_handle() const noexcept override = 0;

		ML_NODISCARD virtual typeof<> const & get_self_type() const noexcept override = 0;

	public:
		virtual bool compile(uint32_t type, size_t count, cstring * str, int32_t const * len = {}) = 0;

		virtual bool bind_uniform(cstring name, ds::method<void(uniform_id)> const & fn) = 0;

		ML_NODISCARD virtual ds::string const & get_info_log() const noexcept = 0;

		ML_NODISCARD virtual pmr::vector<ds::string> const & get_source() const noexcept = 0;

		ML_NODISCARD virtual ds::map<uniform_id, shared<texture>> const & get_textures() const noexcept = 0;

		ML_NODISCARD virtual uint32_t get_type() const noexcept = 0;

	public:
		inline void bind() const noexcept
		{
			get_context()->bind_shader(this);
		}

		inline void unbind() const noexcept
		{
			get_context()->bind_shader(nullptr);
		}

		inline void bind_textures() noexcept
		{
			uint32_t slot{};
			get_textures().for_each([&](uniform_id loc, shared<texture> const & tex) noexcept
			{
				do_upload(loc, tex, slot++);
			});
		}

		template <class T, class ... Extra
		> bool set_uniform(cstring name, T && value, Extra && ... extra) noexcept
		{
			return bind_uniform(name, [&](uniform_id loc) noexcept
			{
				if constexpr (std::is_convertible_v<T, shared<texture>>)
				{
					do_cache(loc, ML_forward(value));
				}
				else
				{
					do_upload(ML_forward(value), ML_forward(extra)...);
				}
			});
		}

	protected:
		virtual void do_cache(uniform_id loc, shared<texture> const & value) = 0;

		virtual void do_upload(uniform_id loc, bool value) = 0;

		virtual void do_upload(uniform_id loc, int32_t value) = 0;

		virtual void do_upload(uniform_id loc, uint32_t value) = 0;

		virtual void do_upload(uniform_id loc, float_t value) = 0;

		virtual void do_upload(uniform_id loc, vec2f const & value) = 0;

		virtual void do_upload(uniform_id loc, vec3f const & value) = 0;

		virtual void do_upload(uniform_id loc, vec4f const & value) = 0;

		virtual void do_upload(uniform_id loc, mat2f const & value, bool transpose = false) = 0;

		virtual void do_upload(uniform_id loc, mat3f const & value, bool transpose = false) = 0;

		virtual void do_upload(uniform_id loc, mat4f const & value, bool transpose = false) = 0;

		virtual void do_upload(uniform_id loc, shared<texture> const & value, uint32_t slot = 0) = 0;
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_RENDER_API_HPP_