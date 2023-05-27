#pragma once


////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes

// Third party includes
#include "glm/glm.hpp"

// Own includes


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


template <typename T>
int pSign(T val) {
    // See https://stackoverflow.com/a/4609795/1070480
    return (val >= T(0)) - (val < T(0));
}

template <typename T>
int sign(T val) {
    // See https://stackoverflow.com/a/4609795/1070480
    return (val > T(0)) - (val < T(0));
}

template <typename T>
float fSign(T val) {
    // See https://stackoverflow.com/a/4609795/1070480
    return static_cast<float>(sign(val));
}

inline glm::ivec3 iCross(glm::ivec3 const& x, glm::ivec3 const& y)
{
    return glm::ivec3(
        x.y * y.z - y.y * x.z,
        x.z * y.x - y.z * x.x,
        x.x * y.y - y.x * x.y);
}

inline glm::ivec3 iVecMult(int factor, glm::ivec3 vec)
{
    return glm::ivec3(
        factor * vec[0],
        factor * vec[1],
        factor * vec[2]);
}

inline glm::vec3 rotatedVecor(glm::vec3 vec, glm::vec3 rotateFromVec, glm::vec3 rotateToVec)
{
    float rotateFromComponent = glm::dot(vec, rotateFromVec);
    float rotateToComponent = glm::dot(vec, rotateToVec);
    return vec + (-rotateFromComponent - rotateToComponent) * rotateFromVec + (rotateFromComponent - rotateToComponent) * rotateToVec;
}

template <typename T>
size_t direction3ToDimensionIndex(T vec)
{
    return (
        0 * (vec[0] != 0) +
        1 * (vec[1] != 0) +
        2 * (vec[2] != 0) );
}
