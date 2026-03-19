#include "opengl_renderer.hpp"

unsigned int OpenGLRenderer::enableVertexBuffer(const std::vector<glm::vec2>& data, AttributeType attribute, bool useDynamicDraw)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(glm::vec2), data.data(), useDynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glVertexAttribPointer((unsigned int)(attribute), 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray((unsigned int)(attribute));
    
    glCheckError();

    return vbo;
}

unsigned int OpenGLRenderer::enableVertexBuffer(const std::vector<glm::vec3>& data, AttributeType attribute, bool useDynamicDraw)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(glm::vec3), data.data(), useDynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glVertexAttribPointer((unsigned int)(attribute), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray((unsigned int)(attribute));

    glCheckError();

    return vbo;
}

unsigned int OpenGLRenderer::enableVertexBuffer(const std::vector<glm::vec4>& data, AttributeType attribute, bool useDynamicDraw)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(glm::vec4), data.data(), useDynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glVertexAttribPointer((unsigned int)(attribute), 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray((unsigned int)(attribute));

    glCheckError();

    return vbo;
}
