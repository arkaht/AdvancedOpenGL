#version 450 core

out vec4 color;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec4 color;
} fs_in;

uniform vec3 light_position;
uniform vec4 light_color;
uniform float light_ambient;
uniform float light_radius;
uniform int is_light;

void main()
{
    if ( is_light == 1 )
    {
        color = light_color;
        return;
    }

    vec3 light_dir = light_position - fs_in.position;
    float diffuse = max( 
        dot( 
            normalize( fs_in.normal ), 
            normalize( light_dir ) 
        ), 
        0.0 
    );

    //  radius fall off
    diffuse *= 1.0 - length( light_dir ) / light_radius;

    color = fs_in.color * light_color * ( light_ambient + diffuse );
} 
