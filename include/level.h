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


constexpr float CUBE_SIDE_LENGTHS = 2.0;


////////////////////////////////////////////////////////////////////////////////
// CLASSES
////////////////////////////////////////////////////////////////////////////////


class Cube
{
public:
    Cube(glm::vec3 pos);
    Cube(glm::ivec3 ipos);

    glm::vec3 pos() const;
    glm::ivec3 iPos() const;

private:
    glm::vec3 _pos;
};


class Face
{
public:
    static constexpr size_t numNeightbors = (goptions::numDimensions - 1) * 2;

    Face(Cube* cube, glm::ivec3 normal);

    const Cube* cube() const;
    glm::ivec3 normal() const;
    const Face* neighbor(size_t neighborIdx) const;
    void setNeighbor(size_t neighborIdx, Face* otherFace);

private:
    Cube* _cube;
    glm::ivec3 _normal;
    Face* _neighbors[numNeightbors];
};
