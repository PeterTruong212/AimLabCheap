#include "crosshair.h"

// Dữ liệu crosshair
float crosshairVertices[] = {
    // Đường ngang
    -0.005f,  0.0f,  // Trái
     0.005f,  0.0f,  // Phải

     // Đường dọc
      0.0f, -0.005f,  // Dưới
      0.0f,  0.005f   // Trên
};

Crosshair::Crosshair() { }

Crosshair::~Crosshair() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void Crosshair::Init(const char* vertexShaderPath, const char* fragmentShaderPath) {
    shaderProgram = createShaderProgram(vertexShaderPath, fragmentShaderPath);
    createVertexArray();
}

void Crosshair::Draw() {
    glUseProgram(shaderProgram);

    // Thiết lập ma trận orthographic projection cho crosshair
    glm::mat4 orthoProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(orthoProjection));

    // Thiết lập ma trận view đơn vị cho crosshair
    glm::mat4 identityView = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(identityView));

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
}

void Crosshair::createVertexArray() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}