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
    void setFaceMap(FaceMap* faces, glm::vec3 playerPos);

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
    glm::vec3 _delta{0.0f};
    FaceMap* _faces;
    float _jumpDuration{0.0f};
    State _state{IDLE};
    Direction _direction{RIGHT};
};