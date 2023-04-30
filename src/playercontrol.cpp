#include "playercontrol.h"

#include "goptions.h"

PlayerControl::PlayerControl(std::shared_ptr<Sprite> sprite, std::shared_ptr<lithium::Input> input) : _sprite(sprite)
{
    _keyCache = std::make_shared<lithium::Input::KeyCache>(std::initializer_list<int>{GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE});
    input->setKeyCache(_keyCache);
}

void PlayerControl::update(float dt)
{
    State oldState = _state;
    bool grounded = onGround();
    if(grounded)
    {
        if(_keyCache->isPressed(GLFW_KEY_A) && (_direction == LEFT || _state == IDLE))
        {
            _delta.x += -1.0f * goptions::playerWalkAcceleration * dt;
            _delta.x = std::max(_delta.x, -goptions::playerWalkSpeedMax);
            _state = WALKING;
            _direction = LEFT;
        }
        else if(_keyCache->isPressed(GLFW_KEY_D) && (_direction == RIGHT || _state == IDLE))
        {
            _delta.x += 1.0f * goptions::playerWalkAcceleration * dt;
            _delta.x = std::min(_delta.x, goptions::playerWalkSpeedMax);
            _state = WALKING;
            _direction = RIGHT;
        }
        else
        {
            _delta.x *= 0.8f;
            if(_delta.x * _delta.x < 1)
            {
                _delta.x = 0;
                _state = IDLE;
            }
        }
        _delta.y = 0;
        if(_keyCache->isPressed(GLFW_KEY_SPACE))
        {
            _state = JUMPING;
            _jumpDuration = goptions::playerJumpTime;
            _delta.x *= 0.9f;
        }
    }
    else
    {
        if(_state == JUMPING)
        {
            _delta.y += goptions::playerJumpAcceleration * dt;
            _jumpDuration -= dt;
            if(_jumpDuration <= 0)
            {
                _state = FALLING;
            }
        }
        _delta.y -= goptions::gravity * dt;

        _delta.x = glm::mix(_delta.x, 0.0f, dt * 0.3f);
    }

    _sprite->setPosition(_sprite->position() + _delta * dt);

    if(!grounded)
    {
        if(_sprite->position().y <= 0)
        {
            _sprite->setPosition(glm::vec3(_sprite->position().x, 0.0, _sprite->position().z));
            _state = IDLE;
        }
    }

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