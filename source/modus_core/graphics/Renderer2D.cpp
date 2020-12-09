#include <modus_core/graphics/Renderer2D.hpp>

// VERTEX SHADER
constexpr auto g_vertex_shader = R"(#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texcoord;
layout(location = 3) in float a_texture_index;
layout(location = 2) in float a_tiling_factor;

out vertex {
	vec3 position;
	vec4 color;
	vec2 texcoord;
	float texture_index;
	float tiling_factor;
} V;

uniform mat4 u_view;
uniform mat4 u_proj;

void main()
{
	V.position = a_position;
	V.color = a_color;
	V.texcoord = a_texcoord;
	V.texture_index = a_texture_index;
	V.tiling_factor = a_tiling_factor;
	gl_Position = (u_view * u_proj) * vec4(V.position, 1.0);
}
)";

// FRAGMENT SHADER
constexpr auto g_fragment_shader = R"(#version 460 core
out vec4 gl_Color;

in vertex {
	vec3  position;
	vec4  color;
	vec2  texcoord;
	float texture_index;
	float tiling_factor;
} V;

uniform sampler2D u_texture;

void main()
{
	gl_Color = V.color * texture(u_texture, V.texcoord);
}
)";

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static struct ML_NODISCARD renderer2d_data final
	{
		renderer2d::metrics metrics{}; // 

		gfx::render_device *		device	{}; // 
		ds::ref<gfx::vertexarray>	va		{}; // 
		ds::ref<gfx::vertexbuffer>	vb		{}; // 
		ds::ref<gfx::program>		shader	{}; // 
		ds::ref<gfx::texture2d>		texture	{}; // 

		uint32 texture_index{}; // 
		ds::array<ds::ref<gfx::texture>, 32> texture_slots{}; // 
		
		quad_vertex *				buffer_base		{}; // 
		quad_vertex *				buffer_ptr		{}; // 
		uint32						index_count	{}; // 
		ds::array<quad_vertex, 4>	vertex_positions{}; // 
	}
	g_data{};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool renderer2d::initialize()
	{
		ML_assert(g_data.device = get_global<gfx::render_device>());

		g_data.va = gfx::vertexarray::create({ gfx::primitive_triangles });
		g_data.va->set_layout(gfx::buffer_layout{
			{ vec3{}	, "a_position"		},
			{ vec4{}	, "a_color"			},
			{ vec2{}	, "a_texcoord"		},
			{ float32{}	, "a_texture_index"	},
			{ float32{}	, "a_tiling_factor"	},
			});

		g_data.vb = gfx::vertexbuffer::create({ gfx::usage_static, 0, nullptr });

		bitmap img{ 512, 512, 4 };
		for (byte & b : img) { b = 255; }
		g_data.texture = gfx::texture2d::create(img, gfx::texture_flags_default);
		if (!g_data.texture) { return false; }

		g_data.shader = gfx::program::create({});
		g_data.shader->attach(gfx::shader_type_vertex, g_vertex_shader);
		g_data.shader->attach(gfx::shader_type_fragment, g_fragment_shader);
		if (!g_data.shader->link()) { return debug::failure(g_data.shader->get_error_log()); }

		return true;
	}

	void renderer2d::finalize()
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	renderer2d::metrics const & renderer2d::get_metrics() noexcept { return g_data.metrics; }

	void renderer2d::reset_metrics() noexcept { g_data.metrics = metrics{}; }

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void renderer2d::begin_scene(camera const & cam)
	{
		g_data.shader->bind();
		g_data.shader->set_uniform("u_proj", cam.get_proj_matrix());
		g_data.shader->set_uniform("u_view", cam.get_view_matrix());
		g_data.shader->set_uniform("u_texture", g_data.texture);

		g_data.index_count		= 0;
		g_data.buffer_ptr		= g_data.buffer_base;
		g_data.texture_index	= 1;
	}

	void renderer2d::end_scene()
	{
		g_data.device->get_active_context()->flush();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void renderer2d::draw_quad(float32 x, float32 y, float32 w, float32 h, float32 rotation)
	{
	}

	void renderer2d::draw_quad(vec2 const & pos, vec2 const & size, float32 rotation)
	{
	}

	void renderer2d::draw_quad(float_rect const & bounds, float32 rotation)
	{
	}

	void renderer2d::draw_quad(mat4 const & m)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}