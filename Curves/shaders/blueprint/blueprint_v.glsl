#version 330 core

layout (location = 0) in vec3 aPos;

uniform float scale;
uniform float aspect;
uniform vec2 offset;

out float s;
out vec2 uv;

void main() {
    uv = aPos.xy * 2.0 - 1.0;
    gl_Position = vec4(uv.x, uv.y, 0.0, 1.0);
    uv.x *= -aspect;
    uv *= 1.0 / scale;
    uv += offset;
    s = 1.0 / scale;
}
