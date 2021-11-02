#include "Program.h"
#include <fstream>
#include <sstream>


GLuint link(GLuint type, std::string path) {
    std::ifstream reader(path);
    GLuint shader = glCreateShader(type);
    if (reader.good()) {
        std::stringstream buf;
        buf << reader.rdbuf();
        std::string str = buf.str();
        const char* src = str.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        char log[512] = { 0 };
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cout << path << " shader info log: " << log << std::endl;
    }
    else {
        std::cerr << "WARNING! Bad shader: " << path << std::endl;
    }

    return shader;
}

void Program::init(std::string vpath, std::string fpath) {
    program = glCreateProgram();
    glAttachShader(program, link(GL_VERTEX_SHADER, vpath));
    glAttachShader(program, link(GL_FRAGMENT_SHADER, fpath));
    glLinkProgram(program);
    char log[512] = { 0 };
    glGetProgramInfoLog(program, sizeof(log), nullptr, log);
    std::cout << "Program info log: " << log << std::endl;
}

void Program::use() {
    glUseProgram(program);
}

GLuint Program::loc(std::string name) {
    auto res = uniforms.find(name);
    if (res != uniforms.end()) {
        return res->second;
    }
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location < 0) {
        std::cerr << "ERR! Uniform name not found: " << name << std::endl;
        return 0;
    }
    uniforms[name] = (GLuint)location;
    return location;
}
