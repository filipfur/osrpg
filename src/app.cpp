#include "app.h"

#include "assetfactory.h"
#include "glassetlibrary.h"
#include "glplane.h"

App::App() : Application{"lithium-lab", glm::ivec2{1440, 800}, lithium::Application::Mode::MULTISAMPLED_4X, false}
{
    // Load all assets from the filesystem.
    AssetFactory::loadMeshes();
    AssetFactory::loadTextures();
    AssetFactory::loadObjects();
    AssetFactory::loadFonts();
    
    // Create the render pipeline
    _pipeline = std::make_shared<Pipeline>(defaultFrameBufferResolution());

    // Key cache for rotating the camera left and right.
    _keyCache = std::make_shared<lithium::Input::KeyCache>(
        std::initializer_list<int>{GLFW_KEY_LEFT, GLFW_KEY_RIGHT});
    input()->setKeyCache(_keyCache);

    // Escape key to close the application.
    input()->addPressedCallback(GLFW_KEY_ESCAPE, [this](int key, int mods) {
        this->close();
        return true;
    });

    // Set the camera oirigin position and target.
    _pipeline->camera()->setPosition(glm::vec3{3.0f, 3.0f, 3.0f});
    _pipeline->camera()->setTarget(glm::vec3{0.0f});

    lithium::AssetLibrary::loadGLTF("assets");

    lithium::AssetLibrary& lib = lithium::AssetLibrary::getInstance();
    std::cout << "Asset Library objects:" << std::endl;
    int x{0};
    for(auto o : lib)
    {
        std::cout << o.first << " number of vertices: " << o.second->mesh()->elementArrayBuffer()->count() << std::endl;
        o.second->setPosition(glm::vec3{(x % 3) * 2.0f - 2.0f, 0.0f, (x / 3) * 2.0f -2.0f});
        x += 1;
        o.second->setScale(4.0f);
        _pipeline->attach(o.second.get());
        _objects.push_back(o.second);
        o.second->stage();
    }

    static auto floorTexture = std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/floor/floor.png", GL_SRGB_ALPHA, GL_RGBA));
    floorTexture->setFilter(GL_NEAREST);
    floorTexture->setWrap(GL_REPEAT);
    std::shared_ptr<lithium::Object> floor = std::make_shared<lithium::Object>(
        std::shared_ptr<lithium::Mesh>(lithium::Plane3D(glm::vec2(99.0f), glm::vec2{99.0f})),
        std::vector<lithium::Object::TexturePointer>{floorTexture});
    floor->setQuaternion(glm::angleAxis(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
    _objects.push_back(floor);
    _pipeline->attach(floor.get());
    floor->stage();

    setMaxFps(120.0f);

    printf("%s\n", glGetString(GL_VERSION));
}

App::~App() noexcept
{
    _pipeline = nullptr;
    _background = nullptr;
    _objects.clear();
}

void App::update(float dt)
{
    lithium::Updateable::update(dt);
    // Apply a rotation to the cube.
    for(auto o : _objects)
    {
        o->update(dt);
    }

    // Rotate the camera around the origin on player input.
    if(_keyCache->isPressed(GLFW_KEY_LEFT))
    {
        _cameraAngle -= glm::pi<float>() * 0.5f * dt;
    }
    else if(_keyCache->isPressed(GLFW_KEY_RIGHT))
    {
        _cameraAngle += glm::pi<float>() * 0.5f * dt;
    }
    static const float cameraRadius = 6.0f;
    float camX = sin(_cameraAngle) * cameraRadius;
    static const float camY = cameraRadius * 0.5f;
    float camZ = cos(_cameraAngle) * cameraRadius;
    _pipeline->camera()->setPosition(glm::vec3{camX, camY, camZ});
    _pipeline->render();
}

void App::onWindowSizeChanged(int width, int height)
{
    _pipeline->setResolution(glm::ivec2{width, height});
}