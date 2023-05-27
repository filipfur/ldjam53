#pragma once

#include <memory>
#include "sprite.h"
#include "glinput.h"
#include "rotationgraph.h"

#include "aadirection.h"

class PlayerControl : public lithium::Updateable
{
public:
    enum State
    {
        IDLE,
        WALKING,
        JUMPING,
        FALLING
    };

    enum Direction
    {
        LEFT,
        RIGHT
    };

    enum PlayerDimension
    {
        HORIZONTAL,
        VERTICAL,
        NORMAL,
        NUM_PLAYER_DIMENSIONS,
    };

    PlayerControl(std::shared_ptr<Sprite> sprite, std::shared_ptr<lithium::Input> input);

    virtual void update(float dt) override;
    void move(float dt);
    void setFaceMap(FaceMap* faces, glm::vec3 playerPos);
    void increaseSpeedComponent(size_t dim, float increase);
    void increaseSpeedRight(float increase);
    void increaseSpeedUp(float increase);
    float speedComponent(size_t dim) const;
    float speedRight() const;
    float speedUp() const;
    void setSpeedComponent(size_t dim, float speed);
    void setSpeedRight(float speed);
    void setSpeedUp(float speed);

    bool onGround()
    {
        return _state != JUMPING && _state != FALLING;
    }

    const glm::vec3& position() const
    {
        return _sprite->position();
    }

    static glm::vec3 playerPosToMidPos(glm::vec3 playerPos, AADirection3 playerUpDirection);
    static glm::vec3 playerMidPosToPos(glm::vec3 playerMidPos, AADirection3 playerUpDirection);
    static glm::ivec3 playerFaceCubeIPos(glm::vec3 playerPos, AADirection3 playerUpDirection, AADirection3 playerNormal);

private:
    std::shared_ptr<Sprite> _sprite;
    std::shared_ptr<lithium::Input::KeyCache> _keyCache;
    Face* _currentFace{nullptr};
    AADirection3 _playerDimensionDirections[goptions::numDimensions]{
        AADirection3(0, 1),  // Right
        AADirection3(1, 1),  // Up
        AADirection3(2, 1)}; // Normal
    glm::vec3 _velocity{0.0f};
    FaceMap* _faces{};
    float _jumpDuration{0.0f};
    State _state{IDLE};
    Direction _direction{RIGHT};

    static const float _playerSizes[goptions::numPlaneDimensions];
};