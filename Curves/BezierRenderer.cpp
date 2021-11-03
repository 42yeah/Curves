#include "BezierRenderer.h"

BezierRenderer::BezierRenderer() {
    dots_prog.init("shaders/default_v.glsl", "shaders/dots_f.glsl");
    bezier_prog = Program("shaders/default_v.glsl", "shaders/default_f.glsl");
    convex_prog = Program("shaders/default_v.glsl", "shaders/convex/convex_f.glsl");
    highlight = -1;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAXIMUM_CAPACITY, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

    glGenVertexArrays(1, &convex_VAO);
    glGenBuffers(1, &convex_VBO);
    glBindVertexArray(convex_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, convex_VBO);
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
}

std::vector<glm::vec3> BezierRenderer::expand(std::vector<glm::vec3>& keypoints) {
    std::vector<glm::vec3> expanded;
    for (int i = 0; i <= INTERPOLATION_COUNT; i++) {
        float t = (float) i / INTERPOLATION_COUNT;

        std::vector<glm::vec3> current = keypoints;
        while (current.size() != 1) {
            std::vector<glm::vec3> next;
            for (int j = 0; j < current.size() - 1; j++) {
                next.push_back(glm::mix(current[j], current[j + 1], t));
            }
            current = next;
        }
        expanded.push_back(current[0]);
    }
    return expanded;
}

void BezierRenderer::render() {
    convex_prog.use();
    glUniformMatrix4fv(convex_prog.loc("ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
    glUniformMatrix4fv(convex_prog.loc("view"), 1, GL_FALSE, glm::value_ptr(view));
    glBindVertexArray(convex_VAO);
    glDrawArrays(GL_LINES, 0, 2 * (int)keypoints.size());

    bezier_prog.use();
    glUniform1i(bezier_prog.loc("tangentMode"), 0);
    glUniformMatrix4fv(bezier_prog.loc("ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
    glUniformMatrix4fv(bezier_prog.loc("view"), 1, GL_FALSE, glm::value_ptr(view));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, (int) expanded.size());

    dots_prog.use();
    glUniform1i(dots_prog.loc("tangentMode"), 0);
    glUniformMatrix4fv(dots_prog.loc("ortho"), 1, GL_FALSE, glm::value_ptr(ortho));
    glUniformMatrix4fv(dots_prog.loc("view"), 1, GL_FALSE, glm::value_ptr(view));
    glBindVertexArray(dots_VAO);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, (int) keypoints.size());
}

void BezierRenderer::set_tangent(int tangent) {
    throw "Not supported";
}

void BezierRenderer::set_keypoints(std::vector<glm::vec3>& keypoints) {
    this->keypoints = keypoints;

    glBindVertexArray(dots_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, dots_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * keypoints.size(), &keypoints[0]);

    std::vector<glm::vec3> expanded_points = expand(keypoints);
    expanded.clear();
    for (int i = 0; i < expanded_points.size() - 1; i++) {
        expanded.push_back(expanded_points[i]);
        expanded.push_back(expanded_points[i + 1]);
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * expanded.size(), &expanded[0]);

    std::vector<glm::vec3> convex;
    for (int i = 0; i < keypoints.size() - 1; i++) {
        convex.push_back(keypoints[i]);
        convex.push_back(keypoints[i + 1]);
    }
    convex.push_back(keypoints[keypoints.size() - 1]);
    convex.push_back(keypoints[0]);
    glBindVertexArray(convex_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, convex_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * convex.size(), &convex[0]);
}

void BezierRenderer::set_highlight(int highlight) {
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

void BezierRenderer::set_slope(glm::vec2 slope) {
    throw "Not supported";
}

void BezierRenderer::set_tangent_highlight(int highlight) {
    throw "Not supported";
}

std::vector<glm::vec3>& BezierRenderer::get_tangent_lines() {
    throw "Not supported";
}
