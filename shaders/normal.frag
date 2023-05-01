#version 330 core

out vec4 fragColor;
in vec3 normal;

uniform vec3 u_face_normal;

void main()
{
    float dp = dot(normal, u_face_normal);
    if(dp * dp < 0.1)
    {
        discard;
    }
    fragColor = vec4(normal, 1.0);
}