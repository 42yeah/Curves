#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <optional>


glm::mat4x2 solve(glm::vec3 kp1, glm::vec3 kp2, glm::vec2 dir1, glm::vec2 dir2);
