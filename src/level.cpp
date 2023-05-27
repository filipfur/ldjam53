

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

Face::Face(Cube* cube, AADirection3 normal) :
    _cube(cube),
    _normal(normal),
    _neighbors()
{
}

const Cube* Face::cube() const
{
    return _cube;
}

AADirection3 Face::normal() const
{
    return _normal;
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

const size_t Face::aADirection3ToEdgeIndex(AADirection3 dir) const
{
    // Calculate index for face edge direction
    int dimOffset = (dir.dimension() - 1 - _normal.dimension() + goptions::numDimensions) % goptions::numDimensions;
    int baseSign = dimOffset == 1 ? _normal.sign() : 1;  // The sign that will yield the lowest edge index for this dimension
    return dimOffset + goptions::numPlaneDimensions * (dir.sign() != baseSign);
}

const size_t Face::spaceDirectionToEdgeIndex(glm::ivec3 dir) const
{
    for (int dirDim = 0; dirDim < goptions::numDimensions; dirDim++) {
        if (dir[dirDim] != 0) {
            return aADirection3ToEdgeIndex(AADirection3(dirDim, dir[dirDim]));
        }
    }
    assert(false);  // Should never reach this line
    return 0;  // Prevent warning "not all control paths return a value"
}

const size_t Face::seminormalizedDirectionToCornerIndex(glm::ivec3 dir) const
{
    glm::ivec3 doubleEdgeDir = dir - cross(normal(), dir);

    for (int dirDim = 0; dirDim < goptions::numDimensions; dirDim++) {
        if (doubleEdgeDir[dirDim] != 0) {
            return aADirection3ToEdgeIndex(AADirection3(dirDim, doubleEdgeDir[dirDim] / 2));
        }
    }
    assert(false);  // Should never reach this line
    return 0;  // Prevent warning "not all control paths return a value"
}

glm::ivec3 Face::facePosToCubeIPos(glm::vec3 pos, AADirection3 normal)
{
    return Cube::cubePosToIPos(pos - (0.5f * goptions::cubeSideLength) * normal);
}

glm::ivec3 Face::directionDimAndSignToDirection(size_t dirDim, int dirSign)
{
    glm::ivec3 dir(0);
    dir[dirDim] = dirSign;
    return dir;
}
