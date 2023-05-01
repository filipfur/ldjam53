#include "app.h"
#include "utility.h"

void App::createLevel1()
{
    auto object = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->package,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    auto longPackage = std::make_shared<lithium::InstancedObject<glm::mat4>>(AssetFactory::getMeshes()->longPackage,
        TextureArray{AssetFactory::getTextures()->packageDiffuse});

    /*for(int i{0}; i < 5; ++i)
    {
        glm::mat4 model{1.0f};
        if(i < 4)
        {
            model = glm::translate(model, glm::vec3{-4.0f, 1.0f, 0.0f} + glm::vec3{2.0f, 0.0f, 0.0f} * (float)i);
        }
        else
        {
            model = glm::translate(model, glm::vec3{0.0f, 3.0f, 0.0f} + glm::vec3{2.0f, 0.0f, 0.0f} * (float)(i - 4));
        }
        object->addInstance(model);
    }
    glm::mat4 box{1.0f};
    box = glm::translate(box, glm::vec3{6.0f, 1.0f, 0.0f});
    object->addInstance(box);
    object->allocateBufferData();
    object->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });*/

    /*glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{4.0f, 3.0f, 0.0f});
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3{0.0f, 1.0f, 0.0f});
    longPackage->addInstance(model);
    longPackage->allocateBufferData();
    longPackage->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });*/

    auto& assets = AssetFactory::getInstance();
    std::vector<lithium::Node*> nodes;
    assets._gltfLoader.loadNodes("assets/package/level1.gltf", nodes);
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
        switch(node->meshId())
        {
            case 0:
                object->addInstance(model);
                break;
            case 2:
                longPackage->addInstance(model);
                break;
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

    auto firstScene = std::make_shared<lithium::Scene>();
    firstScene->addObject(object);
    firstScene->addObject(longPackage);
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