#version 450 core

in vec3 position;
in vec3 normal;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec4 color;
} vs_out;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    vec4 current_position = model * vec4( position, 1.0 );
    gl_Position = projection * current_position;
    
    vs_out.normal = normal;
    vs_out.position = vec3( current_position );
    vs_out.color = gl_Position;//vec4( position, 1.0 );
}
