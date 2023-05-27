#pragma once


////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes

// Third party includes
#include "glm/glm.hpp"

// Own includes
#include "goptions.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CLASSES
////////////////////////////////////////////////////////////////////////////////


class Cube
{
public:
    Cube() = delete;
    Cube(glm::vec3 pos);
    Cube(glm::ivec3 ipos);

    glm::vec3 pos() const;
    glm::ivec3 iPos() const;

    static glm::ivec3 cubePosToIPos(glm::vec3 pos);
    static glm::vec3 cubeIPosToPos(glm::ivec3 pos);

    static const glm::vec3 cubeOffset;

private:
    glm::vec3 _pos;
};


class Face
{
public:
    static constexpr size_t numNeightbors = goptions::numPlaneDimensions * 2;
    static constexpr size_t numCorners = numNeightbors;

    Face() = delete;
    Face(Cube* cube, size_t normalDim, int normalSign);

    const Cube* cube() const;
    glm::ivec3 normal() const;
    Face* neighbor(size_t neighborIdx);
    const Face* neighbor(size_t neighborIdx) const;
    void setNeighbor(size_t neighborIdx, Face* otherFace);
    const size_t neighborBackEdgeIndex(size_t neighborIdx) const;
    void setNeighborBackEdgeIndex(size_t neighborIdx, size_t backEdgeIndex);
    const bool cornerTraversable(size_t cornerIdx) const;
    void setCornerTraversable(size_t cornerIdx, bool traversable);
    const size_t spaceDimensionAndSignToEdgeIndex(size_t spaceDim, int spaceDimSign) const;
    const size_t spaceDirectionToEdgeIndex(glm::ivec3 dir) const;
    const size_t seminormalizedDirectionToCornerIndex(glm::ivec3 dir) const; // seminormalized means that each component is either 0, 1 or -1

    static glm::ivec3 facePosToCubeIPos(glm::vec3 pos, glm::vec3 normal);
    static glm::ivec3 directionDimAndSignToDirection(size_t dirDim, int dirSign);

private:
    Cube* _cube;
    size_t _normalDim;
    int _normalSign;
    Face* _neighbors[numNeightbors];
    size_t _neighborBackEdgeIndex[numNeightbors];
    bool _cornerTraversable[numCorners];
};
