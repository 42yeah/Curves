#include "BlueprintRenderer.h"


BlueprintRenderer::BlueprintRenderer() {
    GLuint VBO;
    float data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

    program = Program("shaders/blueprint/blueprint_v.glsl", "shaders/blueprint/blueprint_f.glsl");
    scale = 1.0f;
}

void BlueprintRenderer::render() {
    glBindVertexArray(VAO);
    program.use();
    glUniform1f(program.loc("scale"), scale);
    glUniform2fv(program.loc("offset"), 1, glm::value_ptr(offset));
    glUniform1f(program.loc("aspect"), ASPECT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


// Those functions are useless for BlueprintRenderer
void BlueprintRenderer::set_tangent(int tangent) {
}

void BlueprintRenderer::set_keypoints(std::vector<glm::vec3>& keypoints) {
}

void BlueprintRenderer::set_anchor(int anchor) {
}

int BlueprintRenderer::get_anchor() {
    return 0;
}

void BlueprintRenderer::set_highlight(int highlight) {
}

void BlueprintRenderer::set_slope(glm::vec2 slope) {
}

void BlueprintRenderer::set_tangent_highlight(int highlight) {
}

std::vector<glm::vec3>& BlueprintRenderer::get_tangent_lines() {
    return dummy_tangent_lines;
}
