#pragma once

#include <memory>
#include "sprite.h"
#include "glinput.h"

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

private:
    std::shared_ptr<Sprite> _sprite;
    std::shared_ptr<lithium::Input::KeyCache> _keyCache;
    glm::vec3 _delta{0.0f};
    State _state{IDLE};
    Direction _direction{RIGHT};
};