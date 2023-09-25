#include "app.h"

#include "assetfactory.h"
#include "glassetlibrary.h"
#include "glplane.h"
#include "glinstancedobject.h"

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

    lithium::AssetLibrary::loadGLTF("assets/level");

    lithium::AssetLibrary& lib = lithium::AssetLibrary::getInstance();
    std::cout << "Asset Library objects:" << std::endl;
    int x{0};
    for(auto o : lib)
    {
        std::cout << o.first
            << " number of vertices: "
            << (o.second->mesh() ? o.second->mesh()->vertexArray()->elementArrayBuffer()->count() : 0)
            << std::endl;
        //o.second->setPosition(glm::vec3{(x % 3) * 2.0f - 2.0f, 0.0f, (x / 3) * 2.0f -2.0f});
        //x += 1;
        //o.second->setScale(4.0f);
        _pipeline->attach(o.second.get());
        _objects.push_back(o.second);
        o.second->stage();
    }

    /*auto planets = lib.object("Icosphere");
    planets->setPosition(glm::vec3{0.0f, 0.0f, 0.0f});
    planets->setScale(4.0f);
    _pipeline->attach(planets.get());
    _objects.push_back(planets);
    planets->stage();*/

    /*static auto floorTexture = std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/floor/floor.png", GL_SRGB_ALPHA, GL_RGBA));
    floorTexture->setFilter(GL_NEAREST);
    floorTexture->setWrap(GL_REPEAT);
    std::shared_ptr<lithium::Object> floor = std::make_shared<lithium::Object>(
        std::shared_ptr<lithium::Mesh>(lithium::Plane3D(glm::vec2(99.0f), glm::vec2{99.0f})),
        std::vector<lithium::Object::TexturePointer>{floorTexture});
    floor->setQuaternion(glm::angleAxis(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
    floor->setObjectName("Floor");
    _objects.push_back(floor);
    _pipeline->attach(floor.get());
    floor->stage();*/

    static auto noiseTexture = std::shared_ptr<lithium::ImageTexture>(lithium::ImageTexture::load("assets/floor/floor.png", GL_SRGB_ALPHA, GL_RGBA));
    noiseTexture->setFilter(GL_NEAREST);
    noiseTexture->setWrap(GL_REPEAT);
    std::shared_ptr<lithium::Object> terrain = std::make_shared<lithium::Object>(
        std::shared_ptr<lithium::Mesh>(lithium::Plane3D(glm::vec2(99.0f), glm::vec2{1.0f})),
        std::vector<lithium::Object::TexturePointer>{noiseTexture});
    terrain->setQuaternion(glm::angleAxis(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
    terrain->setGroupId(Pipeline::TERRAIN);
    _objects.push_back(terrain);
    _pipeline->attach(terrain.get());
    terrain->stage();

    auto instancedObject = std::make_shared<lithium::InstancedObject<glm::mat4>>(
        std::shared_ptr<lithium::Mesh>(lib.object("Collection.001")->child(0)->mesh()->clone()),
        //std::shared_ptr<lithium::Mesh>(AssetFactory::getMeshes()->cube),
        std::vector<lithium::Object::TexturePointer>{AssetFactory::getTextures()->logoDiffuse}
    );
    for(int i{0}; i < 100; ++i)
    {
        instancedObject->addInstance(glm::scale(glm::translate(glm::mat4(1.0f),
                glm::vec3{(rand() % 100 - 50) * 0.1f, 0.0f, (rand() % 100 - 50) * 0.1f}),
            glm::vec3{1.0f + (rand() % 20 - 10) * 0.05f}));
    }
    instancedObject->allocateBufferData();
    instancedObject->linkBuffer({
        lithium::AttributePointer<GL_FLOAT>{0, 4, sizeof(glm::mat4), (void*)0},
        lithium::AttributePointer<GL_FLOAT>{1, 4, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{2, 4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))},
        lithium::AttributePointer<GL_FLOAT>{3, 4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))},
    });
    _objects.push_back(instancedObject);
    _pipeline->attach(instancedObject.get());
    instancedObject->stage();

    /*auto torusObject = std::make_shared<lithium::Object>(
        std::shared_ptr<lithium::Mesh>(lib.object("Collection.001")->child(0)->mesh()->clone()),
        //lib.object("Collection.001")->child(0)->mesh(),
        std::vector<lithium::Object::TexturePointer>{}
    );
    torusObject->setScale(8.0f);
    _objects.push_back(torusObject);
    _pipeline->attach(torusObject.get());
    torusObject->stage();*/

    // TODO: Make gltf loader check for sampler.
    lithium::AssetLibrary::object("Collection.004")->forEachChild([](lithium::Object* child){
        for(size_t i{0}; i < child->mesh()->materialCount(); ++i)
        {
            child->mesh()->material(i)->diffuseMap()->setWrap(GL_REPEAT);
        }
    });
    lithium::AssetLibrary::object("Collection.005")->forEachChild([](lithium::Object* child){
        for(size_t i{0}; i < child->mesh()->materialCount(); ++i)
        {
            child->mesh()->material(i)->diffuseMap()->setWrap(GL_REPEAT);
        }
    });

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
        /*if(o->objectName() != "Floor")
        {
            o->setPosition(glm::vec3{0.0f, 0.8f + sin(time()) * 0.3f, 0.0f});
            o->setRotation(o->rotation() + glm::vec3(dt * 15.0f, 22.5f * dt, 0.0f));
        }*/
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
    static const float cameraRadius = 16.0f;
    float camX = sin(_cameraAngle) * cameraRadius;
    static const float camY = 12.0f;
    float camZ = cos(_cameraAngle) * cameraRadius;
    //_pipeline->camera()->setTarget(lithium::AssetLibrary::object("Collection.004")->position());
    _pipeline->camera()->setTarget(glm::vec3{0.0f, 6.0f, 0.0f});
    _pipeline->camera()->setPosition(glm::vec3{camX, camY, camZ});
    _pipeline->render();
}

void App::onWindowSizeChanged(int width, int height)
{
    _pipeline->setResolution(glm::ivec2{width, height});
}