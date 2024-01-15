#version 450 core

layout(std430, binding = 3) buffer layout_Positions
{
    vec4 Positions[];
};
layout(std430, binding = 4) buffer layout_Velocities
{
    vec4 Velocities[];
};
layout(std430, binding = 5) buffer layout_Colors
{
    vec4 Colors[];
};

layout( local_size_x = 100, local_size_y = 1, local_size_z = 1 ) in;

uniform float dt;
uniform float acceleration_force;
uniform vec3 low_speed_color;
uniform vec3 high_speed_color;
uniform float high_speed_max;
uniform vec3 blackhole1;
uniform vec3 blackhole2;
uniform vec3 blackhole_color;
uniform float blackhole_color_dist_max;
uniform int in_sphere_enabled = 1;
uniform float in_sphere_radius = 50.0;
uniform float in_sphere_bounciness = 0.0; 

vec3 accelerate_towards( vec3 pos, vec3 target, out float dist )
{
    vec3 dir = target - pos;
    dist = length( dir );

    return acceleration_force * dir / ( dist * dist );
}

void main()
{
    uint gid = gl_GlobalInvocationID.x;

    vec3 position = Positions[gid].xyz;
    vec3 velocity = Velocities[gid].xyz;

    //  calculate accelerations towards blackholes
    float dist_to_blackhole1, dist_to_blackhole2;
    vec3 acceleration = accelerate_towards( position, blackhole1, dist_to_blackhole1 );
    acceleration += accelerate_towards( position, blackhole2, dist_to_blackhole2 );

    //  calculte new position & velocity
    vec3 new_position = position + velocity * dt + 0.5 * dt * dt * acceleration;
    vec3 new_velocity = velocity + acceleration * dt;
    float new_velocity_length = length( new_velocity );

    //  apply in-sphere effect
    if ( in_sphere_enabled == 1 && length( new_position ) > in_sphere_radius )
    {
        new_position = normalize( new_position ) * in_sphere_radius;
        //new_velocity = -new_velocity * in_sphere_bounciness;  //  reverse velocity
        new_velocity = new_velocity_length * normalize( -new_position ) * in_sphere_bounciness;  //  bounce towards center (better results)
    }

    //  apply new position & velocity
    Positions[gid].xyz = new_position;
    Velocities[gid].xyz = new_velocity;

    //  find new color
    vec3 color = vec3( 1.0 );
    color = mix( low_speed_color, high_speed_color, min( new_velocity_length / high_speed_max, 1.0 ) );  //  colorize depending on velocity
    color = mix( blackhole_color, color, min( dist_to_blackhole1 / blackhole_color_dist_max + dist_to_blackhole2 / blackhole_color_dist_max, 1.0 ) );  //  colorize depending on closeness to blackholes
    Colors[gid].xyz = color;
}
