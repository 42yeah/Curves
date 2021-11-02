#include "SplineRenderer.h"

#include <random>
#include "solver.h"


std::vector<glm::vec3> SplineRenderer::expand(std::vector<glm::vec3>& keypoints) {
    std::vector<glm::vec3> ret;
    solves.clear();
    solves.resize(keypoints.size() - 1);

    // Solve the anchor first
    // Propagate forward
    bool executed = false;
    for (int i = anchor; i < keypoints.size() - 1; i++) {
        executed = true;
        glm::vec3 kp1 = keypoints[i];
        glm::vec3 kp2 = keypoints[i + 1];
        glm::mat3x2 solved(1.0f);

        if (i != anchor) {
            solved = solve(kp1, kp2, solves[i - 1], {});
        }
        else {
            glm::mat3x2 initial = glm::mat3x2(glm::vec2(0.0f), slope, glm::vec2(0.0f));
            solved = solve(kp1, kp2, initial, {});
        }
        solves[i] = solved;

        glm::vec3 prev(0.0f);
        ret.push_back(kp1);
        for (int j = 1; j <= INTERPOLATION_COUNT; j++) {
            if (j != 1) {
                ret.push_back(prev);
            }
            float t = (float)j / INTERPOLATION_COUNT;
            glm::vec3 sample = glm::vec3(solved * glm::vec3(t * t, t, 1.0f), 0.0f);
            prev = sample;
            ret.push_back(sample);
        }
    }
    if (executed) {
        ret.push_back(keypoints[keypoints.size() - 1]);
    }
    // Now propagate backward!
    for (int i = anchor - 1; i >= 0; i--) {
        glm::vec3 kp1 = keypoints[i];
        glm::vec3 kp2 = keypoints[i + 1];
        glm::mat3x2 solved(1.0f);
        if (i + 2 >= keypoints.size() && !executed) {
            glm::mat3x2 initial = glm::mat3x2(glm::vec2(0.0f), slope, glm::vec2(0.0f));
            solved = solve(kp1, kp2, {}, initial);
        }
        else {
            solved = solve(kp1, kp2, {}, solves[i + 1]);
        }

        solves[i] = solved;
        glm::vec3 prev(0.0f);
        ret.push_back(kp1);
        for (int j = 1; j <= INTERPOLATION_COUNT; j++) {
            if (j != 1) {
                ret.push_back(prev);
            }
            float t = (float)j / INTERPOLATION_COUNT;
            glm::vec3 sample = glm::vec3(solved * glm::vec3(t * t, t, 1.0f), 0.0f);
            prev = sample;
            ret.push_back(sample);
        }
    }
    return ret;
}


void SplineRenderer::set_keypoints(std::vector<glm::vec3>& keypoints) {
    this->keypoints = keypoints;
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
    prev_highlight = -1;
    tangent = -1;

    anchor = 0;
    slope = glm::normalize(glm::vec2(1.0f));
}


void SplineRenderer::render() {
    if (highlight != prev_highlight) {
        glBindVertexArray(dots_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, dots_VBO);
        if (prev_highlight != -1) {
            keypoints[prev_highlight].z = 0.0f;
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * prev_highlight, sizeof(glm::vec3), &keypoints[prev_highlight]);
        }
        if (highlight != -1) {
            keypoints[highlight].z = 1.0f;
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * highlight, sizeof(glm::vec3), &keypoints[highlight]);
        }
        prev_highlight = highlight;
    }
    if (tangent != -1) {
        glm::vec2 center_point = glm::vec2(keypoints[tangent]);
        tangent_lines.clear();
        float dx = -1.0f;
        float dy = -1.0f;
        if (tangent == solves.size()) {
            dx = 2.0f * solves[tangent - 1][0][0] + solves[tangent - 1][1][0];
            dy = 2.0f * solves[tangent - 1][0][1] + solves[tangent - 1][1][1];
        }
        else {
            dx = solves[tangent][1][0];
            dy = solves[tangent][1][1];
        }
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

