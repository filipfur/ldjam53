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

extern const bool shouldApplyCreaseRenderingBugFix;

extern const float cubeSideLength;
extern const float cameraRadius;

extern const float playerWalkSpeedMax;
extern const float playerWalkAcceleration;
extern const float playerAirWalkAcceleration;
extern const float playerJumpAcceleration;
extern const float playerJumpTime;

extern const float gravity;

// Offset from the origin to the middle of the cube with iPos (integer position? index-position?) (0, 0, 0)
extern const float cubeOffsetX;
extern const float cubeOffsetY;
extern const float cubeOffsetZ;

extern const float characterWidth;
extern const float characterHeight;


////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////


constexpr size_t numDimensions = 3;
constexpr size_t numFacesAroundTraversableCorner = 4;


////////////////////////////////////////////////////////////////////////////////
// DERIVED CONSTANTS
////////////////////////////////////////////////////////////////////////////////


// Requires constexpr
constexpr size_t numPlaneDimensions = numDimensions - 1;

// Compile-time intialized in goptions.cpp

extern const float halfCubeSideLength;

} // namespace goptions
