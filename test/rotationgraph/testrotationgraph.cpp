#include <iostream>

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL  // glm::to_string
#include "glm/ext.hpp"  // glm::to_string

#include "rotationgraph.h"
//#include "playercontrol.h"

int main(int argc, const char* argv[])
{
    //assert(PlayerControl::PlayerDimension::NUM_PLAYER_DIMENSIONS == goptions::numDimensions);

    std::vector<Cube*> cubes;
    cubes.push_back(new Cube(glm::ivec3(0, 0, 0)));
    cubes.push_back(new Cube(glm::ivec3(1, 0, 0)));
    cubes.push_back(new Cube(glm::ivec3(1, 1, 0)));
    cubes.push_back(new Cube(glm::ivec3(0, 1, 0)));
    RotationGraph rg;
    rg.construct(cubes);

    // using CubeMapKey = glm::ivec3;
    for (auto cubeIt = rg.cubes().begin(); cubeIt != rg.cubes().end(); cubeIt++)
    {
        glm::ivec3 cubeIpos = cubeIt->first;
        Cube* cube = cubeIt->second;
        std::cout << "Cube at " << glm::to_string(cubeIpos) << ": " << cube << std::endl;
    }

    // using FaceMapKey = std::pair<const glm::ivec3, const glm::ivec3>;
    for (auto faceIt = rg.faces().begin(); faceIt != rg.faces().end(); faceIt++)
    {
        glm::ivec3 cubeIpos = faceIt->first.first;
        glm::ivec3 faceNormal = faceIt->first.second;
        Face* face = faceIt->second;
        std::cout << "Face at " << glm::to_string(cubeIpos) << " -> " << glm::to_string(faceNormal) << ": " << face << std::endl;
        for (int nIdx = 0; nIdx < Face::numNeightbors; nIdx++) {
            const Face* neighbor = face->neighbor(nIdx);
            //std::cout << "* neighbor: " << neighbor << std::endl;
            glm::ivec3 neighborCubeIpos = neighbor->cube()->iPos();
            glm::ivec3 neighborNormal = neighbor->normal();
            std::cout << "* Neighbor " << nIdx << ": at " << glm::to_string(neighborCubeIpos) << " -> " << glm::to_string(neighborNormal) << ": " << neighbor << std::endl;
        }
        for (int nIdx = 0; nIdx < Face::numNeightbors; nIdx++) {
            std::cout << "* Backlink " << nIdx << ": " << face->neighborBackEdgeIndex(nIdx) << std::endl;
        }
        for (int nIdx = 0; nIdx < Face::numNeightbors; nIdx++) {
            std::cout << "* Corner traversable " << nIdx << ": " << face->cornerTraversable(nIdx) << std::endl;
        }
    }

    return 0;
}
