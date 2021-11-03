#include "SplineRenderer.h"

#include <random>
#include "solver.h"


std::vector<glm::vec3> SplineRenderer::expand(std::vector<glm::vec3>& keypoints) {
    std::vector<glm::vec3> ret;
    std::vector<glm::vec3> interpolated;

    // Solve the anchor first
    // Propagate forward
    bool executed = false;
    for (int i = 0; i < keypoints.size() - 1; i++) {
        executed = true;
        glm::vec3 kp1 = keypoints[i];
        glm::vec3 kp2 = keypoints[i + 1];
        glm::vec2 dir1 = directions[i];
        glm::vec2 dir2 = directions[i + 1];
        glm::mat4x2 solved(1.0f);

        solved = solve(kp1, kp2, dir1, dir2);

        for (int j = 0; j <= INTERPOLATION_COUNT; j++) {
            float t = (float) j / INTERPOLATION_COUNT;
            glm::vec3 sample = glm::vec3(solved * glm::vec4(t * t * t, t * t, t, 1.0f), 0.0f);
            interpolated.push_back(sample);
        }
    }

    for (int i = 0; i < interpolated.size() - 1; i++) {
        ret.push_back(interpolated[i]);
        ret.push_back(interpolated[i + 1]);
    }
    return ret;
}

void SplineRenderer::set_keypoints(std::vector<glm::vec3>& keypoints) {
    this->keypoints = keypoints;
    std::uniform_real_distribution<float> distrib;
    std::random_device dev;
    while (directions.size() < keypoints.size()) {
        directions.push_back(glm::normalize(glm::vec2(distrib(dev) * 2.0f - 1.0f, distrib(dev) * 2.0f - 1.0f)));
    }
    expanded = expand(keypoints);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * expanded.size(), &expanded[0]);

    glBindVertexArray(dots_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, dots_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * keypoints.size(), &keypoints[0]);
}

SplineRenderer::SplineRenderer() {
    spline_prog = Program("shaders/default_v.glsl", "shaders/default_f.glsl");
    dots_prog = Program("shaders/default_v.glsl", "shaders/dots_f.glsl");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAXIMUM_CAPACITY, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

    glGenVertexArrays(1, &dots_VAO);
    glGenBuffers(1, &dots_VBO);
    glBindVertexArray(dots_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, dots_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAXIMUM_CAPACITY, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

    glGenVertexArrays(1, &tangent_VAO);
    glGenBuffers(1, &tangent_VBO);
    glBindVertexArray(tangent_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, tangent_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, nullptr, GL_DYNAMIC_DRAW); // There's only going to have 2 vertices ever
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

    ortho = glm::mat4(1.0f);
    view = glm::mat4(1.0f);

    highlight = -1;
    tangent = -1;
}


void SplineRenderer::render() {
    if (tangent != -1) {
        glm::vec2 center_point = glm::vec2(keypoints[tangent]);
        tangent_lines.clear();
        float dx = directions[tangent].x;
        float dy = directions[tangent].y;
        glm::vec2 dir = glm::normalize(glm::vec2(dx, dy)) * 1.0f / scale;

        float z1 = 0.0f, z2 = 0.0f;
        if (tangent_highlight == 0) {
            z1 = 1.0f;
        }
        else if (tangent_highlight == 1) {
            z2 = 1.0f;
        }
        tangent_lines.push_back(glm::vec3(center_point.x + dir.x * 0.1f, center_point.y + dir.y * 0.1f, z1));
        tangent_lines.push_back(glm::vec3(center_point.x - dir.x * 0.1f, center_point.y - dir.y * 0.1f, z2));

        glBindVertexArray(tangent_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, tangent_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * tangent_lines.size(), &tangent_lines[0]);
    }

    spline_prog.use();
    glUniformMatrix4fv(spline_prog.loc("ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
    glUniformMatrix4fv(spline_prog.loc("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform1i(spline_prog.loc("tangentMode"), 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, (int)expanded.size());
    if (tangent != -1) {
        glBindVertexArray(tangent_VAO);
        glUniform1i(spline_prog.loc("tangentMode"), 1);
        glDrawArrays(GL_LINES, 0, (int)tangent_lines.size());
    }

    dots_prog.use();
    glUniform1i(dots_prog.loc("tangentMode"), 0);
    glUniformMatrix4fv(dots_prog.loc("ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
    glUniformMatrix4fv(dots_prog.loc("view"), 1, GL_FALSE, glm::value_ptr(view));
    glBindVertexArray(dots_VAO);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, (int)keypoints.size());
    if (tangent != -1) {
        glBindVertexArray(tangent_VAO);
        glPointSize(5.0f);
        glUniform1i(dots_prog.loc("tangentMode"), 1);
        glDrawArrays(GL_POINTS, 0, (int)tangent_lines.size());
    }
}

void SplineRenderer::set_tangent(int tangent) {
    this->tangent = tangent;
}

void SplineRenderer::set_highlight(int highlight) {
    if (highlight != this->highlight) {
        glBindVertexArray(dots_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, dots_VBO);
        if (this->highlight != -1) {
            keypoints[this->highlight].z = 0.0f;
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->highlight, sizeof(glm::vec3), &keypoints[this->highlight]);
        }
        if (highlight != -1) {
            keypoints[highlight].z = 1.0f;
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * highlight, sizeof(glm::vec3), &keypoints[highlight]);
        }
        this->highlight = highlight;
    }
}

void SplineRenderer::set_slope(glm::vec2 slope) {
    directions[tangent] = slope;
}

void SplineRenderer::set_tangent_highlight(int highlight) {
    this->tangent_highlight = highlight;
}

std::vector<glm::vec3>& SplineRenderer::get_tangent_lines() {
    return tangent_lines;
}
