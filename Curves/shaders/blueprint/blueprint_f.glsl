#version 330 core

in float s;
in vec2 uv;

out vec4 color;

void main() {
    vec2 f = fract(uv);
    float d = min(min(f.x, f.y), min(1.0 - f.x, 1.0 - f.y));
    
    vec3 c = vec3(1.0, 1.0, 1.0);
    if (d < s * 2e-3) {
        c = vec3(0.94, 0.94, 0.96);
    }
    

    color = vec4(c, 1.0);
}
