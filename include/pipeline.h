#pragma once

#include <memory>
#include "glsimplecamera.h"
#include "glrenderpipeline.h"
#include "glframebuffer.h"

class Pipeline : public lithium::RenderPipeline
{
public:
    enum Group
    {
        NONE,
        PLAYER,
        CURRENT_BLOCK
    };

    Pipeline(const glm::ivec2& resolution);

    ~Pipeline() noexcept;

    std::shared_ptr<lithium::SimpleCamera> camera()
    {
        return _camera;
    }

    void setTime(float time)
    {
        _time = time;
    }

    void setOrientation(const glm::vec3& rightVector, bool leftSide)
    {
        _rightVector = rightVector;
        _rightPerpVector = glm::cross(_rightVector, glm::vec3(0.0f, 1.0f, 0.0f));
        _leftSide = leftSide;
    }

private:
    std::shared_ptr<lithium::ShaderProgram> _blockShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _normalShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _normalInstShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _spriteShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _instanceShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _msaaShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _screenShader{nullptr};
    std::shared_ptr<lithium::SimpleCamera> _camera{nullptr};
    
    std::shared_ptr<lithium::RenderGroup> _instanceGroup;
    std::shared_ptr<lithium::RenderGroup> _playerGroup;
    std::shared_ptr<lithium::RenderGroup> _spriteGroup;
    std::shared_ptr<lithium::RenderGroup> _currentBlockGroup;
    std::shared_ptr<lithium::RenderGroup> _mainGroup;

    std::shared_ptr<lithium::RenderStage> _mainStage;
    std::shared_ptr<lithium::RenderStage> _finalStage;

    std::shared_ptr<lithium::FrameBuffer> _frameBuffer;

    glm::vec3 _normalA{0.0f, 0.0f, 1.0f};
    glm::vec3 _normalB{1.0f, 0.0f, 0.0f};
    bool _leftSide{true};
    glm::vec3 _rightVector{1.0f, 0.0f, 0.0f};
    glm::vec3 _rightPerpVector{0.0f, 0.0f, 1.0f};

    float _time{0.0f};
};