#pragma shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_Texcoord;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjMatrix;

void main()
{
    v_Position = a_Position;
    v_Normal = a_Normal;
    v_Texcoord = a_Texcoord;
    gl_Position = (u_ProjMatrix * u_ViewMatrix * u_ModelMatrix) * vec4(v_Position, 1.0);
}

#pragma shader pixel
#version 460 core

out vec4 gl_Color;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_Texcoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture0;

void main()
{
    gl_Color = u_Color * texture(u_Texture0, v_Texcoord);
}