

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes

// Third party includes
#include "glm/glm.hpp"

// Own includes
#include "goptions.h"
#include "rotationgraph.h"


////////////////////////////////////////////////////////////////////////////////
// METHODS
////////////////////////////////////////////////////////////////////////////////


void RotationGraph::construct(std::vector<Cube*> cubes)
{
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

        for (int faceDim = 0; faceDim < goptions::numDimensions; faceDim++) {
            for (auto& faceSign : {-1, 1}) {
                // Create face normal in the given direction
                glm::ivec3 face_normal(0);
                face_normal[faceDim] = faceSign;

                // Check if face is directly facing another cube
                if (_cubes.find(cubeIpos + face_normal) == _cubes.end()) {
                    // Cube face is not facing any other cube => face exists
                    _faces[FaceMapKey(cubeIpos, face_normal)] = new Face(cube, face_normal);
                }
            }
        }
    }

    // Connect faces to each other
    for (auto& cube : cubes) {
        glm::ivec3 cubeIpos = cube->iPos();

        for (int faceDim = 0; faceDim < goptions::numDimensions; faceDim++) {
            for (auto& faceSign : {-1, 1}) {
                // Create face normal in the given direction
                glm::ivec3 face_normal(0);
                face_normal[faceDim] = faceSign;

                // Extract the face
                auto faceIt = _faces.find(FaceMapKey(cubeIpos, face_normal));
                if (faceIt == _faces.end()) {
                    continue;
                }
                Face* face = faceIt->second;

                // Check if face is directly facing another cube
                if (_cubes.find(cubeIpos + face_normal) != _cubes.end()) {
                    // Cube face is facing other cube => face unreachable
                    continue;
                }

                // Check which other faces the face is adjacent to on each side, and connect it if the other face exists
                // Construct dimension direction vectors for the goptions::numDimensions-1 dimension direcions that are orthogonal to face_normal
                glm::ivec3 orthDimDirs[goptions::numDimensions-1];
                // Loop through different possible cube position offsets
                for (int orthDimIdx = 0; orthDimIdx < goptions::numDimensions-1; orthDimIdx++) {
                    int orthDim = orthDimIdx + (orthDimIdx >= faceDim);
                    for (auto& orthDimDirSign : {-1, 1}) {
                        glm::ivec3 orthOffset(0);
                        orthOffset[orthDim] = orthDimDirSign;

                        // y-direction is "up"
                        int faceEdgeIdx = (orthDim - 1 - faceDim + goptions::numDimensions) % goptions::numDimensions + (goptions::numDimensions - 1) * (faceSign * orthDimDirSign == -1);

                        glm::ivec3 otherCubeIpos;
                        // Look for face on cube diagonally to this cube
                        if (_cubes.find(otherCubeIpos = cubeIpos + orthOffset + face_normal) != _cubes.end()) {
                            // Face is adjacent to face on other cube with 90 degree andle to itself in the given orthogonal direction
                            auto otherFace = _faces.find(FaceMapKey(otherCubeIpos, -orthOffset));
                            if (otherFace != _faces.end()) {
                                face->setNeighbor(faceEdgeIdx, otherFace->second);
                            }
                        }
                        // Look for face on cube adjacent to this cube
                        else if (_cubes.find(otherCubeIpos = cubeIpos + orthOffset) != _cubes.end()) {
                            // Face is adjacent to face on other cube with 90 degree andle to itself in the given orthogonal direction
                            auto otherFace = _faces.find(FaceMapKey(otherCubeIpos, face_normal));
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
}

CubeMap& RotationGraph::cubes()
{
    return _cubes;
}

FaceMap& RotationGraph::faces()
{
    return _faces;
}