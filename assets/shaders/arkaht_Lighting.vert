#version 450 core

in vec4 position;

out VS_OUT
{
    vec4 color;
} vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main()
{
    gl_Position = proj_matrix * mv_matrix * position;
    vs_out.color = gl_Position;
}
