#pragma once

#include "Renderer.h"
#include <vector>


class SplineRenderer : public Renderer {
public:
    SplineRenderer();

    void set_keypoints(std::vector<glm::vec3>& keypoints);

    std::vector<glm::vec3> expand(std::vector<glm::vec3>& keypoints);

    virtual void render() override;

    // Shader setup
    Program spline_prog;
    Program dots_prog;
    std::vector<glm::vec3> keypoints;
    std::vector<glm::vec3> expanded;
    std::vector<glm::mat3x2> solves;
    std::vector<glm::vec3> tangent_lines;

    GLuint VAO, VBO, dots_VAO, dots_VBO, tangent_VAO, tangent_VBO;
    glm::mat4 ortho, view;

    int highlight, prev_highlight;
    int tangent, tangent_highlight;
    float scale;

    int anchor;
    glm::vec2 slope;
};
