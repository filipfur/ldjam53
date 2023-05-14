


////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


// Standard library includes

// Third party includes

// Own includes
#include "goptions.h"


////////////////////////////////////////////////////////////////////////////////
// OPTIONS
////////////////////////////////////////////////////////////////////////////////


namespace goptions
{

const float cubeSideLength = 2.0f;
const float cameraRadius = 12.0f;

const float playerWalkSpeedMax = 6.0f;
const float playerWalkAcceleration = 32.0f;
const float playerJumpAcceleration = 70.0f * 4.0f;
const float playerJumpTime = 0.042f;

const float gravity = 9.81f * 4.0f;

const float cubeOffsetX = 0.0f * cubeSideLength;
const float cubeOffsetY = 0.5f * cubeSideLength;
const float cubeOffsetZ = 0.0f * cubeSideLength;


////////////////////////////////////////////////////////////////////////////////
// DERIVED CONSTANTS
////////////////////////////////////////////////////////////////////////////////


const float halfCubeSideLength = cubeSideLength / 2;

} // namespace goptions
