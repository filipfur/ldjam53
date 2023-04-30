#include "sprite.h"

Sprite::Sprite(std::shared_ptr<lithium::Mesh> mesh, const std::vector<lithium::Object::TexturePointer>& textures) : lithium::Object(mesh, textures)
{
    setFramesPerSecond(10.0f);
}

Sprite::Sprite(const Sprite& other) : lithium::Object(other)
{
    _regionDimension = other._regionDimension;
    _regionPosition = other._regionPosition;
}

Sprite::~Sprite() noexcept
{
}

void Sprite::shade(lithium::ShaderProgram* shaderProgram)
{
    lithium::Object::shade(shaderProgram);
    if(visible())
    {
        shaderProgram->setUniform("u_region_dimension", _regionDimension);
        shaderProgram->setUniform("u_region_position", _regionPosition);
    }
}

void Sprite::update(float dt)
{
    lithium::Object::update(dt);
    _frameDuration -= dt;
    if(_frameDuration <= 0)
    {
        _frameDuration = _frameInterval + _frameDuration;
        _regionPosition.x += _regionDimension.x;
        if(_regionPosition.x >= 1.0f)
        {
            _regionPosition.x = 0.0f;
            _regionPosition.y += _regionDimension.y;
            ++_currentFrame;
            if(_regionPosition.y >= 1.0f)
            {
                _regionPosition.y = 0.0f;
                _currentFrame = 0;
            }
        }
    }
}

