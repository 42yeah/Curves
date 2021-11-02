#pragma once

#include "Renderer.h"

class BlueprintRenderer : public Renderer {
public:
    BlueprintRenderer();

    virtual void render() override;

    // Shader setup
    GLuint VAO;
    Program program;
    float scale;
    glm::vec2 offset;
};