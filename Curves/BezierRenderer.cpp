#include "BezierRenderer.h"

BezierRenderer::BezierRenderer() {
    dots_prog.init("shaders/default_v.glsl", "shaders/dots_f.glsl");
    highlight = -1;

    glGenVertexArrays(1, &dots_VAO);
    glGenBuffers(1, &dots_VBO);
    glBindVertexArray(dots_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, dots_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAXIMUM_CAPACITY, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
}

void BezierRenderer::render() {
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
}

void BezierRenderer::set_anchor(int anchor) {
    throw "Not supported";
}

int BezierRenderer::get_anchor() {
    throw "Not supported";
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
