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

    virtual void set_anchor(int anchor) override;

    virtual int get_anchor() override;

    virtual void set_highlight(int highlight) override;

    virtual void set_slope(glm::vec2 slope) override;

    virtual void set_tangent_highlight(int highlight) override;

    virtual std::vector<glm::vec3> &get_tangent_lines() override;

    // Shader setup
    Program spline_prog;
    Program dots_prog;
    std::vector<glm::vec3> keypoints;
    std::vector<glm::vec3> expanded;
    std::vector<glm::mat3x2> solves;
    std::vector<glm::vec3> tangent_lines;

    GLuint VAO, VBO, dots_VAO, dots_VBO, tangent_VAO, tangent_VBO;

    int highlight, prev_highlight;
    int tangent, tangent_highlight;

    int anchor;
    glm::vec2 slope;
};
