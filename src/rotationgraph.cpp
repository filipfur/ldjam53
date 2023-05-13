

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes
#include <iostream>

// Third party includes
#include "glm/glm.hpp"

#define GLM_ENABLE_EXPERIMENTAL  // glm::to_string
#include "glm/ext.hpp"  // glm::to_string

// Own includes
#include "goptions.h"
#include "rotationgraph.h"


////////////////////////////////////////////////////////////////////////////////
// METHODS
////////////////////////////////////////////////////////////////////////////////


void RotationGraph::construct(std::vector<Cube*> cubes)
{
    if (1) {
        for (auto& cube : cubes) {
            //std::cout << "Cube at i-position " << glm::to_string(cube->iPos()) << "; position " << glm::to_string(cube->pos()) << std::endl;
            std::cout << "Cube at i-position " << glm::to_string(cube->iPos()) << std::endl;
            if (glm::length(Cube::cubeIPosToPos(cube->iPos()) - cube->pos()) > 0.001f) {
                //std::cout << "ERROR: Cube at i-position " << glm::to_string(cube->iPos()) << "; position " << glm::to_string(cube->pos()) << std::endl;
                //exit(0);
            }
        }
        std::cout << "All cube i-postions printed." << std::endl;
        //exit(0);
    }
    _cubes.clear();
    _faces.clear();

    // Populate _cubes
    for (auto& cube : cubes) {
        glm::ivec3 cubeIpos = cube->iPos();
        _cubes[cubeIpos] = cube;
    }

    // Populate _faces
    for (auto& cube : cubes) {
        glm::ivec3 cubeIpos = cube->iPos();

        for (int normalDim = 0; normalDim < goptions::numDimensions; normalDim++) {
            for (auto& normalSign : {-1, 1}) {
                glm::ivec3 faceNormal = Face::directionDimAndSignToDirection(normalDim, normalSign);

                // Check if face is directly facing another cube
                if (_cubes.find(cubeIpos + faceNormal) == _cubes.end()) {
                    // Cube face is not facing any other cube => face exists
                    _faces[FaceMapKey(cubeIpos, faceNormal)] = new Face(cube, normalDim, normalSign);
                }
            }
        }
    }

    // Connect faces to each other
    for (auto& cube : cubes) {
        glm::ivec3 cubeIpos = cube->iPos();

        for (int normalDim = 0; normalDim < goptions::numDimensions; normalDim++) {
            for (auto& normalSign : {-1, 1}) {
                glm::ivec3 faceNormal = Face::directionDimAndSignToDirection(normalDim, normalSign);

                // Extract the face
                auto faceIt = _faces.find(FaceMapKey(cubeIpos, faceNormal));
                if (faceIt == _faces.end()) {
                    continue;
                }
                Face* face = faceIt->second;

                // Check if face is directly facing another cube
                if (_cubes.find(cubeIpos + faceNormal) != _cubes.end()) {
                    // Cube face is facing other cube => face unreachable
                    continue;
                }

                // Check which other faces the face is adjacent to on each side, and connect it if the other face exists
                // Construct dimension direction vectors for the goptions::numDimensions-1 dimension direcions that are orthogonal to faceNormal
                glm::ivec3 orthDimDirs[goptions::numDimensions-1];
                // Loop through different possible cube position offsets
                for (int orthDimIdx = 0; orthDimIdx < goptions::numDimensions-1; orthDimIdx++) {
                    int orthDim = orthDimIdx + (orthDimIdx >= normalDim);
                    for (auto& orthDimDirSign : {-1, 1}) {
                        glm::ivec3 orthOffset(0);
                        orthOffset[orthDim] = orthDimDirSign;

                        // Calculate index of face edge
                        int faceEdgeIdx = face->spaceDimensionAndSignToEdgeIndex(orthDim, orthDimDirSign);

                        glm::ivec3 otherCubeIpos;
                        // Look for face on cube diagonally to this cube
                        if (_cubes.find(otherCubeIpos = cubeIpos + orthOffset + faceNormal) != _cubes.end()) {
                            // Face is adjacent to face on other cube with 90 degree andle to itself in the given orthogonal direction
                            auto otherFace = _faces.find(FaceMapKey(otherCubeIpos, -orthOffset));
                            if (otherFace != _faces.end()) {
                                face->setNeighbor(faceEdgeIdx, otherFace->second);
                            }
                        }
                        // Look for face on cube adjacent to this cube
                        else if (_cubes.find(otherCubeIpos = cubeIpos + orthOffset) != _cubes.end()) {
                            // Face is adjacent to face on other cube with 90 degree andle to itself in the given orthogonal direction
                            auto otherFace = _faces.find(FaceMapKey(otherCubeIpos, faceNormal));
                            if (otherFace != _faces.end()) {
                                face->setNeighbor(faceEdgeIdx, otherFace->second);
                            }
                        }
                        else {
                            // Face is simply adjacent to face on the same cube in the given orthogonal direction
                            auto otherFace = _faces.find(FaceMapKey(cubeIpos, orthOffset));
                            if (otherFace != _faces.end()) {
                                face->setNeighbor(faceEdgeIdx, otherFace->second);
                            }
                        }
                    }
                }
            }
        }
    }

    // Create back-links
    for (auto& keyFacePair : _faces) {
        Face* face = keyFacePair.second;
        for (int neighborIdx = 0; neighborIdx < Face::numNeightbors; neighborIdx++) {
            Face* neighbor = face->neighbor(neighborIdx);
            int neighborNeighborIdx = 0;
            for (; neighbor->neighbor(neighborNeighborIdx) != face; neighborNeighborIdx++) {}
            face->setNeighborBackEdgeIndex(neighborIdx, neighborNeighborIdx);
            assert(face->neighbor(neighborIdx)->neighbor(face->neighborBackEdgeIndex(neighborIdx)) == face);
        }
    }

    // Calculate corner traversability
    for (auto& keyFacePair : _faces) {
        Face* face = keyFacePair.second;
        for (int neighborIdx = 0; neighborIdx < Face::numNeightbors; neighborIdx++) {
            Face* currentFace = face;
            int currentNeighborIdx = neighborIdx;
            bool shouldPrint = keyFacePair.first.first == glm::ivec3(0,0,0) && keyFacePair.first.second == glm::ivec3(0,0,1) && currentNeighborIdx == 0;
            shouldPrint = false;
            if (shouldPrint) {
                glm::ivec3 cubePos = currentFace->cube()->iPos();
                glm::ivec3 normal = currentFace->normal();
                std::cout << "0: cubePos = (" << cubePos.x << ", " << cubePos.y << ", " << cubePos.z << "), normal = (" << normal.x << ", " << normal.y << ", " << normal.z << "), currentNeighborIdx = " << currentNeighborIdx << std::endl;
            }
            for (int step = 0; step < goptions::numFacesAroundTraversableCorner; step++) {
                int nextNeighborIdx = (currentFace->neighborBackEdgeIndex(currentNeighborIdx) + (Face::numNeightbors - 1)) % Face::numNeightbors;
                currentFace = currentFace->neighbor(currentNeighborIdx);
                currentNeighborIdx = nextNeighborIdx;
                if (shouldPrint) {
                    glm::ivec3 cubePos = currentFace->cube()->iPos();
                    glm::ivec3 normal = currentFace->normal();
                    std::cout << "0: cubePos = (" << cubePos.x << ", " << cubePos.y << ", " << cubePos.z << "), normal = (" << normal.x << ", " << normal.y << ", " << normal.z << "), currentNeighborIdx = " << currentNeighborIdx << std::endl;
                }
            }
            if (shouldPrint) {exit(0);}
            // The corner is traversable if we have arrived back at the same face after taking goptions::numFacesAroundTraversableCorner around it
            face->setCornerTraversable(neighborIdx, currentFace == face);
        }
    }
}

CubeMap& RotationGraph::cubes()
{
    return _cubes;
}

FaceMap& RotationGraph::faces()
{
    return _faces;
}