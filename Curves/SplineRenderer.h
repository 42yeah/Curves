#pragma once

#include "Renderer.h"
#include <vector>


class SplineRenderer : public Renderer {
public:
    SplineRenderer();

    std::vector<glm::vec3> expand(std::vector<glm::vec3>& keypoints);

    virtual void render() override;

    virtual void set_tangent(int tangent) override;

    virtual void set_keypoints(std::vector<glm::vec3>& keypoints) override;

    virtual void set_highlight(int highlight) override;

    virtual void set_slope(glm::vec2 slope) override;

    virtual void set_tangent_highlight(int highlight) override;

    virtual std::vector<glm::vec3> &get_tangent_lines() override;

    // Shader setup
    Program spline_prog;
    Program dots_prog;
    std::vector<glm::vec3> keypoints;
    std::vector<glm::vec2> directions;
    std::vector<glm::vec3> expanded;
    std::vector<glm::vec3> tangent_lines;

    GLuint VAO, VBO, dots_VAO, dots_VBO, tangent_VAO, tangent_VBO;

    int highlight;
    int tangent, tangent_highlight;
};
