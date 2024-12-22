#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "shader_loader.h" 
#include "global.h"

class Obstacle {
public:
    GLuint VAO, VBO;
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 color;
    GLuint shaderProgram;

    Obstacle(); // Khởi tạo khối hộp trong hàm tạo
    ~Obstacle(); // Hàm hủy để giải phóng tài nguyên
    void Init(const char* vertexShaderPath, const char* fragmentShaderPath);
    void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lightPos, glm::vec3 viewPos);

private:
    void createBox(); // Hàm tạo dữ liệu cho khối hộp
};

#endif