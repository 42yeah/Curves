#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "standards.h"
#include "Program.h"


class Renderer {
public:
    virtual void render() = 0;

    virtual void set_tangent(int tangent) = 0;

    virtual void set_keypoints(std::vector<glm::vec3> &keypoints) = 0;

    virtual void set_highlight(int highlight) = 0;

    virtual void set_slope(glm::vec2 slope) = 0;

    virtual void set_tangent_highlight(int highlight) = 0;

    virtual std::vector<glm::vec3> &get_tangent_lines() = 0;

    glm::mat4 ortho, view;
    float scale;
};
