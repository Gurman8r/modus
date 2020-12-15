#pragma shader vertex
#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

out vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
} V;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

void main()
{
    V.position  = a_position;
    V.normal    = a_normal;
    V.texcoord  = a_texcoord;
    gl_Position = (u_proj * u_view * u_model) * vec4(V.position, 1.0);
}

#pragma shader pixel
#version 460 core

out vec4 gl_Color;

in vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
} V;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main()
{
    gl_Color = u_color * texture(u_texture, V.texcoord);
}