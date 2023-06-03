#include "app.h"
#include "utility.h"

void App::createLevel1()
{
    auto object = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->package,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    auto longPackage = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->longPackage,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    auto slot = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->slot,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    auto beam = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->beam,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    auto& assets = AssetFactory::getInstance();
    std::vector<lithium::Node*> nodes;
    assets._gltfLoader.loadNodes("assets/package/level1.gltf", nodes);
    constructRotationGraph(nodes);
    for(auto node : nodes)
    {
        std::cout << "Node: " << node->name() << " [";
        utility::printGLM(node->position(), 3);
        std::cout << ']' << std::endl;
        glm::mat4 model{1.0f};
        model = glm::translate(model, node->position());
        //model = glm::rotate(model, glm::radians(90.0f) * (rand() % 4), glm::vec3{1.0f, 0.0f, 0.0f});
        //model = glm::rotate(model, glm::radians(90.0f) * (rand() % 4), glm::vec3{0.0f, 1.0f, 0.0f});
        //model = glm::rotate(model, glm::radians(90.0f) * (rand() % 4), glm::vec3{0.0f, 0.0f, 1.0f});
        model *= glm::toMat4(node->rotation());


        int colliderType{0};
        switch(node->meshId())
        {
            case 0:
                object->addInstance(model);
                break;
            case 2:
                longPackage->addInstance(model);
                break;
            case 3:
                slot->addInstance(model);
                colliderType = 1;
                break;
            case 4:
                beam->addInstance(model);
                colliderType = -1;
                break;
        }

        if(colliderType >= 0)
        {
            /*ecs::Entity* e = new ecs::Entity();
            component::Time::attach(*e);
            component::Collider::attach(*e);
            e->get<component::Collider>()->geometry = new lithium::AABB{node->position(), -glm::vec3{1.0f}, glm::vec3{1.0f}};
            e->get<component::Collider>()->typeId = colliderType;
            _entities.emplace(e);*/
        }
    }
    object->allocateBufferData();
    object->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });
    longPackage->allocateBufferData();
    longPackage->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });
    slot->allocateBufferData();
    slot->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });

    beam->allocateBufferData();
    beam->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });


    auto letterSprite = std::make_shared<Sprite>(AssetFactory::getMeshes()->sprite,
        TextureArray{AssetFactory::getTextures()->letterDiffuse}, glm::ivec2{32, 32});
    letterSprite->setPosition(glm::vec3{4.0f, 0.0f, 1.0f});
    letterSprite->setScale(2.0f);
    letterSprite->createAnimation("idle", {0});
    letterSprite->setAnimation("idle");
    letterSprite->setZIndex(-0.02f);

    letterSprite->setUpdateCallback([](lithium::Updateable* u, float time, float dt){
        auto sprite = dynamic_cast<Sprite*>(u);
        sprite->setScale(1.2f + std::sin(time * 6.0f) * 0.4f);
        //rotate around the x axis
        sprite->setRotation(glm::vec3{0.0f, 0.0f, sin(time * 8.0f) * 15.0f});
        auto p0 = sprite->position();
        sprite->setPosition(glm::vec3{p0.x, 0.5f + sin(time * 4.0f) * 0.2f, p0.z});
        return true;
    });

    auto firstScene = std::make_shared<lithium::Scene>();
    firstScene->addObject(object);
    firstScene->addObject(longPackage);
    firstScene->addObject(slot);
    firstScene->addObject(beam);
    firstScene->addObject(letterSprite);
    //firstScene->addObject(playerSprite);
    //firstScene->addObject(level1);
    firstScene->attach(_pipeline);

    /*auto level1 = std::make_shared<lithium::Object>(AssetFactory::getMeshes()->level1,
    TextureArray{AssetFactory::getTextures()->packageDiffuse});*/
    
    lithium::GameState firstGameState;
    firstGameState.addScene(firstScene);
    _gameStates.emplace(GameStateType::MAIN, firstGameState);
    firstGameState.enter();
}