#include "app.h"



App::App() : Application{"lithium-lab", glm::ivec2{1440, 800}, lithium::Application::Mode::MULTISAMPLED_4X, false}
{
    AssetFactory::loadMeshes();
    AssetFactory::loadTextures();
    AssetFactory::loadObjects();
    AssetFactory::loadFonts();
    auto object = std::shared_ptr<lithium::Object>(new lithium::Object(AssetFactory::getMeshes()->cube,
        {AssetFactory::getTextures()->logoDiffuse}));
    object->setPosition(glm::vec3{0.0f});
    object->setScale(1.0f);
    _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());
    _pipeline->attach(object.get());
    object->stage();

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

    _pipeline->camera()->update(dt);
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
