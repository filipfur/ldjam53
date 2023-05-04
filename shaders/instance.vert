#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstance;

uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_model;
uniform float u_time;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;
out float instanceId;

void main()
{
    texCoord = aTexCoords;
    mat3 normalMatrix = transpose(inverse(mat3(aInstance)));
    instanceId = float(gl_InstanceID);
    normal = normalize(normalMatrix * aNormal);
    fragPos = vec3(aInstance * vec4(aPos, 1.0));
    gl_Position = u_projection * u_view * vec4(fragPos, 1.0);
}