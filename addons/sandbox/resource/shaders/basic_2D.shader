#pragma shader vertex
#version 460 core

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

#pragma shader pixel
#version 460 core

out vec4 gl_Color;

in vertex {
    vec3 position;
    vec4 color;
    vec2 texcoord;
    float texture_index;
    float tiling_factor;
} V;

uniform sampler2D u_texture;

void main()
{
    gl_Color = V.color * texture(u_texture, V.texcoord);
}