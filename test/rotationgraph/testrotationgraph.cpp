#include <iostream>

#include <glm/glm.hpp>

#include "rotationgraph.h"

int main(int argc, const char* argv[])
{
    std::vector<Cube*> cubes;
    cubes.push_back(new Cube(glm::ivec3(0, 0, 0)));
    cubes.push_back(new Cube(glm::ivec3(1, 0, 0)));
    cubes.push_back(new Cube(glm::ivec3(1, 1, 0)));
    RotationGraph rg = RotationGraph(cubes);

    // using CubeMapKey = glm::ivec3;
    for (auto cubeIt = rg.getCubes().begin(); cubeIt != rg.getCubes().end(); cubeIt++)
    {
        glm::ivec3 cubeIpos = cubeIt->first;
        Cube* cube = cubeIt->second;
        std::cout << "Cube at (" << cubeIpos.x << ", " << cubeIpos.y << ", " << cubeIpos.z << "): " << cube << std::endl;
    }

    // using FaceMapKey = std::pair<const glm::ivec3, const glm::ivec3>;
    for (auto faceIt = rg.getFaces().begin(); faceIt != rg.getFaces().end(); faceIt++)
    {
        glm::ivec3 cubeIpos = faceIt->first.first;
        glm::ivec3 faceNormal = faceIt->first.second;
        Face* face = faceIt->second;
        std::cout << "Face at (" << cubeIpos.x << ", " << cubeIpos.y << ", " << cubeIpos.z << ") -> (" << faceNormal.x << ", " << faceNormal.y << ", " << faceNormal.z << "): " << face << std::endl;
        for (int nIdx = 0; nIdx < Face::numNeightbors; nIdx++) {
            const Face* neighbor = face->neighbor(nIdx);
            //std::cout << "* neighbor: " << neighbor << std::endl;
            glm::ivec3 neighborCubeIpos = neighbor->cube()->iPos();
            glm::ivec3 neighborNormal = neighbor->normal();
            std::cout << "* Neighbor at (" << neighborCubeIpos.x << ", " << neighborCubeIpos.y << ", " << neighborCubeIpos.z << ") -> (" << neighborNormal.x << ", " << neighborNormal.y << ", " << neighborNormal.z << "): " << neighbor << std::endl;
        }
    }

    return 0;
}
