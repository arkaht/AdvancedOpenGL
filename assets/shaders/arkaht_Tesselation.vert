#version 450 core

in vec3 vertex;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4( vertex, 1.0 );
}
