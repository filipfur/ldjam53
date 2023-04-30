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
    auto object = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->cube,
        TextureArray{AssetFactory::getTextures()->logoDiffuse});
    object->setPosition(glm::vec3{0.0f});
    object->setScale(1.0f);
    //object->stage();

    auto screen = std::make_shared<Sprite>(AssetFactory::getMeshes()->screen,
        TextureArray{AssetFactory::getTextures()->delivermanSheet});
    screen->setPosition(glm::vec3{0.0f});
    screen->setScale(1.0f);
    screen->setSpriteDimension(glm::ivec2{32, 32});
    screen->setRegionPosition(glm::vec2{0.0f, 0.0f});
    screen->setFramesPerSecond(5.0f);

    //_pipeline->attach(object.get());

    for(int i{0}; i < 32; ++i)
    {
        glm::mat4 model{1.0f};
        // Translate random position between -8 and 8 on x, y and z axis with 2 unit between them, using rand()
        model = glm::translate(model, glm::vec3{((rand() % 8) - 4) * 2.0f, ((rand() % 8) - 4) * 2.0f, ((rand() % 8) - 4) * 2.0f});
        object->addInstance(model);
    }
    object->allocateBufferData();
    object->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });
    //object->mesh()->unbind();

    input()->addPressedCallback(GLFW_KEY_ESCAPE, [this](int key, int mods) {
        this->close();
        return true;
    });

    auto firstScene = std::make_shared<lithium::Scene>();
    firstScene->addObject(object);
    firstScene->addObject(screen);
    firstScene->attach(_pipeline);
    
    lithium::GameState firstGameState;
    firstGameState.addScene(firstScene);
    _gameStates.push_back(firstGameState);
    firstGameState.enter();

    _currentGameState = GameStateType::MAIN;

    printf("%s\n", glGetString(GL_VERSION));
}

App::~App() noexcept
{
    _pipeline = nullptr;
}

void App::update(float dt)
{
    lithium::Application::update(dt);

    currentGameState()->update(dt);

    handleStateTransitions();

    std::shared_ptr<lithium::SimpleCamera> camera = _pipeline->camera();
    float t = time() * 0.1f;
    // set t to lock at isometric view, without using magic numbers
    t = glm::radians(45.0f);
    float camX = sin(t) * goptions::cameraRadius;
    float camZ = cos(t) * goptions::cameraRadius;
    // set camY so that it is truly isometric
    float camY = goptions::cameraRadius * 0.5f;
    camera->setPosition(glm::vec3{camX, camY, camZ});

    camera->update(dt);
    _pipeline->setTime(time());
    _pipeline->render();
}

void App::handleStateTransitions()
{
    if(!_stateTransitions.empty())
    {
        currentGameState()->exit();
        _currentGameState = *_stateTransitions.begin();
        _stateTransitions.erase(_stateTransitions.begin());
        currentGameState()->enter();
        //_pipeline->updatePointLights();
    }
}
