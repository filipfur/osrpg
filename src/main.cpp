#include "glapplication.h"
#include "pipeline.h"
#include "assetfactory.h"

class App : public lithium::Application
{
public:
    App() : Application{"A simple application", glm::ivec2{600}, lithium::Application::Mode::DEFAULT, false}
    {
        AssetFactory::loadMeshes();
        AssetFactory::loadTextures();
        AssetFactory::loadObjects();
        AssetFactory::loadFonts();
        _object = std::shared_ptr<lithium::Object>(new lithium::Object(AssetFactory::getMeshes()->mesh,
            {AssetFactory::getTextures()->diffuse}));
        _object->setPosition(glm::vec3{0.0f});
        _object->setScale(1.0f);
        _renderPipeline = new Pipeline(defaultFrameBufferResolution());
        _renderPipeline->addRenderable(_object.get());
        _objects.push_back(_object);
    }

    virtual ~App() noexcept
    {
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
    Pipeline* _renderPipeline{nullptr};
    std::vector<std::shared_ptr<lithium::Object>> _objects;
    std::shared_ptr<lithium::Object> _object;
};

int main(int argc, const char* argv[])
{
    App* app = new App();
    app->run();
    return 0;
}