

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes

// Third party includes

// Own includes
#include "goptions.h"
#include "level.h"
#include "mathematics.h"


////////////////////////////////////////////////////////////////////////////////
// METHODS
////////////////////////////////////////////////////////////////////////////////


Cube::Cube(glm::vec3 pos) :
    _pos(cubeIPosToPos(cubePosToIPos(pos)))
{
}

Cube::Cube(glm::ivec3 ipos) :
    _pos(glm::vec3(ipos) * goptions::cubeSideLength + glm::vec3(goptions::cubeOffsetX, goptions::cubeOffsetY, goptions::cubeOffsetZ))
{
}

glm::vec3 Cube::pos() const
{
    return _pos;
}

glm::ivec3 Cube::iPos() const
{
    return cubePosToIPos(_pos);
}

glm::ivec3 Cube::cubePosToIPos(glm::vec3 pos)
{
    glm::ivec3 iPos;
    for (int idx = 0; idx < goptions::numDimensions; idx++) {
        float componentAsFloat = (pos[idx] - cubeOffset[idx]) / goptions::cubeSideLength;
        iPos[idx] = sign(componentAsFloat) * static_cast<int>(std::abs(componentAsFloat) + 0.5f);
    }
    return iPos;
    /*
    return glm::ivec3(
        static_cast<int>((pos.x - goptions::cubeOffsetX) / goptions::cubeSideLength + 0.5f),
        static_cast<int>((pos.y - goptions::cubeOffsetY) / goptions::cubeSideLength + 0.5f),
        static_cast<int>((pos.z - goptions::cubeOffsetZ) / goptions::cubeSideLength + 0.5f));
    */
}

glm::vec3 Cube::cubeIPosToPos(glm::ivec3 iPos)
{
    return glm::vec3(iPos) * goptions::cubeSideLength + cubeOffset;
}

const glm::vec3 Cube::cubeOffset{
    goptions::cubeOffsetX,
    goptions::cubeOffsetY,
    goptions::cubeOffsetZ
    };

Face::Face(Cube* cube, size_t normalDim, int normalSign) :
    _cube(cube),
    _normalDim(normalDim),
    _normalSign(normalSign),
    _neighbors()
{
}

const Cube* Face::cube() const
{
    return _cube;
}

glm::ivec3 Face::normal() const
{
    return directionDimAndSignToDirection(_normalDim, _normalSign);
}

Face* Face::neighbor(size_t neighborIdx)
{
    return _neighbors[neighborIdx];
}

const Face* Face::neighbor(size_t neighborIdx) const
{
    return _neighbors[neighborIdx];
}

void Face::setNeighbor(size_t neighborIdx, Face* otherFace)
{
    _neighbors[neighborIdx] = otherFace;
}

const size_t Face::neighborBackEdgeIndex(size_t neighborIdx) const
{
    return _neighborBackEdgeIndex[neighborIdx];
}

void Face::setNeighborBackEdgeIndex(size_t neighborIdx, size_t backEdgeIndex)
{
    _neighborBackEdgeIndex[neighborIdx] = backEdgeIndex;
}

const bool Face::cornerTraversable(size_t cornerIdx) const
{
    return _cornerTraversable[cornerIdx];
}

void Face::setCornerTraversable(size_t cornerIdx, bool traversable)
{
    _cornerTraversable[cornerIdx] = traversable;
}

const size_t Face::spaceDimensionAndSignToEdgeIndex(size_t spaceDim, int spaceDimSign) const
{
    // Calculate index for face edge direction
    int dimOffset = (spaceDim - 1 - _normalDim + goptions::numDimensions) % goptions::numDimensions;
    int baseSign = dimOffset == 1 ? _normalSign : 1;  // The sign that will yield the lowest edge index for this dimension
    return dimOffset + goptions::numPlaneDimensions * (spaceDimSign != baseSign);
}

const size_t Face::spaceDirectionToEdgeIndex(glm::ivec3 dir) const
{
    for (int dirDim = 0; dirDim < goptions::numDimensions; dirDim++) {
        if (dir[dirDim] != 0) {
            return spaceDimensionAndSignToEdgeIndex(dirDim, dir[dirDim]);
        }
    }
    assert(false);  // Should never reach this line
    return 0;  // Prevent warning "not all control paths return a value"
}

const size_t Face::seminormalizedDirectionToCornerIndex(glm::ivec3 dir) const
{
    glm::ivec3 doubleEdgeDir = dir - iCross(normal(), dir);

    for (int dirDim = 0; dirDim < goptions::numDimensions; dirDim++) {
        if (doubleEdgeDir[dirDim] != 0) {
            return spaceDimensionAndSignToEdgeIndex(dirDim, doubleEdgeDir[dirDim] / 2);
        }
    }
    assert(false);  // Should never reach this line
    return 0;  // Prevent warning "not all control paths return a value"
}

glm::ivec3 Face::facePosToCubeIPos(glm::vec3 pos, glm::vec3 normal)
{
    return Cube::cubePosToIPos(pos - glm::vec3(normal) * (0.5f * goptions::cubeSideLength));
}

glm::ivec3 Face::directionDimAndSignToDirection(size_t dirDim, int dirSign)
{
    glm::ivec3 dir(0);
    dir[dirDim] = dirSign;
    return dir;
}
