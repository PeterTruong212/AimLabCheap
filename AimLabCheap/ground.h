#ifndef GROUND_H
#define GROUND_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "shader_loader.h" 

class Ground {
public:
    Ground();
    ~Ground();
    void Init(const char* vertexShaderPath, const char* fragmentShaderPath);
    void Draw(glm::mat4 projection, glm::mat4 view, unsigned int textureID);
    GLuint texture;

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;

    void createVertexArray();
    void loadTexture(const char* texturePath);
};

#endif