#pragma once

#include <memory>
#include "glsimplecamera.h"
#include "glrenderpipeline.h"
#include "glframebuffer.h"
#include "gluniformbufferobject.h"
#include "glcubemaphdr.h"

class Pipeline : public lithium::RenderPipeline
{
public:
    enum Group
    {
        DEFAULT,
        BACKGROUND,
        TERRAIN,
        SKYBOX
    };

    Pipeline(const glm::ivec2& resolution);

    ~Pipeline() noexcept;

    std::shared_ptr<lithium::SimpleCamera> camera()
    {
        return _camera;
    }

    void setCubemap(std::shared_ptr<lithium::CubemapHDR> cubemap)
    {
        _cubemap = cubemap;
    }

    virtual void setResolution(const glm::ivec2& resolution) override;

private:
    /* Shaders */
    std::shared_ptr<lithium::ShaderProgram> _blockShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _instanceShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _msaaShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _screenShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _skyboxShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _terrainShader{nullptr};
    std::shared_ptr<lithium::SimpleCamera> _camera{nullptr};

    /*Render groups*/
    std::shared_ptr<lithium::RenderGroup> _screenGroup;
    std::shared_ptr<lithium::RenderGroup> _skyboxGroup;
    std::shared_ptr<lithium::RenderGroup> _mainGroup;
    std::shared_ptr<lithium::RenderGroup> _terrainGroup;
    std::shared_ptr<lithium::RenderGroup> _instanceGroup;

    /*Render stages*/
    std::shared_ptr<lithium::RenderStage> _mainStage;
    std::shared_ptr<lithium::RenderStage> _finalStage;

    /* Framebuffers */
    std::shared_ptr<lithium::FrameBuffer> _frameBuffer;
    
    std::shared_ptr<lithium::UniformBufferObject> _cameraUBO;

    std::shared_ptr<lithium::Mesh> _invCubeMesh{nullptr};
    std::shared_ptr<lithium::CubemapHDR> _cubemap{nullptr};

    /* Meshes */
    std::shared_ptr<lithium::Mesh> _screenMesh;
};