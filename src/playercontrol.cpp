#include "playercontrol.h"

#include "goptions.h"
#include "mathematics.h"

#define GLM_ENABLE_EXPERIMENTAL  // glm::to_string
#include "glm/ext.hpp"  // glm::to_string

PlayerControl::PlayerControl(std::shared_ptr<Sprite> sprite, std::shared_ptr<lithium::Input> input) :
    _sprite(sprite)
{
    _keyCache = std::make_shared<lithium::Input::KeyCache>(std::initializer_list<int>{GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE});
    input->setKeyCache(_keyCache);
}

void PlayerControl::update(float dt)
{
    _sprite->update(dt);
    State oldState = _state;
    if(onGround())
    {
        if(_keyCache->isPressed(GLFW_KEY_A) && (_direction == LEFT || _state == IDLE))
        {
            increaseSpeedRight(-1.0f * goptions::playerWalkAcceleration * dt);
            setSpeedRight(std::max(speedRight(), -goptions::playerWalkSpeedMax));
            _state = WALKING;
            _direction = LEFT;
        }
        else if(_keyCache->isPressed(GLFW_KEY_D) && (_direction == RIGHT || _state == IDLE))
        {
            increaseSpeedRight(1.0f * goptions::playerWalkAcceleration * dt);
            setSpeedRight(std::min(speedRight(), goptions::playerWalkSpeedMax));
            _state = WALKING;
            _direction = RIGHT;
        }
        else
        {
            float sr = speedRight();
            float newSr = sr * 0.8f;
            if(sr * sr < 1)
            {
                newSr = 0;
                _state = IDLE;
            }
            setSpeedRight(newSr);
        }
        if(_keyCache->isPressed(GLFW_KEY_SPACE))
        {
            _state = JUMPING;
            _jumpDuration = goptions::playerJumpTime;
            setSpeedRight(speedRight() * 0.9f);
        }
    }
    else
    {
        if(_state == JUMPING)
        {
            increaseSpeedUp(goptions::playerJumpAcceleration * dt);
            _jumpDuration -= dt;
            if(_jumpDuration <= 0)
            {
                _state = FALLING;
            }
        }
        setSpeedRight(glm::mix(speedRight(), 0.0f, dt * 0.3f));
    }
    increaseSpeedUp(-goptions::gravity * dt);
    // Always let the gravity act on the velocity; if the player stands on ground, the horizontal velocity component will become zero in the move function
    //increaseSpeedUp(-goptions::gravity * dt);

    move(dt);

    if(oldState != _state)
    {
        switch(_state)
        {
        case IDLE:
            _sprite->setAnimation("idle");
            break;
        case WALKING:
            _sprite->setAnimation("walk");
            break;
        case JUMPING:
            _sprite->setAnimation("idle");
            break;
        case FALLING:
            _sprite->setAnimation("walk");
            break;
        }
    }

    _sprite->setFlipped(_direction == LEFT);
}

int signIdxToSign(int signIdx)
{
    return signIdx * 2 - 1;
}

int signToSignIdx(int sign)
{
    return (sign + 1) / 2;
}

void PlayerControl::move(float dt)
{
    constexpr float greaterThanOne = 2.0f;  // Auxilliary constant

    Face* currentFace = _currentFace;  // If the player moves between faces, iterate through the faces and keep track of which face he is on
    glm::vec3 currentMidPos(playerPosToMidPos(_sprite->position(), _playerDimensionDirections[VERTICAL]));  // Keep track of the player's mid position
    glm::vec3 currentVelocity(_velocity);  // Keep track of the player's velocity
    glm::vec3 currentPlayerDimensionDirections[goptions::numDimensions]{  // Keep track of the player's coordinate system axis directions
        _playerDimensionDirections[0],  // Right
        _playerDimensionDirections[1],  // Up
        _playerDimensionDirections[2]}; // Normal

    int loopNumber = 0;
    while (dt > 0) {
        if (++loopNumber > 100) {
            std::cerr << "ERROR: Stuck in a loop!" << std::endl;
            exit(0);
        }

        glm::vec3 deltaPosition = currentVelocity * dt;
    
        const Cube* currentCube = currentFace->cube();
        glm::ivec3 currentCubeIPos = currentCube->iPos();
        glm::vec3 currentCubePos = currentCube->pos();

        glm::vec2 deltaPos2;
        glm::vec2 absDeltaPos2;
        Face* neighbors[goptions::numPlaneDimensions];
        Face* neighborss[goptions::numPlaneDimensions][2];
        bool edgeTraversable[goptions::numPlaneDimensions][2];
        bool edgeAndCornerTraversable[goptions::numPlaneDimensions];

        glm::ivec3 seminormalizedCornerDirection{0};
        for (int dimIdx = 0; dimIdx < goptions::numPlaneDimensions; dimIdx++) {
            deltaPos2[dimIdx] = glm::dot(deltaPosition, currentPlayerDimensionDirections[dimIdx]);
            absDeltaPos2[dimIdx] = std::abs(deltaPos2[dimIdx]);
            for (int signIdx = 0; signIdx < 2; signIdx++) {
                neighborss[dimIdx][signIdx] = currentFace->neighbor(currentFace->spaceDirectionToEdgeIndex(iVecMult(signIdxToSign(signIdx), currentPlayerDimensionDirections[dimIdx])));
                edgeTraversable[dimIdx][signIdx] = neighborss[dimIdx][signIdx]->normal() == currentFace->normal() || dimIdx == HORIZONTAL;
            }
            neighbors[dimIdx] = neighborss[dimIdx][signToSignIdx(pSign(deltaPos2[dimIdx]))];
            seminormalizedCornerDirection += iVecMult(sign(deltaPos2[dimIdx]), currentPlayerDimensionDirections[dimIdx]);
        }

        bool cornerTraversable = (
            deltaPos2[0] == 0.0f ||
            deltaPos2[1] == 0.0f ||
            currentFace->cornerTraversable(currentFace->seminormalizedDirectionToCornerIndex(seminormalizedCornerDirection))
        );

        float ratiosUntilFaceChange[goptions::numPlaneDimensions]{greaterThanOne};
        float ratiosUntilEdgeHit[goptions::numPlaneDimensions]{greaterThanOne};
        bool intersectingEdge[goptions::numPlaneDimensions]{false};

        if (1) {
            std::cout << "=======================================================================" << std::endl;
            std::cout << "loopNumber: " << loopNumber << std::endl;
            std::cout << "dt: " << dt << std::endl;
            std::cout << "currentCubeIPos: " << glm::to_string(currentCubeIPos) << std::endl;
            std::cout << "currentCubePos: " << glm::to_string(currentCubePos) << std::endl;
            std::cout << "currentMidPos: " << glm::to_string(currentMidPos) << std::endl;
            std::cout << "currentVelocity: " << glm::to_string(currentVelocity) << std::endl;
            std::cout << "deltaPosition: " << glm::to_string(deltaPosition) << std::endl;
            std::cout << "deltaPos2: " << glm::to_string(deltaPos2) << std::endl;
            std::cout << "sign(deltaPos2): " << glm::to_string(glm::vec2(sign(deltaPos2[0]), sign(deltaPos2[1]))) << std::endl;
            std::cout << "currentCubeIPos: " << glm::to_string(currentCubeIPos) << std::endl;
            std::cout << "currentPlayerDimensionDirections[HORIZONTAL]: " << glm::to_string(currentPlayerDimensionDirections[HORIZONTAL]) << std::endl;
            std::cout << "currentPlayerDimensionDirections[VERTICAL]: " << glm::to_string(currentPlayerDimensionDirections[VERTICAL]) << std::endl;
            std::cout << "currentPlayerDimensionDirections[NORMAL]: " << glm::to_string(currentPlayerDimensionDirections[NORMAL]) << std::endl;
        }
        bool invalidValues = false;
        for (int dimIdx = 0; dimIdx < goptions::numPlaneDimensions; dimIdx++) {
            if (deltaPos2[dimIdx] != 0.0f) {
                // Face change
                float distanceUntilFaceChange = 0.5f * goptions::cubeSideLength - sign(deltaPos2[dimIdx]) * glm::dot(currentMidPos - currentCubePos, currentPlayerDimensionDirections[dimIdx]);
                ratiosUntilFaceChange[dimIdx] = distanceUntilFaceChange / absDeltaPos2[dimIdx];

                // Edge hit
                float distanceUntilEdgeHit = distanceUntilFaceChange - 0.5f * _playerSizes[dimIdx];
                ratiosUntilEdgeHit[dimIdx] = distanceUntilEdgeHit / absDeltaPos2[dimIdx];
            }
        }
        for (int dimIdx = 0; dimIdx < goptions::numPlaneDimensions; dimIdx++) {
            if (deltaPos2[dimIdx] != 0.0f) {
                // Calculate whether the payer will be able to traverse the edge, with corners taken into account
                int otherDimIdx = 1 - dimIdx; assert(goptions::numPlaneDimensions == 2);
                float edgeHitPosition = glm::dot(currentMidPos - currentCubePos, currentPlayerDimensionDirections[otherDimIdx]) + std::max(ratiosUntilEdgeHit[dimIdx], 0.0f) * deltaPos2[otherDimIdx];
                glm::ivec3 firstSeminormalizedCornerDirection  = sign(deltaPos2[dimIdx]) * currentPlayerDimensionDirections[dimIdx] - currentPlayerDimensionDirections[otherDimIdx];
                glm::ivec3 secondSeminormalizedCornerDirection = sign(deltaPos2[dimIdx]) * currentPlayerDimensionDirections[dimIdx] + currentPlayerDimensionDirections[otherDimIdx];
                int firstCornerIndex = currentFace->seminormalizedDirectionToCornerIndex(firstSeminormalizedCornerDirection);
                int secondCornerIndex = currentFace->seminormalizedDirectionToCornerIndex(secondSeminormalizedCornerDirection);
                bool firstCornerTraversable  = currentFace->cornerTraversable(firstCornerIndex);
                bool secondCornerTraversable = currentFace->cornerTraversable(secondCornerIndex);

                float characterThinningFactor = 1.0f - ((deltaPos2[otherDimIdx] == 0.0f || ratiosUntilEdgeHit[dimIdx] < 0.0f) ? 1e-3f : 0.0f); // Prevent character from hitting corners he should just scrape against but not bump into, which can happen if he is travelling parallel with one axis
                std::cout << "## deltaPos2["<< otherDimIdx << "]: " << deltaPos2[otherDimIdx] << std::endl;
                std::cout << "## characterThinningFactor: " << characterThinningFactor << std::endl;
                edgeAndCornerTraversable[dimIdx] = (
                    edgeTraversable[dimIdx][signToSignIdx(pSign(deltaPos2[dimIdx]))]
                    && (edgeHitPosition > -0.5f * (goptions::cubeSideLength - characterThinningFactor * _playerSizes[otherDimIdx]) || firstCornerTraversable  || !edgeTraversable[otherDimIdx][0])
                    && (edgeHitPosition <  0.5f * (goptions::cubeSideLength - characterThinningFactor * _playerSizes[otherDimIdx]) || secondCornerTraversable || !edgeTraversable[otherDimIdx][1])
                );
                if (0 && dimIdx == HORIZONTAL && !edgeAndCornerTraversable[dimIdx] && ratiosUntilEdgeHit[dimIdx] < 1.0f) {
                    std::cout << "#################################" << std::endl;
                    std::cout << "dimIdx: " << dimIdx << std::endl;
                    std::cout << "currentMidPos: " << glm::to_string(currentMidPos) << std::endl;
                    std::cout << "currentCubePos: " << glm::to_string(currentCubePos) << std::endl;
                    std::cout << "currentCubeIPos: " << glm::to_string(currentCubeIPos) << std::endl;
                    glm::ivec3 cdirs[4] = {glm::ivec3(1, -1, 0), glm::ivec3(1, 1, 0), glm::ivec3(-1, 1, 0), glm::ivec3(-1, -1, 0)};
                    for (int i=0; i < 4; i++) {
                        std::cout << "corner " << glm::to_string(cdirs[i]) << " traversable: " << currentFace->cornerTraversable(currentFace->seminormalizedDirectionToCornerIndex(cdirs[i])) << std::endl;
                    }
                    std::cout << "firstCornerIndex: " << firstCornerIndex << std::endl;
                    std::cout << "secondCornerIndex: " << secondCornerIndex << std::endl;
                    std::cout << "firstCornerTraversable: " << firstCornerTraversable << std::endl;
                    std::cout << "secondCornerTraversable: " << secondCornerTraversable << std::endl;
                    std::cout << "deltaPos2: " << glm::to_string(deltaPos2) << std::endl;
                    std::cout << "firstSeminormalizedCornerDirection : " << glm::to_string(firstSeminormalizedCornerDirection) << std::endl;
                    std::cout << "secondSeminormalizedCornerDirection: " << glm::to_string(secondSeminormalizedCornerDirection) << std::endl;
                    std::cout << "edgeHitPosition: " << edgeHitPosition << std::endl;
                    std::cout << "(edgeHitPosition > -0.5f * (goptions::cubeSideLength - _playerSizes[otherDimIdx]) || firstCornerTraversable ): " << (edgeHitPosition > -0.5f * (goptions::cubeSideLength - _playerSizes[otherDimIdx]) || firstCornerTraversable ) << std::endl;
                    std::cout << "(edgeHitPosition <  0.5f * (goptions::cubeSideLength - _playerSizes[otherDimIdx]) || secondCornerTraversable): " << (edgeHitPosition <  0.5f * (goptions::cubeSideLength - _playerSizes[otherDimIdx]) || secondCornerTraversable) << std::endl;
                    exit(0);
                }
                if (ratiosUntilFaceChange[dimIdx] < 0.0f || ratiosUntilEdgeHit[dimIdx] < 0.0f && !edgeAndCornerTraversable[dimIdx]) {
                    invalidValues = true;
                }
                if (1) {
                    std::cout << "--------" << std::endl;
                    std::cout << "ratiosUntilFaceChange[" << dimIdx << "]: " << ratiosUntilFaceChange[dimIdx] << std::endl;
                    std::cout << "ratiosUntilEdgeHit[" << dimIdx << "]: " << ratiosUntilEdgeHit[dimIdx] << std::endl;
                    std::cout << "characterThinningFactor: " << characterThinningFactor << std::endl;
                    std::cout << "edgeAndCornerTraversable[" << dimIdx << "]: " << edgeAndCornerTraversable[dimIdx] << std::endl;
                    std::cout << "ratiosUntilEdgeHit[" << dimIdx << "]: " << ratiosUntilEdgeHit[dimIdx] << std::endl;
                }
            }
            else {
                ratiosUntilFaceChange[dimIdx] = greaterThanOne;
                ratiosUntilEdgeHit[dimIdx] = greaterThanOne;
                edgeAndCornerTraversable[dimIdx] = true;
            }
            intersectingEdge[dimIdx] = 0.5f * goptions::cubeSideLength - (std::abs(glm::dot(currentMidPos - currentCubePos, currentPlayerDimensionDirections[dimIdx])) + 0.5f * _playerSizes[dimIdx]) < 0;
        }
        if (1) {
            std::cout << "--------" << std::endl;
            std::cout << "_playerSizes: " << glm::to_string(glm::vec2(_playerSizes[0], _playerSizes[1])) << std::endl;
        }
        if (invalidValues) {
            std::cout << "ERROR: Invalid ratio detected (1)!" << std::endl;
            exit(0);
        }

        int minRatioUntilUntraversableEdgeHitPlaneDimIdx = 0;
        int minRatioUntilFaceChangePlaneDimIdx = 0;
        int maxRatioUntilEdgeHitPlaneDimIdx = 0;
        for (int dimIdx = 1; dimIdx < goptions::numPlaneDimensions; dimIdx++) {
            if (edgeAndCornerTraversable[dimIdx] < edgeAndCornerTraversable[minRatioUntilUntraversableEdgeHitPlaneDimIdx] || edgeAndCornerTraversable[dimIdx] == edgeAndCornerTraversable[minRatioUntilUntraversableEdgeHitPlaneDimIdx] && ratiosUntilEdgeHit[dimIdx] < ratiosUntilEdgeHit[minRatioUntilUntraversableEdgeHitPlaneDimIdx]) {
                minRatioUntilUntraversableEdgeHitPlaneDimIdx = dimIdx;
            }
            if (!intersectingEdge[dimIdx] && intersectingEdge[maxRatioUntilEdgeHitPlaneDimIdx] || ratiosUntilEdgeHit[dimIdx] > ratiosUntilEdgeHit[maxRatioUntilEdgeHitPlaneDimIdx]) {
                maxRatioUntilEdgeHitPlaneDimIdx = dimIdx;
            }
            if (ratiosUntilFaceChange[dimIdx] < ratiosUntilFaceChange[minRatioUntilFaceChangePlaneDimIdx]) {
                minRatioUntilFaceChangePlaneDimIdx = dimIdx;
            }
        }
        std::cout << "minRatioUntilFaceChangePlaneDimIdx: " << minRatioUntilFaceChangePlaneDimIdx << std::endl;
        std::cout << "minRatioUntilUntraversableEdgeHitPlaneDimIdx: " << minRatioUntilUntraversableEdgeHitPlaneDimIdx << std::endl;

        // Calculate what percentage of deltaPosition should be used during this iteration
        float deltaPositionRatio = 1.0f;
        if (ratiosUntilEdgeHit[minRatioUntilUntraversableEdgeHitPlaneDimIdx] < std::min(1.0f, ratiosUntilFaceChange[minRatioUntilFaceChangePlaneDimIdx]) && !edgeAndCornerTraversable[minRatioUntilUntraversableEdgeHitPlaneDimIdx]) {
            // Hitting a non-traversable edge
            deltaPositionRatio = ratiosUntilEdgeHit[minRatioUntilUntraversableEdgeHitPlaneDimIdx];

            if (deltaPositionRatio < 0.0f) {
                std::cout << "Negative deltaPositionRatio 1: " << deltaPositionRatio << std::endl;
                exit(0);
            }

            // Stop velocity in the direction of the edge. Model the edge as frictionless and only remove the component of the velocity that is orthogonal to it
            size_t directionDimIdx = direction3ToDimensionIndex(currentPlayerDimensionDirections[minRatioUntilUntraversableEdgeHitPlaneDimIdx]);
            currentVelocity[directionDimIdx] = 0.0f;

            if(minRatioUntilUntraversableEdgeHitPlaneDimIdx == VERTICAL && deltaPos2[VERTICAL] < 0.0f)
            {
                if (_state == FALLING) {
                    _state = IDLE;
                }
            }
        }
        else if (ratiosUntilFaceChange[minRatioUntilFaceChangePlaneDimIdx] < 1) {
            // Hitting a traversable edge => uppdate current face
            deltaPositionRatio = ratiosUntilFaceChange[minRatioUntilFaceChangePlaneDimIdx];

            if (deltaPositionRatio < 0.0f) {
                std::cout << "Negative deltaPositionRatio 2: " << deltaPositionRatio << std::endl;
                exit(0);
            }

            glm::ivec3 oldNormal = currentFace->normal();
            // Update current face
            currentFace = neighbors[minRatioUntilFaceChangePlaneDimIdx];
            glm::ivec3 newNormal = currentFace->normal();

            if (newNormal != oldNormal) {
                // Rotate velocity
                currentVelocity = rotatedVecor(currentVelocity, oldNormal, newNormal);
                // Rotate player dimension directions
                for (int dimIdx = 0; dimIdx < goptions::numDimensions; dimIdx++) {
                    currentPlayerDimensionDirections[dimIdx] = rotatedVecor(currentPlayerDimensionDirections[dimIdx], oldNormal, newNormal);
                }
            }
        }

        if (deltaPositionRatio < 0.0f) {
            std::cout << "ERROR: Invalid ratio detected (2)!" << std::endl;
            exit(0);
        }

        // Move current position
        std::cout << "---------------------------------------------------" << std::endl;
        std::cout << "currentMidPos before update: " << glm::to_string(currentMidPos) << std::endl;
        std::cout << "deltaPositionRatio: " << deltaPositionRatio << std::endl;
        std::cout << "deltaPosition: " << glm::to_string(deltaPosition) << std::endl;
        currentMidPos += deltaPositionRatio * deltaPosition;
        std::cout << "currentMidPos after update: " << glm::to_string(currentMidPos) << std::endl;
        // Update time left
        dt = (1.0f - deltaPositionRatio) * dt;
    }

    glm::vec3 newPosition = playerMidPosToPos(currentMidPos, currentPlayerDimensionDirections[VERTICAL]);
    size_t normalDirectionDimIdx = direction3ToDimensionIndex(currentPlayerDimensionDirections[NORMAL]);
    int normalDirectionSign = currentPlayerDimensionDirections[NORMAL][normalDirectionDimIdx];
    // Move newPosition to a face if it's not already at a face
    newPosition[normalDirectionDimIdx] = currentFace->facePosToCubeIPos(newPosition, currentPlayerDimensionDirections[NORMAL])[normalDirectionDimIdx] * goptions::cubeSideLength + Cube::cubeOffset[normalDirectionDimIdx] + 0.5f * goptions::cubeSideLength * normalDirectionSign;

    // Update all player attributes that changed during the loop
    _currentFace = currentFace;
    _sprite->setPosition(newPosition);
    _velocity = currentVelocity;
    _playerDimensionDirections[0] = currentPlayerDimensionDirections[0];
    _playerDimensionDirections[1] = currentPlayerDimensionDirections[1];
    _playerDimensionDirections[2] = currentPlayerDimensionDirections[2];
}

void PlayerControl::setFaceMap(FaceMap* faces, glm::vec3 playerPos)
{
    _faces = faces;
    glm::ivec3 currentCubeIPos = playerFaceCubeIPos(position(), _playerDimensionDirections[VERTICAL], _playerDimensionDirections[NORMAL]);

    auto faceIt = _faces->find(FaceMapKey(currentCubeIPos, _playerDimensionDirections[NORMAL]));
    assert(faceIt != _faces->end());
    _currentFace = faceIt->second;
}

void PlayerControl::increaseSpeedComponent(size_t dim, float increase)
{
    _velocity += increase * _playerDimensionDirections[dim];
}

void PlayerControl::increaseSpeedRight(float increase)
{
    increaseSpeedComponent(HORIZONTAL, increase);
}

void PlayerControl::increaseSpeedUp(float increase)
{
    increaseSpeedComponent(VERTICAL, increase);
}

float PlayerControl::speedComponent(size_t dim) const
{
    return glm::dot(_velocity, _playerDimensionDirections[dim]);
}

float PlayerControl::speedRight() const
{
    return speedComponent(HORIZONTAL);
}

float PlayerControl::speedUp() const
{
    return speedComponent(VERTICAL);
}

void PlayerControl::setSpeedComponent(size_t dim, float speed)
{
    _velocity += (speed - speedComponent(dim)) * _playerDimensionDirections[dim];
}

void PlayerControl::setSpeedRight(float speed)
{
    setSpeedComponent(HORIZONTAL, speed);
}

void PlayerControl::setSpeedUp(float speed)
{
    setSpeedComponent(VERTICAL, speed);
}

glm::vec3 PlayerControl::playerPosToMidPos(glm::vec3 playerPos, glm::vec3 playerUpDirection)
{
    return playerPos + 0.5f * goptions::characterHeight * playerUpDirection;
}

glm::vec3 PlayerControl::playerMidPosToPos(glm::vec3 playerMidPos, glm::vec3 playerUpDirection)
{
    return playerMidPos - 0.5f * goptions::characterHeight * playerUpDirection;
}

glm::ivec3 PlayerControl::playerFaceCubeIPos(glm::vec3 playerPos, glm::vec3 playerUpDirection, glm::vec3 playerNormal)
{
    return Face::facePosToCubeIPos(playerPosToMidPos(playerPos, playerUpDirection), playerNormal);
}

const float PlayerControl::_playerSizes[goptions::numPlaneDimensions]{
    goptions::characterWidth,   // Horizontal dimension
    goptions::characterHeight}; // Vertical dimension
