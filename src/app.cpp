#include "app.h"
#include "goptions.h"
#include "sprite.h"

App::App() : Application{"lithium-lab", glm::ivec2{1440, 800}, lithium::Application::Mode::MULTISAMPLED_4X, false}
{
    AssetFactory::loadMeshes();
    AssetFactory::loadTextures();
    AssetFactory::loadObjects();
    AssetFactory::loadFonts();
    _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());

    auto playerSprite = std::make_shared<Sprite>(AssetFactory::getMeshes()->sprite,
        TextureArray{AssetFactory::getTextures()->delivermanSheet}, glm::ivec2{32, 32});
    playerSprite->setPosition(glm::vec3{0.0f, 0.0f, 1.0f});
    playerSprite->setScale(2.0f);
    playerSprite->setFramesPerSecond(12.0f);
    playerSprite->createAnimation("idle", {0, 0, 0, 1, 1});
    playerSprite->createAnimation("walk", {2, 3, 4, 5});
    playerSprite->setAnimation("idle");
    playerSprite->setZIndex(-0.01f);
    playerSprite->setGroupId(Pipeline::PLAYER);
    _pipeline->attach(playerSprite.get());
    playerSprite->stage();


    input()->addPressedCallback(GLFW_KEY_ESCAPE, [this](int key, int mods) {
        this->close();
        return true;
    });

    createLevel1();

    _currentGameState = GameStateType::MAIN;

    _playerControl = std::make_unique<PlayerControl>(playerSprite, input());

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

    std::shared_ptr<lithium::SimpleCamera> camera = _pipeline->camera();
    float t = time() * 0.1f;
    // set t to lock at isometric view, without using magic numbers
    t = glm::radians(45.0f);
    float camX = sin(t) * goptions::cameraRadius;
    float camZ = cos(t) * goptions::cameraRadius;
    // set camY so that it is truly isometric
    float camY = goptions::cameraRadius * 0.5f;

    // Camera is following the position of _playerControl
    camera->setTarget(glm::mix(camera->position(), _playerControl->position(), dt));
    camera->setPosition(glm::mix(camera->position(), glm::vec3{camX, camY, camZ} + _playerControl->position(), dt));

    _playerControl->update(dt);

    camera->update(dt);
    _pipeline->setTime(time());
    _pipeline->render();
}

void App::handleStateTransitions()
{
    if(!_stateTransitions.empty())
    {
        currentGameState()->second.exit();
        _currentGameState = *_stateTransitions.begin();
        _stateTransitions.erase(_stateTransitions.begin());
        currentGameState()->second.enter();
        //_pipeline->updatePointLights();
    }
}
