#include "playercontrol.h"

#include "goptions.h"

PlayerControl::PlayerControl(std::shared_ptr<Sprite> sprite, std::shared_ptr<lithium::Input> input) : _sprite(sprite)
{
    _keyCache = std::make_shared<lithium::Input::KeyCache>(std::initializer_list<int>{GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D});
    input->setKeyCache(_keyCache);
}

void PlayerControl::update(float dt)
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


    _sprite->setPosition(_sprite->position() + _delta * dt);
}