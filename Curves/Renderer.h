#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "standards.h"
#include "Program.h"


class Renderer {
public:
    virtual void render() = 0;
};
