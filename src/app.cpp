#include "app.h"
#include "goptions.h"
#include "sprite.h"
#include "utility.h"
#include "gltext.h"

#define __DEBUG_TEXT(text, x, y) _textRenderer.createText(AssetFactory::getFonts()->righteousFont, "")->setPosition(glm::vec3{x, y, 0.0f})->setUpdateCallback([this](lithium::Updateable* u, float time, float dt) { \
        static int lastUpdate = 0.0f; \
        if(time - lastUpdate > 0.2f) \
        { \
            dynamic_cast<lithium::Text*>(u)->setText(text); \
            lastUpdate = time; \
        } \
        return true; \
    });

App::App() :
    Application{"lithium-lab", glm::ivec2{1920, 1080}, lithium::Application::Mode::MULTISAMPLED_4X, false},
    _rotationGraph{},
    _textRenderer{defaultFrameBufferResolution()}
{
    AssetFactory::loadMeshes();
    AssetFactory::loadTextures();
    AssetFactory::loadObjects();
    AssetFactory::loadFonts();
    _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());

    auto spriteMesh = std::shared_ptr<lithium::Mesh>(
        new lithium::Mesh({lithium::VertexArrayBuffer::AttributeType::VEC3},
        {0.0f, 0.0f, 0.0f})
    );
    spriteMesh->setDrawMode(GL_POINTS);

    auto playerSprite = std::make_shared<Sprite>(spriteMesh,
        TextureArray{AssetFactory::getTextures()->delivermanSheet}, glm::ivec2{96, 96});
    playerSprite->setPosition(glm::vec3{6.99f, 0.0f, 1.0f});
    playerSprite->setScale(2.0f);
    playerSprite->setFramesPerSecond(12.0f);
    playerSprite->createAnimation("idle", {0});
    playerSprite->createAnimation("walk", {0});
    playerSprite->setAnimation("idle");
    playerSprite->setZIndex(-0.01f);
    playerSprite->setGroupId(Pipeline::PLAYER);
    _pipeline->attach(playerSprite.get());
    playerSprite->stage();


    input()->addPressedCallback(GLFW_KEY_Q, [this, playerSprite](int key, int mods) {
        playerSprite->setAlpha(playerSprite->alpha() - 0.1f);
        return true;
    });

    input()->addPressedCallback(GLFW_KEY_E, [this, playerSprite](int key, int mods) {
        playerSprite->setAlpha(playerSprite->alpha() + 0.1f);
        return true;
    });

    input()->addPressedCallback(GLFW_KEY_ESCAPE, [this](int key, int mods) {
        this->close();
        return true;
    });

    createLevel1();

    _currentGameState = GameStateType::MAIN;

    _playerControl = std::make_unique<PlayerControl>(playerSprite, input());
    _playerControl->setFaceMap(&_rotationGraph.faces(), playerSprite->position());

    // For turning the camera in debugging purpose
    input()->addPressedCallback(GLFW_KEY_LEFT, [this](int key, int mods) {
        _cameraViewAngle += glm::pi<float>() * 0.5f; 
        return true;
    });
    input()->addPressedCallback(GLFW_KEY_RIGHT, [this](int key, int mods) {
        _cameraViewAngle -= glm::pi<float>() * 0.5f; 
        return true;
    });

    float paddX = 16.0f;
    float paddY = 32.0f;
    float margY = 48.0f;
    float startX = -defaultFrameBufferResolution().x * 0.5f;
    float startY = defaultFrameBufferResolution().y * 0.5f;
    int debugTexts = 0;

    // Debug prints use them for all ur debugging needs
    __DEBUG_TEXT("position: " + utility::vectorToString(_playerControl->position()), startX + paddX, startY - paddY - margY * debugTexts++);
    __DEBUG_TEXT("view angle: " + std::to_string(glm::degrees(_cameraViewAngle)), startX + paddX, startY - paddY - margY * debugTexts++);
    __DEBUG_TEXT("right vector: utility::vectorToString(_playerControl->???)", startX + paddX, startY - paddY - margY * debugTexts++);
    __DEBUG_TEXT(std::string{"side: "} + "(_leftSide ? \"left\" : \"right\")", startX + paddX, startY - paddY - margY * debugTexts++);

    AssetFactory::getTextures()->noiseTexture->bind(GL_TEXTURE1);

    printf("%s\n", glGetString(GL_VERSION));
}

App::~App() noexcept
{
    _pipeline = nullptr;
}

void App::update(float dt)
{
    lithium::Application::update(dt);

    currentGameState()->second.update(dt);

    handleStateTransitions();

    static float alpha = _cameraViewAngle;
    alpha = glm::mix(alpha, _cameraViewAngle, dt * 4.0f);
    auto camera = _pipeline->camera();
    float camX = cos(alpha) * goptions::cameraRadius;
    float camZ = sin(alpha) * goptions::cameraRadius;
    float camY = goptions::cameraRadius * 0.1f;
    glm::vec3 target = _playerControl->position() + glm::vec3{0.0f, camY, 0.0f};

    camera->setTarget(glm::mix(camera->target(), target, dt * 1.5f));
    camera->setPosition(camera->target() + glm::vec3{camX, 0.0f, camZ});

    _playerControl->update(dt);


    _pipeline->setTime(time());
    _pipeline->render();
    _textRenderer.update(dt);
    //_textRenderer.render();
}

void App::handleStateTransitions()
{
    if(!_stateTransitions.empty())
    {
        GameStateType nextState = *_stateTransitions.begin();
        currentGameState()->second.exit(_gameStates.at(nextState));
        _currentGameState = nextState;
        _stateTransitions.erase(_stateTransitions.begin());
        currentGameState()->second.enter();
        //_pipeline->updatePointLights();
    }
}

void App::constructRotationGraph(std::vector<lithium::Node*>& nodes)
{
    std::vector<Cube*> cubes;
    for (auto& node : nodes) {
        cubes.push_back(new Cube(node->position()));
    }
    _rotationGraph.construct(cubes);
}
