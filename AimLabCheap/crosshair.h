#ifndef OBJECT_H
#define OBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "shader_loader.h" 

class Crosshair {
public:
    Crosshair();
    ~Crosshair();
    void Init(const char* vertexShaderPath, const char* fragmentShaderPath);
    void Draw();

private:
    GLuint VAO, VBO;
    GLuint shaderProgram;

    void createVertexArray();
};


#endif