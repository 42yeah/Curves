#version 330 core

in vec3 pos;

uniform bool tangentMode;

out vec4 color;

void main() {
    if (tangentMode) {
        color = mix(vec4(0.118, 0.565, 1.0, 1.0),
                    vec4(1.0, 0.565, 1.0, 1.0), pos.z);
    } else {
        color = mix(vec4(0.0, 0.0, 0.0, 1.0),
                    vec4(0.4, 0.8, 1.0, 1.0), pos.z);
    }
}
