#include "sprite.h"
#include "goptions.h"

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
        shaderProgram->setUniform("u_z_index", _zIndex);
        shaderProgram->setUniform("u_alpha", _alpha);
        shaderProgram->setUniform("u_halfSideLength", 0.5f * goptions::cubeSideLength);
        shaderProgram->setUniform("u_creaseDir", _creaseDir);
        shaderProgram->setUniform("u_normal", _normal);
        shaderProgram->setUniform("u_normalTobendDirFactor", _normalTobendDirFactor);
        shaderProgram->setUniform("u_creaseDirPlaneDim", static_cast<int>(_creaseDirPlaneDim));
        shaderProgram->setUniform("u_creaseDirPlaneSign", _creaseDirPlaneSign);
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
