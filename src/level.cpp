

////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes

// Third party includes

// Own includes
#include "goptions.h"
#include "level.h"


////////////////////////////////////////////////////////////////////////////////
// METHODS
////////////////////////////////////////////////////////////////////////////////


Cube::Cube(glm::vec3 pos) :
    _pos(pos)
{
}

Cube::Cube(glm::ivec3 ipos) :
    _pos(glm::vec3(ipos) * goptions::cubeSideLength)
{
}

glm::vec3 Cube::pos() const
{
    return _pos;
}

glm::ivec3 Cube::iPos() const
{
    return glm::ivec3(
        static_cast<int>(_pos.x / goptions::cubeSideLength + 0.5f),
        static_cast<int>(_pos.y / goptions::cubeSideLength + 0.5f),
        static_cast<int>(_pos.z / goptions::cubeSideLength + 0.5f));
}

Face::Face(Cube* cube, glm::ivec3 normal) :
    _cube(cube),
    _normal(normal),
    _neighbors()
{
}

const Cube* Face::cube() const
{
    return _cube;
}

glm::ivec3 Face::normal() const
{
    return _normal;
}

const Face* Face::neighbor(size_t neighborIdx) const
{
    return _neighbors[neighborIdx];
}

void Face::setNeighbor(size_t neighborIdx, Face* otherFace)
{
    _neighbors[neighborIdx] = otherFace;
}
