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
    void move(glm::vec3 deltaPosition);
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

    void setPosition(const glm::vec3& position)
    {
        _sprite->setPosition(position);
    }

    const glm::vec3& position() const
    {
        return _sprite->position();
    }

    void setRightDirection(const glm::vec3& rightDirection, bool goingLeft)
    {
        float speed = abs(_velocity.x == 0 ? _velocity.z : _velocity.x);
        if(goingLeft)
        {
            speed = -speed;
        }
        glm::vec3 vel = rightDirection * speed;
        _velocity.x = vel.x;
        _velocity.z = vel.z;
        _rightDirection = rightDirection;
        _rightOrthogonal = glm::cross(_rightDirection, _upDirection);
    }

    void land()
    {
        if(_state == FALLING)
        {
            auto pos = position();
            pos.y = glm::round(pos.y);
            setPosition(pos);
            setState(State::IDLE);
        }
    }

    void fall()
    {
        if(_state != JUMPING)
        {
            setState(State::FALLING);
        }
    }

    const glm::vec3& rightDirection() const
    {
        return _rightDirection;
    }

    const glm::vec3& rightOrthogonal() const
    {
        return _rightOrthogonal;
    }

    std::shared_ptr<Sprite> sprite() const
    {
        return _sprite;
    }

    void bumpHead()
    {
        _sprite->setPosition(_sprite->position() - glm::vec3{0.0f, 0.02f, 0.0f});
        _velocity.y = 0.0f;
    }

    void setState(State state)
    {
        if(state != _state)
        {
            _state = state;
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
    }

private:
    std::shared_ptr<Sprite> _sprite;
    std::shared_ptr<lithium::Input::KeyCache> _keyCache;
    glm::vec3 _rightDirection{1.0f, 0.0f, 0.0f};
    glm::vec3 _upDirection{0.0f, 1.0f, 0.0f};
    glm::vec3 _velocity{0.0f};
    glm::vec3 _rightOrthogonal{0.0f, 0.0f, 1.0f};
    float _jumpDuration{0.0f};
    State _state{IDLE};
    Direction _direction{RIGHT};
};