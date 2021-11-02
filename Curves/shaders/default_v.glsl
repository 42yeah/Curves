#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 ortho;

out vec3 pos;

void main() {
    vec2 uv = (ortho * view * vec4(aPos, 1.0)).xy;
    pos = aPos;
    gl_Position = vec4(-uv.x, uv.y, 0.0, 1.0);
}
