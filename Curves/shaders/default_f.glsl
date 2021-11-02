#version 330 core

uniform bool tangentMode;

out vec4 color;

void main() {
    if (tangentMode) {
        color = vec4(0.67, 0.78, 1.0, 1.0);
    } else {
        color = vec4(1.0, 0.5, 0.0, 1.0);
    }
    
}
