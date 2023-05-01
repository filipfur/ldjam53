#include "pipeline.h"
#include "assetfactory.h"
#include "glinstancedobject.h"
#include "sprite.h"

Pipeline::Pipeline(const glm::ivec2& resolution) : lithium::RenderPipeline{resolution},
    _camera{new lithium::SimpleCamera(glm::perspective(glm::radians(45.0f), (float)resolution.x / (float)resolution.y, 0.1f, 100.0f))},
    _frameBuffer{std::make_shared<lithium::FrameBuffer>(resolution)}
{
    enableDepthTesting();
    enableStencilTesting();
    enableBlending();
    blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    enableMultisampling();
    enableFaceCulling();
    stencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    _blockShader = std::make_shared<lithium::ShaderProgram>("shaders/object.vert", "shaders/object.frag");
    _blockShader->setUniform("u_texture_0", 0);
    _blockShader->setUniform("u_projection", _camera->projection());

    _spriteShader = std::make_shared<lithium::ShaderProgram>("shaders/object.vert", "shaders/sprite.frag");
    _spriteShader->setUniform("u_texture_0", 0);
    _spriteShader->setUniform("u_projection", _camera->projection());

    _normalShader = std::make_shared<lithium::ShaderProgram>("shaders/instance.vert", "shaders/normal.frag");
    _normalShader->setUniform("u_projection", _camera->projection());

    _instanceShader = std::make_shared<lithium::ShaderProgram>("shaders/instance.vert", "shaders/object.frag");
    _instanceShader->setUniform("u_texture_0", 0);
    _instanceShader->setUniform("u_projection", _camera->projection());

    _screenShader = std::make_shared<lithium::ShaderProgram>("shaders/screenshader.vert", "shaders/screenshader.frag");
    _msaaShader = std::make_shared<lithium::ShaderProgram>("shaders/screenshader.vert", "shaders/msaa.frag");
    _msaaShader->setUniform("u_texture", 0);
    _camera->setPosition(glm::vec3{30.0f, 30.0f, 30.0f});
    _camera->setTarget(glm::vec3{0.0f});

    _frameBuffer->bind();
    _frameBuffer->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_TEXTURE_2D_MULTISAMPLE);
    _frameBuffer->createTexture(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8, GL_FLOAT, GL_TEXTURE_2D_MULTISAMPLE);
    _frameBuffer->declareBuffers();
    _frameBuffer->unbind();

    _instanceGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return dynamic_cast<lithium::InstancedObject<glm::mat4>*>(renderable);
    });

    _spriteGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return dynamic_cast<Sprite*>(renderable);
    });

    _mainGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return !renderable->hasAttachments();
    });

    _mainStage = addRenderStage(std::make_shared<lithium::RenderStage>(_frameBuffer, glm::ivec4{0, 0, resolution.x, resolution.y}, [this](){
        clearColor(0.0f, 0.0f, 0.0f, 1.0f);
        clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        _screenShader->use();
        AssetFactory().getMeshes()->screen->bind();
        glActiveTexture(GL_TEXTURE0);
        disableDepthWriting();
        AssetFactory().getMeshes()->screen->draw();
        enableDepthWriting();

        _blockShader->setUniform("u_view", _camera->view());
        _mainGroup->render(_blockShader.get());

        _instanceShader->setUniform("u_view", _camera->view());
        _instanceGroup->render(_instanceShader.get());

        _normalShader->setUniform("u_view", _camera->view());
        clear(GL_DEPTH_BUFFER_BIT);
        enableStencilWriting();

        const glm::vec3 normalA{0.0f, 0.0f, 1.0f};
        const glm::vec3 normalB{1.0f, 0.0f, 0.0f};

        disableColorWriting();
            _normalShader->setUniform("u_face_normal", normalA);
            stencilFunc(GL_ALWAYS, 1, 0xFF);
            _instanceGroup->render(_normalShader.get());

            _normalShader->setUniform("u_face_normal", normalB);
            stencilFunc(GL_ALWAYS, 2, 0xFF);
            _instanceGroup->render(_normalShader.get());
        enableColorWriting();


        stencilFunc(GL_EQUAL, 1, 0xFF);
        _spriteShader->setUniform("u_view", _camera->view());
        _spriteShader->setUniform("u_time", _time);
        _spriteGroup->render(_spriteShader.get());

        stencilFunc(GL_EQUAL, 2, 0xFF);
        glm::vec3 oldSpritePos;
        _spriteGroup->forEach([&oldSpritePos, &normalA, &normalB](lithium::Renderable* renderable){
            auto sprite = dynamic_cast<Sprite*>(renderable);
            oldSpritePos = sprite->position();

            glm::ivec3 ivec{oldSpritePos};
            glm::vec3 npos = glm::vec3(ivec.x, oldSpritePos.y, ivec.z) + normalB * 1.01f;
            float len = glm::length(npos - oldSpritePos);
            sprite->setPosition(npos + normalA * len);

            sprite->setRotation(sprite->rotation() + glm::vec3{0.0f, 90.0f, 0.0f});
        });
        _spriteGroup->render(_spriteShader.get());
        _spriteGroup->forEach([&oldSpritePos](lithium::Renderable* renderable){
            auto sprite = dynamic_cast<Sprite*>(renderable);
            sprite->setPosition(oldSpritePos);
            sprite->setRotation(sprite->rotation() - glm::vec3{0.0f, 90.0f, 0.0f});
        });

        stencilFunc(GL_ALWAYS, 0, 0xFF);
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