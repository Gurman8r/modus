#include <modus_core/graphics/Canvas.hpp>

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

// PIXEL SHADER
constexpr auto g_pixel_shader = R"(#version 460 core
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

	bool canvas::initialize(gfx::render_device * device)
	{
		m_va = gfx::vertexarray::create({ gfx::primitive_triangles });
		m_va->set_layout({
			{ vec3{}	, "a_position"		},
			{ vec4{}	, "a_color"			},
			{ vec2{}	, "a_texcoord"		},
			{ float32{}	, "a_texture_index"	},
			{ float32{}	, "a_tiling_factor"	},
			});

		m_vb = gfx::vertexbuffer::create({ gfx::usage_static, 0, nullptr });

		bitmap img{ 512, 512, 4 };
		for (byte & b : img) { b = 255; }
		m_texture = gfx::texture2d::create(img, gfx::texture_flags_default);
		if (!m_texture) { return false; }

		m_shader = gfx::program::create({});
		m_shader->attach(gfx::shader_type_vertex, g_vertex_shader);
		m_shader->attach(gfx::shader_type_fragment, g_pixel_shader);
		if (!m_shader->link()) { return debug::failure(m_shader->get_error_log()); }

		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void canvas::draw_quad(float32 x, float32 y, float32 w, float32 h, float32 rotation)
	{
	}

	void canvas::draw_quad(vec2 const & pos, vec2 const & size, float32 rotation)
	{
	}

	void canvas::draw_quad(float_rect const & bounds, float32 rotation)
	{
	}

	void canvas::draw_quad(mat4 const & m)
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}