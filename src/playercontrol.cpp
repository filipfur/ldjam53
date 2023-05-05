#include "playercontrol.h"

#include "goptions.h"

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
    bool grounded = onGround();
    if(grounded)
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
        setSpeedUp(0);
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
        increaseSpeedUp(-goptions::gravity * dt);

        setSpeedRight(glm::mix(speedRight(), 0.0f, dt * 0.3f));
    }

    move(_velocity * dt);

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

void PlayerControl::move(glm::vec3 deltaPosition)
{
    _sprite->setPosition(_sprite->position() + deltaPosition);
}

void PlayerControl::setFaceMap(FaceMap* faces, glm::vec3 playerPos)
{
    _faces = faces;
}

void PlayerControl::increaseSpeedRight(float increase)
{
    _velocity += increase * _rightDirection;
}

void PlayerControl::increaseSpeedUp(float increase)
{
    _velocity += increase * _upDirection;
}

float PlayerControl::speedRight() const
{
    return glm::dot(_velocity, _rightDirection);
}

float PlayerControl::speedUp() const
{
    return glm::dot(_velocity, _upDirection);
}

void PlayerControl::setSpeedRight(float speed)
{
    _velocity += (speed - speedRight()) * _rightDirection;
}

void PlayerControl::setSpeedUp(float speed)
{
    _velocity += (speed - speedUp()) * _upDirection;
}
