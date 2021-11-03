#include "solver.h"


glm::mat4x2 solve(glm::vec3 kp0, glm::vec3 kp1, glm::vec2 dir0, glm::vec2 dir1) {
    glm::vec2 ab0 = glm::vec2(kp0.x, kp0.y);
    glm::vec2 ab1 = glm::vec2(dir0.x, dir0.y);
    glm::vec2 ab3 = dir1 - 2.0f * glm::vec2(kp1.x, kp1.y) + ab1 + 2.0f * ab0; 
    glm::vec2 ab2 = glm::vec2(kp1.x, kp1.y) - ab3 - ab1 - ab0;

    return glm::mat4x2(ab3, ab2, ab1, ab0);
}

