#include "pipeline.h"
#include "assetfactory.h"
#include "glinstancedobject.h"
#include "sprite.h"

Pipeline::Pipeline(const glm::ivec2& resolution) : lithium::RenderPipeline{resolution},
    _camera{new lithium::SimpleCamera(glm::perspective(glm::radians(45.0f), (float)resolution.x / (float)resolution.y, 0.1f, 100.0f))},
    //_camera{new lithium::SimpleCamera(glm::ortho(-resolution.x * 0.005f, resolution.x * 0.005f, -resolution.y * 0.005f, resolution.y * 0.005f, 0.1f, 100.0f))},
    _frameBuffer{std::make_shared<lithium::FrameBuffer>(resolution)},
    _cameraUBO{std::make_shared<lithium::UniformBufferObject>(sizeof(glm::mat4) * 2 + sizeof(glm::vec3), "CameraUBO", 0)}
{
    enableDepthTesting();
    depthFunc(GL_LESS);
    enableBlending();
    blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    enableMultisampling();
    enableFaceCulling();
    stencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    _blockShader = std::make_shared<lithium::ShaderProgram>("shaders/object.vert", "shaders/object.frag");
    _blockShader->setUniform("u_texture_0", 0);
    _blockShader->setUniform("u_projection", _camera->projection());

    _spriteShader = std::make_shared<lithium::ShaderProgram>(
        "shaders/bend.vert", "shaders/object.frag", "shaders/bend.geom");
    _spriteShader->setUniform("u_texture_0", 0);
    _spriteShader->setUniform("u_projection", _camera->projection());
    _spriteShader->setUniform("u_up", glm::vec3{0.0f, 1.0f, 0.0f});
    _spriteShader->setUniform("u_right", glm::vec3{1.0f, 0.0f, 0.0f});

    _normalShader = std::make_shared<lithium::ShaderProgram>("shaders/instance.vert", "shaders/normal.frag");
    _normalShader->setUniform("u_projection", _camera->projection());

    _instanceShader = std::make_shared<lithium::ShaderProgram>("shaders/instance.vert", "shaders/object.frag");
    _instanceShader->setUniform("u_texture_0", 0);
    _instanceShader->setUniform("u_texture_1", 1);
    _instanceShader->setUniform("u_projection", _camera->projection(), true);

    _cubemapShader = std::make_shared<lithium::ShaderProgram>("shaders/cubemap.vert", "shaders/cubemap.frag");

    _screenShader = std::make_shared<lithium::ShaderProgram>("shaders/screenshader.vert", "shaders/screenshader.frag");
    _msaaShader = std::make_shared<lithium::ShaderProgram>("shaders/screenshader.vert", "shaders/msaa.frag");
    _msaaShader->setUniform("u_texture", 0);
    _camera->setPosition(glm::vec3{30.0f, 30.0f, 30.0f});
    _camera->setTarget(glm::vec3{0.0f});

    _cameraUBO->bindBufferBase({
        _cubemapShader.get()});

    _cameraUBO->bufferSubData(0, _camera->projection());

    _frameBuffer->bind();
    _frameBuffer->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_TEXTURE_2D_MULTISAMPLE);
    _frameBuffer->createTexture(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8, GL_FLOAT, GL_TEXTURE_2D_MULTISAMPLE);
    _frameBuffer->declareBuffers();
    _frameBuffer->unbind();

    _instanceGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return dynamic_cast<lithium::InstancedObject<glm::mat4>*>(renderable);
    });

    _skyboxGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return renderable->groupId() == SKYBOX;
    });

    _playerGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return dynamic_cast<Sprite*>(renderable) && renderable->groupId() == PLAYER;
    });

    _spriteGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return dynamic_cast<Sprite*>(renderable) && !renderable->hasAttachments();
    });

    _mainGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return !renderable->hasAttachments();
    });

    _mainStage = addRenderStage(std::make_shared<lithium::RenderStage>(_frameBuffer, glm::ivec4{0, 0, resolution.x, resolution.y}, [this](){
        _cameraUBO->bufferSubData(sizeof(glm::mat4), _camera->view());
        _cameraUBO->bufferSubData(sizeof(glm::mat4) * 2, _camera->position());
        clearColor(0.0f, 0.0f, 0.0f, 0.0f);
        clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //_screenShader->use();
        //AssetFactory().getMeshes()->screen->bind();
        //glActiveTexture(GL_TEXTURE0);
        //disableDepthWriting();
        //AssetFactory().getMeshes()->screen->draw();
        //enableDepthWriting();

        disableDepthWriting();
        _skyboxGroup->render(_cubemapShader);
        enableDepthWriting();

        _blockShader->setUniform("u_view", _camera->view());
        _mainGroup->render(_blockShader.get());

        _instanceShader->setUniform("u_view", _camera->view());
        _instanceShader->setUniform("u_time", _time);
        _instanceGroup->render(_instanceShader.get());

        _spriteShader->setUniform("u_view", _camera->view());
        _spriteShader->setUniform("u_time", _time);
        _spriteGroup->render(_spriteShader.get());
        _playerGroup->render(_spriteShader.get());
    }));

    _finalStage = addRenderStage(std::make_shared<lithium::RenderStage>(nullptr, glm::ivec4{0, 0, resolution.x, resolution.y}, [this](){
        clearColor(0.0f, 0.0f, 0.0f, 0.0f);
        clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _msaaShader->use();
        _mainStage->frameBuffer()->texture(GL_COLOR_ATTACHMENT0)->bind(GL_TEXTURE0);
        _mainStage->frameBuffer()->bindTexture(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE1);
        AssetFactory::getMeshes()->screen->bind();
        AssetFactory::getMeshes()->screen->draw();
    }));

    setViewportToResolution();
}

Pipeline::~Pipeline()
{
    _blockShader = nullptr;
    _screenShader = nullptr;
    _msaaShader = nullptr;
}