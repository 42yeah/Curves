#include "solver.h"


glm::mat3x2 solve(glm::vec3 kp1, glm::vec3 kp2, std::optional<glm::mat3x2> prev_mat, std::optional<glm::mat3x2> next_mat) {
    glm::vec2 ab0 = glm::vec2(kp1.x, kp1.y);
    glm::vec2 ab1 = glm::normalize(glm::vec2(1.0f, 1.0f)); // Control
    glm::vec2 ab2 = glm::vec2(kp2.x, kp2.y) - ab0 - ab1;
    if (prev_mat.has_value()) {
        ab1 = glm::vec2(2.0f * prev_mat.value()[0][0] + prev_mat.value()[1][0],
            2.0f * prev_mat.value()[0][1] + prev_mat.value()[1][1]);
        ab2 = glm::vec2(kp2.x, kp2.y) - ab0 - ab1;
    }
    else if (next_mat.has_value()) {
        glm::vec2 ab1next(next_mat.value()[1][0], next_mat.value()[1][1]);
        ab2 = ab1next - glm::vec2(kp2) + ab0;
        ab1 = glm::vec2(kp2) - ab2 - ab0;
    }

    return glm::mat3x2(ab2, ab1, ab0);
}

