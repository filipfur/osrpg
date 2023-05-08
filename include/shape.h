#pragma once

#include "glmesh.h"

namespace
{
    using attr = lithium::VertexArrayBuffer::AttributeType;
    static constexpr attr POSITION{attr::VEC3};
    static constexpr attr NORMAL{attr::VEC3};
    static constexpr attr UV{attr::VEC2};
    static constexpr attr BONE_IDS{attr::VEC4};
    static constexpr attr BONE_WEIGHTS{attr::VEC4};
    static constexpr attr COLOR{attr::VEC3};
}

namespace shape
{
    lithium::Mesh* Plane()
    {
        return new lithium::Mesh(
            std::vector<attr>{ POSITION, NORMAL, UV },
            std::vector<GLfloat>{
                -1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
                -1.0,  1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 1.0, 
                1.0,  1.0, 0.0f,	0.0f, 1.0f, 0.0f,	1.0, 1.0,  
                1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	1.0, 0.0f
            },
            std::vector<GLuint>{
                0, 2, 1,
                0, 3, 2
            });
    }
}
