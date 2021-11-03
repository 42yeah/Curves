#pragma once

#include "Renderer.h"
#include <vector>


class BezierRenderer : public Renderer {
public:
    BezierRenderer();

    std::vector<glm::vec3> expand(std::vector<glm::vec3> &points);

    virtual void render() override;

    virtual void set_tangent(int tangent) override;

    virtual void set_keypoints(std::vector<glm::vec3>& keypoints) override;

    virtual void set_anchor(int anchor) override;

    virtual int get_anchor() override;

    virtual void set_highlight(int highlight) override;

    virtual void set_slope(glm::vec2 slope) override;

    virtual void set_tangent_highlight(int highlight) override;

    virtual std::vector<glm::vec3>& get_tangent_lines() override;

    GLuint VAO, VBO, dots_VAO, dots_VBO, convex_VAO, convex_VBO;
    std::vector<glm::vec3> keypoints;
    std::vector<glm::vec3> expanded;
    Program dots_prog, bezier_prog, convex_prog;

    int highlight;
};

