#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 8) out;

in VS_OUT {
    vec2 texCoord;
    vec3 normal;
    vec3 fragPos;
    vec3 up;
    vec3 right;
    vec3 rxu;
} gs_in[];

/*in vec2 gs_texCoord[];
in vec3 gs_normal[];
in vec3 gs_fragPos[];*/

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 u_projection;
uniform float u_time;

uniform float u_alpha;
uniform float u_halfSideLength;

uniform vec3 u_creaseDir;
uniform vec3 u_normal;
uniform int u_normalTobendDirFactor;
uniform int u_creaseDirPlaneDim;
uniform int u_creaseDirPlaneSign;

void createVertex(vec3 offset, vec2 uv)
{
    texCoord = uv;
    normal = gs_in[0].normal;
    fragPos = gs_in[0].fragPos;
    vec3 displacement = offset.x * gs_in[0].right + offset.y * gs_in[0].up + offset.z * gs_in[0].rxu;
    displacement.y += 1.0;
    gl_Position = u_projection * (gl_in[0].gl_Position
        + vec4(displacement, 0.0)); 
    EmitVertex();
}

void main()
{
    /*for(int i = 0; i < 3; i++)
    {
        int index = i % 3;
        texCoord = gs_in[0].texCoord;
        normal = gs_in[0].normal;
        fragPos = gs_in[0].fragPos;
        vec4 offset = vec4(0.0);
        if(i == 0)
        {
            offset = vec4(gs_in[0].up * 1.0, 0.0);
        }
        else
        {
            offset = vec4(gs_in[0].right * 1.0 * (i - 1.5), 0.0);
        }
        gl_Position = u_projection * (gl_in[0].gl_Position + offset); 
        EmitVertex();
    }*/

    float a = u_alpha;
    vec3 creaseDirRotated90 = cross(u_normal, u_creaseDir);
    vec3 bendDir = u_normalTobendDirFactor * u_normal;

    vec2 creaseDirPlaneVec = vec2(0.0);
    vec2 creaseDirPlaneVecRotated90 = vec2(0.0);
    creaseDirPlaneVec[u_creaseDirPlaneDim] = u_creaseDirPlaneSign;
    creaseDirPlaneVecRotated90[1 - u_creaseDirPlaneDim] = u_creaseDirPlaneDim == 0 ? u_creaseDirPlaneSign : -u_creaseDirPlaneSign;

    vec2 textureMidPoint = vec2(0.5);

    createVertex(u_halfSideLength * (-1.0          * u_creaseDir - creaseDirRotated90),  textureMidPoint - 0.5      *creaseDirPlaneVec - 0.5*creaseDirPlaneVecRotated90);
    createVertex(u_halfSideLength * ((1.0 - a*2.0) * u_creaseDir - creaseDirRotated90),  textureMidPoint + (0.5 - a)*creaseDirPlaneVec - 0.5*creaseDirPlaneVecRotated90);
    createVertex(u_halfSideLength * (-1.0          * u_creaseDir + creaseDirRotated90),  textureMidPoint - 0.5      *creaseDirPlaneVec + 0.5*creaseDirPlaneVecRotated90);
    createVertex(u_halfSideLength * ((1.0 - a*2.0) * u_creaseDir + creaseDirRotated90),  textureMidPoint + (0.5 - a)*creaseDirPlaneVec + 0.5*creaseDirPlaneVecRotated90);

    createVertex(u_halfSideLength * ((1.0 - a*2.0) * u_creaseDir - creaseDirRotated90                ),  textureMidPoint + (0.5 - a)*creaseDirPlaneVec - 0.5*creaseDirPlaneVecRotated90);
    createVertex(u_halfSideLength * ((1.0 - a*2.0) * u_creaseDir - creaseDirRotated90 + a*2.0*bendDir),  textureMidPoint + 0.5      *creaseDirPlaneVec - 0.5*creaseDirPlaneVecRotated90);
    createVertex(u_halfSideLength * ((1.0 - a*2.0) * u_creaseDir + creaseDirRotated90                ),  textureMidPoint + (0.5 - a)*creaseDirPlaneVec + 0.5*creaseDirPlaneVecRotated90);
    createVertex(u_halfSideLength * ((1.0 - a*2.0) * u_creaseDir + creaseDirRotated90 + a*2.0*bendDir),  textureMidPoint + 0.5      *creaseDirPlaneVec + 0.5*creaseDirPlaneVecRotated90);
    EndPrimitive();
}
