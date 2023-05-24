#include "app.h"
#include "goptions.h"
#include "sprite.h"
#include "gltext.h"
#include "utility.h"
#include "gltimemeasure.h"

#define DEBUG_TEXT(text, x, y) _textRenderer.createText(AssetFactory::getFonts()->righteousFont, "")->setPosition(glm::vec3{x, y, 0.0f})->setUpdateCallback([this](lithium::Updateable* u, float time, float dt) { \
        static int lastUpdate = 0.0f; \
        if(time - lastUpdate > 0.2f) \
        { \
            dynamic_cast<lithium::Text*>(u)->setText(text); \
            lastUpdate = time; \
        } \
        return true; \
    });

App::App() :
    Application{"lithium-lab", glm::ivec2{1440, 800}, lithium::Application::Mode::MULTISAMPLED_4X, false},
    _textRenderer{defaultFrameBufferResolution()}
{
    AssetFactory::loadMeshes();
    AssetFactory::loadTextures();
    AssetFactory::loadObjects();
    AssetFactory::loadFonts();
    _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());

    auto playerSprite = std::make_shared<Sprite>(AssetFactory::getMeshes()->sprite,
        TextureArray{AssetFactory::getTextures()->delivermanSheet}, glm::ivec2{64, 64});
    playerSprite->setPosition(glm::vec3{0.0f, 0.0f, 1.0f});
    playerSprite->setScale(2.0f);
    playerSprite->setFramesPerSecond(12.0f);
    playerSprite->createAnimation("idle", {0, 0, 0, 0, 0, 1, 1, 1, 1});
    playerSprite->createAnimation("walk", {2, 2, 3, 3, 4, 5, 5, 6, 6, 7});
    playerSprite->setAnimation("idle");
    playerSprite->setZIndex(-0.01f);
    playerSprite->setGroupId(Pipeline::PLAYER);
    _pipeline->attach(playerSprite.get());
    playerSprite->stage();

    _currentCubeObject = std::make_shared<lithium::Object>(AssetFactory::getMeshes()->cube,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});
    _currentCubeObject->setGroupId(Pipeline::CURRENT_BLOCK);
    _pipeline->attach(_currentCubeObject.get());
    _currentCubeObject->stage();
    _currentCubeObject->setPosition(glm::vec3{0.0f, 1.0f, 0.0f});
    //_currentCubeObject->setScale(1.2f);


    input()->addPressedCallback(GLFW_KEY_ESCAPE, [this](int key, int mods) {
        this->close();
        return true;
    });

    input()->addPressedCallback(GLFW_KEY_LEFT, [this](int key, int mods) {
        _cameraViewAngle += glm::pi<float>() * 0.5f; 
        return true;
    });

    input()->addPressedCallback(GLFW_KEY_RIGHT, [this](int key, int mods) {
        _cameraViewAngle -= glm::pi<float>() * 0.5f; 
        return true;
    });

    createLevel1();

    _currentGameState = GameStateType::MAIN;

    _playerControl = std::make_unique<PlayerControl>(playerSprite, input());

    /*component::Time::refreshAll();
    _cubeCollisionSystem.update(_entities, [this](ecs::Entity& entity, const Time& time, const Connection& connection, const Collider& collider) {
        component::Time::refreshAll();
        _cubeCollisionSystem.update(_entities, [this, &entity](ecs::Entity& other, const Time& time, const Connection& connection, const Collider& collider) {
            if(&entity != &other)
            {

            }
        });
    });*/

    float paddX = 16.0f;
    float paddY = 32.0f;
    float margY = 48.0f;
    int debugTexts = 0;

    DEBUG_TEXT("position: " + utility::vectorToString(_playerControl->position()), paddX, defaultFrameBufferResolution().y - paddY - margY * debugTexts++);
    DEBUG_TEXT("view angle: " + std::to_string(glm::degrees(_cameraViewAngle)), paddX, defaultFrameBufferResolution().y - paddY - margY * debugTexts++);
    DEBUG_TEXT("right vector: " + utility::vectorToString(_playerControl->rightDirection()), paddX, defaultFrameBufferResolution().y - paddY - margY * debugTexts++);
    DEBUG_TEXT(std::string{"side: "} + (_leftSide ? "left" : "right"), paddX, defaultFrameBufferResolution().y - paddY - margY * debugTexts++);

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
    // set camY so that it is truly isometric
    float camY = goptions::cameraRadius * 0.5f;

    // Camera is following the position of _playerControl
    camera->setTarget(glm::mix(camera->target(), _playerControl->position(), dt));
    //camera->setPosition(tween(camera->position(), glm::vec3{camX, camY, camZ}, dt));
    camera->setPosition(camera->target() + glm::vec3{camX, camY, camZ});

    _playerControl->update(dt);

    //static lithium::Sphere sphere{glm::vec3{0.0f, 0.0f, 0.0f}, 0.25f};
    //sphere.setPosition(_playerControl->position());

    auto& time = component::Time::get();
    time.seconds += dt;
    time.delta = dt;
    component::Time::set(time);

    static const Collider* lastCollision{nullptr};
    bool collided{false};
    bool footCollided{false};
    bool headCollided{false};
    bool footCurveCollided{false};
    bool roofCollided{false};
    bool curveCollided{false};
    {
        lithium::TimeMeasure::Handle handle2{lithium::TimeMeasure::start("player x cube collision", false)};
        _cubeCollisionSystem.update(_entities, [this, &collided, &headCollided, &footCollided, &footCurveCollided, &roofCollided, &curveCollided](ecs::Entity& entity, const Time& time, const Collider& collider) {
            static int collisionId = -1;
            static lithium::Collision collision;
            const glm::vec3 bodyPos = _playerControl->position() + glm::vec3{0.0f, 0.4f, 0.0f};
            if(collider.geometry->intersect(bodyPos))
            {
                if(collider.typeId == 1)
                {
                    return true;
                }
                if(collisionId != entity.id())
                {
                    //std::cout << "Collided with entity: " << collisionId << std::endl;
                    collisionId = entity.id();
                    _currentCubeObject->setPosition(collider.geometry->position());
                }
                lastCollision = &collider;
                collided = true;
            }
            if(collider.geometry->intersect(bodyPos + _playerControl->rightOrthogonal()))
            {
                curveCollided = true;
            }
            const glm::vec3 footPos = _playerControl->position() + glm::vec3{0.0f, -0.01f, 0.0f};
            const glm::vec3 footCurve = footPos + _playerControl->rightOrthogonal();
            if(collider.geometry->intersect(footPos))
            {
                footCollided = true;
            }
            if(collider.geometry->intersect(footCurve))
            {
                footCurveCollided = true;
            }
            const glm::vec3 headPos = _playerControl->position() + glm::vec3{0.0f, 1.9f, 0.0f};
            static int headId = -1;
            if(collider.geometry->intersect(headPos))
            {
                /*if(headId != entity.id())
                {
                    std::cout << "Head collided with entity: " << headId << std::endl;
                    headId = entity.id();
                }*/
                headCollided = true;
            }
            if(collider.geometry->intersect(headPos + _playerControl->rightOrthogonal()))
            {
                roofCollided = true;
            }
            return true;
        });
    }

    if((!collided || curveCollided) && lastCollision)
    {
        //std::cout << "Time to rotate!!" << std::endl;
        glm::vec3 p0 = lastCollision->geometry->position();
        glm::vec3 p1 = _playerControl->position();
        
        float frx = glm::fract(p1.x);
        float frz = glm::fract(p1.z);
        float fr = std::max(frx, frz);

        glm::vec3 r0 = _playerControl->rightDirection();
        glm::vec3 e0;        

        if(frx != 0)
        {
            e0 = glm::vec3{p1.x - p0.x, 0.0f, 0.0f};
            p1.x = glm::round(p1.x);
        }
        else if(frz != 0)
        {
            e0 = glm::vec3{0.0f, 0.0f, p1.z - p0.z};
            p1.z = glm::round(p1.z);
        }

        glm::vec3 r1;
        bool goingLeft{false};
        if(glm::dot(r0, glm::normalize(e0)) > 0)
        {
            //rotate r0 90 degrees to the right
            r1 = glm::vec3{r0.z, 0.0f, -r0.x};
            _playerControl->sprite()->setRotation(_playerControl->sprite()->rotation() + glm::vec3{0.0f, 90.0f, 0.0f});
            goingLeft = false;
        }
        else
        {
            //rotate r0 90 degrees to the left
            r1 = glm::vec3{-r0.z, 0.0f, r0.x};
            _playerControl->sprite()->setRotation(_playerControl->sprite()->rotation() - glm::vec3{0.0f, 90.0f, 0.0f});
            goingLeft = true;
        }

        if(fr > 0.5f)
        {
            fr = 1.0f - fr;
        }

        _playerControl->setPosition(p1 + (goingLeft ? -r1 : r1) * fr);
        _playerControl->setRightDirection(r1, goingLeft);

        if(goingLeft && _leftSide)
        {
            _cameraViewAngle += glm::pi<float>() * 0.5f;
        }
        else if(!goingLeft && !_leftSide)
        {
            _cameraViewAngle -= glm::pi<float>() * 0.5f;
        }
        else
        {
            _leftSide = !_leftSide;
        }

        _pipeline->setOrientation(r1, _leftSide);

        //lastCollision = nullptr;
    }

   // std::cout << "collided: " << collided << "headCollided: " << headCollided << std::endl;    

    if(collided && (!headCollided || roofCollided))
    {
        _playerControl->bumpHead();
    }

    if(collided && (!footCollided || footCurveCollided))
    {
        _playerControl->land();
    }

    if(collided && footCollided && !footCurveCollided)
    {
        _playerControl->fall();
    }

    camera->update(dt);
    _pipeline->setTime(time.seconds);
    _pipeline->render();
    //glClear(GL_DEPTH_BUFFER_BIT);
    _textRenderer.update(dt);
    _textRenderer.render();
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