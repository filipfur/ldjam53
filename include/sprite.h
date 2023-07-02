#pragma once

#include "globject.h"
#include "glanimation.h"

class Sprite : public lithium::Object
{
    struct Animation
    {
        std::vector<glm::vec2> uvs;
    };

public:
    Sprite(std::shared_ptr<lithium::Mesh> mesh, const std::vector<lithium::Object::TexturePointer>& texture, const glm::ivec2& spriteDimension);
    Sprite(const Sprite& other);
    virtual ~Sprite() noexcept;

    virtual void update(float dt) override;

    virtual void shade(lithium::ShaderProgram* shaderProgram) override;

    void createAnimation(const std::string& name, const std::initializer_list<int>& frameIds)
    {
        Animation animation;
        for(int frameId : frameIds)
        {
            glm::ivec2 frameXY{frameId % _framesXY.x, frameId / _framesXY.x};
            glm::vec2 uvPosition{frameXY.x * _regionDimension.x, frameXY.y * _regionDimension.y};
            animation.uvs.push_back(uvPosition);
        }
        _animations[name] = animation;
        _currentAnimation = _animations.begin();
    }

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

    void setFlipped(bool flipped)
    {
        _flipped = flipped;
    }

    void setAnimation(const std::string& animationName)
    {
        if(_currentAnimation->first == animationName)
        {
            return;
        }
        _currentAnimation = _animations.find(animationName);
        _currentFrame = 0;
    }

    std::string animationName() const
    {
        return _currentAnimation->first;
    }

    void setZIndex(float zIndex)
    {
        _zIndex = zIndex;
    }

    void setAlpha(float alpha)
    {
        _alpha = alpha;
    }

    float alpha() const
    {
        return _alpha;
    }

private:
    std::map<std::string, Animation> _animations;
    std::map<std::string, Animation>::iterator _currentAnimation;
    glm::ivec2 _spriteDimension;
    glm::vec2 _regionDimension{1.0f, 1.0f};
    glm::vec2 _regionPosition{0.0f, 0.0f};
    glm::ivec2 _framesXY;
    int _currentFrame{0};
    float _framesPerSecond;
    float _frameInterval{0.0f};
    float _frameDuration{0.0f};
    float _zIndex{0.0f};
    bool _flipped{false};
    float _alpha{0.0f};
};