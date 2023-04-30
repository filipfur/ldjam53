#pragma once

#include "globject.h"

class Sprite : public lithium::Object
{
public:
    Sprite(std::shared_ptr<lithium::Mesh> mesh, const std::vector<lithium::Object::TexturePointer>& texture);
    Sprite(const Sprite& other);
    Sprite(const Object& other);
    virtual ~Sprite() noexcept;

    virtual void update(float dt) override;

    virtual void shade(lithium::ShaderProgram* shaderProgram) override;

    virtual Sprite* clone() const override
    {
        return new Sprite(*this);
    }

    void setSpriteDimension(const glm::ivec2& spriteDimension)
    {
        _spriteDimension = spriteDimension;
        _regionDimension = glm::vec2(spriteDimension) / glm::vec2(_textures[0]->width(), _textures[0]->height());
        _framesXY = glm::ivec2(_textures[0]->width(), _textures[0]->height()) / spriteDimension;
        // print all three variables
        std::cout << "spriteDimension: " << _spriteDimension.x << ", " << _spriteDimension.y << std::endl;
        std::cout << "regionDimension: " << _regionDimension.x << ", " << _regionDimension.y << std::endl;
        std::cout << "framesXY: " << _framesXY.x << ", " << _framesXY.y << std::endl;

    }

    void setRegionPosition(const glm::vec2& regionPosition)
    {
        _regionPosition = regionPosition;
    }

    const glm::vec2& regionDimension() const
    {
        return _regionDimension;
    }

    const glm::vec2& regionPosition() const
    {
        return _regionPosition;
    }

    void setFramesPerSecond(float framesPerSecond)
    {
        _framesPerSecond = framesPerSecond;
        _frameInterval = 1.0f / _framesPerSecond;
        _frameDuration = _frameInterval;
    }

private:
    glm::ivec2 _spriteDimension;
    glm::vec2 _regionDimension{1.0f, 1.0f};
    glm::vec2 _regionPosition{0.0f, 0.0f};
    glm::ivec2 _framesXY;
    int _currentFrame{0};
    float _framesPerSecond;
    float _frameInterval{0.0f};
    float _frameDuration{0.0f};
};