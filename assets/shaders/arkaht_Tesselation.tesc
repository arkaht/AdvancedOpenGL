#version 450 core

layout (vertices = 3) out;

uniform vec2 mouse;
vec4 mouse_position = vec4( mouse, 0.0, 0.0 );

const int MIN_LEVEL = 2;
const int MAX_LEVEL = 8;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1.2;

void main(void)
{
    if ( gl_InvocationID == 0 ) 
    {
        vec2 center0 = gl_in[0].gl_Position.xy + ( gl_in[2].gl_Position.xy - gl_in[0].gl_Position.xy ) / 2.0;
        vec2 center1 = gl_in[1].gl_Position.xy + ( gl_in[0].gl_Position.xy - gl_in[1].gl_Position.xy ) / 2.0;
        vec2 center2 = gl_in[2].gl_Position.xy + ( gl_in[1].gl_Position.xy - gl_in[2].gl_Position.xy ) / 2.0;

        float dist0 = length( mouse - center0 );
        float dist1 = length( mouse - center1 );
        float dist2 = length( mouse - center2 );

        int tes0 = int( mix( MAX_LEVEL, MIN_LEVEL, clamp( dist0 / MAX_DIST, 0.0, 1.0 ) ) );
        int tes1 = int( mix( MAX_LEVEL, MIN_LEVEL, clamp( dist1 / MAX_DIST, 0.0, 1.0 ) ) );
        int tes2 = int( mix( MAX_LEVEL, MIN_LEVEL, clamp( dist2 / MAX_DIST, 0.0, 1.0 ) ) );

        gl_TessLevelOuter[0] = tes0;
        gl_TessLevelOuter[1] = tes1;
        gl_TessLevelOuter[2] = tes2;

        gl_TessLevelInner[0] = max( tes0, max( tes1, tes2 ) );
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
