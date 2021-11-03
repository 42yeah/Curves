#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <optional>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include "standards.h"
#include "Program.h"
#include "SplineRenderer.h"
#include "BlueprintRenderer.h"
#include "BezierRenderer.h"
#include "UIState.h"


#define NEXT(x, y, w, h) ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once); \
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Once)


void update_vao(GLuint VAO, std::vector<glm::vec3>& new_data) {
    glBindVertexArray(VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * new_data.size(), &new_data[0], GL_DYNAMIC_DRAW);
}

int main(int argc, const char* argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_W, SCR_H, "Curves", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cerr << "ERR! Failed to load OpenGL funcs." << std::endl;
        return -1;
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    int num_points = 5;
    std::vector<glm::vec3> keypoints;
    std::uniform_real_distribution distrib;
    std::random_device dev;
    for (int i = 0; i < num_points; i++) {
        keypoints.push_back(glm::vec3(distrib(dev) * 2.0f - 1.0f, distrib(dev) * 2.0f - 1.0f, 0.0f));
    }
    glm::vec2 offset(0.0f);
    glm::vec2 target_offset(0.0f);
    glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 10.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    BlueprintRenderer blueprint;
    SplineRenderer spline_renderer;
    BezierRenderer bezier_renderer;
    Renderer *renderer = &spline_renderer;
    renderer->set_keypoints(keypoints);

    // ImGui initialization
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.FontGlobalScale = 0.5f;
    io.Fonts->AddFontFromFileTTF("fonts/noto_serif.otf", 38, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    glfwSwapInterval(1);

    float scale = 1.0f;
    float time = glfwGetTime();
    UIState ui_state;

    // Renderer select
    const char* renderers[] = { "样条", "贝塞尔曲线" };

    while (!glfwWindowShouldClose(window)) {
        // State switch
        if (ui_state.old_state != ui_state.renderer_state) {
            switch (ui_state.renderer_state) {
            case RendererState::Bezier:
                renderer = &bezier_renderer;
                break;

            case RendererState::Spline:
                renderer = &spline_renderer;
                break;
            }
            renderer->set_keypoints(keypoints);
        }
        ui_state.old_state = ui_state.renderer_state;

        float now = glfwGetTime();
        float delta_time = now - time;
        time = now;

        // Value upate
        ortho = glm::ortho(-1.0f / scale * ASPECT, 1.0f / scale * ASPECT, -1.0f / scale, 1.0f / scale, 0.01f, 10.0f);
        view = glm::lookAt(glm::vec3(-offset.x, offset.y, -1.0f), glm::vec3(-offset.x, offset.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        renderer->ortho = ortho;
        renderer->view = view;
        renderer->scale = scale;
        blueprint.offset = offset;
        blueprint.scale = scale;

        if (ui_state.renderer_state == RendererState::Spline) {
            if ((ui_state.selected_item == 0 || ui_state.selected_item == keypoints.size() - 1) &&
                (ui_state.state == State::Selected ||
                    ui_state.state == State::TangentHighlighting ||
                    ui_state.state == State::Tangent)) {
                renderer->set_tangent(ui_state.selected_item);
            }
            else {
                renderer->set_tangent(-1);
            }
        }

        glfwPollEvents();

        // Event handle
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        mouse_x /= SCR_W;
        mouse_y /= SCR_H;
        glm::vec2 mouse((float)mouse_x * 2.0f - 1.0f, (float)mouse_y * 2.0f - 1.0f);
        mouse.x *= ASPECT;
        mouse.y = -mouse.y;
        mouse /= scale;
        mouse.x -= offset.x;
        mouse.y += offset.y;

        if (glfwGetKey(window, GLFW_KEY_W)) {
            target_offset.y += delta_time * 2.0f * (1.0 / scale);
        }
        if (glfwGetKey(window, GLFW_KEY_A)) {
            target_offset.x += delta_time * 2.0f * (1.0 / scale);
        }
        if (glfwGetKey(window, GLFW_KEY_S)) {
            target_offset.y -= delta_time * 2.0f * (1.0 / scale);
        }
        if (glfwGetKey(window, GLFW_KEY_D)) {
            target_offset.x -= delta_time * 2.0f * (1.0 / scale);
        }
        glm::vec2 delta_offset = target_offset - offset;
        offset += delta_offset / 5.0f;

        // Scene render
        glClear(GL_COLOR_BUFFER_BIT);

        blueprint.render();
        renderer->render();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        GLuint code = glGetError();
        if (code != GL_NONE) {
            std::cerr << "WHOOPS! Big problem: " << code << std::endl;
            ImGui::ShowDemoWindow();
            return -2;
        }

        NEXT(10.0f, 10.0f, 300.0f, 300.0f);
        if (ImGui::Begin("绘制曲线")) {
            if (ImGui::BeginListBox("关键点集")) {
                for (int i = 0; i < keypoints.size(); i++) {
                    if (ImGui::Selectable((std::to_string(keypoints[i].x) + ", " + std::to_string(keypoints[i].y)).c_str())) {
                        target_offset = glm::vec2(-keypoints[i].x, keypoints[i].y);
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::SliderInt("生成点数", &num_points, 2, 100);
            if (ImGui::Button("重新生成")) {
                keypoints.clear();
                for (int i = 0; i < num_points; i++) {
                    keypoints.push_back(glm::vec3(distrib(dev) * 2.0f - 1.0f, distrib(dev) * 2.0f - 1.0f, 0.0f));
                }
                renderer->set_keypoints(keypoints);
            }
        }
        ImGui::End();
        NEXT(320.0f, 10.0f, 300.0f, 120.0f);
        if (ImGui::Begin("Debug")) {
            ImGui::TextWrapped("鼠标坐标：%lf, %lf。\n变换后：%lf, %lf。", mouse_x * 2.0f - 1.0f, mouse_y * 2.0f - 1.0f, mouse.x, mouse.y);
            std::string state_str;
            switch (ui_state.state) {
            case State::Nothing:
                state_str = "空闲";
                break;

            case State::Creating:
                state_str = "创建";
                break;

            case State::Dragging:
                state_str = "拖动";
                break;

            case State::Highlighting:
                state_str = "高亮";
                break;

            case State::Selected:
                state_str = "选择";
                break;

            case State::Transition:
                state_str = "过渡";
                break;

            case State::Tangent:
                state_str = "切线调整";
                break;

            case State::TangentHighlighting:
                state_str = "切线高亮";
                break;

            default:
                state_str = "不知道";
                break;
            }
            ImGui::TextWrapped("当前 UI 状态：%s", state_str.c_str());
        }
        ImGui::End();
        NEXT(320.0f, 140.0f, 300.0f, 170.0f);
        if (ImGui::Begin("可用操作")) {
            ImGui::TextWrapped("WASD 四处走动。选中点或点击空白处对点进行增加/修改。");
            ImGui::SliderFloat("缩放", &scale, 0.01f, 2.0f);
        }
        ImGui::End();

        NEXT(10.0f, 320.0f, 300.0f, 100.0f);
        if (ImGui::Begin("模式")) {
            ImGui::Combo("模式选择", (int *) &ui_state.renderer_state, renderers, IM_ARRAYSIZE(renderers));
        }
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, FBO_W, FBO_H);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // FSM logic: if user is not holding buttons,
        // then enters select mode automagically.
        // This is handled AFTER the ImGui logic, in order to prevent lines from spawning.
        if (ui_state.state == State::Nothing || ui_state.state == State::Highlighting) {
            ui_state.state = State::Nothing;
            ui_state.highlighting_item = { -1, 0.0f };
            for (int i = 0; i < keypoints.size(); i++) {
                float dist = glm::length(glm::vec2(keypoints[i]) - mouse);
                if (dist < (1.0f / scale) * 5e-2 && (ui_state.highlighting_item.first == -1 ||
                    dist < ui_state.highlighting_item.second)) {
                    ui_state.highlighting_item = { i, dist };
                }
            }
            if (ui_state.highlighting_item.first != -1) {
                renderer->set_highlight(ui_state.highlighting_item.first);
                ui_state.state = State::Highlighting;
            }
            else {
                renderer->set_highlight(-1);
            }
        }
        else if (ui_state.state == State::Selected || ui_state.state == State::TangentHighlighting) {
            ui_state.state = State::Selected;

            if (ui_state.renderer_state == RendererState::Spline) {
                ui_state.tangent_highlighting_item = { -1, 0.0f };
                for (int i = 0; i < renderer->get_tangent_lines().size(); i++) {
                    float dist = glm::distance(glm::vec2(renderer->get_tangent_lines()[i]), mouse);
                    if (dist < (1.0f / scale) * 5e-2 &&
                        (ui_state.tangent_highlighting_item.first == -1 || (dist < ui_state.tangent_highlighting_item.second))) {
                        ui_state.tangent_highlighting_item = { i, dist };
                    }
                }
                if (ui_state.tangent_highlighting_item.first != -1) {
                    ui_state.state = State::TangentHighlighting;
                    renderer->set_tangent_highlight(ui_state.tangent_highlighting_item.first);
                }
                else {
                    renderer->set_tangent_highlight(-1);
                }
            }

        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
            if (ui_state.state == State::Nothing && !io.WantCaptureMouse) {
                // Create, then enter dragging state
                keypoints.push_back(glm::vec3(mouse, 0.0f));
                if (ui_state.renderer_state == RendererState::Spline && renderer->get_anchor() == keypoints.size() - 2) {
                    renderer->set_anchor(keypoints.size() - 1);
                }
                renderer->set_keypoints(keypoints);
                // Highlight this one
                ui_state.state = State::Highlighting;
                ui_state.highlighting_item.first = (int)(keypoints.size() - 1);
            }
            else if (ui_state.state == State::Highlighting && !io.WantCaptureMouse) {
                // Determines what to drag
                ui_state.state = State::Dragging;
                ui_state.dragging_item = ui_state.highlighting_item.first;
                ui_state.offset = glm::vec2(keypoints[ui_state.dragging_item]) - mouse;
                renderer->set_highlight(ui_state.dragging_item);
            }
            else if (ui_state.state == State::Dragging) {
                // Drag
                keypoints[ui_state.dragging_item] = glm::vec3(mouse.x + ui_state.offset.x,
                    mouse.y + ui_state.offset.y,
                    1.0f);
                renderer->set_keypoints(keypoints);
            }
            else if (ui_state.state == State::Selected && !io.WantCaptureMouse) {
                if (glm::distance(glm::vec2(keypoints[ui_state.selected_item]), mouse) < 5e-2) {
                    ui_state.state = State::Highlighting;
                    ui_state.highlighting_item.first = ui_state.selected_item;
                }
                else {
                    keypoints[ui_state.dragging_item].z = 0.0f;
                    renderer->set_keypoints(keypoints);
                    ui_state.state = State::Transition;
                    renderer->set_highlight(-1);
                }
            }
            else if (ui_state.state == State::TangentHighlighting && !io.WantCaptureMouse) {
                // Activate tangent drag mode
                ui_state.state = State::Tangent;
                ui_state.offset = glm::vec2(renderer->get_tangent_lines()[ui_state.tangent_highlighting_item.first]) - mouse;
            }
            else if (ui_state.state == State::Tangent) {
                glm::vec2 pos = glm::normalize(glm::vec2(mouse.x + ui_state.offset.x,
                    mouse.y + ui_state.offset.y) - glm::vec2(keypoints[ui_state.selected_item]));
                if (ui_state.tangent_highlighting_item.first == 1) {
                    pos = -pos;
                }
                renderer->set_anchor(ui_state.selected_item);
                renderer->set_slope(pos);
                renderer->set_keypoints(keypoints);
            }
        }
        else if (ui_state.state == State::Dragging) {
            ui_state.state = State::Selected;
            ui_state.selected_item = ui_state.dragging_item;
        }
        else if (ui_state.state == State::Transition) {
            ui_state.state = State::Nothing;
        }
        else if (ui_state.state == State::Tangent) {
            ui_state.state = State::Selected;
        }

        glfwSwapBuffers(window);
    }

    return 0;
}
