#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <optional>


glm::mat3x2 solve(glm::vec3 kp1, glm::vec3 kp2, std::optional<glm::mat3x2> prev_mat, std::optional<glm::mat3x2> next_mat);
