#shader fragment
// Source: https://www.shadertoy.com/view/WdK3RD

#version 460 core

in Vertex { vec3 position; vec3 normal; vec2 texcoord; } V;

out vec4 gl_Color;

uniform vec4        u_mouse;        // Mouse State
uniform float       u_delta;        // Delta Time
uniform int         u_frame;        // Frame Index
uniform float       u_fps;          // Frame Rate
uniform float       u_time;         // Total Time
uniform vec4        u_color;        // Main Color
uniform sampler2D   u_texture0;     // Main Texture
uniform vec2        u_resolution;   // Display Size

#define S(a,b,t) smoothstep(a,b,t)

float DistLine(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p - a;
    vec2 ba = b - a;
    float t = clamp(dot(pa, ba) / dot(ba, ba), 0., 1.);
    return length(pa - ba * t);
}

float N21(vec2 p)
{
    p = fract(p*vec2(233.34, 851.73));
    p += dot(p, p + 23.45);
    return fract(p.x*p.y);
}

vec2 N22(vec2 p)
{
    float n = N21(p);
    return vec2(n, N21(p + n));
}

vec2 GetPos(vec2 id, vec2 offs)
{
    vec2 n = N22(id + offs)* u_time;

    return offs + sin(n)*.4;
}

float Line(vec2 p, vec2 a, vec2 b)
{
    float d = DistLine(p, a, b); // distance to the line segment
    float m = S(.03, .01, d); // cut out the line
    float d2 = length(a - b);
    m *= S(1.2, .8, d2)*.5 + S(.05, .03, abs(d2 - .75)); // make them not all visible
    return m;
}

float Layer(vec2 uv)
{
    float m = 0.;
    vec2 gv = fract(uv) - .5;
    vec2 id = floor(uv);

    vec2 p[9];
    int i = 0;

    for (float y = -1.; y <= 1.; y++)
    {
        for (float x = -1.; x <= 1.; x++)
        {
            p[i++] = GetPos(id, vec2(x, y));
        }
    }

    float t = u_time * 20.;
    for (int i = 0; i < 9; ++i)
    {
        m += Line(gv, p[4], p[i]);

        vec2 j = (p[i] - gv)*15.;
        float sparkle = 1. / dot(j, j);

        m += sparkle * (sin(t + fract(p[i].x)*10.)*.5 + .5);
    }
    m += Line(gv, p[1], p[3]);
    m += Line(gv, p[1], p[5]);
    m += Line(gv, p[5], p[7]);
    m += Line(gv, p[7], p[3]);

    return m;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = ((fragCoord - 0.5).xy * u_resolution) / u_resolution.y;
    vec2 mouse = (u_mouse.xy / u_resolution) * vec2(1, -1);
    float m = 0.;
    float t = u_time * .1;

    float s = sin(t);
    float c = cos(t);
    mat2 rot = mat2(c, -s, s, c);

    uv *= rot;
    mouse *= rot;

    for (float i = 0.; i < 1.; i += 1. / 4.)
    {
        float z = fract(i + t);
        float size = mix(10., .5, z);
        float fade = S(0., .5, z)*S(1., .8, z);
        m += Layer(uv*size + i * 20. - mouse)*fade;
    }

    vec3 col = vec3(m);

    fragColor = vec4(col, 1.0);
}

void main()
{
    mainImage(gl_Color, V.texcoord);
}