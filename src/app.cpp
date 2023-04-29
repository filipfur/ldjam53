#include "app.h"



App::App() : Application{"lithium-lab", glm::ivec2{1440, 800}, lithium::Application::Mode::MULTISAMPLED_4X, false}
{
    AssetFactory::loadMeshes();
    AssetFactory::loadTextures();
    AssetFactory::loadObjects();
    AssetFactory::loadFonts();
    auto object = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->cube,
        TextureArray{AssetFactory::getTextures()->logoDiffuse});
    object->setPosition(glm::vec3{0.0f});
    object->setScale(1.0f);
    _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());
    _pipeline->attach(object.get());
    object->stage();

    for(int i{0}; i < 200; ++i)
    {
        glm::mat4 model{1.0f};
        // Translate random position between -20 and 20 on x, y and z axis
        model = glm::translate(model, glm::vec3{static_cast<float>(rand() % 40 - 20),
            static_cast<float>(rand() % 40 - 20), static_cast<float>(rand() % 40 - 20)});
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

    float cameraRadius = 30.0f;

    float t = time() * 0.1f;

    float camX = sin(t) * cameraRadius;
    float camZ = cos(t) * cameraRadius;

    camera->setPosition(glm::vec3{camX, 30.0f, camZ});

    camera->update(dt);
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
