#include "sprite.h"

Sprite::Sprite(std::shared_ptr<lithium::Mesh> mesh, const std::vector<lithium::Object::TexturePointer>& textures, const glm::ivec2& spriteDimension) : lithium::Object(mesh, textures)
{
    setSpriteDimension(spriteDimension);
    setFramesPerSecond(10.0f);
}

Sprite::Sprite(const Sprite& other) : lithium::Object(other), _animations{other._animations}
{
    setSpriteDimension(other._spriteDimension);
    setFramesPerSecond(other._framesPerSecond);
    _currentAnimation = _animations.begin();
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
        shaderProgram->setUniform("u_flipped", _flipped);
    }
}

void Sprite::update(float dt)
{
    lithium::Object::update(dt);
    _frameDuration -= dt;
    if(_frameDuration <= 0)
    {
        _frameDuration = _frameInterval + _frameDuration;
        ++_currentFrame;
        _regionPosition = _currentAnimation->second.uvs[_currentFrame % _currentAnimation->second.uvs.size()];
    }
}

