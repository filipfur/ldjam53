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
// CLASSES
////////////////////////////////////////////////////////////////////////////////


class AADirection3
{
public:
    AADirection3() = delete;
    AADirection3(size_t dimension, int sign) : _dimension(dimension), _sign(sign) {}

    size_t dimension() const {return _dimension;}
    int sign() const {return _sign;}

    int dot(AADirection3 vec) const {return (_dimension == vec.dimension()) * _sign * vec.sign();}
    int dot(glm::ivec3 vec) const {return _sign * vec[_dimension];}
    float dot(glm::vec3 vec) const {return _sign * vec[_dimension];}

    glm::ivec3 operator+(AADirection3 dir) const
    {
        return toIVec3() + dir.toIVec3();
    }

    glm::ivec3 operator+(glm::ivec3 vec) const
    {
        return toIVec3() + vec;
    }

    glm::ivec3 operator-(AADirection3 dir) const
    {
        return toIVec3() - dir.toIVec3();
    }

    glm::ivec3 operator-(glm::ivec3 vec) const
    {
        return toIVec3() - vec;
    }

    AADirection3 signMul(int sign) const {return AADirection3(_dimension, sign * _sign);}
    glm::ivec3 operator*(int factor) const
    {
        glm::ivec3 vec(0);
        vec[_dimension] = _sign * factor;
        return vec;
    }
    glm::vec3 operator*(float factor) const
    {
        glm::vec3 vec(0.0f);
        vec[_dimension] = _sign * factor;
        return vec;
    }
    glm::ivec3 toIVec3() const
    {
        glm::ivec3 vec(0);
        vec[_dimension] = _sign;
        return vec;
    }
    glm::vec3 toVec3() const
    {
        glm::vec3 vec(0.0f);
        vec[_dimension] = _sign;
        return vec;
    }

    AADirection3 operator-() const {return AADirection3(_dimension, -_sign);}

    bool operator==(const AADirection3& other) const
    {
        return (
            _dimension == other.dimension() &&
            _sign == other.sign()
            );
    }
    bool operator!=(const AADirection3& other) const
    {
        return (
            _dimension != other.dimension() ||
            _sign != other.sign()
            );
    }

    AADirection3& operator=(const AADirection3& other)
    {
        _dimension = other.dimension();
        _sign = other.sign();
        return *this;
    }


private:
    size_t _dimension;
    int _sign;
};

inline glm::ivec3 operator+(glm::ivec3 vec, AADirection3 dir) {return vec + dir.toIVec3();}
inline glm::ivec3 operator-(glm::ivec3 vec, AADirection3 dir) {return vec - dir.toIVec3();}
inline glm::ivec3 operator*(int factor, AADirection3 dir) {return dir * factor;}
inline glm::vec3 operator*(float factor, AADirection3 dir) {return dir * factor;}

inline int dot(AADirection3 vec1, AADirection3 vec2) {return vec1.dot(vec2);}
inline int dot(AADirection3 vec1, glm::ivec3 vec2) {return vec1.dot(vec2);}
inline int dot(glm::ivec3 vec1, AADirection3 vec2) {return vec2.dot(vec1);}
inline float dot(AADirection3 vec1, glm::vec3 vec2) {return vec1.dot(vec2);}
inline float dot(glm::vec3 vec1, AADirection3 vec2) {return vec2.dot(vec1);}

inline glm::ivec3 cross(AADirection3 const& x, glm::ivec3 const& y)
{
    glm::ivec3 prod(0);
    size_t otherDim1 = (x.dimension() + 1) % goptions::numDimensions;
    size_t otherDim2 = (x.dimension() + 2) % goptions::numDimensions;
    prod[otherDim2] =  x.sign() * y[otherDim1];
    prod[otherDim1] = -x.sign() * y[otherDim2];
    return prod;
}
