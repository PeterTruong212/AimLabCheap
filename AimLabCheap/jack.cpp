#include "jack.h"
#include "stb_image.h"

// Dữ liệu jack
float jackVertices[] = {
    // Vị trí              // Tọa độ texture
    -10.0f, 100.0f, -10.0f,  0.0f, 0.0f,  // Góc trái dưới
     10.0f, 100.0f, -10.0f,  1.0f, 0.0f,  // Góc phải dưới
     10.0f, 100.0f,  10.0f,  1.0f, 1.0f,  // Góc phải trên
    -10.0f, 100.0f,  10.0f,  0.0f, 1.0f   // Góc trái trên
};

unsigned int jackIndices[] = {
    0, 1, 2,  // Tam giác 1
    0, 2, 3   // Tam giác 2
};

Jack::Jack() { }

Jack::~Jack() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

void Jack::Init(const char* vertexShaderPath, const char* fragmentShaderPath) {
    shaderProgram = createShaderProgram(vertexShaderPath, fragmentShaderPath);
    createVertexArray();
    loadTexture("jack.jpg");
}

void Jack::Draw(glm::mat4 projection, glm::mat4 view, unsigned int textureID) {
    glUseProgram(shaderProgram);

    // Truyền ma trận vào shader
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // Liên kết texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "jackTexture"), 0);

    // Vẽ jack
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Jack::createVertexArray() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(jackVertices), jackVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(jackIndices), jackIndices, GL_STATIC_DRAW);

    // Vị trí
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Tọa độ texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Jack::loadTexture(const char* texturePath) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Thiết lập tham số texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}