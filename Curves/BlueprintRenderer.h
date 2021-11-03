#pragma once

#include "Renderer.h"

class BlueprintRenderer : public Renderer {
public:
    BlueprintRenderer();

    virtual void render() override;

    virtual void set_tangent(int tangent) override;

    virtual void set_keypoints(std::vector<glm::vec3>& keypoints) override;

    virtual void set_highlight(int highlight) override;

    virtual void set_slope(glm::vec2 slope) override;

    virtual void set_tangent_highlight(int highlight) override;

    virtual std::vector<glm::vec3>& get_tangent_lines() override;

    // Shader setup
    GLuint VAO;
    Program program;
    glm::vec2 offset;
    std::vector<glm::vec3> dummy_tangent_lines;
};