#pragma once


////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes

// Third party includes

// Own includes


////////////////////////////////////////////////////////////////////////////////
// OPTIONS
////////////////////////////////////////////////////////////////////////////////


namespace goptions
{

// Requires constexpr

// Compile-time intialized in goptions.cpp

extern const float cubeSideLength;
extern const float cameraRadius;

extern const float playerWalkSpeedMax;
extern const float playerWalkAcceleration;
extern const float playerJumpAcceleration;
extern const float playerJumpTime;

extern const float gravity;

extern const float cubeOffsetX;
extern const float cubeOffsetY;
extern const float cubeOffsetZ;


////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////


constexpr size_t numDimensions = 3;


////////////////////////////////////////////////////////////////////////////////
// DERIVED CONSTANTS
////////////////////////////////////////////////////////////////////////////////


// Requires constexpr

// Compile-time intialized in goptions.cpp

extern const float halfCubeSideLength;

} // namespace goptions
