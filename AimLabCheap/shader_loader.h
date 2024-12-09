#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

std::string loadShaderSource(const char* filepath);
GLuint compileShader(GLenum type, const char* source);
GLuint createShaderProgram(const char* vertexFilePath, const char* fragmentFilePath);

#endif // SHADER_LOADER_H
