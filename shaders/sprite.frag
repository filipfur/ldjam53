#version 330 core

layout (std140) uniform SceneBlock
{
    vec4 u_point_lights[10];
};

uniform sampler2D u_texture_0;
uniform vec4 u_color;
uniform float u_time;
uniform vec3 u_view_pos;

uniform vec2 u_region_dimension;
uniform vec2 u_region_position;
uniform int u_flipped;

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

void main()
{
    vec2 uv = texCoord;

    if(u_flipped != 0)
    {
        uv.x = 1.0 - uv.x;
    }

    /*calculate uv based on region dimension and region position*/
    uv = uv * u_region_dimension + u_region_position;

    vec4 color = texture2D(u_texture_0, uv) * u_color;

    fragColor = color;
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/2.2));
}