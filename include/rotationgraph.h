#pragma once


////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes
#include <vector>
#include <map>

// Third party includes

// Own includes
#include "level.h"


////////////////////////////////////////////////////////////////////////////////
// PREDECLARATIONS
////////////////////////////////////////////////////////////////////////////////


class RotationGraphEdge;


////////////////////////////////////////////////////////////////////////////////
// CLASSES
////////////////////////////////////////////////////////////////////////////////


using CubeMapKey = glm::ivec3;
using FaceMapKey = std::pair<const glm::ivec3, const glm::ivec3>;


struct cubeLess {
    constexpr bool operator()(const CubeMapKey& lhs, const CubeMapKey& rhs) const {
        return (
            lhs.x != rhs.x ? lhs.x < rhs.x :
            lhs.y != rhs.y ? lhs.y < rhs.y :
            lhs.z != rhs.z ? lhs.z < rhs.z :
            false
            );
    }
};


struct faceLess {
    constexpr bool operator()(const FaceMapKey& lhs, const FaceMapKey& rhs) const {
        return (
            lhs.first.x != rhs.first.x ? lhs.first.x < rhs.first.x :
            lhs.first.y != rhs.first.y ? lhs.first.y < rhs.first.y :
            lhs.first.z != rhs.first.z ? lhs.first.z < rhs.first.z :
            lhs.second.x != rhs.second.x ? lhs.second.x < rhs.second.x :
            lhs.second.y != rhs.second.y ? lhs.second.y < rhs.second.y :
            lhs.second.z != rhs.second.z ? lhs.second.z < rhs.second.z :
            false
            );
    }
};


using CubeMap = std::map<
    CubeMapKey,
    Cube*,
    cubeLess>;


using FaceMap = std::map<
    FaceMapKey,
    Face*,
    faceLess>;


class RotationGraph
{
public:
    RotationGraph(std::vector<Cube*> cubes);

    const CubeMap& getCubes() const;
    const FaceMap& getFaces() const;

private:
    CubeMap _cubes;
    FaceMap _faces;
};
