#pragma once

#include <memory>
#include "sprite.h"
#include "glinput.h"
#include "rotationgraph.h"

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

    PlayerControl(std::shared_ptr<Sprite> sprite, std::shared_ptr<lithium::Input> input);

    virtual void update(float dt) override;
    void move(glm::vec3 deltaPosition);
    void setFaceMap(FaceMap* faces, glm::vec3 playerPos);
    void increaseSpeedRight(float increase);
    void increaseSpeedUp(float increase);
    float speedRight() const;
    float speedUp() const;
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

private:
    std::shared_ptr<Sprite> _sprite;
    std::shared_ptr<lithium::Input::KeyCache> _keyCache;
    glm::vec3 _rightDirection{1.0f, 0.0f, 0.0f};
    glm::vec3 _upDirection{0.0f, 1.0f, 0.0f};
    glm::vec3 _velocity{0.0f};
    FaceMap* _faces{};
    float _jumpDuration{0.0f};
    State _state{IDLE};
    Direction _direction{RIGHT};
};