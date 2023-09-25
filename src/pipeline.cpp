#include "pipeline.h"
#include "glplane.h"
#include "glinstancedobject.h"

Pipeline::Pipeline(const glm::ivec2& resolution) : lithium::RenderPipeline{resolution},
    _camera{new lithium::SimpleCamera(glm::perspective(glm::radians(45.0f), (float)resolution.x / (float)resolution.y, 0.1f, 100.0f))},
    _frameBuffer{std::make_shared<lithium::FrameBuffer>(resolution)}
{
    enableDepthTesting();
    enableBlending();
    blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    enableMultisampling();

    _blockShader = std::make_shared<lithium::ShaderProgram>("shaders/object.vert", "shaders/object.frag");
    _blockShader->setUniform("u_texture_0", 0);
    _blockShader->setUniform("u_projection", _camera->projection());

    _terrainShader = std::make_shared<lithium::ShaderProgram>("shaders/object.vert", "shaders/terrain.frag");
    _terrainShader->setUniform("u_texture_0", 0);
    _terrainShader->setUniform("u_projection", _camera->projection());

    _instanceShader = std::make_shared<lithium::ShaderProgram>("shaders/inst.vert", "shaders/object.frag");
    _instanceShader->setUniform("u_texture_0", 0);
    _instanceShader->setUniform("u_projection", _camera->projection());

    _msaaShader = std::make_shared<lithium::ShaderProgram>("shaders/screen.vert", "shaders/msaa.frag");
    _msaaShader->setUniform("u_texture", 0);
    _msaaShader->setUniform("u_resolution", resolution);

    _frameBuffer->bind();
    _frameBuffer->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_TEXTURE_2D_MULTISAMPLE);
    _frameBuffer->createTexture(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8, GL_FLOAT, GL_TEXTURE_2D_MULTISAMPLE);
    _frameBuffer->declareBuffers();
    _frameBuffer->unbind();

    _screenMesh = std::shared_ptr<lithium::Mesh>(lithium::Plane2D());

    _terrainGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return renderable->groupId() == Pipeline::TERRAIN;
    });

    _instanceGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return dynamic_cast<lithium::InstancedObject<glm::mat4>*>(renderable);
    });

    _mainGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
        return dynamic_cast<lithium::Object*>(renderable) && !renderable->hasAttachments();
    });

    clearColor(0.0f, 0.0f, 0.0f, 1.0f);

    _mainStage = addRenderStage(std::make_shared<lithium::RenderStage>(_frameBuffer, [this](){
        clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _terrainShader->setUniform("u_view", _camera->view());
        _terrainShader->setUniform("u_view_pos", _camera->position());
        _terrainGroup->render(_terrainShader.get());
        _blockShader->setUniform("u_view", _camera->view());
        _blockShader->setUniform("u_view_pos", _camera->position());
        _blockShader->setUniform("u_time", 0.0f);
        _mainGroup->render(_blockShader.get());
        _instanceShader->setUniform("u_view", _camera->view());
        _instanceShader->setUniform("u_view_pos", _camera->position());
        _instanceShader->setUniform("u_time", 0.0f);
        _instanceGroup->render(_instanceShader.get());
    }));

    _finalStage = addRenderStage(std::make_shared<lithium::RenderStage>(nullptr, [this](){
        setViewportToResolution();
        clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _msaaShader->use();
        _mainStage->frameBuffer()->texture(GL_COLOR_ATTACHMENT0)->bind(GL_TEXTURE0);
        _mainStage->frameBuffer()->bindTexture(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE1);
        _screenMesh->bind();
        _screenMesh->draw();
    }));
}

void Pipeline::setResolution(const glm::ivec2& resolution)
{
    lithium::RenderPipeline::setResolution(resolution);
}

Pipeline::~Pipeline()
{
    _blockShader = nullptr;
    _msaaShader = nullptr;
    _screenMesh = nullptr;
    _terrainShader = nullptr;
}