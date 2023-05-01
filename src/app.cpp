#include "app.h"
#include "goptions.h"
#include "sprite.h"
#include "utility.h"

App::App() : Application{"lithium-lab", glm::ivec2{1440, 800}, lithium::Application::Mode::MULTISAMPLED_4X, false}
{
    AssetFactory::loadMeshes();
    AssetFactory::loadTextures();
    AssetFactory::loadObjects();
    AssetFactory::loadFonts();
    _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());
    auto object = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->package,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    auto longPackage = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->longPackage,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    auto playerSprite = std::make_shared<Sprite>(AssetFactory::getMeshes()->sprite,
        TextureArray{AssetFactory::getTextures()->delivermanSheet}, glm::ivec2{32, 32});
    playerSprite->setPosition(glm::vec3{0.0f, 0.0f, 1.01f});
    playerSprite->setScale(2.0f);
    playerSprite->setFramesPerSecond(5.0f);
    playerSprite->createAnimation("idle", {0, 0, 0, 1, 1});
    playerSprite->createAnimation("walk", {2, 3, 4, 5});
    playerSprite->setAnimation("idle");

    auto level1 = std::make_shared<lithium::Object>(AssetFactory::getMeshes()->level1,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    //_pipeline->attach(object.get());

    for(int i{0}; i < 5; ++i)
    {
        glm::mat4 model{1.0f};
        if(i < 4)
        {
            model = glm::translate(model, glm::vec3{-4.0f, 0.0f, 0.0f} + glm::vec3{2.0f, 0.0f, 0.0f} * (float)i);
        }
        else
        {
            model = glm::translate(model, glm::vec3{0.0f, 2.0f, 0.0f} + glm::vec3{2.0f, 0.0f, 0.0f} * (float)(i - 4));
        }
        object->addInstance(model);
    }
    glm::mat4 box{1.0f};
    box = glm::translate(box, glm::vec3{6.0f, 0.0f, 0.0f});
    object->addInstance(box);
    object->allocateBufferData();
    object->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });

    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{4.0f, 2.0f, 0.0f});
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3{0.0f, 1.0f, 0.0f});
    longPackage->addInstance(model);
    longPackage->allocateBufferData();
    longPackage->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });

    input()->addPressedCallback(GLFW_KEY_ESCAPE, [this](int key, int mods) {
        this->close();
        return true;
    });

    auto firstScene = std::make_shared<lithium::Scene>();
    //firstScene->addObject(object);
    //firstScene->addObject(longPackage);
    firstScene->addObject(playerSprite);
    firstScene->addObject(level1);
    firstScene->attach(_pipeline);

    auto& assets = AssetFactory::getInstance();
    std::vector<lithium::Node*> nodes;
    assets._gltfLoader.loadNodes("assets/package/level1.gltf", nodes);
    for(auto node : nodes)
    {
        std::cout << "Node: " << node->name() << " [";
        utility::printGLM(node->position(), 3);
        std::cout << ']' << std::endl;
    }
    
    lithium::GameState firstGameState;
    firstGameState.addScene(firstScene);
    _gameStates.push_back(firstGameState);
    firstGameState.enter();

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

    // Camera is following the position of _playerControl
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
        currentGameState()->exit();
        _currentGameState = *_stateTransitions.begin();
        _stateTransitions.erase(_stateTransitions.begin());
        currentGameState()->enter();
        //_pipeline->updatePointLights();
    }
}
