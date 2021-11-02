#pragma once

#include <iostream>
#include <map>
#include <glad/glad.h>


class Program {
public:
    Program() : program(0) {}

    Program(std::string vpath, std::string fpath) {
        init(vpath, fpath);
    }

    void init(std::string vpath, std::string fpath);

    void use();

    GLuint loc(std::string name);

    std::map<std::string, GLuint> uniforms;
    GLuint program;
};
