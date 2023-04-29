#include "glapplication.h"
#include "pipeline.h"
#include "assetfactory.h"

class App : public lithium::Application
{
public:
    App() : Application{"lithium-lab", glm::ivec2{1440, 800}, lithium::Application::Mode::MULTISAMPLED_4X, false}
    {
        AssetFactory::loadMeshes();
        AssetFactory::loadTextures();
        AssetFactory::loadObjects();
        AssetFactory::loadFonts();
        _object = std::shared_ptr<lithium::Object>(new lithium::Object(AssetFactory::getMeshes()->cube,
            {AssetFactory::getTextures()->logoDiffuse}));
        _object->setPosition(glm::vec3{0.0f});
        _object->setScale(1.0f);
        _renderPipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());
        _renderPipeline->attach(_object.get());
        _objects.push_back(_object);
        _object->stage();

        input()->addPressedCallback(GLFW_KEY_ESCAPE, [this](int key, int mods) {
            this->close();
            return true;
        });

        printf("%s\n", glGetString(GL_VERSION));
    }

    virtual ~App() noexcept
    {
        _renderPipeline = nullptr;
    }

    virtual void update(float dt) override
    {
        for(auto o : _objects)
        {
            o->update(dt);
            o->setRotation(o->rotation() + glm::vec3{8.0f * dt});
        }
        _renderPipeline->camera()->update(dt);
        _renderPipeline->render();
    }

    virtual void onFramebufferResized(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:
    std::shared_ptr<Pipeline> _renderPipeline{nullptr};
    std::vector<std::shared_ptr<lithium::Object>> _objects;
    std::shared_ptr<lithium::Object> _object;
};

int main(int argc, const char* argv[])
{
    std::unique_ptr<App> app = std::make_unique<App>();
    app->run();
    return 0;
}